/*
Header:          CUG199;
Title:           Header include file of ged editor DeSmet, MSC and TurboC;
Last Updated:    11/29/87;

Description:    "PURPOSE: standard header file, included in all function files,
                 NB. e/qed/ged does NOT require <stdio.h>)";

Keywords:        e, editor, qed, ged, DeSmet, MSDOS;
Filename:        ged.h;
Warnings:        must be included in all ged files

Authors:         G. Nigel Gilbert, James W. Haefner, and Mel Tearle;
Compilers:       DeSmet 3.0;

References:
Endref
*/

/*
e/qed/ged screen editor

(C)   G. Nigel Gilbert, MICROLOGY, 1981 - August-December 1981
      licensed for private non-profitmaking use 1983

Modified:  Aug-Dec   1984:  BDS-C 'e'(vers 4.6a) to 'qe' (J.W. Haefner)
           March     1985:  BDS-C 'qe' to DeSmet-C 'qed' (J.W. Haefner)

           May       1986:  qed converted to ged         (Mel Tearle)
           August    1987:  ged converted to MSC 4.0     (Mel Tearle)

File:      ged.h

*/

#include  <dos.h>

#ifdef  MSC
#include  <stdio.h>
#include  <stdlib.h>
#include  <ctype.h>

#include  <io.h>
#include  <direct.h>
#include  <fcntl.h>
#include  <process.h>

#include  <malloc.h>
#include  <string.h>
#include  <sys\types.h>
#include  <sys\stat.h>
#include  <setjmp.h>
#endif                   /* MSC */


#ifdef  TC
#include  <stdio.h>
#include  <stdlib.h>
#include  <ctype.h>

#include  <io.h>
#include  <fcntl.h>
#include  <process.h>

#include  <mem.h>
#include  <string.h>
#include  <sys\stat.h>

#define   _memavl  coreleft
#endif                /* TC */


/* fixes for DeSmet - MSC/TC
 */
#ifndef  DS
#define  index         strchr
#define  _os(a,b)      bdos(a,b,0)
#define  creat(file)   creat(file,S_IREAD|S_IWRITE)
#endif


#define STKSPACE   1024      /* space for the stack */
#define SECSIZ      128      /* file buffer definitions */
#define NSECTS       16
#define IOBUFSIZ ( NSECTS * SECSIZ + 6 )
#define FILELEN      64      /* max length of file name */

#define JBUFSIZE   6     /* size of buffer required for longjumps */
#define BUFFER    16     /* generic buffer size */
#define REFORM    72     /* default line size used in reformatting paragraph */
#define FLIM      80     /* max length of find/alter string */
#define LLIM     255     /* max length of a line */

#define FAIL      -1
#define DFAIL   0xff     /* DeSmet read fail flag for returning char */
#define NULL       0
#define YES        1
#define NO         0
#define FALSE      0
#define TRUE       1
#define PREV      -2

#define HELP          1    /* table assignments */
#define TABLE1        2
#define TABLE2        3
#define WINDOW        9    /* last line of display */

#define PAGEOVERLAP   1    /* overlap between pages with UP,DOWN-PAGE */
#define OFFWIDTH     32    /* horizontal scroll increment */

#define SHEIGHT      23    /* screen height */
#define PATHLEN      32    /* display lenght of path */
#define SWIDTH       79    /* screen width */

#define NLINES        6    /* number of lines to skip at bottom of page */
#define PAGELEN      66    /* length of page, used in listfile() */

#define EMPOS         0    /* position of error message */
#define WAITPOS       0    /* position of cursor when searching/reading */
#define LNPOS         0    /* position of line number */
#define COLPOS       10    /* position of column number */
#define REPPOS       19    /* position of "Replace?" cursor */

#define CAPLK        19    /* position of capslock message */
#define FNPOS        27    /* status line - position of file name */
#define OVERT        64    /* position of overstrike message */
#define TIMEPOS      75    /* position of current time */

#define BACKSP     0x08    /* backspace */
#define ENDFILE    0x1a    /* CP/M end of text file mark */
#define CTRL       0x1f    /* last control char */
#define PARBIT     0x80    /* parity bit used as a flag by testkey() */
#define NKEYS        32    /* number of key codes */

