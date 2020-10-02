/* 
HEADER: 	CUG
TITLE:		TR.C - Translate Characters
VERSION:	1.00
DATE:		05/03/85
DESCRIPTION:	"A full implementation of the UNIX 'tr' utility.
		It copies input from 'stdin' to 'stdout' with
		substitution or deletion of selected characters
		that are specified in the command line that
		invokes the program."
KEYWORDS:	tr, filter, UNIX, translate
SYSTEM:		Any
FILENAME:	TR.C
WARNINGS:	"Any operating system that converts all command
		line lowercase characters to uppercase makes this
		utility more of a curiosity than a useful tool.
		Lowercase characters can only be specified by
		their octal equivalents."
CRC:		xxxx
SEE-ALSO:	None
AUTHORS:	Ian Ashdown - byHeart Software
COMPILERS:	Any C compiler
REFERENCES:	AUTHORS: Bell Telephone Laboratories;
		TITLE:	 UNIX Programmer's Manual Vol. 1, p. 166;
ENDREF
*/

/*-------------------------------------------------------------*/

/* TR.C  - Translate Characters 
 *
 * Version 1.00		May 5th, 1985
 *
 * Copyright 1985:	Ian Ashdown
 *			byHeart Software
 *			1089 West 21st Street
 *			North Vancouver, B.C. V7P 2C2
 *			Canada
 *
 * This program may be copied for personal, non-commercial use
 * only, provided that the above copyright notice is included in
 * all copies of the source code. Copying for any other use
 * without previously obtaining the written permission of the
 * author is prohibited.
 *
 * pHILANTHROPICAL nOTES:
 *
 * Considerable time and effort went into the development of this
 * software, which was expressly written for the public domain.
 * The author will gladly accept any and all monetary
 * contributions for the purpose of continuing such work!
 *
 * USAGE: tr [-cds] [string_1 [string_2] ]
 *
 * Notes:
 *
 * This is a full implementation of the UNIX "tr" utility. It
 * copies input from "stdin" to "stdout" with substitution or
 * deletion of selected characters. Input characters of
 * "string_1" are mapped into the corresponding characters of
 * "string_2". When "string_2" is shorter than "string_1", the
 * last character of "string_2" is repeatedly mapped into the
 * remaining characters of "string_1".
 *
 * Any combination of the options "-cds" may be used. These
 * options are: 
 *
 *	-c	COMPLEMENT option. All input characters except
 *		those of "string_1" are translated to the first
 *		character of "string_2". Any remaining characters
 *		of "string_2" are ignored.
 *
 *	-d	DELETE option. All input characters matching
 *		those in "string_1" are deleted from the output
 *		to "stdout". "String_2" is not used. 
 *
 *	-s	SQUEEZE option. All sequences of repeated
 *		characters in the input from "stdin" that match
 *		characters in "string_1" are each converted to a
 *		single character (their "string_2" translations).
 *
 * In either string, the notation "x-y" means the range of
 * characters from 'x' to 'y' in increasing ASCII order. 
 *
 * The following "\x"-style escape sequences are supported: 
 *
 * 	\n	newline			(non-UNIX)
 *	\t	horizontal tab		(non-UNIX)
 *	\b	backspace		(non-UNIX)
 *	\r	carriage return		(non-UNIX)
 *	\f	form feed		(non-UNIX)
 *	\ddd	ddd (where 'ddd' is one to three
 *		octal digits)
 *	\c	c (where 'c' is anything else)
 *
 * BUGS:
 *
 * The ASCII NUL character is always deleted from the output - it
 * cannot be specified in either string.
 *
 * Some operating system command processors will not pass
 * lowercase characters and certain other reserved characters to
 * a program in command-line arguments (e.g. - CP/M). All that
 * can be done is to either specify these characters by their
 * octal equivalents (e.g. - 'a' is \141, 'z' is \172), or modify
 * this program such that it specifically asks for the two
 * strings after the program has been invoked (which makes the
 * utility incompatible with the UNIX version of "tr").
 */

/*** Definitions ***/

#define TRUE	 1
#define FALSE	 0

#define OPT_ERR  0	/* Error codes */
#define CMD_ERR  1
#define BSL_ERR  2
#define DSH_ERR  3

/*** Typedefs ***/

typedef int BOOL;	/* Boolean flag */

/*** Include Files ***/

#include <stdio.h>
#include <ctype.h>

/*** Main Body Of Program ***/

