/*
HEADER:		CUG186.26;
TITLE:		Display nesting level of C code;
VERSION:	1.3;
DATE:		06/06/1986;
DESCRIPTION:	"Displays the nesting level number of each BEGIN/END group";
KEYWORDS:	NESTING, BRACES;
SYSTEM:		CP/M-80, V2.2;
FILENAME:	CHECK.C;
SEE-ALSO:	CHECK.COM
AUTHORS:	Ted Rabenko, Richard Conn;
COMPILERS:	C80;
*/
/*  LCHECK by Richard Conn

        LCHECK displays to the user the nesting level number of each
BEGIN/END ({/}) group, thereby helping him to identify problem areas
in his C programs.  It recognizes quoted material and comments and
ignores { and } within these.

08/01/84 - Fixed bug that caused program to treat an \' or \" as a "live"
           quote.  (Therefore, a statement such as "#define QUOTE '\''"
           would screw up the level counter in the program.  Changed version
           number to 1.2A.  (Don McCrady)

06/06/86 - Removed terminal dependent functions. Prevent read past EOF
	   if in the middle of quoted strings or comments. Converted
	   for use with C80.  Changed revision level to 1.3. (Ted Rabenko)
*/

#define VERS    "1.3"  /* Version Number */

#include        "stdio.h"


#define TWIDTH  132     /* Terminal width */
#define ESC     0x1B    /* Escape control sequence */
#define quote   0x27    /* Single Quote */
#define dquote  0x22    /* Double Quote */
#define bslash  0x5c    /* Backslash escape character */
#define BS      0x08    /* Back Space Char */
#define TAB     0x09    /* Tab Char */
#define LF      0x0a    /* Line Feed Char */
#define CR      0x0d    /* Carriage Return Char */
#define ovfl    'Y'     /* Line Overflow */
#define noovfl  'N'     /* No Line Overflow */

int     level, pos, nroutines;

main(argc,argv)
int argc;
char **argv;
{
        int chval, done;
	FILE	in_fp;

        if (argc == 1) {
                printf("CHECK, Version %s\n",VERS);
                printf("Format of Command Line is --\n");
                printf("  CHECK filename.typ");
                exit(FALSE);
                }
        if ((in_fp=fopen(argv[1],"r")) == NULL) {
                printf("Cannot Find File %s\n",argv[1]);
                exit(FALSE);
                }
        printf("CHECK, Version %s -- File:  %s\n",VERS,argv[1]);
        level = 0; nroutines = 0;  /* Init nesting level, routine count */
        prlevel();  /* Print level number */
        while ((chval=getit(in_fp)) != EOF)
	{
		switch (chval) {
			case bslash: getnoeof(in_fp);
				     break; /* Ignore escaped characters */

			case quote:  while(getnoeof(in_fp) != quote)
					   ; /* If quote, flush to end quote */
				     break;  

			case dquote: while(getnoeof(in_fp) != dquote)
					   ; /* If dquote, flush to end */
				     break; 

			case '/'  :	    /* Possible comment */
                        	     if ((chval=getnoeof(in_fp)) == '*')
                                           do {	      /*Yes, it is a comment*/
                                               while(getnoeof(in_fp) != '*')
						   ; /* End comment? */
                                	   } while (getnoeof(in_fp) != '/');
				     break;

			case '{'  :  level++;    /* BEGIN */
				     break;

			case '}'  :  level--;    /* END */
				     if (level == 0) {
				       nroutines++;
				       printf("\n** Routine %d **", nroutines);
				     }
			}
	} 

	printf("\nProgram Level Check is ");
	if (level == 0) printf("OK");
		else printf("NOT OK");
	printf("\nNumber of Routines Encountered: %d",--nroutines);
}


int getnoeof(fp)   /* test for EOF before return */
FILE fp;
{
	int chval;
	if ((chval = getit(fp)) == EOF)
	{
		printf("\npremature EOF\n");
		exit();
	}
	return(chval);
}

int getit(fp)  /* Get and Echo Character */
FILE fp;
{
	int chval;
	chval = getc(fp);
	if ((pos >= TWIDTH) & (chval != CR)) prlevel(ovfl);
	if (chval != EOF)
		 echo(chval);
	return(chval);
}

echo(chval)  /* Echo Char with tabulation */
char chval;
{
	switch (chval) {
		case TAB : putchar(' '); pos++;
			   while (pos%7 != 0) {
				putchar(' ');
				pos++;
				}
			   break;
		case BS  : putchar(BS);
			   pos--;
			   break;
		case LF  : prlevel(noovfl);
			   break;
		case CR  : putchar(CR);
			   pos = 0;
			   break;
		default  : if (chval >= ' ') {
				putchar(chval);
				pos++;
				}
			   break;
		}
}

prlevel(ovfl_flag)  /* Print Level Number and Set Col Count */
char ovfl_flag;
{
	putchar(LF);
	if (level < 10) printf(" %d",level);
		else printf("%d",level);
	if (ovfl_flag == 'Y') putchar('-');
		else putchar(':');
	putchar(' ');
	pos = 5;
}
