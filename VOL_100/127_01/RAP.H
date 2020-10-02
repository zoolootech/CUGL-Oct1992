/*********************************************************************\
** .---------------------------------------------------------------. **
** |                                                               | **
** |                                                               | **
** |         Copyright (c) 1981, 1982, 1983 by Eric Martz.         | **
** |                                                               | **
** |                                                               | **
** |       Permission is hereby granted to use this source         | **
** |       code only for non-profit purposes. Publication of       | **
** |       all or any part of this source code, as well as         | **
** |       use for business purposes is forbidden without          | **
** |       written permission of the author and copyright          | **
** |       holder:                                                 | **
** |                                                               | **
** |                          Eric Martz                           | **
** |                         POWER  TOOLS                          | **
** |                    48 Hunter's Hill Circle                    | **
** |                      Amherst MA 01002 USA                     | **
** |                                                               | **
** |                                                               | **
** `---------------------------------------------------------------' **
\*********************************************************************/

/*--------------------------------------------------------------------------

HEADER FILE FOR "ROFF AND PRINT" = RAP by Eric Martz.
The RAP program is built on ROFF.C by Kathy Bacon, which was obtained
from the BDSC Users Group. ROFF.C, in turn, is an adaptation of
Kernighan and Plauger's ROFF in RATFOR in their book Software Tools.

The first functional RAP was created in August-October 1981.
Debugging, additions and augmentations have continued through the
present (August, 1983).

--------------------------------------------------------------------------
KNOWN BUGS:

	1. Tabs not working if line has substitutions.
	
	2. Justification (spread()) doesnt work.

--------------------------------------------------------------------------
Obsolete Bug (autospacing code has been defeated)

	     No double space after period ending sentence when sentence
		ends at end of input line. Difficult to correct since getspaces()
		cant look ahead to see if next word is capitalized. Left as is
		in accord with preference to fail to double space rather than
		put in inappropriate double spaces, e.g. a line ending in Dr.
		Somebody as does the previous line.

--------------------------------------------------------------------------

FILE LOCATIONS OF RAP FUNCTIONS:

P=RAP.C
1=RAP1.C etc.
R=RAPREAD.C

FUNCTION    FILE	CALLS
-----------------------------------------------------------
main			P	dioinit, init_defaults, anotherline,
				command, text, dioflush

anotherline	3	saveextra, rapgets, ss, expand_controls

baud			R	rapgets, de-comment

biggets		4

brk			3

center		1

checkbig		4

command		1	comtyp, get_val, brk, set, checkbig, gettl, read_command
				find_portion, getpairs, restore_mode, space

comtyp		1

de_comment	4

expand_controls3

expantab		4

fillbuf		3

fillulbuf		3	getspaces, fillbuf

find_portion	R	rapgets

getpairs		4	ssgets, de_comment, ioport, baud

getspaces		3

gettl		2	skip_blanks, substitute

getval		1

getwrd		3	

indent		2

init_defaults	P

init_ss		4	rapgets, de_comment, checkbig, command, getpairs

init_text		2

ioport		R	rapgets, de_comment

leadbl		2

open_read		R

phead		2	skip, puttl

pfoot		2	skip, puttl

put			2	phead, indent, putline, skip, pfoot

putline		3	ss, indent

puttl		2	ss, restore_mode

putwrd		3	brk, fillbuf

rapgets		4	biggets

read_command	R	open_read

restore_mode	2

saveextra		3

screen_status	R

set			1

skip			2

skip_blanks	1

space		2	brk, skip, pfoot

spread		1	trunc_bl

ss			4	delchars, substitute, checkbig

ssgets		4	rapgets

text			2	init_text, space, leadbl, expantab, fillulbuf,
				put, restore_mode, center, getwrd, getspaces,
				putwrd, checkbig

trunc_bl		1

--------------------------------------------------------------------------*/

#include <bdscio.h>
#include "bdscio+.h"

