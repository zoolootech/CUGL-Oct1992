/* assubr.c */

/*
 * (C) Copyright 1989,1990
 * All Rights Reserved
 *
 * Alan R. Baldwin
 * 721 Berkeley St.
 * Kent, Ohio  44240
 */

#include <stdio.h>
#include <setjmp.h>
#include <string.h>
#include <alloc.h>
#include "asm.h"

/*
 * Note an error.
 * If the error is already in the
 * error buffer don't note it again.
 * If the error is serious enough to
 * stop the parse (a `q' error) just
 * give up.
 */
VOID
err(c)
register c;
{
	register char *p;

	p = eb;
	while (p < ep)
		if (*p++ == c)
			return;
	if (p < &eb[NERR]) {
		*p++ = c;
		ep = p;
	}
	if (c == 'q')
		longjmp(jump_env, -1);
}

/*
 * Send errors to the standard output.
 */
VOID
diag()
{
	register char *p;

	if (eb != ep) {
		p = eb;
		while (p < ep) {
			fprintf(stderr, "%c", *p++);
		}
		fprintf(stderr, " %04d\n", line);
	}
}

/*
 * Note an 'r' error.
 */
VOID
rerr()
{
	err('r');
}

/*
 * Note an 'a' error.
 */
VOID
aerr()
{
	err('a');
}

/*
 * Note a 'q' error.
 */
VOID
qerr()
{
	err('q');
}
