/* header file for windowing system
*
*	Copr. (c) Marietta Systems, Inc.  1987,1988,1989
*	All rights reserved
*	Version 1.2x
*
*	This header file declares the functions for the windowing system
*   Globals for the windowing system.
*
*/

/*
* * * * * * * * *
* 	Figure out which compiler is being used
*
*/
#ifdef _CDECL
#define QUICKC
#endif
#ifdef _Cdecl
#define TURBOC
#endif
#ifndef LATTICE
typedef unsigned char byte;
#define _STACK _stklen
#endif

/*
*/
enum fld_type {alphanum, graphic, flag, boolean, calendar, numeric, value,
	real, chrono};
enum COLOR_OF {Mblack,Mblue,Mgreen,Mcyan,Mred,Mmagenta,Myellow,Mwhite};
enum _JUST {left, right, as_typed, center, code, decimal, db_number,
	 c_number, date, _time};
enum ATTRIB {low,high,reverse,blink,blank,alt_low,alt_high,alt_reverse}
	OLD_BRIG = low;
enum F_READ {firstrec, previous, nextrec, lastrec, random, relative};
enum F_MODE {append, readonly, recreate, update};
enum F_TYPE {ascii, binary, dbase3, hashed, marietta};
struct CURSR {int X,Y;} _CURSOR; /* Current cursor position in window */

struct WINDW {  /* window tracking struct */
	void *sobj;  /* pointer to list of screen objects */
	byte *SCREEN, *FUNCTION;
	struct CURSR LOCN;
	int X, Y;   /* location of top left corner of window */
	int H, W;   /* height and width of window, including borders */
	int P;
	int CODE, K, Z, sequ, tx, ty;
	} *WINDOW;

struct CLR_TYPE {
	enum COLOR_OF FORE, BACK;
	int ALT_CLR;
	};

struct F_DEF { /* file definition control struct */
	long prime;        /* size of file in records */
	long location;
	long r_count;      /* active (IN_USE) record count (set by program) */
	byte *record;      /* pointer to start of record */
	void *fnext;       /* pointer to data dictionary */
	void *fptr[5];
	enum F_TYPE ftype; /* type of file */
	int key_len, offset; /* key info for hashed files */
	int start;         /* size of header area at start of file */
	unsigned rec_len;  /* record length */
	unsigned fbloc;
	int keycode;
	int ismemo;        /* set 1 if .DBF file with associated memo file */
	int write;
	int ndx;
	int realfile;      /* contains the real file handle of the file */
	unsigned bufsize;
	char fvol;         /* disk/volume code */
	char fpath[61];    /* directory path */
	char fname[13];    /* file name with extension */
	} *FN;

struct DBF_DEF { /* dBase III Plus data dictionary struct */
	void *dnext;
	enum _JUST just;   /* justification code of field (modifyable) */
	int dbf_len;       /* dBASE field length */
	int dbf_dec;       /* decimals for an N type field */
	int dbf_set;       /* Active field (set by program) */
	int dbf_dig;       /* justify() and accept() 'len' parameter */
	unsigned posn;     /* position of start of field in record */
	unsigned ndxfield;
	byte dbf_name[11]; /* field name in ASCIIZ format */
	byte dbf_type;     /* Type (C, D, L, M, N) */
	};

/*
* * * * * * * * * * * *
*   Initiation of globals for the windowing system.
*
*/
long _SIZMEM;
unsigned SCRN_PTR, CLR_PTR = 0X0000, _DSEG, _DOFF, CLR_SEG = 0XB800,
	MONO_SEG = 0XB000;
unsigned short SCR_SIZE, OLD_CODE, _brkflag;
unsigned INCHAR, INCHAR_F;
int INS_FLAG = 1, W_NUM = 0, CLR_INIT = 0;
int COLOR_SC, EGA_SCRN, CLR_CODE;
byte *MEM_LINE;
char *xgoodbye;

struct CLR_TYPE COLOR [7] = {	/* number of elements must match
								 * "_COLORS" configuration constant */
		{Mgreen, Mblack, 6},    /* base window color, #0 */
		{Mwhite, Mblue, 3},	    /* color #1 */
		{Mcyan, Mblack, 4},	    /* color #2 */
		{Mcyan, Mred, 5},	    /* color #3 */
		{Mwhite, Mmagenta, 2},  /* color #4 */
		{Myellow, Mblack, 1},   /* color #5 */
		{Mwhite, Mred, 0},	    /* color #6 */
				};
struct CLR_TYPE	TOP_CLR = {Myellow,Mblue,6}; /* top line color */
struct CLR_TYPE	ER_COLOR = {Mgreen,Mred,6};  /* bottom line color */

unsigned KEYMATCH[8] /* the key match array which determines which
					  * function keys are treated as 'end-of-field'
					  * keys by the 'isendch' function */
    = {0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0};

