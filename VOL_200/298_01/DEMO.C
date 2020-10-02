/* PC Curses. (C) Copyright 1987 Jeffrey S. Dean.  All Rights Reserved. */

/* demo - simple program to demonstrate curses
 *
 *	The purpose of this program is to demonstrate PC Curses.
 *	It provides a working example of what PC Curses looks like
 *	when running; it also provides examples of how to write code
 *	using the PC Curses package (for those who might not be
 *	familiar with it).
 *
 *	brief outline:
 *	reads in a data file, containing short paragraphs separated
 *	by formfeeds; there can be no more than COUNT paragraphs.
 *	each paragraph is put in a window, and then the user hits
 *	<return> to sequence through the windows.
 *
 *	functions:
 *	main
 *	fillwin 	- fills window with data from a file
 *	changemode 	- toggles between bios and direct screen updates
 *	changerefresh 	- toggles between wrefresh() and wnoutrefresh()
 *	help 		- displays a help screen
 *	message		- places message in prompt window
 */

/* the standard include file for curses */
#include <curses.h>
/* if working on unix, use this special include file
 * for extended or non-standard PC Curses things
 */
#ifdef unix
#include "unix.h"
#endif

/* the data file */
#define DEMODATA "demo.dat"
/* the help file */
#define HELPFILE "demo.hlp"

/* maximum of windows */
#define COUNT 10
/* size of each window */
#define NUMCOLS 60
#define NUMROWS 11

/* an array of window pointers */
WINDOW *winlist[COUNT];
/* the last window in winlist */
int last;

/* coordinates of first window */
#define YFIRST 2
#define XFIRST 2

/* flag to use wnoutrefresh instead of wrefresh */
int norefresh = 0;

/* window for the prompt message */
WINDOW *promptwin;
/* the prompt message */
char prompt[] = 
          "  <RETURN> for next, <BACKSPACE> for previous, F1 for help  \n";

