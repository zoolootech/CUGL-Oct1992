/* [PAT.C of JUGPDS Vol.17]
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

/* pat - makepat and other functions to handle strings */

#include <bdscio.h>
#include "def.h"

/* makpat - make pattern from arg[from], terminate at delim */
makpat(arg, from, delim, pat)
char	delim, arg[], pat[];
int	from;

{
	int	i, j, lastj, lastcl, lj, len, k, kk[10], dp;
	char	c;

	j = 0; lastj = 0; lastcl = 0; k = '1'; dp = 0;
	for (i = from; arg[i] != delim && arg[i] != EOS; i++) {
		lj = j;
		c = arg[i];
		if (c == NPAT) {
			addset(BPAT, pat, &j, MAXPAT);
			addset(k, pat, &j, MAXPAT);
			kk[dp++] = k++;
			}
		else if (c == NPATEND) {
			addset(EPAT, pat, &j, MAXPAT);
			addset(kk[--dp], pat, &j, MAXPAT);
			}
		else if (c == ANY) {
			addset(ANY, pat, &j, MAXPAT);
			}
		else if (c == BOL && i ==from) {
			addset(BOL, pat, &j, MAXPAT);
			}
		else if (c == EOL && arg[i+1] == delim) {
			addset(EOL, pat, &j, MAXPAT);
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
			addset(CHAR, pat, &j, MAXPAT);
			addset(esc(arg, &i), pat, &j, MAXPAT);
			}
		lastj = lj;
		}
	if (arg[i] != delim)
		return ERROR;
	if (j >= MAXPAT)
		return ERROR;
	if (dp < 0)
		return ERROR;
	pat[j] = EOS;
	return(k - '1');
}


_copy(s, d, l)
char	*s, *d;
int	l;

{
	int	i;

	for (i=0; i < l; i++)
		*d++ = *s++;
	return;
}

/* getccl - expand char class at arg[i] into pat[j] */
getccl(arg, i, pat, j)
char	arg[], pat[];
int	*i, *j;

{
	int	jstart;

	(*i)++;
	if (arg[*i] == NOT) {
		pat[(*j)++] = NCCL;
		(*i)++;
		}
	else
		pat[(*j)++] = CCL;
	jstart = *j;
	addset(0, pat, j, MAXPAT);
	filset(CCLEND, arg, i, pat, j, MAXPAT);
	pat[jstart] = (*j) - jstart - 1;
	return (arg[*i] == CCLEND) ? OK : ERROR;
}

/* stclos - insert closure entry at pat[j] */
stclos(pat, j, lastj, lastcl)
char	pat[];
int	*j, lastj, lastcl;

{
	int	jt, jp;

	for (jp = (*j) - 1; jp >= lastj; jp--) {
		jt = jp + CLOSIZE;
		addset(pat[jp], pat, &jt, MAXPAT);
		}
	(*j) += CLOSIZE;
	jp = lastj;
	addset(CLOSURE, pat, &lastj, MAXPAT);
	addset(0, pat, &lastj, MAXPAT);
	addset(lastcl, pat, &lastj, MAXPAT);
	addset(0, pat, &lastj, MAXPAT);
	return jp;
}

/* filset - expand set at array[i] into set[j], stop at delim */ 
filset(delim, array, i, set, j, maxsiz)
int	*i, *j, maxsiz;
char	array[], delim, set[];

{
	char	*digits, *lowalf, *upalf, c;

	digits = "0123456789";
	lowalf = "abcdefghijklmnopqrstuvwxyz";
	upalf  = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	for (; (c = array[*i]) != delim && c != EOS; (*i)++)
		if (c == ESCAPE || c == UPALLOW)
			addset(esc(array, i), set, j, maxsiz);
		else if (c != DASH)
			addset(tolower(c), set, j, maxsiz);
		else if (*j == 0 || array[(*i)+1] == EOS)
			addset(DASH, set, j, maxsiz);
		else if (cindex(digits, set[(*j)-1]) >= 0)
			dodash(digits, array, i, set, j, maxsiz);
		else if (cindex(lowalf, set[(*j)-1]) >= 0)
			dodash(lowalf, array, i, set,j, maxsiz);
		else if (cindex(upalf, set[(*j)-1]) >= 0)
			dodash(upalf, array, i, set, j, maxsiz);
		else
			addset(DASH, set, j, maxsiz);
	return;
}

/* esc - map array[i] into escaped charaters if appropriate */
esc(array, i)
int	*i;
char	array[];

{
	char	c;

	c = tolower(array[*i]);
	if (c == UPALLOW && isalpha(array[(*i)+1])) {
		c = array[++(*i)];
		return(toupper(c));
		}
	if (c != ESCAPE)
		return(c);
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
	return(c);
}

/* dodash - expand array[i-1] -array[i+1] into set[j]...from valid */
dodash(valid, array, i, set, j, maxsiz)
int	*i, *j, maxsiz;
char	array[], set[], valid[];

{
	int	k, limit;

	(*i)++;
	(*j)--;
	limit = cindex(valid, esc(array, i));
	for (k = cindex(valid, set[*j]); k <= limit; k++)
		addset(valid[k], set, j, maxsiz);
}

/* addset - put c in set[j] if it fits, increment j */
addset(c, set, j, maxsiz)
char	c, set[];
int	*j, maxsiz;

{
	if (*j > maxsiz)
		return(NO);
	set[(*j)++] = c;
	return(YES);
}

/* xindex - invert condition returned by index */
xindex(array, c, allbut, lastto)
char	array[];
int	c, allbut, lastto;

{
	if (c == EOF)
		return(ERROR);
	if (allbut == NO)
		return(cindex(array,c));
	if (cindex(array,c) >= 0)
		return(ERROR);
	return(++lastto);
}

/* cindex - find character c in string str */ 
cindex(str, c)
char	*str;
int	c;

{
	int	i;
	i = 0;
	while(*str && *str != c) *str++, i++;
	return (*str == 0 ? ERROR : i);
}

/* getlin - copy line */
getlin(s, lim)
char	*s;
int	lim;

{
	char	*p;
	int	c;

	p = s;
	while (--lim > 0 && (c = getchar()) != EOF && c != NEWLINE)
		*s++ = c;
	if (c == NEWLINE)
		*s++ = c;
	*s = EOS;
	return(s-p);
}
