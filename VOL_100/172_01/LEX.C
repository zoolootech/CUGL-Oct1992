/*
  HEADER: CUG	 nnn.nn;
  TITLE:	 LEX - A Lexical Analyser Generator
  VERSION:	 1.0 for IBM-PC
  DATE: 	 Jan 30, 1985
  DESCRIPTION:	 A Lexical Analyser Generator. From UNIX
  KEYWORDS:	 Lexical Analyser Generator YACC C PREP
  SYSTEM:	 IBM-PC and Compatiables
  FILENAME:      LEX.C
  WARNINGS:	 This program is not for the casual user. It will
		 be useful primarily to expert developers.
  CRC:		 N/A
  SEE-ALSO:	 YACC and PREP
  AUTHORS:	 Scott Guthery 11100 leafwood lane Austin, TX 78750
  COMPILERS:	 LATTICE C 3.0h
  REFERENCES:	 UNIX Systems Manuals
*/

/*
 * Copyright (c) 1978 Charles H. Forsyth
 */

/*
 * lex -- initialisation, allocation, set creation
 *
 *     Revised for PDP-11 (Decus) C by Martin Minow
 */

/* Modified 02-Dec-80 Bob Denny -- Conditionalized debug code for smaller size
 *                           01 -- Moved calls to dfa build, min, print, write
 *                                  and to stat, and code for ending() into
 *                                  this module so that 'ytab' could be put
 *                                  into overlay region.
 *          29-May-81 Bob Denny -- More extern hacking for RSX overlaying.
 * More     19-Mar-82 Bob Denny -- New C library & compiler
 * More     03-May-82 Bob Denny -- Final touches, remove unreferenced autos
 *          28-Aug-82 Bob Denny -- Add "-s" switch to supress references to
 *                                  "stdio.h" in generated code.  Add switch
 *                                  comments in code. Add -e for "easy" com-
 *                                  mand line. "lex -e file" is the short way
 *                                  of saying:
 *                                      "lex -i file.lxi -o file.c -t file"
 * More(!)  30-Oct-82 Bob Denny -- Fix RSX ODL to put lots of FCS junk into
 *                                  overlay, pick up (badly needed) 3KW for
 *                                  NFA nodes, etc.  Change static allocations
 *                                  in LEXLEX.H for RSX so can do non-trivial
 *                                  things.  Task is now big on RSX and grows
 *                                  from big to huge as it runs.
 *                                 Fix "-s" support so it is again possible
 *                                  to do a lexswitch() (dumb!).
 *          14-Apr-83 Bob Denny    VAX-11 C workarounds.
 *                                 Fix definition of toupper().
 *	    20-Nov-83 Scott Guthery  Adapt for IBM PC & DeSmet C
 *          22-Jun-86 Andrew Ward  Adapted for Lattice C ver 3.0h.  Debug and 
 *			 	   Error detection code installed as aid in 
 *			           detection of wild pointers which result 
 *                                 from interchanging pointers and integers. 
 *   			           The interchangable use of pointers and 
 *                                 integers was eliminated. 
 *
 *                                 NOTICE: some Lattice 3.0 functions were used *				   However, these should be easy to recode.  
 *				   Non-ANSII functions will be removed latter
 *          
 */

#ifdef  DOCUMENTATION

title   lex     A Lexical Analyser Generator
index           A Lexical Analyser Generator

synopsis

        lex [-options] [-i grammar] [-o outfile] [-t table]

