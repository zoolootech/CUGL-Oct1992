%{

/*
**    HOC3 calculator from "Unix Programming Environment",
**			p247, ch8, by Kernighan & Pike
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <memory.h>					/* TURBO-C requires MEM.H */
#include <signal.h>
#include <setjmp.h>

#include "hocdecl.h"				/* data declarations, function prototypes */

%}

%union {                /* stack object type    */
    double  val ;       /* actual value         */
    Symbol  *sym ;      /* symbol table ptr     */
}

%token  <val>   NUMBER
%token  <sym>   VAR BLTIN UNDEF
%type   <val>   expr asgn
%right  '='
%left   '+' '-'
%left   '*' '/'
%left   UNARYMINUS
%right  '^'         /*  exponentiation */

%%

list:		/* nothing */
        |   list '\n'
        |   list asgn '\n'
        |   list expr '\n'      { printf("\t%.8g\n", $2) ; }
        |   list error '\n'     { yyerrok ; }
        ;

asgn:		VAR '=' expr		{ $$ = $1->u.val = $3 ; $1->type = VAR ; }
		;
expr:       NUMBER
        |   VAR             { if ($1->type == UNDEF)
                                execerror("undefined variable", $1->name) ;
                            $$ = $1->u.val ; }
        |   asgn
        |   BLTIN '(' expr ')'  { $$ = (*($1->u.ptr))($3) ; }
        |   expr '+' expr   { $$ = $1 + $3 ; }
        |   expr '-' expr   { $$ = $1 - $3 ; }
        |   expr '*' expr   { $$ = $1 * $3 ; }
        |   expr '/' expr   {
                if ($3 == 0.0)
                    execerror("division by zero", "") ;
                $$ = $1 / $3 ; }
        |   expr '^' expr   { $$ = Pow( $1, $3) ; }
        |   '(' expr ')'    { $$ = $2 ; }
        |   '-' expr        %prec UNARYMINUS { $$ = -$2 ; }
        ;

%%

char *progname ;
int lineno = 0 ;
jmp_buf begin ;

/*----------------------------------------------------------------------------
**  MAIN     (hoc3.y)
*/
int main( char *argv[])
{
    progname = argv[0] ;
    init() ;
    setjmp(begin) ;
    signal( SIGFPE, fpecatch) ;				/* Turbo-C uses "ssignal()" */
    yyparse() ;

	return 0 ;
}

/*----------------------------------------------------------------------------
**  YYLEX
**   lexical analyser function
*/
int yylex( void)
{
 int c ;
 char sbuf[100], *p ;
 Symbol *s ;

    p = &sbuf[0] ;

    while ((c = getchar()) == ' ' || c == '\t')
        ;

    if (c == EOF)
        return 0 ;

    if (c == '.' || isdigit(c))
    {
        ungetc( c, stdin) ;
        scanf("%lf", &yylval.val) ;
        return NUMBER ; 
    }

    if (isalpha(c))
    {
        do{
            *p++ = (char) c ;
        } while ((c = getchar()) != EOF && isalnum(c)) ;

        ungetc( c, stdin) ;
        *p = '\0' ;

        if ((s = lookup(&sbuf[0])) == 0)
            s = install( sbuf, UNDEF, 0.0) ;

        yylval.sym = s ;

        return ((s->type == UNDEF) ? (VAR) : (s->type)) ;
    }

    if (c == '\n')
        lineno++ ;

    return c ;
}

/*----------------------------------------------------------------------------
**  EXECERROR
*/
void execerror( char *s, char *t)
{
    warning( s, t) ;
    longjmp( begin, 0) ;
}

/*----------------------------------------------------------------------------
**  FPECATCH
*/
void fpecatch( void)
{
    execerror("floating point exception", (char *) 0) ;
}


/*----------------------------------------------------------------------------
**  YYERROR
*/
void yyerror( char *s)                  /* called by yacc syntax error */
{
    warning( s, (char *) 0) ;
}

/*----------------------------------------------------------------------------
**  WARNING
*/
void warning( char *s, char *t)
{
    fprintf( stderr, "%s: %s", progname, s) ;

    if (t)
        fprintf( stderr, " %s", t) ;

    fprintf( stderr, " near line %d\n", lineno) ;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

