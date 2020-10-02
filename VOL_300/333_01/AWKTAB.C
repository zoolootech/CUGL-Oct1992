/*
 * Created by CSD YACC (IBM PC) from "AWKTAB.Y" */

/* #line 31 "AWKTAB.Y" */
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


/* #line 71 "AWKTAB.Y" */
typedef union 
{
    long	  lval;
    AWKNUM	  fval;
    NODE	 *nodeval;
    int 	  nodetypeval;
    char	 *sval;
    NODE	*(PASCAL *ptrval)(NODE *);
} YYSTYPE;
#define NAME 257
#define REGEXP 258
#define YSTRING 259
#define ERROR 260
#define INCDEC 261
#define NUMBER 262
#define ASSIGNOP 263
#define MATCHOP 264
#define NEWLINE 265
#define CONCAT_OP 266
#define LEX_BEGIN 267
#define LEX_END 268
#define LEX_IF 269
#define LEX_ELSE 270
#define LEX_WHILE 271
#define LEX_FOR 272
#define LEX_BREAK 273
#define LEX_CONTINUE 274
#define LEX_DELETE 275
#define LEX_PRINT 276
#define LEX_PRINTF 277
#define LEX_NEXT 278
#define LEX_EXIT 279
#define RELOP_EQ 280
#define RELOP_GEQ 281
#define RELOP_LEQ 282
#define RELOP_NEQ 283
#define REDIR_APPEND 284
#define LEX_IN 285
#define LEX_AND 286
#define LEX_OR 287
#define INCREMENT 288
#define DECREMENT 289
#define LEX_BUILTIN 290
#define LEX_MATCH_FUNC 291
#define LEX_SUB_FUNC 292
#define LEX_SPLIT_FUNC 293
#define LEX_GETLINE 294
#define UNARY 295
#define yyclearin yychar = -1
#define yyerrok yyerrflag = 0
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
YYSTYPE yylval;         /*CSD & DECUS LEX */
YYSTYPE yyval;          /*CSD & DECUS LEX */
#define YYERRCODE 256

/* #line 713 "AWKTAB.Y" */



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

short yyexca[] =
{
-1, 1,
	0, -1,
	-2, 0,
-1, 3,
	0, 1,
	-2, 5,

};
#define YYNPROD			108
#define YYLAST        1044

