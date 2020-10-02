
/***************************************************************************/
/*			gawk -- GNU version of awk			   */
/*	     YACC input file to create the gAWK semantic parser 	   */
/*									   */
/*		 Copyright (C) 1986 Free Software Foundation		   */
/*		     Written by Paul Rubin, August 1986 		   */
/*									   */
/***************************************************************************/
/*									   */
/* GAWK is distributed in the hope that it will be useful, but WITHOUT ANY */
/* WARRANTY.  No author or distributor accepts responsibility to anyone    */
/* for the consequences of using it or for whether it serves any	   */
/* particular purpose or works at all, unless he says so in writing.	   */
/* Refer to the GAWK General Public License for full details.		   */
/*									   */
/* Everyone is granted permission to copy, modify and redistribute GAWK,   */
/* but only under the conditions described in the GAWK General Public	   */
/* License.  A copy of this license is supposed to have been given to you  */
/* along with GAWK so you can know your rights and responsibilities.  It   */
/* should be in a file named COPYING.  Among other things, the copyright   */
/* notice and this notice must be preserved on all copies.		   */
/*									   */
/* In other words, go ahead and share GAWK, but don't try to stop          */
/* anyone else from sharing it farther.  Help stamp out software hoarding! */
/*									   */
/***************************************************************************/


%{
#define YYDEBUG 12

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "awk.h"

STATIC int	  NEAR PASCAL	 yylex(void);
STATIC int	  NEAR PASCAL	 parse_escape(char **string_ptr);


/* The following variable is used for a very sickening thing.  The awk	  */
/* language uses white space as the string concatenation operator, but	  */
/* having a white space token that would have to appear everywhere in all */
/* the grammar rules would be unbearable.  It turns out we can return	  */
/* CONCAT_OP exactly when there really is one, just from knowing what	  */
/* kinds of other tokens it can appear between (namely, constants,	  */
/* variables, or close parentheses).  This is because concatenation has   */
/* the lowest priority of all operators.  want_concat_token is used to	  */
/* remember that something that could be the left side of a concat has	  */
/* just been returned.	If anyone knows a cleaner way to do this (don't   */
/* look at the Un*x code to find one, though), please suggest it.	  */

static int		  want_concat_token;

/* Two more horrible kludges.  The same comment applies to these two too  */

static int		  want_regexp	 = 0; /* lexical scanning kludge   */

int			  lineno = 1;	     /* JF for error msgs	  */

/* During parsing of a gAWK program, the pointer to the next character	  */
/* is in this variable. 						  */

char			 *lexptr;
char			 *lexptr_begin;

%}

%union
{
    long	  lval;
    AWKNUM	  fval;
    NODE	 *nodeval;
    int 	  nodetypeval;
    char	 *sval;
    NODE	*(PASCAL *ptrval)(NODE *);
}

%type <nodeval> exp start program rule pattern conditional regexp
%type <nodeval> action variable redirect_in redirect_out exp_list builtin
%type <nodeval> statements statement if_statement opt_exp
%type <nodetypeval> whitespace relop

%token <sval> NAME REGEXP YSTRING
%token <lval> ERROR INCDEC
%token <fval> NUMBER
%token <nodetypeval> ASSIGNOP MATCHOP NEWLINE CONCAT_OP
%token <nodetypeval> LEX_BEGIN LEX_END LEX_IF LEX_ELSE
%token <nodetypeval> LEX_WHILE LEX_FOR LEX_BREAK LEX_CONTINUE LEX_DELETE
%token <nodetypeval> LEX_PRINT LEX_PRINTF LEX_NEXT LEX_EXIT
%token <nodetypeval> RELOP_EQ RELOP_GEQ RELOP_LEQ RELOP_NEQ REDIR_APPEND
%token  LEX_IN
%token <lval> LEX_AND LEX_OR INCREMENT DECREMENT
%token <ptrval> LEX_BUILTIN LEX_MATCH_FUNC LEX_SUB_FUNC LEX_SPLIT_FUNC
%token <ptrval> LEX_GETLINE


/* Lowest to highest */
%right ASSIGNOP
%left ','
%right '?' ':'
%left LEX_OR
%left LEX_AND
%left CONCAT_OP
%nonassoc MATCHOP '>' '<' RELOP_EQ RELOP_GEQ RELOP_LEQ RELOP_NEQ
%left '+' '-'
%left '*' '/' '%'
%right UNARY
%right '^'

%%

start	: optional_newlines program
	  {
	      expression_value = $2;
	  }
	;


program	: rule
	  {
	      $$ = node($1, NODE_RULE_LIST, NULL);
	  }

	| program rule
	  {	       /* cons the rule onto the tail of list */
	      $$ = append_right($1, node($2, NODE_RULE_LIST, NULL));
	  }
	;

