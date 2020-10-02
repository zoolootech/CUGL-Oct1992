/*
	RED prompt line module -- Full C version

	Source:  redpmt.c
	Version: December 18, 1983; January 18, 1990.

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
	Define routines local to this module.
*/
static void	pmt1upd	(int x);

/*
	Define variables local to this module.
*/
static int	pmt_zapped;
static int	pmt_hold;
static int	pmt_line;
static int	pmt_col;
static int	pmt_newfn;
static char	pmt_mode [MAXLEN];
static int	pmt_newmd;

/*
	Initialize this module.
*/
void
pmtclr(void)
{
	TICKB("pmtclr");

	pmt_zapped = TRUE;
	pmt_hold   = FALSE;
	pmt_newmd  = TRUE;
	pmt_newfn  = TRUE;

	TICKX("pmtclr");
}

/*
	Indicate that the global file name has been changed.
*/
void
pmtfn(void)
{
	TICKB("pmtfn");

	pmt_newfn = TRUE;

	TICKX("pmtfn");
}

/*
	Disable updates to the prompt line.
*/
void
pmthold(int flag)
{
	TRACEPB("pmthold", sl_lpout(); sl_iout(flag); sl_rpout());

	pmt_hold = flag;

	TICKX("pmthold");
}

/*
	Return the position of the next free spot on the line.
*/
int
pmtlast(void)
{
	SL_DISABLE();

	return 41 + strlen(pmt_mode);
}

/*
	Put error message on prompt line.
	Do NOT restore the cursor.
*/
void
pmtmess(char *s1, char *s2)
{
	/* Make sure line is correct. */
	TRACEPB("pmtmess",   sl_lpout();
		sl_sout(s1); sl_csout();
		sl_sout(s2); sl_rpout());

	pmt_zapped = TRUE;
	pmt1upd(outx);

	/* Output error messages. */
	outxy(41, 0);
	outdeol();
	fmtsout(" ", outx);
	fmtsout(s1,  outx);
	fmtsout(" ", outx);
	fmtsout(s2,  outx);

	/* Make sure the mode field gets restored. */
	pmt_newmd = TRUE;

	TICKX("pmtmess");
}

/*
	Change the mode immediately.
*/
void
pmtmode(char *newmode)
{
	TRACEPB("pmtmode", sl_lpout(); sl_sout(newmode); sl_rpout());

	if (strcmp(newmode, pmt_mode) != 0) {
		strcpy(pmt_mode, newmode);
		pmt_newmd = TRUE;
		pmthold(FALSE);
	}
	pmtupd();	/* 12/18/83 */

	TICKX("pmtmode");
}

/*
	Update the prompt line as needed.
*/
void
pmtupd(void)
{
	int x, y;

	TICKB("pmtupd");

	if (pmt_hold == TRUE) {
		RETURN_VOID("pmtupd");
	}

	/* Save cursor postion. */
	x = outx;
	y = outy;

	pmt1upd(x);

	/* Restore cursor position. */
	outxy(x, y);

	TICKX("pmtupd");
}

static void
pmt1upd(int x)
{
	int i;

	TRACEPB("pmt1upd", sl_lpout(); sl_iout(x); sl_rpout());

	/* Clear the line if it may have been corrupted. */
	if (pmt_zapped == TRUE) {
		outxy(0, 0);
		outdelln();
	}

	/* Update the line field. */
	if (pmt_zapped == TRUE) {
		outxy(0, 0);
		fmtsout("line: ",0);
		putdec(bufln(), 5);
	}
	else if (bufln() != pmt_line) {
		outxy(6, 0);
		putdec(bufln(),5);
	}
	pmt_line = bufln();

	/* Update the column field. */
	if (hascol == TRUE && pmt_zapped == TRUE) {
		outxy(12, 0);
		fmtsout("column: ", 12);
		putdec(x, 3);
	}
	else if (hascol == TRUE &&  x != pmt_col) {
		outxy(20,0);
		putdec(x, 3);
	}
	pmt_col = x;

	/* Update the file name field. */
	if (pmt_zapped == TRUE || pmt_newfn == TRUE) {
		outxy(25,0);
		if (g_file [0] == '\0') {
			fmtsout(" ..no file.. ",25);
		}
		else {
			fmtsout(g_file,25);
			for (i = 0; i < 14 - strlen(g_file); i++) {
				fmtsout(" ", 0);
			}
		}
	}
	pmt_newfn = FALSE;

	/* Update the mode field. */
	if (pmt_zapped == TRUE || pmt_newmd == TRUE) {
		outxy(41,0);
		fmtsout(pmt_mode,41);
		outdeol();
	}
	pmt_newmd = FALSE;

	/* Clear the zapped flag. */
	pmt_zapped = FALSE;

	TICKX("pmt1upd");
}

/*
	Indicate that the prompt line has been overwritten.
*/
void
pmtzap(void)
{
	TICKB("pmtzap");

	pmt_zapped = TRUE;

	TICKX("pmtzap");
}
