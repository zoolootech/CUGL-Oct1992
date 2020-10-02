/*
	RED buffer routines -- Full C version
	Part 3 -- file routines

	Source:  redbuf3.c
	Version: August 8, 1986; January 18, 1990.

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
static void	disk_seek	(void);
static int	read1		(void);
static void	read2		(void);
extern void	write1		(char c);
extern void	wr_flush	(void);

/*
	Data buffer used only in this file.
*/
static char b_buff [DATA_SIZE];

/*
	Kludge to allow more strict checking in swap_out.
	It is usually an internal error if we swap out the current block.
	The exception is in write_file, where we swap out all blocks.
*/
static int ok2swap = FALSE;

/*
	Open the data file.
*/
int
data_open(void)
{
	TICKB("data_open");

	/* Erase the data file if it exists. */
	sysunlink(DATA_FILE);

	/* Create the data file. */
	b_data_fd = syscreat(DATA_FILE);
	if (b_data_fd == ERROR) {
		disk_error("Can not open swap file.");
	}

	RETURN_INT("data_open", b_data_fd);
}

/*
	Make the slot the MOST recently used slot.
*/
void
do_lru(struct BLOCK *bp)
{
	struct BLOCK *bp1;
	int i, lru;

	SL_DISABLE();
	
	/*
		Change the relative ordering of all slots
	 	which have changed more recently than slot.
	 */
	lru = bp -> d_lru;

	/* 12/15/89:  do nothing if the current slot is the most recent. */
	if (lru == 0) {
		return;
	}

	TRACEP("do_lru", sl_lpout();
		sl_pout(bp); sl_sout(") before: "); dump_slots());

	for (i = 0; i < DATA_RES; i++) {
		bp1 = b_bpp [i];
		if (bp1 -> d_lru < lru) {
			bp1 -> d_lru++;
		}
	}

	/* The slot is the most recently used. */
	bp -> d_lru = 0;

	TRACE("do_lru", sl_sout("after: "); dump_slots(); sl_cout('\n'));
}

/*
	Disk error routines
*/
void
disk_error(char *message)
{
	TRACEPB("disk_error",  sl_lpout(); sl_sout(message); sl_rpout());

	error(message);

	/* Clear the buffer if no recovery is possible. */
	if (b_fatal == TRUE) {
		bufnew();
	}

	/* Abort the operation that caused the error. */
	longjmp(DISK_ERR, ERROR);

	TICKX("disk_error");
}

void
disk_full(void)
{
	SL_DISABLE();

	disk_error("Disk or directory full?");
}

void
disk_rdy(void)
{
	SL_DISABLE();

	disk_error("Drive not ready?");
}

static void
disk_seek(void)
{
	SL_DISABLE();

	disk_error("Bad Seek");
}

/*
	Indicate that a slot must be saved on the disk.
*/
#if 0 /* a macro now */
void
is_dirty(struct BLOCK *bp)
{
	TRACEPB("is_dirty",  sl_lpout(); sl_pout(bp); sl_rpout());

	bp -> d_status = DIRTY;

	TICKX("is_dirty");
}
#endif /* 0 */

/*
	Put out the block-sized buffer to the disk sector.
*/
void
put_block(struct BLOCK *bp, int diskp)
{
	int s;

	/* Make sure blocks are written in order. */

	TRACEPB("put_block",  sl_lpout();
		sl_pout(bp);    sl_csout();
		sl_iout(diskp); sl_rpout());

	if (diskp > b_max_put + 1) {
		swap_sync(b_max_put + 1, diskp - 1);
	}
	b_max_put = max(b_max_put, diskp);
	
	/* Seek to the correct sector of the data file. */
	s = sysseek(b_data_fd, diskp);
	if (s == -1) {
		disk_seek();
	}

	/* Write the block to the data file. */
	if (syswrite(b_data_fd, (char *) bp, DATA_SIZE) != DATA_SIZE) {
		disk_full();
	}

	TICKX("put_block");
}

