/*
  HEADER:              CUG  nnn.nn;
  TITLE:               LEX - A Lexical Analyser Generator
  VERSION:             1.1 for IBM-PC
  DATE:                Jan 30, 1985
  DESCRIPTION:         A Lexical Analyser Generator. From UNIX
  KEYWORDS:            Lexical Analyser Generator YACC C PREP
  SYSTEM:              IBM-PC and Compatiables
  FILENAME:            ECLOSU.C
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
 * Andrew M. Ward 22 Jun 86 -- Added type cast and argumnet types
 *			       to newset declaration.	 
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "lexlex.h"


/*
 * Construct the epsilon closure of a given set; this is the set of states
 * that may be reached by some number of epsilon transitions from that state.
 */

extern struct set *newset(struct nfa **, int, int);

struct set *eclosure(t)
struct set *t;
{
        struct nfa *np, *xp;
        int i;
        struct set *tt; /* Return value */
        struct nfa **sp, **tp, **ip, *stack[MAXNFA], *temp[MAXNFA];

#ifdef DEBUG
       /* Test is points to valid region of memory */
       assert( isdata( (char *)t, sizeof( struct set ) ) );
#endif
        tp = &temp[0];

        for(sp = &stack[0], i = 0; i < t->s_len; i++)
        {
             if( sp <= &stack[ MAXNFA ] )  *tp++ = *sp++ = t->s_els[i];
             else {
                    f_error("Stack overflow in `eclosure'","");
             }
        }
        while(sp > stack) {
                np = *(--sp);
                if (np->n_char == EPSILON)
                for(i = 0; i < 2; i++)
                        if(xp = np->n_succ[i]) {
                                for(ip = &temp[0]; ip < tp;)
                                        if(*ip++ == xp)
                                                goto cont;
                                if(tp>= &temp[MAXNFA]) {
                                        f_error("eclosure: list overflow","");
                                }
                                *sp++ = *tp++ = xp;
                        cont:;
                        }
        }

        /* 'tt' is a pointer to allocated storage */
        tt = newset(temp, (int)(tp - temp), 1);
        return((struct set *)tt);
}