#define HISTLEN     100    /* history stack of commands for undoing */
#define HISTINSERT    1    /* type of command on history stack */
#define HISTDELETE    2
#define HISTREPLACE   3

/* Internal key codes - recommended key assignments in brackets
 */
#define LEADIN     0     /* lead in chararacter to key codes -
                            ignored, but following key code parity set */
#define DOWNKEY    1     /* cursor down */
#define UPKEY      2     /* cursor up */
#define LEFTKEY    3     /* cursor left */
#define RIGHTKEY   4     /* cursor right */
#define RIGHTWKEY  5     /* cursor right one word (D) */
#define LEFTWKEY   6     /* cursor left one word (S) */
#define EOLKEY     7     /* cursor to end of line (E) */
#define BOLKEY     8     /* cursor to beginning of line (B) */
#define UPPAGE     9     /* scroll up a page (W) */
#define DOWNPAGE  10     /* scroll down a page (Z) */
#define BOFKEY    11     /* cursor to beginning of file (U) */
#define HOMEKEY   12     /* cursor to end of file (HOME) */
#define DELLEFT   13     /* delete char to left of cursor (DEL) */
#define DELRIGHT  14     /* delete char under cursor (G) */
#define DELLNKEY  15     /* delete cursor line (Y) */
#define DELWDKEY  16     /* delete word to right of cursor (T) */
#define JUMPKEY   17     /* jump to (X) */
#define CRSTILL   18     /* insert newline after cursor (N) */
#define QWIKKEY   19     /* quit (Q) */
#define SCRLDNKEY 20     /* edit and file context (C) */
#define TOPSCRKEY 21     /* find (F) */
#define BOTSCRKEY 22     /* alter (A) */
#define BLOCKKEY  23     /* block operations (O) */
#define SCRLUPKEY 24     /* read a file (P) */
#define REPKEY    25     /* repeat last find/alter (R) */
#define HELPKEY   26     /* display help menu (V) */
#define OOPSKEY   27     /* restore unedited line (\) */
#define TAB       28     /* tab (I) */
#define RETRIEVE  29     /* retrieve last name etc entered here (R) */
#define CR        30     /* return (M) */
#define ESCKEY    31     /* the magic escape key (ESC) */


/* key codes + 0x80 (PARBIT) - used in scanning strings of ctrl chars
 */
#define LEFTKEY_P   131
#define DELLEFT_P   141
#define RETRIEVE_P  157
#define CR_P        158
#define ESCKEY_P    159

#define F1KEY       128   /* these chars are now off limits */
#define F2KEY       129   /* but I doubt if it will matter much */
#define F3KEY       130
#define F4KEY       145
#define F5KEY       132
#define F6KEY       133
#define F7KEY       134
#define F8KEY       135
#define F9KEY       136
#define F10KEY      137
#define INSKEY      138
#define CAPSLKON    139
#define CAPSLKOFF   140
#define ALTKEY      146
#define GREYMINUS   142
#define GREYPLUS    143


/* screen attributes */

#define BRIDIM       '|'   /* char placed in message strings to start/stop
                              dim ( half-intensity ) display */

#define DIM            4
#define HIGH          13
#define RSFW         104
#define REVS         112

#define dim()    attr = DIM
#define high()   attr = HIGH


/* dos interrupts */

#define DOSINT      0x21
#define SELDSK      0x0E
#define CURDSK      0x19
#define LSTOUT      0x05
#define SETDIR      0x3b
#define GETDIR      0x47


/* file management */

#define MAXSLOTS     255      /* virtual memory constants */
#define MAXMEMSLOTS   64      /* 36 for 64k TPA (CP/M816); old = 20 */
#define PAGESIZE    1024
#define PAGESECTS  ( PAGESIZE/SECSIZ )

#define FREE           0
#define INUSE          1
#define NOTAVAIL      -1
#define MAXINT     32767
#define NOFILE   (MAXINT)
#define ABSOLUTE       0
#define UNKNOWN  (MAXINT)
#define VERNO    0x30        /* get MSDOS version number */

#define debug   scr_delete(0,24);       /* 24th line */


/* the structure of the file buffers
 */
struct iobuffer  {
  int   _fd;
  int   _nleft;
  char *_nextp;
  char  _buff[ NSECTS * SECSIZ ];
};


/* address of text of each line
 */
struct addr  {
  int  page;
  int  moffset;
}  *tp;


