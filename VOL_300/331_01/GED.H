/*
HEADER:         CUG000.01;
TITLE:          header file for se.c;
DATE:           5/19/87;
FILENAME:       GED.H;
AUTHORS:        G. Nigel Gilbert, James W. Haefner, Mel Tearle G. Osborn;
*/

/*
     e/qed/ged screen editor

    (C) G. Nigel Gilbert, MICROLOGY, 1981
           licensed for private non-profitmaking use 1983
           August-December 1981

    Modified: Aug-Dec   1984:  BDS-C 'e'(vers 4.6a) to 'qe' (J.W. Haefner)
              March     1985:  BDS-C 'qe' to DeSmet-C 'qed' (J.W.Haefner)
              May       1986:  converted to ged - Mel Tearle
              June      1990:  G. Osborn
    FILE:     ged.h

    PURPOSE:  standard header file, #included in all function files
*/

/*
 * All static variables and #defines were at one time in this file.
 * Variables which are not truely global in function are being removed
 * from here and placed in the individual files.  Parameters which, though
 * sometimes local, are useful for configuring the program without studying
 * the details remain here.     g.o.
 */


#define max(a,b) (a >= b) ? a : b
#define min(a,b) (a <= b) ? a : b
#define abs(x)   (x >= 0) ? x : -x

#define VERSION   "4.6b"

/* Microsoft 4.00 mixed memory model */
#define FAR far
#define HUGE huge
/* use the following two lines instead of the preceeding two if the compiler
 * does not support mixed memory models.  The empty definitions
 * remove all occurences of the far and huge keyword, causing
 * the small memory model to be used.
 */
/*
 *#define FAR
 *#define HUGE
 */



#define SECSIZ      512      /* file buffer definitions */
#define NSECTS        4
#define FILELEN      64      /* max length of file name */

#define JBUFSIZE   6     /* size of buffer required for longjumps, not used */
#define BUFFER    12     /* size of typeahead buffer */
#define FLIM      80     /*  */
#define LLIM     255     /* max length of a line. count includes null marker */

#define FAIL      -1
/* disc fail.  This value is internal and compiler independent */
#define DFAIL     -2
#define ENDCHAR    0x1a    /* end of text file mark. automatic option on input */
#define ENDFILE    -1      /* end of input file */
#define YES        1
#define NO         0
#define FALSE      0
#define TRUE       1
#define PREV      -2

#define PAGEOVERLAP   0    /* overlap between pages with UP,DOWN-PAGE */
#define OFFWIDTH      1    /* horizontal scroll increment */

#define SHEIGHT      24    /* screen height */
#define SWIDTH       79    /* screen width */

#define NLINES        6    /* number of lines to skip at bottom of printer page */
#define PAGELEN      66    /* length of printer page, used in listfile() & roff() */

#define EMPOS         0    /* position of error message */
#define WAITPOS       0    /* position of cursor when searching/reading */
#define LNPOS         7    /* position of line number */
#define COLPOS       18    /* position of column number */
#define REPPOS       19
#define FNPOS        43    /* status line - position of file name */
#define TIMEPOS      72    /* position of current time */

#define BACKSP     0x08    /* backspace */
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
#define WINDUP    20     /* window up one line */
#define TOPSCRKEY 1000   /*not fully implemented  */
#define BOTSCRKEY 1001   /*  */
#define POP       21
#define CPOP      22
#define BLOCKKEY  23     /* block operations (K) */
#define WINDDOWN  24     /* window down one line */
#define REPKEY    25     /* repeat last find/alter (L) */
#define INSKEY    26     /* char insert/replace */
#define OOPSKEY   27     /* restore unedited line (_) */
#define TAB       28     /* tab (I) */
#define RETRIEVE  29     /* retrieve last name etc entered here */
#define CR        30     /* return (M) */
#define ESCKEY    31     /* the magic escape key (ESC) */
#define HELPKEY   1002   /* not implemented.  The F1 help key is implemeted. */

/* key codes + 0x80 (PARBIT) - used in scanning strings of ctrl chars */
#define LEFTKEY_P   131
#define DELLEFT_P   141
#define RETRIEVE_P  157
#define CR_P        158
#define ESCKEY_P    159