rule	: pattern action NEWLINE optional_newlines
	  {
	      $$ = node($1, NODE_RULE_NODE, $2);
	  }
	;


pattern	: /* empty */
	  {
	      $$ = NULL;
	  }

	| LEX_BEGIN
	  {
	      $$ = node(NULL, NODE_K_BEGIN, NULL);
	  }

	| LEX_END
	  {
	      $$ = node(NULL, NODE_K_END, NULL);
	  }
	| conditional
	  {
	      $$ = $1;
	  }

	| conditional ',' conditional
	  {
	      $$ = mkrangenode(node($1, NODE_COND_PAIR, $3));
	  }
	;


conditional :
	  '!' conditional	%prec UNARY
	  {
	      $$ = node($2, NODE_NOT, NULL);
	  }

	| '(' exp_list ')' CONCAT_OP LEX_IN NAME
	  {
	      $$ = node(variable($6), NODE_MEMBER_COND, $2);
	  }

	| exp CONCAT_OP LEX_IN NAME
	  {
	      $$ = node(variable($4), NODE_MEMBER_COND, $1);
	  }

	| conditional LEX_AND conditional
	  {
	      $$ = node($1, NODE_AND, $3);
	  }

	| conditional LEX_OR conditional
	  {
	      $$ = node ($1, NODE_OR, $3);
	  }

	| '(' conditional ')'
	  {
	      $$ = $2;
	      want_concat_token = 0;
	  }

	| regexp
	  {
	      $$ = $1;
	  }

	| exp MATCHOP regexp
	  {
	      $$ = node($1, $2, $3);
	  }

	| exp MATCHOP variable
	  {
	      $$ = node($1, $2, $3);
	  }

	| exp relop exp
	  {
	      $$ = node($1, $2, $3);
	  }
	;

action	: /* empty */
	  {
	      $$ = NULL;
	  }

	| '{' whitespace statements '}'
	  {
	      $$ = $3;
	  }
	;


statements :
	  statement
	  {
	      $$ = node($1, NODE_STATEMENT_LIST, NULL);
	  }

	| statements statement
	  {
	      $$ = append_right($1, node($2, NODE_STATEMENT_LIST, NULL));
	  }
	;

statement_term :
	  NEWLINE optional_newlines
	  {
	      $<nodetypeval>$ = NODE_ILLEGAL;
	  }

	| ';' optional_newlines
	  {
	      $<nodetypeval>$ = NODE_ILLEGAL;
	  }
	;

regexp	: '/'
	  {
	      ++want_regexp;
	  }
	  REGEXP '/'
	  {
	      want_regexp = 0;
	      $$ = node(NULL, NODE_REGEXP, (NODE *) make_regexp($3));
	  }

relop	: '>'
	  {
	      $$ = NODE_GREATER;
	  }
	| '<'
	  {
	      $$ = NODE_LESS;
	  }
	| RELOP_EQ
	  {
	      $$ = NODE_EQUAL;
	  }
	| RELOP_GEQ
	  {
	      $$ = NODE_GEQ;
	  }
	| RELOP_LEQ
	  {
	      $$ = NODE_LEQ;
	  }
	| RELOP_NEQ
	  {
	      $$ = NODE_NOTEQUAL;
	  }
	;

whitespace :
	  /* blank */
	  {
	      $$ = NODE_ILLEGAL;
	  }

	| CONCAT_OP
	| NEWLINE
	| whitespace CONCAT_OP
	| whitespace NEWLINE
	;

