/* $Header: /nw2/tony/src/stevie/src/RCS/linefunc.c,v 1.2 89/03/11 22:42:32 tony Exp $
 *
 * Basic line-oriented motions.
 */

#include "stevie.h"

/*
 * nextline(curr)
 *
 * Return a pointer to the beginning of the next line after the one
 * referenced by 'curr'. Return NULL if there is no next line (at EOF).
 */

LPTR *
nextline(curr)
LPTR	*curr;
{
	static	LPTR	next;

	if (curr->linep->next != Fileend->linep) {
		next.index = 0;
		next.linep = curr->linep->next;
		return &next;
	}
	return (LPTR *) NULL;
}

/*
 * prevline(curr)
 *
 * Return a pointer to the beginning of the line before the one
 * referenced by 'curr'. Return NULL if there is no prior line.
 */

LPTR *
prevline(curr)
LPTR	*curr;
{
	static	LPTR	prev;

	if (curr->linep->prev != Filetop->linep) {
		prev.index = 0;
		prev.linep = curr->linep->prev;
		return &prev;
	}
	return (LPTR *) NULL;
}

/*
 * coladvance(p,col)
 *
 * Try to advance to the specified column, starting at p.
 */

LPTR *
coladvance(p, col)
LPTR	*p;
register int	col;
{
	static	LPTR	lp;
	register int	c, in;

	lp.linep = p->linep;
	lp.index = p->index;

	/* If we're on a blank ('\n' only) line, we can't do anything */
	if (lp.linep->s[lp.index] == '\0')
		return &lp;
	/* try to advance to the specified column */
	for ( c=0; col-- > 0; c++ ) {
		/* Count a tab for what it's worth (if list mode not on) */
		if ( gchar(&lp) == TAB && !P(P_LS) ) {
			in = ((P(P_TS)-1) - c%P(P_TS));
			col -= in;
			c += in;
		}
		/* Don't go past the end of */
		/* the file or the line. */
		if (inc(&lp)) {
			dec(&lp);
			break;
		}
	}
	return &lp;
}
