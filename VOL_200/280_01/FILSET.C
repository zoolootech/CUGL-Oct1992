/* [filset.c of JUGPDS Vol.17] */
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

/* filset - expand set at array[i] into set[j], stop at delim */ 
void filset(delim, array, i, set, j, maxsiz)
int	*i, *j, maxsiz;
char	array[], delim, set[];

{
char	*digits, *lowalf, *upalf ,*kana, c;
void dodash();

	digits = "0123456789";
	lowalf = "abcdefghijklmnopqrstuvwxyz";
	upalf  = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	kana   = "¶ßß®©™´¨≠ÆØ±≤≥¥µ∂∑∏π∫ªºΩæø¿¡¬√ƒ≈∆«»… ÀÃÕŒœ–—“”‘’÷◊ÿŸ⁄€‹¶›";
	for (; (c = array[*i]) != delim && c != EOS; (*i)++)
		if (c == ESCAPE)			/*Modify 1986-11-27*/
			addset(esc(array, i), set, j, maxsiz);
		else if (c != DASH)
			addset(c, set, j, maxsiz);	/*Modify 1986-11-27*/
		else if (*j == 0 || array[(*i)+1] == EOS)  /*Last Literal*/
			addset(DASH, set, j, maxsiz);
		else if (cindex(digits, set[(*j)-1]) != ERROR)
			dodash(digits, array, i, set, j, maxsiz);
		else if (cindex(lowalf, set[(*j)-1]) != ERROR)
			dodash(lowalf, array, i, set,j, maxsiz);
		else if (cindex(upalf, set[(*j)-1])  != ERROR)
			dodash(upalf, array, i, set, j, maxsiz);
		else if (cindex(kana , set[(*i)-1])  != ERROR)
			dodash(kana , array, i, set, j, maxsiz);
		else
			addset(DASH, set, j, maxsiz);
	return;
}
