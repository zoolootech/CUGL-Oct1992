/*
HEADER: 	CUG
TITLE:		PREP.C - Prepare Text for Statistical Processing
VERSION:	1.00
DATE:		11/03/85
DESCRIPTION:	"PREP is a full emulation of Unix's 'prep'
		utility. It reads each text file given on the
		command line in sequence and writes it on the
		standard output, one word to a line."
KEYWORDS:	UNIX
SYSTEM:		ANY
FILENAME:	PREP.C
WARNINGS:	NONE
CRC:		xxxx
SEE-ALSO:	NONE
AUTHORS:	Ian Ashdown - byHeart Software
COMPILERS:	ANY
REFERENCES:	AUTHORS: Bell Laboratories
		TITLE:	 UNIX Programmer's Manual, Volume 1
			 p. 109; Holt, Rinehart and Winston
ENDREF
*/

/*-------------------------------------------------------------*/

/* PREP.C - Prepare Text for Statistical Processing
 *
 * Version 1.00		November 3rd, 1985
 *
 * Copyright 1985:	Ian Ashdown
 *			byHeart Software
 *			1089 West 21st Street
 *			North Vancouver, B.C. V7P 2C6
 *			Canada
 *
 * This program may be copied for personal, non-commercial use
 * only, provided that the above copyright notice is included in
 * all copies of the source code. Copying for any other use
 * without previously obtaining the written permission of the
 * author is prohibited.
 *
 * Synopsis:	PREP [-diop] [file] ...
 *
 * Description:	PREP reads each file given by the command line
 *		in sequence (or standard input if no files are
 *		specified) and writes it on the standard output,
 *		one word at a time. (A "word" is defined as a
 *		string of alphabetic characters and imbedded or
 *		(one singular) trailing apostrophes, delimited by
 *		space or punctuation.) Hyphenated words are
 *		broken apart unless the hyphens occur at the end
 *		of a line; then the hyphen is removed and the two
 *		parts of the word concatenated. All other
 *		characters are normally discarded.
 *
 *		The following options may appear in any order on
 *		the command line. The -i and -o options must
 *		immediately precede their associated filenames.
 *
 *		-d	Precede each word on the standard output
 *			with its number. Words are numbered
 *			sequentially as they are found in the
 *			files.
 *		-i	Ignore the next file by not sending the
 *			words found to the standard output. (The
 *			word count displayed by the -d option,
 *			however, will nevertheless be incremented
 *			as each word is found.)
 *		-o	Only the words found in the next file
 *			will appear on the standard output. All
 *			words found in other files will be
 *			counted for the -d option.
 *		-p	Include punctuation characters (any
 *			nonalphanumeric printable character) as
 *			separate output lines. The punctuation
 *			characters are not counted for the -d
 *			option.
 *
 * Bugs:	A "word" is defined as consisting of a maximum of
 *		alphabetic 256 characters and imbedded or (one
 *		singular) trailing apostrophes.
 *
 * (Note: the above description is a reworded and expanded
 *	  version of that appearing in the "UNIX Programmer's
 *	  Manual", copyright 1979, 1983 Bell Laboratories.)
 */

/*** Definitions ***/

#define FALSE	     0
#define TRUE	     1

#define FIL_ERR      0	/* Error codes */
#define ILL_OPT	     1
#define REP_OPT      2

#define MAXWORD    256	/* Maximum size of "word" */

/*** Typedefs ***/

typedef int BOOL;	/* Boolean flag */

/*** Global Variables ***/

long word_cnt = 0L;	/* Word count */
BOOL dflag = FALSE,	/* Command line option flags */
     fflag = FALSE,
     iflag = FALSE,
     oflag = FALSE,
     pflag = FALSE,
     sflag = FALSE;

/*** Include Files ***/

#include <stdio.h>
#include <ctype.h>

/*** Main Body of Program ***/

