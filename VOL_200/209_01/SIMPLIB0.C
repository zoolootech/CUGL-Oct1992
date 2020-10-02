/* SIMPLIB0.C    VERS:- 01.00  DATE:- 09/26/86  TIME:- 09:39:24 PM */
/* 
Description:

functions for simplex fitting:
	simplex fitting routine			= simpfit()
	quadratic fit for standard deviations	= simpdev()
	supporting functions

By J.A. Rupley, Tucson, Arizona
Coded for ECO C compiler, version 3.40
*/





#include <stdio.h>
#include <ctrlcnst.h>

#define NPARM		10

				/* STRUCTURES */

struct pstruct {
	double val ;
	double parm[NPARM] ;
}  ;

struct qstruct {
	int parmndx[NPARM] ;
	double q[NPARM] ;
	double yplus[NPARM] ;
	double yminus[NPARM] ;
	double a[NPARM] ;
	double bdiag[NPARM] ;
	double inv_bdiag[NPARM] ;
	double std_dev[NPARM] ;
}  ;

/* page eject */

/* SIMPFIT
	SIMPLEX MINIMIZATION BY USE OF NELDER-MEAD ALGORITHM,
	FOR MODEL DEFINED IN  <FUNC()> .
*/

					/* defines for simpfit */
#define ILLEGAL		0
#define REFLECT		1
#define STARCONTRACT	2
#define HIGHCONTRACT	3
#define SHRINK		4
#define FAILREFLECT	5
#define STAREXPAND  	6