short yyact[] =
{
  10,  57,  33,  23, 204, 171, 198,  11, 159, 224,
 127,  35,  14,  81,  21,  75,  74, 111, 110, 146,
 212,  63,   4,  32,  35, 154,  58,  59,  35,  72,
  96, 171, 214,  23, 179, 155, 129, 109,  10,  31,
   2,  23,  35,  46, 146,  11,  81,  23,  44,  47,
  14,  48,  21,  45,  23, 177,  43,  68,  21,  81,
  68,  63, 186,  21,  13, 146,  50,  66,  49,  23,
 130, 195, 193, 137,  81,  81,  46,  10, 225, 185,
  23,  44, 138, 221,  11,  81,  45, 165, 220,  14,
 113,  21, 181, 136, 180, 208,  81,  82, 206, 134,
  43,  35, 133, 205, 132,  23,  86,  81, 131,  80,
 157, 143,  81, 108, 142,  10,  71,  70,  23,  35,
  69,  65,  11,  62,  42,  23, 114,  14, 113,  21,
 184,  10,  16,  43,  23, 100, 106,   5,  11,  30,
   6,  29,   3,  14,   1,  21,   0,   0,   0,   0,
   0,   0,   0,   0,  10,   0, 170,  23,   0,   0,
   0,  11,   0,   0,   0,   0,  14, 113,  21, 139,
  10,   0,   0,  23,   0,   0,   0,  11,  10, 163,
 203,  23,  14,   0,  21,  11, 168, 169,   0,   0,
  14,   0,  21,   0,  10,   0, 187,  23, 160,   0,
 161,  11,   0,   0,   0,   0,  14,   0,  21,   0,
   0,   0,   0,   0,   0,   0,  10,   0,   0,  23,
   0,   0,   0, 151,  22, 145,  20, 172,  14,  19,
  21,   0, 111, 110,  33,  34, 125,   0, 115, 116,
 117, 118, 123, 119, 120, 121, 122,  33,  34,   0,
 145,  33,  34, 172,  22,  17,  18,  24,  27,  28,
  26,  25,  22,   0,  20,  33,  34,  19,  22,   0,
  41, 145,  40,   0, 125,  22, 115, 116, 117, 118,
 123, 119, 120, 121, 122,   0,  51,  52,  53,  54,
  22, 213, 191,  17,  18,  24,  27,  28,  26,  25,
 219,  22,   0,  20,  46,   0,  19, 174, 183,  44,
  47, 182,  48, 125,  45, 115, 116, 117, 118, 123,
 119, 120, 121, 122,  33,  34,  22,  50, 189,  49,
   0,   0,  17,  18,  24,  27,  28,  26,  25,  22,
   0,  20,  33,  34,  19,   0,  22,   4, 101,   7,
   8,   0,   0,   0,   0,  22,   0,  20,   0,   0,
  19,  43, 215, 216,   0,   7,   8,   0,   0,   0,
  17,  18,  24,  27,  28,  26,  25,   0,  22,   0,
  20,   0,   0,  19,   0,   0,  17,  18,  24,  27,
  28,  26,  25,   0,  22,   0,  20,   0,   0,  19,
   0,   0,  22,   0,  20,   0,   0,  19, 112,  17,
  18,  24,  27,  28,  26,  25,   0,   0, 166,   0,
  20,   0,  84,  19,   0,  17,  18,  24,  27,  28,
  26,  25,   0,  17,  18,  24,  27,  28,  26,  25,
  22,   0,  20,   0,  73,  19,   0,   0,   0,  17,
  18,  24,  27,  28,  26,  25,  10,   0,   0,  23,
   0,   0,   0, 149,   0,   0,   0,   0,  14,   0,
  21,  17,  18,  24,  27,  28,  26,  25,  46,   0,
   0,   0, 210,  44,  47,   0,  48,   0,  45,   0,
   0,   0, 144,   0,   0,   0,   0,   0,   0,   0,
  46,  50,   0,  49, 196,  44,  47,  46,  48,   0,
  45,   0,  44,  47, 135,  48,   0,  45, 140,   0,
   0,   0,   0,  50,   0,  49,  37,   0,   0,   0,
  50,  41,  49,  40,   0,  43,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,  46,  51,  52,  53,
  54,  44,  47,   0,  48,  46,  45,  43, 141,  83,
  44,  47,   0,  48,  43,  45,   0, 126,   0,  50,
   0,  49, 140,   0,   0,   0,  46,   0,  50,   0,
  49,  44,  47,  46,  48,   0,  45,   0,  44,  47,
  97,  48,  99,  45,   0,   0, 103, 105, 107,  50,
   0,  49,   0,  43,   0,   0,  50,   0,  49,   0,
 211, 147,  43,   0, 152, 153,   0, 156, 218,   0,
  56,   0,   0,   9,   9,   0,   0,   0,   0, 200,
 201,  36,  38,  43,   0, 230, 231, 232, 233,   0,
  43,   0,   0, 209,   0,   0, 148, 150,   0,   0,
   0,   0,   0,  76,  77,  78,   0,   0,   0,   0,
   0,   0,   0, 188,   0, 226, 227, 194,   0, 228,
 229,   0,   0,   0,   0,   0, 173,   0, 175,   0,
  22,   0,  20,   0,   0,  19,   0,   0,   0, 207,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0, 217,   0,   0, 197,  41,   0,  40, 222, 223,
   0,  17,  18,  24,  27,  28,  26,  25,   0,   0,
   0,  51,  52,  53,  54,   0,   0,  41,   0,  40,
   0,   0,   0,   0,  41,   0,  40,   0,   0,   0,
   0,   0,   0,  51,  52,  53,  54,   0,   0,   0,
  51,  52,  53,  54,   0,   0,   0,   0,   0,   0,
   0,   0,  15, 164,   0,   0,   0,   0,   0,   0,
  38,   0,  38,  41,   0,  40,   0,   0, 178,   0,
  60,  61,  41,   0,  40,   0,   0,   0,  67,  51,
  52,  53,  54,   0,   0,   0,   0,   0,  51,  52,
  53,  54,   0,  41,  87,  40,   0, 202,   0,   0,
  41,   0,   0,   0,   0,   0,   0,   0,   0,  51,
  52,  53,  54,   0,   0,   0,  51,  52,  53,  54,
 124, 102,   0,  12,  12,   0,   0,   0,   0,   0,
   0,  12,  39,   0,   0,  55,   0,   0,   0,   0,
   0,   0,   0,   0,  64,   0,   0,   0,   0,   0,
   0,   0,   0,  12,  12,  12,  79,   0,   0,   0,
   0,  85,   0,  88,  89,  90,  91,  92,  93,  94,
   0,   0,   0,   0,   0,   0,   0,   0,  95,   0,
   0,   0,   0,   0,  98,   0,  98,   0,   0,   0,
  98, 104,  98,   0,   0,   0,   0,   0,   0,   0,
   0,   0, 128,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0, 190, 192,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
  98,  98,   0,   0,   0,   0,   0, 158,   0,   0,
   0,   0,   0,   0, 128,   0,   0,   0, 162,   0,
   0,   0,   0,  12, 167,   0,   0,   0,   0,   0,
  39,   0,  39,   0,   0, 176,   0,   0,  12,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,  98,   0,
   0,   0,   0, 199,   0,   0,   0,  12,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0, 167,   0,   0,   0,   0,   0,
   0,   0,   0, 167
};

short yypact[] =
{
-1000,-1000,  82,  98,-1000,-1000, -84,-1000,-1000, -21,
 145, 145, 539,-1000, 145,-262,-1000,  18,  18,-1000,
-1000,-1000, -30, 145,  81,  -3,  80,  77,  76,-1000,
-236,-250, 145, 145, 145, 145,-1000,  68,  56, 518,
 137,  11, 145, 145, 145, 145, 145, 145, 145,-1000,
-1000,-1000,-1000,-1000,-1000, -38, -52, 145,-1000,-1000,
-1000,-1000,-228, 145, -38, 145,-1000,   0,  89, 145,
 145, 145,-1000, -33,-1000,-1000, -52,-1000,-284, 509,
-256, 145,-1000,-1000,-221, 546,-1000,-1000, 539, -38,
 -38, -38, -38,  39,  39, 539,  23,  15, 539,  63,
-1000,-1000,-1000,  58, 470,  52,  29,  41,-243,  44,
-1000,-1000,-1000,-250,-1000,  74,  71, -40, -40, 423,
 183, -40, -15,-222,   6,  70, 145,-277, 539,-1000,
-1000,-1000,-1000, 145,-1000,  16,-1000, 145,-1000,-1000,
-1000, -33, 145, 161,-1000,-1000,-1000,-1000, -31, 145,
 -31, 145,-1000,-1000, 145, -36,-1000, 145, 539,-223,
  53,  51, 267,   5,  38,   3, -70, 539,-243,-243,
 -40,  69,  33,  31, -40,  30, 463, 145, -35,-1000,
-1000,-1000, 145,-1000,-250,-250, 121,-281,-1000,-1000,
-1000,-1000,-1000,-256,-1000,-256, -40,   2,-250, 441,
-248, -33, -39, 145,-225, -57, -57,-1000, -40, -33,
-1000,-1000, 145,  47,  42, -40, -40,-1000,-261,  37,
-250,-250,-1000,-1000,-250,-250, -33, -33, -33, -33,
-1000,-1000,-1000,-1000
};

