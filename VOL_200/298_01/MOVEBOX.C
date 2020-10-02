/* movebox - use cursor keys to move box around screen */
#include <curses.h>

#define HELPMSG "<Enter> turns bounce on.  Hit F1 for more help."
#define HELPFILE "movebox.hlp"

#define FIRST_X 5
#define FIRST_Y 5
#define W_LINES 5
#define W_COLS 12

int scrtype = T_DIRECT;
/* the popup window (box) and its shadow */
WINDOW *small, *shadow;
/* current position of the popup */
int y, x;

main()
{
 int i;
 int ch;
 
	initscr();
	pc_uptype(scrtype);
	keypad(stdscr,TRUE);
	
	message(HELPMSG);
	/* fill up the main window */
	for( y = 1; y < LINES; y++ )
		for( x = 0; x < COLS; x++ )
			mvaddch(y, x, ' ' + x);

	wnoutrefresh(stdscr);
	
	/* create a small pop-up window to move around */
	y = FIRST_Y;
	x = FIRST_X;
	small = newwin(W_LINES, W_COLS, y, x);
	/* set the window to inverse video */
	wattrset(small, A_REVERSE);
	/* make the whole box inverse video */
	wclear(small);
	/* add text to window */
	mvwaddstr(small, 1, 1, "PC Curses");
	mvwaddstr(small, 2, 2,  "Movebox");
	mvwaddstr(small, 3, 3,   "Demo");
	/* turn off cursor */
	leaveok(small, TRUE);
	/* box it, with the box also in inverse video */
	box(small, A_REVERSE | D_VERT, A_REVERSE | D_HOR);
	shadow = shadowwin(small, NULL);

	/* main command loop -- move the small window around the large */
	for(;;) {
		/* popup the small window */
		shadowwin(small, shadow);
		touchwin(small);
		wrefresh(small);

		/* get command from user */
		ch = getch();
		/* clear away the small window */
		wnoutrefresh(shadow);
		switch(ch) {
			case KEY_RIGHT:
				if( OK == mvwin(small, y, x+1) ) x++;
				break;
			case KEY_LEFT:
				if( OK == mvwin(small, y, x-1) ) x--;
				break;
			case KEY_UP:
				if( OK == mvwin(small, y-1, x) ) y--;
				break;
			case KEY_DOWN:
				if( OK == mvwin(small, y+1, x) ) y++;
				break;
			case '\r':
			case '\n':
				bounce();
				message(HELPMSG);
				break;
			case KEY_F(1):
				if( ERR == help(HELPFILE) )
				    message("Error setting up help window");
				break;
			case KEY_F(2):
				if( scrtype == T_DIRECT ) {
					scrtype = T_BIOS;
					message("Now using BIOS updating");
				}
				else {
					scrtype = T_DIRECT;
					message("Now using DIRECT updating");
				}
				pc_uptype(scrtype);
				break;
			default:
				endwin();
				exit(0);
			}
	}
}

fatal(str)
 char *str;
{
	puts(str);
	endwin();
	exit(1);
}

/* bounce - make the small window bounce, vaguely like pong */
bounce()
{
 int yi = 1, xi = 1;
 int ch;
 static int randomize = 0;

	message("Hit any key to stop bouncing");
	for( ; !kbhit(); ) {
		/* try continue moving in same direction */
		if( OK == mvwin(small, y+yi, x+xi) ) {
			y += yi;
			x += xi;
		}
		/* time to change direction */
		else {
			if( ERR == mvwin(small, y+yi, x) ) {
				yi = -yi;
				if( randomize ) {
					if( rand() & 1 ) 
						if( yi % 2 ) yi = yi % 2;
						else yi = 1;
					else yi = yi * 2;
				}
			}
			else {
				xi = -xi;
				if( randomize ) {
					if( rand() & 1 ) 
						if( xi % 2 ) xi = xi % 2;
						else xi = 1;
					else xi = xi * 2;
				}
			}
		}
		/* make the small window popup again */
		shadowwin(small, shadow);
		touchwin(small);
		wrefresh(small);
		/* and remove it */
		wnoutrefresh(shadow);
	}
	randomize = !randomize;
	/* trash the input that caused us to stop */
	flushinp();
}

message(str)
 char *str;
{
	move(0,0);
	clrtoeol();
	standout();
	printw("  %s  ", str);
	standend();
	refresh();
}

/** the following two functions, help() and fillwin(), are general
 *  purpose routines.  help displays a help window; fillwin fills
 *  a window from a file, interpreting control characters as attributes.
 *
 *  Usage:
 *	help(filename)
 *	  char *filename;
 *	returns OK if successful, ERR otherwise.
 *	In this version, the filename is only used the first time
 *	through; subsequent calls just use the same data.
 *	An error return is possible only on the first call, and
 *	indicates a problem with the help file or a memory shortage.
 *
 *	The help window disappears when any key is pressed.
 *
 *	fillwin(f, win);
 *	 FILE *f; WINDOW *win;
 *	returns 0 on EOF, 1 otherwise
 *	Reads data until a formfeed, puts data into window.  Several
 *	control characters are allowed; their occurrence in the file
 *	causes fillwin to modify attributes.  The valid characters are:
 *		^B - bold
 *		^F - flash
 *		^N - normal
 *		^R - reverse
 *		^U - underline
 */

help(file)
 char *file;
{
 static WINDOW *helpwin = NULL, *shadow;
 FILE *f;
 
	if( helpwin == NULL ) {
		/* create help window */
		helpwin = newwin(0,0,0,0);
		shadow = newwin(0,0,0,0);
		if( helpwin == NULL || shadow == NULL ) return(ERR);
		leaveok(helpwin, TRUE);
		keypad(helpwin, TRUE);
		/* open up help file */
		if( NULL == (f = fopen(file, "r")) ) return(ERR);
		/* read help file into window */
		fillwin(f, helpwin);
		(void)fclose(f);
	}
	touchwin(helpwin);
	shadowwin(helpwin, shadow);
	wrefresh(helpwin);
	(void)wgetch(helpwin);
	wrefresh(shadow);
	return(OK);
}

/* fillwin - fill window with next paragraph from a file
 *
 *	returns 1 on success (or end of window), 0 if at end of file
 *
 *	if special control characters are found, they are
 *	interpreted as attribute control:
 *		^B bold
 *		^F flashing
 *		^N normal
 *		^R reverse
 *		^U underline
 */
fillwin(f, win)
 FILE *f; WINDOW *win;
{
 int ch;
 int y,x;
 
	 if( feof(f) ) return(0);
	 wmove(win, 0, 3);
	 while( EOF != (ch = getc(f)) ) {
		 /* break on formfeed */
		 if( ch == '\014' ) break;
		 switch(ch) {
			case '\n':
				getyx(win, y, x);
				if( ERR == wmove(win, y+1, 3) )
					return(1);
				break;
			case 2:		/* ^B */
				wattron(win, A_BOLD);
				break;
			case 6:		/* ^F */
				wattron(win, A_BLINK);
				break;
			case 14:	/* ^N */
				wattrset(win, 0);
				break;
			case 18:	/* ^R */
				wattron(win, A_REVERSE);
				break;
			case 21:	/* ^U */
				wattron(win, A_UNDERLINE);
				break;
			default:
				if( ERR == waddch(win, ch) )
					return(1);
				break;
		}
	}
	return(1);
}
