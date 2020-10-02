/*
	RED window module -- Full C version

	Source:  reded.c
	Version: March 28, 1986; January 18, 1990;

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
static void	edadj	(void);
static void	edredraw(void);
static int	edscan	(int xpos);
static void	ed_sup	(int topline);
static void	ed_sdn	(int topline);
static void	ed_ind	(void);
static void	ed_inu	(void);
static void	ed_del	(void);

/*
	Declare variables local to this module.
*/
static char	editbuf[MAXLEN];	/* the edit buffer	*/
static int	editp;			/* cursor: buffer index	*/
static int	editpmax;		/* length of buffer	*/
static int	edcflag;		/* buffer change flag	*/
static int	edaflag;		/* auto mode flag	*/
static int	edtop;			/* current top of screen */

/*
	True if the current edit line is displayed at bottom of screen.
*/
#define edatbot() (outy == SCRNL1)
#define edattop() (outy == 1)

/* The x position of the cursor on screen. */
#define edxpos() (fmtcol[editp])

/*
	Abort any changes made to current line.
*/
void
edabt(void)
{
	TICKB("edabt");

	/* Get unchanged line and reset cursor. */
	edgetln();
	edredraw();
	edbegin();
	edcflag = FALSE;

	TICKX("edabt");
}

/*
	Put cursor at beginning of current line.
*/
void
edbegin(void)
{
        TICKB("edbegin");

        editp = 0;
	outxy(0, outy);

	TICKX("edbegin");
}

/*
	Move the cursor back one word.
*/
void
edbword(void)
{
	int c;

	TICKB("edbword");

	/* Move up one line if at left margin. */
	if (editp == 0) {
		if (!bufattop()) {
			edup();       
			edend();
		}
		RETURN_VOID("edbword");
	}

	/* Scan for white space. */
	while (--editp > 0) {
		c = editbuf [editp];
		if (c == ' ' || c == '\t') {
			break;
		}
	}

	/* Scan for non-white space. */
	while (editp > 0) {
		c = editbuf [--editp];
		if (c != ' ' && c != '\t') {
			break;
		}
	}

	/* Reset cursor. */
	outxy(edxpos(), outy);

	TICKX("edbword");
}

/*
	Change editbuf[editp] to c.
	Don't make change if line would become to long.
*/
void
edchng(c)
char c;
{
	char oldc;
	int k;

	TRACEPB("edchng", sl_lpout(); sl_cout(c); sl_rpout());

	/* If at right margin then insert char. */
	if (editp >= editpmax) {
		edins(c);
		RETURN_VOID("edchng");
	}

	/* Change char and print length of line. */
	oldc = editbuf[editp];
	editbuf[editp] = c;
	fmtadj(editbuf, editp, editpmax);

	k = fmtcol[editp+1];
	if (k > SCRNW1) {

		/*
			The line would become too long.
			Undo the change.
		*/
		editbuf[editp] = oldc;
		fmtadj(editbuf, editp, editpmax);
	}
	else {
		/* Set change flag, bump cursor, redraw line. */
		edcflag = TRUE;
		editp++;
		edadj();
		edredraw();
	}

	TICKX("edchng");
}

/*
	Indicate that the screen has been changed by
	an agent outside of this module.
*/
void
edclr(void)
{
	TICKB("edclr");

	edtop = -1;

	TICKX("edclr");
}

/*
	Delete the char to left of cursor if it exists.
*/
void
eddel(void)
{
	int i, k;

	TICKB("eddel");

	/* Just move left one column if past end of line. */
	if (edxpos() < outx) {
		outxy(outx-1, outy);
		RETURN_VOID("eddel");
	}

	/* Do nothing if cursor is at left margin. */
	if (editp == 0) {
		RETURN_VOID("eddel");
	}
	edcflag = TRUE;

	/* Compress buffer (delete char). */
	k = editp;
	while (k < editpmax) {
		editbuf[k-1] = editbuf[k];
		k++;
	}

	/* Update pointers, update line. */
	editp--;
	editpmax--;
	edredraw();

	TICKX("eddel");
}

