/*  VERSION 0033  (DATE: 30/04/87)  (TIME: 13:17)  */
/*
	e (qe) screen editor

	(C) G. Nigel Gilbert, MICROLOGY, 1981
	licensed for private non-profitmaking use 1983
	
	August-December 1981
	Modified: To QE from e (ver 4.6a) by J.W. Haefner -- Oct 84-Mar 85
			  To QEA 26.vii.85
	FILE: qe.h

	PURPOSE: standard header file, #included in all function files
		(NB e does NOT require "bdscio.h")

*/

#define VERSION "4.6a/QE:1.21"

/*  10.iv.87
  compile with -e9080 (e10 with -o as well) [qe11+ no wwrap]
  load    with -tA8b0  using l2
*/
/* QET */
/* compile: w/  qe11 (VERSION) + WWRAP=NO  use: -e9080, -tA8B0*/
/* compile: w/  qe11 (VERSION) + WWRAP=YES use: -e9580, -tad80 (??)*/
/* compile: w/o qe11 (VERSION) + WWRAP=YES use: -e8C80, -tA500 */
/* compile: w/o qe11 (VERSION) + WWRAP=NO  use: -e8580, -t9D00 */
/* QEA */
/* compile: w/  qe11 (VERSION) + WWRAP=NO  use: -e8FD0, -tA880 */

#define TOP 0xA8B0			/*top of program (STACK space=TOP-Externals End)*/
#define CCP 0x0800			/*length of CP/M's CCP */

#define SECSIZ 128			/*file buffer definitions*/
#define NSECTS 16
#define IOBUFSIZ (NSECTS * SECSIZ + 6 )
#define FILELEN 16			/*max length of file name*/

#define JBUFSIZE 6			/*size of buffer required for longjumps*/

#define LLIM 255			/*max length of a line*/
#define FLIM 80				/*max length of find/alter string*/

#define FAIL -1
#define NULL 0
#define YES 1
#define NO 0
#define PREV -2

/* --- major editor options flags --- */
#define WWRAP (NO)			/*allow word wrap and reformatting?*/
#define	SCRNDIMMD (NO)		/*yes==>highlite current line,no==>all brite */
#define PRINTALL (YES)		/*yes==>send all chars to printer*/
#define VRSNNUM (YES)		/*put version number at file top on save*/
#define VRSNDATE (YES)		/*put version date/time w/ version num */

#define NOWLOC 0x50			/*CP/M page 0 loc for date and time*/
#define NOCLOCK (YES)		/*clock for version*/
#define VRSNLEN 56			/*length of verson string*/

#define SWIDTH 79			/*screen size*/
#define SHEIGHT 23
#define OFFWIDTH 20			/*horizontal scroll increment*/
#define PAGEOVERLAP 4		/*overlap between pages with UP,DOWN-PAGE*/
#define HELPLINES 8			/*top line number when help menu is on display */

#define FNPOS 8				/*status line - position of file name*/
#define LNPOS 23			/*		position of line number*/
#define EMPOS 32			/*		position of error message*/
#define WAITPOS 32			/*		position of cursor when searching/reading*/
#define REPPOS 50			/*		position of "Replace?" message*/

#define BACKSP 0x08			/*backspace*/
#define ENDFILE 0x1a		/*CP/M end of text file mark */
#define CTRL 0x1f			/*last control char.*/
#define PARBIT 0x80			/*parity bit used as a flag by testkey() */
#define NKEYS 32			/*number of key codes*/
#define GOTOLEN 6			/*approx no. keys in control code <e4.8>*/

	/*internal key codes ('e' ver 4.6a key assignments in parentheses)*/

#define LEADIN 0			/*lead in chararacter to key codes -
							  ignored, but following key code parity set*/
