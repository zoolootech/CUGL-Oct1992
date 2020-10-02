/* [expand.c of JUGPDS Vol.46] */
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

/*  expand - uncompress standard input */

#define	RCODE		127

void	main(argc,argv)
int	argc;
char	*argv[];

{
	int	c, code;

	while ((code = getchar()) != EOF)
		if (code == RCODE) {
			if ((c = getchar()) == EOF)
				break;
			if ((code = getchar()) == EOF)
				break;
			code -= '0';
			while (code--)
				putchar(c);
		} else {
			code -= '0';
			while (code--) {
				if ((c = getchar()) == EOF)
					break;
				putchar(c);
				}
			if ( c == EOF )
				break;
			}
}
