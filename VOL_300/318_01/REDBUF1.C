/*
	RED buffer routines -- Full C version
	Part 1 -- goto, output and status routines.

 	Source:  redbuf1.c
	Version: February 4, 1985; January 18, 1990.

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
	Boundary conditions:

	1.  Only bufins() can extend the buffer, NOT
	    bufgo() and bufdn().

	2.  bufatbot() is true when the current line is
	    PASSED the last line of the buffer.  Both
	    bufgo() and bufdn() can cause bufatbot() to
	    become true.  bufgetln() returns a zero length
	    line if bufatbot() is true.

	3.  b_max_line is the number of lines in the buffer.
	    However, b_line == b_max_line + 1 is valid and
	    it means that b_line points at a null line.

	4.  All buffer routines assume that the variables
            b_bp, b_line and b_start describe the current
	    line when the routine is called.  Thus, any
	    routine which changes the current line must
	    update these variables.
*/

/*
	Define the format of the status line used only by
	the bufsusp() and bufinit() routines.
*/

#define MAGIC 1234
#define STATUS_LENGTH (25+SYSFNMAX)

struct STATUS {
	char stype      [5];	/* Magic byte		*/
	char stail      [5];	/* saved b_tail		*/
	char smax_diskp [5];	/* saved b_max_diskp	*/
	char smax_line  [5];	/* saved b_max_line	*/
	char sfree      [5];	/* saved b_free		*/
	char sfile [SYSFNMAX];	/* saved file name	*/
};

/*
	Return TRUE if at bottom of buffer (past the last line).
*/
#if 0 /* a macro now. */
int
bufatbot(void)
{
	TRACEP("bufatbot",
		sl_sout("returns ");
		sl_bout(b_line > b_max_line);
		sl_cout('\n'));

	return b_line > b_max_line;
}
#endif /* 0 */

/*
	Return TRUE if at top of buffer.
*/
#if 0 /* a macro now */
int
bufattop(void)
{
	TRACEP("bufattop",
		sl_sout("returns ");
		sl_bout(b_line == 1);
		sl_cout('\n'));

	return b_line == 1;
}
#endif /* 0 */

/*
	Return TRUE if the buffer has been changed.
*/
#if 0 /* now a macro */
int
bufchng(void)
{
	TRACEP("bufchng",
		sl_sout("returns ");
		sl_bout(b_cflag);
		sl_cout('\n'));

	return b_cflag;
}
#endif /* 0 */

/*
	Clear the slot table.  This should be done after
	read_file() overwrites  the slot table.
*/
void
buf_clr(void)
{
	struct BLOCK *bp;
	int i;

	TICK("buf_clr");

	for (i = 0; i < DATA_RES; i++) {
		bp = b_bpp [i];
		bp -> d_back   = ERROR;
		bp -> d_next   = ERROR;
		bp -> d_lines  = 0;
		bp -> d_status = FREE;
		bp -> d_lru    = i;
		bp -> d_diskp  = ERROR;
	}
}

/*
	Move towards end of buffer.
*/
void
bufdn(void)
{
	TICKB("bufdn");

	if (bufatbot()) {
		RETURN_VOID("bufdn");
	}
	else {
		b_line++;
		buf_gofw();
	}

	TICKX("bufdn");
}

/*
	Clean up any temporary files.
*/
void
bufend(void)
{
	TICKB("bufend");

	if (b_data_fd != ERROR) {
		sysclose(b_data_fd);	/* Bug fix: 6/17/84 */
		b_data_fd = ERROR;	/* Bug fix: 6/17/84 */
		sysunlink(DATA_FILE);
	}

	TICKX("bufend");
}

