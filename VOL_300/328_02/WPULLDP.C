/* wpulldown_pages()
 *	install pulldown menus for programs with multiple video pages
 *
 */

#include "wsys.h"

void wpulldown_pages ( WMENU *topmenu )
	{

	int n, savepage;

	savepage = wnextpage;

	wnextpage = 0;
	wpulldown ( topmenu );

	for ( n = 1; n<= wlastpage; ++n )
		{
		wnextpage = n;
		wpulldown_draw ();
		}
	wnextpage = savepage;

	return;

	}