/*
	Fill in the header fields of the output buffer and
	write it to the disk.
	avail is the number of free characters in the buffer.
*/
char *
put_buf(int avail)
{
	struct BLOCK *bp;

	/*
		Fill in the back and next links immediately.
		This can be done because we are not waiting
		for the LRU algorithm to allocated disk blocks.
		The last block that put_buf() writes will have
		an incorrect next link.  Read_file() will make
		the correction.
	*/

	TRACEPB("put_buf", sl_lpout(); sl_iout(avail); sl_rpout());

	bp = (struct BLOCK *) b_buff;
	bp -> d_back  = b_max_diskp - 1;
	bp -> d_next  = b_max_diskp + 1;
	bp -> d_lines = b_line - b_start;

	if (avail < 0) {
		cant_happen("put_buf");
	}

	/* Update block and line counts. */
	b_max_diskp++;
	b_start = b_line;

	/* Write the block. */
	put_block( (struct BLOCK *) b_buff, b_max_diskp - 1);

	TICKX("put_buf");
}

/*
	Write out the slot to the data file.
*/
void
put_slot(struct BLOCK *bp)
{
	TRACEPB("put_slot", sl_lpout(); sl_pout(bp); sl_rpout());

	if (bp -> d_diskp == ERROR) {
		cant_happen("put_slot");
	}
	put_block(bp, bp -> d_diskp);

	TICKX("put_slot");
}

/*
	Read a file into the buffer.

	This version of read_file puts an index table at
	the end of each block.  The index table's entry
	for each line tells the distance of the LAST character
	of the line from the start of the data buffer.

	The global variables br_count, br_bufp, and br_bufc
	are used to communicate with read1().  Using these
	variables speeds the code by a factor of 3!

	The "global" variables br_avail and br_out are used
	only by read_file() -- again, purely to speed the code.
*/
void
read_file(char file_name [])
{
	struct BLOCK *bp;

	/* global:  char * br_bufp   pointer to buffer	*/
	/* global:  int    br_bufc   index into buffer	*/
	/* global:  int    br_count  number of buffer	*/

	/* global:  int    br_avail  available chars	*/
	/* global:  int    br_out    index into outbuf	*/

	char	*outbuf;	/* the output buffer	*/
	int	out_save;	/* line starts here	*/
	int	c, i, j;

	/* Clear the swapping buffers and the files. */

	TRACEPB("read_file", sl_lpout(); sl_pout(file_name); sl_rpout());

	bufnew();
	b_bp -> d_status = FREE;

	/* Open the user file. */
	b_user_fd = sysopen(file_name);
	if (b_user_fd == ERROR) {
		disk_error("File not found.");
	}

	/* Clear the buffer on any disk error. */
	b_fatal = TRUE;

	/* Open the data file. */
	data_open();

	/* The file starts with line 1. */
	b_line = 1;
	b_start = 1;

	/* There are no blocks in the file yet. */
	b_head = b_tail = ERROR;
	b_max_diskp = 0;

	/* Point outbuf to start of the output data area. */
	outbuf = b_buff + HEADER_SIZE;

	/* Force an initial read in read1(). */
	br_count = DATA_SIZE;
	br_bufc  = DATA_RES;

	/* Zero the pointers into the output buffer. */
	br_out = out_save = 0;

	/* Allocate space for the first table entry. */
	br_avail = BUFF_SIZE - sizeof(int);
	
	/* Set the current line counts. */
	b_line = b_start = 1;

	for(;;) {

		if (br_avail <= 0 && out_save == 0) {
			/* The line is too long. */
			error ("Line split.");

			/* End the line. */
			b_settab( (struct BLOCK *) b_buff,
				  b_line - b_start,
				  br_out
				);
			b_line++;

			/* Clear the output buffer. */
			put_buf(br_avail);
			br_out = out_save = 0;
			br_avail = BUFF_SIZE - sizeof(int);
		}

		else if (br_avail <= 0) {

			/*
				Deallocate last table entry and
				reallocate space used by the
				partial line.
			*/
			br_avail += (sizeof(int) + br_out - out_save);

			/* Write out the buffer. */
			put_buf(br_avail);

			/* Move the remainder to the front. */
			sysmove(outbuf + out_save,
				outbuf,
				br_out - out_save);

			/* Reset restart point. */
			br_out   = br_out - out_save;
			out_save = 0;
			br_avail = BUFF_SIZE - sizeof(int) - br_out;
		}

		c = read1();

		if (c == EOF_MARK) {

			if (br_out != out_save) {

				/* Finish the last line. */
				b_settab( (struct BLOCK *) b_buff,
					  b_line-b_start,
					  br_out	/* 3/8/85 */
					);
				b_line++;
				out_save = br_out;
			}
			else {
				/* No last line after all. */
				br_avail += sizeof(int);
			}

			/* bug fix:  2/20/84, 4/2/84 */
			if (br_avail !=  BUFF_SIZE) {
				put_buf(br_avail);
			}
			break;
		}

		else if (c == '\n') {

			/* Finish the line. */
			b_settab( (struct BLOCK *) b_buff,
				  b_line - b_start,
				  br_out
				);
			br_avail -= sizeof(int);

			/* Set restart point. */
			b_line++;
			out_save = br_out;
		}

		else if (c == '\r') {
			/* Ignore CP/M's pseudo-newline. */
			continue;
		}

		else {

			/* Copy normal character. */
			outbuf [br_out++] = c;
			br_avail--;
		}
	}

	/* Close the user' file. */
	sysclose(b_user_fd);

	/* Special case:  null file. */
	if (b_max_diskp == 0) {
		bufnew();
		RETURN_VOID("read_file");
	}

	/* Rewrite the last block with correct next field. */
	bp = (struct BLOCK *) b_buff;
	bp -> d_next = ERROR;
	put_block( (struct BLOCK *) b_buff, b_max_diskp - 1);

	/* Set the pointers to the first and last blocks. */
	b_max_diskp--;
	b_head = 0;
	b_tail = b_max_diskp;

	/*
		Clear all slots.  This is REQUIRED since
		read_file has just overwritten all slots.
	*/
	buf_clr();

	/* Move to the start of the file. */
	b_max_line = b_line - 1;
	b_line = 1;
	b_start = 1;
	b_bp = swap_in(b_head);

	b_fatal = FALSE;

	TICKX("read_file");
}