/*
	Delete the character under the cursor.
*/
void
ed2del(void)
{
	int i, k;

	TICKB("ed2del");

	/* Just move left one column if past end of line. */
	if (editp > 0 && editp == editpmax && edxpos() <= outx) {
		outxy(outx-1, outy);
		RETURN_VOID("ed2del");
	}

	/* Do nothing if cursor is at left margin. */
	if (editpmax == 0) {
		RETURN_VOID("ed2del");
	}
	edcflag = TRUE;

	/* Compress buffer (delete char). */
	k = editp+1;
	while (k < editpmax) {
		editbuf[k-1] = editbuf[k];
		k++;
	}

	/* Adjust the cursor if now at end. */
	if (editp+1 >= editpmax && editp > 0) {
		editp--;
	}
	editpmax--;
	if (editp == editpmax && editp > 0) {
		editp--;
	}
	edredraw();

	TICKX("ed2del");
}

/*
	Edit the next line.  Do not go to end of buffer.
*/
void
eddn(void)
{
	int oldx;

	TICKB("eddn");

	/* Save visual position of cursor. */
	oldx = outx;

	/* Replace current edit line. */
	edrepl();

	/* Do not go past last non-null line. */
	if (bufnrbot()) {
		if (edatbot()) {
			ed_sup(bufln()-SCRNL2+1);
			bufout(bufln() + 1, SCRNL1, 1);
			outxy(0, SCRNL2);
		}
		RETURN_VOID("eddn");
	}

	/* Move down one line in buffer. */
	bufdn();
	edgetln();

	/*
		Put cursor as close as possible on this
		new line to where it was on the old line.
	*/

	editp = edscan(oldx);

	/* Update screen. */
	if (edatbot()) {
		ed_sup(bufln()-SCRNL2);
		outxy(oldx, SCRNL1);
	}
	else {
		outxy(oldx, outy+1);
	}

	RETURN_VOID("eddn");
}

/*
	Put cursor at the end of the current line.
	Make sure it doesn't go off the screen.
*/
void
edend(void)
{
	TICKB("edend");

	editp = editpmax;
	edadj();
	outxy(edxpos(), outy);

	TICKX("edend");
}

/*
	Move the cursor forward one word.
	Move to the next line if at end of line.
*/
void
edfword(void)
{
	int c;

	TICKB("edfword");

	/* Move down one line if at right margin. */
	if (editp == editpmax) {
		eddn();
		edbegin();
		RETURN_VOID("edfword");
	}

	/* Scan for white space. */
	while (++editp < editpmax) {
		c = editbuf [editp];
		if (c == ' ' || c == '\t') {
			break;
		}
	}

	/* Scan for non-white space. */
	while (editp < editpmax) {
		c = editbuf [++editp];
		if (c != ' ' && c != '\t') {
			break;
		}
	}

	/* Reset cursor. */
	edadj();
	outxy(edxpos(), outy);

	TICKX("edfword");
}

/*
	Fill edit buffer from current main buffer line.
	The caller must check to make sure the main
	buffer is available.
*/
void
edgetln(void)
{
	int k;

	TICKB("edgetln");

	/* Put cursor on left margin, reset flag. */
	editp = 0;
	edcflag = FALSE;

	/* Get edit line from main buffer. */
	k = bufgetln(editbuf,MAXLEN);
	if (k > MAXLEN) {
		error("line truncated");
		editpmax = MAXLEN;
	}
	else {
		editpmax = k;
	}
	fmtadj(editbuf,0,editpmax);

	TICKX("edgetln");
}

