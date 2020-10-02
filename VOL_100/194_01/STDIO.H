/* [STDIO.H of JUGPDS Vol.17]
*****************************************************************
*								*
*	Written by  Hakuo Katayose (JUG-CP/M No.179)		*
*		    49-114 Kawauchi-Sanjuunin-machi		*
*		    Sendai, Miyagi 980                          *
*		    Phone: 0222-61-3219				*
*								*
*	Edited & tested by Y. Monma (JUG-C/M Disk Editor)       * 
*								*
*****************************************************************
*/

/* stdio.h - Standard I/O Header for Software Tools */

#include <bdscio.h>

/*
 *	Definitions for CP/M 
 */


#define STDIN  0
#define STDOUT 1
#define STDLST 2
#define DEVRDR 3
#define DEVPUN 3
#define STDERR 4

#define LETTER 'a'
#define DIGIT  '0'

#define YES    1
#define NO     0

#define EOS		'\0'
#define CPMBREAK	0x003
#define BELL		0x007
#define BACKSPACE	0x008
#define TAB		'\t'
#define CRETURN		'\r'
#define VT		0x00b
#define FORMFEED	0x00c
#define NEWLINE		'\n'
#define ESC		0x01b
#define SPACE		' '
#define BLANK		' '


/*
 *	macros
 */

/*
#define abs(x)		((x) < 0 ? -(x) : (x))
#define isalpha(c)	(islower(c) || isupper(c))
#define isdigit(c)	('0' <= (c) && (c) <= '9')
#define islower(c)	('a' <= (c) && (c) <= 'z')
#define isupper(c)	('A' <= (c) && (c) <= 'Z')
#define iswhite(c)	((c) <= ' ' || 0177 <= (c)) /* ASCII ONLY */
#define isspace(c)	((c) == ' ' || (c) == '\t' || (c) == '\n')
#define tolower(c)	(isupper(c) ? ((c) + ('a' - 'A')) : (c))
#define toupper(c)	(islower(c) ? ((c) - ('a' - 'A')) : (c))
*/

#define max(x, y)	(((x) < (y)) ? (y) : (x))
#define min(x, y)	(((x) < (y)) ? (x) : (y))

#define remark(str)	(fprintf(STDERR,"%s\n",(str)))
