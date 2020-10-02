
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
#ifndef YYSTYPE
#define YYSTYPE int
#endif
#define	CHAR	258
#define	NUMBER	259
#define	SECTEND	260
#define	SCDECL	261
#define	XSCDECL	262
#define	WHITESPACE	263
#define	NAME	264
#define	PREVCCL	265

