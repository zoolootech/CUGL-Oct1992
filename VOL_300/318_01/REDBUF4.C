/*
	RED buffer routines -- Full C version
	Part 4 -- debugging routines.

	Source:  redbuf4.c
	Version: December 3, 1984; January 18, 1990;

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
#define PRINTER stdout

/*
	Define output file for sherlock.
*/
static FILE * sl_file = 0L;

/*
	The XTRACE macro is used to eliminate traces when Sherlock
	is not being used.
*/
#undef XTRACE

#ifdef SHERLOCK
#define XTRACE(a) a
#else
#define XTRACE(a)
#define sl_cout(c)	putchar(c)
#define sl_csout()	printf(", ");
#define sl_iout(i)	printf("%d",i)
#define sl_pout(p)	printf("%p",p)
#define sl_sout(s)	printf("%s",s)
#endif

/*
	The observer effect:  None of the routines of this file
	should ever call swap_in() because swap_in() causes
	all kinds changes to the data structures which these
	routines are trying to print out.
*/

/*
	Dump global variables, all resident slots and
	the current block.
*/
void
bufdump(void)
{
	SL_DISABLE();

	outxy(0,SCRNL1);
	dump_vars();
	dump_memory();
	dump_block(b_bp);

	pmtzap();
	pmtupd();
	syscin();
	pmtzap();
	edclr();
	edgo(bufln(), 0);
}

/*
	The following routine is used only if Sherlock is used.
	It produces too much output to send to the screen.
*/
#ifdef SHERLOCK 

void
buftrace(void)
{
	struct BLOCK * bp;
	int i;

	SL_DISABLE();

	sl_sout("\n\nbuftrace...\n");
	trace_vars();
	trace_memory();

	for (i = 0; i < DATA_RES; i++) {
		bp = b_bpp[i];
		if (bp != 0L) {
			trace_block(b_bpp[i]);
		}
	}
}

#endif /* SHERLOCK */

/*
	System error routine.
*/
void
cant_happen(char *message)
{
	SL_DISABLE();

	pmtmess	(message, ":can't happen");
	XTRACE(	sl_sout(message);
		sl_sout(":cant_happen");
		sl_cout('\n'));

	syscin();
	syscin();

	XTRACE(buftrace());

	if (sl_file != 0L) {
		fclose(sl_file);
	}
	exit(1);
}

/*
	Check the current block for consistency.
*/
void
check_block(char *message)
{
	int avail, i, nlines, total;

	SL_DISABLE();

	nlines = b_bp -> d_lines;
	avail  = b1_avail();

	if (b_bp == (struct BLOCK *) ERROR ||
	    b_line < 0 || b_line > b_max_line + 1) {

		error("In check block 1.");
		XTRACE(sl_sout("In check block 1\n"));
		cant_happen(message);
	}

	if ( nlines < 0 ||
	     nlines >= BUFF_SIZE ||
	     avail  < 0
	   ) {

		error("In Check block 2.");
		XTRACE(sl_sout("In check block 2\n"));
		cant_happen(message);
	}

	/* Make sure there are at least enough lines. */
	for (i = 0; i < nlines; i++) {
		total = b1_tab(i);
		if (total < 0 || total > BUFF_SIZE - avail) {

			error("In check block 3.");
			XTRACE(sl_sout("In check block 3\n"));
			cant_happen(message);
		}
	}
}

/*
	Dump the current block.
*/
void
dump_block(struct BLOCK *bp)
{
	char *buffer;
	int  c, count, i, j, limit, nlines, offset;

	SL_DISABLE();

	XTRACE(	sl_sout("\n\ndump of block in slot ");
		sl_iout(bp -> d_diskp); sl_sout("\n\n"));

	nlines = bp -> d_lines;
	buffer = bp -> d_data;
        offset = 0;
	
	for (i = 0; i < nlines; i++) {

		limit = b_tab(bp, i);
		count = limit - offset;

		fprintf(PRINTER,
		"line %3d, offset %3d, length %3d, total %3d:  ",
		i + 1, offset, count, limit);

		XTRACE(	sl_sout("line ");	sl_iout(i+1);
			sl_sout(", offset ");	sl_iout(offset);
			sl_sout(", length ");	sl_iout(count);
			sl_sout(", total ");	sl_iout(limit);
			sl_sout(": "));

		if (count < 0) {
			return;
		}

		if (count >= 30) {
			fprintf(PRINTER, "     \n");
			XTRACE(sl_cout('\n'));
		}
		for (j = 0; j < count && j < 80; j++) {
			c = buffer [offset + j] & 127;
			if (c == '\t') {
				fprintf(PRINTER, " ");
				XTRACE(sl_sout(" "));
			}
			else if (c < 32) {
				fprintf(PRINTER,"^%c", c + 64);
				XTRACE(	sl_cout('^');
					sl_cout(c + 64));
			}
			else {
				fprintf(PRINTER, "%c", c);
				XTRACE(sl_cout(c));
			}
		}
		fprintf(PRINTER, "     \n");
		XTRACE(sl_cout('\n'));

		offset = limit;
	}
}

