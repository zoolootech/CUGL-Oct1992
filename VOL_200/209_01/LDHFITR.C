/* LDHFITR.C    VERS:- 01.00  DATE:- 09/26/86  TIME:- 09:39:02 PM */
/*
cp b:$1 -e -s0
z80asm $1.bb
slrnk b:$1, b:simpmain, b:simplib1/s, b:simplib0/s, b:$1/n/e
*/
/* 
Description:

VERSION OF XXXXFITn, for:

nonlinear least squares fit by simplex minimizaton (Nelder-Mead algorithm).

analysis of lactate dehydrogenase initial rate kinetic data:
	two substrate -- two product mechanism,
	with inhibition by first product,
	and with reactant and product abortive complexes;

see LDHFITR documentation for description of rate law.

compile with:
	simpmain, simplib1, simplib0
	floating point functions

the precision of the floating point functions must be high, ie 14 or more
	decimal digits; eight digit precision is not enough for the 
	matrix operations.

contents of the xxxxfitn module  =  
declarations and routines for simplex fitting special to function to be fit:

	function for calculation of dependent variable and
		weighted sum of residuals squared		= func()

	print of <data> records					= fdatprint()
								= fpointprint()

	other special displays					= fspecial()

	all external declarations, except for <data>, which is 
		defined as dummy storage in SIMPLIB1 
		or its modification

By J.A. Rupley, Tucson, Arizona
Coded for ECO C compiler, version 3.40
*/

	/* page eject */

/*
     To make more readable the coding in <func()> of the model 
equation to be fit to the data: 

(1) use mnemonic member names in declaring <struct dat> in 
XXXXFITn; 

(2) declare a dummy structure, <struct pnamestruct>, that is 
entirely equivalent to the structure that holds the parameter 
values, <pstruct>, but that has mnemonic member names; the 
mnemonic dummy structure then can be used with the <pstruct> 
address passed as a parameter to <func()>.


     Change in the model being fit should not require recoding 
and recompilation of <read_data()> or any other routines except 
those of XXXXFITn; of course, change in the model requires change 
of <func()>, <fdatprint()>, and of the declarations of <struct dat> 
and <struct pnamestruct> in XXXXFITn.

*/

/* page eject */

#include <stdio.h>
#include <ctrlcnst.h>

#define NVERT		8	/* set equal to 1 + number of parameters 
				used in <func()> */


#define NPARM		10	/* do NOT change this define */



				/* EXTERNALS AND STRUCTURES */

				/* do NOT change any structure except <dat>*/

				/* the structure <dat> MUST be changed 
				to accord with the requirements of 
				<func()> and <fdatprint()>; */
				/* all members of <struct dat> MUST be of 
				type double. */
				/* as written the program accomodates 
				more than 100 six-member data points */
				/* see the header above for more description
				of the declaration of <struct dat> in XXXXFITn
				and the definition of the aggregate <data> in
				SIMPLIB1 */

				/* the structure <pnamestruct> is equivalent 
				to <pstruct>, which holds the parameter values;
				use of <pnamestruct> allows mnemonic access 
				to the contents of <pstruct> */

struct dat {			
	double y ;		
	double yc ;
	double w ;
	double a ;
	double b ;
	double p ;
} ;

struct pstruct {
	double val ;
	double parm[NPARM] ;
} ;

struct pnamestruct {
	double val ;
	double vmax ;
	double kma ;
	double kmb ;
	double kmab ;
	double kmq_kmpq ;
	double kip_1 ;
	double kib_k3k4 ;
	double dummy[3] ;
} ;

struct qstruct {
	int parmndx[NPARM] ;
	double q[NPARM] ;
	double yplus[NPARM] ;
	double yminus[NPARM] ;
	double a[NPARM] ;
	double bdiag[NPARM] ;
	double inv_bdiag[NPARM] ;
	double std_dev[NPARM] ;
} ;


struct pstruct p[NVERT] ;

struct pstruct pcent ;

struct pstruct *p_p[NVERT] ;

