// VECGRPH.CPP 
//		quick graph routine for Vectors using SEG and WTWG libs.
//
#include <stdlib.h>
#include "wtwg.h"
#include "seg.h"
#include "vector.h"

		// character size of window
#define WIDTH  40
#define HEIGHT  15

void graphVector ( Vector &V, char *title, float  xbase, int segWin )
	{
	int  l, t;				// window location;

	int key;
	
	wlocate ( &l, &t, WIDTH, HEIGHT );
	
	wopen ( l,t,l+WIDTH, t+HEIGHT, WHITE, DOUBLE_BORDER, WHITE, WSAVE2RAM );

	if ( title != NULL )
		{
		wtitle ( title );
		}
		
	SetCurrentWindow (segWin);
	

	DefGraphWindow ((l+1)*wpxchar, (t+1)*wpychar,
					(l+WIDTH)*wpxchar, (t+HEIGHT)*wpychar,
					segWin );

	SetWin2PlotRatio (segWin, 0.1, 0.05, 0.2, 0.05);

	int   Vn = V.dim();

	if ( xbase <= 0 ) xbase = (float) Vn;

	Vector Vbase(Vn);
	Vbase.base ( 0, xbase );

	SelectColor ( LIGHTGRAY );
	AutoAxes (Vbase, V, Vn, /* axisflag= */ 0 );
	LinePlotData ( Vbase, V, Vn, WHITE, SOLID_LINE );
	
	
	wbutton_add ( "ESCAPE", (WIDTH/2)-3, HEIGHT-1 ,7, ESCAPE, 0 );
	while ( ESCAPE != (key=wgetc()))  { /*wait*/ }
	wclose ();	
	
	return;		// graphVector()
	}
	/*------------ end of VECGRPH.CPP ---------------------*/