/*
**	name:		regerror.c
**	purpose:	Look at the code
*/
#include <stdio.h>
#include "regexp.h"

void
regerror(s)
char *s;
{
#ifdef ERRAVAIL
	error("regexp: %s", s);
#else
	fprintf(stderr, "regexp(3): %s", s);
	exit(1);
#endif
	/* NOTREACHED */
}