statement :
	  '{' whitespace statements '}' whitespace
	  {
	      $$ = $3;
	  }

	| if_statement
	  {
	      $$ = $1;
	  }

	| LEX_WHILE '(' conditional ')' whitespace statement
	  {
	      $$ = node($3, NODE_K_WHILE, $6);
	  }

	| LEX_FOR '(' opt_exp ';' conditional ';' opt_exp ')' whitespace statement
	  {
	      $$ = node($10, NODE_K_FOR, (NODE *) make_for_loop($3, $5, $7));
	  }

	| LEX_FOR '(' opt_exp ';' ';' opt_exp ')' whitespace statement
	  {
	      $$ = node($9, NODE_K_FOR,
			(NODE *) make_for_loop($3, NULL, $6));
	  }

	| LEX_FOR '(' NAME CONCAT_OP LEX_IN NAME ')' whitespace statement
	  {
	      $$ = node($9, NODE_K_ARRAYFOR,
			(NODE *) make_for_loop(variable($3),
					       NULL, variable($6)));
	  }

	| LEX_BREAK statement_term
	  /* for break, maybe we'll have to remember where to break to */
	  {
	      $$ = node(NULL, NODE_K_BREAK, NULL);
	  }

	| LEX_CONTINUE statement_term
	  /* similarly */
	  {
	      $$ = node(NULL, NODE_K_CONTINUE, NULL);
	  }

	| LEX_PRINT exp_list redirect_out statement_term
	  {
	      $$ = node($2, NODE_K_PRINT, $3);
	  }

	| LEX_PRINT '(' exp_list ')'	      /* BW: print(...) */
	  {
	      want_concat_token = FALSE;
	  }
	  redirect_out statement_term
	  {
	      $$ = node($3, NODE_K_PRINT, $6);
	  }

	| LEX_PRINTF exp_list redirect_out statement_term
	  {
	      $$ = node($2, NODE_K_PRINTF, $3);
	  }

	| LEX_PRINTF '(' exp_list ')'
	  {
	      want_concat_token = FALSE;
	  }
	  redirect_out statement_term
	  {
	      $$ = node($3, NODE_K_PRINTF, $6);
	  }

	| LEX_NEXT statement_term
	  {
	      $$ = node(NULL, NODE_K_NEXT, NULL);
	  }

	| LEX_EXIT statement_term
	  {
	      $$ = node(NULL, NODE_K_EXIT, NULL);
	  }

	| LEX_EXIT '(' exp ')' statement_term
	  {
	      $$ = node($3, NODE_K_EXIT, NULL);
	  }

	| LEX_DELETE NAME '[' exp_list ']' statement_term
	  {
	      $$ = node(variable($2), NODE_K_DELETE, $4);
	  }

	| exp statement_term
	  {
	      $$ = $1;
	  }
	;


if_statement:
	  LEX_IF '(' conditional ')' whitespace statement
	  {
	      $$ = node($3, NODE_K_IF,
			node($6, NODE_IF_BRANCHES, NULL));
	  }

	| LEX_IF '(' conditional ')' whitespace statement
	      LEX_ELSE whitespace statement
	  {
	      $$ = node($3, NODE_K_IF,
			node($6, NODE_IF_BRANCHES, $9));
	  }
	;

optional_newlines :
	  /* empty */
	| optional_newlines NEWLINE
	  {
	      $<nodetypeval>$ = NODE_ILLEGAL;
	  }
	;

redirect_out :
	  /* empty */
	  {
	      $$ = NULL;
	  }

	| '>' YSTRING	       %prec UNARY
	  {
	      $$ = node(make_string($2, -1), NODE_REDIRECT_OUTPUT, NULL);
	  }
	| '>' variable	       %prec UNARY
	  {
	      $$ = node($2, NODE_REDIRECT_OUTPUT, NULL);
	  }
	| REDIR_APPEND YSTRING	%prec UNARY
	  {
	      $$ = node(make_string($2, -1), NODE_REDIRECT_APPEND, NULL);
	  }
	| REDIR_APPEND variable %prec UNARY
	  {
	      $$ = node($2, NODE_REDIRECT_APPEND, NULL);
	  }
	;

redirect_in :
	  /* empty */
	  {
	      $$ = NULL;
	  }

	| '<' YSTRING	       %prec UNARY
	  {
	      $$ = node(make_string($2, -1), NODE_REDIRECT_INPUT, NULL);
	  }
	| '<' variable	       %prec UNARY
	  {
	      $$ = node($2, NODE_REDIRECT_INPUT, NULL);
	  }
	;

variable :
	  NAME
	  {
	      $$ = variable($1);
	  }

	| NAME '[' exp_list ']'
	  {
	      $$ = node(variable($1), NODE_SUBSCRIPT, $3);
	  }

	| '$' exp	  %prec UNARY
	  {
	      $$ = node($2, NODE_FIELD_SPEC, NULL);
	  }
	;

/* optional expression, as in for loop */

opt_exp : /* empty */
	  {
	      $$ = NULL;
	  }

	| exp
	  {
	      $$ = $1;
	  }
	;

exp_list :
	  /* empty */
	  {
	      $$ = NULL;
	  }

	| exp
	  {
	      $$ = node($1, NODE_EXPRESSION_LIST, NULL);
	  }

	| exp_list ',' exp
	  {
	      $$ = append_right($1, node($3, NODE_EXPRESSION_LIST, NULL));
	  }
	;

