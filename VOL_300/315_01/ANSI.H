
/* ansi.h defines some of the ANSI.SYS driver commands.  ANSI.SYS must    */
/* be installed on your computer for these routines to work.  To use,     */
/* simply add #include "ansi.h" or #include <ansi.h> in your preprocessor */
/* directives.  The header file is self-documenting.  Written by T. Clune */

#ifndef ANSI_H
#define ANSI_H

	/* to move the cursor to x,y -- where 1<=x<=80 and 1<=y<=25 */
#define POSITION(x, y)  printf("\033[%d;%dH",y,x)

	/* to move the cursor UP y lines -- where 0<y<=24 */
#define UP_SHIFT(y)           printf("\033[%dA",y)

	/* to move the cursor DOWN y lines -- where 0<y<=24 */
#define DOWN_SHIFT(y)         printf("\033[%dB",y)

	/* to move the  cursor RIGHT x columns -- where 0<x<=79 */
#define RIGHT_SHIFT(x)        printf("\033[%dC",x)

	/* to move the cursor LEFT x columns -- where 0<x<=79 */
#define LEFT_SHIFT(x)         printf("\033[%dD",x)

	/* to save the current cursor position (internal to ansi.sys) */
#define SAVE_POSITION   printf("\033[s")

	/* to restore cursor to SAVE_POSITION position */
#define RESTORE_POSITION        printf("\033[u")

	/* to clear screen */
#define CLS             printf("\033[2J")

	/* to erase a line from the cursor position to the end of line */
#define LINE_CLEAR      printf("\033[K")

	/* to change screen attributes as follows: x=0, restore to normal */
	/* x=1, high intensity; x=4, underscore (monochrome screen only) */
	/* x=5, blinking chars; x=7, reverse video; x=8, invisible display */
#define CHAR_ATTRIBUTE(x)       printf("\033[%dm",x)

#define NORMAL 0
#define HIGH_INTENSITY 1
#define UNDERSCORE 4
#define BLINKING 5
#define REVERSE_VIDEO 7
#define INVISIBLE 8


	/* tab to an absolute position on the screen */
#define TAB(x)  LEFT_SHIFT(79); RIGHT_SHIFT(x)

       /* vtab tabs vertically to an absolute row */
#define VTAB(y) UP_SHIFT(24); DOWN_SHIFT(y)

#endif

