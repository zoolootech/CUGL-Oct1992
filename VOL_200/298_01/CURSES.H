/* PC Curses. (C) Copyright 1987 Jeffrey S. Dean.  All Rights Reserved. */

/* curses.h -- include file for PC Curses */

#ifndef PC_CURSES

/* PC_CURSES is used to identify this particular package;
 *  also ensures that this file is processed only once.
 */
#define PC_CURSES

#include <stdio.h>

/* version information */
extern char *_pc_curses;

/* characters in PC Curses are always of type CHTYPE
 *  in the current implementation, the upper byte is
 *  used to store attribute information.
 */
#define CHTYPE short

/* the basic window structure */
typedef struct  _win {
	CHTYPE **txt;			/* text, as an array of line ptrs */
	short cury, curx;		/* current x and y coordinates */
	short ox, oy;			/* x and y origin */
	short lines, cols;		/* size of full screen */
	short mfy, mfx, mly, mlx;	/* area to be refreshed */
	CHTYPE attrs;			/* screen attributes */
	short flags;			/* miscellaneous information */
	short scrtop, scrbot;		/* scrolling region boundaries */
	CHTYPE *base;			/* base address of character map */
} WINDOW;

#define A_ATTRIBUTES	0xff00
#define A_CHARTEXT	0x00ff

/* Support is provided for both monochrome and color displays,
 *  by providing a set of attributes that can be used with
 *  the "attr" functions.
 *
 *  The meaning of the attributes depends on the display used.
 *  Note that the meanings may not translate well: for example,
 *  "underline" on a color display appears as blue text on a 
 *  black background.
 */ 

/* Although an attrbiute of zero means "invisible" on a PC, this package
 * has been hacked (for compatibility) so that zero also means "normal".
 */
#define	A_NONE		0
#define A_NORMAL	0x0700

/* On a PC, if !(attributes & A_VISIBLE), character will be invisible */
#define A_VISIBLE	0x7700

/* screen attributes for (monochrome) IBM PC
 *   Note that not all combinations are meaningful;
 *   see a PC reference manual for more info.
 */
#define A_REVERSE	0x7000 
#define A_UNDERLINE	0x0100 
#define A_BOLD		0x0800 
#define A_BLINK		0x8000 
#define A_STANDOUT	A_REVERSE 
#define A_DIM		A_NONE

/* color attributes
 *  use the A_COLOR macro to specify foreground and background colors.
 *  For example, to set the stdscr to use green characters on a yellow
 *  background, you would say:
 *	attrset( A_COLOR(A_GREEN, A_YELLOW) );
 */
#define A_COLOR(fore,back)	( (fore) << 8 | (back) <<12 )

/* primary colors */
#define A_BLACK		0
#define A_BLUE		1
#define A_GREEN		2
#define A_RED		4

/* secondary colors (combinations of primary colors) */
#define A_CYAN		3	/* blue-green */
#define A_MAGENTA	5	/* purple */
#define A_BROWN		6	/* (or dark yellow) */
#define A_WHITE		7	/* white (or light gray) */

/* the "intensity" bit
 *  Warning: If used on the background color, the intensity bit
 *  will cause the text to blink.  This reduces the number of
 *  background colors from 16 to 8.  (There is a hardware-dependent
 *  method around this problem, but PC Curses does not support it.)
 */
#define A_INTENSE	8

/* "light" colors */
#define A_GRAY		(A_INTENSE|A_BLACK)
#define A_LBLUE		(A_INTENSE|A_BLUE)
#define A_LGREEN	(A_INTENSE|A_GREEN)
#define A_LCYAN		(A_INTENSE|A_CYAN)
#define A_LRED		(A_INTENSE|A_RED)
#define A_LMAGENTA	(A_INTENSE|A_MAGENTA)
#define A_YELLOW	(A_INTENSE|A_BROWN)
#define A_BRIGHTWHITE	(A_INTENSE|A_WHITE)

