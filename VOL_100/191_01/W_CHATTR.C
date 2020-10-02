/*#title 'W_CHATTR    04/18/86' */
/****************************************************************
**                                                             **
**      file: W_CHATTR.C   									   **
**                                                             **
**      library function:   w_chattr                           **
**                                                             **
**      programmer: George Woodley                             **
**                                                             **
*****************************************************************

    NAME
        w_chattr - Change the display attributes
				   of a message in a c_window.

    SYNOPSIS
        success = w_chattr (x, y, attrib, n_bytes);
			WORD success;			1 = Success, 0 = error.
			WORD int x, y;				Starting coords of msg.
			char attrib;			New display attribute.
			WORD n_bytes;			Size of message.

    DESCRIPTION
        W_CHATTR positions the cursor in a Chris McVicar C_WINDOW
		and sets the attribute of N_BYTES bytes at the current
		cursor position to the specified value.  If N_BYTES
		exceeds the window dimensions wraparound and scrolling will
		take place according to the rules of C_WINDOW.  

		The cursor position will be left undisturbed.

		W_CHATTR returns TRUE = 1 if the x,y coordinates are within
		the window, and FALSE = 0 if they are outside the window.
			
    CAUTIONS
        None.

****************************************************************/
/*#eject */

#include "STD.H"
#include "C_WDEF.H"
extern char cw_ulx;   /* upper lft X of writeable window, absolute */
extern char cw_uly;	  /* upper lft Y of writeable window, absolute */
extern char cw_wid;	  /* width of current window, excluding border */
extern char cw_hgt;	  /* height of current window, excludng border */
extern char cw_page;  /* display page in alphanumeric mode */
extern int  scrnbase; /* segment of screen display RAM */


/****************************************************************
    W_CHATTR function
****************************************************************/
w_chattr (x, y, attrib, n_bytes)

WORD x, y;				/* Starting coords of msg. */
char attrib;			/* New display attribute. */
WORD n_bytes;			/* Size of message. */
{
	WORD offset, count;

	if ((x >= cw_wid) || (y >= cw_hgt)) {
		w_msg ("W_CHATTR - Cursor out of range");
		return (FALSE);
        }
	for (count = 1; count <= n_bytes; count++)  {
		offset = (((y + cw_uly) * 80 + (x + cw_ulx)) * 2)
				 + (4000 * cw_page) + 1;
		poke (scrnbase, offset, &attrib, 1);

		x++;
		if (x >= cw_wid) {
			x = 0;
			y++;
			}
		if (y >= cw_hgt) {
			w_scroll (UP, 1);
			y = cw_hgt - 1;
			}
		}
	}
