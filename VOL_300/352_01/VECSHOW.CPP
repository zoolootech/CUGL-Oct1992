// VECSHOW.CPP
//		show values of a Vector in an on-screen window
//		used principally for debugging.
//
#include <stdlib.h>
#include <stdio.h>
#include "wtwg.h"
#include "dblib.h"
#include "vector.h"

void Vector::show(char *title )
	{
	/* room for 100 numbers, 5 per row, 20 rows */
	#define NUM_PER_ROW	5
	#define BYTES_PER_NUM	9
	#define ROWLEN          ( NUM_PER_ROW*(1+BYTES_PER_NUM) + 5 )

	int ndata = n;

	wopen ( 2,2, ROWLEN,20, (LIGHTBLUE<<4),SINGLE_BORDER,(LIGHTBLUE<<4),
			 WSAVE2RAM );
	wtitle (title);

	for ( int i=0; ( i<= 100 && i<= ndata ); ++i)
		{
		wprintf ("%.7f ", v[i]);
		if ( i % NUM_PER_ROW )
			{
			wputc ('\n');
			}
		}
	wgoto ( 5, 19 );		// last row
	wputs ("Press any key...");

	wgetc();
	wclose();

	return; 		// Vector::show()
	}
//---------------- end of VECSHOW.CPP -----------------------