description

        Lex compiles a lexical analyser from a grammar and description of
        actions.  It is described more fully in lex.doc: only usage is
        described.  The following options are available:
        .lm +16
        .s.i-16;-a              Disable recognition of non-ASCII characters
        (codes > 177 octal) for exception character classes (form [^ ...]).
        .s.i-16;-d              Enable debugging code within lex.  Normally
        needed only for debugging lex.
        .s.i-16;-e              "Easy" command line. Saying "lex#-e#name" is the
        same as saying:
        .s.i 4;"lex -i name.lxi -o name.c -t name"
        .s
        Do not include devices or an extension on "name" or make it longer
        than 8 characters, or you'll get several error messages.
        .s.i-16;-i file         Read the grammar from the file.  If "-i" is not
        specified, input will be read from the standard input.
        .s.i-16;-m              Enable state minimization. Currently not

        implemented, switch is a no-op.
        .s.i-16;-o file         Write the output to the file.  If "-o" is not
        specified, output will be written to file "lextab.c".
        .s.i-16;-s              "Stand-alone" switch.  Supresses the line
        "#include <stdio.h>" normally generated in the lex output.  Use this
        if LEX is generating a module to be used in a program which does not
        use the "standard I/O" package.
        .s.i-16;-t table        Name the recognizer "table" instead of the
        default "lextab".  If -o is not given, output will be written to file
        "table.c".
        .s.i-16;-v [file]       Verify -- write internal tables to the
        indicated file.  If "-v" is given without a file name argument,
        tables will be written to "lex.out".
        .lm -16

diagnostics

        The following error messages may occur on invocation.  See lex
        documentation for information on compilation errors.
        .lm +8
        .s.i -8;Can't create ...
        .s.i -8;Cannot open ...
        .s.i -8;Illegal option.
        .s.i -8;Illegal switch combination.
        .s
        "-i", "-o" or "-t" given with "-e" or vice-versa
        .s.i -8;Table name too long.
        .s
        The table name (argument to "-t") must not be longer than 8 bytes.
        .s.i -8;Missing table name.
        .s.i -8;Missing input file.
        .s.i -8;Missing output file.
        .s.i -8;Missing name.
        .lm -8

author
        Charles Forsyth
        Modified by Martin Minnow, Bob Denny & Scott Guthery
        Modified by Andrew M. Ward, Jr.
bugs


#endif

/*
 * LEX -- Lexical scanner acquired from C-users group.
 *      In original form this program was an example of what good C
 *      programing is NOT.  I have tried to eliminate machine dependancies
 *      where ever possible.  In particular, the interchangable use of
 *      pointers and ints has been eliminated.  External fuctions are declared
 *         with argument typed.
 *  1986
 * 02 June AMW: installed Lattice 3.0 functions in place of some combersom routines
 *         used by original code.
 * 04 June routine lexsort: qsort replaced with call to Lattice function of same name.
 *         Result: LEX produces output with proper appearance, but does not
 *         compile correctly.  Dangaling pointer is cause.
 * 22 June My substitution code for NEWCCL was faulty.  Original code replaced
 *         and system behaved properly.  Further, using C-terp it was determined
 *         that the routine lexswitch returned the wrong value, this was corrected.
 * 23 June modifications made to DEBUG segments.  Program LEX produces compilable
 *         output that functions for word.lxi -> word.c as intended.  hword.lxi
 *         does not process to completion.  Symptom: lex hangs-up while displaying
 *         XX nets NN.  The offending code is the DEBUG segment and the surrounding
 *         loop.  Assert lines were installed but detected no errors.
 * 24 June Newset modified to use sqsort() when routine called for DFA request.
 *         This modification may improve execution speed.
 * 25 June The program now compiles and produces functional code for hword.lxi
 *         and word.lxi.  Both programs fuction correctly.  Further, I uncovered
 *         an error in the debug code I installed.  It appears that certain control
 *         codes disable ANSI.SYS and the display can not be restarted.  The code
 *         is in BASE.C and is identified by the UDEBUG segment.
 *
 */
#include <stdio.h>
#include "lexlex.h"  

#ifdef LATTICE
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>

/* Set memory size for procedure.  Lattice specific */
int  _STACK = 32000;
long _MNEED = 45000;

extern        char  tolower();
extern        char  *calloc();

/* Lex functions */
extern        char  *lalloc(unsigned, unsigned, char *);
extern        char  *newccl(char *);			 
extern        void  heading(void);			 
extern        void  dfabuild(void);			 
extern        void  setline(void);			 
extern        void  dfawrite( void );			 
extern        void  error( char *, char *);	 	 
extern        void  f_error( char *, char *);  /* Fatal error */
extern        int   setcomp( struct nfa **, struct nfa **);
extern struct set   *newset( struct nfa **, int, int);	 
extern struct trans *newtrans(struct nfa *, struct nfa *);
extern struct dfa   *newdfa( void );
extern struct nfa   *newnfa(int, struct nfa *, struct nfa *);
extern        int   eqvec( struct nfa **, struct nfa **, int );

