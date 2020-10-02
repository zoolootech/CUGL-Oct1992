/* [esc.c of JUGPDS Vol.17] */
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

/* esc - map array[i] into escaped charaters if appropriate */
int  esc(array, i)
int	*i;
char	array[];

{
char	c;

	c = array[*i];					/*Modify 1986-11-27*/
	if (c != ESCAPE)
		return( (int) c);
	if (array[(*i)+1] == EOS)
		return(ESCAPE);
	c = array[++(*i)];
	c = tolower(c);
	if (c == 'b')
		return(BACKSPACE);
	if (c == 'f')
		return(FORMFEED);
	if (c == 'n')
		return(NEWLINE);
	if (c == 'r')
		return(CRETURN);
	if (c == 's')
		return(BLANK);
	if (c == 't')
		return(TAB);
	return( (int) c);
}
