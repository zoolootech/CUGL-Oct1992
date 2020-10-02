/*	
 *	Program Name	: MAKE.H
 *	Author		: Allen Holub
 *	Implementor	: Kenji Hino
 *	Description	: This is a header file for MAKE.C
 *	Compiler	: Microsoft C ver.4.0, Lattice C ver. 3
 */

#define	MICRO_C
/* #define	LATTICE_C */

#include "stdio.h"
#include "stdlib.h"
#include "fcntl.h"
#include "dos.h"
#include "string.h"
#include "time.h"

#ifdef	MICRO_C
#include <sys\types.h>
#include <sys\stat.h>
#include <io.h>
#include <memory.h>
#include <malloc.h>
#endif

/* define NEVER	*/		/* for debugging */

#ifdef	NEVER
#define DEBUG		1	/* Include for debug diags in make() */
#endif

#define	MAXLINE		(80*10)	/* Maximum input line length */
#define MAXBLOCK	64 	/* Max number of lines in an action */
#define MAXDEP		32	/* Max number of dependencies */
#define	COMMENT		'#'	/* Delimits a comment */
#define MAKEFILE	"mkfile" /* Name of makefile */
#define DATETIME	0x57	/* "to get or set file's date & time */
#define	DEFTIME		0x0	/* The default time returned by gtime when a */
				/* file doesn't exist */
				
/* iswhite(c)		evaluates true if c is white space.
   skipwhite(s)		skips the character pointer s past any white space
   skipnonwhite(s)	skips s past any non-white characters.
*/

#define iswhite(c)	((c) == ' ' || (c) == '\t')
#define skipwhite(s)	while( iswhite(*s) )	++s;
#define	skipnonwhite(s)	while( *s && !iswhite(*s) ) ++s;


/* 
	The entire makefile is read into memory before it's processed. It's
	stored in a bimary tree composed of the following structures:
	depends_on and do_this are argv-like arrays of the pointers to character
	pointers. The arrays are null terminated so no count is required.
	The time field is a 32 bit long consisting of the date and time
	fields returned from a DOS 0x57 call. The date and time are
	concatenated with the date in the most siginificant 16 bits and the
	time in the least significant. This way they can be compared as
	a single number.
*/

typedef struct _tn
{
	struct _tn	*lnode;		/* pointer to left sub-tree */
	struct _tn	*rnode;		/* pointer to right sub-tree */
	char		*being_made;	/* name of file being made */
	char 		**depends_on;	/* names of dependant files */
	char		**do_this;	/* Actions to be done to make file */
	long		time;		/* time & date last modified */
} TNODE;

static	TNODE		*Root	= 0;	/* Root of file-name tree */
static	FILE		*Makefile;	/* Pointer to opened makefile */
static 	int 		Inputline = 1;	/* current input line number */
static	char		*First	= "";	/* Default file to make */

