/*
	RED buffer routines -- Full C version
	Part 2 -- line routines.

	Source:  redbuf2.c
	Version: February 20, 1984; January 18, 1990.

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
	Declare routines local to this file.
*/
static void		del_bl		(int nlines);
static void		del_mid		(int dlines);
static void		del_tail	(int slines, int nlines);
static void 		combine1	(int diskp1, int diskp2);
static void		free_block	(struct BLOCK * bp);
static struct BLOCK *	new_block	(void);
static struct BLOCK *	split_block	(void);

/*
	b_getnum(char_ptr)
	Get an integer given a pointer to char.

	b_putnum(char_ptr)
	Put an integer given a pointer to char.
*/

#if 0 /* The following are all defined as macros in redbuf.h */

int
b_getnum(char *p)
{
	TRACEP("b_getnum",
		sl_lpout(); sl_pout(p);
		sl_sout(") returns ");
		sl_iout( * (int *) p);
		sl_cout('\n'));

	return  (* (int *) p);
}

void
b_putnum(char *p, int num)
{
 	TRACEP("b_putnum",  sl_lpout();
		sl_sout("* (int *) (");
		sl_pout(p);
		sl_sout(") = ");
		sl_iout(num); 
		sl_cout('\n'));

 	* (int *) p = num;
}

/*
	The following routines manage the index table,
	which grows down from the end of each block.

	b_tab(bp, index)
	b1_tab(index)
	Get an entry of the index table.

	b_settab(bp, index, value)
	b1_settab(index, value)
	Set an entry of the index table.
*/
int
b1_tab(int index)
{
	TRACEP("b1_tab", sl_sout(":: "));

	b_tab(b_bp, index);
}

int
b_tab(struct BLOCK *bp, int index)
{
	TRACEP("b_tab",  sl_lpout();
		sl_pout(bp); sl_csout();
		sl_iout(index); 
		sl_sout(") :: "));

	b_getnum(bp->d_data + BUFF_SIZE - (sizeof(int)*(index+1)));
}

void
b1_settab(int index, int value)
{
	TRACEP("b1_settab", sl_sout(":: "));

	b_settab(b_bp, index, value);
}

void
b_settab(struct BLOCK  *bp, int index, int value)
{
	TRACEP("b_settab", sl_lpout();
		sl_pout(bp); sl_csout();
		sl_iout(index); sl_csout(); 
		sl_iout(value);
		sl_sout(") :: "));

	b_putnum(bp->d_data + BUFF_SIZE - (sizeof(int)*(index+1)),
		 value);
}

/*
	b_len(bp, line)
	b1_len(line)
	Return the length of a line.

	b_ptr(bp, line)
	b1_ptr(line)
	Return a pointer to a line.

	b_prefix(bp, line)
	b1_prefix(line)
	Return the number of chars befor the start of a line.
*/
int
b1_length(int line)
{
	TRACEP("b1_length", sl_sout(":: "));

	b_length(b_bp, line);
}

int
b_length(struct BLOCK *bp, int line)
{
	TRACEP("b_length",  sl_lpout();
		sl_pout(bp); sl_csout(); sl_iout(line);
		sl_sout(") :: "));

	if (line < 0 || line >= bp -> d_lines) {
		cant_happen("b_len");
	}

	if (bufatbot()) {
		/* The last line is always null. */
		TRACE("b_length", sl_iout(0); sl_cout('\n'));
		return 0;
	}
	else if (line == 0) {
		TRACE("b_length", sl_iout(b_tab(bp, line)); sl_cout('\n'));
		return b_tab(bp, line);
	}
	else {
		TRACE("b_length",
			sl_iout(b_tab(bp, line) - b_tab(bp, line - 1));
			sl_cout('\n'));

		return (b_tab(bp, line) - b_tab(bp, line - 1));
	}
}
		
char *
b1_ptr(int line)
{
	TRACEP("b1_ptr", sl_sout(":: "));

	b_ptr(b_bp, line);
}

char *
b_ptr(struct BLOCK  *bp, int line)
{
	TRACEP("b_ptr",  sl_lpout();
		sl_pout(bp); sl_csout(); sl_iout(line);
		sl_sout(") :: "));

	if (line < 0 || line >= bp -> d_lines) {
		cant_happen("b_ptr");
	}
	if (line == 0) {
		TRACE("b_ptr", sl_pout(bp -> d_data); sl_cout('\n'));
		return bp -> d_data;
	}
	else {
		TRACE("b_ptr",
			sl_pout(bp -> d_data + b_tab(bp, line - 1));
			sl_cout('\n'));
		return (bp -> d_data + b_tab(bp, line - 1));
	}
}