exp	: '-' exp	  %prec UNARY
	  {
	      $$ = node($2, NODE_UNARY_MINUS, NULL);
	  }

	| variable ASSIGNOP exp
	  {
	      $$ = node($1, $2, $3);
	  }

	| builtin

	| '(' exp ')'
	  {
	      $$ = $2;
	  }

	| INCREMENT variable %prec UNARY
	  {
	      $$ = node($2, NODE_PREINCREMENT, NULL);
	  }

	| DECREMENT variable %prec UNARY
	  {
	      $$ = node($2, NODE_PREDECREMENT, NULL);
	  }

	| variable INCREMENT  %prec UNARY
	  {
	      $$ = node($1, NODE_POSTINCREMENT, NULL);
	  }

	| variable DECREMENT  %prec UNARY
	  {
	      $$ = node($1, NODE_POSTDECREMENT, NULL);
	  }

	| variable
	  {
	      $$ = $1;
	  }

	| NUMBER
	  {
	      $$ = make_number($1);
	  }

	| YSTRING
	  {
	      $$ = make_string($1, -1);
	  }

/* Binary operators in order of decreasing precedence.  */
	| exp '^' exp
	  {
	      $$ = node($1, NODE_EXPONENTIAL, $3);
	  }

	| exp '*' exp
	  {
	      $$ = node($1, NODE_TIMES, $3);
	  }

	| exp '/' exp
	  {
	      $$ = node($1, NODE_QUOTIENT, $3);
	  }

	| exp '%' exp
	  {
	      $$ = node($1, NODE_MOD, $3);
	  }

	| exp '+' exp
	  {
	      $$ = node($1, NODE_PLUS, $3);
	  }

	| exp '-' exp
	  {
	      $$ = node($1, NODE_MINUS, $3);
	  }

	  /* Empty operator.  See yylex for disgusting details. */
	| exp CONCAT_OP exp
	  {
	      $$ = node($1, NODE_CONCAT, $3);
	  }

	| conditional '?' exp ':' exp
	  {
	      $$ = node($1, NODE_CONDEXP,
			node($3, NODE_CONDEXP_BRANCHES, $5));
	  }
	;

builtin : LEX_BUILTIN '(' exp_list ')'
	  {
	      $$ = snode($3, NODE_BUILTIN, $1);
	  }

	| LEX_BUILTIN
	  {
	      $$ = snode(NULL, NODE_BUILTIN, $1);
	  }

	| LEX_GETLINE redirect_in
	  {
	      auto     NODE	*tmp;

	      tmp = node(NULL, NODE_GETLINE, $2);
	      $$  = snode(tmp, NODE_BUILTIN, $1);
	  }

	| LEX_GETLINE variable redirect_in
	  {
	      auto     NODE	*tmp;

	      tmp = node($2, NODE_GETLINE, $3);
	      $$  = snode(tmp, NODE_BUILTIN, $1);
	  }

	| LEX_SPLIT_FUNC '(' exp_list ')'
	  {
	      auto     int	num;

	      num = count_arguments($3);
	      if (num < 2 || num > 3)
		  yyerror("Invalid number of arguments for function");
	      $$ = snode($3, NODE_BUILTIN, $1);
	  }

	| LEX_SPLIT_FUNC '(' exp_list ',' regexp ')'
	  {
	      auto     NODE	*tmp;

	      tmp = node($5, NODE_EXPRESSION_LIST, NULL);
	      tmp = append_right($3, tmp);
	      if (count_arguments($3) != 3)
		  yyerror("Invalid number of arguments for function");
	      $$  = snode($3, NODE_BUILTIN, $1);
	  }

	| LEX_MATCH_FUNC '(' exp ',' regexp ')'
	  {
	      auto     NODE	*tmp;

	      tmp = node($5, NODE_EXPRESSION_LIST, NULL);
	      tmp = node($3, NODE_EXPRESSION_LIST, tmp);
	      $$  = snode(tmp, NODE_BUILTIN, $1);
	  }

	| LEX_MATCH_FUNC '(' exp_list ')'
	  {
	      if (count_arguments($3) != 2)
		  yyerror("Invalid number of arguments for function");
	      $$ = snode($3, NODE_BUILTIN, $1);
	  }

	| LEX_SUB_FUNC '(' regexp ',' exp ',' exp ')'
	  {
	      auto     NODE	 *tmp;

	      tmp = node($7, NODE_EXPRESSION_LIST, NULL);
	      tmp = node($5, NODE_EXPRESSION_LIST, tmp);
	      tmp = node($3, NODE_EXPRESSION_LIST, tmp);
	      $$  = snode(tmp, NODE_BUILTIN, $1);
	  }

	| LEX_SUB_FUNC '(' regexp ',' exp ')'
	  {
	      auto     NODE	 *tmp;

	      tmp = make_number((AWKNUM) 0.0);
	      tmp = node(tmp, NODE_FIELD_SPEC, NULL);
	      tmp = node(tmp, NODE_EXPRESSION_LIST, NULL);
	      tmp = node($5,  NODE_EXPRESSION_LIST, tmp);
	      tmp = node($3,  NODE_EXPRESSION_LIST, tmp);
	      $$  = snode(tmp, NODE_BUILTIN, $1);
	  }

	| LEX_SUB_FUNC '(' exp_list ')'
	  {
	      auto     int	num;
	      auto     NODE    *tmp;

	      num = count_arguments($3);
	      if (num < 2 || num > 3)
		  yyerror("Invalid number of arguments for function");
	      if (num == 2)
	      {
		  tmp = make_number((AWKNUM) 0.0);
		  tmp = node(tmp, NODE_FIELD_SPEC, NULL);
		  append_right($3, node(tmp, NODE_EXPRESSION_LIST, NULL));
	      }
	      $$  = snode($3, NODE_BUILTIN, $1);
	  }
	;