/*
	Start editing line n;  set editp to x.

	At one time I thought that the search and change
	commands needed a separate version of this routine
	that would try to draw the line at the top of the
	screen.  However,  that did not provide enough
	context for the person looking at the patterns.
*/
void
edgo(newline, x)
int newline, x;
{
	int line;
	int oldlast;

	TRACEPB("edgo",  sl_lpout();
		sl_iout(newline); sl_csout();
		sl_iout(x);       sl_rpout());

	/* Save the status of the screen. */
	oldlast  = edtop + SCRNL2;

	/* Replace current line. */
	edrepl();

	/* Go to new line. */
	bufgo(newline);

	/* Prevent going past end of buffer. */
	if (bufatbot()) {
		bufup();
	}
	newline = bufln();

	/* Do not redraw the whole screen if not needed. */
	if (edtop > 0 && newline >= edtop && newline <= oldlast) {
		/* Just move to new line. */
		line = 1 + (newline - edtop);
	}
	else if (bufln() + SCRNL/2 > bufmax()) {
		/* The line is near the end of file. */
		edtop = max(1,  bufmax() - (SCRNL-3));
		line = 1 + (bufln() - edtop);
		bufout(edtop, 1, SCRNL1);
	}
	else {
		/* Draw the line in the middle of the screen. */
		edtop = max(1, bufln() - SCRNL/2);
		line  = 1 + (bufln() - edtop);
		bufout(edtop, 1, SCRNL1);
	}

	edgetln();
	editp = x;
	outxy(edxpos(), line);

	TICKX("edgo");
}

/*
	Insert c into the buffer if possible.
	Return TRUE if the line was auto-split.
*/
int
edins(char c)
{
	int i, k;
	int oldcflag;

	TRACEPB("edins", sl_lpout(); sl_cout(c); sl_rpout());

	/* Do nothing if edit buffer is full. */
	if (editpmax >= MAXLEN) {
		RETURN_INT("edins", FALSE);
	}

	/* Provisionally say we've made a change. */
	oldcflag = edcflag;
	edcflag  = TRUE;

	/* Fill out line if we are past its end. */
	if (editp == editpmax && edxpos() < outx) {
		k = outx - edxpos();
		editpmax = editpmax + k;
		while (k-- > 0) {
			editbuf [editp++] = ' ';
		}
		editp = editpmax;
	}

	/* Make room for inserted character. */
	k = editpmax;
	while (k > editp) {
		editbuf[k] = editbuf[k-1];
		k--;
	}

	/* Insert character.  Update pointers. */
	editbuf[editp] = c;
	editp++;
	editpmax++;

	/* Find where the cursor will be. */
	fmtadj(editbuf,editp-1,editpmax);
	k = fmtcol[editp];

	if (k > SCRNW1 && editp == editpmax && haswrap) {
		/* Auto-split the line (line wrap) */

		/* Scan for start of current word. */
		k = editp - 1;
		while (k >= 0 && editbuf[k] != ' ' && editbuf[k] != '\t') {
			k--;
		}

		/* Never split a word. */
		if (k < 0) {
			eddel();
			edcflag = oldcflag;
			RETURN_INT("edins", FALSE);
		}

		/* Split the line at the current word. */
		editp = k + 1;
		edsplit();
		edend();
		RETURN_INT("edins", TRUE);
	}
	else if (k > SCRNW1) {

		/*
			Cursor would move off the screen.
			Delete what we just inserted.
		*/
		eddel();
		edcflag = oldcflag;
	}
	else {
		/* Set change flag, redraw line. */
		edredraw();
	}
	RETURN_INT("edins", FALSE);
}

/*
	Join (concatenate) the current line with the one above it.
	Put cursor at the join point unless it would be off screen.
*/
void
edjoin(void)
{
	int k1, k2;
	int k;

	TICKB("edjoin");

	/* Do nothing if at top of file. */
	if (bufattop()) {
		RETURN_VOID("edjoin");
	}

	/* Replace lower line temporarily. */
	edrepl();

	/* Get upper line into buffer. */
	bufup();
	k1 = bufgetln(editbuf, MAXLEN);

	/* Append lower line to buffer. */
	bufdn();
	k2 = bufgetln(editbuf+k1, MAXLEN-k1);

	/* Abort if the line isn't wide enough. */
	if (k1 + k2 > MAXLEN1) {
		bufgetln(editbuf,MAXLEN);
		RETURN_VOID("edjoin");
	}

	/* Replace upper line, set cursor to middle of line. */
	bufup();
	editpmax = k1 + k2;
	editp = k1;
	edadj();
	edcflag = TRUE;
	edrepl();

	/* Delete the lower line from the buffer. */
	bufdn();
	bufdel();

	/*
		Delete the lower line on the screen,
		move up and redraw.
	*/
		
	if (!edattop()) {
		ed_del();
		outxy(outx, outy - 1);
		bufup();
		edredraw();
	}
	else {
		bufup();
		edtop = bufln();

		/* Redraw the ENTIRE line. */
		fmtadj (editbuf,0,editpmax);
		fmtsubs(editbuf,0,editpmax);
		outxy(edxpos(), outy);
	}

	TICKX("edjoin");
}