/* function keys */
#define F1KEY     210
#define F2KEY     211  /* f2 = search */
#define F3KEY     212  /* f3=backward */
#define F4KEY     213  /* f4=forward  */
#define F5KEY     214
#define F6KEY     215
#define F7KEY     216
#define F8KEY     217
#define F9KEY     218   /* f9 exit */
#define F10KEY    219   /* f10 save */

/*
 * The video parameters in this file were originally set for a Hercules
 * video card, then changed for a VGA card with a NEC grey scale monitor.
 * The UNDERLINE attribute was used for the status line on the Hercules
 * card, but is not supported by the VGA card.  The values selected in
 * this version will work on all systems, but are not the best choices
 * for any one system.  The first four bits and the second four bits of
 * the attribute byte are independently selected.  Experiment with them.
 */

/*
 * normal screen color/intensity.  selected by a call to colornorm()
 */
#define ATTR0  0x07    /* PC default.  White on black.*/

/* a screen attribute used like highlighting or italics.
 * The significance to the program is that it is abnormal, not whether it
 * is bright or dim.  On the PC it is brighter than normal.
 * Selected by a call to color1();
 */
#define ATTR1 0x0F    /* bright white on black */

/* Reverse field.  Used for marking blocks and for error messages.  Affects
 * whitespace so that the spaces within block boundaries can be seen.
 * Selected by a call to colorblock().  Also selected by colorerr(); in
 * this version.  (0x70 and 0x78 are the only codes which will reverse
 * field a PC monochrome display, excluding colored error messages if
 * compatibility is a consideration).
 */
#define ATTR2 0x70   /* reverse field, normal intensity */

/* Was used only to underline the header to seperate it from the
 * text.  Could also be used for conventional underlining of text.
 */
/* #define ATTR3 0x01 */    /* normal intensity, underlined (Hercules & other early cards) */
/* vga, temporary +++ */
#define ATTR3 0x70

/*#define ATTR4 0x09 */    /* bright and underlined (Hercules) */
/* vga grey scale for header, temporary +++ */
#define ATTR4 0x60

/* Virtual memory constants.  The following are sized with a fixed
 * disc system and 640kb of RAM in mind.  There is a small memory savings in
 * reducing the magnitude of the constants for smaller systems,
 * but, other than for optimizaton, it is not necessary to adjust the
 * values to match the resources.  The maximum document size in bytes
 * is PAGESIZE*MAXSLOTS, with a 16384 line limit.
 */

/* PAGESIZE should be a multiple of the disc sector size for efficiency.
 * It is helpful also if it holds more than a screenful of text.
 * The size must be evenly divisible into 64 k in this version.
 */
#define PAGESIZE    0x800

/* Max allowable number of pagess in RAM.  The actual amount
 * of available RAM is determined by halloc(), and need not be
 * specified here.  If the document is too big to be held in RAM then
 * a temporary disc file is used.  Unmodified RAM pages usually have
 * a corresponding disc image, requiring a disc area large enough for
 * the entire document.
 */
#define RAMSLOTS   250

#define MAXSLOTS   1500



/* --- */

#define MAXINT     0x7FFF   /* largest possible positive integer */
#define NOFILE   (MAXINT)


/* the structure of the file buffers */
struct iobuffer  {
    int   fdes;
    int   nleft;
    unsigned char  *nextp;
    unsigned char  buff[NSECTS * SECSIZ];
} fbuf1, tbuf1;

struct iobuffer *fbuf;
struct iobuffer *textbuf;

/* The pointer to the tp array must be type huge (not far) in the large
 * memory model allocation or the pointer arithmetic is wrong.  This
 * requirement is not the same as the problem of strings crossing 64 k
 * boundaries.  In being unspecified, the Microsoft 4.0 compiler assumes
 * that the array size exceeds 128 k, resulting in the restriction that the
 * element size be a power of 2.  There are 12 unused bits in each
 * element if expansion is needed.
 */
struct addr  {
    int page;     /* virtual memory page number containing line of text */
    int moffset;  /* line offset into page containing text */
} HUGE *tp;


/* command history stack for undo()
 */
struct histstep  {
    struct addr histp;    /* address of text modified by command */
    int histline;         /* line number of modified text */
    int histtype;         /* type of command (insert,delete,replace) */
    int histcomm;         /* number of command modulo 256 */
}
history[HISTLEN];

