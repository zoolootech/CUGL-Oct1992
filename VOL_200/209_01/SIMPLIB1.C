/* SIMPLIB1.C   VERS:- 01.00  DATE:- 09/26/86  TIME:- 09:39:26 PM */
/* 
Description:

routines for input of data and control values for simplex minimization:
	definition of the aggregate <data>, with a 
		dummy structure declaration
	usage message displayed on command line error 		= use_mess()
	opening of files for input and optional output		= file()
	input of variables and data				= read_data()

the routines are (largely) independent of the function and data to be fit
	(coded in XXXXFITn), and they (generally) need not be altered when 
	XXXXFITn is modified.

the DEFINITION of the aggregate <data> and the functions <use_mess()>, 
<file()>, and <read_data()> are in a separate file, SIMPLIB1; 
this is to to allow expansion of the aggregate <data> by overwriting 
most of SIMPLIB1;
the SIMPLIB1 routines are entered only once, at the start of execution.

the DECLARATION of <struct dat> according to the requirements of the model
function described by <func()> is given in XXXXFITn;
<func()>, etc. reference the aggregate <data> as external to XXXXFITn, 
but through the structure <dat>, declared locally in XXXXFITn and with
mnemonic member names suitable for use in coding of <func()>, etc.;
for more detail on this arrangement, see the header and documentation 
of XXXXFITn and SIMPMAIN.


By J.A. Rupley, Tucson, Arizona
Coded for ECO C compiler, version 3.40
*/

/* page eject */

#include <stdio.h>
#include <ctrlcnst.h>


#define NDATA		300	/* space for NDATA double values are allocated
				as storage at the start of the SIMPLIB1
				module; this allocation can be changed, 
				according to the need for more data points, as
				balanced against the need for possible 
				expansion of the code of <main()> & <func()> */


#define NPARM		10	/* do NOT change this define */



				/* EXTERNALS AND STRUCTURES */

				/* do NOT change any structure */

				/* the use of a dummy structure in the
				definition of <data> is to allow: 
				(1) a generalized read of the data file into
				the data array, in <read_me>;
				(2) the "true" declaration of the structure
				for <data> in a different file, XXXXFITn,
				that must be recompiled with each change in
				the function and data being fit;
				it should not be necessary to recompile
				this file, SIMPLIB1 */
struct dummy {			
	double dummy ;		
} data[NDATA] ;

struct pstruct {
	double val ;
	double parm[NPARM] ;
} ;

/* page eject */

void use_mess()
{
	void puts() ;

puts("\nUsage:") ;
puts("A>simpfitx   [d:]input_file   [[d:]output_file]") ;
puts("   the optional output file can be LST: or a disk file;") ;
puts("   the required input file has the following structure:") ;
puts("     one-line title, with no tabs or ctrl characters;") ;
puts("     lines following give control variables, the starting simplex,") ;
puts("              and the data array;") ;
puts("     the order of entry is fixed by the order in <read_data()>;") ;
puts("              see sample file for structure;") ;
puts("     the one-word descriptors must be present in the data file;") ;
puts("     the data format is free-form (no set field widths);");
puts("\n") ;
puts("      comments at the end of the file are not read by the program") ;
puts("              and any other information to be stored should be there") ;
puts("\n") ;
}				/* END OF USE_MESS			*/





FILE *file(argc, argv, nargv, string)
int argc ; 
char *argv[] ;
int nargv ;
char *string ;
{
	void exit(), printf() ;
	FILE *fptr, *fopen() ;

	if (argc < (nargv + 1)) {
		use_mess() ;
		exit (ERROR) ;
	}

	if ((fptr = fopen(argv[nargv], string)) == NULL) {
		printf("\nSorry, I cannot open the file.\n\n") ;
		return (fptr) ;
	}
	printf("\nOpen file %s    for \"%s\"\n", argv[nargv], string) ;

	return (fptr) ;
}				/* END OF FILE				*/

/* page eject */

/* READ_DATA
	ENTRY OF TITLE, CONTROL VARIABLES, STARTING SIMPLEX, AND DATA
		FROM DISK FILE GIVEN AS ARGUMENT 1 OF COMMAND LINE
	MANIPULATION OF DATA PRIOR TO FITTING SHOULD BE DONE HERE (OR IN
		MAIN())
	THE DISK DATA FILE IS FREE-FORM (IE, WITHOUT FIELD WIDTHS), 
		BUT THE SEQUENCE OF ENTRIES MUST BE EXACTLY AS BELOW
	THE MNEMONIC IDENTIFIERS MUST BE IN THE DATA FILE (THEY ARE
		DISCARDED IN THE READ, BUT ARE ASSUMED TO BE PRESENT)
	THE IDENTIFIERS CANNOT CONTAIN WHITE SPACE
	THE ONE-LINE TITLE CANNOT CONTAIN CONTROL CHARACTERS
*/