int
b1_prefix(int line)
{
	TRACEP("b1_prefix", sl_sout(":: "));

	return b_prefix(b_bp, line);
}

int
b_prefix(struct BLOCK *bp, int line)
{
	TRACEP("b_prefix",  sl_lpout();
		sl_pout(bp); sl_csout();
		sl_iout(line);
		sl_sout(") :: "));

	if (line < 0 || line >= bp -> d_lines) {
		cant_happen("b_prefix");
	}
	if (line == 0) {
		TRACE("b_prefix", sl_iout(0); sl_cout('\n'));
		return 0;
	}
	else {
		TRACE("b_prefix",
			sl_iout(b_tab (bp, line - 1));
			sl_cout('\n'));
		return (b_tab (bp, line - 1));
	}
}

/*
	The following routines make accessing fields of blocks
	a bit easier.  They also save a few bytes of memory.

	b1_nlines()
	b_nlines(bp)
	Return the number of lines in a block.

	b1_avail()
	b1_avail(bp)
	Return the number of free characters in a block.
*/

int
b1_nlines(void)
{
	TRACEP("b1_nlines",
		sl_sout("returns ");
		sl_iout(b_bp -> d_lines);
		sl_cout('\n'));

	return b_bp -> d_lines;
}

int
b_nlines(struct BLOCK *bp)
{
	TRACEP("b_nlines",
		sl_lpout();
		sl_pout(bp);
		sl_sout(") returns ");
		sl_iout(bp -> d_lines);
		sl_cout('\n'));

	return bp -> d_lines;
}

int
b1_avail(void)
{
	TRACEP("b1_avail", sl_sout(" :: "));

	return b_avail(b_bp);
}

int
b_avail(struct BLOCK *bp)
{
	TRACEP("b_avail",
		sl_lpout();
		sl_pout(bp);
		sl_sout(") returns "));

	if (bp -> d_lines == 0) {
		TRACE("b_avail", sl_iout(BUFF_SIZE); sl_cout('\n'));
		return BUFF_SIZE;
	}
	else {
		TRACE("b_avail",
			sl_iout(BUFF_SIZE - b_tab(bp, bp->d_lines-1) -
			       (sizeof(int) * bp->d_lines));
			sl_cout('\n'));

		return (BUFF_SIZE - b_tab(bp, bp->d_lines-1) -
		       (sizeof(int) * bp->d_lines));
	}
}

#endif /* 0 */

/*
	Delete the current line.
*/
void
bufdel(void)
{
	TICKB("bufdel");

	buf_d(1);
	combine();

	TICKX("bufdel");
}


/*
	Delete multiple lines starting with the current line.
	nlines is the number of lines to delete.
*/
void
bufdeln(int nlines)
{
	TRACEPB("bufdeln",  sl_lpout(); sl_iout(nlines); sl_rpout());

	buf_d(nlines);
	combine();

	TICKX("bufdeln");
}
	
/*
	Internal delete routine.  

	This routine does not combine blocks so that bufrepl()
	can wait to the last possible moment to call combine().
*/
void
buf_d(int nlines)
{
	int lines, slines;

	TRACEPB("buf_d", sl_lpout(); sl_iout(nlines); sl_rpout());

	while (nlines > 0 && !bufatbot()) {

		/* The current block will become dirty. */
		is_dirty(b_bp);
		b_cflag = TRUE;

		/*
			Precompute the number of lines in the
			block and the number of lines befor
			the current line.
		*/

		lines  = b_bp -> d_lines;
		slines = b_line - b_start;

		/* Delete lines block by block. */
		if (slines == 0 && nlines >= lines) {

			/* Delete the whole block. */
			del_bl(lines);
			nlines -= lines;
			check_block("delete 1");
		}
		else if (nlines >= lines - slines) {

			/* Delete tail of the block. */
			del_tail(slines, lines - slines);
			nlines -= (lines - slines);
			check_block("delete 2");
		}
		else {
			/* Delete from middle of the block. */
			del_mid(nlines);
			check_block("delete 3");
			break;
		}
	}

	TICKX("buf_d");
}