/*--------------------------------------------------------------------------
Version history:

1.0->1.1:

Change of input file does not automatically begin a new page.
Initialization string comes from {init{ in SS#.
Bug in }RF} fixed.
\\ // -> {{ }}

1.1->1.2:

Don't double space after a period unless preceding word length > 3.
Replace {#{ with auto-incrementing arabic number.
Direct "no ss0 file" error message to STDERR.

1.2->2.0:
Implemented ".re filename portion" command.
Implemented {ux{/{dx{ macro expansion for super/sub-scripts.
Implemented .ss 1/0 command.
Made PORT_OUT the dioinit() default.

2.0->2.0a:
Implemented built-in substitutions.
Changed SS0-9 to SSx...
Added .d1, .d2, .ou.
Implemented commands in SS capability, i/o port, baud in SS.

2.0a->2.0b = 2.00
Rearranged argument parsing so '>/...' overrides .ou in SS.
Added '>' to zero _Outflag.
Fixed head/foot flushing and centering for proportional fonts.
Implemented '.bo' command.
Implemented VIEW capability.

2.00 -> 2.10
Changed SSFILE to FILE.SS to work with Microshell. (DEFAULT.SS)
Used #ifdef DEBUG to remove all debugging code to reduce size.
Linked with L2 to reduce size. Externals 8c00.
#defined FILENAME as 16 to accept drive specification.
Fixed badname() to accept drive specification.
Implemented Deinit, Interrupted (with ^C dioflush()).
Added backspace to cancel space from console in view mode.

2.10 -> 2.11
Changed NAKED_NEWLINE from ^N to ^] ("group separator", ASCII 1Dh, 29d)
Redirection mark '/' changed to ')'

2.11 -> 2.12
{#{ is incremented only when {#+{; only one per input line.
A repeated original ss string causes the new replacement to replace the
	former replacement.

2.12 -> 2.13
	.br now starts a ".pi" paragraph (Tival = Pival).

2.13 -> 2.14
	.fn range 0-5 expanded to 0-16

2.14 -> 2.15
added }"console prompt} feature
added }wait} function
added .cf continuous forms
added .bl blanking character
arranged to have lines beginning with an invalid command passed into text()

2.15 -> 2.16
deleted NAKED_NEWLINE feature
added "compress control" feature with exception for ^J (->naked)
	to allow ^Z

2.16 -> 2.17
made non-continuous forms not wait at page bottoms with -p# flag

2.17 -> 2.18
fixed underlining when }u+} not defined and filling in putwrd()
	to account for post-substitutions and backspaces

2.18 -> 2.19
Added .lf command for printers which generate auto-lf on cr; also, when
	.lf 1, a literal cr (as in a segmented header) generates a Rev_lf.

2.19 -> 2.20
Removed coded licensee ident; commented out portion code with .re.
Made getspaces() use same spacing as input text.
Added {_...{, {!...{.
Rebuilt underlining and boldfacing code, all processing at output.
	Added shift and restore for each.
Bug which failed to break at blank line containing spaces fixed.
Added 10 new dot commands: .ar, .bd, .ed, .i@, .ig, .ix, .ne,
	.p#, .pd, .tc.

2.20 -> 2.21
Defeated blank-squeezing in ss()
Fixed centering to ignore leading/trailing white

--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------

GENERAL PURPOSE CONSTANTS

--------------------------------------------------------------------------*/

#define	STDERR		4
/* send error messages to console while in DIO */

#define	CTRL_C		'\3'
#define	BIGBUF		512
#define	HUGE			135	/* generally large number */
#define	FILENAME		16	/* big enough for cp/m name A:12345678.123 */
#define	COMMAND		'.'	/* all commands starts with this */

#define	LIST_OUT	2
#define	UNKNOWN		-1	/* returned if doesn't recg. command */
#define	NO_VAL		-32760	/* returned when no argument w/commad */

#define	CR			0x0D
#define	FORMFEED		'\12'
#define	FORCEPRINT	0
#define	SPACE		' '
#define	BACKSPACE		'\b'
#define	TAB			'\t'
#define	BLANK		' '
#define	DASH			'-'

