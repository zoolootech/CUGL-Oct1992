/*
	CPP V5 -- utilities.

	Source:  utl.c
	Started: October 7, 1985
	Version:
		July 21, 1988
		March 3, 1989
			bug fix to skip_ws() and copy_ws().
		August 1, 1989
			Support for C++ style single-line comments added to
			skip_pp(), skip_ws() and copy_ws().

	Written by Edward K. Ream.
	This software is in the public domain.

	See the read.me file for disclaimer and other information.
*/

#include "cpp.h"

/*
	Return the value of a character constant.
*/
int
char_val(s)
register char *s;
{
	int val;

	if (*s != '\\') {
		return (int) *s;
	}
	s++;
	switch (*s) {
	case 'b':	return '\b';
	case 'f':	return '\f';
	case 'n':	return '\n';
	case 'r':	return '\r';
	case 't':	return '\t';
	case '\'':	return '\'';
	case '\\':	return '\\';

	default:
		if (*s < '0' || *s > '7') {
			return (int)*s;
		}
		val = 0;
		while (*s >= '0' && *s <= '7') {
			val = val * 8 + (int)*s - '0';
			s++;
		}
		return val;
	}
}

/*
	Re-evaluate a string constant; this may shorten it

	Return its length; it may contain imbedded zeroes once processed

	CAUTION: if the string is transformed, it is always shortened.  Callers
	of this routine may count on that fact.
*/
unsigned int
str_val(d)
register unsigned char *d;
{
	register unsigned char val, *dd, *s;
	TICK("str_val");

	/* outer loop to scan the string */
	s = dd = d;
	while (*s) {
		if (*s != '\\') {
			*d++ = *s++;
			continue;
		}
		++s;
		switch (*s) {
		case 'b':	*d++ = '\b'; ++s; break;
		case 'f':	*d++ = '\f'; ++s; break;
		case 'n':	*d++ = '\n'; ++s; break;
		case 'r':	*d++ = '\r'; ++s; break;
		case 't':	*d++ = '\t'; ++s; break;
		case '\'':	*d++ = '\''; ++s; break;
		case '\\':	*d++ = '\\'; ++s; break;

		default:
			if (*s < '0' || *s > '7') {
				continue;
			}
			val = 0;
			while (*s >= '0' && *s <= '7') {
				val = val * 8 + *s - '0';
				s++;
			}
			*d++ = val;
		}
	}
	*d++ = '\0';
	TRACEP("str_val", printf("returns length %d\n", (int)(d - dd)));
	return (unsigned int)(d - dd);
}

/*
	Return the value of a string of digits in a given radix.
*/
int
conv2i(string, radix)
char *string;
int radix;
{
	register int value;
	register int digit;

	TICK("conv2i");

	value = 0;
	while (digit = *string++) {
		if (digit >= 'a' && digit <= 'z') {
			digit = digit - 'a' + 10;
		}
		else if (digit >= 'A' && digit <= 'Z') {
			digit = digit - 'A' + 10;
		}
		else {
			digit = digit - '0';
		}
		value = (value * radix) + digit;
	}
	return value;
}

/*
	Convert a signed integer n to a string s[].
	The length of s must be large enough to hold the result.
*/
void
conv2s (a, s)
int a;
register char *s;
{
	register char *d, *ss;
	register int sn;
	register unsigned long n;
	char temp [INT_DIGITS];

	TICK("conv2s");

	/* Do the sign and handle 0x8000 correctly */
	if (a >= 0) {
		sn = 0;
		/* these casts ARE NOT redundant: DO NOT fix them! */
		/* see note in lint.doc about unsigned extending casts */
		n = (unsigned long) (long) a;
	}
	else {
		sn = 1;
		n = (unsigned long) (long) (-a);
	}

	/* put digits in reverse order into temp */
	d = &temp[0];
	*d++ = 0;
	if (n) while (n) {
		/* NOTE: in assembly, we would divide once */
		*d++ = (char)(n % 10) + '0';
		n = n / 10;
	}
	else {
		*d++ = '0';
	}

	/* insert the sign */
	if (sn) {
		*d++ = '-';
	}

	/* Reverse temp into s. */
	ss = s;
	while(*ss++ = *--d) {};

	TRACE("conv2s", printf("conv2s returns: %s\n", s));
}