/*
	Delete the current block containing 'nlines' lines.

	There is much more to this code than meets the eye.
	First,  notice that using combine() here would slow
	down the a  multiple-line  delete  much  too  much.
	Second,  we never want to call free_block() for the
	first block since we want any recovery  program  to
	find  the  first block of the work file at block 0.
	Third,  the  call  to  combine()  at the end of the
	bufdeln() routine will eventually fill up  block  0
	unless the entire file is deleted.
*/
static void
del_bl(int nlines)
{
	struct BLOCK *bp1, *oldblock;
	int back, next, current;

	TRACEPB("del_bl",  sl_lpout(); sl_iout(nlines); sl_rpout());

	/* Point to the previous and next blocks. */
	back    = b_bp -> d_back;
	next    = b_bp -> d_next;
	current = b_bp -> d_diskp;

	/* Remember the current slot. */
	oldblock = b_bp;

	/* Special case block 0. */
	if (current == 0) {

		TRACEP("del_bl",
			sl_sout("***** zero FIRST block 0\n"));

		/* Zero the block but do not actually delete. */
		b_bp -> d_lines = 0;
		is_dirty(oldblock);

		/* Move to next block. */
		if (next != ERROR) {
			b_bp = swap_in(next);
		}
		b_start = b_line = 1;
	}
	else if (next != ERROR) {

		TRACEP("del_bl",
			sl_sout("***** free next block ");
			sl_iout(next); sl_cout('\n'));

		/* Move to the START of the next block. */
		b_bp    = swap_in(next);
		b_start = b_line;

		/* Adjust back pointer of new block. */
		b_bp -> d_back = back;
		is_dirty(b_bp);

		/* Adjust next pointer. */
		bp1 = swap_in(back);
		bp1 -> d_next = next;
		is_dirty(bp1);

		/* Actually delete the old block. */
		free_block(oldblock);
	}
	else if (back != ERROR) {

		TRACEP("del_bl",
			sl_sout("***** free previous block ");
			sl_iout(next); sl_cout('\n'));

		/*
			Move the the END of the previous block.
			Set bufatbot() true.
		*/
		b_bp     = swap_in(back);
		b_start -= b_bp -> d_lines;
		b_line   = b_start + b_bp -> d_lines;

		/* Adjust forward pointer of new block. */
		b_bp -> d_next = next;
		is_dirty(b_bp);

		/* Adjust pointer to the last block. */
		b_tail = back;

		/* Actually delete the old slot. */
		free_block(oldblock);
	}
	else {
		/* Only block 0 has both links == ERROR. */
		cant_happen("del_bl");
	}

	/* Adjust total number of lines. */
	b_max_line -= nlines;

	TICKX("del_bl");
}

/*
	Delete dlines from the current block.
	There is at least one line after the deleted lines.
*/
static void
del_mid(int dlines)
{
	char * source, * dest;
	int  i, length, limit, line, nlines, offset;

	TRACEPB("del_mid", sl_lpout(); sl_iout(dlines); sl_rpout());

	/* Compute some constants. */
	line   = b_line - b_start;
	nlines = b_bp -> d_lines;

	/* Compress the block. */
	source = b1_ptr(line + dlines);
	dest   = b1_ptr(line);
	length = b1_tab(nlines - 1) - b1_tab(line + dlines - 1);

	TRACEP("del_mid", sl_sout("***** compressing block with sysmove\n"));

	sysmove(source, dest, length);

	/* Compress the index table. */
	offset = b1_prefix(line + dlines) - b1_prefix(line);
	limit  = nlines - dlines;
	for (i = line; i < limit; i++) {
		b1_settab(i, b1_tab(i + dlines) - offset);
	}

	/* Adjust the counts. */
	b_bp -> d_lines -= dlines;
	b_max_line      -= dlines;

	TRACE("v", buftrace());

	TICKX("del_mid");
}

