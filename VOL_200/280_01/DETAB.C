/* [DETAB.c of JUGPDS Vol.46] */
/*
*****************************************************************
*								*
*	Written by  Hakuo Katayose (JUG-CP/M No.179)		*
*		    49-114 Kawauchi-Sanjuunin-machi		*
*		    Sendai, Miyagi 980                          *
*		    Phone: 0222-61-3219				*
*								*
*       Modifird by Toshiya Oota   (JUG-CPM No.10)              *
*                   Sakae ko-po 205 				*
*		    5-19-6 Hosoda				*
*		    Katusikaku Tokyo 124			*
*								*
*		for MS-DOS Lattice C V3.1J & 80186/V20/V30	*
*								*
*	Compiler Option: -ccu -k0(1) -ms -n -v -w		*
*								*
*	Edited & tested by Y. Monma (JUG-CP/M Disk Editor)	*
*			&  T. Ota   (JUG-CP/M Sub Disk Editor)	*
*								*
*****************************************************************
*/

/* Library functions for Software Tools */

#include "stdio.h"
#include "dos.h"
#include "ctype.h"
#include "tools.h"
#include "toolfunc.h"

/* detab - convert tabs to euivalent number of blanks */

void	main(argc, argv)
int	argc;
char 	**argv;

{
char	tabs[MAXLINE];
int	c, col ;
void	settab();
int	tabpos();

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
}


int	tabpos( col, tabs )
int	col;
char	tabs[];

{
	return( (col > MAXLINE) ? YES : (int) tabs[col] );
}


void	settab(tabs, maxlen)
int	maxlen;
char	tabs[];
{
int	i;

	for(i = 0; i <= maxlen; i++)
		tabs[i] = ( ( i % 8 == 1 ) ? YES : NO );
	return;
}
