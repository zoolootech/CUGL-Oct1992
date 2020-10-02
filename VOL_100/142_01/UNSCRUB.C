/*
	Program to 'unscrub' an ASCII text file for processing with
	Wordstar.	Ref Your Computer May 1984, p 120

	Modification History - most recent first.

	14 Oct 1984, Modified to compile using Aztec CII
	ver 1.05g under CP/M 2.2.  Functions agetc and aputc 
	used are ASCII versions of getc and putc which translate
	\n from and to CR/LF.  Nested switch in linefeed count
	detection rewritten to appease Aztec compiler.  The SECOND
	successive linefeed is taken to mean a new paragraph, and
	put out as 2 CR/LF pairs, to make up for the one lost. 
	Alan Coates.

	18 Feb 1984, Bill Bolton, from idea by Harvey Lord.
	Digital Research C.

 */

#include "libc.h"

/*	Macros for constant definitions		*/

#define VERS 1		/* main version number	*/
#define REVISION 1	/* sub version number	*/
#define DEL 0x7F	/* ASCII delete		*/
#define WORKING 1024
#define NEXTLINE (WORKING*32)
#define CPMEOF 0x1A
#define ERROR 0
#define FERROR -1
#define NO 0
#define void int	/* Aztec CII v 1.05g doesn't like void */
#define YES_CR1 1
#define YES_CR2	2
#define YES_SP	3
/*
	Argument vector indices
 */

#define FROM_FILE	1
#define TO_FILE		2
/*
	main to open files for unscrub()
	and handle invocation errors
 */

main(argc,argv)
int argc;
char *argv[];
{
	FILE *fdin,*fdout;
	char buf[12];
	printf("\n\tWordstar file UNSCRUBber, CP/M-80 Version %d.%d\n",
		VERS,REVISION);
	printf("Bill Bolton, Software Tools, modified by Alan Coates\n");
	if ( argc != 3 )
		usage();
	else {
		if( (fdin = fopen(argv[FROM_FILE],"r")) == ERROR){
		  printf("\nCant find file%s\n",argv[FROM_FILE]);
		  usage();
		}
		else {
			if( (fdout = fopen(argv[TO_FILE],"w")) == ERROR)
			  printf("\nCant open %s\n",argv[TO_FILE]);
			else{
				printf("\nWorking ");
				unscrub(fdin,fdout);
			}
		}
	}
	exit(0);	/* dummy argument in Aztec CII */
}
/*
	procedure unscrub -- copy file to file deleting unwanted control chars
 */

void unscrub(fpin,fpout) /* Aztec doesn't like same name for arguments */
FILE *fpin;	/* the input file buffer	*/
FILE *fpout;	/* and the output	*/

{
	int c;		/* 1 char buffer	*/
	int loop;	/* counter		*/
	long count;	/* chars processed	*/
	long killed;	/* bytes deleted	*/
	int cr_flag;
	int space_flag;

	loop 	= 0;
	count 	= 0;
	killed	= 0;
	cr_flag = NO;	
	space_flag = NO;

	while((c = agetc(fpin)) != EOF && c != CPMEOF)
	{
		c &= 0x7F;
		loop++;
		count++;
		if (loop % WORKING == 0)
		{
			printf("*");	/* still alive! 	*/
			if (loop % NEXTLINE == 0)
				printf("\n\t");	/* newline at intervals	*/
			
		}

		switch (c)
		{
		case '\n':
			if (cr_flag == NO){
				putc(' ',fpout); /* replace with a space */
				cr_flag = YES_CR1;	/* reset flag */
				killed++;
			}
			else if (cr_flag == YES_CR1){
				aputc('\n',fpout); 
				aputc('\n',fpout); /* 2xCR/LF, new para */
				cr_flag = YES_CR2;
			}
			else
				aputc('\n',fpout); /* save other linefeeds */
			break;

		case ' ':
			if (space_flag == NO)	/* first space?	*/
				putc(c,fpout);	/* then keep it	*/
			else
				killed++;
			space_flag = YES_SP;	/* flag to discard later sp */
			break;

		default:
			cr_flag = NO;
			space_flag = NO;
			putc(c,fpout);		/* write all else to output */
			break;
		}
	}

	putc(CPMEOF,fpout);
	printf("\n");
	if (fclose(fpout) == FERROR)
		exit(puts("\nOutput file close error\n"));
	
	printf("\n%ld characters processed\n",count);
	printf("%ld characters were deleted.\n",killed);
}

void usage()

{
	printf("\nUsage:\n");
	printf("\tUNSCRUB d:file1 d:file2\n\n");
	printf("Prepares a single spaced ASCII file for reformatting\n");
	printf("with Wordstar, by removing CR/LFs from the end of lines\n");
	printf("within single spaced paragraphs, removing excess spaces\n");
	printf("between words, leaving the file ready for ^B formatting\n");
	exit(0);
}