%%


struct token
{
    char	  *operator;
    int 	   value;
    int 	   class;
    NODE	 *(PASCAL *ptr)(NODE *);
};



/* Tokentab is sorted ascii ascending order, so it can be binary searched. */
/* DO NOT enter table entries out of order lest search can't find them.    */

static struct token tokentab[] =
{
    { "BEGIN",	   NODE_ILLEGAL,	   LEX_BEGIN,	   NULL        },
    { "END",	   NODE_ILLEGAL,	   LEX_END,	   NULL        },
    { "atan2",	   NODE_BUILTIN,	   LEX_BUILTIN,    do_atan2    },
#ifndef FAST
    { "bp",	   NODE_BUILTIN,	   LEX_BUILTIN,    do_bp       },
#endif
    { "break",	   NODE_K_BREAK,	   LEX_BREAK,	   NULL        },
    { "close",	   NODE_BUILTIN,	   LEX_BUILTIN,    do_close    },
    { "continue",  NODE_K_CONTINUE,	   LEX_CONTINUE,   NULL        },
    { "cos",	   NODE_BUILTIN,	   LEX_BUILTIN,    do_cos      },
    { "delete",    NODE_K_DELETE,	   LEX_DELETE,	   NULL        },
    { "else",	   NODE_ILLEGAL,	   LEX_ELSE,	   NULL        },
    { "exit",	   NODE_K_EXIT, 	   LEX_EXIT,	   NULL        },
    { "exp",	   NODE_BUILTIN,	   LEX_BUILTIN,    do_exp      },
    { "for",	   NODE_K_FOR,		   LEX_FOR,	   NULL        },
    { "getline",   NODE_BUILTIN,	   LEX_GETLINE,    do_getline  },
    { "gsub",	   NODE_BUILTIN,	   LEX_SUB_FUNC,   do_gsub     },
    { "if",	   NODE_K_IF,		   LEX_IF,	   NULL        },
    { "in",	   NODE_ILLEGAL,	   LEX_IN,	   NULL        },
    { "index",	   NODE_BUILTIN,	   LEX_BUILTIN,    do_index    },
    { "int",	   NODE_BUILTIN,	   LEX_BUILTIN,    do_int      },
    { "length",    NODE_BUILTIN,	   LEX_BUILTIN,    do_length   },
    { "log",	   NODE_BUILTIN,	   LEX_BUILTIN,    do_log      },
    { "lower",	   NODE_BUILTIN,	   LEX_BUILTIN,    do_lower    },
    { "match",	   NODE_BUILTIN,	   LEX_MATCH_FUNC, do_match    },
    { "next",	   NODE_K_NEXT, 	   LEX_NEXT,	   NULL        },
    { "print",	   NODE_K_PRINT,	   LEX_PRINT,	   NULL        },
    { "printf",    NODE_K_PRINTF,	   LEX_PRINTF,	   NULL        },
#ifndef FAST
    { "prvars",    NODE_BUILTIN,	   LEX_BUILTIN,    do_prvars   },
#endif
    { "rand",	   NODE_BUILTIN,	   LEX_BUILTIN,    do_rand     },
    { "reverse",   NODE_BUILTIN,	   LEX_BUILTIN,    do_reverse  },
    { "sin",	   NODE_BUILTIN,	   LEX_BUILTIN,    do_sin      },
    { "split",	   NODE_BUILTIN,	   LEX_SPLIT_FUNC, do_split    },
    { "sprintf",   NODE_BUILTIN,	   LEX_BUILTIN,    do_sprintf  },
    { "sqrt",	   NODE_BUILTIN,	   LEX_BUILTIN,    do_sqrt     },
    { "srand",	   NODE_BUILTIN,	   LEX_BUILTIN,    do_srand    },
    { "sub",	   NODE_BUILTIN,	   LEX_SUB_FUNC,   do_sub      },
    { "substr",    NODE_BUILTIN,	   LEX_BUILTIN,    do_substr   },
    { "system",    NODE_BUILTIN,	   LEX_BUILTIN,    do_system   },
    { "upper",	   NODE_BUILTIN,	   LEX_BUILTIN,    do_upper    },
    { "while",	   NODE_K_WHILE,	   LEX_WHILE,	   NULL        }
};

