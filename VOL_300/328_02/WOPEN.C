/*! wopen
 *
 *
 *  open a window, save previous contents, draw borders
 *
 *
 *
 */
#include  "wscreen.h"
#include  "wsys.h"






WINDOW *wopen   (int x, int y,  int xmax,  int ymax,
		unsigned char color, int boxtype, unsigned char box_color,
		unsigned char save2)
	{


	wdefine (x, y, xmax, ymax, color, boxtype, box_color);




	/* save contents of old screen underlying new window
	 * to restore when wclose
	 */
	if (save2 == WSAVE2RAM)
		{
		wsave();
		}


	/* store and paint new borders surrounding window.
	 */
	if (boxtype)
		{

		wframe (x-1, y-1, x+xmax, y+ymax, boxtype, box_color);
		wclear();
		}


	return(w0);
	}  /*end of wopen*/