int histptr;            /* index into history stack */
int histcnt;            /* number of commands on stack */
int storehist;          /* commands only stored on history when true */
unsigned ncommand;      /* count of commands executed since startup */
int stkptr;             /* text stack pointer */
int stkcnt;             /* no. items in stack.  (preliminary.  see pop() ) */

int fullmem;  /* max no. of kb of RAM which will be allocated. -Mnnn option */
int allocp;             /* location of next free in newpage */
int attr;               /* attribute for norm() and high() */
int clock;              /* counter for least recently used paging */
int charep;             /* zero if char replace mode, 1 if overlay mode */
int eofchar;               /* write ENDCHAR on output if !eofchar */
int charmask;              /* mask for zeroing bit zero of file input */
int newpage;                 /* page no of page being filled with text */
int pagefd;                  /* file descriptor of paging file */

int slotsinmem;              /* actual no. of slots in memory */
int tpslots;                 /* no. of RAM pages in use for line vectors */
int virtslot[MAXSLOTS];      /* current location of each page, RAM or disc index */
int usage[RAMSLOTS];         /* records usage of ram slots */
int auxloc[RAMSLOTS];        /* disc slot no. corresponding to mem slot */
char dskslots[MAXSLOTS];     /* disk slot use/free flag */

char pagingfile[FILELEN];     /* name of paging file */

/* the slotaddr array must be type huge (not far) in the large
 * memory model allocation or the pointer arithmetic is wrong with FAR
 * arithmetic.  This requirement is unrelated to the problem of strings
 * crossing 64 k boundaries.
 */
char HUGE *slotaddr[RAMSLOTS];  /* memory addr of start of each in-memory slot */

char altered;              /* if YES, current line has been altered by edits */
char blankedmess;          /* message area has been blanked, not overwritten */
int  displine;             /* last displayed line number */
int  dispcol;              /* last displayed column number */
char dskerr[JBUFSIZE];     /* for bdos disk error traps, unused */

char goteof;               /* true when eof found */
unsigned char inbuf[BUFFER]; /* input buffer for typeahead */
char isdim;                /* last char was in dim */

char mainmenu[JBUFSIZE];   /* for emergency longjump, unused */
char text[LLIM];           /* buffer for current line of text */
unsigned char tran[NKEYS]; /* translation table from external to internal
                              key codes */

int  charn;                /* position of current char in current line */
int  cline;                /* current line */
int  curdsk;               /* currently logged in (default) disk */
int  cursorx, cursory;     /* current position of cursor */

int  goodline;          /* line number we know we can display */
int  inbufp;            /* pointer to next char in inbuf */

int  lastl;             /* number of last line of text */
int  lastoff;           /* offset the last time resetcursor() was called */
int  lastc;             /* last line number changed, for jump cmd */
int  linem1;            /* marked line numbers for line jump */
int  linem2;
int  linem3;

int  offset;            /* first col. to display on screen */
int  pfirst, plast;     /* top and bottom line displayed on screen */
int  topline;           /* first line on screen for text */

/* --------------->>  environment options  <<--------------- */

char autoin;            /* do auto indentation */
char backup;            /* at end, old version becomes file.bak */
char blockscroll;       /* horizontal scroll whole page */

char displaypos;        /* display line:col at top of screen */
int tabwidth;           /* width between tabstops */
char trail;             /* don't chop trailing blanks from text lines */
char ctrl;              /* Control characters allowed if true */
char ans[6];            /* line to jump to */
char blocking;          /* true if block is or has been defined */
char defext[5];         /* default file extension, saved from last file. */
                        /*     ".xxx\0" */
                        /* hardcoded defaults .c and .doc in this version */

char filename[FILELEN]; /* current file being edited */
char justfile[15];      /* filename w/o path */

char name[FILELEN];     /* filename for read and write files */
char nocheck;           /* don't ask for confirmation before replacing */
char opts[5];           /* options for alter */
char patt[FLIM];        /* test string */
char repeat;            /* repeat last find/replace */
char replace;           /* last find/replace was a replace */

int  findir;            /* direction of last find */
int  vbord1,vbord2;     /* block start and end */
int  jmpto, initjmp;    /* initial line to jump */


char  *getline(), *strcpy();
unsigned char   getlow();
unsigned char testlow();

/* registers for syscall() in term.c */
unsigned int  rax, rbx, rcx, rdx, rsi, rdi, res, rds;
unsigned char  carryf, zerof;


/* end of standard header */