/* Read one token, getting characters through lexptr.  */

STATIC int NEAR PASCAL yylex(void)
{
    register int	     c;
    register int	     namelen;
    register char	    *tokstart;
    static   int	     last_tok_1  = 0;
    static   int	     last_tok_2  = 0;
    static   int	     did_newline = 0; /* JF the grammar insists that */
					      /* actions end with newlines.  */
    auto     int	     do_concat;
    auto     int	     seen_e = 0;      /* These are for numbers	     */
    auto     int	     seen_point = 0;
    auto     int	     next_tab;

retry:
    if(!lexptr)
	return(0);

    if (want_regexp)
    {
	/* there is a potential bug if a regexp is followed by an equal     */
	/* sign: "/foo/=bar" would result in assign_quotient being returned */
	/* as the next token.  Nothing is done about it since it is not     */
	/* valid awk, but maybe something should be done anyway.	    */
	want_regexp = 0;

	tokstart = lexptr;
	while (c = *lexptr++)
	{
	    switch (c)
	    {
		case '\\':
		    if (*lexptr++ == EOS)
		    {
			yyerror ("unterminated regexp ends with \\");
			return(ERROR);
		    }
		    break;
		case '/':		  /* end of the regexp */
		    lexptr--;
		    yylval.sval = tokstart;
		    return(REGEXP);
		case '\n':
		case EOS:
		    yyerror("unterminated regexp");
		    return(ERROR);
	    }
	}
    }
    do_concat	      = want_concat_token;
    want_concat_token = 0;

    if (*lexptr == EOS)
    {
	lexptr = NULL;
	return(NEWLINE);
    }

    /* if lexptr is at white space between two terminal tokens or parens,  */
    /* it is a concatenation operator.					   */
    if (do_concat && (*lexptr == ' ' || *lexptr == '\t'))
    {
	while (*lexptr == ' ' || *lexptr == '\t')
	    lexptr++;
	if (isalnum(*lexptr) || *lexptr == '\"' || *lexptr == '('
			     || *lexptr == '.'	|| *lexptr == '$')
	    return(CONCAT_OP);
    }

    while (*lexptr == ' ' || *lexptr == '\t')
	lexptr++;

    tokstart   = lexptr;	      /* JF */
    last_tok_1 = last_tok_2;
    last_tok_2 = *lexptr;

    switch (c = *lexptr++)
    {
	case 0:
	    return(0);
	case '\n':
	    ++lineno;
	    if (',' == last_tok_1)	 /* BW: allow lines to be continued */
		goto retry;		 /*	at a comma		    */
	    return(NEWLINE);
	case '#':		      /* it's a comment */
	    while (*lexptr != '\n' && *lexptr != EOS)
		lexptr++;
	    goto retry;
	case '\\':
	    if (*lexptr == '\n')
	    {
		++lexptr;
		++lineno;
		goto retry;
	    }
	    break;
	case ')':
	case ']':
	    ++want_concat_token;	     /* intentional fall thru */
	case '(':		 /* JF these were above, but I don't see why */
	case '[':		 /* they should turn on concat. . . &	     */
	case '{':
	case ',':		 /* JF */
	case '$':
	case ';':
	case '?':
	case ':':
	    /* set node type to ILLEGAL because the action should set it to */
	    /* the right thing						    */
	    yylval.nodetypeval = NODE_ILLEGAL;
	    return(c);
	case '^':			       /* added by BW 12-12-88 */
	    if (*lexptr == '=')
	    {
		yylval.nodetypeval = NODE_ASSIGN_EXPONENTIAL;
		++lexptr;
		return(ASSIGNOP);
	    }
	    yylval.nodetypeval = NODE_ILLEGAL;
	    return(c);
	case '*':
	    if (*lexptr == '=')
	    {
		yylval.nodetypeval = NODE_ASSIGN_TIMES;
		++lexptr;
		return(ASSIGNOP);
	    }
	    yylval.nodetypeval = NODE_ILLEGAL;
	    return(c);
	case '/':
	    if (*lexptr == '=')
	    {
		yylval.nodetypeval = NODE_ASSIGN_QUOTIENT;
		++lexptr;
		return(ASSIGNOP);
	    }
	    yylval.nodetypeval = NODE_ILLEGAL;
	    return(c);
	case '%':
	    if (*lexptr == '=')
	    {
		yylval.nodetypeval = NODE_ASSIGN_MOD;
		++lexptr;
		return(ASSIGNOP);
	    }
	    yylval.nodetypeval = NODE_ILLEGAL;
	    return(c);
	case '+':
	    if (*lexptr == '=')
	    {
		yylval.nodetypeval = NODE_ASSIGN_PLUS;
		++lexptr;
		return(ASSIGNOP);
	    }
	    if (*lexptr == '+')
	    {
		yylval.nodetypeval = NODE_ILLEGAL;
		++lexptr;
		return(INCREMENT);
	    }
	    yylval.nodetypeval = NODE_ILLEGAL;
	    return(c);
	case '!':
	    if (*lexptr == '=')
	    {
		yylval.nodetypeval = NODE_ILLEGAL;
		++lexptr;
		return(RELOP_NEQ);
	    }
	    if (*lexptr == '~')
	    {
		yylval.nodetypeval = NODE_NOMATCH;
		++lexptr;
		return(MATCHOP);
	    }
	    yylval.nodetypeval = NODE_ILLEGAL;
	    return(c);
	case '<':
	    yylval.nodetypeval = NODE_ILLEGAL;
	    if (*lexptr == '=')
	    {
		++lexptr;
		return(RELOP_LEQ);
	    }
	    return(c);
	case '=':
	    if (*lexptr == '=')
	    {
		yylval.nodetypeval = NODE_ILLEGAL;
		++lexptr;
		return(RELOP_EQ);
	    }
	    yylval.nodetypeval = NODE_ASSIGN;
	    return(ASSIGNOP);
	case '>':
	    yylval.nodetypeval = NODE_ILLEGAL;
	    if ('>' == *lexptr)
	    {
		++lexptr;
		return(REDIR_APPEND);
	    }
	    if (*lexptr == '=')
	    {
		++lexptr;
		return(RELOP_GEQ);
	    }
	    return(c);
	case '~':
	    yylval.nodetypeval = NODE_MATCH;
	    return(MATCHOP);
	case '}':
	    if (did_newline)
	    {
		did_newline = 0;
		return(c);
	    }
	    ++did_newline;
	    --lexptr;
	    return(NEWLINE);
	case '"':
	    while (*lexptr != EOS)
	    {
		switch (*lexptr++)
		{
		    case '\\':
			if (*lexptr++ != EOS)
			    break;
							/* fall through */
		    case '\n':
			yyerror("unterminated string");
			return(ERROR);
		    case '\"':
			yylval.sval = tokstart + 1;
			++want_concat_token;
			return(YSTRING);
		}
	    }
	    return(ERROR);	    /* JF this was one level up, wrong? */
	case '-':
	    if (*lexptr == '=')
	    {
		yylval.nodetypeval = NODE_ASSIGN_MINUS;
		++lexptr;
		return(ASSIGNOP);
	    }
	    if (*lexptr == '-')
	    {
		yylval.nodetypeval = NODE_ILLEGAL;
		++lexptr;
		return(DECREMENT);
	    }

	    /* JF I think space tab comma and newline are the legal places  */
	    /* for a UMINUS.  Have I missed any?			    */
	    if ((!isdigit(*lexptr) && *lexptr!='.')
				   ||
		(lexptr > lexptr_begin + 1 &&  !index(" \t,\n",lexptr[-2])))
	    {
		/* set node type to ILLEGAL because the action should set   */
		/* it to the right thing				    */
		yylval.nodetypeval = NODE_ILLEGAL;
		return(c);
	    }
					   /* FALL through into number code */
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case '.':
	    namelen = ('-' == c) ? 1 : 0;
	    for (; (c = tokstart[namelen]) != EOS; namelen++)
	    {
		switch (c)
		{
		    case '.':
			if (seen_point)
			    goto got_number;
			++seen_point;
			break;
		    case 'e':
		    case 'E':
			if (seen_e)
			    goto got_number;
			++seen_e;
			if (tokstart[namelen+1] == '-'
				      ||
			    tokstart[namelen+1] == '+')
			    namelen++;
			break;
		    default:
			if (c < '0' || c > '9')
			    goto got_number;
			break;
		}
	    }

got_number:
	    lexptr = tokstart + namelen;
	    c		= *lexptr;		    /* BW: some versions of  */
	    *lexptr	= EOS;			   /*	  atof() are very   */
	    yylval.fval = (AWKNUM) atof(tokstart);  /*	   picky about what  */
	    *lexptr	= c;			    /*	   follows a number. */
	    ++want_concat_token;
	    return(NUMBER);

	case '&':
	    if (*lexptr == '&')
	    {
		yylval.nodetypeval = NODE_AND;
		++lexptr;
		return(LEX_AND);
	    }
	    return(ERROR);
	case '|':
	    if (*lexptr == '|')
	    {
		yylval.nodetypeval = NODE_OR;
		++lexptr;
		return(LEX_OR);
	    }
	    return(ERROR);
    }

    if (!isalpha(c))
    {
	yyerror("Invalid char '%c' in expression\n", c);
	return(ERROR);
    }

    /* its some type of name-type-thing.  Find its length */
    for (namelen = 0; is_identchar(tokstart[namelen]); namelen++)
	;

    /* See if it is a special token.  binary search added by BW */
    {
	register int	    x;
	auto	 char	    tok1;
	auto	 int	    l = 0, r = MAXDIM(tokentab) - 1;

	do
	{
	    x = (l + r) / 2;
	    tok1 = tokentab[x].operator[0];
	    if (tok1 == *tokstart)
	    {
		tok1 = memcmp(tokstart, tokentab[x].operator, namelen);
		if (0 == tok1 && EOS == tokentab[x].operator[namelen])
		    break;
		if (tok1 > 0)
		    l = x + 1;
		else
		    r = x - 1;
	    }
	    else
	    {
		if (*tokstart < tok1)
		    r = x - 1;
		else
		    l = x + 1;
	    }
	} while (l <= r);
	if (l <= r)
	{
	    lexptr = tokstart + namelen;
	    if (NODE_BUILTIN == tokentab[x].value)
		yylval.ptrval = tokentab[x].ptr;
	    else
		yylval.nodetypeval = tokentab[x].value;
	    return(tokentab[x].class);
	}
    }

    /* It's a name.  See how long it is.  */
    yylval.sval = tokstart;
    lexptr = tokstart+namelen;
    ++want_concat_token;
    return(NAME);
}


