/*
  HEADER:              CUG  nnn.nn;
  TITLE:               LEX - A Lexical Analyser Generator
  VERSION:             1.1 for IBM-PC
  DATE:                Jan 30, 1985
  DESCRIPTION:         A Lexical Analyser Generator. From UNIX
  KEYWORDS:            Lexical Analyser Generator YACC C PREP
  SYSTEM:              IBM-PC and Compatiables
  FILENAME:            MIN.C
  WARNINGS:            This program is not for the casual user. It will
                       be useful primarily to expert developers.
  CRC:                 N/A
  SEE-ALSO:            YACC and PREP
  AUTHORS:             Charles H. Forsyth
                       Scott Guthery 11100 leafwood lane Austin, TX 78750
                       Andrew M. Ward, Jr.  Houston, Texas (Modifications)
  COMPILERS:           LATTICE C
  REFERENCES:          UNIX Systems Manuals -- Lex Manual on distribution disks
*/
/*
 * Copyright (c) 1978 Charles H. Forsyth
 *
 */

/*
 * lex -- dfa minimisation routines
 */

#include <stdio.h>
#include "lexlex.h"

#ifdef  MIN
#else
/*
 * Dummy routine
 */
dfamin()
{
}
#endif

#ifdef  MIN

member(e, v, i)
int e, *v, i;
{

	while (i--)
		if (e==*v++)
			return(1);
	return(0);
}

extern struct set **oldpart;
extern int **newpart;
extern int nold, nnew;

struct  xlist {
	struct  set     *x_set;
	struct  trans   *x_trans;
	      };

xcomp(a, b)
struct xlist *a, *b;
{
	if (a->x_trans > b->x_trans)
		return(1);
	if (a->x_trans==b->x_trans)
		return(0);
	return(-1);
}

dfamin()
{
	struct xlist *temp, *tp, *xp, *zp;
	struct trans *trp;
	int *tp2, *ip;

	struct set *gp, **xch;
	int i, j, k, niter;

	if(mflag == 0) return;          /*** NOTE ***/

	temp = lalloc(ndfa, sizeof(*temp), "minimisation");
	oldpart = lalloc(ndfa, sizeof(*oldpart), "minimisation");
	newpart = lalloc(ndfa*2+1, sizeof(*newpart), "minimisation");
	setlist = 0;
/*
 * partition first into final
 * states which identify different
 * translations, and non-final
 * states.
 */
	tp = temp;
	for (i = 0; i < ndfa; i++, tp++) {
		tp->x_set = dfa[i].df_name;
		if (tp->x_set->s_final)
			tp->x_trans = nfa[tp->x_set->s_final].n_trans;
		else
			tp->x_trans = 0;
	}
	qsort(temp, tp-temp, sizeof(*tp), xcomp);
	for (xp = temp; xp < tp; xp = zp) {
		ip = newpart;
		for (zp = xp; zp < tp && zp->x_trans==xp->x_trans; zp++)
			*ip++ = zp->x_set->s_state-d&a;
	}
}

q_tex(i, j, k)
char *i, *j, *k;
{
	char *ri, *rj, *rk;
	int c;
	int n;

	n = size;
	ri = i;
	rj = j;
	rk = k;
	do {
		c = *ri;
		*ri++ = *rk;
		*rk++ = *rj;
		*rj++ = c;
	} while(--n);
}
#endif