#else
/* Function types of arguments are removed, otherwise identical to Lattice */
/* section */
extern         char  tolower();
extern         char  *lalloc();
extern         char  *calloc();
extern         char  *newccl();
extern         void  heading();
extern         void  dfabuild();
extern         void  setline();
extern         void  dfawrite();
extern         void  error();
extern         void  f_error();
extern         int   setcomp();
extern struct  set   *newset();
extern struct  trans *newtrans();
extern struct  dfa   *newdfa();
extern struct  nfa   *newnfa();
extern         int   eqvec();

#endif

/* includes system configuration constants */

extern 	       char  ccls[NCCLS][(NCHARS+1)/NBPC];

struct  xset  sets[NCHARS];

char    insets[NCHARS];

struct  trans   trans[NTRANS];
struct  trans   *transp = &trans[0];

FILE    *llout;
FILE    *lexin;
FILE    *lexlog;


char    infile[FMSIZE]  = "";
char    outfile[FMSIZE] = "";
char    *tabname = "lextab  ";
char    tabfile[FMSIZE];
char    *progname;

struct  dfa     dfa[MAXDFA];
struct  move    move[NNEXT];

struct  nfa      nfa[MAXNFA];
struct  nfa     *nfap = &nfa[1]; /* &nfa[1]; */
int     ndfa = 0;


char    ccls[NCCLS][(NCHARS+1)/NBPC];
int     nccls = 0;
int     llnxtmax = 0;
int     yyline;
char    llbuf[100];
char    *llend, *llp2;



/*
 * Flags.  Allow globals only for those requiring same. Some only
 * used for checking for bad combos. this
 */
        int     aflag = 0;      /* Ignore non-ASCII in [^ ...] */
static  int     eflag = 0;      /* Easy command line */
static  int     iflag = 0;      /* "-i" given */
        int     mflag = 0;      /* Enable state minimization (not imp.) */
static  int     oflag = 0;      /* "-o" given */
        int     sflag = 0;      /* Supress "#include <stdio.h>" in output */
static  int     tflag = 0;      /* "-t" given */

struct  set     *setlist = NULL;