int simpfit(fptr)
FILE *fptr ;
{
	register int j, i ;

	int opcode ;
	int c ;
	int test_break ;
	int nlow, nhigh  ;

	struct pstruct *p_best, pbar, pstar, p2star ;
	
	void bsort(), fprintf() ;
	int getchar() ;
	double sqrt() ;

	int pvalcomp(), p_swap() ;
	int centroid(), func(), ptrial() ;
	void fpprint(), pcopy() ;

	extern struct pstruct 	p[], pcent, *p_p[] ;
	extern double   	exit_test, mean_func, rms_func, test, rms_data;
	extern int 		iter, maxiter, nparm, nvert, ndata, nfree ;


				/* expansion-contraction coefficients */
	double alpha ;
	double beta ;
	double gamma ;

        			/* descriptions of expn-cntrctn operations */
	static char *opname[] = {
		"illegal",
		"reflect",
		"reflect & contract",
		"contract",
		"shrink on lowest vertex",
		"reflect after fail to expand",
		"reflect and expand"
	}  ;
	
	nlow =		0 ;
	nhigh =		nvert - 1 ;




				/* initialization:
					coefficients
						pointers */
	alpha =	1 ;
	beta  = .5 ;
	gamma =  2 ;

	for (j = 0; j < nvert; j++)
		p_p[j] = &p[j] ;

	fprintf(fptr, 
		"\n\ntype ^X to exit loop, ^S to pause\n\n") ;




				/* MAIN LOOP OF MINIMIZATION */
	while (++iter) {


				/* ascending sort of pointers p_p 
				to struct array p */
		bsort(nvert, pvalcomp, p_swap) ;


				/* form reduced simplex, pbar, 
				for (nvert - 1) vertices 
				ie without high vertex */
		centroid(&pbar, p_p, (nvert - 1)) ;


				/* form pstar, new reflection trial vertex */
		ptrial(&pstar, &pbar, p_p[nhigh], (1 + alpha), -alpha) ;
		func(&pstar) ;






				/* NELDER-MEAD ALGORITHM = test trial vertex
 				vs old high and low vertices ;
				construct new trial vertices as appropriate; 
				set pointer to best new vertex */
		if (pstar.val < p_p[nlow]->val){
			ptrial(&p2star, &pstar, &pbar, (1 + gamma), -gamma) ;
			func(&p2star) ;
			if (p2star.val < p_p[nlow]->val) {
				p_best = &p2star ;
				opcode = STAREXPAND ;
			} else {
				p_best = &pstar ;
				opcode = FAILREFLECT ;
			}
		} else  if (pstar.val <= p_p[nhigh-1]->val) {
				p_best = &pstar ;
				opcode = REFLECT ;
		} else {
			if (pstar.val <= p_p[nhigh]->val) {
				pcopy(p_p[nhigh], &pstar) ;
				opcode = STARCONTRACT ;
			} else
				opcode = HIGHCONTRACT ;

			ptrial(&p2star, p_p[nhigh], &pbar, beta, (1-beta)) ;
			func(&p2star) ;

			if (p2star.val <= p_p[nhigh]->val)
				p_best = &p2star ;
			else
				opcode = SHRINK ;
		}
				/* END OF NELDER-MEAD ALGORITHM */





				/* possible exit from loop on operator kbhit 
				clear keyboard of any extra chars typed */
		test_break = FALSE ;
		if (KBHIT) {
			c = getchar() ;
			while (KBHIT) {
				getchar() ;
			}
			if ((c == CTRLX) || (c == CTRLC))
				test_break = TRUE ;
			else if (c == CTRLS)
				getchar() ;
		}


				/* print results */
		fprintf(fptr, "\n%5d %20.14e %20.14e %s\n",
			iter, p_p[nhigh]->val, p_best->val, opname[opcode]) ;
		fpprint(fptr, p_p[nhigh]) ;
		fpprint(fptr, p_best) ;


				/* adjust simplex 
				either loop over all vertices > lowest = [0]
				to shrink on lowest
				else copy best movement into high vertex */
		if (opcode == SHRINK)
			for (j = 1; j < nvert; j++) {
				for (i = 0; i < nparm; i++)
					p_p[j]->parm[i] = (p_p[nlow]->parm[i] 
							+ p_p[j]->parm[i])/2 ;
				func(p_p[j]) ;
			}
		else
			pcopy(p_p[nhigh], p_best) ;



				/* calculate and print new centroid */
		centroid(&pcent, p_p, nvert) ;
		fpprint(fptr, &pcent) ;


				/* calculate and print
				mean and rms of function values */
		mean_func = 0 ;
		for (j = 0; j < nvert; j++)
			mean_func = mean_func + p[j].val ;
		mean_func = mean_func/nvert ;
	
		rms_func = 0 ;
		for (j = 0; j < nvert; j++)
			rms_func = rms_func 
			     + (p[j].val - mean_func) * (p[j].val - mean_func) ;
		rms_func = sqrt(rms_func/nvert) ;

		test = rms_func/mean_func ;
		rms_data = sqrt(p_p[nlow]->val/(ndata - nfree));		
		fprintf(fptr, "mean=%20.14e rms=%20.14e test=%20.14e\n",
			mean_func, rms_func, test) ;
		fprintf(fptr, 
"root mean square weighted error of best fit to data = %20.14e\n\n",
				rms_data) ;
		fspecial(fptr) ;



				/* exit test
				calculate centroid function value if exit */
		if ((test_break == TRUE) || (iter == maxiter)) {
			func(&pcent) ;
			break ;
		} 
		if (test < exit_test) {
			func(&pcent) ;
			if ((pcent.val - mean_func) < (2 * rms_func))
				break ;
		}
	}		
				/* END OF MAIN LOOP OF MINIMIZATION */

	return (OK) ;
}                          	/* END OF SIMPFIT      			*/

/* page eject */

/* SIMPDEV
	QUADRATIC FIT TO FUNCTION SURFACE FOR ESTIMATION OF:
		VARIANCE-COVARIANCE MATRIX 
		STANDARD DEVIATIONS OF PARAMETERS
*/

