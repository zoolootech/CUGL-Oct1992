/*
	RED general utilities -- Full C version

	Source:  redutl.c
	Version: September 21, 1983; January 18, 1990.

	Written by
	
		Edward K. Ream
		166 N. Prospect
		Madison WI 53705
		(608) 257-0802


	PUBLIC DOMAIN SOFTWARE

	This software is in the public domain.

	See red.h for a disclaimer of warranties and other information.
*/

#include "red.h"

/*
	Return TRUE if the first token in args is a number.
	Return the value of the number in  *val.
*/
int
number(char *args, int  *val)
{
	TRACEPB("number", sl_lpout();
		sl_sout(args); sl_csout();
		sl_pout(val);  sl_rpout());

	if (!isdigit(args [0])) {
		RETURN_INT("number", FALSE);
	}
	else {
		*val = atoi(args);
		RETURN_INT("number", TRUE);
	}
}

/*
	Output adecimal integer n in field width >= w.
	Left justify the number in the field.
*/
void
putdec(int n, int w)
{
	char chars[10];
	int i,nd;

	TRACEPB("putdec", sl_lpout();
		sl_iout(n); sl_csout();
		sl_iout(w); sl_rpout());

	nd = itoc(n,chars,10);
	i  = 0;
	while (i < nd) {
		outchar(chars[i++]);
	}
	i = nd;
	while (i++ < w) {
		outchar(' ');
	}

	TICKX("putdec");
}

/*
	Convert integer n to character string in str.
*/
int
itoc(int n, char *str, int size)
{
	int absval;
	int len;
	int i,j,k;

	TRACEPB("itoc",  sl_lpout();
		sl_iout(n);    sl_csout();
		sl_sout(str);  sl_csout(); 
		sl_iout(size); sl_rpout());

	absval = abs(n);

	/* Generate digits. */
	str[0] = 0;
	i = 1;
	while (i < size) {
		str[i++] = (absval % 10)+'0';
		absval   = absval / 10;
		if (absval == 0) {
			break;
		}
	}

	/* Generate sign. */
	if (i < size && n < 0) {
		str[i++] = '-';
	}
	len = i-1;

	/* Reverse sign, digits. */
	i--;
	j = 0;
	while (j < i) {
		k      = str[i];
		str[i] = str[j];
		str[j] = k;
		i--;
		j++;
	}

	RETURN_INT("itoc", len);
}

/*
	User error routine.
*/
void
error(char *message)
{
	int x, y;

	TRACEPB("error", sl_lpout(); sl_sout(message); sl_rpout());

	/* Save cursor. */
	x = outx;
	y = outy;

	pmtmess("Error: ",message);

	/* Wait for any key. */
	syscin();

	/* Restore cursor. */
	outxy(x, y);

	TICKX("error");
}

/*
	User warning routine.
*/
void
warning(char *message)
{
	int x, y;

	TRACEPB("warning", sl_lpout(); sl_sout(message); sl_rpout());

	/* Save cursor. */
	x = outx;
	y = outy;

	pmtmess("Warning: ",message);

	/* Wait for any key. */
	syscin();

	/* Restore cursor. */
	outxy(x, y);

	TICKX("warning");
}
