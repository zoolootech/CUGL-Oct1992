/*
	Program to "scrub" a WordStar text file back to a
	standard ASCII file.

VERSION LIST, most recent version first

14/Oct/84
	Speedup (as given in Your Computer May 1984 by Bill Bolton),
	and adapted to Aztec CII v1.05g for CP/M 2.2 by Alan Coates.

26/Sep/82
	Forces MSB of all characters to 0, then scans for control
	codes. TAB, CR and LF are passed unchanged to the output
	file. US (soft hyphen) is replaced by a hard hyphen.
	Checking for legal CP/M filename on destination file
	added. Expanded "usage" message. Added "working" messages.
	Bill Bolton. 

	This program was developed from a program called SCRUB
	on BDS "C" User Group disk "Utilities 2" (Volume 2 in
	the Software Tools RCPM BDSCAT.ALL).
*/


#include "libc.h"	/* Aztec header file*/
#define void int	/* void not supported */
#define VERSION 1	/* main version number */
#define REVISION 3	/* sub version number */
#define DEL 0x7F	/* ASCII delete character */
#define WORKING 1024	/* number of chars between progress markers */
#define NEXTLINE (WORKING * 32) /* number of progess chars on a screen line */
#define CPMEOF 0x1a	/* changed from 0x19	*/

#define FROM_FILE 1
#define TO_FILE   2

/*
	main to open the files for scrub()
	and handle invocation errors.
*/

main(argc,argv)
int argc;
char *argv[];

{
	FILE *fdin;
	FILE *fdout;
	char buf[12];
	void scrub();
	printf("\n\tWordStar file Scrubber Version %d.%d\n",VERSION,REVISION);
	printf("Bill Bolton, Software Tools, modified by Alan Coates\n");
	if( argc != 3 )
			usage();
	fdin = fopen(argv[FROM_FILE],"r");
	if (fdin == NULL){
		printf("\nCannot find file %s\n",argv[FROM_FILE]);
		usage();
	}
	fdout  = fopen(argv[TO_FILE],"w");
	if (fdout == NULL){
		printf("\nCan't open %s\n",argv[TO_FILE]);
		usage();
	}
	printf("\nWorking ");
	scrub(fdin,fdout);
	exit(0);
}

/*
	procedure scrub -- copy file to file deleting unwanted control chars
*/

void scrub(fpin,fpout)
FILE *fpin;	/* the input file buffer */
FILE *fpout;	/* the output file buffer */

{
	int c;			/* 1 char buffer */
	int loop;		/* a loop counter*/
	long count;		/* count of characters processed */
	long killed;		/* numbers of bytes deleted */
	long hyphen;		/* number of soft hyphens replaced */

	loop = 0;
	count  = 0;
	killed = 0;
	hyphen = 0;

	while( (c = agetc(fpin)) != EOF  && c != CPMEOF ){
		c &= 0x7F;
		loop++;
		count++;
		if (loop % WORKING == 0)
			printf("*");		/* still alive */
		if (loop % NEXTLINE == 0)
			printf("\n\t");		/* new line every so often */
		if( c >= ' ' && c < '\177' )	/* visible character ? */
			aputc(c,fpout);
		else	{
			switch(c) {
				case '\r':
				case '\n':
				case '\t':
					aputc(c,fpout); /* ok control chars */
					break;

				case '\037':	/* replace WS soft hyphen */
					aputc('-',fpout);
					hyphen++;
					break;

				default:
					killed++;
					break;		 /* ignore it */
  			}
		}
	}
	aputc(CPMEOF,fpout);			 /* sent textual end of file */
	printf("\n");
	if( fclose(fpout) == EOF ){		/* fclose returns -1 on error*/
		printf("\nOutput file close error\n");
		exit(0);
	}
	printf("\n%d characters processed\n",count);
	printf("%d characters were deleted\n",killed);
	printf("%d soft hyphens replaced\n",hyphen);
}

void usage()

{
		printf("\nUsage:\n\n");
		printf("\tSCRUB d:file1 d:file2\n\n");
		printf("Where:\n");
		printf("\tfile1 = source file, (* and ? not allowed)\n");
		printf("\tfile2 = destination file, (* and ? not allowed)\n");
		printf("\td:    = optional drive identifier\n\n");
		printf("e.g.\tSCRUB A:FOOBAR.WST B:FOOBAR.SCR\n");
		exit(0);	/* dummy argument in Aztec v 1.05g */
}

/* end of scrub */
