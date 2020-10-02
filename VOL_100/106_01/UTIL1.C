/*
		Function Summary
		----------------

charcnt(bufpntr)

	returns the number of characters in the file at bufpntr.
	file must be in a continuous block of RAM.

linecnt(bufpntr)

	returns the number of lines in the file at bufpntr.
	file must be in a continuous block of RAM.

ntoi(number,base)

	converts ASCII digits to an integer using any base
	except split octal (see otoi). white space may preceed
	the ASCII string which must end with '\0' after the
	final digit.

otoi(number)

	converts ASCII digits representing a split octal
	number in the format xxx.xxx{a} to an integer. white
	space may preceed the ASCII string which must be
	7 characters long (3 chars,'.',3 chars). any following
	characters are ignored (ie. does not require '\0' as
	final delimiter).

wordcnt(bufpntr)

	returns the number of words in the file at bufpntr.
	file must be in a continuous block of RAM. note that
	'\n' is handled explicitly as CRLF to avoid false
	triggering of the word count on an empty line.
*/

#define	LF	10
#define	CR	13
#define	EOF	26
#define	NO	0
#define	YES	-1

charcnt(bufpntr)
char *bufpntr;
{
	int nc;
	char c;

	nc = 0;
	while((c = *bufpntr++) != EOF) nc += 1;
	return nc;
}

linecnt(bufpntr)
char *bufpntr;
{
	int nc;
	char c;

	nc = 0;
	while((c = *bufpntr++) != EOF) {
	   if (c == '\n') nc += 1;
	}
	return nc;
}

ntoi(n,b)
char *n;
int b;
{
	int val,sign;
	char c;

	val=0; sign=1;
	while ((c = *n) == '\t' || c == ' ') ++n;
	if (c == '-') {sign = -1; n++;}
	while (dig(c = *n++)) {
	   if (b == 16 && c >= 'A' && c <= 'F') c -= 7;
	   val = val * b + c - '0';
	}
	return sign*val;
}

otoi(n)
char *n;
{
	int val, b, i;
	char c;

	val = 0; b = 16384;

	while ((c = *n) == '\t' || c == ' ') ++n;
	for (i = 0; i < 7; i++) {
	   if ((c = *n) == '.') {++n; b = 64;}
	   else {c = *n++; val = val + (b * (c - '0')); b /= 8;}
	}
	return val;
}

wordcnt(bufpntr)
char *bufpntr;
{
	int inword, wc;
	char c;

	wc = 0; inword = NO;
	while((c = *bufpntr++) != EOF) {
	   if (c == ' ' || c == CR || c == '\t') inword = NO;
	   else if ((inword == NO) && (c != LF)) {
	      inword = YES; wc += 1;
	   }
	}
	return wc;
}

