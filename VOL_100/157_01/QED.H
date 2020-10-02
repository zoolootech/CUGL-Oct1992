/*  VERSION 0007  (DATE: 29/04/87)  (TIME: 12:41)  */
/*
	e screen editor  (qed)

	(C) G. Nigel Gilbert, MICROLOGY, 1981
	licensed for private non-profitmaking use 1983
	
	August-December 1981

	Modified: Aug-Dec 	1984:  BDS-C 'e' to 'qe' 			(J.W.Haefner)
			  March		1985:  BDS-C 'qe' to DeSmet-C 'qed' (J.W.Haefner)

	FILE: qed.h

	PURPOSE: standard header file, #included in all function files
		(NB e does NOT require "bdscio.h")

*/

#define VERSION "4.6b/QED:1.23"

#define STKSPACE 1536			/*space for the stack*/
#define SECSIZ 128				/*file buffer definitions*/
#define NSECTS 16
#define IOBUFSIZ (NSECTS * SECSIZ + 6 )
#define FILELEN 60				/*max length of file name*/
#define PATHLEN 24				/*max length of path*/

#define JBUFSIZE 6				/*size of buffer required for longjumps*/

#define LLIM 255				/*max length of a line*/
#define FLIM 80					/*max length of find/alter string*/

#define FAIL -1
#define DFAIL 0xff				/*DeSmet read fail flag for returning char*/
#define NULL 0
#define YES 1
#define NO 0
#define FALSE 0
#define TRUE 1
#define PREV -2

#define DOS2 (YES)
#define CPM (NO)
#define IBMPC (YES)

#if IBMPC
#define NOTIBMPC (NO)
#else
#define NOTIBMPC (YES)
#endif

/* -- compile flags for major editor options -- */
#define	SCRNDIMMD (NO)	/*yes==>highlite current line,no==>all brite */
#define WWRAP (YES)		/*compile word wrap code*/
#define PRINTALL (YES)	/*send all chars to printer*/
#define VRSNNUM (YES)	/*put version number at file top on save*/
#define VRSNDATE (YES)  /*put version date/time w/ version num*/

#define NOCLOCK (NO)	/*NO ==> have a clock*/
#define VRSNLEN 56		/*length of version string*/

#define SWIDTH 79		/*screen size*/
#define SHEIGHT 23
#define OFFWIDTH 20		/*horizontal scroll increment*/
#define PAGEOVERLAP 4	/*overlap between pages with UP,DOWN-PAGE*/
#define HELPLINES 8		/*top line number when help menu is on display */
#define GOTOLEN 6		/* approx len of contrl line */

#define FNPOS 8			/*status line - position of file name*/
#define LNPOS 23		/*position of line number*/
#define EMPOS 32		/*position of error message*/
#define WAITPOS 32		/*position of cursor when searching/reading*/
#define REPPOS 50		/*position of "Replace?" message*/

#define BACKSP 0x08		/*backspace*/
/*#define ENDFILE 0x1a*/	/*CP/M end of text file mark */
#define ENDFILE 0xff	/*for MSDOS 2.x and DeSmet-C, 28.iv.87*/
#define CTRL 0x1f		/*last control char.*/
#define PARBIT 0x80		/*parity bit used as a flag by testkey() */
#define NKEYS 32		/*number of key codes*/

	/*internal key codes (recommended key assignments in brackets)*/

#define LEADIN 0		/*lead in chararacter to key codes -
				ignored, but following key code parity set*/
#define DOWNKEY 1		/*cursor down */
#define UPKEY   2		/*cursor up*/
#define LEFTKEY 3		/*cursor left*/
#define RIGHTKEY 4		/*cursor right*/
#define RIGHTWKEY 5		/*cursor right one word (D) */
#define LEFTWKEY 6		/*cursor left one word (S) */
#define EOLKEY 7		/*cursor to end of line (E) */
#define BOLKEY 8		/*cursor to beginning of line (B) */
#define UPPAGE 9		/*scroll up a page (W) */
#define DOWNPAGE 10		/*scroll down a page (Z) */
#define BOFKEY 11		/*cursor to beginning of file (U) */
#define HOMEKEY 12		/*cursor to end of file (HOME) */
#define DELLEFT 13		/*delete char to left of cursor (DEL) */
#define DELRIGHT 14		/*delete char under cursor (G) */
#define DELLNKEY 15		/*delete cursor line (Y) */
#define DELWDKEY 16		/*delete word to right of cursor (T) */
#define JUMPKEY 17		/*jump to (X) */
#define CRSTILL 18		/*insert newline after cursor (N) */
#define QWIKKEY 19		/*quit (Q) */
#define SCRLDNKEY 20	/*edit and file context (C) */
#define TOPSCRKEY 21	/*find (F) */
#define BOTSCRKEY 22	/*alter (A) */
#define BLOCKKEY 23		/*block operations (O) */
#define SCRLUPKEY 24	/*read a file (P) */
#define REPKEY 25		/*repeat last find/alter (R) */
/*#define HELPKEY 26*/		/*display help menu (V) */
#define UNDELINE 26		/*restore deleteted line below cursor*/
#define OOPSKEY 27		/*restore unedited line (\) */
#define TAB 28			/*tab (I)*/
#define RETRIEVE 29		/*retrieve last name etc entered here (R)*/
#define CR 30			/*return (M)*/
#define ESCKEY 31		/*the magic escape key (ESC)*/

	/* key codes + 0x80 (PARBIT) used in scanning strings of ctrl chars */
#define LEFTKEY_P	131
#define	DELLEFT_P	141
#define RETRIEVE_P	157
#define	CR_P		158
#define	ESCKEY_P	159

