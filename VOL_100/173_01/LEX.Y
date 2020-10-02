/*
  HEADER:              CUG  nnn.nn;
  TITLE:               LEX - A Lexical Analyser Generator
  VERSION:             1.1 for IBM-PC
  DATE:                Jan 30, 1985
  DESCRIPTION:         A Lexical Analyser Generator. From UNIX
  KEYWORDS:            Lexical Analyser Generator YACC C PREP
  SYSTEM:              IBM-PC and Compatiables
  FILENAME:            LEX.Y
  WARNINGS:            This program is not for the casual user. It will
                       be useful primarily to expert developers.
                       This file must be processed by YACC with the -d
                       option.
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
 *
 * Modified 22-Jun-86 Andrew Ward -- Modified code to compile under Lattice C
 *                                 version 3.0h.  Corrected several errors
 *                                 from the assumption that pointers and
 *                                 integers are the same size.     
 *                                 New debug code for LATTICE C using assert
 *                                 to test for wild pointers.  Using a proper
 *                                 YACC this will produce a ytab.c file that
 *				   does not need hand modification.	 
 */

/*
 * lex -- grammar/lexical analyser
 */

%{
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
    extern int     yyline;
    %}
%union {
    char   *buff;
    struct nlist *list;
    struct des   *des_ptr;
    struct nfa   *elem_ptr;
}
%term NAME CCLASS STRING CONCAT
%token <buff>  STRING NAME CCLASS
%type <buff> NAME CCLASS
%type <list> namedef name
%type <des_ptr> pattern regexp
%left ';'
%left '='
%left '/'
%left '|'
%left '(' NAME STRING CCLASS
%left CONCAT
%left '*'
%%
%{
    struct nfa *np, *nbase;
    char *cp;
    struct des *dp, *dp1;
    struct trans *tp;
    struct nlist *nl;
    int i, c;
    %}
lexfile:
auxiliary_section translation_section
|

;

auxiliary_section:
auxiliaries '%' '%'
|       '%' '%'
;

auxiliaries:
auxiliaries auxiliary
|       auxiliary
;

auxiliary:
namedef '=' regexp ';' ={
    dp = $3;
    nl = $1;
    np = nl->nl_base;
    nl->nl_start = dp->d_start;
    nl->nl_final = dp->d_final;
    nl->nl_end = nfap;
#ifdef DEBUG
    printf("NFA for %s\n", nl->nl_name);
    nfaprint(dp->d_start, nl->nl_base);
#endif
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
}
|       '%' '{'         ={
    copycode();
}
;

namedef:
NAME    ={
    $$ = lookup($1);
    $$->nl_base = nfap;
    if ($$->nl_start)
       yyerror("%s redefined", $$->nl_name);
}
;

name:
NAME    = {
    $$ = lookup($1);
}
;

