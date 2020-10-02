/*
  HEADER: CUG	 nnn.nn;
  TITLE:	 LEX - A Lexical Analyser Generator
  VERSION:	 1.0 for IBM-PC
  DATE: 	 Jan 30, 1985
  DESCRIPTION:	 A Lexical Analyser Generator. From UNIX
  KEYWORDS:	 Lexical Analyser Generator YACC C PREP
  SYSTEM:	 IBM-PC and Compatiables
  FILENAME:      DFA.C
  WARNINGS:	 This program is not for the casual user. It will
		 be useful primarily to expert developers.
  CRC:		 N/A
  SEE-ALSO:	 YACC and PREP
  AUTHORS:	 Scott Guthery 11100 leafwood lane Austin, TX 78750
  COMPILERS:	 DESMET-C
  REFERENCES:	 UNIX Systems Manuals
*/
/*
 * Copyright (c) 1978 Charles H. Forsyth
 *
 * Modified 02-Dec-80 Bob Denny -- Conditionalize debug code for reduced size
 * Modified 29-May-81 Bob Denny -- Clean up overlay stuff for RSX.
 * More     19-Mar-82 Bob Denny -- New C library & compiler
 * More     03-May-82 Bob Denny -- Final touches, remove unreferenced autos
 * More     29-Aug-82 Bob Denny -- Clean up -d printouts
 * More     29-Aug-82 Bob Denny -- Reformat for readability and comment
 *                                 while learning about LEX.
 * More		20-Nov-83 Scott Guthery -- Adapt for IBM PC & DeSmet C
 *
 * Modified 22-Jun-86 Andrew Ward -- Modified code to compile under Lattice C 
 *	        		 version 3.0h.  Corrected several errors
 *				 from the assumption that pointers and
 *				 integers are the same size.	 
 */

/*
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::::::::             Start of Procedure: DFA                         :::::::::
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
*/

#include <stdio.h>
#include "lexlex.h"

#define PAGE(title)

#ifdef LATTICE
/* Lattice headers */
#include <assert.h>
#include <string.h>
#include <stdlib.h>
/* argument typed functions */
extern char        *lalloc(unsigned, unsigned, char *);
extern struct set  *newset(struct nfa **, int, int);
extern struct set  *eclosure(struct set *);          /* Used only by DFA */
extern struct dfa  *defalt(struct dfa *,struct xset **);/* Used only by DFA */
extern struct dfa  *newdfa();  /* --> current DFA state */
extern struct move *stbase(struct xset *);           /* Used only by DFA */
extern struct xset *addxset( int, struct xset *);
extern struct xset *addset(struct nfa *, struct xset *);
extern void        add(struct nfa **, struct nfa ***, struct nfa *);
extern void        error( char *, char * );
extern void        f_error( char *, char * );
extern void        setbase(struct dfa *, struct move *, struct xset *);

#else

/* Use if not Lattice C or not lint */
#define void int

extern struct set  *newset();
extern struct set  *eclosure();          /* Used only by DFA */
extern struct move *stbase();           /* Used only by DFA */
extern struct dfa  *newdfa();  /* --> current DFA state */
extern struct dfa  *defalt();            /* Used only by DFA */
extern struct xset *addxset();
extern struct xset *addset();
extern char        *lalloc();
extern void        add();
extern void        error();
extern void        f_error();  /* Fatal error */
extern void        setbase();
#endif



/*
 * Build the DFA from the NFA
 */
PAGE( DFABUILD )