int simpdev(fptr)
FILE *fptr ;
{
	register int i, j, k, l ;
	int xfree, free_count ;
	int c ;
	int err_count ;

	double dtemp ;
	double yminusij, yplusij ;

	double tmparray[NPARM] ;

	struct pstruct ptemp ;

	void fprintf() ;
	int getchar() ;
	double sqrt() ;

	int func() ;
	void fpprint(), pcopy() ;
	void fqprint(), fmatprint() ;

	void bsort();
	int pvalcomp(), p_swap() ;

	extern struct pstruct 	*p_p[], p[], pcent, pmin ;
	extern struct qstruct 	q ;
	extern double 		qmat[][NPARM] ;
	extern double   	rms_data, yzero, ymin, ypmin, mse ;
	extern int 		nparm, nvert, ndata, nfree ;




				/* be sure that pcent.val is set */
	if (func(&pcent) == ERROR)
		return (ERROR) ;


				/* ascending sort of pointers p_p 
				to struct array p */
	bsort(nvert, pvalcomp, p_swap) ;

				/* if lowest vertex < centroid, then
				replace centroid by lowest vertex */
	if (p_p[0]->val < pcent.val) {
		pcopy(&pcent, p_p[0]);
		fprintf(fptr, "\n\nlowest vertex replaces centroid");
	}


				/* set yzero */
	yzero = pcent.val ;
	fprintf(fptr, 
		"\n\nlowest function value = yzero = %20.14e\n\n", yzero) ;
	fpprint(fptr, &pcent) ;
	fprintf(fptr, "\n") ;


				/* calculate 
				q value = avg devn of a free parameter 
						from the centroid value ;
				store the q value in structure q :
			        q.q[free_cnt] = q value ;
				also store map of free parm to parm array in
				q.parmndx[free_cnt] = index of parm 
						in p[nvert].parm[nparm] ;
				set nfree and nvert */
	free_cnt = 0 ;
	for (i = 0; i < nparm; i++) {
		dtemp = 0 ;
		for (j = 0; j < nvert; j++)
			dtemp = dtemp + ABS(p[j].parm[i] - pcent.parm[i]) ;
		dtemp = dtemp/nvert ;
		if (ABS(dtemp/pcent.parm[i]) < 1.e-16) {
			fprintf(fptr, "parameter %d is fixed\n", i) ;
			continue ;
		} else {
			q.q[free_cnt] = dtemp ;
			q.parmndx[free_cnt] = i ;
			free_cnt++ ;
		}	
	}
	nfree = free_cnt ;
	if (nvert != (nfree + 1)) {
		fprintf(fptr, "\nerror in count of free parameters\n") ;
		return (ERROR) ;
	}
	fprintf(fptr, 
		"\nq values for the %d free parameters out of %d total:\n", 
		nfree, nparm) ;
	fqprint(fptr, q.q) ;
	
			


				/* check and if necessary adjust q values
				for each free parameter, to be sure that the
				function values are OK for the 
				centroid parameter values  (+  &  -)  q ;
				store function value for parameter value
				(+  &  -)  q   in  q.yplus  and  q.yminus */
	for (i = 0; i < nfree; i++) {
		k = q.parmndx[i] ;
		pcopy(&ptemp, &pcent) ;
		fprintf(fptr, 
			"\nchecking q value %d, for parameter %d: ", i, k) ;

		while (TRUE) {
			fprintf(fptr, "*") ;

			while (TRUE) {
				ptemp.parm[k] = pcent.parm[k] - q.q[i] ;
				if (func(&ptemp) == ERROR)
					q.q[i] = q.q[i] / 2 ;
				else
					break ;
			}
			q.yminus[i] = ptemp.val ;

			dtemp = q.q[i] ;
			while (TRUE) {
				ptemp.parm[k] = pcent.parm[k] + dtemp ;
				if (func(&ptemp) == ERROR)
					dtemp = dtemp / 2 ;
				else
					break ;
			}
			q.yplus[i] = ptemp.val ;

			if (dtemp == q.q[i])
				break ;
			else
				q.q[i] = dtemp ;
		}
	}
	fprintf(fptr, "\n\nadjusted q values:\n") ;
	fqprint(fptr, q.q) ;
	fprintf(fptr, "\nyplus values:\n") ;
	fqprint(fptr, q.yplus) ;
	fprintf(fptr, "\nyminus values:\n") ;
	fqprint(fptr, q.yminus) ;



				/* calculate  
				<a>  vector
				store in q.a */
				/* calculate  
				<B>  matrix diagonal elements ;
				store in  qmat  and  q.bdiag  */
	for (i = 0; i < nfree; i++) {
		q.a[i] = 0.25 * (q.yplus[i] - q.yminus[i]) ;
		qmat[i][i] = q.bdiag[i] = 0.5 * (q.yplus[i] + q.yminus[i]
						- 2 * yzero) ;
	}
	fprintf(fptr, "\n<a> vector:\n") ;
	fqprint(fptr, q.a) ;
	fprintf(fptr, 
		"\n<B> matrix diagonal:\n") ;
	fqprint(fptr, q.bdiag) ;




				/* calculate 
				<B>  matrix off-diagonal elements ;
				keep track of any function errors ;
				store results in  qmat  */
	err_count = 0 ;
	for (i = 1; i < nfree; i++) {
		k = q.parmndx[i] ;
		fprintf(fptr, 
		     "\ncalculating off-diag Bij for row %d: ", i) ;
		for (j = 0; j < i; j++) {
			fprintf(fptr, "*") ;
			l = q.parmndx[j] ;
			pcopy(&ptemp, &pcent) ;

			ptemp.parm[k] = pcent.parm[k] + q.q[i] ;
			ptemp.parm[l] = pcent.parm[l] + q.q[j] ;
			if (func(&ptemp) == ERROR)
				++err_count ;
			yplusij = ptemp.val ;

			ptemp.parm[k] = pcent.parm[k] - q.q[i] ;
			ptemp.parm[l] = pcent.parm[l] - q.q[j] ;
			if (func(&ptemp) == ERROR)
				++err_count ;
			yminusij = ptemp.val ;

			qmat[j][i] = qmat[i][j] = 0.25 * (yplusij + yminusij
				+ 2 * yzero - q.yplus[i] - q.yminus[i]
					    - q.yplus[j] - q.yminus[j]) ;
		}
	}
	if (err_count > 0) {
		fprintf(fptr, "\n\n%d function errors in <B> matrix calculation\n",
			err_count) ;
		return (ERROR) ;
	}
	fprintf(fptr, "\n\n<B> matrix:\n") ;
	fmatprint(fptr, qmat) ;




				/* invert
				<B>  matrix  */
	xfree = nfree - 1 ;
	for (l = 0; l < nfree; l++) {
		tmparray[xfree] = 1 / qmat[0][0] ;

		for (i = 0; i < xfree; i++)
			tmparray[i] = tmparray[xfree] * qmat[0][i+1] ;

		for (i = 0; i < xfree; i++) {
			qmat[i][xfree] = -tmparray[xfree] * qmat[i+1][0] ;
			for (j = 0; j < xfree; j++)
				qmat[i][j] = qmat[i+1][j+1]
					- tmparray[j] * qmat[i+1][0] ;
		}

		for (i = 0; i < nfree; i++)
			qmat[xfree][i] = tmparray[i] ;
	}
	fprintf(fptr, "\n<B> matrix inverse:\n") ;
	fmatprint(fptr, qmat) ;




				/* store
				inv B matrix diagonal in q.inv_bdiag */
				/* calculate
				ymin = yzero - sumij(ai * invBij * aj) */
				/* calculate 
				pmin(k) = centroid(k) - sumj(qi * invBij * aj)
					where k = parmndx[i] if parm[k] free
					and pmin = centroid if parm[k] fixed */
	ymin = yzero ;
	pcopy(&pmin, &pcent) ;
	for (i = 0; i < nfree; i++) {
		q.inv_bdiag[i] = qmat[i][i] ;
		k = q.parmndx[i] ;
		for (j = 0; j < nfree; j++) {
			ymin = ymin - q.a[i] * qmat[i][j] * q.a[j] ;
			pmin.parm[k] = pmin.parm[k] 
					- q.q[i] * qmat[i][j] * q.a[j] ;
		}
	}
	if (func(&pmin) == ERROR)
		fprintf(fptr, "\nerror in y-pmin\n") ;
	ypmin = pmin.val ;





			       /* calculate 
				mse = (func val)/degrees freedom 
				rms error */
				/* calculate 
				variance-covariance matrix
				vcij = qi * invBij * qj * mse */
				/* calculate 
				standard deviations */
	mse = pcent.val / (ndata - nfree) ;
	rms_data = sqrt(mse) ;
	for (i = 0; i < nfree; i++) {
		for (j = 0; j < nfree; j++) {
			qmat[i][j] = qmat[j][i] 
				   = q.q[i] * qmat[i][j] * q.q[j] * mse ;
		}
		q.std_dev[i] = sqrt(qmat[i][i]) ;
	}
	fprintf(fptr, 
		"\nvariance-covariance matrix:\n") ;
	fmatprint(fptr, qmat) ;
	



				/* replace centroid with pmin,
				if y-pmin  <  y-centroid */
				/* reconstruct simplex, based on q values
				and centroid, in preparation for more fitting ;
				nfree vertices are based on nfree q values ;
				the remaining vertex is set at the centroid 
				(or pmin if it is lower) */
	fprintf(fptr,"\n\nCalculating for reconstructed simplex...\n\n") ;
	if (pmin.val < pcent.val)
		pcopy(&p[nfree], &pmin) ;
	else 
		pcopy(&p[nfree], &pcent) ;
	for (i = 0; i < nfree; i++) {
		pcopy(&p[i], &pcent) ;
		k = q.parmndx[i] ;
		p[i].parm[k] = p[i].parm[k] + q.q[i] ;
		func(&p[i]) ;
	}

	return (OK) ;
}                          	/* END OF SIMPDEV      			*/