/*
	Delete WORDS until end of line or c found.
*/
void
edkill(char c)
{
	int k,p;
	int lastc;

	TRACEPB("edkill", sl_lpout(); sl_cout(c); sl_rpout());

	/* Do nothing if at right margin. */
	if (editp == editpmax) {
		RETURN_VOID("edkill");
	}
	edcflag = TRUE;

	/*
		Count number of deleted characters.
		The matched char must start a word.
	*/
	k = 1;
	lastc = ' ';
	while ((editp+k) < editpmax) {
		if ( editbuf[editp+k] == c &&
		     (!isalpha(lastc) || !isalpha(c))
		   ) {
			break;
		}
		else {
			lastc = editbuf[editp+k];
			k++;
		}
	}

	/* Compress buffer (delete chars). */
	p = editp+k;
	while (p < editpmax) {
		editbuf[p-k] = editbuf[p];
		p++;
	}

	/* Update buffer size, redraw line. */
	editpmax = editpmax-k;
	edredraw();

	TICKX("edkill");
}

/*
	Move cursor left one column.
	Never move the cursor off the current line.
*/
void
edleft(void)
{
	int k;

	TICKB("edleft");

	/* If past right margin, move left one column. */
	if (edxpos() < outx) {
		outxy(max(0, outx-1), outy);
	}

	/* Inside the line.  move left one character. */
	else if (editp != 0) {
		editp--;
		outxy(edxpos(), outy);
	}

	TICKX("edleft");
}

/*
	Insert a new blank line below the current line.
*/
void
ednewdn(void)
{
	TICKB("ednewdn");

	/*
		Make sure there is a current line and 
		Put the current line back into the buffer.
	*/

	if (bufatbot()) {
		bufins(editbuf,editpmax);
	}
	edrepl();

	/* Move past current line. */
	bufdn();

	/* Insert place holder:  zero length line. */
	bufins(editbuf,0);

	/* Start editing the zero length line. */
	edgetln();

	/* Update the screen. */
	ed_ind();

	TICKX("ednewdn");
}

/*
	Insert a new blank line above the current line.
*/
void
ednewup(void)
{
	TICKB("ednewup");

	/* Put current line back in buffer. */
	edrepl();

	/* Insert zero length line at current line. */
	bufins(editbuf,0);

	/* Start editing the zero length line. */
	edgetln();

	/* Update the screen. */
	ed_inu();

	TICKX("ednewup");
}

/*
	Replace current main buffer line by edit buffer.
	The edit buffer is NOT changed or cleared.
*/
void
edrepl(void)
{
	TICKB("edrepl");

	/* Do nothing if nothing has changed. */
	if (edcflag == FALSE) {
		RETURN_VOID("edrepl");
	}

	/* Make sure we don't replace the line twice. */
	edcflag = FALSE;

	/* Insert instead of replace if at bottom of file. */
	if (bufatbot()) {
		bufins(editbuf,editpmax);
	}
	else {
		bufrepl(editbuf,editpmax);
	}

	TICKX("edrepl");
}

/*
	Move cursor right one character.
	Never move the cursor off the current line.
*/
void
edright(void)
{
	TICKB("edright");

	/* If we are outside the line move right one column. */
	if (edxpos() < outx) {
		outxy (min(SCRNW1, outx+1), outy);
	}

	/* If we are inside a tab move to the end of it. */
	else if (edxpos() > outx) {
		outxy (edxpos(), outy);
	}

	/* Move right one character if inside line. */
	else if (editp < editpmax) {
		editp++;
		edadj();
		outxy(edxpos(), outy);
	}

	/* Else move past end of line. */
	else {
		outxy (min(SCRNW1, outx+1), outy);
	}

	TICKX("edright");
}

