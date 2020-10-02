/* [stclos.c of JUGPDS Vol.17] */
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

/* stclos - insert closure entry at pat[j] */
int  stclos(pat, j, lastj, lastcl)
char	pat[];
int	*j, lastj, lastcl;

{
int	jt, jp;

	for (jp = (*j) - 1; jp >= lastj; jp--) {
		jt = jp + CLOSIZE;
		addset(pat[jp], pat, &jt, MAXSET);
		}
	(*j) += CLOSIZE;
	jp = lastj;
	addset(CLOSURE, pat, &lastj, MAXSET);
	addset(0, pat, &lastj, MAXSET);
	addset(lastcl, pat, &lastj, MAXSET);
	addset(0, pat, &lastj, MAXSET);
	return jp;
}