/* page eject */

/* SUPPORTING  FUNCTIONS 
	NOTE THAT <FUNC()> AND <FDATPRINT()>, <MAIN()>, AND 
	THE INPUT ROUTINE <READ_DATA()> AND ITS SUPPORT ARE IN 
	SEPARATE FILES
*/

int centroid(pdest, psrc, xvert)
struct pstruct *pdest, *psrc[] ;
int xvert ;
{
	register int i, j ;

	extern int 	nparm ;

	for (i = 0; i < nparm; i++) {
		pdest->parm[i] = psrc[0]->parm[i] ;
		for (j = 1; j < xvert; j++)
			pdest->parm[i] = pdest->parm[i] 
				+ (psrc[j]->parm[i] 
					- psrc[0]->parm[i]) / xvert ;
	}
	return (OK) ;

}                          	/* END OF CENTROID    			*/

/* code changes parm even if all parms equal-- WRONG CODE
	for (i = 0; i < nparm; i++) {
		pdest->parm[i] = 0 ;
		for (j = 0; j < xvert; j++)
			pdest->parm[i] = pdest->parm[i] + psrc[j]->parm[i] ;
		pdest->parm[i] = pdest->parm[i] / xvert ;
	} */



int ptrial(pnew, p1, p2, coef1, coef2)
struct pstruct *pnew, *p1, *p2 ;
double coef1, coef2 ;
{
	register int i ;

	extern int 	nparm ;

	for (i = 0; i < nparm; i++)
		pnew->parm[i] = p1->parm[i] + 
					coef2 * (p2->parm[i] - p1->parm[i]) ;
	return (OK) ;
}				/* END OF PTRIAL			*/