#define DOWNKEY 1			/*cursor down  (X)*/
#define UPKEY   2			/*cursor up (E)*/
#define LEFTKEY 3			/*cursor left (S)*/
#define RIGHTKEY 4			/*cursor right (D)*/
#define RIGHTWKEY 5			/*cursor right one word (F) */
#define LEFTWKEY 6			/*cursor left one word (A) */
#define EOLKEY 7			/*cursor to end of line (P) */
#define BOLKEY 8			/*cursor to beginning of line (O) */
#define UPPAGE 9			/*move up a page (R) */
#define DOWNPAGE 10			/*move down a page (C) */
#define BOFKEY 11			/*cursor to beginning of file (U) */
#define HOMEKEY 12			/*cursor to end of file (B) */
#define DELLEFT 13			/*delete char to left of cursor (DEL) */
#define DELRIGHT 14			/*delete char under cursor (G) */
#define DELLNKEY 15			/*delete cursor line (\) */
#define DELWDKEY 16			/*delete word to right of cursor (T) */
#define JUMPKEY 17			/*jump to (J) */
#define CRSTILL 18			/*insert newline after cursor (N) */
#define QWIKKEY 19			/*quick actions (find etc) (Q) */
#define SCRLDNKEY 20		/*scroll down (Z)*/
#define TOPSCRKEY 21		/*top of screen (Y) */
#define BOTSCRKEY 22		/*bottom of screen (V) */
#define BLOCKKEY 23			/*block operations (K) */
#define SCRLUPKEY 24		/*scrolll up (W) */
#define REPKEY 25			/*repeat last find/alter (L) */
/*#define HELPKEY 26*/			/*display help menu (V) */
#define UNDELINE 26			/*inject a previously deleted line*/
#define OOPSKEY 27			/*restore unedited line (_) */
#define TAB 28				/*tab (I)*/
#define RETRIEVE 29			/*retrieve last name etc entered here (L)*/
#define CR 30				/*return (M)*/
#define ESCKEY 31			/*the magic escape key (ESC)*/

	/* key codes + 0x80 (PARBIT) used in scanning strings */
#define LEFTKEY_P	131
#define DELLEFT_P	141
#define RETRIEVE_P	157
#define	CR_P		158
#define ESCKEY_P	159

#define BRIDIM '|'			/*char placed in e's message strings to start/stop
		  					  dim (half-intensity) display */

	/* CP/M bdos function codes */

#define RESET 0
#define CONIN 0x01
#define CONOUT 0x02
#define LSTOUT 0x05
#define DIRIO 0x06
#define IOBYTE 0x07
#define SETIOBYTE 0x08
#define PRINTS 0x09
#define READBUF 0x0A
#define CONSTAT 0x0B
#define VERNO 0x0C
#define RESETDSK 0x0D
#define SELDSK 0x0E
#define OPEN 0x0F
#define CLOSEF 0x10
#define SEARCH1 0x11
#define SEARCHN 0x12
#define DELETEF 0x13
#define READ 0x14
#define WRITE 0x15
#define MAKE 0x16
#define RENAME 0x17
#define LOGINV 0x18
#define CURDSK 0x19
#define SETDMA 0x1A
#define PROTECT 0x1C
#define GETROV 0x1d
#define DSKPARAMS 0x1F
#define READR 0x21
#define WRITER 0x22
#define FSIZE 0x23
#define SETRR 0x24
#define RESETDRV 0x25

#define MAXSLOTS 128			/*virtual memory constants*/
#define MAXMEMSLOTS 20			/*for 64k TPA (CP/M816); old=20*/
#define PAGESIZE 1024
#define PAGESECTS (PAGESIZE/SECSIZ)

#define FREE 0
#define INUSE 1
#define NOTAVAIL -1
#define MAXINT 32767
#define NOFILE (MAXINT)
#define ABSOLUTE 0
#define UNKNOWN (MAXINT)

#define sint char

struct addr {
	sint page;
	int moffset;
	} *tp;					/*address of text of each line*/

int allocp;					/*location of next free in newpage */
int pageloc[MAXSLOTS];		/*current location of each page*/
int usage[MAXMEMSLOTS];		/*records usage of in memory slots*/
char *slotaddr[MAXMEMSLOTS];	/*memory addr of start of each in memory slot*/
sint dskslots[MAXSLOTS];	/*is disk slot free*/
sint slotsinmem;			/*max no. of slots available in memory*/
sint tppages;				/*no. of pages used to hold text pointers*/
sint newpage;				/*page no of page being filled with text*/
char *npaddr;				/*base of slot holding newpage*/
int clock;					/*counter for least recently used paging*/