byte *KEY_DISPLAY[7] = {"Srch", "Move", "Ins", "Del", "Zoom","Amnd", "AKey"};
    /* meanings for function keys F3 through F9 displayed on bottom line */

byte *BANNER_MSG = /* Modifyable initial window display on software loading */
" The \"c_wndw\" (tm) and \"c_ndx\" (tm) toolkits \n\
 are the copyright of Marietta Systems, Inc. \n\
 1987, 1988, 1989.  All rights reserved. \n\
  Release Version 2.01 - 8/30/89 ";

/*
* * * * * * * * * * * * *
*	Configuration constants
*
*	Change their initial value to suit your set-up
*		- but be careful!!
*
*/
int CLR_TONE = 25;   /* controls display of copywrite box by 'clr_scrn()' */
int A_TRACK = -1;    /* used by 'accept()' function */
int SAVEWAIT = 900; /* clock ticks before 'scrnsave(0)' flushes file buffers */
enum ATTRIB ACC_DISP = reverse; /* Attribute of accept() fields after entry */
int ERR_BEEP = 2;	/* For beep length with 'shortbeep()', 0 = tiny beep */
int UP_NAME = 0;	/* 1 if names are to be in uppercase only */
int FLD_FULL = 1;   /* 1 for auto ENTER on full field; 0 = ENTER required */
int TAB_SET = 4;	/* size of tab increments on screen for '\t' code */
int _WINDW = 10;	/* maximum number of windows */
int _COLORS = 7;	/* maximum number of different color selections */
int SCRN_LEN = 25;  /* screen length in lines */
int SCRN_WID = 80;  /* screen width in columns */
int TOP_LINE = 1;   /* lines dedicated to the "top-frame" at top of screen */
int BOT_LINE = 1;   /* True if active keys to be displayed on bottom line */
int MENULINE = 1;	/* True to display assist line for menus */
int ERRORMSG = 1;   /* True to display non-critical errors with sys_wndw() */
int GOODBYEMSG = 1; /* True to display end of program message */
int MAXFILES = 9;   /* maximum number of files to create file buffers */
int D_FORMAT = 'U'; /* U for USA format, E for european, I for intl */
int D_PUNCT = '/';	/* punctuation character for date formats */
int D_20XX = 10;    /* split between 20xx and 19xx in years */
int PERIOD = '.';	/* decimal point character */
int COMMA = ',';	/* thousands seperator */
int BOOL_YES = 'Y'; /* The TRUE or YES boolean entry code */
int BOOL_NO = 'N';  /* The FALSE or NO boolean entry code */
int CLOCK24 = 0;	/* 0 for am/pm, or 1 for 24 hour european/military time */
int QTRHOUR = 0;    /*  */
int T_PUNCT = ':';  /* time punctuation character, default colon */
unsigned DISCBLCK = 4100; /* maximum record size of any file to be opened,
		   * Minimum size is 512, but recommended minimum is 2060.
		   * if actual record size is less than one third of this size,
		   * the file access software will perform disk caching
		   * Maximum size is 65000 */
short MEM_WARN = 1024; /* Warning will be issued when unused heap memory is
						* under this.  Maximum is 9999, minimum is 0 */
long MAX_WIND = 15000L; /* maximum amount of heap space for windowing
		  * functions. The maximum size is 65000, and the minimum size
		  * of MAX_WIND should be 8500. To calculate, use the formula:-
		  * ((number of windows * average size * 2) + 2500)
		  * where 'average size' is (height * width)   */
long _MNEED = 20000L; /* minimum heap space needed, which must be
		  * at least MAX_WIND + 5000. */
unsigned _STACK = 10000; /* size of stack, (at least 7000) */

/*
* * * * * * * * * * *
* 	This section contains the #define codes
*
*/
#define CTL_A     1 /* all the other CTL combinations can be defined here */
#define PGDN      3
#define CRS_RT    4 /* Right cursor */
#define CRS_UP    5 /* Up cursor */
#define DEL       7
#define B_SPACE   8
#define TAB       9
#define CTL_ENT  10
#define SFT_TAB  11
#define ENTER    13
#define PGUP     18
#define CRS_LT   19 /* Left cursor */
#define CTL_BSPC 20 /* Ctrl + backspace */
#define INS      22
#define HOME     23
#define CRS_DN   24 /* Down cursor */
#define END      26
#define CTL_Z    26 /* 0X1A */
#define ESC      27
#define ALT_Q   129 /* The other ALT alpha keys can be defined here */
#define ALT_P   138 /* Print translated screen */
#define QUIT_NOW 158 /* Alt+X used for fast terminate of program */
#define ALT_V   160 /* display version information */
#define ALT_M   163
#define HELP    168 /* F1 Key  */
#define UNDO    169 /* F2 Key  */
#define SEARCH  170 /* F3 Key  */
#define MOVE    171 /* F4 Key  */
#define INSERT  172 /* F5 Key  */
#define DELETE  173 /* F6 Key  */
#define ZOOM    174 /* F7 Key  */
#define AMEND   175 /* F8 Key  */
#define KEY_CH  176 /* F9 Key  */
#define QUIT    177 /* F10 Key */
/* 174 thru 218 reserved for standard graphics characters */
#define SFT_F1  220 /* The other SHIFT+Fnn keys can be defined here */
#define SFT_F10 229
#define CTL_LT  248 /* Ctrl + left cursor */
#define CTL_RT  249 /* Ctrl + right cursor */
#define CTL_END 250
#define CTL_PGDN 251
#define CTL_HOME 252
#define CTL_PGUP 254
/* 		*/
#define DELETED 0X2A /* '*' code has dBase 3+ compatibility */
#define IN_USE  0X20 /* ' ' code has dBase 3+ compatibility */
#define EMPTY   0X26 /* '&' code, for hashed files */
#define SYS_REC 0X03 /* code has dBase 3+ compatibility */
/*		*/
#define MFALSE  128
#define MTRUE   129