short yypgo[] =
{
   0, 830, 144, 142, 137, 140, 620,  64, 139, 762,
  67, 156, 526, 132,  37, 408, 126,  87, 444, 124,
  40, 492, 123, 103,  98
};
short yyr1[]={

   0,   2,   3,   3,   4,   5,   5,   5,   5,   5,
   6,   6,   6,   6,   6,   6,   6,   6,   6,   6,
   8,   8,  14,  14,  21,  21,  22,   7,  19,  19,
  19,  19,  19,  19,  18,  18,  18,  18,  18,  15,
  15,  15,  15,  15,  15,  15,  15,  15,  23,  15,
  15,  24,  15,  15,  15,  15,  15,  15,  16,  16,
  20,  20,  11,  11,  11,  11,  11,  10,  10,  10,
   9,   9,   9,  17,  17,  12,  12,  12,   1,   1,
   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
   1,   1,   1,   1,   1,   1,   1,  13,  13,  13,
  13,  13,  13,  13,  13,  13,  13,  13 };
short yyr2[]={

   0,   2,   1,   2,   4,   0,   1,   1,   1,   3,
   2,   6,   4,   3,   3,   3,   1,   3,   3,   3,
   0,   4,   1,   2,   2,   2,   0,   4,   1,   1,
   1,   1,   1,   1,   0,   1,   1,   2,   2,   5,
   1,   6,  10,   9,   9,   2,   2,   4,   0,   7,
   4,   0,   7,   2,   2,   5,   6,   2,   6,   9,
   0,   2,   0,   2,   2,   2,   2,   0,   2,   2,
   1,   4,   2,   0,   1,   0,   1,   3,   2,   3,
   1,   3,   2,   2,   2,   2,   1,   1,   1,   3,
   3,   3,   3,   3,   3,   3,   5,   4,   1,   2,
   3,   4,   6,   6,   4,   8,   6,   4 };
short yychk[]={

-1000,  -2, -20,  -3, 265,  -4,  -5, 267, 268,  -6,
  33,  40,  -1,  -7,  45,  -9, -13, 288, 289, 262,
 259,  47, 257,  36, 290, 294, 293, 291, 292,  -4,
  -8, 123,  44, 286, 287,  63,  -6, -12,  -6,  -1,
 266, 264, -19,  94,  42,  47,  37,  43,  45,  62,
  60, 280, 281, 282, 283,  -1,  -6, 263, 288, 289,
  -9,  -9, -22,  91,  -1,  40, -10,  -9,  60,  40,
  40,  40, 265, -18, 266, 265,  -6,  -6,  -6,  -1,
  41,  44,  41,  41, 285,  -1,  -7,  -9,  -1,  -1,
  -1,  -1,  -1,  -1,  -1,  -1, 258, -12,  -1, -12,
 -10, 259,  -9, -12,  -1, -12,  -7, -12, -20, -14,
 266, 265, -15, 123, -16, 271, 272, 273, 274, 276,
 277, 278, 279, 275,  -1, 269,  58, 266,  -1, 257,
  47,  93,  41,  44,  41,  44,  41,  44,  41, 125,
 -15, -18,  40,  40, -21, 265,  59, -21, -12,  40,
 -12,  40, -21, -21,  40, 257, -21,  40,  -1, 285,
  -7,  -7,  -1, -14,  -6, -17, 257,  -1, -20, -20,
 -11,  62, 284, -12, -11, -12,  -1,  91,  -6, 257,
  41,  41,  44,  41, 125,  41,  59, 266, -21, 259,
  -9, 259,  -9,  41, -21,  41,  41, -12,  41,  -1,
 -18, -18,  -6,  59, 285, -23, -24, -21,  93, -18,
  41, -15,  59, -17, 257, -11, -11, -21, -15, -17,
  41,  41, -21, -21, 270,  41, -18, -18, -18, -18,
 -15, -15, -15, -15 };
short yydef[]={

  60,  -2,   5,  -2,  61,   2,  20,   6,   7,   8,
   0,  75,   0,  16,   0,  86,  80,   0,   0,  87,
  88,  26,  70,   0,  98,  67,   0,   0,   0,   3,
   0,  34,   0,   0,   0,   0,  10,   0,   0,  76,
   0,   0,   0,   0,   0,   0,   0,   0,   0,  28,
  29,  30,  31,  32,  33,  78,   0,   0,  84,  85,
  82,  83,   0,  75,  72,  75,  99,  67,   0,  75,
  75,  75,  60,   0,  35,  36,   9,  13,  14,   0,
   0,   0,  15,  81,   0,  95,  17,  18,  19,  89,
  90,  91,  92,  93,  94,  79,   0,   0,  76,   0,
 100,  68,  69,   0,  76,   0,  16,   0,   4,   0,
  37,  38,  22,  34,  40,   0,   0,   0,   0,  75,
  75,   0,   0,   0,   0,   0,   0,   0,  77,  12,
  27,  71,  97,   0, 101,   0, 104,   0, 107,  21,
  23,   0,   0,  73,  45,  60,  60,  46,  62,  75,
  62,  75,  53,  54,   0,   0,  57,   0,  96,   0,
  16,   0,   0,   0,   0,   0,  70,  74,  24,  25,
   0,   0,   0,   0,   0,   0,   0,  75,   0,  11,
 102, 103,   0, 106,  34,  34,   0,   0,  47,  63,
  64,  65,  66,  48,  50,  51,   0,   0,  34,   0,
  39,   0,   0,  73,   0,  62,  62,  55,   0,   0,
 105,  41,  73,   0,   0,   0,   0,  56,  58,   0,
  34,  34,  49,  52,  34,  34,   0,   0,   0,   0,
  43,  44,  59,  42 };