main()
{
 int i, j, cmd;
 FILE *demofile;
 
	/** standard curses initialization **/
	if( ERR == initscr() ) {
		fprintf(stderr, "cannot initialize screen!\n");
		exit(1);
	}
	/* set up function key translation */
	keypad(stdscr, TRUE);
	/* raw mode means we get to handle ^C etc. */
	raw();
	/* do not automatically echo user's input */
	noecho();
	/* nonl() is mainly for Unix compatibility */
	nonl();

	/* set up the prompt window */
	if( NULL == (promptwin = newwin(1, sizeof(prompt), LINES-1, 10)) ) {
		fprintf(stderr, "cannot open prompt window\n");
		endwin();
		exit(1);
	}
	else wstandout(promptwin);
	message(prompt);
	/* setting leaveok turns off the cursor in the window */
	leaveok(promptwin, TRUE);
	leaveok(curscr, TRUE);
	
	/* set up the demo data file */
	demofile = fopen(DEMODATA, "r");
	if( demofile == NULL ) {
		 perror(DEMODATA);
		 exit(1);
	}
	/* we now create and fill up windows from the demo data file */
	for( i = 0; i < COUNT; i++ ) {
		/* allocate a new window */
		winlist[i] = newwin(NUMROWS, NUMCOLS, 
					YFIRST + i, XFIRST + 2*i);
		if( winlist[i] == NULL ) {
			fprintf(stderr, "cannot open window #%d\n", i);
			endwin();
			exit(1);
		}
		/* draw a double box around it */
		box(winlist[i], D_VERT, D_HOR);
		/* and break if no more data */
		if( fillwin(demofile, winlist[i]) == 0 ) break;
	}
	fclose(demofile);
	
	last = i-1;
	/* up to this point, windows have been filled but
	 *  none have been displayed.  now display them,
	 *  layered on top of each other, starting with
	 *  the last and working up to the first.
	 *
	 * Note that refresh here is more complicated than it will
	 *  normally be in an application, since we handle two kinds
	 *  of refreshing (termcap-style and terminfo-style), based
	 *  on the norefresh flag.
	 */
	for( i = last; i >= 0; i-- ) {
		if( norefresh ) wnoutrefresh(winlist[i]);
		else wrefresh(winlist[i]);
	}
	if( norefresh ) doupdate();
	/* sequence through the windows */
	for( i = 0; i <= last; ) {
		/* draw highlighted box around the top window
		 *  note that we are boxing with both single and horizontal
		 *  lines; box() is smart enough to choose the right
		 *  corner characters.
		 */
		box(winlist[i], A_REVERSE|S_VERT, A_REVERSE|D_HOR);

		/* refresh current window (pop it to the top) */
		if( norefresh ) wnoutrefresh(winlist[i]);
		else wrefresh(winlist[i]);
		
		/* display whatever is on the prompt line */
		wrefresh(promptwin);
		/* and wait for user input */
		cmd = getch();
		/* restore normal (unhighlighted) box */
		box(winlist[i], S_VERT, D_HOR);
		if( norefresh ) wnoutrefresh(winlist[i]);
		else wrefresh(winlist[i]);

		/* process user's command
		 *  this is an example of a typical curses command loop
		 */
		switch(cmd) {
			case KEY_UP:
			case KEY_LEFT:
			case '\b':	/* backspace goes back one window */
				if( i > 0 ) i--;
				break;
			case KEY_DOWN:
			case KEY_RIGHT:
			case '\r':	/* return goes forward one window */
			case '\n':
				i++;
				break;
			case '\002':	/* control-b toggle bios updating */
				changemode();
				break;
			case '\003':	/* control-c aborts */
				endwin();
				exit(1);
			case '\007':	/* control-g flashes */
				beep();
				flash();
				break;
			case '\f':	/* formfeed redisplays the screen */
				/* fix the prompt line */
 				message(prompt);
				/* clear the screen */
				wclear(curscr);
				/* redisplay to get to initial state */
				for( j = last; j >= 0; j-- ) {
					touchwin(winlist[j]);
					if( norefresh )
						wnoutrefresh(winlist[j]);
					else 	wrefresh(winlist[j]);
				}
				/* move back down to current window */
				for( j = 0; j < i; j++ ) {
					touchwin(winlist[j]);
					if( norefresh )
						wnoutrefresh(winlist[j]);
					else wrefresh(winlist[j]);
				}
				break;
			case '\027':	/* control-w toggles refresh mode */
				changerefresh();
				break;
			case KEY_F(1):
				help(HELPFILE);
				break;
			default:
 				message(prompt);
				break;
			}
	}
	/** normal procedure for exiting curses **/
	/* clear the screen */
	clear();
	refresh();
	/* tell curses we are done */
	endwin();
	exit(0);
}

int scradr = T_BIOS;

/* changemode - change screen update mode */
changemode()
{
 char buf[80];
 
	if( scradr == T_BIOS ) {
		sprintf(buf, "Screen update method: DIRECT (at 0x%4.4X)",
				scradr = pc_uptype(T_DIRECT) );
	}
	else {
		scradr = pc_uptype(T_BIOS);
		strcpy(buf, " Screen update method: BIOS");
	}
	message(buf);
}

/* changerefresh - change refresh method
 *
 *	old style of refresh is to use wrefresh;
 *	new style of refresh is to use wnoutrefresh, then
 *	doupdate() after all change have been made, resulting
 *	in just one screen update.
 */
changerefresh()
{
	norefresh = !norefresh;
	if( norefresh )
		message("Using new style (wnoutrefresh/doupdate) updating");
	else
		message("Using old style (wrefresh) updating");
}

/* message - display message in the prompt window */
message(str)
 char *str;
{
	wmove(promptwin, 0, 0);
	wclrtoeol(promptwin);
	wmove(promptwin, 0, 1);
	waddstr(promptwin, str);
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
 static int stat = OK;
 FILE *f;
 
	if( stat == ERR ) return(ERR);
	if( helpwin == NULL ) {
		/* create help window */
		helpwin = newwin(0,0,0,0);
		shadow = newwin(0,0,0,0);
		if( helpwin == NULL || shadow == NULL ) return(stat=ERR);
		leaveok(helpwin, TRUE);
		keypad(helpwin, TRUE);
		/* open up help file */
		if( NULL == (f = fopen(file, "r")) ) return(stat=ERR);
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
