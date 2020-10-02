/* [quick.c of JUGPDS Vol.46] */
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

void quick(v, l, r, comp, exch)
char	*v[];
int	l, r;
int	(*comp)(), (*exch)();
{
char	*vx;
int	i, j;

	i = l;  j = r;
	vx = v[ (l+r)/2 ];
	while(i <= j) {
		while( (*comp)(v[i], vx) < 0 )
			i++;
		while( (*comp)(vx, v[j]) < 0 )
			j--;
		if(i <= j) {
			(*exch)(&v[j], &v[i]);
			i++;
			j--;
			}
		}
	if(l < j) quick(v,l,j,comp,exch);
	if(i < r) quick(v,i,r,comp,exch);
}
