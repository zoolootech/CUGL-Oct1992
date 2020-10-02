/* PC Curses. (C) Copyright 1987 Jeffrey S. Dean.  All Rights Reserved. */

/* unix.h - definitions for running PC Curses on Unix
 *
 *	PC Curses was designed to be compatible with Unix curses.
 *	However, there are a few upward-compatible changes or
 *	extensions in PC Curses.  When porting a PC Curses program
 *	to Unix, copy this include file over, and include it in
 *	the program.
 */
#ifdef unix

#define CHTYPE short

/* approximation of line drawing characters */
#define S_HOR '-'
#define S_VERT '|'
#define D_HOR '='
#define D_VERT '"'

/* no screen attributes */
#define A_NONE		0

/* standard function definitions */
WINDOW *newwin();
WINDOW *shadowwin();

/* PC Curses only: terminfo cinch() == termcap inch() */
#define wcinch(win)		(A_ATTRIBUTES & winch(win)) 
#define cinch()			wcinch(stdscr)
#define	mvwcinch(win,y,x)	(wmove(win,y,x)==ERR?ERR:wcinch(win))
#define	mvcinch(y,x)		mvwcinch(stdscr,y,x)

/* The curses package is lacking in a method for accessing window
 *  information, aside from directly accessing the window struct
 *  (a bad idea, and a violation of the abstraction).  These macros
 *  are one answer to that problem.
 */
#define BEGY(w)		w->_begy	/* starting position of window */
#define BEGX(w)		w->_begx
#define CURY(w)		w->_cury	/* current position in window */
#define CURX(w)		w->_curx
#define MAXY(w) 	w->_maxy	/* number of lines */
#define MAXX(w)		w->_maxx	/* number of columns */
#define WGETC(w,y,x)	w->_y[y][x]	/* get character from window */
#define WPUTC(w,y,x,c)	w->_y[y][x]=c	/* place character in window */

#define KEY_END KEY_LL

/* unnecessary functions */
#define pc_uptype(scr)

/* unnecessary defines */
#define T_BIOS		0
#define T_DIRECT	0

#endif /* unix */
