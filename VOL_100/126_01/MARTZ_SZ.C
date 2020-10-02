/*********************************************************************\
** .---------------------------------------------------------------. **
** |                                                               | **
** |                                                               | **
** |         Copyright (c) 1981, 1982, 1983 by Eric Martz.         | **
** |                                                               | **
** |                                                               | **
** |       Permission is hereby granted to use this source         | **
** |       code only for non-profit purposes. Publication of       | **
** |       all or any part of this source code, as well as         | **
** |       use for business purposes is forbidden without          | **
** |       written permission of the author and copyright          | **
** |       holder:                                                 | **
** |                                                               | **
** |                          Eric Martz                           | **
** |                         POWER  TOOLS                          | **
** |                    48 Hunter's Hill Circle                    | **
** |                      Amherst MA 01002 USA                     | **
** |                                                               | **
** |                                                               | **
** `---------------------------------------------------------------' **
\*********************************************************************/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* MARTZLIB.C IS A LIBRARY OF FUNCTIONS OF GENERAL USE.

	MARTZ-SZ.C CONTAINS:

		setbaud(baud)
		substitute(before, oldstring, newstring)
		todelim(left,buf,delim)
		trimwhite(side, first, tnull) DOES NOT WORK, USE CHOPWHITE
		upper(s)
		ynq(s)
*/
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
setbaud(baud)

/* SETS BAUD FOR SERIAL PORT ON CALIFORNIA COMPUTER SYSTEMS MODEL
2810 Z80 S-100 CPU BOARD */

	int baud;
	{
	int hi, lo;
	hi = 0;
	/* LOOK UP DIVISOR LATCH VALUES */
	switch (baud){
		case 110:
			hi = 4;
			lo = 23; /* 1047 */
			break;
		case 300:
			hi = 1;
			lo = 128; /* 384 */
			break;
		case 1200:
			lo = 96;
			break;
		case 2400:
			lo = 48;
			break;
		case 4800:
			lo = 24;
			break;
		case 9600:
			lo = 12;
			break;
		case 19200:
			lo = 6;
			break;
		default:
			return(1);
	}
	outp(35,131);	/* SET DIVISOR LATCH ACCESS BIT */
	outp(32,lo);	/* DIVISOR LATCH, LEAST SIGN. BYTE */
	outp(33,hi);	/* DIVISOR LATCH, MOST SIGN. BYTE */
	outp(35,3);	/* TURN OFF DIVISOR LATCH ACCESS */
	fprintf(STDERR,"CCS PORT baud = %d\n",baud);
	return(0);
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
substitute(before,oldstring,newstring)

/* Copyright (c) 1983 by Eric Martz, Amherst MA */

/* REPLACES ALL OCCURRENCES OF oldstring IN THE BUFFER before WITH newstring */
/* newstring MAY INCLUDE OR BE IDENTICAL WITH oldstring */

	char *before, *oldstring, *newstring;
	{
	char after[1024], left[MAXLINE];
	int done;
	done = 0;
	after[0] = NULL;
	while ((todelim(left,before,oldstring)) != -1) {
		strcat(after,left);
		strcat(after,newstring);
	}
	strcat(after,before);
	strcpy(before,after);
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
todelim(x,s,d)

/* Copyright (c) 1983 by Eric Martz, Amherst MA */

/* This function breaks string "s" into two pieces at the first occurrence
of the delimiter "d". The left piece (up to but not including "d") is put in
"x". The right piece is put into "s" (beginning after the end of "d").

The length of "x" is returned. If "d" is not found, -1 is returned.

Example:

	n = todelim(x,"crocodiles imply water"," imply ");

	Upon return,
		n = 10
		x contains "crocodiles"
		s contains "water" */

char *x, *s, *d;
{
        int i, r, gone;
        *x = '\0';
        if ((i=jindex(s,d)) == -1) return(-1);
        r = jsubstr(x,s,0,i);
        gone = r + strlen(d);
        i=0;
        do {
                s[i] = s[i+gone];
                i++;
        } 
        while (s[i-1] != '\0');
        return(r);
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*trimwhite(side, first, tnull)

/* Copyright (c) 1983 by Eric Martz, Amherst MA */

/* WARNING:
DOES NOT WORK WITH BDS C -- UNTESTED WITH REAL C
SEE chopwhite() */

/* TRIMS WHITESPACE (SPACES, TABS, NEWLINES) OFF SIDE ('r' RIGHT,
'l' LEFT, 'b' BOTH) BY MOVING POINTERS first AND tnull. IF THE
CALLING VALUE OF *tnull IS 0, IT WILL BE SET TO POINT TO THE
TERMINAL NULL USING STRLEN(). */

	char side, **first, **tnull;
	{
	char **extra;
	*extra = 0;
	if (side EQ 'r' OR side EQ 'b') {
		if (tnull EQ 0) tnull = extra; /* for trimwhite(BOTH,buf,0)! */
		if (*tnull EQ 0)
			*tnull = *first + strlen(*first);
		while (*tnull > *first AND isspace(*((*tnull)-1))) {
			(*tnull)--;
		}
		**tnull = NULL;
	}
	if (side EQ 'l' OR side EQ 'b')
		while (**first AND isspace(**first)) {
			(*first)++;
		}
}*/
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
upper(s)
	char *s;
	{
	for (; *s; s++) *s = toupper(*s);
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
ynq(s)

/* Copyright (c) 1983 by Eric Martz, Amherst MA */

/* Yes/no question. The string "s" is printed suffixed with "? (y/n)?  ". The
next line of input is examined for the first non-blank character. If it is
"y", ynq returns 1; if it is "n", ynq returns 0; if neither, the question is
reiterated. */

	char *s;
	{
	char c;
	c = NULL;
	while (c != 'n' && c != 'y') {
		printf("%s? (y/n)?  ",s);
		leftc1(&c);
	}
		if (c=='n') return (0);
		else return(1);
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*	END OF MARTZ-SZ.C	*/
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

		}
		**tnull = NULL;
	}
	if (side EQ 'l' OR side EQ 'b')
		while (**first AND isspace(**fir