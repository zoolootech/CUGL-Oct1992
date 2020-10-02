/*
**	name:		regexp.h
**	purpose:	Define stuff to compile under OS/2
*/
#define	OS2	1
#include	<string.h>
#include	<stdlib.h>
void	file();
void	cant();
void	help();
void	usage();
void	grep();
int	match();
void	error();



/*
 * Definitions etc. for regexp(3) routines.
 *
 * Caveat:  this is V8 regexp(3) [actually, a reimplementation thereof],
 * not the System V one.
 */
#define NSUBEXP  10
typedef struct regexp {
	char *startp[NSUBEXP];
	char *endp[NSUBEXP];
	char regstart;		/* Internal use only. */
	char reganch;		/* Internal use only. */
	char *regmust;		/* Internal use only. */
	int regmlen;		/* Internal use only. */
	char program[1];	/* Unwarranted chumminess with compiler. */
} regexp;

#ifndef LINT_ARGS
extern regexp *regcomp();
extern int regexec();
extern void regsub();
extern void regerror();
#else
extern regexp *regcomp(char *);
extern int regexec(regexp *,char *);
extern void regsub(regexp *,char *,char *);
extern void regerror(char *);
#endif

