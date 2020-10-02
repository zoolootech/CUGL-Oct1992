/*
  HEADER:              CUG  nnn.nn;
  TITLE:               LEX - A Lexical Analyser Generator
  VERSION:             1.1 for IBM-PC
  DATE:                Jan 30, 1985
  DESCRIPTION:         A Lexical Analyser Generator. From UNIX
  KEYWORDS:            Lexical Analyser Generator YACC C PREP
  SYSTEM:              IBM-PC and Compatiables
  FILENAME:            LEXLEX.H
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
 * Modified 02-Dec-80 Bob Denny -- Conditionalize debug code for reduced size
 * Modified 29-May-81 Bob Denny -- Clean up overlay stuff for RSX.
 * More     19-Mar-82 Bob Denny -- New C library & compiler
 * More     03-May-82 Bob Denny -- Final touches, remove unreferenced autos
 * More     29-Aug-82 Bob Denny -- Clean up -d printouts
 * More     29-Aug-82 Bob Denny -- Reformat for readability and comment
 *                                 while learning about LEX.
 * More     20-Nov-83 Scott Guthery -- Adapt for IBM PC & DeSmet C
 *
 * Modified 22-Jun-86 Andrew Ward -- Modified code to compile under Lattice C
 *                                 version 3.0h.  Corrected several errors
 *                                 from the assumption that pointers and
 *                                 integers are the same size.
 *                                 New debug code for LATTICE C using assert
 *                                 to test for wild pointers.
 */

/* #define DEBUG */
#define LATTICE
/*
 * Original allocations.
 */
#define NCHARS  0400    /* Size of character set */
#define NCPW    2       /* # characters per word was 2*/
#define NBPC    8       /* # bits per character */
#define NBPW    (NCPW*NBPC)     /* # bits per word */

#define MAXNFA  600     /* Number of NFA states */
#define MAXDFA  800     /* Number of DFA states */
#define NTRANS  128     /* Number of translations */
#define NCCLS    50     /* Number of character classes */
#define NNEXT  2400     /* Size of dfa move vectors (later: allocate) */
#define CMASK  '\377'    /* Character mask */
/*
 * Special node characters.
 */
#define CCL     NCHARS          /* One of a character class */
#define EPSILON NCHARS+1        /* Transition on epsilon */
#define FIN     NCHARS+2        /* Final state; NFA */

/*
 * Set of state numbers (dfa state name).
 */
struct  set {
        struct  set     *s_next;
        struct  dfa     *s_state;       /* pointer into dfa array  */
        struct  set     *s_group;       /* pointer to owning group (dfamin) */
        int     s_final;                /* nf state which matches  */
        int     s_flag;                 /* see below */
        int     s_look;                 /* look-ahead bits */
        int     s_len;                  /* number of elements in set */
        struct nfa *s_els[1];
};

/*
 * State entry
 */
struct  nfa {
        int     n_char;
        char    *n_ccl;
        int     n_flag;
        int     n_look;         /* lookahead index */
        struct  nfa     *n_succ[2];
        struct  trans   *n_trans;
};

/*
 * DFA transition entry.
 */
struct  move {
        struct  set     *m_next;
        struct  dfa     *m_check;
};

/*
 * Structure of DFA vector.
 */
struct  dfa {
        struct  set     *df_name;
        struct  move    *df_base;
        struct  move    *df_max;
        struct  dfa     *df_default;
        int     df_ntrans;
};

/*
 * s_flag values for DFA node
 */

#define LOOK    01      /* Lookahead mark */
#define ADDED   02      /* DFA construction mark */
#define FLOOK   04      /* Mark on final state of lookahead translation */

/*
 * Flag used to print node
 */
#define NPRT    010     /* NFA node printed */

/*
 * Transition set.
 */
struct  xset {
        struct  set     *x_set;
        int     x_char;
        int     x_defsame;
};

/*
 * Translations
 */
struct  trans {
	struct  nfa     *t_start;
	struct  nfa     *t_final;
};

/*
 * External definitions.
 */
extern  struct  trans   trans[NTRANS];
extern  struct  trans   *transp;
extern  struct  nfa     nfa[MAXNFA];
extern  struct  nfa     *nfap;
extern  struct  dfa     dfa[MAXDFA];
extern  int     ndfa;
extern  struct  move    move[NNEXT]; /* AMW: was move[]; */
extern  struct  xset    sets[NCHARS]; /* AMW: was sets[]; */
extern  char    insets[NCHARS];
extern  struct  set     *setlist;
extern  char    ccls[NCCLS][(NCHARS+1)/NBPC];
extern  char    *newccl(char *);
extern  struct  nfa *newnfa(int, struct nfa *, struct nfa *);
extern  struct  trans *newtrans(struct nfa *, struct nfa *);
extern  int     nccls;
extern  int     llnxtmax;
extern  char    *tabname;
extern  FILE    *llout;
extern  FILE    *lexin;
extern  int     aflag;
extern  int     sflag;
extern  char    infile[]; /* AMW: Was *infile */
extern  FILE    *lexlog;
extern  int     lldebug;
extern  int     mflag;

xtern  char    infile[]; /* AMW: Was *infile */
extern  FILE