/*
	Go to line n.
	Set b_bp, b_line, b_start.
*/
void
bufgo(int n)
{
	int distance, oldline;

	TRACEPB("bufgo", sl_lpout(); sl_iout(n); sl_rpout());

	/* Put the request in range. */
	oldline  = b_line;
	b_line   = min(n, b_max_line + 1);
	b_line   = max(1, b_line);
	distance = b_line - oldline;

	if (distance == 0) {

		/* We are already at the requested line. */

		RETURN_VOID("bufgo");
	}
	else if (distance == 1) {

		/* Go forward from here. */
		buf_gofw();

		RETURN_VOID("bufgo");
	}
	else if (distance == -1) {

		/* Go back from here. */
		buf_gobk();

		RETURN_VOID("bufgo");
	}
	else if (distance > 0) {
		if ( b_line >
		     oldline + ((b_max_line - oldline) / 2)
		   ) {

			/* Search back from end of file. */
			b_bp  = swap_in(b_tail);
			b_start =
			    1 + b_max_line - b_bp -> d_lines;
			buf_gobk();

			RETURN_VOID("bufgo");
		}
		else {

			/* Search forward from here. */
			buf_gofw();

			RETURN_VOID("bufgo");
		}
	}
	else {
		if (b_line < oldline / 2) {

			/* Search from start of file. */
			b_bp    = swap_in(b_head);
			b_start = 1;
			buf_gofw();

			RETURN_VOID("bufgo");
		}
		else {

			/* Search back from here. */
			buf_gobk();

			RETURN_VOID("bufgo");
		}
	}
}

/*
	Search backwards from block for b_line.
	The starting line number of the block is b_start.
	Set b_bp and b_start.
*/
void
buf_gobk(void)
{
	int diskp;

	TRACEPB("buf_gobk",
		sl_sout("b_start: ");	sl_iout(b_start);
		sl_sout(", b_line: ");	sl_iout(b_line);
		sl_cout('\n'));

	if (b_bp == (struct BLOCK *) ERROR ||
	    b_start < 1 || b_start > b_max_line ||
	    b_line  < 1 || b_line  > b_max_line + 1) {

		cant_happen("buf_gobk 1");
	}

	/* Scan backward for the proper block. */
	while (b_start > b_line) {

		TRACEP("buf_bobk",
			sl_sout("***** go back to block ");
			sl_iout(b_bp -> d_back);
			sl_cout('\n'));

		/* Get the previous block in memory. */
		diskp = b_bp -> d_back;
		if (diskp == ERROR) {
			cant_happen("buf_gobk 2");
		}
		b_bp = swap_in(diskp);

		/* Calculate the start of the next block. */
		b_start -= b_bp -> d_lines;
		if (b_start <= 0) {
			cant_happen("buf_gobk 3");
		}
	}

	TICKX("buf_gobk");
}

/*
	Search forward from parcel par for line n.
	Set b_bp and b_start.
*/
void
buf_gofw(void)
{
	int diskp;

	TRACEPB("buf_gofw",
		sl_sout("b_start: ");	sl_iout(b_start);
		sl_sout(", b_line: ");	sl_iout(b_line);
		sl_cout('\n'));

	/* The last line is always null. */
	if (bufatbot()) {
		RETURN_VOID("buf_gofw");
	}

	if (b_bp == (struct BLOCK *) ERROR ||
	    b_start < 1 || b_start > b_max_line ||
	    b_line  < 1 || b_line  > b_max_line + 1) {

		cant_happen("buf_gofw 1");
	}

	/* Scan forward to the proper block. */
	while (b_start + b_bp -> d_lines <= b_line) {

		TRACEP("buf_bofw",
			sl_sout("***** go forward to block ");
			sl_iout(b_bp -> d_next);
			sl_cout('\n'));

		/* Get the start of the next block. */
		b_start += b_bp -> d_lines;

		/* Swap in the next block. */
		diskp = b_bp -> d_next;
		if (diskp == ERROR || b_start > b_max_line) {
			cant_happen("buf_gofw 2");
		}
		b_bp = swap_in(diskp);
	}

	TICKX("buf_gofw");
}