void yyerror(char *mesg, ...)
{
    register char	 *ptr, *beg;
    auto     va_list	  ap;
    auto     int	  colno = 0;
    auto     int	  i, next_tab;

    /* Find the current line in the input file */
    if (!lexptr)
    {
	beg   = "(END OF FILE)";
	ptr   = beg + strlen(beg);
    }
    else
    {
	if (*lexptr == '\n' && lexptr != lexptr_begin)
	    --lexptr;
	for (beg = lexptr; beg != lexptr_begin && *beg != '\n'; --beg)
	    ;
	for (ptr = lexptr; *ptr && *ptr != '\n'; ptr++)
	    ;
	if (beg != lexptr_begin)
	    ++beg;
	i = 0;
	while (beg + i <= lexptr)
	{
	    if ('\t' == beg[i])
	    {
		next_tab = colno % TABSTOPS;
		colno	+= (next_tab ? next_tab : TABSTOPS);
	    }
	    else
		++colno;
	    ++i;
	}
    }

    fprintf(stderr, "\n'%.*s'\n", ptr - beg, beg);
    fprintf(stderr, "%*s\n", colno + 1, "^");

    /* figure out line number, etc. later */
    va_start(ap, mesg);
    vfprintf(stderr, mesg, ap);
    va_end(ap);
    fprintf(stderr, " near line %d column %d\n", lineno, colno);
    exit(1);
}