int main(argc,argv)
int argc;
char **argv;
{
  int arg_count = argc;
  char *s,
       **arg_vector = argv;
  void proc_file(),
       error();

  /* Parse the command line for user-selected global options */

  while(--arg_count)
  {
    if((*++arg_vector)[0] == '-')
      for(s = arg_vector[0] + 1; *s != '\0'; s++)
	switch(toupper(*s))
	{
	  case 'D':
	    dflag = TRUE;
	    break;
	  case 'I':	/* Ignore this option on first pass */
	    break;
	  case 'O':	/* Suppress all files but -o option */
	    if(sflag == TRUE)
	      error(REP_OPT,NULL);
	    else
	      sflag = TRUE;
	    break;
	  case 'P':
	    pflag = TRUE;
	    break;
	  default:
	    error(ILL_OPT,*arg_vector);
	}
  }

  /* Process the files */

  while(--argc)
  {
    if((*++argv)[0] == '-')
      for(s = argv[0] + 1; *s != '\0'; s++)
	switch(toupper(*s))
	{
	  case 'D':
	  case 'P':
	    break;
	  case 'I':
	    iflag = TRUE;
	    break;
	  case 'O':
	    oflag = TRUE;
	    break;
	  default:
	    break;
	}
    else
    {
      fflag = TRUE;
      proc_file(*argv);
      iflag = FALSE;
      oflag = FALSE;
    }
  }
  if(fflag == FALSE)
    proc_file(NULL);
}

/*** Functions ***/

/* PROC_FILE() */

void proc_file(fname)
char *fname;
{
  char word[MAXWORD+1];
  int c,
      getword();
  FILE *fp,
       *freopen();
  void error();

  /* Open input file (if specified) */

  if(fname)
    if((fp = freopen(fname,"r",stdin)) == NULL)
      error(FIL_ERR,fname);

  /* Output words in accordance with user-selected options */

  do
  {
    c = getword(word);
    if(*word)	/* Display the word (if any) */
    {
      word_cnt++;
      if(iflag == FALSE && (sflag == FALSE || (sflag == TRUE &&
	  oflag == TRUE)))
      {
	if(dflag == TRUE)
	  printf("%05ld: ",word_cnt);
	puts(word);
      }
    }
    if(pflag == TRUE)	/* Display any punctuation */
      if(ispunct(c))
      {
	putchar(c);
	putchar('\n');
      }
  }
  while(c != EOF);
}

/* GETWORD() - Gets next "word" from input. Leaves "word" found
 *	       (may be NULL) in array "word[]" and returns last
 *	       character obtained from standard input. Hyphens
 *	       followed by newlines are ignored, and "word" may
 *	       contain imbedded or (one singular) trailing
 *	       apostrophes. Size in characters limited to
 *	       MAXWORD.
 */

int getword(word)
char *word;
{
  int c,
      limit = MAXWORD;
  BOOL aflag = FALSE;

  while(limit)
  {
    c = getchar();
    if(c == '-')	/* Hyphen at end of line? */
    {
      c = getchar();
      if(c == '\n')
	continue;
      else
        break;
    }
    else if(c == '\'')	/* Imbedded or trailing apostrophe? */
    {
      if(aflag == TRUE || limit == MAXWORD)
	break;
      aflag = TRUE;
    }
    else if(!isalpha(c))
      break;
    *word++ = c;	/* Add character to word */
    limit--;
  }
  *word = '\0';		/* Terminate word string */
  return c;
}

/* ERROR() - Error reporting. Returns an exit status of 2 to the
 *	     parent process.
 */

void error(n,str)
int n;
char *str;
{
  fprintf(stderr,"\007\n*** ERROR - ");
  switch(n)
  {
    case FIL_ERR:
      fprintf(stderr,"Cannot open input file: %s",str);
      break;
    case ILL_OPT:
      fprintf(stderr,"Illegal command line option: %s",str);
      break;
    case REP_OPT:
      fprintf(stderr,"Repeated -o option");
      break;
    default:
      break;
  }
  fprintf(stderr," ***\n\nUsage: prep [-diop] [file] ...\n");
  exit(2);
}

/*** End of PREP.C ***/
 */
    {
      c = getchar();
    