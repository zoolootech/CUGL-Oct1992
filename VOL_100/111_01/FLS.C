/*
HEADER:		;
TITLE:		File list specifier;
VERSION:	1.1;
DATE:		06/16/1981;

DESCRIPTION:	"A useful program, which converts ambiguous file names
		in a command line to lists of specific file names,
		for input to another program in the same command line.
		See SQUEEZER.DOC for details.";

KEYWORDS:	wildexp, dio;
SYSTEM:		CP/M-80;
FILENAME:	FLS.C;
WARNINGS:	"Programs called by fls.c must be able to accept
		directed input.";
SEE-ALSO:	SQ.C, USQ.C, SQUEEZER.DOC;
AUTHORS:	Dick Greenlaw;
COMPILERS:	BDS C;
*/
/************************************************************************/

#define VERSION "1.1   06/16/81"
#define STDERR	4	/* Error output stream (always console) */

#include <bdscio.h>
#include <dio.h>

#define SRCH 17 /*bdos search for file pattern*/
#define SRCHNXT 18
#define SETDMA 26
#define TBUFF (0x80+BASE)	/*default disk buffer*/

struct fcb {		/* File control block */
	char xxx[36];	/* enough for CP/M 2 */
};


main(argc, argv)
int argc;
char *argv[];
{
	int i,c;
	int getchar();		/* Directed io version */
	int putchar();		/* Directed io version */
	char inparg[16];	/* parameter from input */

	dioinit(&argc, argv);	/* obey directed to args */

	fprintf(STDERR, "Parameter list builder - Version %s by\n\tRichard Greenlaw\n\t251 Colony Ct.\n\tGahanna, Ohio 43230\n", VERSION);
	fprintf(STDERR, "Accepts redirection and pipes.\nOmit other parameters for help and prompt\n\n");

	/* Process the parameters in order */
	for(i = 1; i < argc; ++i)
		obey(argv[i]);

	if(argc < 2) {
		fprintf(STDERR, "\nParameters are from command line or (singly) from console input.\n");
		fprintf(STDERR, "Drive names and -options are passed thru.\nAmbiguous file names are expanded. CR or EOF to stop.\n");
		do {
			fprintf(STDERR, "\n*");
			for(i = 0; i < 16; ++i) {
				if((c = getchar()) == EOF)
					c = '\n';	/* fake empty (exit) command */
				if((inparg[i] = c) == '\n') {
					inparg[i] = '\0';
					break;
				}
			}
			if(inparg[0] != '\0')
				obey(inparg);
		} while(inparg[0] != '\0');
	}
	dioflush();	/* clean up any directed io */
}

/*
 * Function to convert an input parameter to a list of
 * output parameters. Drives (d:), options (-string) and
 * specific file names (w/ optional drive) are passed through.
 * Ambiguous file names are expanded (w/ optional drive)
 * or, if not found, are ignored with comment.
 *
 * Any parameter beginning with a '-' and drive: alone
 * are simply passed to the output.
 *
 * Results are sent to standard output (presumably redirected)
 * with one output parameter per line.
 */

obey(afnp)
char *afnp;	/* possible ambiguous file name*/
{
	struct fcb sfcb;
	char *p, *q, i, byteaddr;
	int	n;
	char ufn[15];	/* unambiguous file name */

	if(*afnp == '-' || (*(afnp + 1) == ':' && *(afnp + 2) == '\0'))
		printf("%s\n", afnp);	/* pass through option or drive */
	/* Try to build CP/M FCB */
	else if(setfcb(&sfcb, afnp) == ERROR)
		fprintf(STDERR, "%s is bad afn\n", afnp);
	else {
		/* Search disk directory for all ufns which match afn*/
		for(n = 0; ; ++n) {
			bdos(SETDMA, TBUFF);
			byteaddr = n ? bdos(SRCHNXT,&sfcb) : bdos(SRCH,&sfcb);
			if(byteaddr == 255)
				break;
			p = ufn;
			if(*(afnp+1) == ':') {
				/* Drive spec.*/
				*p++ = *afnp;
				*p++ = ':';
			}

			/*Copy filename from directory*/
			q = TBUFF + 32 * (byteaddr % 4);
			for(i =8; i; --i)
				if((*p = 0x7F & *++q) != ' ') ++p;
			*p++ = '.' ;

			/*Copy file extent*/
			for(i = 3; i; --i)
				if((*p = 0x7F & *++q) != ' ') ++p;
			*p = '\0' ;

			/* Output result */
			printf("%s\n", ufn);
		}
		if(n == 0)
			fprintf(STDERR, "%s not found - ignored\n", afnp);
	}
}