/*
	Delete the current line and all followings lines in
	the current block.
	slines is the number of preceeding lines in the block.
*/
static void
del_tail(int slines, int nlines)
{
	int next;

	TRACEPB("del_tail", sl_lpout();
		sl_iout(slines); sl_csout();
		sl_iout(nlines); sl_rpout());

	/* Adjust the line count. */
	b_bp -> d_lines = slines;
	is_dirty(b_bp);

	next = b_bp -> d_next;
	if (next != ERROR) {

		TRACEP("del_tail",
			sl_sout("***** current block becomes ");
			sl_iout(next); sl_cout('\n'));

		/* Move the current block forward. */
		b_bp    = swap_in(next);
		b_start = b_line;
	}

	/* Adjust the total number of lines. */
	b_max_line -= nlines;

	TICKX("del_tail");
}

/*
	Copy the current line from the buffer to line [].
	The size of line [] is linelen.
	Return k = the length of the line.
	If k > linelen then truncate k - linelen characters.
*/
int
bufgetln(char *line, int linelen)
{
	int	count, limit;
	char	*src;

	TRACEPB("bufgetln", sl_lpout();
		sl_pout(line);    sl_csout();
		sl_iout(linelen); sl_rpout());

	/* Return null line at the bottom of the buffer. */
	if (bufatbot()) {
		line [0] = '\n';
		RETURN_INT("bufgetln", 0);
	}

	/* Copy line to buffer. */
	src   = b1_ptr    (b_line - b_start);
	count = b1_length (b_line - b_start);
	limit = min(count, linelen - 1);
	sysmove(src, line, limit);
	
	/* End with '\n'. */
	line [limit] = '\n';

	/* Return the number of characters in the line. */

	RETURN_INT("bufgetln", count);
}

/*
	Insert line before the current line.  Thus, the line
	number of the current line does not change.  The line
	ends with '\n'.

	This is fairly crude code, as it can end up splitting
	the current block into up to three blocks.  However,
	the combine() routine does an effective job of keeping
	the size of the average block big enough.
*/
void
bufins(char insline [], int inslen)
{
	struct BLOCK *bp2;
	char *dest, *source;
	int  i, length, line, nlines, prefix;

	TRACEPB("bufins",  sl_lpout();
		sl_pout(insline); sl_csout(); sl_iout(inslen);
		sl_sout(") block: "); sl_iout(b_bp -> d_diskp);
		sl_sout(", line: ");  sl_iout(b_line);
		sl_cout('\n'));

	if (inslen > BUFF_SIZE) {
		cant_happen("bufins 1");
	}

	/* See if the new line will fit. */
	if (inslen + sizeof(int) > b1_avail()) {

		TRACEP("bufins", sl_sout("***** splitting block\n"));
		TRACE("v", trace_vars());

		/* Split off the trailing lines. */
		bp2 = split_block();

		/* See if there is enough room in the old block. */
		if (inslen + sizeof(int) > b1_avail()) {

			/* Move on to the next block. */
			b_start += b_bp -> d_lines;
			b_bp     = bp2;

			/*
			At this point,  the new line is the
			the first line of the block.  Alas,
			we may still have to split off the
			trailing lines so the new line will fit.
			*/

			if (inslen + sizeof(int) > b1_avail()) {

		TRACEP("bufins", sl_sout("***** splitting block again\n"));
		TRACE("v", trace_vars());

				/* Split the block a second time. */
				split_block();
			}
		}
	}

	if (inslen + sizeof(int) > b1_avail()) {
		cant_happen("bufins 2");
	}


	/*
		At this point,  we know that the new line can
		be inserted before the current line with room
		for any following lines.
	*/

	is_dirty(b_bp);
	line   = b_line - b_start;
	nlines = b_bp -> d_lines;

	if (nlines == 0) {

		TRACEP("bufins",
			sl_sout("***** copy line to empty block\n"));

		/* Copy line to empty block. */
		sysmove(insline, b_bp -> d_data, inslen);

		/* Create an index table. */
		b1_settab(0, inslen);
	}
	else if (line >= nlines) {

		TRACEP("bufins", sl_sout("***** copy line to end of block\n"));

		/* Copy line to end of block. */
		sysmove(insline,
			b_bp -> d_data + b1_tab(nlines - 1),
			inslen);

		/* Append index to index table. */
		b1_settab(nlines, b1_tab(nlines - 1) + inslen);
	}
	else {

		TRACEP("bufins",
		  sl_sout("***** make a hole at current line with sysmove\n"));

		/* Make a hole at the current line. */
		source = b1_ptr(line);
		dest   = b1_ptr(line) + inslen;
		prefix = b1_prefix(line);
		length = b1_tab(nlines - 1) - prefix;
		sysmove(source, dest, length);

		/* Copy the new line into the hole. */
		sysmove(insline, source, inslen);

		/* Make a hole in the index table. */
		for (i = nlines; i > line; i--) {
			b1_settab(i, b1_tab(i - 1) + inslen);
		}

		/* Copy the new index into the hole. */
		b1_settab(line, prefix + inslen);
	}
		
	/*
		Special case: inserting a null line at the
	 	end of the file is not a significant change.
	*/
	if ( ! (inslen == 0 && bufnrbot()) ) {
		b_cflag = TRUE;
	}

	/* Bump the counts. */
	b_bp -> d_lines++;
	b_max_line++;

	/*
		It DOES make sense to call combine here.
		In the most common case,  after a split the
		current block might be small enough to be
		combined with the preceding block.
	*/
	TRACEP("bufins", sl_sout("***** combine at end of insert\n"));
	TRACE("v", trace_vars());

	combine();

	/* Check the format of the block. */
	check_block("bufins");

	TRACE("v", buftrace());

	TICKX("bufins");
}