#define	WAIT			"\227"	/* meta control W */
#define	PROMPT		"\220" /* meta control P */

/* BDOS CALLS ETC */
#define	CONIN		1
#define	CON_STATUS	11
#define	CONOUT		2

/*--------------------------------------------------------------------------

FORMATTING COMMANDS TO BE IMBEDDED IN INPUT TEXT

--------------------------------------------------------------------------*/

#define	AR	1	/* arabic for {#{ */
#define	BD	2	/* begin draft */
#define	BL	3	/* blank out character */
#define	BO	4	/* bold */
#define	BP	5	/* begin page  */
#define	BR	6	/* causes break */
#define	CC	7	/* compress controls */
#define	CE	8	/* center line(s) */
#define	CF	9	/* continuous forms */
#define	D1	10   /* pre ss delim */
#define	D2	11	/* post ss delim */
/*
#define	DEBUG	12	/* turn on Debug */
*/
#define	EC	13	/* expand control characters */
#define	ED	14	/* end draft */
#define	FI	15	/* fill lines */
#define	FN	16	/* select primary font */
#define	FO	17	/* footer title */
#define	HE	18	/* header title */
#define	IAT	19	/* ignore leading @ */
#define	IC	20	/* ignore subsequent commands */
#define	IG	21	/* ignore */
#define	IN	22	/* left indent */
#define	IX	23	/* index */
#define	JU	24	/* justify */
#define	LF	25	/* on: expand LF to CR+LF; off: issue CR only */
#define	LS	26	/* line spacing */
#define	M1	27	/* top margin */
#define	M2	28	/* second top margin */
#define	M3	29	/* first bottom margin */
#define	M4	30	/* bottom-most margin       */
#define	MO	31	/* restore mode on Sanders */
#define	NEED	32	/* lines needed else new page; NE conflicts with != */
#define	NF	33	/* no fill */
#define	NJ	34	/* no justify */
#define	NX	35	/* next input file name */
#define	OU	36	/* _Outflag */
#define	PI	37	/* paragraph indent */
#define	PD	38	/* print draft */
#define	PL	39	/* set page length */
#define	PNO	40	/* set page # */
#define	POST_SS	41	/* }} replacement off/on */
#define	RE	42	/* read sub input file/portion */
#define	PRE_SS	43	/* {{ replacement off/on */
#define	RF	44	/* restore font to primary/default */
#define	RM	45	/* set right margin */
#define	SA	46	/* select/deselect Sanders printer */
#define	SP	47	/* add blank line(s) */
#define	SS	48	/* input additional ss pairs */
#define	TA	49	/* set tab modulus */
#define	TC	50	/* table of contents */
#define	TI	51	/* temporary indent */
#define	TP	52	/* read in file at next top of page */
#define	UL	53	/* underline line(s) */

/*--------------------------------------------------------------------------

CONSTANTS FOR STRING SUBSTITUTION (SS) UTILITY

--------------------------------------------------------------------------*/

#define	MAXPACK 2048 /* dimension of Oldnewbuf */
#define	MAXPAIRS 100

/*--------------------------------------------------------------------------

FILE I/O

--------------------------------------------------------------------------*/

#define NIOBUFS 2 /* 2 needed only for subread capability */
#include <fio1.h>

/*--------------------------------------------------------------------------

EXTERNALS BEGIN HERE

--------------------------------------------------------------------------*/
#ifdef DEBUG
int Debug;
#endif