/* Unix Sys V.3 extended character set
 *
 * Note: this is an approximation of the V.3 approach,
 * which requires that all line graphics characters have the
 * bit A_ALTCHARSET turned on (if it is off, it means that
 * the terminal does not support that particular character).
 * Since PC Curses provides color support (see above), there are
 * no extra bits available for A_ALTCHARSET.  As an approximation,
 * A_ALTCHARSET is defined as 0x80.  Since many of the PC graphics
 * characters have this bit set, this approximation works most of
 * the time.  However, some graphics characters fall in the range
 * 0x00 to 0x1F, which means that testing for A_ALTCHARSET will
 * fail.
 */
#define A_ALTCHARSET	0x0080
#define ACS_ULCORNER	218
#define ACS_LLCORNER	192
#define ACS_URCORNER	191
#define ACS_LRCORNER	217
#define ACS_RTEE	180
#define ACS_LTEE	195
#define ACS_BTEE	193
#define ACS_TTEE	194
#define ACS_HLINE	196
#define ACS_VLINE	179
#define ACS_PLUS	'+'
#define ACS_DIAMOND	4
#define ACS_CKBOARD	176
#define ACS_DEGREE	248
#define ACS_PLMINUS	241
#define ACS_BULLET	249
#define ACS_LARROW	27
#define ACS_RARROW	26
#define ACS_DARROW	25
#define ACS_UARROW	24
#define ACS_BOARD	'#'
#define ACS_LANTERN	'#'

/* "double line" characters (not defined by SysV.3) */
#define ACS_DULCORNER	201
#define ACS_DLLCORNER	100
#define ACS_DURCORNER	187
#define ACS_DLRCORNER	188
#define ACS_DRTEE	185
#define ACS_DLTEE	204
#define ACS_DBTEE	202
#define ACS_DTTEE	203
#define ACS_DHLINE	205
#define ACS_DVLINE	186

/* Curses provides getyx() to return current coordinates; newer versions
 * of curses also provide getbegyx() and getmaxyx() to return beginning
 * and ending coordinates.  The coordinate accessing macros below are
 * non-standard, but they allow just a single coordinate to be accessed.
 * This saves the trouble of declaring an unneeded variable, eliminates
 * an unneeded assignment, and prevents the compiler (or lint) from
 * complaining about a variable that is set but never used.
 */
#define BEGY(w)		w->oy		/* starting position of window */
#define BEGX(w)		w->ox
#define CURY(w)		w->cury		/* current position in window */
#define CURX(w)		w->curx
#define MAXY(w) 	w->lines	/* number of lines */
#define MAXX(w)		w->cols		/* number of columns */
#define WGETC(w,y,x)	w->txt[y][x]	/* get character from window */
#define WPUTC(w,y,x,c)	w->txt[y][x]=c	/* place character in window */

/* for win->flags */
/* #define W_WRAPOK	0x1		/* ok for lines to wrap around */
#define W_CLEAROK	0x2		/* ok to clear window */
#define W_MODIFIED	0x4		/* window has been modified */
#define W_SCROLLOK	0x8		/* ok to scroll window */
#define W_KEYPAD	0x10		/* use keypad translations */
#define W_NODELAY	0x20		/* non-blocking tty input */
#define W_TOUCHED	0x40		/* window has been touched */
#define W_SUBWIN	0x80		/* window is a sub-window */
#define W_LEAVEOK	0x100		/* ignore cursor */
#define W_META		0x200		/* enable meta mode */
#define W_PAD		0x400		/* window is really a pad */
#define W_PADNOCUR	0x800		/* internal pad cursor control */
#define W_FULLWIN	0x1000		/* window is full screen */

/* macros to set/clear win->flags */
#define scrollok(win,flag)	_winflag(win,flag,W_SCROLLOK)
#define nodelay(win,flag)	_winflag(win,flag,W_NODELAY)
#define leaveok(win,flag)	_winflag(win,flag,W_LEAVEOK)
#ifndef SIMPGETCH
#define keypad(win,flag)	_winflag(win,flag,W_KEYPAD)
#define meta(win,flag)		_winflag(win,flag,W_META)
#endif
#define clearok(win,flag) \
	( (win)->flags & W_FULLWIN ? _winflag(win, flag, W_CLEAROK) : ERR )

