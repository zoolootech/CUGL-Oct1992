/* [makpat.c of JUGPDS Vol.17] */
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

/* makpat - make pattern from arg[from], terminate at delim */
int  makpat(arg, from, delim, pat)
char	delim, arg[], pat[];
int	from;

{
int	i, j, lastj, lastcl, lj, len, k, kk[10], dp;
char	c;
int  addset();
void _copy();

	j = 0; lastj = 0; lastcl = 0;
	k = '1'; dp = 0;
	for (i = from; arg[i] != delim && arg[i] != EOS; i++) {
		lj = j;
		c = arg[i];
		if (c == NPAT) {
			addset(BPAT, pat, &j, MAXSET);
			addset(k, pat, &j, MAXSET);
			kk[dp++] = k++;
			}
		else if (c == NPATEND) {
			addset(EPAT, pat, &j, MAXSET);
			addset(kk[--dp], pat, &j, MAXSET);
			}
		else if (c == ANY) {
			addset(ANY, pat, &j, MAXSET);
			}
		else if (c == BOL && i ==from) {
			addset(BOL, pat, &j, MAXSET);
			}
		else if (c == EOL && arg[i+1] == delim) {
			addset(EOL, pat, &j, MAXSET);
			}
		else if (c == CCL) {
			if (getccl(arg, &i, pat, &j) == ERROR) break;
			}
		else if ((c == CLOSURE || c == PLUS) && i > from) {
			lj = lastj;
			if (pat[lj] == BOL ||
			    pat[lj] == EOL || 
			    pat[lj] == CLOSURE)
				break;
			if (c == PLUS) {
				len = j - lj;
				_copy(&pat[lj], &pat[j], len);
				j += len;
				lj += len;
				lastj += len;
				}
			lastcl = stclos(pat, &j, lastj, lastcl);
			}
		else {
			addset(CHAR, pat, &j, MAXSET);
			addset(esc(arg, &i), pat, &j, MAXSET);
			}
		lastj = lj;
		}
	if (arg[i] != delim)
		return ERROR;
	if (j >= MAXSET)
		return ERROR;
	if (dp < 0)
		return ERROR;
	pat[j] = EOS;
	return( (int) (k - '1') );
}
