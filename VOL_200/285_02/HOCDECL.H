/*
** HOCDECL.H   definitions for the hoc3.y calculator
**          	in K&P p246 ch8.
*/

typedef struct Symbol {    /* symbol table entry */
         char *name ;
         short type ;      /* VAR, BLTIN, UNDEF  */
         union {
               double val ;         /* if VAR */
               double (*ptr)() ;    /* if BLTIN */
         } u ;
         struct Symbol *next ;
} Symbol ;

/*
**		functions in HOC3.C
*/

extern int main( char **) ;
extern int yylex( void) ;
extern void execerror( char *, char *) ;
extern void fpecatch( void) ;
extern void yyerror( char *) ;
extern void warning( char *, char *) ;

/*
**		functions in SYMBOL.C
*/

extern Symbol *lookup( char *) ;
extern Symbol *install( char *, int, double) ;
extern char *emalloc( unsigned) ;

/*
**		function in INIT.C
*/

extern void init( void) ;

/*
**		functions in MATH.C
*/

extern  double Log(double x);
extern  double Log10(double x);
extern  double Exp(double x);
extern  double Pow(double x,double y);
extern  double integer(double x);
extern  double Sqrt(double x);
extern  double errcheck( double, char *) ;

