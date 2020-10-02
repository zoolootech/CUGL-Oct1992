/* hello.c - a simple curses program */

#include <curses.h>

main()
{
 WINDOW *win;

	/* initialize screen, and check for failure */
	if( ERR == initscr() ) {
		printf("initscr failed!\n");
		exit(1);
	}
	/* set up 'win' to be the standard screen.
	 *  (Despite the macros for handling stdscr, it is 
	 *  easier in the long term not to use them.)
	 */
	win = stdscr;

	/* set up tty modes */
	raw();			/* ignore ^C, etc. */
	noecho();		/* do not echo input */
	keypad(win, TRUE);	/* should generally use this on the PC */
	leaveok(win, TRUE);	/* turn off cursor for this window */
	
	/* position cursor and write message to window */
	wmove(win, LINES/2, 22);
	waddstr(win, "Hello world!");
	
	/* turn on inverse video mode */
	wattrset(win, A_REVERSE);

	/* can write and move at the same time */
	mvwaddstr(win, LINES/2+2, 18, "Hit any key to continue");

	/* turn off inverse video mode */
	wattrset(win, 0);
	
	/* display the window */
	wrefresh(win);
	
	/* wait for the user to press a key, discarding the input */
	(void)wgetch(win);
	
	/* standard termination */
	endwin();
	
	exit(0);
}

	