/*
	Dump all the resident slots.
*/
void
dump_memory(void)
{
	struct BLOCK *bp;
	int i;

	SL_DISABLE();

	XTRACE(sl_sout("\n\nfull dump of slots\n\n"));

	for (i = 0; i < DATA_RES; i++) {
		bp = b_bpp [i];

		fprintf(PRINTER, "slot %2d, ", i);
		XTRACE(sl_sout("slot "); sl_iout(i); sl_csout());

		fprintf(PRINTER,
		"address %p, back %3d, diskp %3d, next %3d     \n",
		bp, bp -> d_back, bp -> d_diskp, bp -> d_next);

		XTRACE(	sl_sout("address ");	sl_pout(bp);
			sl_sout(", back ");	sl_iout(bp->d_back);
			sl_sout(", diskp ");	sl_iout(bp->d_diskp);
			sl_sout(", next ");	sl_iout(bp->d_next);
			sl_cout('\n'));

		fprintf(PRINTER,
		"lines %3d, lru %3d, status %3d, avail %3d     \n",
		bp -> d_lines, bp -> d_lru,  bp -> d_status,
		b_avail(bp));

		XTRACE(	sl_sout("lines ");	sl_iout(bp->d_lines);
			sl_sout(", lru ");	sl_iout(bp->d_lru);
			sl_sout(", status ");	sl_iout(bp->d_status);
			sl_sout(", avail ");	sl_iout(b_avail(bp));
			sl_cout('\n'));
	}
}

/*
	Dump a brief description of the slots.
*/
void
dump_slots(void)
{
	struct BLOCK *bp1;
	int i;

	SL_DISABLE();

	XTRACE(
		for (i = 0; i < DATA_RES; i++) {
			bp1 = b_bpp[i];
			sl_iout(bp1->d_diskp);	sl_cout('(');
			sl_iout(bp1->d_lru);
			/* Limit of 10 per line. */
			if ( (i%10) == 9) {
				sl_sout(")\n");
			}
			else {
				sl_sout("), ");
			}
		}
	);
}

/*
	Dump all global variables.
*/
void
dump_vars(void)
{
	SL_DISABLE();

	XTRACE(sl_sout("\n\nDump of globals\n\n"));

	fprintf(PRINTER,
	"start %d, line %d, maxline %d     \n",
	b_start, b_line, b_max_line);

	XTRACE(	sl_sout("start ");	sl_iout(b_start);
		sl_sout(", line ");	sl_iout(b_line);
		sl_sout(", maxline ");	sl_iout(b_max_line);
		sl_cout('\n'));

	fprintf(PRINTER,
	"head %d, tail %d, free %d, max_diskp %d     \n",
	b_head, b_tail, b_free, b_max_diskp);

	XTRACE(	sl_sout("head ");	sl_iout(b_head);
		sl_sout(", tail ");	sl_iout(b_tail);
		sl_sout(", free ");	sl_iout(b_free);
		sl_sout(", max_diskp ");sl_iout(b_max_diskp);
		sl_cout('\n'));

	fprintf(PRINTER,
	"address %p, back %d, diskp %d, next %d, avail %d     \n",
	b_bp, b_bp -> d_back,  b_bp -> d_diskp,
	b_bp -> d_next, b1_avail());

	XTRACE(	sl_sout("address ");	sl_pout(b_bp);
		sl_sout(", back ");	sl_iout(b_bp->d_back);
		sl_sout(", diskp ");	sl_iout(b_bp->d_diskp);
		sl_sout(", next ");	sl_iout(b_bp->d_next);
		sl_sout(", avail ");	sl_iout(b1_avail());
		sl_cout('\n'));

	fprintf(PRINTER,
	"lines %d, lru %d, status %d     \n\n",
	b_bp -> d_lines, b_bp -> d_lru,  b_bp -> d_status);

	XTRACE(	sl_sout("lines ");	sl_iout(b_bp->d_lines);
		sl_sout(", lru ");	sl_iout(b_bp->d_lru);
		sl_sout(", status ");	sl_iout(b_bp->d_status);
		sl_cout('\n');		sl_cout('\n'));
}