void dfabuild()
{
    struct trans *trp;
    struct nfa **vec, **tp, *np, *temp[MAXNFA+1];
    int a, i;
    struct set *sp, *stack[MAXDFA], **spp, *xp, *tset;  /* DFA set stack */
    struct dfa *state;    /* --> current DFA state */
    struct xset *xs, *xse;

  /*
   * Simulate an epsilon transition from nfa state 0 to
   * the initial states of the machines for each
   * translation.
   */
    nfa[0].n_char = EPSILON;    /* Set NFA state 0 transition EPSILON */

    /*
    * Allocate a state vector, each node of which will
    * point to an NFA starting state.  Each translation
    * generates an NFA, so the number of translations
    * equals the number of NFA start states.
    */

    i = (transp - trans);  /* transp is updated in newtrans(); */
#ifdef DEBUG
    fprintf(stdout, "\nThe number of i states is: %d\nThe size is: %d\n", i, sizeof(struct nfa *) );
#endif
    i++;
    /* Allocate "vec" for i states */
    vec = (struct nfa **)lalloc( (unsigned)i, sizeof(struct nfa *), "dfabuild");

    /*
    * Fill in the start state vector
    */

    vec[0] = &nfa[0];               /* vec[0] always --> NFA state 0 */

    for (a = 1, trp = &trans[0]; trp < transp; trp++)  /* For each translation */
    {
       assert( isdata( (char *)trp->t_start, sizeof( struct nfa )));
       vec[a++] = trp->t_start;         /* Pick up the NFA start state */
    }
#ifdef DEBUG
    fprintf(stdout, "\nDFA: line 80" );
#endif

    /*
    * Now build the set sp --> e-CLOSURE(vec)
    *    Seperated newset & ecl nesting to isolated failure mode.
    */
    tset = newset( vec, i, 1 );

#ifdef DEBUG
    fprintf(stdout, "\nDFA: line 90 -- Prior to eclosure call" );
#endif

    sp = eclosure( tset );
#ifdef DEBUG
    assert( isdata( (char *)sp, sizeof(struct set) ) );
#endif

    /* Deallocate the start state vector */
    assert( isdptr( (char *)vec ) );
    if(free( (char *)vec) != 0)
    {
        f_error("\nDFA: Error is the release of v","");
    }

    /*
    * At this point state 0 of the DFA is constructed.
    * This is the start state of the DFA.
    * Mark it "added" and push it on to the stack.
    */
#ifdef DEBUG
    fprintf(stdout, "\nDFA: line 111 -- build the dfa stack" );
#endif
    sp->s_flag |= ADDED;
    spp = stack;
    *spp++ = sp;

    /*
    * From state 0, which is now stacked, all further DFA
    * states will be derived.
    */

    while(spp > stack)
    {
       sp = *--spp;
       for(a = 0; a < NCHARS; a++) insets[a] = '\0';

       xse = &sets[0];
       for (i = 0; i < sp->s_len; i++)
       {
#ifdef DEBUG
           fprintf( stdout,"\nADDSET LOOP: count = %d", i);
           fprintf( stdout,"\nADDSET LOOP: offset= %d", sp->s_els[i] - &nfa[0]);
           tmp11 = sp->s_els[i];
           if( tmp11 != NULL )
                assert( isdata( (char *)tmp11, sizeof( long ) ) );
#endif
           xse = addset(sp->s_els[i], xse);
       }

       sp->s_state = state = newdfa();
       state->df_name = sp;

#ifdef DEBUG
       fprintf( stdout, "\nbuild state %d ", state - dfa);
       fprintf( stdout, "\n" );
#endif

       state->df_ntrans = xse - sets;
       for(xs = sets; xs < xse; xs++)
       {
           a = xs->x_char & CMASK;
           tp = temp;
           for(i = 0; i < sp->s_len; i++)
           {
               if(
                   (np = sp->s_els[i])->n_char == a ||
                   np->n_char == CCL &&
                   ( np->n_ccl[a/NBPC] & ( 1 << (a%NBPC) ) )
                )
                   add( temp, &tp, np->n_succ[0] );
           }
#ifdef DEBUG
    fprintf(stdout, "\nDFA: line 137 -- Prior to NEWSET call" );
#endif

           xp = newset((struct nfa **)temp, (int)(tp- temp), 1 );

#ifdef DEBUG
    fprintf(stdout, "\nDFA: line 141 -- Post NEWSET call" );
    assert( isdata( (char *)xp, sizeof( struct set) ) );
#endif

           xp = eclosure(xp);

#ifdef DEBUG
           putc('\t',stdout);
           chprint( a );
           putc('\t', stdout);
           fprintf( stdout, "\n");
#endif

           xs->x_set = xp;
           if(xp->s_state == NULL && (xp->s_flag & ADDED)==0)
           {
               xp->s_flag |= ADDED;
               if(spp >= &stack[MAXDFA]) /* AMW: modified to use '&' */
               {
                   f_error("dfabuild: stack overflow","");
               }
               *spp++ = xp;
           }
       }

       /* the sub DEFALT() is in BASE.C */
#ifdef DEBUG
    fprintf(stdout, "\nDFA: line 165 -- prior to calling defalt, stbase, etc." );
#endif
       state->df_default = defalt(state, &xse);

       /* the following subs are in BASE.C */
       setbase(state, stbase(xse), xse);
    }
}

PAGE( ADD )
/*
 * If an nfa state is not already a member of the vector `base', add it.
 */

void add(base, tpp, np)
struct nfa **base, ***tpp, *np;
{
    struct nfa **tp;

    for(tp = base; tp < *tpp; tp++)
       if(*tp == np) return;
    *(*tpp)++ = np;
    return;
}

PAGE( ADDSET )
/*
 * Add the character(s) on which state `np' branches to the transition vector.
 */

struct xset *addset(np, xse)
struct nfa *np;
struct xset *xse;
{
    int a;
    char *ccl;
#ifdef DEBUG
    if( np !=NULL) assert( isdata( (char *)np, sizeof( struct nfa ) ) );
    if( xse != NULL) assert( isdata( (char *)xse, sizeof( struct xset ) ) );
#endif
    if( ( a = np->n_char ) < NCHARS )
       xse = addxset( a, xse );
#ifdef DEBUG
    assert( isdata( (char *)xse, sizeof( struct xset ) ) );
#endif
    if(a != CCL)   return( xse );
    ccl = np->n_ccl;
    for( a = 0; a < NCHARS; a++ )
       if( ccl[ a / NBPC ] & ( 1 << ( a%NBPC ) ) )
           xse = addxset( a, xse );
#ifdef DEBUG
    assert( isdata( (char *)xse, sizeof( struct xset ) ) );
#endif
    return( xse );
}

PAGE( ADDXSET )
/*
 * Add a character to the transition vector, if it isn't there already.
 */

struct xset *addxset(a, xse)
int a;
struct xset *xse;
{
    struct xset *xs;
    int temp;
    /*
   * VMS native doesn't do this correctly:
   *    if (insets[a]++)
   */
    if( a > NCHARS ) {
       f_error("\nADDXSET: Value for \"a\" exceeded NCHARS","");
    }

    temp = insets[a];
    insets[a] += 1;

    if(temp) return(xse);

    xs = xse;
#ifdef DEBUG
    assert( isdata( (char *)xs, sizeof( struct xset ) ) );
#endif
    xs->x_char = a;
    xs->x_set = (struct set *)NULL;
    xs->x_defsame = '\0';

    return(++xse);

}