/*
	Combine the current block with the preceeding and
	following blocks if possible.
	Make the new block the current bloc.
*/
void
combine(void)
{
	TICKB("combine");

	combine1(b_bp -> d_back,  b_bp -> d_diskp);
	combine1(b_bp -> d_diskp, b_bp -> d_next);

	TICKX("combine");
}


/*
	Combine two blocks into one if possible.
	Make the new block the current block.

	Note that nline1 can be 0 because of the special case
	code in the del_bl() routine.  nline2 is never 0.
*/
static void
combine1(int diskp1, int diskp2)
{
	struct BLOCK *bp1, *bp2, *bp3;
	char	*source, *dest;
	int	i, length, limit, nlines1, nlines2, offset;

	SL_DISABLE();

	/* Make sure the call makes sense. */
	if (diskp1 == ERROR || diskp2 == ERROR) {
		return;
	}

	TRACEPB("combine1",  sl_lpout();
		sl_iout(diskp1); sl_csout();
		sl_iout(diskp2); sl_rpout());

	/* Get the two blocks. */
	TRACEP("combine1", sl_sout("***** swap in two blocks...\n"));

	bp1 = swap_in(diskp1);
	bp2 = swap_in(diskp2);

	if ( bp1 -> d_next != diskp2 ||
	     bp2 -> d_back != diskp1
	   ) {
		cant_happen("combine 1");
	}
	
	/* Do nothing if the blocks are too large. */
	if (b_avail(bp1) + b_avail(bp2) < BUFF_SIZE) {
		RETURN_VOID("combine1");
	}

	/* Compute the number of lines in each block. */
	nlines1 = bp1 -> d_lines;
	nlines2 = bp2 -> d_lines;
	if (nlines2 <= 0) {
		cant_happen("combine");
	}

	/* Copy buffer 2 to end of buffer 1. */

	TRACEP("combine1",
		sl_sout("***** copy buffer "); sl_iout(diskp1);
		sl_sout(" to end of buffer "); sl_iout(diskp2);
		sl_sout("with sysmove\n"));

	source = bp2 -> d_data;
	if (nlines1 == 0) {
		dest   = bp1 -> d_data;
		offset = 0;
	}
	else {
		dest   = bp1 -> d_data + b_tab(bp1, nlines1-1);
		offset = b_tab(bp1, nlines1 - 1);
	}
	length = b_tab(bp2, nlines2 - 1);
	sysmove(source, dest, length);

	/* Copy table 2 to table 1. */
	for (i = 0; i < nlines2; i++) {
		b_settab(bp1, i + nlines1,
			 b_tab(bp2, i) + offset);
	}

	/* Both blocks are now dirty. */
	is_dirty(bp1);
	is_dirty(bp2);

	/* Adjust the back pointer of the next block. */
	if (bp2 -> d_next != ERROR) {

		TRACEP("combine1",
		    sl_sout("***** swap in next block.  Adjust back ptr\n"));

		bp3 = swap_in(bp2 -> d_next);
		bp3 -> d_back = bp1 -> d_diskp;
		is_dirty(bp3);
	}

	/*
		Adjust the current block if needed.
	 	The value of b_start must be decremented
	 	by the OLD value of bp1 -> d_lines.
	*/

	if (b_bp == bp2) {
		b_bp     = bp1;
		b_start -= bp1 -> d_lines;
	}

	/* Adjust the header for block 1. */
	bp1 -> d_lines += bp2 -> d_lines;
	bp1 -> d_next   = bp2 -> d_next;

	/* Adjust the pointers to the last block. */
	if (diskp2 == b_tail) {
		b_tail = diskp1;
	}

	/* Slot 2 must remain in core until this point. */
	free_block(bp2);

	/* Check the format of the block. */
	check_block("combine");

	TICKX("combine1");
}