/*
	All Sherlock output comes here.

	For those of you with Sherlock, comment out the version
	of this routine in sherlock.c.

	The reason this routine is needed is so that Sherlock output
	can be sent to a file instead of to stdout.
*/
static int cout_inited = FALSE;

#ifdef SHERLOCK

void
sl_cout(char c)
{

	if (!cout_inited) {
		cout_inited = TRUE;

		sl_file = sysfcreat("trace");
		if (sl_file == 0L) {
			disk_error("Can not create trace");
		}
	}

	/* Do not call sysfputc or we will get into an infinite recursion. */
	if (sl_file != 0L) {
		fputc(c, sl_file);
	}
}

#endif

/*
	Same as dump_block, except to Sherlock output.
*/
void
trace_block(struct BLOCK *bp)
{
	char *buffer;
	int  c, count, i, j, limit, nlines, offset;

	SL_DISABLE();

	XTRACE(	sl_sout("\n\ndump of disk block ");
		sl_iout(bp -> d_diskp); sl_sout("\n\n"));

	nlines = bp -> d_lines;
	buffer = bp -> d_data;
        offset = 0;
	
	for (i = 0; i < nlines; i++) {

		limit = b_tab(bp, i);
		count = limit - offset;

		XTRACE(	sl_sout("line ");	sl_iout(i+1);
			sl_sout(", offset ");	sl_iout(offset);
			sl_sout(", length ");	sl_iout(count);
			sl_sout(", total ");	sl_iout(limit);
			sl_sout(": "));

		if (count < 0) {
			return;
		}
		for (j = 0; j < count && j < 80; j++) {
			c = buffer [offset + j] & 127;
			if (c == '\t') {
				XTRACE(sl_sout(" "));
			}
			else if (c < 32) {
				XTRACE(	sl_cout('^');
					sl_cout(c + 64));
			}
			else {
				XTRACE(sl_cout(c));
			}
		}
		XTRACE(sl_cout('\n'));

		offset = limit;
	}
}

/*
	Dump all the resident slots.
*/
#ifdef SHERLOCK

void
trace_memory(void)
{
	struct BLOCK *bp;
	int i;

	SL_DISABLE();

	XTRACE(sl_sout("\n\nfull dump of slots\n\n"));

	for (i = 0; i < DATA_RES; i++) {
		bp = b_bpp [i];

		XTRACE(sl_sout("slot "); sl_iout(i); sl_csout());

		XTRACE(	sl_sout("address ");	sl_pout(bp);
			sl_sout(", back ");	sl_iout(bp->d_back);
			sl_sout(", diskp ");	sl_iout(bp->d_diskp);
			sl_sout(", next ");	sl_iout(bp->d_next);
			sl_cout('\n'));

		XTRACE(	sl_sout("lines ");	sl_iout(bp->d_lines);
			sl_sout(", lru ");	sl_iout(bp->d_lru);
			sl_sout(", status ");	sl_iout(bp->d_status);
			sl_sout(", avail ");	sl_iout(b_avail(bp));
			sl_cout('\n'));
	}
}

#endif /* SHEROCK */

/*
	Dump all global variables.
*/
#ifdef SHERLOCK

void
trace_vars(void)
{
	SL_DISABLE();

	XTRACE(sl_sout("\n\nDump of globals\n\n"));

	XTRACE(	sl_sout("start ");	sl_iout(b_start);
		sl_sout(", line ");	sl_iout(b_line);
		sl_sout(", maxline ");	sl_iout(b_max_line);
		sl_cout('\n'));

	XTRACE(	sl_sout("head ");	sl_iout(b_head);
		sl_sout(", tail ");	sl_iout(b_tail);
		sl_sout(", free ");	sl_iout(b_free);
		sl_sout(", max_diskp ");sl_iout(b_max_diskp);
		sl_cout('\n'));

	XTRACE(	sl_sout("address ");	sl_pout(b_bp);
		sl_sout(", back ");	sl_iout(b_bp->d_back);
		sl_sout(", diskp ");	sl_iout(b_bp->d_diskp);
		sl_sout(", next ");	sl_iout(b_bp->d_next);
		sl_sout(", avail ");	sl_iout(b1_avail());
		sl_cout('\n'));

	XTRACE(	sl_sout("lines ");	sl_iout(b_bp->d_lines);
		sl_sout(", lru ");	sl_iout(b_bp->d_lru);
		sl_sout(", status ");	sl_iout(b_bp->d_status);
		sl_cout('\n');		sl_cout('\n'));
}

#endif /* SHERLOCK */
