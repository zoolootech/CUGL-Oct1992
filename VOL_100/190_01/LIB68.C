/******************************************************************
	lib68.c, the library of misc. functions used by as68
*/

/*		(C) Copyright 1982 Steve Passe		*/
/*		All Rights Reserved					*/

/* version 1.01

	8/30/83	ver. 1.01 modified for Aztec ver. 1.05g	smp
	9/1/83	split off from as68.c, the main module

*/

/* begincode */

/* includes */

#define AZTECZII 1

#ifndef AZTECZII
#include <stdio.h>
#else
#include "stdio.h"								/* with aztecII compiler */
#endif
#include "b:as68.h"

/* copy the first word from string into word, return length */

word_copy(word, maxl, str)
char *word;
int maxl;
char *str;
{
	register int length = 0;
	while (isspace(*str))	++str;								/* skip ws */
	if (!*str) return NULL;										/* eos */
	while ((!isspace(*word = *str)) && *str && maxl--) {		/* copy word */
		++str;													/* bump... */
		++word;													/* ...ptrs */
		++length;												/* count it */
	}
	*word = '\0';
	return length;
}

/* check for length, report error to console */

too_long(str, maxl)
char *str;
int maxl;
{
	if (strlen(str) > maxl) {
		printf("\...too long (\"%s\")", str);
		return YES;
	}
	return OK;
}

/* compare two strings, first converting uppercase alphas to lower in first */

a1strcmp(s, t)
char *s;
char *t;
{
	register char c;
	for ( ; (c = ((*s >= 'A') && (*s <= 'Z')) ? *s + 32 : *s) == *t; ++s, ++t)
		if (c == '\0') return NULL;
	return (c - *t);
}

isalpha(c)
char c;
{
	return (((c >= 'A') && (c <= 'Z')) || ((c >= 'a') && (c <= 'z')));
}

/* returns pointer to char c in str, or NULL if not found */

char *
cisat(c, str)
char c;
char *str;
{
	while (*str) if (c == *str++) return --str;
	return NULL;
}

/* change all uppercase alpha to lowercase */

purge_uc(str)
char *str;
{
	for ( ; *str = tolower(*str); ++str)
		;
}

/** redo in .asm someday  --- now done, keep this for those without it:

movmem(a, b, c)
char *a;
char *b;
int c;
{
	if (a < b) {					// moving toward high memory //
		a += (c - 1);					// calculate tail of source block //
		b += (c - 1);					// calculate tail of dest block //
		for ( ; c--; *b-- = *a--)
			;
	}
	else if (b < a) {				// moving toward low memory //
		for ( ; c--; *a-- = *b--)
			;
	}
	return;
}
**/

/** now using -> fputs(str, lst_d);
lputs(str)
char *str;
{
	register int y, x = 0;

	while (str[x]) {
		if (str[x] == '\t') {
			for (y = 8 - (x % 8); y--; ) bdos(5, ' ');
			++x;
		}
		else bdos(5, str[x++]);
	}
}
**/


/***
	buffer words from second field to first field, wrapping or cutting
as necessary

buf_field(dest, src, size)
char *dest;
char **src;
int size;
{
	register int x, cnt = 1;

	for ( ;--size; ++cnt) {
		switch (**src) {
		case NULL:
		case '\n':
			*dest++ = '\n';
			*dest = NULL;
			if (!**src) return NULL;
			(*src)++;
			return TRUE;
		case '\t':
			size -= (x = 7 - (cnt % 8));	/* 7 'cause loop subs 1 //
			if (size < 1) {					/* tab overflow //
				size = 1;
				continue;
			}
			cnt += x;						/* fall thru to default //
		default:
			*dest++ = *(*src)++;
			continue;
		}
	}
	*dest++ = '\n';
	*dest = NULL;
	return TRUE;
}
***/

/* endcode */
/ moving toward low memory //
		for ( ; c--; *a-- = *b--)
			;
	}