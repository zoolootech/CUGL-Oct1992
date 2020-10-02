/* [DETAB.C of JUGPDS Vol.17]
*****************************************************************
*								*
*	Written by  Hakuo Katayose (JUG-CP/M No.179)		*
*		    49-114 Kawauchi-Sanjuunin-machi		*
*	            Sendai, Miyagi 980                          *
*		    Phone: 0222-61-3219				*
*								*
*	Edited & tested by Y. Monma (JUG-C/M Disk Editor)       * 
*								*
*****************************************************************
*/

/* detab - convert tabs to euivalent number of blanks */

#include "stdio.h"
#include <dio.h>

main(argc, argv)
int	argc;
char 	**argv;

{
	char	tabs[MAXLINE];
	int	c, col, getchar();

	dioinit(&argc, argv);	/* initialize redirection */
	settab(tabs, MAXLINE);
	col = 1;
	while ( (c = getchar() ) != EOF )
		if ( c == '\t' )
			do {
				putchar(' ');
				col++;
		}
			while( tabpos(col, tabs) == NO );
		else {
 			putchar(c);
			col = ( ( c == '\n' ) ? 1 : col + 1 );
			}
	dioflush();
}


tabpos( col, tabs )
int	col;
char	tabs[];
{
	return( (col > MAXLINE) ? YES : tabs[col] );
}


settab(tabs, maxlen)
int	maxlen;
char	tabs[];
{
	int	i;

	for(i = 0; i <= maxlen; i++)
		tabs[i] = ( ( i % 8 == 1 ) ? YES : NO );
	return;
}