/* standard stuff for curses */
#define TRUE	1
#define FALSE	0
#define	ERR	0
#define	OK	1

/* standard curses variables */
extern int LINES, COLS;			/* size of full screen */
extern WINDOW *stdscr, *curscr;		/* current and standard screens */

/* for tty input emulation mode */
extern int _ttyflags;

#define T_RAW	01
#define T_CRMOD	02
#define T_ECHO	04
#define T_NONL	010

/* settings for screen update method; this is still experimental */
#define T_BIOS		0		/* use bios updating */
#define T_DIRECT	-1		/* direct update, figure out type */
#define T_MA		0xb000		/* direct update, assume monochrome */
#define T_CGA		0xb800		/* direct update, assume cga */
#define T_EGA		0xa800		/* direct update, assume ega ?? */

/* PC line drawing characters (horizontal/vertical pairs) */
#define S_HOR	196	/* single line */
#define S_VERT	179
#define D_HOR	205	/* double line */
#define D_VERT	186
#define T_HOR	220	/* thick solid line, not quite perfect */
#define T_VERT	219

WINDOW	*initscr(), *newwin(), *subwin(), *newpad();
WINDOW	*shadowwin();
char	*unctrl();

#define	getyx(win,y,x)		(y = CURY(win), x = CURX(win))
#define getbegyx(win,y,x)	(y = BEGY(win), x = BEGX(win))
#define getmaxyx(win,y,x)	(y = MAXY(win), x = MAXX(win))

#define raw()			_ttyflags |= T_RAW
#define noraw()			_ttyflags &= ~T_RAW
#define crmode()		_ttyflags |= T_CRMOD
#define nocrmode()		_ttyflags &= ~T_CRMOD
/* cbreak and crmode refer to the same thing */
#define cbreak()		_ttyflags |= T_CRMOD
#define nocbreak()		_ttyflags &= ~T_CRMOD
#define echo()			_ttyflags |= T_ECHO
#define noecho()		_ttyflags &= ~T_ECHO
#define nonl()			_ttyflags |= T_NONL
#define nl()			_ttyflags &= ~T_NONL

#define savetty()
#define resetty()
#define gettmode()
#define setterm(x)

#define	getch()		wgetch(stdscr)
#define	addch(ch)	waddch(stdscr, ch)
#define	addstr(str)	waddstr(stdscr, str)
#define	move(y, x)	wmove(stdscr, y, x)
#define	clear()		wclear(stdscr)
#define	erase()		werase(stdscr)
#define	clrtobot()	wclrtobot(stdscr)
#define	clrtoeol()	wclrtoeol(stdscr)
#define getstr(buf)	wgetstr(stdscr,buf)
#define	insertln()	winsertln(stdscr)
#define	deleteln()	wdeleteln(stdscr)
#define	refresh()	wrefresh(stdscr)
#define	inch()		winch(stdscr)
#define	insch(c)	winsch(stdscr,c)
#define	delch()		wdelch(stdscr)
#define	standout()	wstandout(stdscr)
#define	standend()	wstandend(stdscr)
#define	attron(at)	wattron(stdscr,at)
#define	attroff(at)	wattroff(stdscr,at)
#define	attrset(at)	wattrset(stdscr,at)
#define setscrreg(t,b)	wsetscrreg(stdscr,t,b)

#ifdef SIMPMOVE
/* SIMPMOVE provides a simplified version of wmove()
 *  by circumventing error checking.  Its use is not
 *  recommended unless legal values can be guaranteed.
 *  It is normally undefined.
 */
#define wmove(win,y,x)		((win)->cury=(y),(win)->curx=(x),OK)
#endif

