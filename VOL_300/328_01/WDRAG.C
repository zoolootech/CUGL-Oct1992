/*! WDRAG.C ()
 *
 *	drag the current window around the screen
 *	accepts a key (ARROW, PAGE UP/DN, END or HOME)
 *	and moves window as indicated
 *
 *	RETURN 0 if unsuccessful move.
 *
 */
#include "wsys.h"




int wdrag (int keystroke)
	{
	int extra, extra2;

	int l,t;

	if ( w0-> winsave == NULL )
		{
		return (0);
		}

	/* need extra margin for borders ?
	 */
	extra = (w0-> winbox)? 1: 0;
	extra2 = extra *2;



	l = w0-> winleft ;
	t = w0-> wintop  ;

	switch (keystroke)
		{

	case (LT_ARROW):
		if (l >6)
			l -= 5;
		else
			keystroke=0;
		break;

	case (RT_ARROW):
		if (l +6 <= wxabsmax )
			l += 5;
		else
			keystroke=0;
		break;

	case (DN_ARROW):
		if (t +6 <= wyabsmax )
			t += 5;
		else
			keystroke=0;
		break;

	case (UP_ARROW):
		if (t>6)
			t -= 5;
		else
			keystroke=0;
		break;

	case (HOME):
		l= 1+extra;
		break;

	case (END):
		l = wxabsmax -( w0->winxmax ) -extra2;
		break;

	case (PAGE_UP):
		t = 2+extra;
		break;

	case (PAGE_DN):
		t = wyabsmax -( w0->winymax ) -extra2;
		break;

	default:
		keystroke =0;

		}	/* end swtich */

	if ( keystroke != 0 )
		{
		wrelocate(l,t);
		}


	return (keystroke);	/* wdrag */
	}


        
