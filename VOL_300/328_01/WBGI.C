/*! WBGI.C
 *
 *
 *  Interface the windows library routines with TurboC BGI
 *	all calls to the Borland Graphics Interface are made in this module.
 *
 *	walign () - 	set the BGI veiwport, activepage, and position
 *			also set cursor status, video-ram pointer, etc.
 *                      to match the current window.
 *
 *	wgrmode ()-	set graphics mode, initialize the BGI routines
 *	wtxmode ()-	shut down the BGI drivers, restore CRT mode = text.
 *	wmovepxpy()- 	move current graphics location to winpx,winpy.
 *	wherepxpy()-	get current graphics location, set in px, py.
 *
 * Second half of this routine contains code for microsoft C version
 *
 */

#ifdef __TURBOC__
	/* This file contains TURBOC specific graphics library calls 
	 * for other compilers, you must generate a functionally equivalent
	 * file for all of the routines defined here.
	 */


#include  "wscreen.h"
#include  "wsys.h"

#include <graphics.h>   /* Turbo C graphics */



#ifdef TEXTONLY

	/* in text only library, never call the graphics drivers 
	 * Therefore, provide 'dummy entry points for the linker
	 */

void  	walign  	( int x )       {return;}
void  	wgraphmode 	( void )	{return;}
void 	wtextmode 	( void )	{return;}
void 	wherepxpy 	( void )	{return;}
void 	movepxpy  	( void )	{return;}

#else 		/* graphics version of these functions */







void 	walign  (int currpage)
	{
	/* This function 'aligns' the graphics driver with the windows settings
	 * settings which must be aligned include 
	 * activepage  (which page graphics library calls write to)
	 * w0-> pixel bounds (winpxmax, winpymax) computed from text-bounds
	 * setviewport () sets graphics driver co-ords to current window
	 * 				 and sets clipping ON
	 *current graphics position set to current window pixel position.
 	 */
	int pl, pt;	/* pixels offsets */


	if ( wmode == 'T' )
		{
		return;
		}


	/* tell BGI to start using the current windows page #
	 * only if it doesn't match the curretn active page #
	 */
	if ( currpage != w0-> winpage )
		{
		setactivepage(w0-> winpage);
		}
	w0-> winpxmax = wpxchar * (w0->winxmax)
			 + wpxchar-1;
	w0-> winpymax = wpychar * (w0->winymax)
			 + wpychar -1;

	pl = wpxchar * w0->winleft ;
	pt = wpychar * w0->wintop  ;

	setviewport ( pl , pt, (pl + w0->winpxmax), (pt + w0->winpymax), 1);

	wmovepxpy();

	return;
	}  /*end of walign for BGI */








/* wmovepxpy()
 * 	set BGI graphics position to match the position in the current window
 */
void wmovepxpy (void)
	{
	if ( wmode == 'G' )
		{
		moveto ( w0-> winpx, w0-> winpy );
		}
	return;	/* wmovepxpy */
	}


/* wherepxpy ()
 *	get the current x,y position from the BGI driver. save in WINDOW 
 */
void wherepxpy (void)
	{
	if ( wmode == 'G' )
		{
		w0->winpx = getx ();
		w0->winpy = gety ();
		}
	return;	/* wherepxpy */
	}

/* wtextmode()
 *	restore CRT mode to text, shut down BGI
 */
void wtextmode (void)
	{
	if ( wmode == 'G' )
		{
		restorecrtmode();
		closegraph();
		}
	return;	/* wtextmdoe() */
	}


static char errmsg[] = "Graphics initializing error #= check DOS ERRORLEVEL";


/* wgraphmode()
 *	initialize BGI to appropriate graphics mode
 *  initialize screen size globals 
 *  setup mouse to page 0 in hercules mode.
 */
void wgraphmode (void)
	{
	int BGImode, BGIdriver, n;



	switch (wmonitor)
		{
	case	('V'):
		BGIdriver = VGA;
		if ( wneedpage )
			{
			/* video mode 0x10 */
			BGImode = VGAMED;	/* 640*350, 2 pages*/
			wlastpage = 1;
			}
		else
			{
			/* video mode 0x12 */
			BGImode = VGAHI;	/* 640*480 */
			wlastpage =0;
			}
			break;

	case 	('E'):
		BGIdriver = EGA;
		/* video mode 0x10 */
		BGImode   = EGAHI;		/* 640 * 350 */
		wlastpage = wega256 ? 1 : 0; 	/*need 256k to page*/
		break;

	case 	('H'):
		BGIdriver   = HERCMONO;
		BGImode     = HERCMONOHI;

		/* tell the mouse driver that we're starting on page 0
		 * ( This is important for Microsoft mouse but not Logitech.)
		 *	see the Microsoft mouse book ---
		 *	(or see notes in file wmouse.c)
		 */
		*( HERC_ROM_VIDMODE_AREA ) = 6;

		wlastpage   =1;
		break;

	default:
		werror ('W', "UNSUPPORTED MONITOR");

		}	/* end switch on monitor */




	initgraph ( &BGIdriver, &BGImode, wpath_BGI );
	if ( grOk != (n=graphresult() ) )
		{
		werror (n, errmsg);
		}
	wpxabsmax = getmaxx ();
	wxabsmax  = ((wpxabsmax +1) / wpxchar) -1;
	wpyabsmax = getmaxy ();
	wyabsmax  = ((wpyabsmax +1) / wpychar) -1;


	return;	/*wgraphmode */
	}