/*
	Split the current line into two parts.
	Scroll the first half of the old line up.
*/
void
edsplit(void)
{
	int p, q;
	int k;

	TICKB("edsplit");

	/* Indicate that edit buffer has been saved. */
	edcflag = FALSE;

	/* Replace current line by the first half of line. */
	if (bufatbot()) {
		bufins(editbuf, editp);
	}
	else {
		bufrepl(editbuf, editp);
	}

	/* Redraw the first half of the line. */
	p = editpmax;
	q = editp;
	editpmax = editp;
	editp = 0;
	edredraw();

	/* Move the second half of the line down. */
	editp = 0;
	while (q < p) {
		editbuf [editp++] = editbuf [q++];
	}
	editpmax = editp;
	editp = 0;

	/* Insert second half of the line below the first. */
	bufdn();
	bufins(editbuf, editpmax);

	/* Insert a line on the screen and draw it. */
	ed_ind();
	edredraw();

	TICKX("edsplit");
}

/*
	Move cursor right until end of line or char c found.
	Do not move the cursor off the end of the line.
*/
void
edsrch(char c)
{
	int lastc;

	TRACEPB("edsrch", sl_lpout(); sl_cout(c); sl_rpout());

	/* Do nothing if at right margin. */
	if (editp == editpmax) {
		RETURN_VOID("edsrch");
	}

	/* Scan for search character. */
	editp++;
	if (editpmax == 0) {
		lastc = ' ';
	}
	else {
		/* 3/28/86 */
		lastc = editbuf [editp];
	}
	while (editp < editpmax) {
		if ( editbuf[editp] == c &&
		     (!isalpha(lastc) || !isalpha(c))
		   ) {
			break;
		}
		else {
			lastc = editbuf[editp];
			editp++;
		}
	}

	/* Reset cursor. */
	edadj();
	outxy(edxpos(), outy);

	TICKX("edsrch");
}

/*
	Move cursor up one line if possible.
*/
void
edup(void)
{
	int oldx;

	TICKB("edup");

	/* Save visual position of cursor. */
	oldx = outx;

	/* Put current line back in buffer. */
	edrepl();

	/* Done if at top of buffer. */
	if (bufattop()) {
		RETURN_VOID("edup");
	}

	/* Start editing the previous line. */
	bufup();
	edgetln();

	/*
		Put cursor on this new line as close as
		possible to where it was on the old line.
	*/
	editp = edscan(oldx);

	/* Update screen. */
	if (edattop()) {
		ed_sdn(bufln());
		outxy(oldx, 1);
	}
	else {
		outxy(oldx, outy-1);
	}

	TICKX("edup");
}

/*
	Delete the current line.
*/
void
edzap(void)
{
	int k;

	TICKB("edzap");

	/* Delete the line in the buffer. */
	bufdel();

	/* Move up one line if now at bottom. */
	if (bufatbot() && edattop()) {
		bufup();
		edtop = bufln();
		edgetln();
		edredraw();
	}
	else if (bufatbot()) {
		ed_del();
		outxy(0, outy - 1);
		bufup();
		edgetln();
	}
	else {
		ed_del();
		edgetln();
	}

	TICKX("edzap");
}


/* ----- utility routines (not used outside this file) ----- */


/*
	Adjust the cursor position so the cursor stays on screen.
	This must be called whenever the cursor could move right,
	i.e., in edchng(), edend(), edjoin(), edright() and edsrch().
*/
static void
edadj(void)
{
	TICKB("edadj");

	while (fmtcol[editp] > SCRNW1) {
		editp--;
	}

	TICKX("edadj");
}