/* Parse a C escape sequence.  STRING_PTR points to a variable
   containing a pointer to the string to parse.  That pointer
   is updated past the characters we use.  The value of the
   escape sequence is returned.

   A negative value means the sequence \ newline was seen,
   which is supposed to be equivalent to nothing at all.

   If \ is followed by a null character, we return a negative
   value and leave the string pointer pointing at the null character.

   If \ is followed by 000, we return 0 and leave the string pointer
   after the zeros.  A value of 0 does not mean end of string.  */

STATIC int NEAR PASCAL parse_escape(char **string_ptr)
{
    register int	  c = *(*string_ptr)++;

    switch (c)
    {
	case 'a':
	    return('\a');
	case 'b':
	    return('\b');
	case 'e':
	    return(033);
	case 'f':
	    return('\f');
	case 'n':
	    return('\n');
	case 'r':
	    return('\r');
	case 't':
	    return('\t');
	case 'v':
	    return('\v');
	case '\n':
	    return(-2);
	case 0:
	    (*string_ptr)--;
	    return(0);
	case '^':
	    c = *(*string_ptr)++;
	    if (c == '\\')
		c = parse_escape(string_ptr);
	    if (c == '?')
		return(0177);
	    return((c & 0200) | (c & 037));
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	    {
		register int	  i = c - '0';
		register int	  count = 0;

		while (++count < 3)
		{
		    if ((c = *(*string_ptr)++) >= '0' && c <= '7')
		    {
			i *= 8;
			i += c - '0';
		    }
		    else
		    {
			(*string_ptr)--;
			break;
		    }
		}
		return(i);
	    }
	default:
	    return(c);
    }
}
