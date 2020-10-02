/*
	RED--Definitions of all global variables.

	Source:  redglb.c
	Version: November 18, 1983; August 8, 1986; January 18, 1990.

	Written by
	
		Edward K. Ream
		166 N. Prospect
		Madison WI 53705
		(608) 257-0802


	PUBLIC DOMAIN SOFTWARE

	This software is in the public domain.

	See red.h for a disclaimer of warranties and other information.
*/

/*
	Include red.h instead of just redbuf.h to get rid of the necessity
	for multiple definitions of SYSFNMAX.
*/

#include "red.h"

/*
	Define global file name.
*/
char g_file [SYSFNMAX];		/* file name for (re)save */

/*
	Define the global recovery point for disk errors.
*/
jmp_buf DISK_ERR;

/*
	Define globals used by the 'again' command.
*/
int	a_start, a_end, again_flag, a_rev;
int	a_rflag, a_wflag, a_cflag;
char	a_spat [MAXLEN1], a_rpat [MAXLEN1];

/*
	fmtcol[i] is the first column at which buf[i] is printed.
	fmtsub() and fmtlen() assume fmtcol[] is valid on entry.
*/
int fmtcol[MAXLEN1];

/*
	Define globals used to describe the terminal.
	At present,  they are set by the outinit routine,
	but the sysinit routine would be a better place if
	you want to support multiple terminals without
	recompiling RED.
*/
int	hasdn;	 /* has scroll down			*/
int	hasup;	 /* has scroll up			*/
int	hasins;	 /* has insert line (ABOVE current line)*/
int	hasdel;	 /* has delete line			*/
int	hasint;	 /* use interrupt driven screen		*/
int	hascol;	 /* put columns on prompt line		*/
int	haswrap; /* enables line wrapping		*/
int	hasword; /* restricts matches to word boundaries*/

/*
	Define the current location of the cursor.
*/
int	outx;
int	outy;

/*
	Allocate memory for variables global to the
	buffer routines.
*/
int	DATA_RES;	/* pseudo constant		*/
			/* no greater than MAX_RES	*/

int	b_fatal;	/* clear buffer on error	*/
int	b_cflag;	/* buffer changed flag		*/

int	b_line;		/* current line number		*/
int	b_max_line;	/* highest line number		*/
int	b_start;	/* first line of current block	*/

int	b_head;		/* # of first disk block	*/
int	b_tail;		/* # of last disk block		*/

int	b_max_diskp;	/* last block allocated		*/
int	b_max_put;	/* last block written		*/

int	b_data_fd;	/* file descriptor of data file	*/
int	b_user_fd;	/* file descriptor of user file	*/
int	b_free;		/* head of list of free blocks	*/

struct BLOCK * b_bp;	/* mem pointer to current block	*/

/*
	Define an array of pointers to each slot.
	The DATA_RES pseudo constant tells how many
	slots have actually been allocated.
*/
struct BLOCK * b_bpp [MAX_RES];

/*
	Define variables used only by read_file() and read1().
	They are used to speed up the code.
*/
char *	br_bufp;	/* pointer to input buffer	*/
int	br_bufc;	/* number of current buffer	*/
int	br_count;	/* index into input buffer	*/

int	br_avail;	/* number of free characters	*/
int	br_out;		/* index into outbuf		*/

/*
	Define variables used only by write_file() and write1().
	They are used to speed up the code.
*/

int	bw_count;	/* index into buffer		*/