/*
	Convert a long n to a string s[].
	The length of s must be large enough to hold the result.
*/
void
convl2s (a, s)
long a;
register char *s;
{
	register char *d, *ss;
	register int sn;
	register unsigned long n;
	char temp [LONG_DIGITS];

	TICK("convl2s");

	/* Do the sign and handle 0x80000000 correctly */
	if (a >= 0) {
		sn = 0;
		n = (unsigned long) a;
	}
	else {
		sn = 1;
		n = (unsigned long) (-a);
	}

	/* put digits in reverse order into temp */
	d = &temp[0];
	*d++ = 0;
	if (n) while (n) {
		/* NOTE: in assembly, we would divide once */
		*d++ = (char)(n % 10) + '0';
		n = n / 10;
	}
	else {
		*d++ = '0';
	}

	/* insert the sign */
	if (sn) {
		*d++ = '-';
	}

	/* Reverse temp into s. */
	ss = s;
	while(*ss++ = *--d) {};

	TRACE("convl2s", printf("convl2s (%ld): %s\n", a, s));
}

/*
	Convert a long n to a string s[], minimum digit count c.
	The length of s must be large enough to hold any result.
*/
void
conul2sc(n, s, c)
register unsigned long n;
register char *s;
register int c;
{
	register char *d, *ss;
	char temp [LONG_DIGITS];

	TICK("conul2sc");

	/* put digits in reverse order into temp */
	d = &temp[0];
	*d++ = 0;
	while (n) {
		/* NOTE: in assembly, we would divide once */
		*d++ = (char)(n % 10) + '0';
		n /= 10;
		if (c > 0) {
			c--;
		}
	}
	while (c > 0) {
		c--;
		*d++ = '0';
	}

	/* Reverse temp into s. */
	ss = s;
	while(*ss++ = *--d) {};

	TRACE("conul2sc", printf("conul2sc returns: %s\n", s));
}

/*
	Convert a long n to a hex string s[], minimum digit count c.
	The length of s must be large enough to hold any result.
*/
void
conl2h(n, s, c)
register unsigned long n;
register char *s;
register int c;
{
	register char *d, *ss;
	char temp [LONG_DIGITS];

	TICK("conl2h");

	/* put digits in reverse order into temp */
	d = &temp[0];
	*d++ = 0;
	while (n) {
		*d = ((char)n & 15) + '0';
		if (*d > '9') {
			*d += 7;
		}
		d++;
		n >>= 4;
		if (c > 0) {
			c--;
		}
	}
	while (c > 0) {
		c--;
		*d++ = '0';
	}

	/* Reverse temp into s. */
	ss = s;
	while(*ss++ = *--d) {};

	TRACE("conl2h", printf("conl2h returns: %s\n", s));
}

/*
	Skip blanks and tabs,  but not newlines.
*/
void
skip_bl()
{
	TICKB("skip_bl");

	while (ch == ' ' || ch == '\t' || ch == '\v' || ch == '\f') {
		sysnext();
	}
	
	RETURN_VOID("skip_bl");
}

/*
	Skip characters up to but NOT including a newline.
*/
void
skip_1line()
{
	TICKB("skip_1line");

	while (ch != END_FILE && ch != '\n') {
		sysnext();
	}

	RETURN_VOID("skip_1line");
}

/*
	Skip characters up to and including a newline.
*/
void
skip_past()
{
	TICKB("skip_past");

	while (ch != END_FILE && ch != '\n') {
		sysnext();
	}

	if (ch == '\n') {
		sysnext();
		if (com_flag) {
			sysnlput();
		}
		bump_line();
	}

	RETURN_VOID("skip_past");
}

/*
	Skip to the end of the current preprocessor directive.
	I.e., skip to the first newline not contained in a comment.
*/
void
skip_pp()
{
	TICKB("skip_pp");

	while (ch != '\n' && ch != END_FILE) {
		if (ch == '/') {
			/* Possible comment. */
			sysnext();
			if (ch == '*') {
				sysnext();
				skip_comment();
			}
			else if (slc_flag && ch == '/') {
				/* 8/1/89 Single-line comment */
				while (ch != END_FILE && ch != '\n') {
					sysnext();
				}
				RETURN_VOID("skip_pp");
			}
			else {
				syspushback(ch);
				ch = '/';
				RETURN_VOID("skip_pp");
			}
		}
		else {
			sysnext();
		}
	}
	RETURN_VOID("skip_pp");
}

/*
	Skip blanks, tabs, and comments.
	Also skip newlines if nl_flag is TRUE;
*/
void
skip_ws(nl_flag)
bool nl_flag;
{
	TRACEPB("skip_ws", printf("(%s)\n", pr_bool(nl_flag)));

	for(;;) {

		switch(ch) {

		case ' ':
		case '\t':
		case '\f':
		case '\v':
			sysnext();
			continue;

		case '\n':
			if (nl_flag) {
				bump_line();
				sysnext();
				continue;
			}
			else {
				/* 3/3/89 */
				goto done;
			}

		case '/':
			sysnext();
			if (ch == '*') {
				sysnext();
				skip_comment();
				continue;
			}
			else if (slc_flag && ch == '/') {
				/* 8/1/89 Single-line comment */
				while (ch != END_FILE && ch != '\n') {
					sysnext();
				}
				continue;
			}
			else {
				syspushback(ch);
				ch = '/';
				goto done;
			}

		default:
			goto done;
		}
	}
done:
	RETURN_VOID("skip_ws");
}