regexp:
CCLASS = {
    np = elem(CCL, $1);
    $$ = newdp(np, (np->n_succ[0] = elem(FIN, (char *)NULL ) ) );
}
|       STRING ={
    cp = $1;
    if (str_len == 0) {
       np = elem(EPSILON, (char *)NULL );
       $$ = newdp(np, (np->n_succ[0] = elem(FIN, (char *)NULL ) ) );
       /* return(0);*/ /* AMW: the return here appears in error */
       }
    else /* AMW: else stmt added 1 May 1986 $$->d_start added */
       {
       $$->d_start = np = elem(*cp++, (char *)NULL);
       while(--str_len > 0)
           np = np->n_succ[0] = elem(*cp++,(char *)NULL);
       $$ = newdp($$->d_start, ( np->n_succ[0] = elem( FIN, (char *)NULL ) ) );
       }
    }
    |       name    ={
       if ((nl = $1)->nl_end == NULL)
       {
           yyerror("%s not defined", nl->nl_name);
           nl->nl_base = nl->nl_end = elem(FIN, (char *)NULL);
           nl->nl_start = nl->nl_final = nl->nl_base;
       }
       $$ = dp = (struct des *)lalloc(1, sizeof(struct des), "dfa input");
       nbase = nfap;
       i = nl->nl_end - nl->nl_base;
       if ((nfap += i) >= &nfa[MAXNFA]) {
           errmsg("Out of NFA nodes");
           exit(1);
       }
       copynfa(nl, nbase, dp);
    }
    |       regexp '*' ={
       $$ = dp = $1;
       dp->d_start = newnfa(EPSILON, (np = dp->d_start), (struct nfa *)NULL);
       dp->d_final->n_char = EPSILON;
       dp->d_final->n_succ[0] = np;
       dp->d_final->n_succ[1] = np = elem(FIN, (char *)NULL);
       dp->d_start->n_succ[1] = np;
       dp->d_final = np;
    }
    |       regexp '|' regexp ={
       $$ = dp = $1;
       dp->d_start = newnfa(EPSILON, dp->d_start, $3->d_start);
       dp->d_final->n_char = EPSILON;
       dp->d_final = dp->d_final->n_succ[0] = np = elem(FIN, (char *)NULL );
       dp = $3;
       dp->d_final->n_char = EPSILON;
       dp->d_final->n_succ[0] = np;
#ifdef DEBUG
       assert( isdata( (char *)$3, sizeof( struct des) ) );
#endif
       free((char *)$3);
    }
    |       regexp regexp %prec CONCAT ={
       $$ = $1;
       dp = $2;
       np = $$->d_final;
       $$->d_final = dp->d_final;
       np->n_char = dp->d_start->n_char;
       np->n_ccl = dp->d_start->n_ccl;
       np->n_succ[0] = dp->d_start->n_succ[0];
       np->n_succ[1] = dp->d_start->n_succ[1];
#ifdef DEBUG
       assert( isdata( (char *)$2, sizeof( struct nlist) ) );
#endif
       free((char *)$2);
    }
    |       '(' regexp ')' ={
       $$ = $2;
    }
    ;

translation_section:
    translations ={
       ending();
trans1:

       printf("\nNFA for complete syntax\n");
       printf("state 0\n");
       for (tp = trans; tp < transp; tp++)
           printf("\tepsilon\t%d\n", tp->t_start-nfa);
       for (tp = trans; tp < transp; tp++)
           nfaprint(tp->t_start, nfa);
       ;
    }
    |       ={
       goto trans1;
    }
    ;

translations:
    translations translation
       |       llactr translation
       ;

llactr:
    ={
       llactr();
    }
    ;

translation:
    pattern action ={
       dp = $1;
       newtrans(dp->d_start, dp->d_final);
    }
    |       '%' '{'         ={
       copycode();
    }
    |       '%' '%'         ={
       ending();
       while ((c = get()) != EOF)
           putc(c, llout);
    }
    ;

action:
    '{'     ={
       action();
    }
    ;

pattern:
    regexp '/' regexp ={
       if (nlook >= NBPW)
           yyerror("More than %d translations with lookahead",NBPW);
       $$ = dp = $1;
       np = dp->d_final;
       np->n_char = EPSILON;
       np->n_flag |= LOOK;
       np->n_succ[0] = $3->d_start;
       dp->d_final = $3->d_final;
       np->n_look = nlook;
       dp->d_final->n_look = nlook++;
       dp->d_final->n_flag |= FLOOK;
#ifdef DEBUG
       assert( isdata( (char *)$3, sizeof( struct des) ) );
#endif
       free((char *)$3);
    }
    |       regexp
       ;

%%
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
           return(c);
       }
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
#ifdef DEBUG
       if( st != NULL) assert( isdata( (char *)st, sizeof( struct nfa ) ) );
       if( fi != NULL) assert( isdata( (char *)fi, sizeof( struct nfa ) ) );
#endif
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
