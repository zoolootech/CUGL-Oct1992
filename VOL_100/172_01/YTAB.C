/*
  HEADER:              CUG  nnn.nn;
  TITLE:               LEX - A Lexical Analyser Generator
  VERSION:             1.1 for IBM-PC
  DATE:                Jan 30, 1985
  DESCRIPTION:         A Lexical Analyser Generator. From UNIX
  KEYWORDS:            Lexical Analyser Generator YACC C PREP
  SYSTEM:              IBM-PC and Compatiables
  FILENAME:            YTAB.C
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

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "lexlex.h"

char    ccl[ (NCHARS+1) / NBPC ];
int  sz_ccl = (NCHARS+1) / NBPC;
    char *yysterm[] = {
       "error",
       "NAME",
       "CCLASS",
       "STRING",
       "CONCAT",
       0     };
    extern char *progname;
    extern char *breakc;
    extern char *ignore;
    extern char *illeg;
    extern int  nlook;
    char *lalloc();
    struct des {
       struct nfa *d_start;
       struct nfa *d_final;
    };
    struct nlist {
       struct  nlist   *nl_next;
       struct  nfa     *nl_base;
       struct  nfa     *nl_end;
       struct  nfa     *nl_start;
       struct  nfa     *nl_final;
       char    *nl_name;
    }
    *nlist;
    extern  int str_len;
    extern  struct  nfa     *nfap;
    extern  struct  nfa     *elem(int, char *);
    extern  struct  des     *newdp(struct nfa *, struct nfa *);
    extern  struct  nlist   *lookup(char *);
    extern int     mapc(int);
    extern int     cclass(void);
    extern void    name(int);
    extern void    action(void);
    extern void    skipstr(int);
    extern void    copycode(void);
    extern void    string(int);
    extern void    copynfa(struct nlist *, struct nfa *, struct des *);
    extern void    spccl(char *, char *, struct des *, char **);
    extern void    unget( int );
    extern void    copy(char *, char *, int);
    extern void    errmsg(char *);
    extern void    yyerror(char *, char *);
    extern void    newcase(int);
    extern void    llactr(void);
    extern void    setline(void);
    extern void    cclprint(char *);
    extern  int     yyline;

typedef union  {
    char   *buff;
    struct nlist *list;
    struct des   *des_ptr;
    struct nfa   *elem_ptr;
} YYSTYPE;
#define NAME 257
#define CCLASS 258
#define STRING 259
#define CONCAT 260
#define yyclearin yychar = -1
#define yyerrok yyerrflag = 0
extern int yychar;
extern short yyerrflag;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
YYSTYPE yylval, yyval;

    struct nfa *np, *nbase;
    char *cp;
    struct des *dp, *dp1;
    struct trans *tp;
    struct nlist *nl;
    int i, c;
    #define YYERRCODE 256


    /*
 * Lexical analyser
 * (it isn't done with lex...)
 */
       char    buffer[150];

int yylex()
    {

       int c;
       /*      char *cp; */
       int lno;

       if (yyline == 0)
           yyline++;
loop:
       c = get();
       if (isupper(c)) {
           name(c);
           yylval.buff = strlwr( yylval.buff );
           return(STRING);
       }
       else if (islower(c) || c == '_') {
           name(c);
           return(NAME);
       }
       switch (c) {
       case EOF:
           return(0);

       case '[':
           return(cclass());

       case '(':
       case ')':
       case '{':
       case '}':
       case '*':
       case '|':
       case '=':
       case ';':
       case '%':
           return(c);

       case '/':
           if ((c = get()) != '*') {
               unget(c);
               return('/');
           }
           lno = yyline;
           for (; c != EOF; c = get())
               if (c == '*')
                   if ((c = get()) == '/')
                       goto loop;
                   else
                       unget(c);
           yyline = lno;
           errmsg("End of file in comment");

       case '\'':
       case '"':
           yylval.buff = buffer;
           string(c);
           return(STRING);

       case '\n':
       case '\r':
       case ' ':
       case '\t':
           goto loop;

       default:

           yylval.buff = buffer;
/*         if (c == '\\') {
               unget(c);
               c = mapch(EOF);
           }
*/
           buffer[0] = c;
           buffer[1] = '\0';
           str_len = 1;
           return(STRING);
       }
/* ERR: Warning 85: function return value mismatch */
    }