#define BRIDIM '|'		/*char placed in e's message strings to start/stop
				dim (half-intensity) display */

		/* CP/M and MSDOS bdos function codes */

#define DOSINT 0x21

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
#define FREESP	0x36
#define SETDIR	0x3b
#define GETDIR  0x47
#define FIND1	0x4e
#define FINDN	0x4f

#define MAXSLOTS 1024		/*virtual memory constants*/
#define MAXMEMSLOTS 64		/*36 for 64k TPA (CP/M816); old=20*/
#define PAGESIZE 1024
#define PAGESECTS (PAGESIZE/SECSIZ)

#define FREE 0
#define INUSE 1
#define NOTAVAIL -1
#define MAXINT 32767
#define NOFILE (MAXINT)
#define ABSOLUTE 0
#define UNKNOWN (MAXINT)

#define sint int			/*carry-over from 'e', just in case*/

struct addr {
	int page;
	int moffset;
	} *tp;					/*address of text of each line*/

int allocp;					/*location of next free in newpage */
int pageloc[MAXSLOTS];		/*current location of each page*/
int usage[MAXMEMSLOTS];		/*records usage of in memory slots*/
char *slotaddr[MAXMEMSLOTS];	/*memory addr of start of each in memory slot*/
char dskslots[MAXSLOTS];	/*is disk slot free*/
int slotsinmem;				/*max no. of slots available in memory*/
int tppages;				/*no. of pages used to hold text pointers*/
int newpage;				/*page no of page being filled with text*/
char *npaddr;				/*base of slot holding newpage*/
int clock;					/*counter for least recently used paging*/

int pagefd;					/*file descriptor of paging file*/
char pagingfile[FILELEN];	/*name of paging file*/
char pagingdisk;			/*disk to put paging file on*/

int curdsk;					/*currently logged in (default) disk*/
char bakpath[PATHLEN];		/*default path for backup*/

char mainmenu[JBUFSIZE];	/*for emergency longjump*/
int goodline;				/*line number we know we can display */
unsigned errold[4];			/*temp store for CP/M's error jump addrs*/
char dskerr[JBUFSIZE];		/*for bdos disk error traps*/

char textbuf[IOBUFSIZ];		/*io buffer for file being edited*/

int cursorx, cursory;		/*current position of cursor*/
char tran[NKEYS];			/*translation table from external to internal
								key codes*/
char isdim;					/*last char was in dim*/
int lastl;					/*number of last line of text*/
int lastread;				/*last line actually read to date*/
char goteof;				/*true when eof of file being edited*/
char changed;				/*true if file has been modified*/
int cline;					/*current line*/
char text[LLIM];			/*buffer for current line of text*/
char textb[LLIM];			/*buffer for last deleted line*/
int charn;					/*position of current char in current line*/
char altered;				/*if YES, current line has been altered by edits*/

int pfirst, plast;			/*top and bottom line displayed on screen*/
int topline;				/*first line on screen for text*/
int offset;					/*first col. to display on screen*/
int lastoff;				/*offset last time resetcursor() was called*/
char blankedmess;			/*message area has been blanked, not overwritten*/

char inbuf[20];				/*input buffer for typeahead*/
int  inbufp;				/*pointer to next char in inbuf*/

			/*environment options*/
			
char autoin;				/*do auto indentation*/
char backup;				/*at end, old version becomes file.bak*/
char readall;				/*to read as much text as possible*/
char tabwidth;				/*width between tabstops*/
char trail;					/*don't chop trailing blanks from text lines*/
char wboot;					/*do a warm boot on exit; else jump to CCP*/
char displaypos;			/*display line:col at top of screen*/
char helpon;				/*display help menu*/
char expert;				/*expert mode, brief prompts*/
char blockscroll;			/*horizontal scroll whole page*/
#if WWRAP
int  rtmarg;				/*right margin for wordwrap*/
#endif
#if VRSNNUM
char version;				/*flag put version line on file top*/
#endif

char filename[FILELEN];		/*current file being edited*/
char justfile[16];			/*filename w/o path*/
char defext[4];				/*default file extension */
char *errmess;				/*last error message*/

char ans[9];				/*line to jump to*/
char patt[FLIM];			/*pattern to find*/
char changeto[FLIM];		/*replacement string*/
char opts[5];				/*options for alter*/
char replace;				/*last find/replace was a replace*/
int findir;					/*direction of last find*/
char nocheck;				/*don't ask for confirmation before replacing*/
char repeat;				/*repeat last find/replace*/

char name[FILELEN];			/*filename for read and write files*/

int jmpto,initjmp;			/*initial line to jump */

int from,to;				/*block start and end*/
char blocking;				/*true if block is or has been defined*/

char fbuf[IOBUFSIZ];  		/*file buffer for all file ops other than edited file*/

struct iobuffer {
	int _fd;
	int _nleft;
	char *_nextp;
	char _buff[NSECTS * SECSIZ];
	};						/*the structure of the file buffers*/

/* globals for version control compile option*/
#if VRSNNUM
char comnt[5];			/*comment symbols for version line*/
char *comntsym();		/*get comment symbol for this file*/
char date[9];			/*version date*/
char time[6];			/*version time*/
#endif

char *getline(), *strcpy();
char getkey(), getlow(), getscankey(), inkey(), testkey(), testlow();

/* DeSmet C variables for 8086 registers and flags */
extern unsigned _rax, _rbx, _rcx, _rdx, _rsi, _rdi, _res, _rds;
extern char _carryf, _zerof;

#if IBMPC
/*DeSmet C vars for ibm pc scrn functions */
extern char scr_cols,scr_rows,scr_mode,scr_page,scr_attr; 
#endif

/*end of standard header*/