#define	mvwaddch(win,y,x,ch)	(wmove(win,y,x)==ERR ? ERR: waddch(win,ch))
#define	mvwgetch(win,y,x)	(wmove(win,y,x)==ERR ? ERR: wgetch(win))
#define	mvwaddstr(win,y,x,str)	(wmove(win,y,x)==ERR ? ERR: waddstr(win,str))
#define	mvwgetstr(win,y,x,str)	(wmove(win,y,x)==ERR ? ERR: wgetstr(win,str))
#define	mvwinch(win,y,x)	(wmove(win,y,x)==ERR ? ERR: winch(win))
#define	mvwdelch(win,y,x)	(wmove(win,y,x)==ERR ? ERR:(wdelch(win),OK))
#define	mvwinsch(win,y,x,c)	(wmove(win,y,x)==ERR ? ERR:(winsch(win,c),OK))
#define	mvaddch(y,x,ch)		mvwaddch(stdscr,y,x,ch)
#define	mvgetch(y,x)		mvwgetch(stdscr,y,x)
#define	mvaddstr(y,x,str)	mvwaddstr(stdscr,y,x,str)
#define	mvgetstr(y,x,str)	mvwgetstr(stdscr,y,x,str)
#define	mvinch(y,x)		mvwinch(stdscr,y,x)
#define	mvdelch(y,x)		mvwdelch(stdscr,y,x)
#define	mvinsch(y,x,c)		mvwinsch(stdscr,y,x,c)

/* we made these into macros */
#define wstandout(win)		(win)->attrs |= A_STANDOUT
#define wstandend(win)		(win)->attrs = A_NONE
#define wattroff(win,at)	(win)->attrs &= ~(at)
#define wattron(win,at)		(win)->attrs |= (at)
#define wattrset(win,at)	(win)->attrs = (at)
/* for some reason, there is no standard way to get attributes;
 *  hence, wattrget...
 */
#define wattrget(win)		(win)->attrs
#define attrget()		wattrget(stdscr)

#define winch(win)		((win)->txt[(win)->cury][(win)->curx])

/* overlay() and overwrite() are cases of copywin() */
#define overlay(src, dst) \
    copywin(src, dst, 0, 0, 0, 0, \
	min(MAXY(src), MAXY(dst)) - 1, min(MAXX(src), MAXX(dst)) - 1, 1)
#define overwrite(src, dst) \
    copywin(src, dst, 0, 0, 0, 0, \
	min(MAXY(src), MAXY(dst)) - 1, min(MAXX(src), MAXX(dst)) - 1, 0)

/* PC Curses only
 *	The terminfo and termcap curses definitions of winch() are different.
 *	PC Curses uses the terminfo definition.  For the convenience of
 *	termcap curses uses, the macro wcinch() and its variants provide
 *	the old function.
 */
#define wcinch(win)		(A_CHARTEXT & winch(win)) 
#define cinch()			wcinch(stdscr)
#define	mvwcinch(win,y,x)	(wmove(win,y,x)==ERR?ERR:wcinch(win))
#define	mvcinch(y,x)		mvwcinch(stdscr,y,x)

/* for compatibility -- no need for cursor optimization */
#define mvcur(fy, fx, ty, tx)	_mvcur(ty, tx)

#define flushinp() 		bdos(0xC,0xFF,6)

/* "funny" function definitions */
#define idlok(win,flag)
#define longname(tbuf,buf)	strcpy(buf, "ibm-pc")
#define draino(X)		ERR
#define intrflush(win,flg)
#define typeahead(fd)
#define baudrate()		9600	/* or any value you want! */
/* erasechar and killchar are used by getstr(); you can set
 * your own values if you recompile getstr() or do not use it.
 */
#define erasechar()	'\b'		/* backspace */
#define killchar()	'\025'		/* Control-U */