int pagefd;					/*file descriptor of paging file*/
char pagingfile[FILELEN];	/*name of paging file*/
char pagingdisk;			/*disk to put paging file on*/

sint curdsk;				/*currently logged in (default) disk*/

char mainmenu[JBUFSIZE];	/*for emergency longjump*/
int goodline;				/*line number we know we can display */
unsigned errold[4];			/*temp store for CP/M's error jump addrs*/
char dskerr[JBUFSIZE];		/*for bdos disk error traps*/

char textbuf[IOBUFSIZ];		/*io buffer for file being edited*/

sint cursorx, cursory;		/*current position of cursor*/
char tran[NKEYS];			/*translation table from external to internal
							  key codes*/
sint isdim;					/*last char was in dim*/
int lastl;					/*number of last line of text*/
int lastread;				/*last line actually read to date*/
sint goteof;				/*true when eof of file being edited*/
sint changed;				/*true if file has been modified*/
int cline;					/*current line*/
char text[LLIM];			/*buffer for current line of text*/
char textb[LLIM];			/*buffer for last deleted line*/
int charn;					/*position of current char in current line*/
sint altered;				/*if YES, current line has been altered by edits*/

int pfirst, plast;			/*top and bottom line displayed on screen*/
sint topline;				/*first line on screen for text*/
sint offset;				/*first col. to display on screen*/
sint lastoff;				/*offset last time resetcursor() was called*/
sint blankedmess;			/*message area has been blanked, not overwritten*/

char inbuf[20];				/*input buffer for typeahead*/
sint  inbufp;				/*pointer to next char in inbuf*/

			/*environment options*/
			
sint autoin;				/*do auto indentation*/
sint backup;				/*at end, old version becomes file.bak*/
sint readall;				/*to read as much text as possible*/
sint tabwidth;				/*width between tabstops*/
sint trail;					/*don't chop trailing blanks from text lines*/
sint wboot;					/*do a warm boot on exit; else jump to CCP*/
sint displaypos;			/*display line:col at top of screen*/
sint helpon;				/*display help menu*/
sint expert;				/*expert mode, brief prompts*/
sint blockscroll;			/*horizontal scroll whole page*/
#if WWRAP
int  rtmarg;				/*right margin for wordwrap*/
#endif
#if VRSNNUM
char version;				/*put version line on file top on save*/
#endif

char filename[FILELEN];		/*current file being edited*/
char defext[4];				/*default file extension*/
char *errmess;				/*last error message*/

char ans[6];				/*line to jump to*/
char patt[FLIM];			/*pattern to find*/
char changeto[FLIM];		/*replacement string*/
char opts[5];				/*options for alter*/
sint replace;				/*last find/replace was a replace*/
int findir;					/*direction of last find*/
sint nocheck;				/*don't ask for confirmation before replacing*/
sint repeat;				/*repeat last find/replace*/

char name[FILELEN];			/*filename for read and write files*/

int jmpto,initjmp;			/*initial line to jump */

int from,to;				/*block start and end*/
sint blocking;				/*true if block is or has been defined*/

char fbuf[IOBUFSIZ];		/*file buffer for all file ops other than edited file*/

struct iobuffer {
	int _fd;
	int _nleft;
	char *_nextp;
	char _buff[NSECTS * SECSIZ];
	};						/*the structure of the file buffers*/
	
/* globals for version control compile option*/
#if VRSNNUM
char comnt[5];				/*comment symbols for version line*/
char *comntsym();			/*get comment symbol for this file*/
char date[9];				/*version date*/
char time[6];				/*version time*/
#endif

char *getline(), *strcpy(),*firstwhite();
char getkey(),getlow(),getscankey(),inkey(),testkey(),testlow();

/*end of standard header*/
t options