int cclass()
    {
       int c, i, lc;
       int compl;

       compl = 0;
       /* Zero the ccl array */
       for (i = 0; i < sz_ccl; i++)  ccl[i] = '\0';
       /* Check if exclusion definition */
       if ((c = get()) == '^') compl++;
       else  unget(c);

       lc = -1;
       while( ( c = mapc(']') ) != EOF)
       {
           if (c == '-' && lc >= 0)
           {
               if((c = mapc(']')) == EOF)   break;
               /* Map 'c' into bit pattern */
               for(i = lc; i <= c; i++)  ccl[ i / NBPC ] |= 1 << (i % NBPC);
               lc = -1;
               continue;
           }

           ccl[ c / NBPC ] |= 1 << ( c % NBPC );
           lc = c;
       }

       if (compl) {
           for (i = 0; i < sz_ccl; i++)
               ccl[i] ^= -1;
           if (aflag == 0)
               for (i = 0200; i < ( 1 << NBPC ); i++)
                   ccl[i/NBPC] &= ~(1 << (i%NBPC));
       }
       yylval.buff = newccl( ccl );
       return(CCLASS);
    }

void string(ec)
int ec;
    {
       char *cp;
       int c;
       extern int str_len;
       for(cp = buffer; (c = mapc(ec)) != EOF;)
           *cp++ = c;
       *cp = '\0';
       str_len = strlen( buffer );
    }

int mapc(ec)
       int ec;
    {

       int c, v, i;

       if((c = get()) == ec)  return(EOF);
       switch(c) {
       case EOF:
           errmsg("End of file in string");
           return(c);

       case '\\':
           if ((c = get()) >= '0' && c <= '7') {
               i = 0;
               for (v = 0; c>='0' && c<='7' && i++<3; c = get())
                   v = v*010 + c-'0';
               unget(c);
               return(v&CMASK);
           }
           switch (c) {
           case 'n':
               return('\n');

           case 't':
               return('\t');

           case 'b':
               return('\b');

           case 'r':
               return('\r');

           case '\n':
               yyline++;
               return(mapc(ec));
           }

       default:
       }
       return(c);
    }

void name(c)
       int c;
    {
       char *cp;

       for (yylval.buff=cp=buffer; isalpha(c) || isdigit(c) || c=='_'; c=get())
           *cp++ = c;
       *cp = '\0';
       str_len = strlen( buffer );
       unget(c);
    }

/*
 * Miscellaneous functions
 * used only by lex.y
 */
struct nfa *elem(k, v)
int k;
char *v;
    {
       struct nfa *fp;
       fp = newnfa(k, (struct nfa *)NULL, (struct nfa *)NULL);

       if (k == CCL)
           fp->n_ccl = v;
       return(fp);
    }

struct des *newdp(st, fi)
    struct nfa *st, *fi;
    {
       struct des *dp;

       dp = (struct des *)lalloc(1, sizeof(struct des), "dfa input");
       if( st != NULL) assert( isdata( (char *)st, sizeof( struct nfa ) ) );
       if( fi != NULL) assert( isdata( (char *)fi, sizeof( struct nfa ) ) );
       dp->d_start = st;
       dp->d_final = fi;
       return(dp);
    }

void action()
    {
       int c;
       int lno, lev;

       newcase(transp-trans);
       lno = yyline;
       lev = 0;
       for (; (c = get()) != EOF && (c != '}' || lev); putc(c, llout))
           if (c == '{')
               lev++;
           else if (c == '}')
               lev--;
           else if (c == '\'' || c == '"') {
               putc(c, llout);
               skipstr(c);
           }
       fprintf(llout, "\n\tbreak;\n");
       if (c == EOF) {
           yyline = lno;
           errmsg("End of file in action");
       }
    }

void skipstr(ec)
       int ec;
    {
       int c;

       while ((c = get()) != ec && c != EOF) {
           putc(c, llout);
           if (c == '\\' && (c = get()) != EOF)
               putc(c, llout);
       }
    }


void copycode()
    {
       int lno;
       int c;

       setline();
       lno = yyline;
       for (; (c = get()) != EOF; putc(c, llout))
           if (c == '%')
           {
               if ((c = get()) == '}')
                   return;
               unget(c);
               c = '%';
           }
       yyline = lno;
       errmsg("Incomplete %{ declaration");
       exit(1);
    }