/*
	Put the block in the slot on the free list.
*/
static void
free_block(struct BLOCK * bp)
{
	TRACEPB("free_block",  sl_lpout(); sl_pout(bp); sl_rpout());

	/*
		Each block in the free list contains the
		pointer to the next block in the d_next field.
	*/
	bp -> d_next = b_free;
	b_free = bp -> d_diskp;

	/* Erase the block. */
	bp -> d_lines = 0;

	/* Make sure the block is rewritten. */
	is_dirty(bp);

	TICKX("free_block");
}

/*
	Create a new block linked after the current block.
	Return a pointer to the new block.
*/
static struct BLOCK *
new_block (void)
{
	struct BLOCK *bp1, *bp2;
	int diskp;

	TICKB("new_block");

	/* Get a free disk sector. */
	if (b_free != ERROR) {

		TRACEP("new_block",
			sl_sout("***** get new block from free list\n"));

		/* Take the first block on the free list. */
		diskp = b_free;

		/* Put the block in a free slot. */
		bp1 = swap_in(diskp);

		/* Adjust the head of the free list. */
		b_free = bp1 -> d_next;
	}	
	else {
		TRACEP("new_block",
			sl_sout("***** allocate brand new block \n"));

		/* Get a free slot. */
		diskp = ++b_max_diskp;
		bp1   = swap_new(diskp);
	}

	/* Link the new block after the current block. */
	bp1  -> d_next = b_bp -> d_next;
	bp1  -> d_back = b_bp -> d_diskp;
	b_bp -> d_next = diskp;
	if (bp1 -> d_next != ERROR) {

		TRACEP("new_block",
		    sl_sout("***** swap in to adjust back ptr in next block\n"));

		bp2 = swap_in(bp1 -> d_next);
		bp2 -> d_back = diskp;
		is_dirty(bp2);
	}
	
	/* The block is empty. */
	bp1 -> d_lines = 0;
	is_dirty(bp1);

	/* Return a pointer to the new block. */
	RETURN_PTR("new_block", bp1);
}

/*
	Split the current block before the current line.
	The current line, and following lines are put in
	a new block.
*/
static struct BLOCK *
split_block(void)
{
	struct	BLOCK *bp2;
	char	*dest, *source;
	int	nlines, nlines1, nlines2;
	int	i, length, line, offset;

	TRACEPB("split_block", sl_sout("***** create new block\n"));

	/* Create a new block. */
	bp2 = new_block();

	/* Mark both blocks as dirty. */
	is_dirty(b_bp);
	is_dirty(bp2);

	/*
		Count the lines in each block.
		Adjust nline1 and nlines1 if bufatbot().	
	*/
	line    = b_line - b_start;
	nlines  = b_bp -> d_lines;
	nlines1 = min(nlines, b_line - b_start);
	nlines2 = max(0, nlines - nlines1);

	/* Copy data area to new block. */
	if (!bufatbot()) {

		TRACEP("split_block",
		    sl_sout("***** copy data to new block with sysmove\n"));

		offset = b1_prefix(line);
		source = b_bp -> d_data + offset;
		dest   = bp2  -> d_data;
		length = b1_tab(nlines - 1) - offset;
		sysmove(source, dest, length);
	}

	/* Copy index to new block. */
	for(i = 0; i < nlines2; i++) {
		b_settab(bp2, i, b1_tab(i + nlines1) - offset);
	}

	/* Adjust the headers. */
	bp2  -> d_lines = nlines2;
	b_bp -> d_lines = nlines1;

	/* Adjust the pointer to the last block. */
	if (b_bp -> d_diskp == b_tail) {
		b_tail = bp2 -> d_diskp;
	}

	/* Return a pointer to the new block. */
	RETURN_PTR("split_block", bp2);
}