/*
	Copy blanks, tabs and comments to the output.
	Also copy newlines if nl_flag is TRUE;
*/
void
copy_ws(nl_flag)
bool nl_flag;
{
	TRACEPB("copy_ws", printf("(%s)\n", pr_bool(nl_flag)));

	for(;;) {

		switch(ch) {

		case ' ':
		case '\t':
		case '\f':
		case '\v':
			syscput(ch);
			sysnext();
			continue;

		case '\n':
			if (nl_flag) {
				bump_line();
				sysnlput();
				sysnext();
				continue;
			}
			else {
				/* 3/3/89 */
				goto done;
			}

		case '/':
			sysnext();
			if (ch == '*') {
				sysnext();
				syscput('/');
				syscput('*');
				copy_comment();
				continue;
			}
			else if (slc_flag && ch == '/') {
				/* 8/1/89 Single-line comment */
				sysnext();
				syscput('/');
				syscput('/');
				while (ch != END_FILE && ch != '\n') {
					syscput(ch);
					sysnext();
				}
				continue;
			}
			else {
				syspushback(ch);
				ch = '/';
				goto done;
			}

		default:
			goto done;
		}
	}
done:
	RETURN_VOID("copy_ws");
}

/*
	Process a non-fatal error messages.
*/
void
error(message)
char *message;
{
	TICK("error");
	syscsts();
	t_errcount++;
	if (t_inlevel == 0) {
		printf("line %3d: %s\n", t_line, message);
	}
	else {
		printf("line %3d, %s: %s\n", t_line, t_file, message);
	}
}

void
err2(mess1, mess2)
char *mess1;
char *mess2;
{
	TICK("err2");
	syscsts();
	t_errcount++;
	if (t_inlevel == 0) {
		printf("line %3d: %s%s\n", t_line, mess1, mess2);
	}
	else {
		printf("line %3d, %s: %s%s\n", t_line, t_file, mess1, mess2);
	}
}

void
err3(mess1, mess2, mess3)
char *mess1;
char *mess2;
char *mess3;
{
	TICK("err3");
	syscsts();
	t_errcount++;
	if (t_inlevel == 0) {
		printf("line %3d: %s%s%s\n", t_line, mess1, mess2, mess3);
	}
	else {
		printf("line %3d, %s: %s%s%s\n",
			t_line, t_file, mess1, mess2, mess3);
	}
}

/*
	Give an error message and exit.
*/
void
fatal(message)
char * message;
{
	printf("\nOh dear.  I can't go on like this...\n\n");
	printf("line %3d, %s: %s\n", t_line, t_file, message);

	TRACE("dump", m_stat());
	TRACE("dump", sl_dump());
	sysabort();
	
}

/*
	Internal error.
*/
void
syserr(message)
char *message;
{
	printf("\nOh dear.  There is something wrong with me...\n\n");
	printf("line %3d, %s: %s\n", t_line, t_file, message);

	TRACE("dump", m_stat());
	TRACE("dump", sl_dump());
	sysabort();
}

/*
	Process a non-fatal warning message.
*/
void
warning(message)
char *message;
{
	TICK("warning");
	syscsts();
	if (t_inlevel == 0) {
		printf("line %3d: (Warning) %s\n", t_line, message);
	}
	else {
		printf("line %3d, %s: (Warning) %s\n",
			t_line, t_file, message);
	}
}

void
warn2(mess1, mess2)
char *mess1;
char *mess2;
{
	TICK("warn2");
	syscsts();
	if (t_inlevel == 0) {
		printf("line %3d: (Warning) %s%s\n", t_line, mess1, mess2);
	}
	else {
		printf("line %3d, %s: (Warning) %s%s\n",
			t_line, t_file, mess1, mess2);
	}
}

void
warn3(mess1, mess2, mess3)
char *mess1;
char *mess2;
char *mess3;
{
	TICK("warn3");
	syscsts();
	if (t_inlevel == 0) {
		printf("line %3d: (Warning) %s%s%s\n",
			t_line, mess1, mess2, mess3);
	}
	else {
		printf("line %3d, %s: (Warning) %s%s%s\n",
			t_line, t_file, mess1, mess2, mess3);
	}
}