struct nlist *lookup(s)
       char *s;
    {
       struct nlist *nl;
       char *cp;

       for (nl = nlist; nl; nl = nl->nl_next)
           if (!strcmp(s, nl->nl_name))
               return(nl);
       nl = (struct nlist *)lalloc(1, sizeof(struct nlist), "namelist");
       nl->nl_start = nl->nl_end = nl->nl_base = nl->nl_final = (struct nfa *)NULL;
       nl->nl_next = nlist;
       nlist = nl;
       nl->nl_name = cp = strdup( s );
       return(nl);
    }

void copynfa(nl, nbase, dp)
       struct nlist *nl;
       struct des *dp;
       struct nfa *nbase;
{
       struct nfa *np, *ob;
       int j;
       int i;

       ob = nl->nl_base;
       i = nl->nl_end - ob;

#ifdef DEBUG
       fprintf(stdout, "\nCOPYNFA: index i = %d",i);
#endif
       np = nbase;
       copy((char *)np, (char *)ob, sizeof(struct nfa)*i);
       for (np = nbase; i-- > 0; np++) {
           np->n_flag &= ~NPRT;
           for (j = 0; j < 2; j++)
               if (np->n_succ[j])
                   np->n_succ[j] = nbase + (np->n_succ[j] - ob);
       }
       dp->d_start = nbase + (int)(nl->nl_start-ob);
       dp->d_final = nbase + (int)(nl->nl_final-ob);
}

void spccl(nm, isit, dp, where)
    char       *nm, *isit;
    struct des *dp;
    char       **where;
    {
       if (!strcmp(nm, isit))
       {
            if(*where != 0 )
                 yyerror("redefinition of %s class", isit);
            if (dp->d_start->n_char == CCL &&
                 dp->d_start->n_succ[0] == dp->d_final)
                 *where = dp->d_start->n_ccl;
            else
                 yyerror("Illegal %s class", isit);
       }
    }

int get()
    {
       int c;

       if ((c = getc(lexin)) == '\n')
           yyline++;
       return(c);
    }

void unget(c)
    int c;
    {
       if (c == '\n')
           yyline--;
       ungetc(c, lexin);
    }

void errmsg(s)
       char *s;
    {
       if (yyline)
           fprintf(stderr, "%d: ", yyline);
       fprintf(stderr, "%r", &s);
       if (yychar > 256)
           fprintf(stderr, " near `%s'", yysterm[yychar-256]);
       else if (yychar < 256 && yychar > 0)
           fprintf(stderr, " near `%c'", yychar);
       fprintf(stderr, "\n");
    }

void copy(out, in, count)
char   *out;
char   *in;
int    count;
{
        while (--count >= 0)
                *out++ = *in++;
}

void yyerror(format, value)
char *format, *value;
{
        if (yyline)
                fprintf(stderr, "%d: ", yyline);
        fprintf(stderr, format, value);
        if (yychar > 256)
                fprintf(stderr, " near '%s'", yysterm[yychar-256]);
        else if (yychar < 256 && yychar > 0)
                fprintf(stderr, " near '%c'", yychar);
        fprintf(stderr, "\n");
}
int nterms = 15;
int nnonter = 13;
int nstate = 41;

char *yysnter[14] = {
"$accept",
"lexfile",
"auxiliary_section",
"translation_section",
"auxiliaries",
"auxiliary",
"namedef",
"regexp",
"name",
"translations",
"translation",
"llactr",
"pattern",
"action" };






short yyexca[] ={
-1, 1,
        0, -1,
        -2, 0,
-1, 2,
        0, 19,
        -2, 22,
        };
#define YYNPROD 29
#define YYLAST 244
short yyact[]={

  18,  31,  26,  23,  32,  13,  23,  40,  32,  23,
  29,  32,  23,  16,  32,  23,  34,  32,  23,  35,
  32,  23,  11,   4,  25,   5,  19,  19,  37,  12,
  28,  15,  27,  10,   9,   3,   8,   2,   1,  17,
  36,  22,   6,   0,   0,   0,   0,   0,   0,   0,
  38,   0,  39,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,  30,  14,   0,
  33,  14,   0,  33,   0,   0,  33,   0,   0,  33,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
  24,  20,  21,  24,  20,  21,  24,  20,  21,  24,
  20,  21,  24,  20,  21,  24,  20,  21,  24,  20,
  21,   0,   7,   7 };