void main(argc, argv)
int argc;
char *argv[];
{
#ifdef LATTICE
	progname = argv[0]; /* Who we are */
#endif
       for (; argc>1 && *argv[1]=='-'; argv++, argc--)
       switch (tolower(argv[1][1])) {

       /*
        * Enable state minimization. Currently not implemented.
         */
       case 'm':
               mflag++;
               break;

       /*
        * Disable matching of non-ASCII characters (codes > 177(8))
        * for exception character classes (form "[^ ...]").
        */
       case 'a':
               aflag++;
               break;

       /*
        * Supress "#include <stdio.h>" in generated
        * code for programs not using standard I/O.
        */
       case 's':
               sflag++;
               break;

       /*
        * "Easy" command line
        */
       case 'e':
               if(iflag || oflag || tflag) {
                       f_error("Illegal switch combination\n","");
               }
               if(--argc <= 1) {
                       f_error("Missing name\n","");
               }
               if(strlen(tabname = (++argv)[1]) > 8) {
                      f_error("Name too long\n","");
               }
               strmfe(infile, tabname, "lxi");
               printf("Input read from %s\n", infile);
               if( ( lexin = fopen( infile, "r" ) ) == NULL ) {
                       f_error( "Cannot open input \"%s\"\n", infile );
               }
               strmfe( outfile, tabname, "c" );
               break;

       /*
        * Specify input file name.
        */
       case 'i':
               if( eflag ) {
                      f_error("Illegal switch combination\n","");
               }
	       
               iflag++;
               if( --argc <= 1 ) {
                       f_error( "Missing input file\n","" );
               }
	       
               strcpy(infile, (++argv)[1]);
               printf("Input read from %s\n", infile);
               if((lexin = fopen(infile, "r")) == NULL) {
                       f_error("Cannot open input \"%s\"\n", infile);
               }
	       
               strcpy( outfile, "lextab.c");
               break;

       /*
        * Specify output file name. Default = "lextab.c"
        */
       case 'o':
              if(eflag) {
                       f_error("Illegal switch combination\n","");
               }
               oflag++;
               if(--argc <= 1) {
                       f_error("Missing output file","");
               }
               strcpy(outfile,(++argv)[1]);
               break;

       /*
        * Specify table name.  Default = "lextab.c".  If "-o"
        * not given, output will go to "tabname.c".
        */
       case 't':
               if(eflag) {
                       f_error("Illegal switch combination\n","");
               }
              tflag++;
               if(--argc <= 1) {
                       f_error("Missing table name","");
               }
               if(strlen(tabname = (++argv)[1]) > 8) {
                       f_error("Table name too long\n","");
               }
               break;

       default:
               f_error("Illegal option: %s\n", argv[1]);
       }

       if(!strcmp(infile,"") ) strcpy(infile, "lex.lxi");
       (void)strlwr( infile );
       tabname = strlwr( tabname );
       if( !strcmp(outfile, "") ) {
               strmfe( tabfile, tabname, "c");
               strcpy( outfile, tabfile );
       }
       printf("Analyzer written to %s\n", outfile);
       if((llout = fopen(outfile, "w"))==NULL) {
               f_error("Can't create %s\n", outfile);
       }

       /* Now that all the options are set the real work begins */
       heading();
       fprintf(stderr, "Parse LEX source ...\n");
       if(yyparse()) error("Parse failed\n","");
       fprintf(stderr, "Build NFA then DFA ...\n");
       dfabuild();                                             /* 01+ */
       fprintf(stderr, "Minimize DFA ...\n");
       dfamin();
       fprintf(stderr, "Create C source ...\n");
       dfaprint();
       dfawrite();
       fprintf(stderr, "\07LEX done.\n");
       fclose(llout);
       exit(0);
} /** END OF MAIN **/

void ending()
{
       static int ended;

        if(ended++) return;
       fprintf(llout, "\t}\n\treturn(LEXSKIP);\n}\n");
       setline();
}


/*
 * The following functions simply
 * allocate various kinds of
 * structures.
 */

struct nfa *newnfa(ch, nf1, nf2)
int  ch;
struct nfa *nf1, *nf2;
{
       struct nfa *nf;
       extern struct nfa *nfap;
       /* Pull an NFA slot off the nfa stack and test */
       if ((nf = nfap++) >= &nfa[MAXNFA]) {
               f_error("Too many NFA states","");
       }
        nf->n_char = ch;
#ifdef DEBUG
        if( nf1 !=NULL) assert( isdata( (char *)nf1, sizeof( struct nfa) ) );
        if( nf2 !=NULL) assert( isdata( (char *)nf2, sizeof( struct nfa) ) );
#endif
        nf->n_succ[0] = nf1;
        nf->n_succ[1] = nf2;
        nf->n_trans = (struct trans *)NULL;
        nf->n_flag = '\0';
        nf->n_look = '\0';
        return(nf);
}

struct dfa *newdfa()
{
        struct dfa *df;
        /* Pull a DFA slot of the dfa stack and test */
        if ((df = &dfa[ndfa++]) >= &dfa[MAXDFA]) {
               f_error("Out of dfa states","");
        }
#ifdef DEBUG
        fprintf( stdout,"\nCurrent DFA count is: %d", ndfa);
#endif
        return(df);
}

char *newccl(ccl)
char *ccl;
{
        int j;
        int i;
        char *p, *q;
        extern int sz_ccl;
        extern int nccls;
        for (j = 0; j < nccls; j++)
        {
            p = ccl;
            q = ccls[j];
            for( i = sizeof( ccls[j]); i--; )
              if(*p++ != *q++) goto cont;
            return( ccls[j] );
cont:       ;
        }
        if (nccls >= NCCLS) {
               f_error("Too many character classes","");
        }
        /* the character block 'ccl' is fixed length */
        /* and may have intervening '\0'             */
        /* return( memcpy(ccls[nccls++], ccl, sz_ccl ) ); */
        p = ccl;
        q = ccls[ j = nccls++];
        for( i = sizeof(ccls[j]); i--; )
           *q++ = *p++;
        return( ccls[j] );
}