main(argc,argv)
int argc;
char **argv;
{
  static char translate[128];	/* Character translation array */
  char ch_1,		/* String_1 character */
       ch_2,		/* String_2 character */
       low_1 = NULL,	/* String_1 low range character */
       high_1,		/* String_1 high range character */
       low_2 = NULL,	/* String_2 low range character */
       high_2,		/* String_2 low range character */
       *str_1,		/* String_1 pointer */
       *str_2,		/* String_2 pointer */
       *opt_ptr,	/* Command line option pointer */
       exp_str();
  int i,		/* Temporary variable */
      trans,		/* Translation character */
      curr,		/* Current input character */
      prev = NULL;	/* Previous input character */
  BOOL cflag = FALSE,	/* Complement option flag */
       dflag = FALSE,	/* Delete option flag */
       sflag = FALSE,	/* Squeeze option flag */
       r_1 = FALSE,	/* String_1 range flag */
       r_2 = FALSE;	/* String_2 range flag */
  void error();

  /* Parse the command line for user-selected options */

  while(--argc && (*++argv)[0] == '-')
    for(opt_ptr = argv[0]+1; *opt_ptr != '\0'; opt_ptr++)
      switch(toupper(*opt_ptr))
      {
	case 'C':	/* Complement flag */
	  cflag = TRUE;
	  break;
	case 'D':	/* Delete flag */
	  dflag = TRUE;
	  break;
	case 'S':	/* Squeeze flag */
	  sflag = TRUE;
	  break;
	default:	/* Illegal command line option */
	  error(OPT_ERR,NULL);
       }

  /* Check for valid command line */

  if(!((dflag == TRUE && argc) || (argc > 1)))
    error(CMD_ERR);

  /* Expand the source and translation strings */

  str_1 = argv;
  str_2 = ++argv;

  if(cflag == FALSE)	/* Complement option not selected */
  {
    while(ch_1 = exp_str(str_1,&low_1,&high_1,&r_1))
    {
      if(curr = exp_str(str_2,&low_2,&high_2,&r_2))
	ch_2 = curr;
      translate[ch_1] = ch_2;
    }
    for(i = 1; i < 128; i++)
      if(translate[i] == NULL)
	translate[i] = i;
  }
  else	/* Complement option selected */
  {
    while(ch_1 = exp_str(str_1,&low_1,&high_1,&r_1))
      translate[ch_1] = ch_1;
    ch_2 = exp_str(str_2,&low_2,&high_2,&r_2);
    for(i = 1; i < 128; i++)
      if(translate[i] == NULL)
	translate[i] = ch_2;
  }

  /* Process the input */

  while((curr = getchar()) != EOF)
  {
    trans = translate[curr];
    if(dflag == FALSE)		/* Delete option not selected */
    {
      if(sflag == FALSE)	/* Squeeze option not selected */
	putchar(trans);
      else			/* Squeeze option selected */
      {
	if(curr == trans)
	  putchar(trans);
	else
	  if(trans != prev)
	    putchar(trans);
	prev = trans;
      }
    }
    else			/* Delete option selected */
      if(curr == trans)
	putchar(trans);
  }
}

/*** Functions ***/

/* EXP_STR() - Expand a character string. The arguments passed
 *	       are a pointer to a pointer to a character string
 *	       ("str"), a pointer to the low value of a character
 *	       range ("low"), a pointer to the high value of the
 *	       same range ("high"), and a pointer to a boolean
 *	       flag ("range") that indicated whether or not the
 *	       range is currently being expanded. The current
 *	       character of "str" or of a range implicit in "str"
 *	       currently being expanded is returned. "exp_str"
 *	       uses pointer to variables external to the function
 *	       rather than internal static variables so that the
 *	       calling function can use more than one set of
 *	       variables at a time. 
 */

char exp_str(str,low,high,range)
char **str,
     *low,
     *high;
BOOL *range;
{
  char curr;

  if(*range == FALSE)	/* Not expanding character range */
  {
    switch(curr = *(*str)++)
    {
      case NULL:	/* End of string - back up pointer */
	(*str)--;
	return NULL;
      case '\\':	/* Must be '\x'-style escape sequence */
	curr = literal_sw(str);
	break;
      case '-':		/* Must be character range */
	if(*low == NULL)
	  break;
	*high = *(*str)++;
	if(*high == '-' || *high == NULL)
	  error(DSH_ERR);
	if(*high == '\\')
	  *high = literal_sw(str);
	*range = TRUE;
	break;
      default:
	break;
    }
    if(*range == FALSE)  /* Not expanding character range */
    {
      *low = curr;
      return curr;
    }
  }
  curr = ++(*low);	/* Expanding character range */
  if(curr == *high)
    *range = FALSE;
  return curr;
}

/* LITERAL_SW() - Convert characters following '\' operator to
 *		  their equivalents. The following escape
 *		  sequences are supported:
 *
 *			\n	newline
 *			\t	horizontal tab
 *			\b	backspace
 *			\r	carriage return
 *			\f	form feed
 *			\ddd	ddd (where 'ddd' is one to three
 *				octal digits)
 *			\c	c (where 'c' is anything else)
 *
 *		  The equivalent character is returned. If a NULL
 *		  is passed as the argument, an error message is
 *		  generated.
 */

int literal_sw(buff_ptr)
char **buff_ptr;
{
  char c;	/* Current input character */
  int num_1,	/* Octal constant calculation variables */
      num_2;

  switch(c = *(*buff_ptr)++)
  {
    case 'n':		/* Convert to newline */
      return '\n';
    case 't':		/* Convert to horizontal tab */
      return '\t';
    case 'b':		/* Convert to backspace */
      return '\b';
    case 'r':		/* Convert to carriage return */
      return '\r';
    case 'f':		/* Convert to form feed */
      return '\f';
    case '0':		/* Convert to octal constant */
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
      num_1 = c - '0';
      if((c = **buff_ptr) >= '0' && c <= '7')
      {
	(*buff_ptr)++;
	num_1 = (num_1<<3) + c - '0';
	if((c = **buff_ptr) >= '0' && c <= '7')
	  if((num_2 = (num_1<<3) + c - '0') <= 0177)
	  {
	    (*buff_ptr)++;
	    return num_2;
	  }
      }
      return num_1;
    case '\0':
      error(BSL_ERR);
    default:		/* Must be a literal character */
      return c;
  }
}

/* ERROR() - Error reporting procedure */

void error(n)
int n;
{
  fprintf(stderr,"\007\nTR: Error - ");
  switch(n)
  {
    case OPT_ERR:
      fputs("Illegal command line option.",stderr);
      break;
    case CMD_ERR:
      fputs("Illegal command line.",stderr);
      break;
    case BSL_ERR:
      fputs("Illegal use of '\\' operator.",stderr);
      break;
    case DSH_ERR:
      fputs("Illegal use of '-' operator.",stderr);
      break;
    default:
      break;
  }
  fputs("\n\nUsage: tr [-cds] [string_1 [string_2] ]\n",stderr);
  exit(0);
}

/* End of TR.C */
':
      num_1 = c - '0';
    