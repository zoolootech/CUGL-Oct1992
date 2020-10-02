typedef union {                /* stack object type    */
    double  val ;       /* actual value         */
    Symbol  *sym ;      /* symbol table ptr     */
} YYSTYPE;

#ifndef YYLTYPE
typedef
  struct yyltype
    {
      int timestamp;
      int first_line;
      int first_column;
      int last_line;
      int last_column;
      char *text;
   }
  yyltype;

#define YYLTYPE yyltype
#endif

#define	YYACCEPT	return(0)
#define	YYABORT	return(1)
#define	YYERROR	goto yyerrlab
#define	NUMBER	258
#define	VAR	259
#define	BLTIN	260
#define	UNDEF	261
#define	UNARYMINUS	262