struct trans *newtrans(st, en)
struct nfa *st, *en;
{
       struct trans *tp;
       extern struct trans *transp;
       if((tp = transp++) >= &trans[NTRANS]) {
               f_error("Too many translations","");
       }
#ifdef DEBUG
	/* Test for valid data */
	if( st !=NULL) assert( isdata( (char *)st, sizeof( struct nfa) ) );
	if( en !=NULL) assert( isdata( (char *)en, sizeof( struct nfa) ) );
#endif
       tp->t_start = st;
       tp->t_final = en;
       en->n_trans = tp;
       return( (struct trans *)tp);
}

/*
 * Create a new set. `sf', if set, indicates that the elements of the
 * set are states of an NFA). If `sf' is not set, the elements are state
 * numbers of a DFA.
 */
struct set *newset(v, i, sf)
struct nfa **v;
int i;
int sf;
{
       struct set *t;
       long k;
       int kk;
       extern int setcomp();

       if( i != 0 && sf ) {
        qsort((char *)v, i, 4 /*sizeof(struct nfa *)*/, setcomp);
       }
       else
       {
       /* AMW: was (char *) 26 July 1986 */
        sqsort((short *)v, i );
       }
#ifdef DEBUG
       if(setlist !=NULL) assert( isdptr( (char *)setlist ) );
       assert( i >= 0 );
#endif
       for (t = setlist; t != NULL; t = t->s_next)
              if (t->s_len==i && eqvec(t->s_els, v, i)) return(t);
       t = (struct set *)lalloc(1, (unsigned)( sizeof(struct set)+i*sizeof(struct nfa *) ), "set nodes");
       t->s_next = setlist;

       setlist = t;
       t->s_final = 0;
       t->s_state =  (struct dfa *)NULL;
       t->s_flag = '\0';
       t->s_len = i;
       t->s_group = (struct set *)NULL;
       t->s_look = 0;
       for (v += i; i;) {
               --v;
              if (sf) {
                       if ((*v)->n_char==FIN)
                       {
                         kk = (*v) - nfa;
                         t->s_final = kk;
                       }
                      if( (*v)->n_flag & LOOK )
                               t->s_look |= 1 << ((*v)->n_look);
               } else {
                      error("\nNEWSET: called for a DFA request","");
                      k = (long)*v;  /* AMW: try to clear warning on invalid conversion */
                      printf("\nLEX (NEWSET) k = %d", k);
                       dfa[k].df_name->s_group = t;
               }
#ifdef DEBUG
               assert( isdata( (char *)v, sizeof( struct nfa ) ) );
#endif
               t->s_els[--i] = *v; /* 'i' must be greater than 0 */
       }
       return(t);
}

/* Compare different sets */
int setcomp(n1p, n2p)
struct nfa **n1p, **n2p;
{
       struct nfa *n1, *n2;

       n1 = *n1p;
       n2 = *n2p;
       if( n1 > n2 )
               return(1);
       if( n1 == n2 )
              return(0);
       return(-1);
}

int eqvec(a, b, i)
struct nfa **a, **b;    /* long *a;long *b;*/
int   i;
{
       if( i )
               do{
                       if( *a++ != *b++ )
                               return(0);
               } while(--i);
       return(1);
}


/*
 * Ask for core, and complain if there is no more.
 */
char *lalloc(n, s, w)
unsigned n, s;
char *w;
{
       char *cp;
       if ((cp = calloc(n, s)) == NULL) {
              f_error("No space for %s", w);
       }
       return(cp);
}

/* Error Functions used by LEX */

void error(format, argument)
char *format, *argument;
{
       fprintf(stderr, format, argument);
}

void f_error( format, argument )
char *format, *argument;
{
	error( format, argument );
	exit(1);
}

#include "stats.c"




