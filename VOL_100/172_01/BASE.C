/*
  HEADER:              CUG  nnn.nn;
  TITLE:               LEX - A Lexical Analyser Generator
  VERSION:             1.1 for IBM-PC
  DATE:                Jan 30, 1985
  DESCRIPTION:         A Lexical Analyser Generator. From UNIX
  KEYWORDS:            Lexical Analyser Generator YACC C PREP
  SYSTEM:              IBM-PC and Compatiables
  FILENAME:            BASE.C
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
 * More     20-Nov-83 Scott Guthery -- Adapt for IBM PC & DeSmet C
 *
 * Modified 22-Jun-86 Andrew Ward -- Modified code to compile under Lattice C
 *                                 version 3.0h.  Corrected several errors
 *                                 from the assumption that pointers and
 *                                 integers are the same size.     
 *                                 New debug code for LATTICE C using assert
 *                                 to test for wild pointers.
 */

/*
 * lex -- find and set base values for `move' vector
 */
#include <stdio.h>
#include "lexlex.h"

extern struct set *chase(struct dfa *, int);
extern int resolve(struct dfa *, struct dfa *, struct xset **);
extern struct move *stbase(struct xset *);
extern void setbase(struct dfa *, struct move *, struct xset *);

/*
 * Choose the best default state for `st'.
 * Only states previous to the current state are considered,
 * as these are guaranteed to exist.
 */

struct dfa *defalt(st, xsep)
struct dfa *st;
struct xset **xsep;
{
        struct   dfa *dp;
        int  minv, u; /* AMW: was unsigned */
        struct   dfa *def;
        struct   xset *xse;
        int i;

        xse = *xsep;
        i = xse - sets;
        if( i == 0 )   return(NULL);

#ifdef DEBUG
        fprintf(stdout, "State %d, Default: \n", st - dfa );
#endif
        minv = -1;
        def = NULL;
        for(dp = dfa; dp < st; dp++) {
                u = compat(st, dp, xse);
#ifdef DEBUG
        fprintf( stdout, "\nDEFAULT: In Loop 1 \n");
        fprintf( stdout, "\t%d rates %d\n", dp - dfa, u );
#endif
                if(u < minv) {
                        def = dp;
                        minv = u;
                }
        }
        if( minv == -1 || 10*(i-(int)minv) < i)
                def = NULL;
#ifdef DEBUG
        if(def != NULL) fprintf( stdout, "\t%d chosen\n", def - dfa);
        else fprintf( stdout, "\n def - dfa = %d, will resolve", def - dfa );
#endif

        if(def)
                resolve(st, def, xsep);
        return(def);
}

/*
 * State `b' is compatible with, and hence a suitable default state
 * for state `a', if its transitions agree with
 * those of `a', except those for which `a' has transitions to the
 * (alleged) default. Circularity of the default
 * relation is also not allowed. If the state `b' has at least
 * twice as many transitions as `a', it is not even worth considering.
 */

int compat(a, b, xse)
struct dfa *a, *b;
struct xset *xse;
{
        struct dfa *dp;
        struct xset *xs;
        int nt;

        if(a==b || ( b->df_ntrans >= a->df_ntrans*2 ) )  return(-1);

        for(dp = b; dp; dp = dp->df_default)
                if (dp == a) return(-1);

        nt = b->df_ntrans + a->df_ntrans;

        for(xs = sets; xs < xse; xs++)
                if(chase(b, xs->x_char) == xs->x_set)  nt -= 2;

#ifdef DEBUG
        fprintf( stdout, "\nCOMPAT: nt = %d", nt);
#endif

        return( nt );
}

struct set *chase(st, c)
struct dfa *st;
int c;
{
        struct move *dp;

        c &= CMASK;
        while((dp = st->df_base) != NULL && ((dp += c) >= st->df_max
             || dp->m_check != st) )
                if ((st = st->df_default) == NULL) return((struct set *)NULL);

        if(dp != NULL) return( dp->m_next );
        else           return((struct set *)NULL);
}

/*
 * set `sets' to indicate those characters on which the state `a'
 * and its default agree and those characters on which `a'
 * should go to `error' (as the default accepts it, but `a' does not).
 */
int resolve(a, def, xsep)
struct dfa *a, *def;
struct xset **xsep;
{
        struct move *dp;
        int c, i;
        struct xset *xs, *xse;

        a = a;                          /* Quiet compiler the easy way */

        xse = *xsep;
        i = xse - sets;
        for(xs = sets; xs < xse; xs++) xs->x_defsame = '\0';
        for(; def; def = def->df_default)
        for(dp = def->df_base; dp < def->df_max; dp++)
                if(dp->m_check == def) {
                        c = dp - def->df_base;
                        for (xs = sets; xs < xse; xs++)
                                if (c==(xs->x_char&CMASK)) {
                                        if (xs->x_set==dp->m_next) {
                                                xs->x_defsame++;
                                                i--;
                                        }
                                        break;
                                }
                        if(xs >= xse) {
                                xs->x_defsame = '\0';
                                xs->x_char = c;
                                xs->x_set = (struct set *)NULL;
                                i++;
                                xse++;
                        }
                }
        *xsep = xse;
        return(i);
}

/*
 * Choose a base in `move' for the current state.
 * The transitions of that state are in the vector `sets'.
 */
struct move *stbase(xse)
struct xset *xse;
{
        int  a;
        struct move *base;
        int conflicts;
        struct xset *xs;

        if( xse == sets)
                return(NULL);
        base = move;
        do{
                if(base - move >= NNEXT)
                {
                        error("No space in `move' (stbase)","");
                        exit(1);
                }
                conflicts = 0;
                for(xs = &sets[0]; xs < xse; xs++) {
                        a = xs->x_char & CMASK;
                        if(xs->x_defsame == 0 &&
                           ( &base[a] >= &move[NNEXT] || /* AMW: modified */
                             base[a].m_check != (struct dfa*)NULL)
                           )
                             {
                                conflicts++;
                                base++;
                                break;
                             }
                }
        } while(conflicts);
        return(base);
}

/*
 * Given a state, its `base' value in `move',
 * and the set of transitions in `sets' (ending near `xse'),
 * set the `move' values.
 */
void setbase(st, base, xse)
struct dfa *st;
struct move *base;
struct xset *xse;
{
        struct move *dp;
        struct xset *xs;
        struct move *maxdp;

        st->df_base = base;
        st->df_max = base;
#ifdef DEBUG
    fprintf( stdout, "\nSetbase: state %d\n", st - dfa );
    if( base == NULL ) fprintf( stdout, "\tno base\n");
#endif
        if(base == NULL) return;
        maxdp = base;
        for(xs = sets; xs < xse; xs++)
                if(xs->x_defsame == '\0' ) {
                /* Base is a pointer into the move[] array */
                        dp = base + (int)xs->x_char ;
                        if(dp > maxdp) maxdp = dp;
                        dp->m_next = xs->x_set;
                        dp->m_check = st;
                        if( (dp - move) > llnxtmax) llnxtmax = dp - move;
#ifdef UDEBUG
       /* This set of statements was causing the program to abort */
                        fprintf(stdout, "\t%c nets %d\n", xs->x_char&CMASK, dp - move );
#endif
                }

        st->df_max = maxdp+1;
}



