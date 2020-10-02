/*
HEADER:		CUG208;
TITLE:		'e' for CP/M68K
VERSION:	1.48+

DESCRIPTION:	"a screen editor";

KEYWORDS:	editor;
SYSTEM:		CP/M68K, V1.2;
FILENAME:	e/e.h
WARNINGS:	"the default value is for systems with 128K bytes
		 of memory or more";
SEE-ALSO:	cpm68k.c, e68k.doc, CUG VOL 133;
AUTHORS:	G.N.Gilbert('e'), J.W.Haefner(for DeSmet C on MSDOS and UNIX)
CODER:		Yoshimasa Tsuji
COMPILERS:	DRI C(Alcyon C) for CP/M68K;
*/
/*
	PURPOSE: include file for e
*/
/* ------- begin of re-compilation options -----	*/
#define YES 1
#define NO 0
#define CPM68K	1
#define MSDOS	0

#define LARGE	1	/* if you can allow 64K byte arrays */

/* -------- end of re-compilation options ----------- */
#define VERSION "4.8b"
#include <stdio.h>
#include <ctype.h>

typedef short bool;

#ifdef CPM68K
#define FILELEN 18	/*max length of file name (uud:ffffffff.xxx\0)*/
#else
#define FILELEN 50
#endif

#define INBUFLEN 20	/*length of type-in buffer */
#define CURSORWAIT 1000
#define LLIM 255	/*max length of a line*/
#define FLIM 80		/*max length of find/alter string*/

#define FAIL -1
#define PREV -2

#define HISTLEN 40	/*history stack of commands for undoing */
#define HISTINSERT 1	/*type of command on history stack */
#define HISTDELETE 2
#define HISTREPLACE 3

#define SWIDTH 79	/*screen size*/
#define SHEIGHT 23
#define OFFWIDTH 20	/*horizontal scroll increment*/
#define PAGEOVERLAP 4	/*overlap between pages with UP,DOWN-PAGE*/
#define HELPLINES 8	/*top line number when help menu is on display */

#define FNPOS 8		/*status line - position of file name*/
#define LNPOS 23	/*		position of line number*/
#define EMPOS 32	/*		position of error message*/
#define WAITPOS 32	/*		position of cursor when searching/reading*/
#define REPPOS 50	/*		position of "Replace?" message*/

#define BACKSP 0x08	/*backspace*/
#define ENDFILE 0x1a	/*CP/M end of text file mark */
#define CTRL 0x1f	/*last control char.*/
#define PARBIT 0x80	/*parity bit used as a flag by testkey() */
#define NOPARITY 0x7f	/*mask to strip parity bit */
#define NKEYS 32	/*number of key codes*/
#define GOTOLEN 6	/*approx number of chars in control code string for
				cursor addressing*/

	/*internal key codes (recommended key assignments in brackets)*/

#define LEADIN 0	/*lead in chararacter to key codes -
				ignored, but following key code parity set*/
#define DOWNKEY 1	/*cursor down */
#define UPKEY	2	/*cursor up*/
#define LEFTKEY 3	/*cursor left*/
#define RIGHTKEY 4	/*cursor right*/
#define RIGHTWKEY 5	/*cursor right one word (D) */
#define LEFTWKEY 6	/*cursor left one word (S) */
#define EOLKEY 7	/*cursor to end of line (E) */
#define BOLKEY 8	/*cursor to beginning of line (B) */
#define UPPAGE 9	/*scroll up a page (W) */
#define DOWNPAGE 10	/*scroll down a page (Z) */
#define BOFKEY 11	/*cursor to beginning of file (U) */
#define HOMEKEY 12	/*cursor to end of file (HOME) */
#define DELLEFT 13	/*delete char to left of cursor (DEL) */
#define DELRIGHT 14	/*delete char under cursor (G) */
#define DELLNKEY 15	/*delete cursor line (Y) */
#define DELWDKEY 16	/*delete word to right of cursor (T) */
#define JUMPKEY 17	/*jump to (X) */
#define CRSTILL 18	/*insert newline after cursor (N) */
#define QUITKEY 19	/*quit (Q) */
#define ENVIRKEY 20	/*edit and file context (C) */
#define FINDKEY 21	/*find (F) */
#define ALTERKEY 22	/*alter (A) */
#define BLOCKKEY 23	/*block operations (O) */
#define RDFILEKEY 24	/*read a file (P) */
#define REPKEY 25	/*repeat last find/alter (R) */
#define HELPKEY 26	/*display help menu (V) */
#define UNDOKEY 27	/*undo history of commands (\) */
#define TAB 28		/*tab (I)*/
#define RETRIEVE 29	/*retrieve last name etc entered here (R)*/
#define CR 30		/*return (M)*/
#define ESCKEY 31	/*the magic escape key (ESC)*/

#define BRIDIM '|'	/*char placed in e's message strings to start/stop
				dim (half-intensity) display */

# if LARGE
#define MAXSLOTS 256	/*virtual memory constants*/
#define MAXMEMSLOTS 64
# else
#define MAXSLOTS 128	/*virtual memory constants*/
#define MAXMEMSLOTS 20
# endif

