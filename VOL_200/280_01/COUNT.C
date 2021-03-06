/* [count.c of JUGPDS Vol.46] */
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

/* count - count characters, lines & words of a file */

void	main(argc, argv)
int	argc;
char	**argv;

{
int   c, wc, nl, nc, inword;

	nc = 0;
	nl = 0;
	wc = 0;
	inword = NO;
	while ( (c = getchar() ) != EOF ) {
		if ( c == BLANK || c == TAB )
			inword = NO;
		else if ( c == NEWLINE ) {
			inword = NO;
			nl++;
			}
		else if ( inword == NO ) {
			inword = YES;
			wc++;
			}
		nc++;
		}
	printf("CNT101 chars =%6d, words =%5d, lines =%4d\n", nc, wc, nl);
}
