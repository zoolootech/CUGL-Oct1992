/*  wn_glob.h
 *
 *  Global header for Unicorn Windows Version 3.0
 *
 */

/*			  ulc, urc, llc, lrc, top, side 	      */
BORDER	    bdr_ln    = { 218, 191, 192, 217, 196, 179 };
BORDER	    bdr_dln   = { 201, 187, 200, 188, 205, 186 };
BORDER	    bdr_sln   = {  32,	32,  32,  32,  32,  32 };
BORDER	    bdr_12n   = { 213, 184, 212, 190, 205, 179 };
BORDER	    bdr_21n   = { 214, 183, 211, 189, 196, 186 };


WINDOW	 *listhead = ( WINDOWPTR ) NULL;
WINDOW	 *listtail = ( WINDOWPTR ) NULL;

int insert_mode = 0;

int sound_on	= 1;   /* sound on by default	   */
int auto_exit	= 1;   /* auto_exit on by default  */


WINDOWPTR   wnd_message = ( WINDOWPTR ) NULL;

WINDOWPTR   wnd_hlp;

WINDOWPTR   wnd_err;