#define PAGESIZE 1024

#define FREE 0

#define INUSE 1
#define NOTAVAIL -1
#define MAXINT 32767
#define NOFILE (MAXINT)
#define ABSOLUTE 0
#define UNKNOWN (MAXINT)

struct addr {
	short page;
	int moffset;
	};			/*address of text of each line*/

struct histstep {		/*command history stack for undoing */
	struct addr histp;	/*address of text modified by command*/
	int histline;		/*line number of modified text*/
	short histtype;		/*type of command (insert,delete,replace)*/
	short histcomm;		/*number of command modulo 256 */
	};
extern	char *strcpy(), *strcat();
char *getline();


/* global variables for 'e'
 */

# ifdef DEF
#define extern
# endif

extern struct addr *tp;		/*address of text of each line*/

extern int allocp;			/*location of next free in newpage */
extern int pageloc[MAXSLOTS];		/*current location of each page*/
extern int usage[MAXMEMSLOTS];		/*records usage of in memory slots*/
extern char *slotaddr[MAXMEMSLOTS];	/*memory addr of start of each in memory slot*/
extern bool dskslots[MAXSLOTS];	/*is disk slot free*/
extern short slotsinmem;	/*max no. of slots available in memory*/
extern short tppages;		/*no. of pages used to hold text pointers*/
extern short newpage;		/*page no of page being filled with text*/
extern char *npaddr;		/*base of slot holding newpage*/
extern int clock;		/*counter for least recently used paging*/

extern int pagefd;			/*file descriptor of paging file*/
extern char pagingfile[FILELEN];	/*name of paging file*/

#ifdef CPM68K
#define PATHLEN 6
#else
#define PATHLEN 30
#endif

extern char pagingdir[PATHLEN];		/*directory to put paging file in*/
extern char curdir[PATHLEN];	/*currently logged in (default) directory */

extern unsigned ncommand;	/*count of commands executed since startup*/
extern int goodline;	/*line number we know we can display */

extern struct histstep	history[HISTLEN];
extern int histptr;			/*pointer into history stack*/
extern short histcnt;			/*number of commands on stack*/
extern short storehist;		/*commands only stored on history when true*/

extern FILE *textfp;			/*io buffer for file being edited*/

extern short cursorx, cursory;		/*current position of cursor*/
extern char tran[NKEYS];	/*translation table from external to internal
				key codes*/
extern bool isdim;			/*last char was in dim*/
extern int lastl;			/*number of last line of text*/
extern int lastread;		/*last line actually read to date*/
extern bool goteof;			/*true when eof of file being edited*/
extern int cline;			/*current line*/
extern char text[LLIM];		/*buffer for current line of text*/
extern int charn;		/*position of current char in current line*/
extern bool altered;	/*if YES, current line has been altered by edits*/

extern short pfirst, plast;	/*top and bottom line displayed on screen*/
extern int topline;		/*first line on screen for text*/
extern short offset;			/*first col. to display on screen*/
extern short lastoff;	/*offset last time resetcursor() was called*/
extern bool blankedmess; /*message area has been blanked, not overwritten*/

extern char inbuf[INBUFLEN];		/*input buffer for typeahead*/
extern short  inbufp;			/*pointer to next free in inbuf*/

#if DEF
bool autoin=YES;	/*auto indent  [YES/NO]*/
bool backup=NO;	/*make ".BAK" file [YES/NO]*/
bool trail=NO;	/*don't strip trailing blanks [YES/NO]*/
bool helpon=NO;	/*start with help menu on [YES/NO]*/
short tabwidth=8;	/*tab stops every n cols [number]*/
bool displaypos=YES;	/*display line:column at top of screen*/
bool blockscroll=YES;	/*don't scroll whole page, just current line*/
bool hilight=NO;	/*highlight current line*/

# else
extern bool autoin;			/*do auto indentation*/
extern bool backup;		/*at end, old version becomes file.bak*/
extern bool helpon;			/*display help menu*/
extern short tabwidth;			/*width between tabstops*/
extern bool trail;		/*don't chop trailing blanks from text lines*/
extern bool displaypos;		/*display line:col at top of screen*/
extern bool blockscroll;		/*horizontal scroll whole page*/
extern bool hilight;			/*make current line bright, rest dim*/
# endif

extern char filename[FILELEN];		/*current file being edited*/
extern char *errmess;			/*last error message*/

extern char ans[6];			/*line to jump to*/
extern char patt[FLIM];		/*pattern to find*/
extern char changeto[FLIM];		/*replacement string*/
extern char opts[5];			/*options for alter*/
extern bool replace;			/*last find/replace was a replace*/
extern int findir;			/*direction of last find*/
extern bool nocheck;	/*don't ask for confirmation before replacing*/
extern bool repeat;			/*repeat last find/replace*/

extern bool nocase;			/*ignore case in comparison*/

extern char name[FILELEN];		/*filename for read and write files*/


extern int from,to;			/*block start and end*/
extern bool blocking;		/*true if block is or has been defined*/