/*
	Get one character from the input file.

	This version of read1 uses all slots as an input buffer.
	The slots to not need to be contiguous in memory
	(which they are generally are NOT because of hidden
	header information used only by sysalloc()).

	This version uses the globals br_count, br_bufc and
	br_bufp to speed up the code.
*/
static int
read1(void)
{
	TICKB("read1");

	if (br_count == DATA_SIZE) {

		if (br_bufc >= DATA_RES - 1) {

			/* Read into buffers. */
			read2();
			br_count = br_bufc = 0;
		}
		else {

			/* Switch to next buffer. */
			br_bufc++;
			br_count = 0;
		}
		br_bufp = (char *) b_bpp [br_bufc];
	}

	/* Get the character and mask off parity bit. */

	RETURN_INT("read1", br_bufp [br_count++] & 0x7f);
}

/*
	Read user file into all slots.
*/
static void
read2(void)
{
	int i, s;

	TICKB("read2");

	for (i = 0; i < DATA_RES; i++) {

		/* Point at the next slot. */
		br_bufp = (char *) b_bpp [i];

		/* Read the next sector. */
		s = sysread(b_user_fd, br_bufp);

		if (s == ERROR) {
			disk_rdy();
		}

		/* Force a CPM end of file mark. */
		if (s < DATA_SIZE) {
			br_bufp [s] = EOF_MARK;
			break;
		}
	}

	TICKX("read2");
}