int
	Arabic,		/* autoincrementing number replaces {#{ */
	Bottom,		/* last live line */
	Bo_input,
	Bo_plus,
	Carat,
	Ce_input,		/* set equal to number of lines to be Centered */
	Comp_carat,
	Con_forms,	/* continuous forms */
	Curpag,		/* current output page number, init = 0 */
	Dir,			/* for "spreading" of lines    */
	End_wait,		/* if set, wait at bottom of current page */
	Exp_ctrl_chars,/* expand control characters, e.g. '\0' to ^@ */
	Extrafull,
	Fill,		/* set to YES or NO, default YES */
	First_text,	/* Yes until first text line encountered */
	Font,		/* Current primary Sanders font */
	Ignore_at,	/* on for ingnore leading @ */
	In_linecnt,	/* line count in input file */
	In_main,		/* line count in main while reading sub file */
	Index,		/* on for index */
	Indraft,		/* input text between .bd and .ed */
	Interrupted,	/* flag used in putchar to deal with multiple ^C's */
	Inval,		/* left indent -> default  0 */
	Justify,		/* YES or NO, default NO */
	Lineno,		/* next line to be printed, init = 0 */
	Lines_out,
	Lsval,		/* line spacing value -> default will be 1 */
	M1val,		/* margin before and including header */
	M2val,		/* margin after header */
	M3val,		/* margin after last text line */
	M4val,		/* bottom margin, including footer */
	Newpag,		/* next output page number, init = 1 */
	Oldnewtot,
	Outlen,
	Outwrds,		/* no. words in outbuf, init = 0 */
	Pagestop,
	Pdata,
	Pimask,
	Pival,		/* paragraph indent, default Inval+5 */
	Plval,		/* page length in lines, init=66 */
	Pomask,
	Post_ss,		/* // delimited ss off/on */
	Pre_ss,		/* \\ delimited ss off/on */
	Pready,
	Preset,
	Presetval,
	Press_all,	/* Set if SSn contains non-delimited original strings */
	Press_flg,	/* Line contains pre format substitution delimiters */
	Print_at,		/* on suppresses printing until page # = Print_at */
	Printdraft,	/* on for print draft */
	Prompt,
	Pstatus,
	Rmval,		/* right margin */
	Sanders,		/* set to YES or NO for Sanders Printer */
	Screen_size,
	Skip_cnt,
	Spacecnt,
	Spval,		/* blank lines to be spaced down */
	Ss_pair_cnt,
	Suppress_lf,	/* on for printers which auto lf on cr */
	Tabcont,		/* on for table of contents */
	Tabval,		/* tabulation modulus, default 5 */
	Tival,		/* temporary indent -> default  0 */
	Ul_input,		/* set equal to number of lines to be underlined */
	Ul_plus,
	Wordlen;

char
	*Bar,
	*Bo_off,
	*Bo_on,
	*Bo_restore,
	*Bo_shift,
	*Center_mode,
	*Deinit,
	*Flush_left,
	*Flush_right,
	*Font_c,
	*Font_o,
	*Forceprint,
	*Init,
	*Just_one,
	*Just_two,
	*Newstring[MAXPAIRS],
	*Nextin,
	*Nextout,
	*Oldstring[MAXPAIRS],
	*Pp[16],
	*Rev_lf,
	*Ul_off,
	*Ul_on,
	*Ul_restore,
	*Ul_shift,
	*Wordbegin,
	*Wordend;

char	
	Blank_c,	/* Character to blank out */
	Extralines[BIGBUF],
	Footer[ MAXLINE ],	/* footer title */
	Header[ MAXLINE ],	/*  header title */
	Inbuf[BIGBUF],
	Nxname[FILENAME],
	Oldnewbuf[MAXPACK],
	Outbuf[BIGBUF],
	Post_ss_delim,
	Pre_ss_delim,
	Ss_name[FILENAME],
	Ulbuf[BIGBUF];

/*
/* FOR SUB-READ WITH PORTIONS */

#define PORTIT_O "[[" /* PORTION TITLE OPENING DELIMITER */
#define PORTIT_C "]]" /* PORTION TITLE CLOSING DELIMITER */
int Old_portion, New_portion;
*/
char Fn_main[FILENAME], Fn_read[FILENAME];
FILE *Fpin, *Fp_main, *Fp_read;

/* NEEDED BY RAPIO.C */

int _Outflag;
char Out_file[FILENAME];
char _Dobuf[BUFSIZ];	/* I/O buffers used for direction  */
int Xflag;

#define SCREEN_SIZE 20

/*--------------------------------------------------------------------------
		END OF RAP.H
--------------------------------------------------------------------------*/