/*
	Initialize the buffer module.
	If the work file exists,  read block 0 into slot 0.
	Otherwise,  call bufnew to clear everything.
*/
void
bufinit(void)
{
	struct BLOCK *bp;
	struct STATUS *sp;
	int i, type;
	char *p;

	TICKB("bufinit");

	/* The data file has not been opened yet. */
	b_data_fd = ERROR;

	/* Dynamically allocate all slots. */
	for (i = 0; i < MAX_RES; i++) {

		p = sysalloc (BLOCK_SIZE);
		if (p == NULL) {
			break;
		}
		b_bpp [i] = (struct BLOCK *) p;
	}

	/* Set pseudo constant. */
	DATA_RES = i;

	/* The code requires at least three buffers. */
	if (DATA_RES < MIN_RES) {
		error("Not enough room for buffers.");
		exit(1);
	}

#ifdef SUSPEND
	/* Do nothing if no work file. */
	if (sysexists(DATA_FILE) == FALSE) {
		bufnew();
		RETURN_VOID("bufinit");
	}
	else {
		b_data_fd = sysopen(DATA_FILE);
		if (b_data_fd == ERROR) {
			error("Can not re-open work file.");
			exit(1);
		}
	}

	/* Read the first block of the work file. */
	b_bp = b_bpp [0];


	sysread(b_data_fd, (char *) b_bp);

	/*
		Restore RED's status which was written by
		the bufsusp() routine.
	*/
	b_head = 0;

#if 0
	sscanf(b_bp -> d_data, "%x %x %x %x %x %s\0",
		&type,
		&b_tail, &b_max_diskp, &b_max_line, &b_free,
		g_file);
	b_max_put = b_max_diskp;
#endif

	sp = (struct STATUS *) b_bp -> d_data;
	type        = atoi(sp -> stype);
	b_tail      = atoi(sp -> stail);
	b_max_diskp = atoi(sp -> smax_diskp);
	b_max_line  = atoi(sp -> smax_line);
	b_free      = atoi(sp -> sfree);
	strcpy(g_file, sp -> sfile);

	b_max_put   = b_max_diskp;	/* Bug fix: 5/24/84 */

	if (type != MAGIC) {
		/* Do NOT erase the work file!! */
		error("Previous work file garbled.");
		exit(1);
	}

	/* Free all slots. */
	buf_clr();

	/* Delete the status line. */
	b_line = b_start = 1;
	swap_in(0);
	bufdel();

	/* Make sure that the buffer will be saved. */
	b_cflag = TRUE;

	/* Do not erase work file on disk error. */
	b_fatal = FALSE;
#else
	bufnew();
#endif

	TICKX("bufinit");
}

/*
	Return the current line number.
*/
#if 0 /* now a macro */
int
bufln(void)
{
	TRACEP("bufln",
		sl_sout("returns b_line: ");
		sl_iout(b_line);
		sl_cout('\n'));

	return b_line;
}
#endif /* 0 */

/*
	Return the maximum line number.
*/

#if 0 /* now a macro */
int
bufmax(void)
{
	TRACEP("bufmax",
		sl_sout("returns b_max_line: ");
		sl_iout(b_max_line);
		sl_cout('\n'));

	return b_max_line;
}
#endif /* 0 */

/*
	Clear the buffer module.
*/
void
bufnew(void)
{
	struct BLOCK *bp;

	/* Free all slots. */

	TICKB("bufnew");

	buf_clr();

	/* Allocate the first slot. */
	b_bp        = b_bpp [0];
	b_head      = 0;
	b_tail      = 0;
	b_max_diskp = 0;
	b_max_put   = 0;
	b_bp -> d_diskp  = 0;
	b_bp -> d_status = DIRTY;

	/* Make sure temp file is erased. */
	if (b_data_fd != ERROR) {
		sysclose(b_data_fd);
		b_data_fd = ERROR;
		sysunlink(DATA_FILE);
	}
	b_free = ERROR;

	/* Set the current and last line counts. */
	b_line     = 1;
	b_max_line = 0;
	b_start    = 1;

	/* Indicate that the buffer has not been changed */
	b_cflag = FALSE;

	/* Do not erase work file on disk error. */
	b_fatal = FALSE;

	TICKX("bufnew");
}

/*
	Return TRUE if buffer is near the bottom line.
*/
#if 0 /* now a macro */
int
bufnrbot(void)
{
	TRACEP("bufnrbot",
		sl_sout("returns b_line >= b_max_line: ");
		sl_bout(b_line >= b_max_line);
		sl_cout('\n'));

	return b_line >= b_max_line;
}
#endif

/*
	Put nlines lines from buffer starting with line topline at
	position topy of the screen.
*/
void
bufout(int topline, int topy, int nlines)
{
	int l, x, y;

	SL_DISABLE();

	x = outx;
	y = outy;
	l = b_line;

	while (nlines > 0) {
		outxy(0, topy++);
		bufoutln(topline++);
		nlines--;
		if (hasint == TRUE) {
			sysintr(topline,topy,nlines);
			break;
		}
	}
	outxy(x,y);
	bufgo(l);
}
	
