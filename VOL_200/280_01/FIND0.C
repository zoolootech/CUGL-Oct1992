/*[FIND0.c of JUGPDS Vol.46]*/
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

/* find - find patterns in text */

#include "stdio.h"
#include "dos.h"
#include "tools.h"
#include "toolfunc.h"

void main(argc, argv)
int	argc;
char	**argv;

{
char	lin[MAXLINE], pat[MAXPAT];
char	opt_l,opt_id, *ap, *cp;
int	i, lno, nl, nx;

    if (argc < 2) {
	error("FND991 Usage: find0 [-l][-i] pattern <infile >outfile\n");
	exit();
    }
    opt_l = OFF;
    opt_id= OFF;

    nl = 1; i = 0;
    i = 0;
    while (--argc > 0) {
	if ((*++argv)[0] == '-')
	    for (ap = argv[0]+1; *ap != '\0'; ap++) {
		if (tolower(*ap) == 'l') {
		    opt_l = ON;
		    nl = 0;
		    for(; isdigit(nx = *(ap+1)); ap++)
			nl = nl*10 + nx - '0';
		    nl = (nl == 0 ? 1 : nl);
		}
		else if (tolower(*ap) == 'i')
		    opt_id = ON;
		else
		    fprintf(STDERR,
		    "FND901 [-%c]:Illigal option\n",tolower(*ap));
	    }
	cp = *argv;
    }
    if (getpat(cp, pat) == ERROR)
	exit(fprintf(STDERR, "FND903 Illegal pattarn.\n"));
    lno = 0;
    while (getlin(lin, MAXLINE) > 0) {
	lno += nl;
	if (match(lin, pat) == YES)
	    if (opt_l == ON) {
		if (opt_id == ON)
		    printf("FND101 %6d: %s", lno, lin);
		else
		    printf("%6d: %s", lno, lin);
	    }
	    else {
		if (opt_id == ON)
		    printf("FND103 %s", lin);
		else
		    printf("%s", lin);
	    }
    }
}

/* match - find match anywhere on line */
match(lin, pat)
char	lin[], *pat;
{
int i;


    for (i = 0; lin[i] != '\0'; i++)
    if (amatch(lin, i, pat) >= 0)
	return (YES);
    return(NO);
}

/* amatch (non-recursive) - look for stating at lin(from) */
amatch(lin, from, pat)
char	lin[], pat[];
int	from;

{
int	i, j, offset, stack;

    stack = 0;
    offset = from;
    for (j = 0; pat[j] != '\0'; j += patsiz(pat, j))
	if (pat[j] == CLOSURE) {
	    stack = j;
	    j += CLOSIZE;
	    for (i = offset; lin[i] != '\0'; )
		if (omatch(lin, &i, pat, j) == NO)
		    break;
		pat[stack + COUNT] = i - offset;
		pat[stack + START] = offset;
		offset = i;
	}
	else if (omatch(lin, &offset, pat, j) == NO) {
	    for (; stack > 0; stack = pat[stack + PREVCL]) {
		if (pat[stack + COUNT] > 0)
		    break;
	}
	if (stack <= 0)
	    return(-1);
	pat[stack + COUNT]--;
	j = stack + CLOSIZE;
	offset = pat[stack + START] + pat[stack + COUNT];
    }
    return(offset);
}

/* patsiz - returns size of pattern entry at pat(n) */
int patsiz(pat, n)
char	pat[];
int	n;

{
    switch (pat[n]) {
	case BPAT :
	case EPAT :
	case CHAR :
	      return(2);
	case BOL :
	case EOL :
	case ANY :
	      return(1);
	case CCL :
	case NCCL:
	      return (pat[n+1] + 2);
	case CLOSURE :
	      return CLOSIZE;
	default  :
	      exit(fprintf(STDERR, "FND911 Error in patsiz: can't happen.\n"));
	}
}

/* omatch - try to match a single pattern at pat(j) */
int  omatch(lin, i, pat, j)
char	lin[], pat[];
int	*i, j;

{
int	bump;
char	c;

    c = lin[ *i ];
    if (c == '\0')
	return(NO);
    bump = -1;
    switch (pat[j]) {
	case BPAT :
	case EPAT : return(YES);
	case CHAR : if (c == pat[j+1])
			bump= 1;
		    break;
	case BOL :  if (*i == 0)
			bump = 0;
		    break;
	case ANY :  if (c != '\n')
			bump = 1;
		    break;
	case EOL :  if (c == '\n')
			bump = 0;
		    break;
	case CCL :  if (locate(c, pat, j+1) == YES)
			bump = 1;
		    break;
	case NCCL:  if (c != '\n' && locate(c, pat, j+1) == NO)
			bump = 1;
		    break;
	default  :  fprintf(STDERR, "FND913 Error in omatch: can't happen.\n");
    }
    if (bump >= 0) {
	*i += bump;
	return(YES);
    }
    return(NO);
}