/* command history stack for undo()
 */
struct histstep  {
  struct addr histp;    /* address of text modified by command */
  int histline;         /* line number of modified text */
  int histtype;         /* type of command (insert,delete,replace) */
  int histcomm;         /* number of command modulo 256 */
}   history[HISTLEN];


int histptr;            /* pointer into history stack  */
int histcnt;            /* number of commands on stack */
int storehist;          /* commands only stored on history when true */

unsigned ncommand;      /* count of commands executed since startup  */

int allocp;             /* location of next free in newpage */
int attr;               /* attribute for norm() and high()  */
int pclock;             /* counter for least recently used paging */

int newpage;                 /* page no of page being filled with text */
int pagefd;                  /* file descriptor of paging file */
int pageloc[MAXSLOTS];       /* current location of each page */
int slotsinmem;              /* max no. of slots available in memory */
int tppages;                 /* no. of pages used to hold text pointers */
int usage[MAXMEMSLOTS];      /* records usage of in memory slots */

char dskslots[MAXSLOTS];      /* is disk slot free */
char *npaddr;                 /* base of slot holding newpage */
char pagingfile[FILELEN];     /* name of paging file */
char pagingdisk;              /* disk to put paging file on */
char *slotaddr[MAXMEMSLOTS];  /* memory addr of start of each in memory slot */

char timestr[6];              /* used to in xgettime */

char altered;              /* if YES, current line has been altered by edits */
char blankedmess;          /* message area has been blanked, not overwritten */
char changed;              /* true if file has been modified */
char dskerr[JBUFSIZE];     /* for bdos disk error traps */

char goteof;               /* true when eof of file being edited */
char inbuf[BUFFER];        /* input buffer for typeahead */
char isdim;                /* last char was in dim */

char lastgraph;            /* graphic character returned from get_graphic() */
char mainmenu[JBUFSIZE];   /* for emergency longjump */

char text[LLIM];           /* buffer for current line of text */
char textbuf[IOBUFSIZ];    /* io buffer for file being edited */

unsigned char tran[NKEYS];     /* translation table from al to internal
                                  key codes */

int  charn;                /* position of current char in current line */
int  cline;                /* current line */
int  curdsk;               /* currently logged in (default) disk */
int  cursorx, cursory;     /* current position of cursor */

int  goodline;          /* line number we know we can display */
int  inbufp;            /* pointer to next char in inbuf */

int  lastl;             /* number of last line of text */
int  lastoff;           /* offset last time resetcursor() was called */
int  lastread;          /* last line actually read to date */

int  offset;            /* first col. to display on screen */
int  pfirst, plast;     /* top and bottom line displayed on screen */
int  rtmarg;            /* used in reform function */

int  screen;            /* current value of help screen */
int  topline;           /* first line on screen for text */
int  window;            /* number of lines for top psuedo-window area */

/* --------------->>  environment options  <<--------------- */

char autoin;            /* do auto indentation */
char backup;            /* at end, old version becomes file.bak */
char blockscroll;       /* horizontal scroll whole page */

char overtype;          /* INSKEY toggle */
char readall;           /* to read as much text as possible */

char trail;             /* don't chop trailing blanks from text lines */

char ans[6];            /* line to jump to */
char blocking;          /* true if block is or has been defined */
char changeto[FLIM];    /* replacement string */
char defext[4];         /* default file extension, saved from last file */

char *envstr;           /* used with getenv to hold environmental string */
char *errmess;          /* last error message */
char fbuf[IOBUFSIZ];    /* file buffer for all file ops other than edited file */
char filename[FILELEN]; /* current file being edited */

char name[FILELEN];     /* filename for read and write files */
char nocheck;           /* don't ask for confirmation before replacing */
char opts[5];           /* options for alter */

char pathname[FILELEN]; /* pathname of file being edited */
char patt[FLIM];        /* pattern to find */
char repeat;            /* repeat last find/replace */
char replace;           /* last find/replace was a replace */

int  findir;            /* direction of last find */
int  from, to;          /* block start and end */
int  jmpto, initjmp;    /* initial line to jump */
int  tabwidth;          /* width between tabstops */

#ifndef DS

jmp_buf mark;

int   caps_on(void);

int   scr_ci(void);
int   scr_setup(void);
int   scr_sinp(void);

