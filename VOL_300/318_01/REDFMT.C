/*
	RED output format module -- Full C version

	Source:  redfmt.c
	Version: October 16, 1983; January 18, 1990;

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
	Declare routines local to this module.
*/
static void	fmtdevch	(char c);
static int	fmtlench	(char c,int col);
static void	fmtoutch	(char c,int col);

/*
	Declare variables local to this module.
*/
static int fmttab;	/* maximal tab length				*/
static int fmtdev;	/* device flag -- TRUE/FALSE = LIST/CONSOLE	*/
static int fmtwidth;	/* devide width.  LISTW/SCRNW1			*/

/*
	Adjust fmtcol[] to prepare for calls on fmtout() and
	fmtlen().

	NOTE:  this routine is needed as an efficiency measure.
	Without fmtadj(), calls on fmtlen() become too slow.
*/
void
fmtadj(char *buf, int minind, int maxind)
{
	int k;

	TRACEPB("fmtadj",  sl_lpout();
		sl_sout(buf);    sl_csout();
		sl_iout(minind); sl_csout(); 
		sl_iout(maxind); sl_rpout());

	/* Line always starts at left margin. */
	fmtcol[0] = 0;

	/* Start scanning at minind. */
	k = minind;
	while (k < maxind) {
		fmtcol[k+1] = fmtcol[k]+fmtlench(buf[k],fmtcol[k]);
		k++;
	}

	TICKX("fmtadj");
}

/*
	Direct output from this module to either the console or
	the list device.
*/
void
fmtassn(int listflag)
{
	TRACEPB("fmtassn", sl_lpout(); sl_iout(listflag); sl_rpout());

	if (listflag == TRUE) {
		fmtdev = TRUE;
		fmtwidth = LISTW;
	}
	else {
		fmtdev = FALSE;
		fmtwidth = SCRNW1;
	}

	TICKX("fmtassn");
}

/*
	Output a newline to the current device.
*/
void
fmtcrlf(void)
{
	TICKB("fmtcrlf");

	if (fmtdev == TRUE) {
		syslout('\r');
		syslout('\n');
	}
	else if (hasdel == TRUE) {
		outxy(0, 1);
		outdel();
		outxy(0, SCRNL1);
		outdelln();
	}
	else {
		outxy(0,SCRNL1);
		outchar('\r');
		outchar('\n');
		pmtzap();
		pmtupd();
	}

	TICKX("fmtcrlf");
}

/*
	Output character to current device.
*/
static void
fmtdevch(char c)
{
	TRACEPB("fmtdevch", sl_lpout(); sl_cout(c); sl_rpout());

	if (fmtdev == TRUE) {
		syslout(c);
	}
	else {
		outchar(c);
	}

	TICKX("fmtdevch");
}

/*
	Return length of char c at column col.
*/
static int
fmtlench(char c, int col)
{
	TRACEPB("fmtlench",   sl_lpout();
		sl_cout(c);   sl_csout();
		sl_iout(col); sl_rpout());

	if (c == '\t') {
		/* tab every fmttab columns */
		RETURN_INT("fmtlench",(fmttab-(col%fmttab)));
	}
	else if (c<32) {
		/* control char */
		RETURN_INT("fmtlench",(2));
	}
	else {
		RETURN_INT("fmtlench",(1));
	}
}

/*
	Output one character to current device.
	Convert tabs to blanks.
*/
static void
fmtoutch(char c, int col)
{
	int k;

	TRACEPB("fmtoutch",   sl_lpout();
		sl_cout(c);   sl_csout();
		sl_iout(col); sl_rpout());

	if (c == '\t') {
		k = fmtlench('\t',col);
		while ((k--)>0) {
			fmtdevch(' ');
		}
	}
	else if (c<32) {
		fmtdevch('^');
		fmtdevch(c+64);
	}
	else {
		fmtdevch(c);
	}

	TICKX("fmtoutch");
}

/*
	Set tabs at every n columns.
*/
void
fmtset(int n)
{
	TRACEPB("fmtset", sl_lpout(); sl_iout(n); sl_rpout());

	fmttab = max(1,n);

	TICKX("fmtset");
}

/*
	Print string which ends with '\n' or '\0'.
	to current device.
	Truncate the string if it is too long.
*/
void
fmtsout(char *buf, int offset)
{
	char c;
	int col,k;

	TRACEPB("fmtsout",  sl_lpout();
		sl_sout(buf);    sl_csout();
		sl_iout(offset); sl_rpout());

	col = 0;

	for (;;) {
		c = *buf++;
		if (c == '\0' || c == '\n') {
			break;
		}
		k = fmtlench(c,col);
		if (col + k + offset > fmtwidth) {
			break;
		}
		fmtoutch(c,col);
		col += k;
	}

	TICKX("fmtsout");
}

/*
	Print buf[i] ... buf[j-1] on current device so long as
	characters will not be printed in last column.
*/
void
fmtsubs(char *buf, int i, int j)
{
	int k;

	TRACEPB("fmtsubs", sl_lpout();
		sl_sout(buf); sl_csout();
		sl_iout(i);   sl_csout(); 
		sl_iout(j);   sl_rpout());

	if (fmtcol[i] >= fmtwidth) {
		RETURN_VOID("fmtsubs");
	}

	/* Position the cursor. */
	outxy(fmtcol[i], outy);
	while (i < j) {
		if (fmtcol[i+1] > fmtwidth) {
			break;
		}
		fmtoutch(buf[i],fmtcol[i]);
		i++;
	}

	/* Clear rest of the line. */
	outdeol();

	TICKX("fmtsubs");
}