struct pstruct pmin ;

struct qstruct q ;


double qmat[NPARM][NPARM] ;

double mean_func, rms_func, test, rms_data ;

double yzero, ymin, ypmin, mse ;

double quad_test, exit_test ;

int iter, maxiter, nparm, nvert, ndata, maxquad_skip, prt_cycle, ndatval ;

int nfree ;

char title[80] ;

/* page eject */

/* FUNC
	CALCULATION OF LEAST SQUARES FUNCTION
	CODED ACCORDING TO MODEL BEING FIT
	IT SHOULD BE EFFICIENT
	DURING THE FIT, TIME IS MOSTLY SPENT HERE
	(THE OVERHEAD ELSEWHERE IS ONLY SEVERAL SECONDS PER CYCLE)
*/

int func(g)
struct pstruct *g ;
{
	void printf() ;
	register int n ;

	struct pnamestruct *pnam ;

	extern struct dat 	data[] ;
	extern int 		nparm ;
	extern int 		ndata ;

	for (n = 0; n < nparm; n++)
		if (g->parm[n] <= 0) {
			g->val = 1.E38 ;
			printf("function error\n") ;
			return (ERROR) ;
		}

	pnam = g ;
	pnam->val = 0 ;
	for (n = 0; n < ndata; n++) {
		data[n].yc = 
			pnam->vmax / 
			(
		      	1 + pnam->kmq_kmpq * data[n].p 
			  + pnam->kma / data[n].a 
			  + pnam->kmb / data[n].b 
				* (1 + pnam->kmq_kmpq * data[n].p)
				* (1 + pnam->kip_1 * data[n].p)
			  + pnam->kmab / data[n].a / data[n].b 
				* (1 + pnam->kmq_kmpq * data[n].p) 
			  + pnam->kib_k3k4 * data[n].b
			) ;
		pnam->val = pnam->val + (data[n].y - data[n].yc) 
				* (data[n].y - data[n].yc) 
				* data[n].w 
				* data[n].w ;
	}
	return (OK) ;
}                          	/* END OF FUNC        			*/

/* page eject */

/* FDATPRINT
	PRINT DATA AND COMPARE WITH CALCULATED VALUES
	CODED ACCORDING TO MODEL AND DATA
*/

void fdatprint(fptr)
FILE *fptr ;
{
	register int j ;

	void fprintf() ;

	extern int 		iter, ndata, maxiter ;
	extern struct dat 	data[] ;
	extern char		title[] ;

	fprintf(fptr, "\1\f\n%-s\n\niteration number %d\n\n", title, iter) ;
	fprintf(fptr, 
"        yobs   ycalc   del-y  weight       a         b         p\n") ;
	for (j = 0; j < ndata; j++) 
		fprintf(fptr, 
			"%4d %7.3f %7.3f %7.3f %7.3f %10.5f %10.5f %10.5f\n", 
			(j+1), data[j].y, data[j].yc, (data[j].y - data[j].yc),
			data[j].w, data[j].a, data[j].b, data[j].p) ;

	if (maxiter != 0)
		fpointprint(fptr) ;
}				/* END OF FDATPRINT			*/

/* page eject */

/* FPOINTPRINT
	PRINT POINTS FOR CONSTRUCTION OF PRIMARY AND SECONDARY PLOTS
	CODED ACCORDING TO MODEL AND DATA
*/