void   scr_aputch(int,int,char,int);
void   scr_aputs(int,int,char *,int);
void   scr_clr(void);
void   scr_clrl(void);
void   scr_cls(int);
void   scr_co(int);
void   scr_cursoff(void);
void   scr_curson(void);

void   scr_delete(int,int);
void   scr_putch(char,int);
void   scr_putstr(int,int,char *,int);
void   scr_scdn(void);
void   scr_scrdn(int,int,int,int,int);
void   scr_scrup(int,int,int,int,int);
void   scr_scup(void);
void   scr_setmode(int);
void   scr_term(void);
void   scr_xy(int,int);

char   *firstwhite(char *,int ,char *);
char   *getline(int);

char   dirmsk(void);
char   get_graphic(void);
char   getkey(void);
char   getlow(void);
char   getscankey(void);
char   inkey(void);
char   scr_csts(void);
char   setstatusname(void);
char   testkey(void);
char   testlow(void);

int   askforfile(void);
int   balloc(unsigned int);
int   blockops(void);
int   blockpos(int);
int   calcoffset(void);
int   checkexists(char *);

int   dflush(struct iobuffer *);
int   dispose(void);
int   egetc(struct iobuffer *);
int   eputc(char ,struct iobuffer *);
int   exists(char *);

int   fcreat(char *,struct iobuffer *);
int   ffclose(struct iobuffer *);
int   ffopen(char *,struct iobuffer *);
int   files(void);
int   find(void);
int   finish(int);
int   freememslot(void);
int   frename(char *,char *);
int   funlink(char *);

int   getcursor(void);
int   getnumb(void);
int   inword(char);
int   jumpline(int);
int   loc(int ,int);

int   movechar(int);
int   moveline(int);
int   opentext(char *);
int   prnstat(void);
int   readfile(char *);

int   scans(char *,int);
int   seldisk(int);
int   swappin(int);
int   uspr(int ,long);
int   writefile(int ,int ,char *,char *,int);
int   writ_txb(int ,char *);

unsigned int writepage(int);

void   addhistory(int ,int);
void   adjustc(int);
void   calcjmp(void);
void   chkbuf(void);
void   crdelete(int);
void   crinsert(int);
void   curdrv(char *);

void   deletechar(int);
void   deleteword(void);
void   deltp(int);
void   display_help(void);
void   display_table1(void);
void   display_table2(void);
void   dofindrep(int);

void   edit(void);
void   envir(void);
void   error(char *);
void   fatalerror(char *);
void   findorrep(void);
void   format(char *);
void   getpath(char *);
void   gettext(int);

void   info(void);
void   initialise(void);
void   initvm(void);
void   insertchar(char);
void   keytranslate(void);
void   listfile(int ,int);

void   main(int ,char * *);
void   makebright(void);
void   makedim(void);
void   makeother(void);
void   movepage(int);
void   moveword(int);

void   putline(int ,int);
void   putlineno(int);
void   putmess(char *);
void   putonoff(int);
void   putpage(void);
void   putpart(int ,int);
void   putret(void);
void   putstatusline(int);
void   putstr(char *);
void   puttext(void);

void   readpage(unsigned int ,unsigned int);
void   readtext(int);
void   reform(int ,int);
void   resetcursor(void);
void   retag(char *,char *);
void   rewrite(int ,int);
void   run(char *,char *,int);

void   scroll(int);
void   show_envir(void);
void   show_fkeys(void);
void   show_table(int);
void   show_time(void);
void   swapout(int);
void   sync(int);

void   undo(void);
void   unmess(void);
void   xit(void);
void   xgettime(void);
void   zf_time(int ,char *);

#else

unsigned int writepage();

char  *getline();
char  *strcpy();

char   dirmsk();
char   getkey();
char   getlow();
char   getscankey();
char   inkey();
char   testkey();
char   testlow();

void   addhistory();
void   adjustc();
void   crdelete();
void   dofindrep();
void   edit();
void   error();

void   fatalerror();
void   findorrep();
void   initialise();
void   listfile();

void   putlineno();
void   putonoff();
void   putpart();
void   putstr();
void   readtext();
void   readpage();

void   scroll();
void   show_envir();
void   show_fkeys();
void   swapout();
void   sync();

void   xit();
void   zf_time();

#endif


/* end of standard header */
