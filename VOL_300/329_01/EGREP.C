/*
**	name:		egrep
**	purpose:	Regular expresion matcher
*/
#define LINT_ARGS
#include <stdio.h>
#include <ctype.h>
#include "regexp.h"
char *fgets();
/*
 * egrep.
 */


char	*documentation[] = 
{
"egrep searches a file for a given pattern.  Execute by",
"   egrep [flags] regular_expression file_list",
"",
"Flags are single characters preceeded by '-':",
"   -c      Only a count of matching lines is printed",
"   -f      Print file name for matching lines switch, see below",
"   -n      Each line is preceeded by its line number",
"   -v      Only print non-matching lines",
NULL
};


char	*patdoc[] = 
{
"The regular_expression defines the pattern to search for.",
"The expression should be quoted to prevent file-name translation.",
"x      An ordinary character (not mentioned below) matches that character.",
"'\\'    The backslash quotes any character.  \"\\$\" matches a dollar-sign.",
"'^'    A circumflex at the beginning of an expression matches the",
"       beginning of a line.",
"'$'    A dollar-sign at the end of an expression matches the end of a line.",
"'.'    A period matches any character except \"new-line\".",
"'*'    An expression followed by an asterisk matches zero or more",
"       occurrances of that expression.",
"'+'    An expression followed by a plus sign matches one or more",
"       occurrances of that expression.",
"'?'    An expression followed by a question mark matches zero or one",
"       occurrences of that expression.",
"'[]'   A string enclosed in square brackets matches any character in",
"       that string, but no others.  If the first character in the",
"       string is a circumflex, the expression matches any character",
"       except \"new-line\" and the characters in the string.",
"       If two characters in the string are seperated by \"-\"",
"       this matches all characters between and including those two",
"       characters.  To include a \"]\" in the sequence, make it the first",
"       character.  To include \"-\" in the sequence, make it the first or",
"       last character.",
"'()'   A regular expression in parentheses matches that regular expression.",
"'|'    Two regular expressions joined by \"|\" match either the first, or",
"       the second regular expression.",
"       The concatenation of regular expressions is a regular expression.",
NULL
};

int	cflag;
int	fflag;
int	nflag;
int	vflag;
int	nfile;

int	debug	=	0;	   /* Set for debug code      */

char	*pp;
#define LMAX 512
char	lbuf[LMAX];
regexp	*pbuf;
/*******************************************************/

int main(argc, argv)
char *argv[];
{
	register char   *p;
	register int    i,c;
	int		   gotpattern;

   FILE 	   *f;

   	if (argc <= 1)
      usage("No arguments");
   	if (argc == 2 && (0 == strncmp(argv[1],"-h",2) || 
						0 == strncmp(argv[1],"-H",2))
		)
	{
      help(documentation);
      help(patdoc);
      return(1);
	}
   nfile = argc-1;
   gotpattern = 0;
   for (i=1; i < argc; ++i) 
   {
      p = argv[i];
      if (*p == '-') 
	  {
		 ++p;
		 while (0 != (c = *p++)) 
		 {
		    switch(tolower(c)) 
			{
		    case 'h':
		       help(documentation);
		       break;
		    case 'c':
		       ++cflag;
		       break;
		    case 'd':
		       ++debug;
		       break;
		    case 'f':
		       ++fflag;
		       break;
		    case 'n':
		       ++nflag;
		       break;
		    case 'v':
		       ++vflag;
		       break;
		    default:
		       usage("Unknown flag");
		    }
		 }
		 argv[i] = 0;
		 --nfile;
      } else if (!gotpattern) 
	  {
		 pbuf = regcomp(p);
		 argv[i] = 0;
		 ++gotpattern;
		 --nfile;
      }
   }
   if (!gotpattern)
      usage("No pattern");
   if (nfile == 0)
      grep(stdin, 0);
   else 
   {
   	  fflag = ((nfile > 0) ? (fflag ^ 1) : fflag);
      for (i=1; i < argc; ++i) 
	  {
		 if (0 != (p = argv[i])) 
		 {
		    if ((f=fopen(p, "r")) == NULL)
		       cant(p);
		    else 
			{
		       grep(f, p);
		       fclose(f);
		    }
		 }
      }
   }
return(0);
}

/*******************************************************/

void file(s)
char *s;
{
   printf("File %s:\n", s);
}

/*******************************************************/

void cant(s)
char *s;
{
   fprintf(stderr, "%s: cannot open\n", s);
}


/*******************************************************/

void help(hp)
char **hp;  /* dns added extra '*'  */
/*
 * Give good help
 */
{
   register char   **dp;

   for (dp = hp; *dp; dp++)
      printf("%s\n", *dp);
}


/*******************************************************/

void usage(s)
char	*s;
{
   fprintf(stderr,
      "%s\nUsage: grep [-cfhnv] pattern [file ...].  grep -h for help\n",s);
   exit(1);
}



/*******************************************************/

void grep(fp, fn)
FILE	   *fp;       /* File to process	    */
char	   *fn;       /* File name (for -f option)  */
/*
 * Scan the file for the pattern in pbuf[]
 */
{
   register int lno, count, m;

   lno = 0;
   count = 0;
   while (fgets(lbuf, LMAX, fp)) 
   {
		++lno;
		m = match();
		if ((m && !vflag) || (!m && vflag)) 
		{
			++count;
			if (!cflag) 
			{
				if (fflag && fn) 
				{
					file(fn);
					fn = 0;
				}
				if (nflag)
					printf("%d\t", lno);
					printf("%s", lbuf);
			}
		}
	}
	if (cflag) 
	{
		if (fflag && fn)
			file(fn);
		printf("%d\n", count);
	}
}


/*******************************************************/

int match()
/*
 * Match the current line (in lbuf[]), return 1 if it does.
 */
{
   register char   *l;	      /* Line pointer	    */

   for (l = lbuf; *l; l++) 
   {
      if (regexec(pbuf,l))
		 return(1);
   }
   return(0);
}


/*******************************************************/
void error(s)
char *s;
{
   fprintf(stderr, "%s", s);
   exit(1);
}
