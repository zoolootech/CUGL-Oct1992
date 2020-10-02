/* lib68.c - the library of misc. functions used by as68
 *	(C) Copyright 1982 Steve Passe
 *	All Rights Reserved
 * 
 * version 1.01
 *
 *   8/30/83 ver. 1.01 modified for Aztec ver. 1.05g smp
 *   9/1/83  split off from as68.c, the main module
 *
 */
 
/* begincode */
 
/* includes */
 
#include <stdio.h>
#include <ctype.h>
#include "as68.h"
 
/* copy the first word from string into word, return length */
 
word_copy(word,	maxl, str)
register char *word;
int maxl;
register char *str;
{
    register int length = 0;
    while (isspace(*str))   ++str;				/* skip ws */
    if (!*str) return NULL;					/* eos */
    while ((!isspace(*word = *str)) && *str && maxl--) {	/* copy word */
	++str;							/* bump... */
	++word;							/* ...ptrs */
	++length;						/* count it */
    }
    *word = '\0';
    return length;
}
 
/* compare two strings,	first converting uppercase alphas to lower in first */
 
a1strcmp(s, t)
register char *s;
register char *t;
{
    register char c;
    for ( ; (c = isupper(*s) ? *s + 32 : *s) == *t; ++s, ++t)
	if (c == '\0') return NULL;
    return (c - *t);
}

/* replace with memcpy if you're sure it works for overlapping buffers
 * (this is ONLY used on overlapping buffers in AS68 - destination has
 * higher address than source.)
 */ 
movmem(a, b, c)
register char *a;
register char *b;
register int c;
{
    if (a < b) {		    	/* moving toward high memory */
	a += c;   		/* calculate tail of source block */
	b += c;   		/* calculate tail of dest block */
	for ( ; c--; *--b = *--a)
	    ;
    }
    else if (b < a) {   	    	/* moving toward low memory */
	for ( ; c--; *b++ = *a++)
	    ;
    }
    return;
}
 
/* endcode */