/* code that possibly changes parm even if all parms equal-- WRONG CODE
 pnew->parm[i] = (coef1 * p1->parm[i]) + (coef2 * p2->parm[i]) ; */



void pcopy(pdest, psrc)
struct pstruct *pdest, *psrc ;
{
	register int i ;

	extern int 	nparm ;

	pdest->val = psrc->val ;
	for (i = 0; i < nparm; i++)
		pdest->parm[i] = psrc->parm[i] ;

}				/* END OF PCOPY				*/



				/* NOTE-- the swap routine exchanges
					pointers to structure records ;
					the comp routines compare a structure
					member for two structure records, 
					which may be swapped later 	*/

				/* swap contents of array elements x and y
					which are pointers to structure
					records tested in comp routines */
int p_swap(x, y)
int x, y ;
{
	int *temp ;

	extern struct pstruct 	*p_p[] ;

	temp = p_p[x] ;
	p_p[x] = p_p[y] ;
	p_p[y] = temp ;

	return (OK) ;
}				/* END OF P_SWAP				*/



				/* compare float contents of a structure 
				member for records x and y		*/
int pvalcomp(x, y)
int x,y ;
{
	extern struct pstruct 	*p_p[] ;

	if (p_p[x]->val < p_p[y]->val)
		return (-1) ;
	else if (p_p[x]->val > p_p[y]->val)
		return (1) ;
	else 
		return (0) ;
}				/* END OF PVALCOMP			*/



char *str_in(fptr, dummy)
char *dummy ;
FILE *fptr ;
{
	register int i ;

	int isspace() ;
	int getc() ;
	int ungetc() ;

	while (isspace(dummy[0] = getc(fptr))) {
	}

	for (i = 1; !isspace(dummy[i] = getc(fptr)); i++)
		;
	dummy[i] = '\0' ;

	return (&dummy[0]) ;
}				/* END OF *STR_IN			*/



