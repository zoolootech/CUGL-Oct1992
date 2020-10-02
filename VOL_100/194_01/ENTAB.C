/* [ENTAB.C of JUGPDS Vol.17]
*****************************************************************
*								*
*	Written by  Hakuo Katayose (JUG-CP/M No.179)		*
*		    49-114 Kawauchi-Sanjuunin-machi		*
*		    Sendai, Miyagi 980                          *
*		    Phone: 0222-61-3219				*
*								*
*	Edited & tested by Y. Monma (JUG-C/M Disk Editor)       * 
*								*
*****************************************************************
*/

/* entab - replace blanks by tabs and blanks */

#include "stdio.h"
#include <dio.h>


main(argc, argv)
int	argc;
char	**argv;

{
	int	c, col, i, newcol, tabs[MAXLINE];

	dioinit(&argc, argv);
	settab(tabs, MAXLINE);
	col = 1;
	do {
		newcol = col;
		while ( ( c = getchar() ) == BLANK ) {
			newcol++;
			if ( tabpos( newcol, tabs ) == YES ) {
				putchar( TAB );
				col  = newcol;
 				}
			}
		for ( ; col < newcol; col++ )
			putchar( BLANK );
		if ( c == EOF )
			break;
		putchar( c );
		col = ( c == NEWLINE ) ? 1 : col + 1;
		} while(1);
	dioflush();
}