#endif 	/* end graphics version */

#else	
	/*---------------------- Microsoft C version ----------------------*/

#include <stdlib.h>
#include <graph.h>
#include <dos.h>

#include "wscreen.h"
#include "wsys.h"

void    walign          ( int currpage )
	{
	/* This function 'aligns' the graphics driver with the windows settings
	 * settings which must be aligned include:
	 * activepage  (which page graphics library calls write to)
	 * w0-> pixel bounds (winpxmax, winpymax) computed from text-bounds
	 * setviewport () sets graphics driver co-ords to current window
	 *                               and sets clipping ON
	 *current graphics position set to current window pixel position.
	 */
	int pl, pt;     /* pixels offsets */


	if ( wmode == 'T' )
		{
		return;
		}


	/* tell MSC to start using the current windows page #
	 * only if it doesn't match the curretn active page #
	 */
	if ( currpage != w0-> winpage )
		{
		_setactivepage(w0-> winpage);
		}
	w0-> winpxmax = wpxchar * (w0->winxmax)
			 + wpxchar-1;
	w0-> winpymax = wpychar * (w0->winymax)
			 + wpychar -1;

	pl = wpxchar * w0->winleft ;
	pt = wpychar * w0->wintop  ;

	_setviewport ( pl , pt, (pl + w0->winpxmax), (pt + w0->winpymax));


	wmovepxpy();

	
	
	return;		/* walign, microsoft */
	}


/* wmovepxpy()
 *      set MSC graphics position to match position in the current window
 */
void wmovepxpy (void)
	{
	if ( wmode == 'G' )
		{
		_moveto ( w0-> winpx, w0-> winpy );
		}
	return; /* wmovepxpy */
	}


/* wherepxpy ()
 *      get the current x,y position from the driver. save in WINDOW
 */
void wherepxpy (void)
	{
	struct xycoord  xy;
	if ( wmode == 'G' )
		{
		xy = _getcurrentposition();
		w0->winpx = xy.xcoord;
		w0->winpy = xy.ycoord;
		}
	return; /* wherepxpy, microsoft */
	}

/* wtextmode()
 *      restore CRT mode to text, shut down BGI
 */
void wtextmode (void)
	{
	short  vidmode;
	if ( wmode == 'G' )
		{
		_setvideomode ( _DEFAULTMODE );
		}
	return; /* wtextmode, microsoft */
	}


static char errmsg[] = "Graphics initializing error #= check DOS ERRORLEVEL";


/* wgraphmode()
 *      initialize MSC to appropriate graphics mode
 *  initialize screen size globals
 *  setup mouse to page 0 in hercules mode.
 */
void wgraphmode (void)
	{
	short vidmode;
	int  n;

	int maxx, maxy;


	switch (wmonitor)
		{
	case    ('V'):
		vidmode   = _VRES16COLOR;
		maxx = 640;
		maxy = 480;
		if ( wneedpage )
			{
			/* video mode 0x10 */
			vidmode = _ERESCOLOR;         /* 640 x 350 */
			maxy = 350;
			wlastpage = 1;
			}
		else
			{
			/* video mode 0x12 */
			wlastpage =0;
			}
			break;

	case    ('E'):
		vidmode = _HRES16COLOR;         /* 640 x 350 */
		/* video mode 0x10 */
		wlastpage = wega256 ? 1 : 0;    /*need 256k to page*/
		maxx = 640;
		maxy = 350;
		break;

	case    ('H'):
		vidmode    = _HERCMONO;
		maxx = 720;
		maxy = 348;

		/* tell the mouse driver that we're starting on page 0
		 * ( This is important for Microsoft mouse but not Logitech.)
		 *      see the Microsoft mouse book ---
		 *      (or see notes in file wmouse.c)
		 */
		*( HERC_ROM_VIDMODE_AREA ) = 6;

		wlastpage   =1;
		break;

	default:
		werror ('W', "UNSUPPORTED MONITOR");

		}       /* end switch on monitor */




	n = _setvideomode ( vidmode );
	if ( n == 0 )
		{
		werror (n, "Error initializing graphics" );
		}
	wpxabsmax = maxx;
	wxabsmax  = ((wpxabsmax +1) / wpxchar) -1;
	wpyabsmax = maxy;
	wyabsmax  = ((wpyabsmax +1) / wpychar) -1;


	return; /*wgraphmode, microsoft */
	}





#endif  /* end if not TURBOC...Microsoft */

/*--------------------- end of WBGI.C ---------------------*/