/* extended key definitions */
#define KEY_BREAK	0401	/* unused */
#define KEY_DOWN	0402
#define KEY_UP		0403
#define KEY_LEFT	0404
#define KEY_RIGHT	0405
#define KEY_HOME	0406
#define KEY_F0		0410
#define KEY_F(x)	(KEY_F0+(x))
#define KEY_DL		0510	/* unused */
#define KEY_IL		0511	/* unused */
#define KEY_DC		0512
#define KEY_IC		0513
#define KEY_EIC		0514	/* unused */
#define KEY_CLEAR	0515	/* unused */
#define KEY_EOS		0516	/* unused */
#define KEY_EOL		0517	/* unused */
#define KEY_SF		0520	/* unused */
#define KEY_SR		0521	/* unused */
#define KEY_NPAGE	0522
#define KEY_PPAGE	0523
#define KEY_STAB	0524	/* unused */
#define KEY_CTAB	0525	/* unused */
#define KEY_CATAB	0526	/* unused */
#define KEY_ENTER	0527	/* unused */
#define KEY_SRESET	0530	/* unused */
#define KEY_RESET	0531	/* unused */
#define KEY_PRINT	0532	/* unused */
#define KEY_LL		0533	/* unused */
#define KEY_BTAB	0541	/* back (shifted) tab */
#define KEY_END		0550

/* PC Curses only
 *	new code values start from 0600 (arbitrarily chosen)
 */
#define KEY_cNPAGE	0600	/* Ctrl-PgDn */
#define KEY_cPPAGE	0601	/* Ctrl-PgUp */
#define KEY_cHOME	0602	/* Ctrl-Home */
#define KEY_cEND	0603	/* Ctrl-End */
#define KEY_cLEFT	0604	/* Ctrl-LeftArrow */
#define KEY_cRIGHT	0605	/* Ctrl-RightArrow */
#define KEY_cPRTSC	0606	/* Ctrl-PrtSc */

/* PC Curses only: function keys with shift, control, and alt */
#define KEY_sF(x)	(KEY_F(10)+x)
#define KEY_cF(x)	(KEY_sF(10)+x)
#define KEY_aF(x)	(KEY_cF(10)+x)

/** function prototypes **/

int	beep(void);
int	box(WINDOW *,CHTYPE,CHTYPE);
int	copywin(WINDOW *,WINDOW *,int,int,int,int,int,int,int);
int	delwin(WINDOW *);
int	doupdate(void);
int	drawbox(WINDOW *,CHTYPE,CHTYPE,int,int,int,int);
int	endwin(void);
int	flash(void);
WINDOW *initscr(void);
int	mvprintw(int,int,char *,...);
int	mvwin(WINDOW *,int,int);
int	mvwprintw(WINDOW *,int,int,char *,...);
int	mvscanw(int,int,char *,...);
int	mvwscanw(WINDOW *,int,int,char *,...);
WINDOW *newpad(int,int);
WINDOW *newwin(int,int,int,int);
int	pnoutrefresh(WINDOW *,int,int,int,int,int,int);
int	prefresh(WINDOW *,int,int,int,int,int,int);
int	printw(char *,...);
int	scanw(char *,...);
int	scroll(WINDOW *);
WINDOW *shadowwin(WINDOW *,WINDOW *);
WINDOW *subwin(WINDOW *,int,int,int,int);
int	touchwin(WINDOW *);
int	traceclose(void);
int	tracedump(WINDOW *,int);
int	traceoff(void);
int	traceon(void);
int	traceprint(char *,...);
char   *unctrl(CHTYPE);
int	waddch(WINDOW *,CHTYPE);
int	waddstr(WINDOW *,char *);
int	wclear(WINDOW *);
int	wclrtobot(WINDOW *);
int	wclrtoeol(WINDOW *);
int	wdelch(WINDOW *);
int	wdeleteln(WINDOW *);
int	werase(WINDOW *);
int	wgetch(WINDOW *);
int	wgetstr(WINDOW *,char *);
int	winsch(WINDOW *,CHTYPE);
int	winsertln(WINDOW *);
int	wmove(WINDOW *,int,int);
int	wnoutrefresh(WINDOW *);
int	wprintw(WINDOW *,char *,...);
int	wrefresh(WINDOW *);
int	wrelmove(WINDOW *,int,int);
int	wscanw(WINDOW *,char *,...);
int	wsetscrreg(WINDOW *,int,int);
 
#endif /* PC_CURSES */
