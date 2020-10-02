/*
	e screen editor

	(C) G. Nigel Gilbert, MICROLOGY, 1981
	licensed for private non-profitmaking use 1983

	August-December 1981

	FILE: e.h

	PURPOSE: standard header file, #included in all function files
		(NB e does NOT require "bdscio.h")

*/

#define VERSION "4.8a"

/*
  compile with -e7e00 (e10 with -o as well)
  load	  with -t 9500	using l2	(see TOP below)
*/

#define TOP 0x9500	/*top of program- MUST BE EQUAL TO the -t arg to l2 */
#define CCP 0x0800	/*length of CP/M's CCP */

#define CURSORWAIT 1000	/*set this according to the speed of your
			microprocessor clock
				- for a Z80A set to 1000
				- for an 8080 set to 500
				- for a Z80B set to 1500
			Note that an incorrect setting of this constant
			does NOT create errors - just makes the cursor
			flash too fast/slow	*/

#define SECSIZ 128	/*file buffer definitions*/
#define NSECTS 16
#define IOBUFSIZ (NSECTS * SECSIZ + 6 )
#define FILELEN 18	/*max length of file name (UU/D:FFFFFFFF.xxx\0)*/
#define INBUFLEN 20	/*length of type-in buffer */

#define JBUFSIZE 6	/*size of buffer required for longjumps*/

#define LLIM 255	/*max length of a line*/
#define FLIM 80		/*max length of find/alter string*/

#define FAIL -1
#define NULL 0
#define YES 1
#define NO 0
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
#define DEBUGSCR NO	/*display control chars as numeric literals for
				testing screen displays*/

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
#define USERCODE 0x20
#define READR 0x21
#define WRITER 0x22
#define FSIZE 0x23
#define SETRR 0x24
#define RESETDRV 0x25

#define CPM3ERRORMODE 45	/*set error mode to return and display
					(specific to CP/M Plus) */

#define MAXSLOTS 128	/*virtual memory constants*/
#define MAXMEMSLOTS 20
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
	} *tp;			/*address of text of each line*/
	
sint cpm2x;			/*operating system is CP/M 2.x*/
int allocp;			/*location of next free in newpage */
int pageloc[MAXSLOTS];		/*current location of each page*/
int usage[MAXMEMSLOTS];		/*records usage of in memory slots*/
char *slotaddr[MAXMEMSLOTS];	/*memory addr of start of each in memory slot*/
sint dskslots[MAXSLOTS];	/*is disk slot free*/
sint slotsinmem;		/*max no. of slots available in memory*/
sint tppages;			/*no. of pages used to hold text pointers*/
sint newpage;			/*page no of page being filled with text*/
char *npaddr;			/*base of slot holding newpage*/
int clock;			/*counter for least recently used paging*/

int pagefd;			/*file descriptor of paging file*/
char pagingfile[FILELEN];	/*name of paging file*/
char pagingdisk;		/*disk to put paging file on*/

sint curdsk;			/*currently logged in (default) disk*/

unsigned ncommand;		/*count of commands executed since startup*/
char mainmenu[JBUFSIZE];	/*for emergency longjump*/
int goodline;			/*line number we know we can display */
unsigned errold[4];		/*temp store for CP/M's error jump addrs*/
char dskerr[JBUFSIZE];		/*for bdos disk error traps*/

char textbuf[IOBUFSIZ];		/*io buffer for file being edited*/

struct histstep {		/*command history stack for undoing */
	struct addr histp;	/*address of text modified by command*/
	int histline;		/*line number of modified text*/
	sint histtype;		/*type of command (insert,delete,replace)*/
	sint histcomm;		/*number of command modulo 256 */
	} history[HISTLEN];
int histptr;			/*pointer into history stack*/
sint histcnt;			/*number of commands on stack*/
sint storehist;			/*commands only stored on history when true*/

sint cursorx, cursory;		/*current position of cursor*/
char tran[NKEYS];		/*translation table from external to internal
				key codes*/
sint isdim;			/*last char was in dim*/
int lastl;			/*number of last line of text*/
int lastread;			/*last line actually read to date*/
sint goteof;			/*true when eof of file being edited*/
int cline;			/*current line*/
char text[LLIM];		/*buffer for current line of text*/
int charn;			/*position of current char in current line*/
sint altered;			/*if YES, current line has been altered by edits*/

int pfirst, plast;		/*top and bottom line displayed on screen*/
sint topline;			/*first line on screen for text*/
sint offset;			/*first col. to display on screen*/
sint lastoff;			/*offset last time resetcursor() was called*/
sint blankedmess;		/*message area has been blanked, not overwritten*/

char inbuf[INBUFLEN];		/*input buffer for typeahead*/
sint  inbufp;			/*pointer to next free in inbuf*/

			/*environment options*/
			
sint autoin;			/*do auto indentation*/
sint backup;			/*at end, old version becomes file.bak*/
sint tabwidth;			/*width between tabstops*/
sint trail;			/*don't chop trailing blanks from text lines*/
sint wboot;			/*do a warm boot on exit; else jump to CCP*/
sint displaypos;		/*display line:col at top of screen*/
sint blockscroll;		/*horizontal scroll whole page*/
sint hilight;			/*make current line bright, rest dim*/

char filename[FILELEN];		/*current file being edited*/
char *errmess;			/*last error message*/

char ans[6];			/*line to jump to*/
char patt[FLIM];		/*pattern to find*/
char changeto[FLIM];		/*replacement string*/
char opts[5];			/*options for alter*/
sint replace;			/*last find/replace was a replace*/
int findir;			/*direction of last find*/
sint nocheck;			/*don't ask for confirmation before replacing*/
sint repeat;			/*repeat last find/replace*/
sint nocase;			/*ignore case in comparison*/

char name[FILELEN];		/*filename for read and write files*/

sint helpon;			/*display help menu*/

int from,to;			/*block start and end*/
sint blocking;			/*true if block is or has been defined*/

char fbuf[IOBUFSIZ];		/*file buffer for all file ops other than edited file*/

struct iobuffer {
	int _fd;
	int _nleft;
	char *_nextp;
	char _buff[NSECTS * SECSIZ];
	};			/*the structure of the file buffers*/
	
/*end of standard header*/
e;			/*current line*/
char text[LLIM];		/*buffer for curren