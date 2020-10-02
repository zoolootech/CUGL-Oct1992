/*
	RED special key definitions and compiler options.

	Source:  red1.h
	Version: February 15, 1985; January 18, 1990.

	Written by
	
		Edward K. Ream
		166 N. Prospect
		Madison WI 53705
		(608) 257-0802


	PUBLIC DOMAIN SOFTWARE

	This software is in the public domain.

	See red.h for a disclaimer of warranties and other information.
*/

/*
	Define how modes change initially.
*/
#define DEFIMODE EDITMODE	/* Initial mode & mode after commands.	*/
#define DEF1MODE INSMODE	/* Mode after UP_INS and DOWN_INS.	*/
#define DEF2MODE EDITMODE	/* Mode after UP and DOWN.		*/	

/*
	Define which keys are used for special edit functions.
*/
#define AGAIN 1			/* control a (again)	*/
#define ESCAPE 27		/* escape sequence	*/

#define UP_INS 10		/* line feed		*/
#define DOWN_INS 13		/* carriage return	*/

#define UP 21			/* control u (up)	*/
#define DOWN 4			/* control d (down)	*/
#define LEFT 12 		/* control l (left)	*/
#define RIGHT 18		/* control r (right)	*/

#define WORD_B 2		/* control b (back)	*/
#define WORD_F 6		/* control f (forward)	*/

#define DEL1 8 			/* control h (BS)	*/
#define DEL2 127		/* del			*/
#define ZAP 26			/* control z (zap)	*/
#define UNDO 24			/* control x (undo)	*/

#define SPLIT 19		/* control s (split)	*/
#define JOIN 7			/* control g (glue)	*/
#define VERBATIM 22		/* control v (verbatim)	*/

#define INS 14			/* control n (iNsert)	*/
#define CMND 3			/* control c (command)	*/
#define EDIT 5			/* control e (edit)	*/
#define OVER 20			/* control t (Overtype)	*/

#define SCRN_DN 15		/* control o		*/
#define SCRN_UP 23		/* control w		*/
#define PAGE_UP 17		/* control q		*/
#define PAGE_DN 16		/* control p		*/

/*
	Special key codes.  The values of these codes do not matter
	as long as they do not conflict with the values above.
*/
#define HOME_KEY 200
#define END_KEY  201

/*
	Define length and width of screen and printer.
*/
#define SCRNW 80
#define SCRNW1 79
#define SCRNL 25
#define SCRNL1 24
#define SCRNL2 23
#define LISTW 132