/*
	Redraw edit line from index to end of line and
	reposition cursor.
*/
static void
edredraw(void)
{
	TICKB("edredraw");

	fmtadj(editbuf,0,editpmax);
	fmtsubs(editbuf,max(0,editp-1),editpmax);
	outxy(edxpos(), outy);

	TICKX("edredraw");
}

/*
	Set editp to the largest index such that
	buf[editp] will be printed <= xpos
*/
static int
edscan(int xpos)
{
	TRACEPB("edscan", sl_lpout(); sl_iout(xpos); sl_rpout());

	editp = 0;
	while (editp < editpmax) {
		if (fmtcol[editp] < xpos) {
			editp++;
		}
		else {
			break;
		}
	}

	RETURN_INT("edscan", editp);
}

/*
	Scroll the screen up.  Topline will be new top line.
*/
static void
ed_sup(int topline)
{
	TRACEPB("ed_sup", sl_lpout(); sl_iout(topline); sl_rpout());

	edtop = topline;
	if (hasdel == TRUE) {

		/* Delete line 1. */
		outxy(0, 1);
		outdel();

		/* Redraw bottom line. */
		bufout(topline+SCRNL2,SCRNL1,1);
	}
	else if (hasup == TRUE) {

		/* Hardware scroll. */
		outsup();

		/* Redraw bottom line. */
		bufout(topline+SCRNL2,SCRNL1,1);

		/* Restore the prompt line. */
		pmtzap();
	}
	else {

		/* Redraw whole screen. */
		bufout(topline,1,SCRNL1);

		/* Restore the prompt line. */
		pmtzap();
	}

	TICKX("ed_sup");
}

/*
	Scroll screen down.  Topline will be new top line.
*/
static void
ed_sdn(topline)
int topline;
{
	TRACEPB("ed_sdn", sl_lpout(); sl_iout(topline); sl_rpout());

	edtop = topline;
	if (hasins == TRUE) {

		/* Insert a line above line 1. */
		outxy(0, 1);
		outins();

		/* Redraw top line. */
		bufout(topline,1,1);
	}
	else if (hasdn == TRUE) {

		/* Hardware scroll. */
		outsdn();

		/* Redraw top line. */
		bufout(topline,1,1);

		/* Redraw the prompt line. */
		pmtzap();
	}
	else {

		/* Redraw whole screen. */
		bufout(topline,1,SCRNL1);

		/* Redraw the prompt line. */
		pmtzap();
	}

	TICKX("ed_sdn");
}

/*
	Insert one line below the current line on the screen.
*/
static void
ed_ind(void)
{
	int y;

	TICKB("ed_ind");

	if (edatbot()) {
		ed_sup(bufln()-SCRNL2);
		outxy(edxpos(),SCRNL1);
	}
	else if (hasins == TRUE) {
		y = outy;
		outxy(0, y+1);
		outins();
		outxy(edxpos(), y+1);
	}
	else {
		y = outy;
		bufout(bufln(), y+1, SCRNL1-y);
		outxy(edxpos(), y+1);
	}

	TICKX("ed_ind");
}

/*
	Insert a line above the current line on the screen.
*/
static void
ed_inu(void)
{
	int y;

	TICKB("ed_inu");

	if (hasins == TRUE) {
		y = outy;
		outins();
		outxy(edxpos(), y);
	}
	else if (edattop()) {
		ed_sdn(bufln());
		outxy(edxpos(), 1);
	}
	else {
		y = outy;
		bufout(bufln(), y, SCRNL - y);
		outxy(edxpos(), y);
	}

	TICKX("ed_inu");
}

/*
	Delete one line from the screen.
*/
static void
ed_del(void)
{
	int y;

	TICKB("ed_del");

	/* Remember what line we are on. */
	y = outy;

	if (hasdel == TRUE) {
		outdel();
		bufout(bufln() + SCRNL1 - y, SCRNL1, 1);
	}
	else if (edattop()) {
		ed_sup(bufln());
		bufout(bufln() + SCRNL1 - y, SCRNL1, 1);
	}
	else {
		bufout(bufln(), y, SCRNL - y);
	}

	/* Set cursor to beginning of line. */
	outxy(0, y);

	TICKX("ed_del");
}
