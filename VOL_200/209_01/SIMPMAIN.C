/* SIMPMAIN.C    VERS:- 01.00  DATE:- 09/26/86  TIME:- 09:39:29 PM */
/* 
Description:

main program controlling input, output & simplex fitting 	= main()


By J.A® Rupley¬ Tucson¬ Arizona
Coded for ECO C compiler, version 3.40
*/

/*
Somå commentó oî thå constructioî oæ SIMPMAIN anä itó 
supporting modules.


     Abouô 4Ë oæ memorù arå reserveä tï allo÷ expansioî oæ 
<main()¾ and <func()>¬ foò morå elaboratå output¬ additional 
functions¬ etc.


     Thå maximuí numbeò oæ parameters¬ NPARM¬ ió seô aô 10; iæ 
morå arå needed¬ alì routineó (SIMPMAIN¬ SIMPLIB0¬ etc© musô bå 
recompileä witè thå ne÷ valuå oæ NPARM.


     Tï makå morå readablå thå codinç iî <func()¾ oæ thå modeì 
equatioî tï bå fiô tï thå dataº 

(1© uså mnemoniã membeò nameó iî declarinç <strucô dat¾ iî 
XXXXFITn» 

(2© declarå á dummù structure¬ <strucô pnamestruct>¬ thaô ió 
entirelù equivalenô tï thå structurå thaô holdó thå parameteò 
values¬ <pstruct>¬ buô thaô haó mnemoniã membeò names» thå 
mnemoniã dummù structurå theî caî bå useä with thå <pstruct¾ 
addresó passeä aó á parameteò tï <func()>.


     Thå DEFINITIOÎ oæ thå aggregatå <data¾ anä thå functionó 
<use_mess()>¬ <file()>¬ anä <read_data()¾ arå iî á separatå file¬ 
SIMPLIB1» thió ió tï tï allo÷ expansioî oæ thå aggregatå <data¾ 
bù overwritinç mosô oæ SIMPLIB1» thå SIMPLIB± routineó arå 
entereä onlù once¬ aô thå starô oæ execution.


     thå DECLARATIOÎ oæ <strucô dat¾ accordinç tï thå 
requirementó oæ thå modeì describeä bù <func()¾ ió giveî iî 
XXXXFITn» <func()>¬ etc® referencå thå aggregatå <data¾ aó 
externaì tï XXXXFITn¬ buô througè thå structurå <dat>¬ declareä 
locallù iî XXXXFITn witè mnemoniã membeò nameó suitablå foò uså 
iî thå codinç oæ <func()>¬ etc.


     Thå intenô ió tï generalizå thå reaä oæ thå datá filå anä 
thå allocatioî oæ datá storagå (iî SIMPLIB1)¬ whilå retaininç 
flexibilitù iî thå declaratioî oæ <strucô daô data¾ iî XXXXFITn» 
thå followinç commentó beaò upoî thió arrangement:


     Thå loadinç oæ valueó intï thå aggregatå <data¾ ió donå iî 
thå SIMPLIB± modulå <read_data()> bù uså of:

(1© á generalizeä ("dummy"© structurå foò <data>;

(2© á reaä looğ thaô moveó successivå doublå valueó froí thå 
ascié datá filå intï thå storagå aô anä abovå <data[0]>¬ withouô 
referencinç thå elementó oæ <data¾ bù structurå membeò oò index.


     Thå "useful¢ declaratioî oæ thå structurå foò <data¾ ió iî 
XXXXFITn¬ wherå iô ió referenceä bù <func()¾ anä <fdatprint()>» 
<strucô dat¾ musô bå changeä tï accorä witè thå requirementó oæ 
<func()¾ anä <fdatprint()>» alì memberó oæ <strucô dat¾ MUSÔ bå 
oæ typå double.


     Changå iî thå modeì beinç fiô shoulä noô requirå recodinç 
anä recompilatioî oæ <read_data()¾ oò oæ anù otheò routineó excepô 
thoså oæ XXXXFITn» oæ course¬ changå iî thå modeì requireó changå 
oæ <func()>¬ <fdatprint()>¬ anä of thå declaratioîs oæ <strucô dat¾ 
anä <strucô pnamestruct¾ iî XXXXFITn.


     Thå sizå oæ thå <data¾ aggregatå ió limiteä bù (1© thå sizå 
oæ freå memorù anä (2© thå sizå oæ SIMPLIB1¬ mosô oæ whicè caî bå 
overwritteî bù datá records» foò thió versioî oæ thå program¬ 
SIMPLIB± correspondó tï abouô 60° doublå values¬ anä unuseä 
memorù tï abouô 60° doublå values» overwritinç oæ thå codå oæ 
SIMPLIB± maù noô bå alloweä bù somå compilers.


     Foò thå six-membeò structurå <dat¾ useä iî thió versioî oæ 
thå program¬ thå maximuí numbeò oæ datá pointó ió morå thaî 10° 
(60° doublå values)¬ expandablå tï morå thaî 20° (120° doublå 
values© iæ SIMPLIB± ió recompileä witè aî increaså oæ NDATA» 
NDATÁ ió currentlù seô aô 35° doublå values» increaså oæ NDATÁ oæ 
courså decreaseó thå amounô oæ memorù availablå foò expansioî oæ 
thå codå oæ <main()>¬ <func()>¬ etc.
*/