void fpointprint(fptr)
FILE *fptr ;
{
	void fprintf() ;
	extern struct pstruct	pcent ;
	struct pnamestruct	*pnam ;
	extern char 		title[] ;
	extern int		iter ;
	double v,k1,k2,k3,k4,k5,vint,v1,v2,s1,s2 ;

	pnam = &pcent ;
	v = pnam->vmax ;
	k1 = pnam->kma ;
	k2 = pnam->kmb ;
	k3 = pnam->kmab ;
	k4 = pnam->kmq_kmpq ;
	k5 = pnam->kip_1 ;
	vint = (1 - k1 * k2/k3)/v ;
	v1 = (1 + k1/.035)/v ;
	v2 = v1 + k4 * 150/v ;
	s1 = (k2 + k3/.035)/v ;
	s2 = (s1 + k2 * k5 * 150/v) * (1 + k4 * 150) ;

	fprintf(fptr, "\1\f\n%-s\n\niteration number %d\n\n", title, iter) ;
	fprintf(fptr, 
"For primary plot at fixed b = [Pyruvate]:\n") ;
	fprintf(fptr, 
"intersection (1/a = 1/[NADH], 1/V)      %10.2f%10.2f\n",-k2/k3,vint) ;
	fprintf(fptr, 
"1/V intercepts at b = (.7,.35,.22,.15)      %8.2f%8.2f%8.2f%8.2f\n\n",
		(1+k2/.7)/v,(1+k2/.35)/v,(1+k2/.22)/v,(1+k2/.15)/v) ;

	fprintf(fptr, 
"For primary plot at fixed a = [NADH]:\n") ;
	fprintf(fptr, 
"intersection (1/b = 1/[Pyruvate], 1/V)  %10.2f%10.2f\n",
		-k1/k3,vint) ;
	fprintf(fptr, 
"1/V intercepts at a = (.035,.022,.015,.011) %8.2f%8.2f%8.2f%8.2f\n\n",
		(1+k1/.035)/v,(1+k1/.022)/v,(1+k1/.015)/v,(1+k1/.011)/v) ;

	fprintf(fptr, 
"For secondary plot:\n") ;
	fprintf(fptr, 
"1/a = 1/[NADH] and 1/b = 1/[Pyruvate] intercepts %9.2f%10.2f\n",-1/k1,-1/k2);
	fprintf(fptr, 
"1/V intercept     \t\t\t\t%10.2f\n\n",1/v) ;

	fprintf(fptr, "For lactate inhibition, at a = [NADH] = 0.035\n") ;
	fprintf(fptr, "1/V values at 1/b = 1/[Pyruvate] = 0 and 5.0\n");
	fprintf(fptr, "\t\t\t- Lactate%7.2f%10.2f\n", v1, (v1 + 5 * s1)) ;
	fprintf(fptr, "\t\t\t+ Lactate%7.2f%10.2f\n\n",v2, (v2 + 5 * s2)) ;

	fprintf(fptr, 
"R(+/-,intercept), R(+/-,slope)%10.2f%10.2f\n",v2/v1,s2/s1);

	fprintf(fptr, 
"\n\n\n\nValues of the kinetic constants\n") ;
	fprintf(fptr, "parm(0) = V        %17.11e\n",v) ;
	fprintf(fptr, "parm(1) = KmA      %17.11e\n",k1) ;
	fprintf(fptr, "parm(2) = KmB      %17.11e\n",k2) ;
	fprintf(fptr, "parm(3) = KmAB     %17.11e\n",k3) ;
	fprintf(fptr, "parm(4) = KmQ/KmPQ %17.11e\n",k4) ;
	fprintf(fptr, "parm(5) = 1/K(I,P) %17.11e\n",k5) ;

}				/* END OF FPOINTPRINT			*/

/* page eject */

/* FSPECIAL
	DISPLAY ADDITIONAL INFORMATION DURING
	TRACKING OF MINIMIZATION, IN SIMPFIT()
*/

void fspecial(fptr)
FILE *fptr ;
{
	register j ;

	void fprintf() ;

	extern int 		maxiter ;
	extern double		ypmin, yzero, quad_test ;

	if (ypmin > 0.99E38) {
		fprintf(fptr, "y-pmin error") ;
		for (j = 0; j < nvert; j++)
			if (pmin.parm[j] < 0)
				fprintf(fptr, " (parm(%d) < 0)", j) ;
	}
	else if (ypmin > yzero)
		fprintf(fptr, "y-pmin > yzero") ;

	fprintf(fptr, "    maxiter = %d  quad_test = %7.2e\n", 
		maxiter, quad_test) ;

	return ;
}				/* END OF FSPECIAL			*/