/*
* * * * * * * * * * * * * * * * *
*	The prototypes of functions used by "c_wndw" (tm) software
*
*/
 int   accept (byte*,enum _JUST,enum ATTRIB,int,int);
 int   acptbig (byte*, enum _JUST, enum ATTRIB, unsigned, unsigned);
 int   acptchar (char*, enum _JUST, enum ATTRIB, unsigned);
 int   acptdbl (double*, enum _JUST, enum ATTRIB, unsigned, unsigned);
 int   acptint  (int*, enum _JUST, enum ATTRIB, unsigned);
 int   acptlong (long*, enum _JUST, enum ATTRIB, unsigned);
 int   acptmask (byte*, byte*, enum ATTRIB);
 int   acptmenu (byte*, byte*, enum ATTRIB, unsigned);
 void  clr_keys (void);
 void  clr_scrn (char*);
 void  clr_wndw (void);
 byte* concat (byte*, int);
 int   datecomp (char*, char*);
 long  datelong (char*);
 void  datemath (char*, int);
 void  dateread (char*);
 void  date_in (char*, char*);
 void  date_out (char*, char*);
 void  display (byte*, int, int, enum ATTRIB);
 int   dispchar (char, int, int, enum ATTRIB, enum _JUST, unsigned);
 int   dispdbl (double, int, int, enum ATTRIB, enum _JUST, unsigned, unsigned);
 int   dispint (int, int, int, enum ATTRIB, enum _JUST, unsigned);
 int   displong (long, int, int, enum ATTRIB, enum _JUST, unsigned);
 void  dispkeys (void);
 int   dispmask (byte*, int, int, enum ATTRIB, byte*);
 int   dispwndw (unsigned, byte*, unsigned, unsigned, enum ATTRIB);
#define disp_err(s,i)  err_wndw(s, i, 0)
#define disp_msg(s,i)  err_wndw(s, i, 0)
#define disp_qry(s)    qry_wndw(s)
 int   err_wndw (byte*, int, int);
 int   fileback (char*);
#define fileclose fileclos
 int   fileclos (int);
 int   fileinit (int, unsigned, unsigned, long);
 int   fileopen (char*, enum F_TYPE, enum F_MODE);
 int   fileread (int, enum F_READ, long*);
 long  filerecno (int);
 int   fileseek (int, long);
#define filewrite filewrit
 int   filewrit (int, long*);
 int   file_err (int, int);
 int   fld_len (enum _JUST, unsigned, unsigned);
 int   flt_wndw (unsigned, unsigned, byte*);
 void  goodbye (short);
 int   grabchar (void);
 void  helpmenu (int);
 int   helpscrn (byte*);
 void  idleloop (int);
 int   isendch (byte*, int, enum ATTRIB);
 byte* justify (enum _JUST, byte*, byte*, unsigned, unsigned);
 int   lastline (byte*, unsigned, enum ATTRIB);
 int   maskfield (byte*, byte*, byte*);
 int   maskvalid (byte*, byte*);
 int   menu (char*, int, int, int);
 int   mk_wndw (int, int, int, int, byte*);
 long  primenbr (long);
 void  prn_scrn (void);
 int   qry_wndw (byte*);
 void  read_kb (void);
 int   rm_wndw (void);
 void  scrnsave (int);
 void  scrnline (byte*, unsigned, int);
 void  scrn_map (byte*, int, int);
 void  scroll (int, int);
 void  set_clr (int, enum ATTRIB);
 void  set_crsr (int, int);
 void  set_keys (unsigned, unsigned, ...);
 int   set_wndw (unsigned, enum _JUST, enum ATTRIB, unsigned);
 void  shortbeep (int);
 int   sys_wndw (byte*, int, int);
 int   top_menu (char*, int, int);
 void  top_spot (int);
 void  unset_keys (unsigned, unsigned, ...);
 int   validate (byte*, enum _JUST, unsigned, unsigned);
 int   v_bool (int);
 void  warble (int);

/* 	end of header file */