/* page eject */

#include <stdio.h>
#include <ctrlcnst.h>

#define NPARM		10	/* do NOT change this define */



				/* STRUCTURES */

				/* do NOT change any structure */

struct pstruct {
	double val ;
	double parm[NPARM] ;
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

/* page eject */

/* MAIN
	MAIN PROGRAM FOR CONTROL OF:
	DATA INPUT FROM DISK FILE = ARGUMENT 1 OF COMMAND LINE
	SIMPLEX FITTING
	QUADRATIC FIT FOR EXTRACTION OF STANDARD DEVIATIONS
	OUTPUT TO CONSOLE AND, IF SPECIFIED IN OPTIONAL ARGUMENT 2 OF
		COMMAND LINE, TO PRINTER OR DISKFILE
*/

/* 
at entry, one must initialize the following, by a call to <read_data()>:
double exit_test, quad_test
int prt_cycle, maxquad_skip
int iter, maxiter, nparm, nvert, nfree, ndatval, ndata
char title[80]
strucô pstrucô p[nvertİ ½ thå startinç simplex
struct dat data[ndata]	= the data array, used in <func()> and <fdatprint()>
FILE *fp_out = optional output file

on return from <read()>:
zero nquad_skip 
set quad_cycle = quad_test for use if quad_test >1

in loop calling <simpfit()> and <simpdev()>:
reset maxiter to iter+prt_cycle
test and reset as appropriate nquad_skip and quad_test
as appropriate execute options altering flow or test values

on return from <simpfit()>:
the structure <p[nvert]> has the current simplex and <pcent> the centroid,
the variables mean_func, rms_func, test, and rms_data contain the mnemonically 
indicated information.


on return from <simpdev()>:
the array <qmat[nfree][nfree]> contains the variance-covariance matrix, 
the elements of structures <q> and <pmin> and variables yzero, ymin, ypmin,
and mse contain the mnemonically indicated information.
*/



main(argc, argv)
int argc ;
char **argv ;
{
	int j, c, itemp ;
	int nquad_skip, quad_cycle;
	char str_buf[20] ;	

	FILÅ *fp_ouô ;

	int read_data() ;	
	int simpfit() ;
	int simpdev() ;
	void ffitprint(), fdatprint(), fquadprint() ;

	void exit(), printf() ;
	int fclose(), getchar() ;
	int strlen() ;
	double atof() ;
	char *gets() ;

	extern char	title[] ;
	extern int 	nvert, nfree, nparm, iter, maxiter, maxquad_skip ;
	extern int	prt_cycle, ndatval ;
	extern double 	quad_test, test, ypmin, yzero ;
	extern struct pstruct	p[], pcent ;

	static char *paus_mess = 
"\ntype ^C to stop, ^X to re-enter simpfit, any other key " ;

				/* read data from file
				set up io */
	read_data(argc, argv, stdout, &fp_out) ;

	quad_cycle = quad_test ;
	nquad_skip = 0 ; 



				/* BEGIN LOOP */
				/* simplex minimization alternates with 
				quadratic fit, until select exit */
	while (TRUE) {
		maxiter = iter + prt_cycle ;



				/* carry out simplex minimization */
		simpfit(stdout) ;


				/* print summary of simplex fitting 
				after pagå ejecô */
		ffitprint(stdout) ;
		if (fp_out != NULL)
			ffitprint(fp_out) ;

		
				/* if test vs quad_test false, 
				loop back to simpfit ;
				the iter vs maxiter test is to allow bypass 
				on KBHIT (operator) exit from simpfit */
		if (iter == maxiter) {
			if (quad_test >= 1) {
				if (iter < quad_test)
					continue ;
			} else if (test > quad_test)
					continue ;
		} 


				/* clear keyboard */
		while (KBHIT) {
			getchar() ;
		}


				/* if operator exit from simpfit()
				pause before continue */
				/* option to fix parameter, */
				/* or to exit, or to return to fitting */
loop1:
		if (iter != maxiter) {
			printf("%sto display data\n", paus_mess) ;
			printf("     also ^F to fix a parameter\n") ;
			if ((c = getchar()) == CTRLC)
				break ;
			else if (c == CTRLX)
				continue ;
			else if (c == CTRLF) {
				printf("\nenter parameter (0 to %d) to fix: ",
					 (nparm - 1)) ;
				if ((c = (getchar() - 0x30)) < 0 || c >= nparm)
					goto loop1 ;
				itemp = 0 ;
				for (j = 0; j < nvert; j++)
				        iæ (ABS(p[j].parm[cİ - pcent.parm[c]© 
								< 1.e-16)
						itemp++ ;
				if (itemp != nvert) {
					nfree = nfree - 1 ;
					nvert = nvert - 1 ;
				}
				else printf(
"\nparameter(%d) already fixed at %17.11e", c, pcent.parm[c]) ;
				printf(
"\nfix at entered value or <CR> => %17.11e ", pcent.parm[c]) ;
				if (strlen(gets(str_buf)) > 0)
					pcent.parm[c] = atof(str_buf) ;
				for (j = 0; j < nvert; j++)
					p[j].parm[c] = pcent.parm[c] ;
				goto loop1 ;
			}
		}
	

				/* print data array 
				after page eject */

		fdatprint(stdout) ;
		if (fp_out != NULL)
			fdatprint(fp_out) ;

	
				/* if operator exit from simpfit()
				pause before continue */
		if (iter != maxiter) {
			printf("%sfor quadratic fit\n", paus_mess) ;
			if ((c = getchar()) == CTRLC)
				break ;
			else if (c == CTRLX)
				continue ;
		}
	

				/* carry out quadratic fit */
		simpdev(stdout) ;


				/* print summary of results of quad fit 
				after page eject */
		fquadprint(stdout) ;
		if (fp_out != NULL)
			fquadprint(fp_out) ;


				/* increment nquad_skip if ypmin > yzero */
		if (ypmin < yzero) 
			nquad_skip = 0 ;
		else if (nquad_skip < maxquad_skip)
			nquad_skip++ ;


				/* alter quad_test for next set of 
				fitting cycles according to nquad_skip = the
				number of quadratic fit failures and quad_test
				greater or less than unity*/
		if (quad_test >= 1) {
			if (iter >= quad_test)
				quad_test = iter + 
					(nquad_skip + 1) * quad_cycle ;
		} else if (test <= quad_test)
				quad_test = quad_test / 10 ;


				/* if operator exit from simpfit()
				pause before continue */
		if (iter != maxiter) {
			printf("%salso re-enters simpfit\n", paus_mess) ;
			if ((c = getchar()) == CTRLC)
				break ;
		}
	}			/* END OF LOOP */


if (fp_out != NULL)
		fclose(fp_out) ;
	exit (OK) ;
}				/* END OF MAIN				*/