void ffitprint(fptr)
FILE *fptr;
{
	void fprintf() ;
	void fpprint(), fsprint();

	extern int 		iter ;
	extern double 		mean_func, rms_func, test, rms_data ;
	extern struct pstruct 	pcent ;
	extern char		title[] ;


				/* first all vertices of simplex */
	fprintf(fptr, 
"\1\f\n%-s\n\nsummary of simplex fitting results:\niteration number %d\n",
		 title, iter) ;
	fsprint(fptr, "\nsimplex:\n") ;
		
				/*then centroid */
	fprintf(fptr, 
"\ncentroid:\nfunction value = %20.14e\n", pcent.val) ;
	fpprint(fptr, &pcent) ;
	
				/* then test for exit, etc */
	fprintf(fptr, 
"mean=%20.14e rms=%20.14e test=%20.14e\n", mean_func, rms_func, test) ;
	fprintf(fptr, 
"root mean square weighted error of best fit to data = %20.14e\n\n", rms_data);

}				/* END OF FFITPRINT			*/



void fquadprint(fptr)
FILE *fptr;
{
	register int i, k ;

	void fprintf() ;
	void fsprint();

	extern int 		iter, nfree ;
	extern double 		rms_data ;
	extern double 		yzero, ymin, ypmin, mse ; 
	extern double		mean_func, rms_func, test ;

	extern struct qstruct 	q ;
	extern struct pstruct 	pcent, pmin ;
	extern char		title[] ;

	fprintf(fptr, 
"\1\f\n%-s\n\nsummary of results of quadratic fit\niteration number %d\n", 
		title, iter) ;
	fprintf(fptr, "\nmean=%20.14e rms=%20.14e test=%20.14e\n",
		mean_func, rms_func, test) ;
	fprintf(fptr, 
"\nmse    = %20.14e   rms weighted error = %20.14e\n", mse, rms_data) ;
	fprintf(fptr, 
"\nyzero  = %20.14e\nymin   = %20.14e\ny-pmin = %20.14e\n\n", 
		yzero, ymin, ypmin) ;
	
	fprintf(fptr, 
"free parm    q                centroid         pmin             std_dev\n") ;

	for (i = 0; i < nfree; i++) {
		k = q.parmndx[i] ;
		fprintf(fptr, "%3d %3d %17.11e %17.11e %17.11e %17.11e\n", 
		     k, i, q.q[i], pcent.parm[k], pmin.parm[k], q.std_dev[i]) ;
	}
}				/* END OF FQUADPRINT			*/



				/* print nfree x nfree matrix 
				calls fqprint()				*/
void fmatprint(fptr, g)
FILE *fptr ;
double g[][NPARM] ;
{
	register int i ;

	extern int 	nfree ;

	void fqprint() ;

	for (i = 0; i < nfree; i++)
		fqprint(fptr, g[i]) ;
}				/* END OF FMATPRINT			*/



void fqprint(fptr, g)
FILE *fptr ;
double *g ;
{
	register int i ;
	void fprintf() ;

	extern int 	nfree ;

	for (i = 0; i < nfree; i++) {
		if ((i > 0) && ((i % 4) == NULL))
			fprintf(fptr, "\n  ") ;
		fprintf(fptr, "%19.10e", g[i]) ;
	}
	fprintf(fptr, "\n") ;
}				/* END OF FQPRINT			*/



void fpprint(fptr, g)
FILE *fptr ;
struct pstruct *g ;
{
	register int i ;
	void fprintf() ;

	extern int 	nparm ;

	for (i = 0; i < nparm; i++) {
		if (i > 0 && (i % 4) == NULL)
			fprintf(fptr, "\n  ") ;
		fprintf(fptr, "%19.10e", g->parm[i]) ;
	}
	fprintf(fptr, "\n") ;
}				/* END OF FPPRINT			*/



void fsprint(fptr, string)
char *string ;
FILE *fptr ;
{
	register int j ;

	void fprintf() ;
	void fpprint() ;

	extern int 		nvert ;
	extern struct pstruct 	p[] ;
	
	fprintf(fptr, "%s", string) ;
	for (j = 0; j < nvert; j++) {
		fprintf(fptr, "vertex = %d   function value = %20.14e\n", 
			j, p[j].val) ;
		fpprint(fptr, &p[j]) ;
	}
}				/* END OF FSPRINT			*/