/*
	Print one line on screen.
*/
void
bufoutln(line)
int line;
{
	char buffer [MAXLEN1];
	int n;

	SL_DISABLE();

	bufgo(line);

	if ( (b_max_line == 0 && line == 2) ||
	     (b_max_line >  0 && line == b_max_line + 1)) {
		fmtsout("---------------- End of file. ----------------",0);
		outdeol();
	}
	else if (line > b_max_line) {
		outdeol();
	}
	else {
		n = bufgetln(buffer, MAXLEN);
		n = min(n, MAXLEN);
		buffer [n] = '\n';
		fmtsout(buffer, 0);
		outdeol();
	}
}

/*
	Replace current line with the line that p points to.
	The new line is of length n.
*/
void
bufrepl(char line [], int n)
{
	TRACEPB("bufrepl", sl_lpout();
		sl_pout(line); sl_csout();
		sl_iout(n);    sl_rpout());

	/* Do not replace null line.  Just insert. */
	if (bufatbot()) {
		bufins(line, n);
		RETURN_VOID("bufrepl");
	}

	/* Do not combine blocks until after insertion. */
	buf_d(1);
	bufins(line, n);
	combine();

	TICKX("bufrepl");
}

/*
	Save the work file in a temporary file in preparation
	for changing windows on the screen.

	NOTE:  This routine is not used at present.
*/
void
bufreset(char *window_file)
{
	TRACEPB("bufreset", sl_lpout(); sl_sout(window_file); sl_rpout());

	/* Make sure the work file is written. */
	swap_all();

	/* Close the work file. */
	sysclose(b_data_fd);
	b_data_fd = ERROR;
	
	/* Rename the work file to be the window file. */
	sysrename(DATA_FILE, window_file);

	TICKX("bufreset");
}

/*
	Indicate that the file has been saved.
*/
void
bufsaved(void)
{
	TRACEP("bufsaved", sl_sout("b_cflag = FALSE\n"));

	b_cflag = FALSE;
}

/*
	Suspend RED's execution for restart later.
*/

#ifdef SUSPEND
void
bufsusp(void)
{
	struct STATUS *sp;
	int length, i;
	char line [MAXLEN];

	TICKB("bufsusp");

	/* Bug fix 2/4/85: make sure the data file is open. */
	if (b_data_fd == ERROR) {
		b_data_fd = data_open();
	}

	/*
		Allocate space for the line.
		(This will ALWAYS be in block 0.)
	*/
	bufgo(1);
	for (i = 0; i < STATUS_LENGTH; i++) {
		line [i] = ' ';
	}
	bufins(line, STATUS_LENGTH);

	/*
		Set up the file status line.
		The bufinit() routines reads this line.

		The call to sprintf will also work but takes
		up about 400 hex bytes more space.
	*/

#if 0 /* avoid using printf */
	sprintf(line, "%4x %4x %4x %4x %4x %s\0",
		MAGIC, b_tail, b_max_diskp, b_max_line,
		b_free, g_file);
#endif

	sp = (struct STATUS *) line;

	length = itoc(MAGIC, sp -> stype, 5);
	sp -> stype [length] = '\0';

	length = itoc(b_tail, sp -> stail, 5);
	sp -> stail [length] = '\0';

	length = itoc(b_max_diskp, sp -> smax_diskp, 5);
	sp -> smax_diskp [length] = '\0';

	length = itoc(b_max_line, sp -> smax_line, 5);
	sp -> smax_line [length] = '\0';

	length = itoc(b_free, sp -> sfree, 5);
	sp -> sfree [length] = '\0';

	strcpy(sp -> sfile, g_file);

	/*
		Rewrite the status line WITHOUT changing
		the disk status.  This is fairly tricky;
		a call to bufrepl() here would not work.
	*/
	sysmove(line, b_bp -> d_data, STATUS_LENGTH);
	
	/* Make sure that work file is completely written. */
	swap_all();
	sysclose(b_data_fd);

	TICKX("bufsusp");
}
#endif

/*
	Move towards the head of the file.
*/
void
bufup(void)
{
	TICKB("bufup");

	if (bufattop()) {
		RETURN_VOID("bufup");
	}
	else {
		b_line--;
		buf_gobk();
	}

	TICKX("bufup");
}