/*
	Swap out all dirty blocks.
*/
void
swap_all(void)
{
	struct BLOCK *bp;
	int i;

	TICKB("swap_all");

	for (i = 0; i < DATA_RES; i++) {
		bp = b_bpp [i];
		if (bp -> d_status == DIRTY) {
			put_slot (bp);
			bp -> d_status = FULL;
		}
	}

	TICKX("swap_all");
}

/*
	Swap out the first dirty block.   This routine does
	not swap the dirty block since that would waste time.
	This routines is called when nothing else is happening.
*/
void
swap_one(void)
{
	struct BLOCK *bp;
	int i;

	SL_DISABLE();

	for (i = 0; i < DATA_RES; i++) {
		bp = b_bpp [i];
		if (bp != b_bp && bp -> d_status == DIRTY) {
			put_slot (bp);
			bp -> d_status = FULL;
			break;
		}
	}
}

/*
	Get the block from the disk into a slot in memory.
	Return a pointer to the block.
*/
struct BLOCK *
swap_in(int diskp)
{
	struct BLOCK *bp;
	int i, status;

	SL_DISABLE();

	if (diskp < 0 || diskp > b_max_diskp) {
		cant_happen("swap_in 1");
	}

	/* See whether the block is already in a slot. */
	for (i = 0; i < DATA_RES; i++) {
		bp = b_bpp [i];
		if (bp -> d_status != FREE &&
		    bp -> d_diskp  == diskp) {

			/* Reference the block. */
			STATB("swap_in");
			do_lru(bp);
			STATX("swap_in");

			/* Point to the slot. */
			return bp;
		}
	}

	/* Clear a slot for the block. */
	bp = swap_new(diskp);

	/* Read from temp file to block. */
	status = sysseek(b_data_fd, diskp);
	if (status == -1) {
		disk_rdy();
	}

	/* Read the block into the slot. */
	status = sysread(b_data_fd, (char *) bp);
	if (status == ERROR) {
		disk_rdy();
	}

	/* Swap_new() has already called do_lru(). */
	TRACEP("swap_in",  sl_lpout();
		sl_iout(diskp); sl_sout(") ");
		dump_slots();
		sl_sout("returns "); sl_pout(bp); sl_cout('\n'));

	/* Return a pointer to the block. */
	return bp;
}

/*
	Free a slot for a block located at diskp.
	Swap out the least recently used block if required.
	Return a pointer to the block.
*/
struct BLOCK *
swap_new(int diskp)
{
	struct BLOCK *bp;
	int i;

	SL_DISABLE();

	/* Search for an available slot. */
	for (i = 0; i < DATA_RES; i++) {
		bp = b_bpp [i];
		if (bp -> d_status == FREE) {
			break;
		}
	}

	/* Swap out a block if all blocks are full. */
	if (i == DATA_RES) {
		bp = swap_out();
	}

	/* Make sure the block will be written. */
	bp -> d_status = FULL;
	bp -> d_diskp  = diskp;

	/* Reference the slot. */
	do_lru(bp);

	/* Return a pointer to the slot. */
	TRACEP("swap_new",      sl_lpout();
		sl_iout(diskp); sl_sout(") returns ");
		sl_pout(bp);    sl_cout('\n'));

	return bp;
}

/*
	Swap out the least recently used (LRU) slot.
	Return a pointer to the block.
*/
struct BLOCK *
swap_out(void)
{
	struct BLOCK *bp;
	int i;

	SL_DISABLE();

	/* Open the temp file if it has not been opened. */
	if (b_data_fd == ERROR) {
		b_data_fd = data_open();
	}

	/* Find the least recently used slot. */
	for (i = 0; ;i++) {
		bp = b_bpp [i];
		if (bp -> d_lru == DATA_RES - 1) {
			break;
		}
	}

	TRACEP("swap_out",
		sl_sout("***** swapping out block ");
		sl_iout(bp -> d_diskp);
		sl_sout(", returns "); sl_pout(bp);
		sl_cout('\n'));

	if (bp == b_bp && !ok2swap) {
		cant_happen("swap_out1");
	}

	/* Do the actual swapping out if memory is dirty. */
	if (bp -> d_status == DIRTY) {
		put_slot(bp);
		return bp;
	}

	/* d_diskp is not ERROR if status is not DIRTY. */
	if (bp -> d_diskp == ERROR) {
		cant_happen("swap_out");
	}

	/* Indicate that the slot is available. */
	bp -> d_status = FREE;
	bp -> d_diskp  = ERROR;

	/* Return a pointer to the block. */
	return bp;
}

