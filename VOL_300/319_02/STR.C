/*
	CPP V5 -- string handling routines.

	source: str.c
	started: October 6, 1985
	version: May 26, 1988; July 21, 1988

	Written by Edward K. Ream.
	This software is in the public domain.

	See the read.me file for disclaimer and other information.
*/

#include "cpp.h"

/*
	Allocate memory big enough to hold the string,
	then copy the string to the allocated memory.
*/
char *
str_alloc(s)
register char *s;
{
	register char * p;
	register int n;

	TRACEPB("str_alloc", printf("(%s)\n", pr_str(s)));

	n = str_len(s) + 1;
	p = m_alloc(n);
	str_cpy(p, s);

	RETURN_PTR("str_alloc", p);
}

/*
	Concatenate s2 to the end of s1.
*/

void
str_cat(s1, s2)
register char *s1;
register char *s2;
{

#ifdef SHERLOCK
	char *old_s1=s1;
	char *old_s2=s2;
#endif

	TRACEP("str_cat", printf("(%p, %s)", s1, s2));

	while(*s1++) {
		;
	}
	s1--;
	while(*s2) {
		*s1++ = *s2++;
	}
	*s1 = '\0';

	TRACE("str_cpy", printf(" strlen(s1=old_s1+s2): %d=%d+%d\n",
	    str_len(old_s1), strlen(old_s1)-strlen(old_s2), strlen(old_s2)));
}

/*
	Allocate global memory for two strings
	This is NOT the same as strcat()!!
*/
char *
str_mcat(s1, s2)
register char *s1;
register char *s2;
{
	register char * p;
	register int n1, n2;

	TRACEP("str_mcat", printf("(%s, %s)\n", s1, s2));

	n1 = str_len(s1);
	n2 = str_len(s2);
	p = m_alloc(n1 + n2 + 1);
	str_cpy(p, s1);
	str_cpy(p + n1, s2);

	TRACEPN("str_mcat", printf("returns <%s>\n", p));
	return p;
}

/*
	Compare s1 and s2.
	Return <0  ==0  >0
*/

int
str_cmp(s1, s2)
register char *s1;
register char *s2;
{
	TRACEP("str_cmp", printf("(%s, %s)\n", s1, s2));

	while (*s1 == *s2) {
		if (*s1 == '\0') {
			return 0;
		}
		else {
			s1++;
			s2++;
		}
	}
	return ((int)*s1) - ((int)*s2);
}

/*
	Copy s2 to s1.
	s1 must be large enough to hold s2.
*/
void
str_cpy(s1, s2)
register char *s1;
register char *s2;
{

#ifdef SHERLOCK
	char * old_s1=s1;
#endif

	TRACEP("str_cpy", printf("(%p, %s)", s1, s2));

	while (*s2) {
		*s1++ = *s2++;
	}
	*s1 = '\0';

	TRACE("str_cpy", printf(" strlen(s1): %d\n", strlen(old_s1)));
}

/*
	Return TRUE if s1 == s2
*/
bool
str_eq(s1, s2)
register char *s1;
register char *s2;
{
	TRACEP("str_eq", printf("(%s, %s)\n", s1, s2));

	while(*s1) {
		if (*s1++ != *s2++) {
			TRACEP("str_eq", printf("returns FALSE\n"));
			return FALSE;
		}
	}
	TRACEPN("str_eq",
		printf("returns %s\n", (!*s2) ? "TRUE" : "FALSE"));
	return !*s2;
}

/*
	Return the length of a string.
*/
int
str_len(s)
register char *s;
{
	register int len;

	TRACEP("str_len", printf("(%s)\n", s));

	len=0;
	while (*s++) {
		len++;
	}

	TRACEPN("str_len", printf("returns %d\n", len));
	return len;
}

/*
	Convert a string to lower case.
*/
void
str_lower(s)
register char *s;
{
	while (*s) {
		*s = tolower(*s);
		s++;
	}
}

/*
	Convert a string to upper case.
*/
void
str_upper(s)
register char *s;
{
	while (*s) {
		*s = toupper(*s);
		s++;
	}
}