/*
  TITLE:	 YACC - Yet Another Compilier-Compilier
  VERSION:	 1.0 for IBM-PC
  DATE: 	 JAN 28, 1985
  DESCRIPTION:	 LALR(1) Parser Generator. From UNIX
*/

#define YYFLAG		      -1000
#define YYERROR 	      goto yyerrlab
#define YYACCEPT	      return(0)
#define YYABORT 	      return(1)

/*      parser for yacc output  */

int			  yydebug = 0;	    /* 1 for debugging		   */
YYSTYPE 		  yyv[YYMAXDEPTH];  /* where the values are stored */
int			  yychar = -1;	    /* current input token number  */
int			  yynerrs = 0;	    /* number of errors 	   */
short			  yyerrflag = 0;    /* error recovery flag	   */

int yyparse(void)
{

    short           yys[YYMAXDEPTH];
    short           yyj, yym;
    register YYSTYPE *yypvt;
    register short  yystate, *yyps, yyn;
    register YYSTYPE *yypv;
    register short *yyxi;

    yystate = 0;
    yychar = -1;
    yynerrs = 0;
    yyerrflag = 0;
    yyps = &yys[-1];
    yypv = &yyv[-1];

yystack:			/* put a state and value onto the stack */
    if (yydebug)
	printf("state %d\n", yystate);
    if (++yyps > &yys[YYMAXDEPTH])
    {
	yyerror("yacc stack overflow");
	return(1);
    }
    *yyps = yystate;
    ++yypv;
#ifdef UNION
    yyunion(yypv, &yyval);
#else
    *yypv = yyval;
#endif

yynewstate:
    yyn = yypact[yystate];

    if (yyn <= YYFLAG)
	goto yydefault;		/* simple state */

    if (yychar < 0)
    {
	if ((yychar = yylex()) < 0)
	    yychar = 0;
	if (yydebug)
	    printf("LEX token %d [%c]\n", yychar,
		   yychar >= ' ' && yychar <= 'z' ? yychar : ' ');
    }
    if ((yyn += yychar) < 0 || yyn >= YYLAST)
	goto yydefault;

    if (yychk[yyn = yyact[yyn]] == yychar)
    {
	/* valid shift */
	yychar = -1;
#ifdef UNION
	yyunion(&yyval, &yylval);
#else
	yyval = yylval;
#endif
	yystate = yyn;
	if (yyerrflag > 0)
	    --yyerrflag;
	goto yystack;
    }
yydefault:
    /* default state action */

    if ((yyn = yydef[yystate]) == -2)
    {
	if (yychar < 0)
	{
	    if ((yychar = yylex()) < 0)
		yychar = 0;
	    if (yydebug)
		printf("LEX token %d [%c]\n", yychar,
		       yychar >= ' ' && yychar <= 'z' ? yychar : ' ');
	}
	/* look through exception table */

	for (yyxi = yyexca; (*yyxi != (-1)) || (yyxi[1] != yystate); yyxi += 2);	/* VOID */

	for (yyxi += 2; *yyxi >= 0; yyxi += 2)
	{
	    if (*yyxi == yychar)
		break;
	}
	if ((yyn = yyxi[1]) < 0)
	    return (0);		/* accept */
    }

    if (yyn == 0)
    {
	/* error */
	/* error ... attempt to resume parsing */

	switch (yyerrflag)
	{

	case 0:		/* brand new error */

	    yyerror("syntax error");
yyerrlab:
	    ++yynerrs;

	case 1:
	case 2:		/* incompletely recovered error ... try again */

	    yyerrflag = 3;

	    /* find a state where "error" is a legal shift action */

	    while (yyps >= yys)
	    {
		yyn = yypact[*yyps] + YYERRCODE;
		if (yyn >= 0 && yyn < YYLAST && yychk[yyact[yyn]] == YYERRCODE)
		{
		    yystate = yyact[yyn];	/* simulate a shift of
						 * "error" */
		    goto yystack;
		}
		yyn = yypact[*yyps];

		/* the current yyps has no shift onn "error", pop stack */

		if (yydebug)
		    printf("error recovery pops state %d, uncovers %d\n", *yyps, yyps[-1]);
		--yyps;
		--yypv;
	    }

	    /* there is no state on the stack with an error shift ... abort */

yyabort:
	    return(1);

	case 3:		/* no shift yet; clobber input char */

	    if (yydebug)
		printf("error recovery discards char %d\n", yychar);

	    if (yychar == 0)
		goto yyabort;	/* don't discard EOF, quit */
	    yychar = -1;
	    goto yynewstate;	/* try again in the same state */

	}

    }

    /* reduction by production yyn */

    if (yydebug)
	printf("reduce %d\n", yyn);
    yyps -= yyr2[yyn];
    yypvt = yypv;
    yypv -= yyr2[yyn];
#ifdef UNION
    yyunion(&yyval, &yypv[1]);
#else
    yyval = yypv[1];
#endif
    yym = yyn;
    /* consult goto table to find next state */
    yyn = yyr1[yyn];
    yyj = yypgo[yyn] + *yyps + 1;
    if (yyj >= YYLAST || yychk[yystate = yyact[yyj]] != -yyn)
	yystate = yyact[yypgo[yyn]];
    switch (yym)
    {
	
case 1:
/* #line 116 "AWKTAB.Y" */
{
	      expression_value = yypvt[-0].nodeval;
	  } break;
case 2:
/* #line 123 "AWKTAB.Y" */
{
	      yyval.nodeval = node(yypvt[-0].nodeval, NODE_RULE_LIST, NULL);
	  } break;
case 3:
/* #line 128 "AWKTAB.Y" */
{	       /* cons the rule onto the tail of list */
	      yyval.nodeval = append_right(yypvt[-1].nodeval, node(yypvt[-0].nodeval, NODE_RULE_LIST, NULL));
	  } break;
case 4:
/* #line 134 "AWKTAB.Y" */
{
	      yyval.nodeval = node(yypvt[-3].nodeval, NODE_RULE_NODE, yypvt[-2].nodeval);
	  } break;
case 5:
/* #line 141 "AWKTAB.Y" */
{
	      yyval.nodeval = NULL;
	  } break;
case 6:
/* #line 146 "AWKTAB.Y" */
{
	      yyval.nodeval = node(NULL, NODE_K_BEGIN, NULL);
	  } break;
case 7:
/* #line 151 "AWKTAB.Y" */
{
	      yyval.nodeval = node(NULL, NODE_K_END, NULL);
	  } break;
case 8:
/* #line 155 "AWKTAB.Y" */
{
	      yyval.nodeval = yypvt[-0].nodeval;
	  } break;
case 9:
/* #line 160 "AWKTAB.Y" */
{
	      yyval.nodeval = mkrangenode(node(yypvt[-2].nodeval, NODE_COND_PAIR, yypvt[-0].nodeval));
	  } break;
case 10:
/* #line 168 "AWKTAB.Y" */
{
	      yyval.nodeval = node(yypvt[-0].nodeval, NODE_NOT, NULL);
	  } break;
case 11:
/* #line 173 "AWKTAB.Y" */
{
	      yyval.nodeval = node(variable(yypvt[-0].sval), NODE_MEMBER_COND, yypvt[-4].nodeval);
	  } break;
case 12:
/* #line 178 "AWKTAB.Y" */
{
	      yyval.nodeval = node(variable(yypvt[-0].sval), NODE_MEMBER_COND, yypvt[-3].nodeval);
	  } break;
case 13:
/* #line 183 "AWKTAB.Y" */
{
	      yyval.nodeval = node(yypvt[-2].nodeval, NODE_AND, yypvt[-0].nodeval);
	  } break;
case 14:
/* #line 188 "AWKTAB.Y" */
{
	      yyval.nodeval = node (yypvt[-2].nodeval, NODE_OR, yypvt[-0].nodeval);
	  } break;
case 15:
/* #line 193 "AWKTAB.Y" */
{
	      yyval.nodeval = yypvt[-1].nodeval;
	      want_concat_token = 0;
	  } break;
case 16:
/* #line 199 "AWKTAB.Y" */
{
	      yyval.nodeval = yypvt[-0].nodeval;
	  } break;
case 17:
/* #line 204 "AWKTAB.Y" */
{
	      yyval.nodeval = node(yypvt[-2].nodeval, yypvt[-1].nodetypeval, yypvt[-0].nodeval);
	  } break;
case 18:
/* #line 209 "AWKTAB.Y" */
{
	      yyval.nodeval = node(yypvt[-2].nodeval, yypvt[-1].nodetypeval, yypvt[-0].nodeval);
	  } break;
case 19:
/* #line 214 "AWKTAB.Y" */
{
	      yyval.nodeval = node(yypvt[-2].nodeval, yypvt[-1].nodetypeval, yypvt[-0].nodeval);
	  } break;
case 20:
/* #line 220 "AWKTAB.Y" */
{
	      yyval.nodeval = NULL;
	  } break;
case 21:
/* #line 225 "AWKTAB.Y" */
{
	      yyval.nodeval = yypvt[-1].nodeval;
	  } break;
case 22:
/* #line 233 "AWKTAB.Y" */
{
	      yyval.nodeval = node(yypvt[-0].nodeval, NODE_STATEMENT_LIST, NULL);
	  } break;
case 23:
/* #line 238 "AWKTAB.Y" */
{
	      yyval.nodeval = append_right(yypvt[-1].nodeval, node(yypvt[-0].nodeval, NODE_STATEMENT_LIST, NULL));
	  } break;
case 24:
/* #line 245 "AWKTAB.Y" */
{
	      yyval.nodetypeval = NODE_ILLEGAL;
	  } break;
case 25:
/* #line 250 "AWKTAB.Y" */
{
	      yyval.nodetypeval = NODE_ILLEGAL;
	  } break;
case 26:
/* #line 256 "AWKTAB.Y" */
{
	      ++want_regexp;
	  } break;
case 27:
/* #line 260 "AWKTAB.Y" */
{
	      want_regexp = 0;
	      yyval.nodeval = node(NULL, NODE_REGEXP, (NODE *) make_regexp(yypvt[-1].sval));
	  } break;
case 28:
/* #line 266 "AWKTAB.Y" */
{
	      yyval.nodetypeval = NODE_GREATER;
	  } break;
case 29:
/* #line 270 "AWKTAB.Y" */
{
	      yyval.nodetypeval = NODE_LESS;
	  } break;
case 30:
/* #line 274 "AWKTAB.Y" */
{
	      yyval.nodetypeval = NODE_EQUAL;
	  } break;
case 31:
/* #line 278 "AWKTAB.Y" */
{
	      yyval.nodetypeval = NODE_GEQ;
	  } break;
case 32:
/* #line 282 "AWKTAB.Y" */
{
	      yyval.nodetypeval = NODE_LEQ;
	  } break;
case 33:
/* #line 286 "AWKTAB.Y" */
{
	      yyval.nodetypeval = NODE_NOTEQUAL;
	  } break;
case 34:
/* #line 293 "AWKTAB.Y" */
{
	      yyval.nodetypeval = NODE_ILLEGAL;
	  } break;
case 39:
/* #line 305 "AWKTAB.Y" */
{
	      yyval.nodeval = yypvt[-2].nodeval;
	  } break;
case 40:
/* #line 310 "AWKTAB.Y" */
{
	      yyval.nodeval = yypvt[-0].nodeval;
	  } break;
case 41:
/* #line 315 "AWKTAB.Y" */
{
	      yyval.nodeval = node(yypvt[-3].nodeval, NODE_K_WHILE, yypvt[-0].nodeval);
	  } break;
case 42:
/* #line 320 "AWKTAB.Y" */
{
	      yyval.nodeval = node(yypvt[-0].nodeval, NODE_K_FOR, (NODE *) make_for_loop(yypvt[-7].nodeval, yypvt[-5].nodeval, yypvt[-3].nodeval));
	  } break;
case 43:
/* #line 325 "AWKTAB.Y" */
{
	      yyval.nodeval = node(yypvt[-0].nodeval, NODE_K_FOR,
			(NODE *) make_for_loop(yypvt[-6].nodeval, NULL, yypvt[-3].nodeval));
	  } break;
case 44:
/* #line 331 "AWKTAB.Y" */
{
	      yyval.nodeval = node(yypvt[-0].nodeval, NODE_K_ARRAYFOR,
			(NODE *) make_for_loop(variable(yypvt[-6].sval),
					       NULL, variable(yypvt[-3].sval)));
	  } break;
case 45:
/* #line 339 "AWKTAB.Y" */
{
	      yyval.nodeval = node(NULL, NODE_K_BREAK, NULL);
	  } break;
case 46:
/* #line 345 "AWKTAB.Y" */
{
	      yyval.nodeval = node(NULL, NODE_K_CONTINUE, NULL);
	  } break;
case 47:
/* #line 350 "AWKTAB.Y" */
{
	      yyval.nodeval = node(yypvt[-2].nodeval, NODE_K_PRINT, yypvt[-1].nodeval);
	  } break;
case 48:
/* #line 355 "AWKTAB.Y" */
{
	      want_concat_token = FALSE;
	  } break;
case 49:
/* #line 359 "AWKTAB.Y" */
{
	      yyval.nodeval = node(yypvt[-4].nodeval, NODE_K_PRINT, yypvt[-1].nodeval);
	  } break;
case 50:
/* #line 364 "AWKTAB.Y" */
{
	      yyval.nodeval = node(yypvt[-2].nodeval, NODE_K_PRINTF, yypvt[-1].nodeval);
	  } break;
case 51:
/* #line 369 "AWKTAB.Y" */
{
	      want_concat_token = FALSE;
	  } break;
case 52:
/* #line 373 "AWKTAB.Y" */
{
	      yyval.nodeval = node(yypvt[-4].nodeval, NODE_K_PRINTF, yypvt[-1].nodeval);
	  } break;
case 53:
/* #line 378 "AWKTAB.Y" */
{
	      yyval.nodeval = node(NULL, NODE_K_NEXT, NULL);
	  } break;
case 54:
/* #line 383 "AWKTAB.Y" */
{
	      yyval.nodeval = node(NULL, NODE_K_EXIT, NULL);
	  } break;
case 55:
/* #line 388 "AWKTAB.Y" */
{
	      yyval.nodeval = node(yypvt[-2].nodeval, NODE_K_EXIT, NULL);
	  } break;
case 56:
/* #line 393 "AWKTAB.Y" */
{
	      yyval.nodeval = node(variable(yypvt[-4].sval), NODE_K_DELETE, yypvt[-2].nodeval);
	  } break;
case 57:
/* #line 398 "AWKTAB.Y" */
{
	      yyval.nodeval = yypvt[-1].nodeval;
	  } break;
case 58:
/* #line 406 "AWKTAB.Y" */
{
	      yyval.nodeval = node(yypvt[-3].nodeval, NODE_K_IF,
			node(yypvt[-0].nodeval, NODE_IF_BRANCHES, NULL));
	  } break;
case 59:
/* #line 413 "AWKTAB.Y" */
{
	      yyval.nodeval = node(yypvt[-6].nodeval, NODE_K_IF,
			node(yypvt[-3].nodeval, NODE_IF_BRANCHES, yypvt[-0].nodeval));
	  } break;
case 61:
/* #line 422 "AWKTAB.Y" */
{
	      yyval.nodetypeval = NODE_ILLEGAL;
	  } break;
case 62:
/* #line 429 "AWKTAB.Y" */
{
	      yyval.nodeval = NULL;
	  } break;
case 63:
/* #line 434 "AWKTAB.Y" */
{
	      yyval.nodeval = node(make_string(yypvt[-0].sval, -1), NODE_REDIRECT_OUTPUT, NULL);
	  } break;
case 64:
/* #line 438 "AWKTAB.Y" */
{
	      yyval.nodeval = node(yypvt[-0].nodeval, NODE_REDIRECT_OUTPUT, NULL);
	  } break;
case 65:
/* #line 442 "AWKTAB.Y" */
{
	      yyval.nodeval = node(make_string(yypvt[-0].sval, -1), NODE_REDIRECT_APPEND, NULL);
	  } break;
case 66:
/* #line 446 "AWKTAB.Y" */
{
	      yyval.nodeval = node(yypvt[-0].nodeval, NODE_REDIRECT_APPEND, NULL);
	  } break;
case 67:
/* #line 453 "AWKTAB.Y" */
{
	      yyval.nodeval = NULL;
	  } break;
case 68:
/* #line 458 "AWKTAB.Y" */
{
	      yyval.nodeval = node(make_string(yypvt[-0].sval, -1), NODE_REDIRECT_INPUT, NULL);
	  } break;
case 69:
/* #line 462 "AWKTAB.Y" */
{
	      yyval.nodeval = node(yypvt[-0].nodeval, NODE_REDIRECT_INPUT, NULL);
	  } break;
case 70:
/* #line 469 "AWKTAB.Y" */
{
	      yyval.nodeval = variable(yypvt[-0].sval);
	  } break;
case 71:
/* #line 474 "AWKTAB.Y" */
{
	      yyval.nodeval = node(variable(yypvt[-3].sval), NODE_SUBSCRIPT, yypvt[-1].nodeval);
	  } break;
case 72:
/* #line 479 "AWKTAB.Y" */
{
	      yyval.nodeval = node(yypvt[-0].nodeval, NODE_FIELD_SPEC, NULL);
	  } break;
case 73:
/* #line 487 "AWKTAB.Y" */
{
	      yyval.nodeval = NULL;
	  } break;
case 74:
/* #line 492 "AWKTAB.Y" */
{
	      yyval.nodeval = yypvt[-0].nodeval;
	  } break;
case 75:
/* #line 499 "AWKTAB.Y" */
{
	      yyval.nodeval = NULL;
	  } break;
case 76:
/* #line 504 "AWKTAB.Y" */
{
	      yyval.nodeval = node(yypvt[-0].nodeval, NODE_EXPRESSION_LIST, NULL);
	  } break;
case 77:
/* #line 509 "AWKTAB.Y" */
{
	      yyval.nodeval = append_right(yypvt[-2].nodeval, node(yypvt[-0].nodeval, NODE_EXPRESSION_LIST, NULL));
	  } break;
case 78:
/* #line 515 "AWKTAB.Y" */
{
	      yyval.nodeval = node(yypvt[-0].nodeval, NODE_UNARY_MINUS, NULL);
	  } break;
case 79:
/* #line 520 "AWKTAB.Y" */
{
	      yyval.nodeval = node(yypvt[-2].nodeval, yypvt[-1].nodetypeval, yypvt[-0].nodeval);
	  } break;
case 81:
/* #line 527 "AWKTAB.Y" */
{
	      yyval.nodeval = yypvt[-1].nodeval;
	  } break;
case 82:
/* #line 532 "AWKTAB.Y" */
{
	      yyval.nodeval = node(yypvt[-0].nodeval, NODE_PREINCREMENT, NULL);
	  } break;
case 83:
/* #line 537 "AWKTAB.Y" */
{
	      yyval.nodeval = node(yypvt[-0].nodeval, NODE_PREDECREMENT, NULL);
	  } break;
case 84:
/* #line 542 "AWKTAB.Y" */
{
	      yyval.nodeval = node(yypvt[-1].nodeval, NODE_POSTINCREMENT, NULL);
	  } break;
case 85:
/* #line 547 "AWKTAB.Y" */
{
	      yyval.nodeval = node(yypvt[-1].nodeval, NODE_POSTDECREMENT, NULL);
	  } break;
case 86:
/* #line 552 "AWKTAB.Y" */
{
	      yyval.nodeval = yypvt[-0].nodeval;
	  } break;
case 87:
/* #line 557 "AWKTAB.Y" */
{
	      yyval.nodeval = make_number(yypvt[-0].fval);
	  } break;
case 88:
/* #line 562 "AWKTAB.Y" */
{
	      yyval.nodeval = make_string(yypvt[-0].sval, -1);
	  } break;
case 89:
/* #line 568 "AWKTAB.Y" */
{
	      yyval.nodeval = node(yypvt[-2].nodeval, NODE_EXPONENTIAL, yypvt[-0].nodeval);
	  } break;
case 90:
/* #line 573 "AWKTAB.Y" */
{
	      yyval.nodeval = node(yypvt[-2].nodeval, NODE_TIMES, yypvt[-0].nodeval);
	  } break;
case 91:
/* #line 578 "AWKTAB.Y" */
{
	      yyval.nodeval = node(yypvt[-2].nodeval, NODE_QUOTIENT, yypvt[-0].nodeval);
	  } break;
case 92:
/* #line 583 "AWKTAB.Y" */
{
	      yyval.nodeval = node(yypvt[-2].nodeval, NODE_MOD, yypvt[-0].nodeval);
	  } break;
case 93:
/* #line 588 "AWKTAB.Y" */
{
	      yyval.nodeval = node(yypvt[-2].nodeval, NODE_PLUS, yypvt[-0].nodeval);
	  } break;
case 94:
/* #line 593 "AWKTAB.Y" */
{
	      yyval.nodeval = node(yypvt[-2].nodeval, NODE_MINUS, yypvt[-0].nodeval);
	  } break;
case 95:
/* #line 599 "AWKTAB.Y" */
{
	      yyval.nodeval = node(yypvt[-2].nodeval, NODE_CONCAT, yypvt[-0].nodeval);
	  } break;
case 96:
/* #line 604 "AWKTAB.Y" */
{
	      yyval.nodeval = node(yypvt[-4].nodeval, NODE_CONDEXP,
			node(yypvt[-2].nodeval, NODE_CONDEXP_BRANCHES, yypvt[-0].nodeval));
	  } break;
case 97:
/* #line 611 "AWKTAB.Y" */
{
	      yyval.nodeval = snode(yypvt[-1].nodeval, NODE_BUILTIN, yypvt[-3].ptrval);
	  } break;
case 98:
/* #line 616 "AWKTAB.Y" */
{
	      yyval.nodeval = snode(NULL, NODE_BUILTIN, yypvt[-0].ptrval);
	  } break;
case 99:
/* #line 621 "AWKTAB.Y" */
{
	      auto     NODE	*tmp;

	      tmp = node(NULL, NODE_GETLINE, yypvt[-0].nodeval);
	      yyval.nodeval  = snode(tmp, NODE_BUILTIN, yypvt[-1].ptrval);
	  } break;
case 100:
/* #line 629 "AWKTAB.Y" */
{
	      auto     NODE	*tmp;

	      tmp = node(yypvt[-1].nodeval, NODE_GETLINE, yypvt[-0].nodeval);
	      yyval.nodeval  = snode(tmp, NODE_BUILTIN, yypvt[-2].ptrval);
	  } break;
case 101:
/* #line 637 "AWKTAB.Y" */
{
	      auto     int	num;

	      num = count_arguments(yypvt[-1].nodeval);
	      if (num < 2 || num > 3)
		  yyerror("Invalid number of arguments for function");
	      yyval.nodeval = snode(yypvt[-1].nodeval, NODE_BUILTIN, yypvt[-3].ptrval);
	  } break;
case 102:
/* #line 647 "AWKTAB.Y" */
{
	      auto     NODE	*tmp;

	      tmp = node(yypvt[-1].nodeval, NODE_EXPRESSION_LIST, NULL);
	      tmp = append_right(yypvt[-3].nodeval, tmp);
	      if (count_arguments(yypvt[-3].nodeval) != 3)
		  yyerror("Invalid number of arguments for function");
	      yyval.nodeval  = snode(yypvt[-3].nodeval, NODE_BUILTIN, yypvt[-5].ptrval);
	  } break;
case 103:
/* #line 658 "AWKTAB.Y" */
{
	      auto     NODE	*tmp;

	      tmp = node(yypvt[-1].nodeval, NODE_EXPRESSION_LIST, NULL);
	      tmp = node(yypvt[-3].nodeval, NODE_EXPRESSION_LIST, tmp);
	      yyval.nodeval  = snode(tmp, NODE_BUILTIN, yypvt[-5].ptrval);
	  } break;
case 104:
/* #line 667 "AWKTAB.Y" */
{
	      if (count_arguments(yypvt[-1].nodeval) != 2)
		  yyerror("Invalid number of arguments for function");
	      yyval.nodeval = snode(yypvt[-1].nodeval, NODE_BUILTIN, yypvt[-3].ptrval);
	  } break;
case 105:
/* #line 674 "AWKTAB.Y" */
{
	      auto     NODE	 *tmp;

	      tmp = node(yypvt[-1].nodeval, NODE_EXPRESSION_LIST, NULL);
	      tmp = node(yypvt[-3].nodeval, NODE_EXPRESSION_LIST, tmp);
	      tmp = node(yypvt[-5].nodeval, NODE_EXPRESSION_LIST, tmp);
	      yyval.nodeval  = snode(tmp, NODE_BUILTIN, yypvt[-7].ptrval);
	  } break;
case 106:
/* #line 684 "AWKTAB.Y" */
{
	      auto     NODE	 *tmp;

	      tmp = make_number((AWKNUM) 0.0);
	      tmp = node(tmp, NODE_FIELD_SPEC, NULL);
	      tmp = node(tmp, NODE_EXPRESSION_LIST, NULL);
	      tmp = node(yypvt[-1].nodeval,  NODE_EXPRESSION_LIST, tmp);
	      tmp = node(yypvt[-3].nodeval,  NODE_EXPRESSION_LIST, tmp);
	      yyval.nodeval  = snode(tmp, NODE_BUILTIN, yypvt[-5].ptrval);
	  } break;
case 107:
/* #line 696 "AWKTAB.Y" */
{
	      auto     int	num;
	      auto     NODE    *tmp;

	      num = count_arguments(yypvt[-1].nodeval);
	      if (num < 2 || num > 3)
		  yyerror("Invalid number of arguments for function");
	      if (num == 2)
	      {
		  tmp = make_number((AWKNUM) 0.0);
		  tmp = node(tmp, NODE_FIELD_SPEC, NULL);
		  append_right(yypvt[-1].nodeval, node(tmp, NODE_EXPRESSION_LIST, NULL));
	      }
	      yyval.nodeval  = snode(yypvt[-1].nodeval, NODE_BUILTIN, yypvt[-3].ptrval);
	  } break;/* End of actions */
    }
    goto yystack;		/* stack new state and value */
}
