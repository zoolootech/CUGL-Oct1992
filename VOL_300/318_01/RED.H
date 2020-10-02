/*
	Header file for RED -- Desmet C version
	Source:  red.h
	Version: January 18, 1990.

	Written by
	
		Edward K. Ream
		166 N. Prospect
		Madison WI 53705
		(608) 257-0802


	PUBLIC DOMAIN SOFTWARE

	This software is in the public domain.


	DISCLAIMER

	With respect to the programs and documentation on this disk,
	Edward K. Ream specifically disclaims all warranties, express or 
	implied, including but not limited to implied warranties of
	merchantability and fitness for a particular purpose.  In no event
	shall Ream be liable for any loss of profit or commercial damage, 
	including but not limited to special, incidental, consequential or 
	other damages.
*/

/*
	The following constants select the compiler and enable features.
	If     #define'd,  the feature is enabled.
	If NOT #define'd,  the feature is disabled.

	IBM:		code is for the IBM PC/XT/AT
	DESMET:		code is for the Desmet C compiler.
	TURBOC:		code is for TURBO C v2.0 compiler.

	SWAP:		enables auto-swapping of dirty blocks.
	SUSPEND:	enables quit and auto-restart of RED.
*/

#define IBM		1
#define SUSPEND		1

#include "d:\sherlock\sl.h"

#include <ctype.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>

#include "redtmp.h"
#include "red1.h"
#include "redbuf.h"

/*
	Define constants describing a text line.
	These constants must be less than the maximum block
	size defined in redbuf.h.
*/
#define MAXLEN	200	/* max chars per line */
#define MAXLEN1	201	/* MAXLEN + 1 */

/*
	Define operating system constants.
*/
#define SYSFNMAX 200			/* max length of a path name. */
#define TEMP_FILE "@@TEMP@@.TMP"	/* name of temp file */
#define DATA_FILE "@@DATA@@.TMP"	/* name of work file */

/*
	Define misc. constants.
*/
#define TRUE		(1)
#define FALSE		(0)
#define ERROR   	(-1)	/* error code			*/
#define OK		(0)	/* opposite of ERROR		*/
#define EOF_MARK	(0x1a)	/* end-of-file mark		*/
#define HUGE		(32000)	/* practical infinity		*/

/*
	Declare globals variables.
*/
extern int	hasdn;			/* describe terminals.      */
extern int	hasup;
extern int	hasins;
extern int	hasdel;
extern int	hasint;
extern int	hascol;
extern int	haswrap;
extern int	hasword;

extern int	outx;			/* cursor position. */
extern int	outy;

extern char	g_file [SYSFNMAX];
extern jmp_buf	DISK_ERR;

extern int	fmtcol[MAXLEN1];

extern int	a_start, a_end, again_flag, a_rev;
extern int	a_rflag, a_wflag, a_cflag;
extern char	a_spat [MAXLEN1], a_rpat [MAXLEN1];

/*
	Used by buffer routines.
*/
extern int	DATA_RES;
extern int	b_fatal;
extern int	b_cflag;
extern int	b_line;
extern int	b_max_line;
extern int	b_start;
extern int	b_head;
extern int	b_tail;
extern int	b_max_diskp;
extern int	b_max_put;
extern int	b_data_fd;
extern int	b_user_fd;
extern int	b_free;
extern struct BLOCK * b_bp;
extern struct BLOCK * b_bpp [MAX_RES];
extern char *	br_bufp;
extern int	br_bufc;
extern int	br_count;
extern int	br_avail;
extern int	br_out;
extern int	bw_count;