int read_data(argc, argv, fp_out, p_fp_optional)
int argc ;
char *argv[] ;
FILE *fp_out ;
FILE **p_fp_optional ;
{
	register int i, j ;
	char dummy[80] ;
	double *p_x ;

	extern char		title[] ;
	extern double 		exit_test, quad_test ;
	extern int 		iter, maxiter, nparm, nvert, ndata, nfree ;
	extern int		maxquad_skip, prt_cycle, ndatval ;
	extern struct dummy 	data[] ;
	extern struct pstruct 	p[] ;

	char *str_in() ;
	void fpprint(), fsprint(), use_mess() ;
	int func() ;

	double atof() ;
	int atoi() ;
	int getc(), iscntrl() ;
	void fprintf(), exit(), fclose() ;

	FILE *fptr, *file() ;




					/* open data file specified in the 
					command line */
	if ((fptr = file(argc, argv, 1, "r")) == NULL)
		exit (ERROR) ;



					/* open optional output file */
	*p_fp_optional = NULL ;
	if (argc > 2)
		if (!(strcmp(argv[2], "STDLST")))
			*p_fp_optional = stdlst ;
		else if (!(strcmp(argv[2], "LST:")))
			*p_fp_optional = stdlst ;
		else if (!(strcmp(argv[2], "PRINTER")))
			*p_fp_optional = stdlst ;
		else if ((*p_fp_optional = file(argc, argv, 2, "w")) == NULL)
			exit (ERROR) ;


					/* read and print
					title */
	for (i = 0; i < 79; i++) {
		if (iscntrl(title[i] = getc(fptr)))
			break ;
	}
	title[i] = '\0' ;
	fprintf(fp_out, "\n%-s\n", title) ;



					/* read and print
					nvert, nparm, ndata, ndatval */
	str_in(fptr, dummy) ;
	fprintf(fp_out, "\nnvert = %d\n",  
			nvert = atoi(str_in(fptr, dummy))) ;
	str_in(fptr, dummy) ;
	fprintf(fp_out, "\nnparm = %d\n",  
			nparm = atoi(str_in(fptr, dummy))) ;
	str_in(fptr, dummy) ;
	fprintf(fp_out, "\nndata = %d\n",  
			ndata = atoi(str_in(fptr, dummy))) ;
	str_in(fptr, dummy) ;
	fprintf(fp_out, "\nndatval = %d\n",  
			ndatval = atoi(str_in(fptr, dummy))) ;
	nfree = nvert - 1 ;


					/* read and print
					iter, maxiter, exit_test, 
					prt_cycle, quad_test */
	str_in(fptr, dummy) ;
	fprintf(fp_out, "\niter = %d\n",  
			iter = atoi(str_in(fptr, dummy))) ;
	str_in(fptr, dummy) ;
	fprintf(fp_out, "\nmaxquad_skip = %d\n",  
			maxquad_skip = atoi(str_in(fptr, dummy))) ;
	str_in(fptr, dummy) ;
	fprintf(fp_out, "\nexit_test = %20.14e\n",  
			exit_test = atof(str_in(fptr, dummy))) ;
	str_in(fptr, dummy) ;
	fprintf(fp_out, "\nprt_cycle = %d\n",  
			prt_cycle = atoi(str_in(fptr, dummy))) ;
	str_in(fptr, dummy) ;
	fprintf(fp_out, "\nquad_test = %20.14e\n",  
			quad_test = atof(str_in(fptr, dummy))) ;



					/* read 
					simplex */
	fprintf(fp_out, "\n\n\n%-79s\n", str_in(fptr, dummy)) ;
	for (j = 0; j < nvert; j++) {
		for (i = 0; i < nparm; i++) {
			p[j].parm[i] = atof(str_in(fptr, dummy)) ;
		}
	}



					/* read and print
					data array */
	fprintf(fp_out, "\n%-79s\n", str_in(fptr, dummy)) ;
	p_x = &data[0] ;
	for (j = 0; j < ndata; j++) {
		for (i = 0; i < ndatval; i++, p_x++)
			*p_x = atof(str_in(fptr, dummy)) ;
	}
	maxiter = 0 ;
	fdatprint(fp_out) ;

					/* calculate function values
					and print simplex */
	fprintf(fp_out, "\n\ncalculating function values: ") ;
	for (j = 0; j < nvert; j++) {
		func(&p[j]) ;
		fprintf(fp_out, "%d ", j) ;
	}
	fsprint(fp_out, "\n\nsimplex:\n") ;

	fclose(fptr) ;
	return (OK) ;
}				/* END OF READ_DATA			*/