short yypact[]={

 -14,-1000,-1000, -15, -32,-1000, -30,-1000,-1000, -37,
 -37, -35,-1000,-1000,-1000, -19,-1000,-113, -36, -28,
-1000,-1000,-1000, -19,-1000,-1000,-1000, -31,-1000,-1000,
-1000,-1000,-1000, -19, -38, -19, -34,-1000, -22, -25,
-1000 };
short yypgo[]={

   0,  42,  41,  39,  16,  38,  37,  36,  35,  25,
  34,  13,  33,  30 };
short yyr1[]={

   0,   5,   5,   6,   6,   8,   8,   9,   9,   1,
   2,   4,   4,   4,   4,   4,   4,   4,   7,   7,
  10,  10,  12,  11,  11,  11,  13,   3,   3 };
short yyr2[]={

   0,   2,   0,   3,   2,   2,   1,   4,   2,   1,
   1,   1,   1,   1,   2,   3,   2,   3,   1,   0,
   2,   2,   0,   2,   2,   2,   1,   3,   1 };
short yychk[]={

-1000,  -5,  -6,  -8,  37,  -9,  -1, 257,  -7, -10,
 -12,  37,  -9,  37, 123,  61, -11,  -3,  37,  -4,
 258, 259,  -2,  40, 257, -11,  37,  -4, -13, 123,
 123,  37,  42, 124,  -4,  47,  -4,  59,  -4,  -4,
  41 };
short yydef[]={

   2,  -2,  -2,   0,   0,   6,   0,   9,   1,  18,
   0,   0,   5,   4,   8,   0,  20,   0,   0,  28,
  11,  12,  13,   0,  10,  21,   3,   0,  23,  26,
  24,  25,  14,   0,  16,   0,   0,   7,  15,  27,
  17 };
#define YYFLAG -1000
#define YYERROR goto yyerrlab
#define YYACCEPT return(0)
#define YYABORT return(1)
#ifdef  MSDOS
#define Move(s,d) movmem( (char *)s,(char *)d,sizeof(YYSTYPE))
#endif

/*      parser for yacc output  */

int     yydebug = 0;         /* 1 for debugging */
YYSTYPE yyv[YYMAXDEPTH];     /* where the values are stored */
int     yychar = -1;         /* current input token number */
int     yynerrs = 0;         /* number of errors */
short   yyerrflag = 0;       /* error recovery flag */

