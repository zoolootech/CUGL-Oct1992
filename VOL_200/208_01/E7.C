/*
HEADER:		CUG208;
TITLE:		'e' for CP/M68K
VERSION:	1.48+

DESCRIPTION:	"a screen editor";

KEYWORDS:	editor;
SYSTEM:		CP/M68K, V1.2;
FILENAME:	e/e7.c
WARNINGS:	"the default value is for systems with 128K bytes
		 of memory or more";
SEE-ALSO:	cpm68k.c, e68k.doc, CUG VOL 133;
AUTHORS:	G.N.Gilbert('e'), J.W.Haefner(for DeSmet C on MSDOS and UNIX)
CODER:		Yoshimasa Tsuji
COMPILERS:	DRI C(Alcyon C) for CP/M68K;
*/
/*
	FUNCTIONS: getkey,testkey,getlow,testlow,tranchar, tranlow,
			getscankey,inchar,inkey,putret,dispch,putch,
			checkkey,inword,uspr, dohelp
	PURPOSE: low level terminal i/o functions
*/

#include "e.h"

getkey()	/*wait for a key to be pressed & return it, translating
			to internal codes */
{
	return tranchar(inchar());
}

testkey()	/*if a key has been pressed, return it, else 0, translating*/
{
	return tranchar(inkey());
}


getlow()	/*get a key, converting control and upper case characters
		to lower case */
{

	return tranlow(inchar());
}

testlow()	/*if a key has been pressed, return it, converting control
			and upper case characters to lower case, else 0 */
{
	return tranlow(inkey());
}

tranchar(c)	/*translate c to internal codes */
char c;
{
	register char *tranp;
	register int i;

	if (c == 0) return 0;
	if (c == *(tranp=tran)) {
		while (!(c=inkey()));
		c|=PARBIT;
	}
	for (i=1; i < NKEYS; i++)
		if (c == *++tranp) return i;
	return c;
}

tranlow(c)		/*convert upper and control chars in c to lower case*/
{
	if (c == 0) return 0;
	if (tran[ESCKEY] == c) return ESCKEY;
	if (c <= CTRL) c=c+96;
	return tolower(c);
}

getscankey()	/*get a key, translation of ESCKEY, CR, LEFTKEY, DELLEFT,
			RETRIEVE only */
{
	register int c;

	c=inchar();
	if (tran[ESCKEY] == c) return ESCKEY;
	if (tran[CR] == c) return CR;
	if (tran[LEFTKEY] == c) return LEFTKEY;
	if (tran[DELLEFT] == c) return DELLEFT;
	if (tran[RETRIEVE] == c) return RETRIEVE;

	return c;
}

inchar()		/*wait for and return a character */
{
	register int c;

	if (c=unbuf()) return c;
	return _conin() & NOPARITY;
}

inkey()		/*return last key press,or 0 if none; don't wait for
				a character*/
{
	register int c;

	if (c=unbuf()) return c;
	if (_inkey()) return _conin() & NOPARITY;
	return 0;
}

unbuf()		/*return char from type-ahead buffer, or 0 if none*/
{
	register int c,i;

	if (inbufp == 0) return 0;

	c=inbuf[0] & NOPARITY;
	inbufp--;
	for (i=0; i < inbufp; i++) inbuf[i]=inbuf[i+1];
	return c;
}

putret()	/*type a CR/LF*/
{
	putch('\n');
}

dispch(c)	/*type a character (in different intensity if non-printable*/
{
	if (c <= CTRL) {
		makeother();
		putch(c+64);
		makeother();
	}
	else putch(c);
}

putch(c)	/*type a character; test for keyboard input*/
{
	_conout(c & NOPARITY);
	checkkey();
}

checkkey()	/*check keyboard for input.  If found, store away in input
		  buffer*/
{
	if (_inkey() && inbufp < INBUFLEN) inbuf[inbufp++]= _conin();
}

inword(c)	/*return true if c is part of a token */
{
	return  isalnum(c)|| c == '_' ;
}

uspr(n)		/*print 'n' as a number; return number of chars typed*/
{
	int temp;

	if (n < 10) {
		putch(n+'0');
		return 1;
	}
	temp=uspr(n/10);
	uspr(n%10);
	return temp+1;
}


makeother()
{
	if (isdim) standend();
	else standout();
}


dohelp()	/*display help menu */
{
	int y;

	pfirst=loc(pfirst,(HELPLINES-1));
	topline=HELPLINES;
	for (y=1; y < HELPLINES; y++) deleteline(0, y);
	if (cline < pfirst || pfirst == lastl) {
		puttext();
		putpage();
	}
	gotoxy(0,1);
	help();
}