/*
	Swap out blocks found between low and high on the disk.
*/
void
swap_sync(int low, int high)
{
	struct BLOCK *bp;
	int disk, i;

	TRACEPB("swap_sync",   sl_lpout();
		sl_iout(low);  sl_csout();
		sl_iout(high); sl_rpout());

	/* Search the slot table for each disk. */
	for (disk = low; disk <= high; disk++) {
		for (i = 0; i < DATA_RES; i++) {
			bp = b_bpp [i];
			if (bp -> d_diskp == disk) {

				/* Write the slot. */
				put_slot(bp);
				bp -> d_status = FULL;
				break;
			}
		}
		if (i == DATA_RES) {
			cant_happen("swap_sync");
		}
	}

	TICKX("swap_sync");
}

/*
	Write the entire buffer to file.
*/
void
write_file(char *file_name)
{
	/* global:  bw_count */

	struct BLOCK *bp;
	char *data;
	int slot, line, nlines, length, next, count;
	int c;
        int blocks;
	
	TRACEPB("write_file", sl_lpout(); sl_sout(file_name); sl_rpout());

	/* Open the user file.  Erase it if it exists. */
	b_user_fd = syscreat(file_name);
	if (b_user_fd == ERROR) {
		disk_full();
	}

	/* Allow the current block to be swapped out. */
	ok2swap = TRUE;

	/* Copy each block of the file. */
	bw_count = 0;
	blocks   = 0;
	for (next = b_head; next != ERROR; ) {

		/* One more check for file consistency. */
		if (blocks++ > b_max_diskp) {
			cant_happen("write_file1");
		}

		/* Swap in the next block. */
		bp = swap_in(next);

		/* Get data from the header of the block. */
		next   = bp -> d_next;
		nlines = bp -> d_lines;
		data   = bp -> d_data;
		
		/* Copy each line of the block. */
		count = 0;
		for (line = 0; line < nlines; line++) {

			/* Get length of the line. */
			if (line == 0) {
				length = b_tab(bp, line);
			}
			else {
				length = b_tab(bp,line) -
					 b_tab(bp,line - 1);
			}
			
			/* Copy each char of the line. */
			for (; length; length--) {
				c = data [count++];
				write1(c);
			}

			/* Add end-of-line characters at end. */
			write1('\r');
			write1('\n');
		}
	}

	/* Force an end of file mark. */
#ifdef CPM
	write1(EOF_MARK);
#endif

	/* Flush the buffer and close the file. */
	wr_flush();
	sysclose(b_user_fd);

	/* Kludge:  go to line 1 for a reference point. */
	b_bp   = swap_in(b_head);
	b_line = b_start = 1;

	ok2swap = FALSE;

	TICKX("write_file");
}

/*
	Write one character to the user's file.
	bw_count is the current position in the file buffer.
*/
static void
write1(char c)
{
	TRACEPB("write1", sl_lpout(); sl_cout(c); sl_rpout());

	b_buff [bw_count++] = c;
	if (bw_count == DATA_SIZE) {
		if (syswrite(b_user_fd, b_buff, DATA_SIZE) != DATA_SIZE) {
			disk_full();
		}
		bw_count = 0;
	}

	TICKX("write1");
}

/*
	Flush b_buff to the user's file.
*/
static void
wr_flush(void)
{
	TICKB("wr_flush");

	if (bw_count == 0) {
		RETURN_VOID("wr_flush");
	}
	/* 3/28/86 */
	if (sysflush(b_user_fd, b_buff, bw_count) != 1) {
		disk_full();
	}

	TICKX("wr_flush");
}