yyparse()
{

        short    yys[YYMAXDEPTH];
        short    yyj, yym;
        register YYSTYPE *yypvt;
        register short yystate, *yyps, yyn;
        register YYSTYPE *yypv;
        register short *yyxi;

        yystate = 0;
        yychar = -1;
        yynerrs = 0;
        yyerrflag = 0;
        yyps= &yys[-1];
        yypv= &yyv[-1];

 yystack:    /* put a state and value onto the stack */

        if( yydebug  ) printf( "state %d, char 0%o\n", yystate, yychar );
        /* The next statement produces a warning when compiled by LATTICE */
        /* Ver 2.15, i.e. beyound object size.  The code still works  */
        if( ++yyps > &yys[YYMAXDEPTH] ) { yyerror( "yacc stack overflow" ); return(1); }
                *yyps = yystate;
                ++yypv;
#ifdef MSDOS
             Move(&yyval,yypv); /* LATTICE and Ver. 7 conflict resolution */
#else
             *yypv = yyval;
#endif
 yynewstate:

        yyn = yypact[yystate];

        if( yyn<= YYFLAG ) goto yydefault; /* simple state */

        if( yychar<0 ) if( (yychar=yylex())<0 ) yychar=0;
        if( (yyn += yychar)<0 || yyn >= YYLAST ) goto yydefault;

        if( yychk[ yyn=yyact[ yyn ] ] == yychar ){ /* valid shift */
                yychar = -1;
#ifdef MSDOS
                Move(&yylval, &yyval); /* AMW */
#else
                yyval = yylval;
#endif
                yystate = yyn;
                if( yyerrflag > 0 ) --yyerrflag;
                goto yystack;
                }

 yydefault:
        /* default state action */

        if( (yyn=yydef[yystate]) == -2 ) {
                if( yychar<0 ) if( (yychar=yylex())<0 ) yychar = 0;
                /* look through exception table */

                for( yyxi=yyexca; (*yyxi!= (-1)) || (yyxi[1]!=yystate) ; yyxi += 2 ) ; /* VOID */
                while( *(yyxi+=2) >= 0 ){
                        if( *yyxi == yychar ) break;
                        }
                if( (yyn = yyxi[1]) < 0 ) return(0);   /* accept */
                }

        if( yyn == 0 ){ /* error */
                /* error ... attempt to resume parsing */

                switch( yyerrflag ){

                case 0:   /* brand new error */

                        yyerror( "syntax error" );
                yyerrlab:
                        ++yynerrs;

                case 1:
                case 2: /* incompletely recovered error ... try again */

                        yyerrflag = 3;

                        /* find a state where "error" is a legal shift action */

                        while ( yyps >= yys ) {
                           yyn = yypact[*yyps] + YYERRCODE;
                           if( yyn>= 0 && yyn < YYLAST && yychk[yyact[yyn]] == YYERRCODE ){
                              yystate = yyact[yyn];  /* simulate a shift of "error" */
                              goto yystack;
                              }
                           yyn = yypact[*yyps];

                           /* the current yyps has no shift onn "error", pop stack */

                           if( yydebug ) printf( "error recovery pops state %d, uncovers %d\n", *yyps, yyps[-1] );
                           --yyps;
                           --yypv;
                           }

                        /* there is no state on the stack with an error shift ... abort */

        yyabort:
                        return(1);


                case 3:  /* no shift yet; clobber input char */

                        if( yydebug ) printf( "error recovery discards char %d\n", yychar );

                        if( yychar == 0 ) goto yyabort; /* don't discard EOF, quit */
                        yychar = -1;
                        goto yynewstate;   /* try again in the same state */

                        }

                }

        /* reduction by production yyn */

                if( yydebug ) printf("reduce %d\n",yyn);
                yyps -= yyr2[yyn];
                yypvt = yypv;
                yypv -= yyr2[yyn];
#ifdef MSDOS
                Move(&yypv[1],&yyval); /* LATTICE and Ver. 7 conflict resolution */
#else
                yyval = yypv[1];       /* Ver 7 permits asignments of this type */
#endif
                yym=yyn;
                        /* consult goto table to find next state */
                yyn = yyr1[yyn];
                yyj = yypgo[yyn] + *yyps + 1;
                if( yyj>=YYLAST || yychk[ yystate = yyact[yyj] ] != -yyn ) yystate = yyact[yypgo[yyn]];
                switch(yym)
{

case 7:
{
    dp = yypvt[-1].des_ptr;
    nl = yypvt[-3].list;
    np = nl->nl_base;
    nl->nl_start = dp->d_start;
    nl->nl_final = dp->d_final;
    nl->nl_end = nfap;
/* */
    printf("NFA for %s\n", nl->nl_name);
    nfaprint(dp->d_start, nl->nl_base);
/* */
    i = nl->nl_end - nl->nl_base;
    nbase = (struct nfa *)lalloc(i, sizeof(struct nfa), "nfa storage");
    copynfa(nl, nbase, dp);
    nl->nl_start = dp->d_start;
    nl->nl_final = dp->d_final;
    nl->nl_end = nbase+i;
    nl->nl_base = nbase;
    nfap = np;
    spccl(nl->nl_name, "ignore", dp, &ignore);
    spccl(nl->nl_name, "break", dp, &breakc);
    spccl(nl->nl_name, "illegal", dp, &illeg);
} break;
case 8:
{
    copycode();
} break;
case 9:
{
    yyval.list = lookup(yypvt[-0].buff);
    yyval.list->nl_base = nfap;
    if (yyval.list->nl_start)
       yyerror("%s redefined", yyval.list->nl_name);
} break;
case 10:
 {
    yyval.list = lookup(yypvt[-0].buff);
} break;
case 11:
 {
    np = elem(CCL, yypvt[-0].buff);
    yyval.des_ptr = newdp(np, (np->n_succ[0] = elem(FIN, (char *)NULL ) ) );
} break;
case 12:
{
    cp = yypvt[-0].buff;
    if (str_len == 0) {
       np = elem(EPSILON, (char *)NULL );
       yyval.des_ptr = newdp(np, (np->n_succ[0] = elem(FIN, (char *)NULL ) ) );
       return(0); /* AMW: the return here appears in error */
       }
    else /* AMW: else stmt added 1 May 1986 $$->d_start added */
       {
       yyval.des_ptr->d_start = np = elem(*cp++, (char *)NULL);
       while(--str_len > 0)
           np = np->n_succ[0] = elem(*cp++,(char *)NULL);
       yyval.des_ptr = newdp(yyval.des_ptr->d_start, ( np->n_succ[0] = elem( FIN, (char *)NULL ) ) );
       }
    } break;
case 13:
{
       if ((nl = yypvt[-0].list)->nl_end == NULL)
       {
           yyerror("%s not defined", nl->nl_name);
           nl->nl_base = nl->nl_end = elem(FIN, (char *)NULL);
           nl->nl_start = nl->nl_final = nl->nl_base;
       }
       yyval.des_ptr = dp = (struct des *)lalloc(1, sizeof(struct des), "dfa input");
       nbase = nfap;
       i = nl->nl_end - nl->nl_base;
       if ((nfap += i) >= &nfa[MAXNFA]) {
           errmsg("Out of NFA nodes");
           exit(1);
       }
       copynfa(nl, nbase, dp);
    } break;
case 14:
{
       yyval.des_ptr = dp = yypvt[-1].des_ptr;
       dp->d_start = newnfa(EPSILON, (np = dp->d_start), (struct nfa *)NULL);
       dp->d_final->n_char = EPSILON;
       dp->d_final->n_succ[0] = np;
       dp->d_final->n_succ[1] = np = elem(FIN, (char *)NULL);
       dp->d_start->n_succ[1] = np;
       dp->d_final = np;
    } break;
case 15:
{
       yyval.des_ptr = dp = yypvt[-2].des_ptr;
       dp->d_start = newnfa(EPSILON, dp->d_start, yypvt[-0].des_ptr->d_start);
       dp->d_final->n_char = EPSILON;
       dp->d_final = dp->d_final->n_succ[0] = np = elem(FIN, (char *)NULL );
       dp = yypvt[-0].des_ptr;
       dp->d_final->n_char = EPSILON;
       dp->d_final->n_succ[0] = np;
       assert( isdata( (char *)yypvt[-0].des_ptr, sizeof( struct des) ) );
       free((char *)yypvt[-0].des_ptr);
    } break;
case 16:
{
       yyval.des_ptr = yypvt[-1].des_ptr;
       dp = yypvt[-0].des_ptr;
       np = yyval.des_ptr->d_final;
       yyval.des_ptr->d_final = dp->d_final;
       np->n_char = dp->d_start->n_char;
       np->n_ccl = dp->d_start->n_ccl;
       np->n_succ[0] = dp->d_start->n_succ[0];
       np->n_succ[1] = dp->d_start->n_succ[1];

       assert( isdata( (char *)yypvt[-0].des_ptr, sizeof( struct nlist) ) );
       free((char *)yypvt[-0].des_ptr);
    } break;
case 17:
{
       yyval.des_ptr = yypvt[-1].des_ptr;
    } break;
case 18:
{
       ending();
trans1:

       printf("\nNFA for complete syntax\n");
       printf("state 0\n");
       for (tp = trans; tp < transp; tp++)
           printf("\tepsilon\t%d\n", tp->t_start-nfa);
       for (tp = trans; tp < transp; tp++)
           nfaprint(tp->t_start, nfa);
       ;
    } break;
case 19:
{
       goto trans1;
    } ;
case 22:
{
       llactr();
    } break;
case 23:
{
       dp = yypvt[-1].des_ptr;
       newtrans(dp->d_start, dp->d_final);
    } break;
case 24:
{
       copycode();
    } break;
case 25:
{
       ending();
       while ((c = get()) != EOF)
           putc(c, llout);
    } break;
case 26:
{
       action();
    } break;
case 27:
{
       if (nlook >= NBPW)
           yyerror("More than %d translations with lookahead",NBPW);
       yyval.des_ptr = dp = yypvt[-2].des_ptr;
       np = dp->d_final;
       np->n_char = EPSILON;
       np->n_flag |= LOOK;
       np->n_succ[0] = yypvt[-0].des_ptr->d_start;
       dp->d_final = yypvt[-0].des_ptr->d_final;
       np->n_look = nlook;
       dp->d_final->n_look = nlook++;
       dp->d_final->n_flag |= FLOOK;

       assert( isdata( (char *)yypvt[-0].des_ptr, sizeof( struct des) ) );
       free((char *)yypvt[-0].des_ptr);
    } break;
}
                goto yystack;  /* stack new state and value */

/* ERR: Warning 85: function return value mismatch */
}



