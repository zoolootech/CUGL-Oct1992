/* SIMPMAIN.C    VERS:- 01.00  DATE:- 09/26/86  TIME:- 09:39:29 PM */
/* 
Description:

main program controlling input, output & simplex fitting 	= main()


By J.A� Rupley� Tucson� Arizona
Coded for ECO C compiler, version 3.40
*/

/*
Som� comment� o� th� constructio� o� SIMPMAIN an� it� �
supporting modules.


     Abou� 4� o� memor� ar� reserve� t� allo� expansio� o� �
<main()� and <func()>� fo� mor� elaborat� output� additional 
functions� etc.


     Th� maximu� numbe� o� parameters� NPARM� i� se� a� 10; i� �
mor� ar� needed� al� routine� (SIMPMAIN� SIMPLIB0� etc� mus� b� �
recompile� wit� th� ne� valu� o� NPARM.


     T� mak� mor� readabl� th� codin� i� <func()� o� th� mode� �
equatio� t� b� fi� t� th� data� 

(1� us� mnemoni� membe� name� i� declarin� <struc� dat� i� �
XXXXFITn� 

(2� declar� � dumm� structure� <struc� pnamestruct>� tha� i� �
entirel� equivalen� t� th� structur� tha� hold� th� paramete� �
values� <pstruct>� bu� tha� ha� mnemoni� membe� names� th� �
mnemoni� dumm� structur� the� ca� b� use� with th� <pstruct� �
addres� passe� a� � paramete� t� <func()>.


     Th� DEFINITIO� o� th� aggregat� <data� an� th� function� �
<use_mess()>� <file()>� an� <read_data()� ar� i� � separat� file� �
SIMPLIB1� thi� i� t� t� allo� expansio� o� th� aggregat� <data� �
b� overwritin� mos� o� SIMPLIB1� th� SIMPLIB� routine� ar� �
entere� onl� once� a� th� star� o� execution.


     th� DECLARATIO� o� <struc� dat� accordin� t� th� �
requirement� o� th� mode� describe� b� <func()� i� give� i� �
XXXXFITn� <func()>� etc� referenc� th� aggregat� <data� a� �
externa� t� XXXXFITn� bu� throug� th� structur� <dat>� declare� �
locall� i� XXXXFITn wit� mnemoni� membe� name� suitabl� fo� us� �
i� th� codin� o� <func()>� etc.


     Th� inten� i� t� generaliz� th� rea� o� th� dat� fil� an� �
th� allocatio� o� dat� storag� (i� SIMPLIB1)� whil� retainin� �
flexibilit� i� th� declaratio� o� <struc� da� data� i� XXXXFITn� �
th� followin� comment� bea� upo� thi� arrangement:


     Th� loadin� o� value� int� th� aggregat� <data� i� don� i� �
th� SIMPLIB� modul� <read_data()> b� us� of:

(1� � generalize� ("dummy"� structur� fo� <data>;

(2� � rea� loo� tha� move� successiv� doubl� value� fro� th� �
asci� dat� fil� int� th� storag� a� an� abov� <data[0]>� withou� �
referencin� th� element� o� <data� b� structur� membe� o� index.


     Th� "useful� declaratio� o� th� structur� fo� <data� i� i� �
XXXXFITn� wher� i� i� reference� b� <func()� an� <fdatprint()>� �
<struc� dat� mus� b� change� t� accor� wit� th� requirement� o� �
<func()� an� <fdatprint()>� al� member� o� <struc� dat� MUS� b� �
o� typ� double.


     Chang� i� th� mode� bein� fi� shoul� no� requir� recodin� �
an� recompilatio� o� <read_data()� o� o� an� othe� routine� excep� �
thos� o� XXXXFITn� o� course� chang� i� th� mode� require� chang� �
o� <func()>� <fdatprint()>� an� of th� declaratio�s o� <struc� dat� �
an� <struc� pnamestruct� i� XXXXFITn.


     Th� siz� o� th� <data� aggregat� i� limite� b� (1� th� siz� �
o� fre� memor� an� (2� th� siz� o� SIMPLIB1� mos� o� whic� ca� b� �
overwritte� b� dat� records� fo� thi� versio� o� th� program� �
SIMPLIB� correspond� t� abou� 60� doubl� values� an� unuse� �
memor� t� abou� 60� doubl� values� overwritin� o� th� cod� o� �
SIMPLIB� ma� no� b� allowe� b� som� compilers.


     Fo� th� six-membe� structur� <dat� use� i� thi� versio� o� �
th� program� th� maximu� numbe� o� dat� point� i� mor� tha� 10� �
(60� doubl� values)� expandabl� t� mor� tha� 20� (120� doubl� �
values� i� SIMPLIB� i� recompile� wit� a� increas� o� NDATA� �
NDAT� i� currentl� se� a� 35� doubl� values� increas� o� NDAT� o� �
cours� decrease� th� amoun� o� memor� availabl� fo� expansio� o� �
th� cod� o� <main()>� <func()>� etc.
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
struc� pstruc� p[nvert� � th� startin� simplex
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

	FIL� *fp_ou� ;

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
				after pag� ejec� */
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
				        i� (ABS(p[j].parm[c� - pcent.parm[c]� 
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