/*
   Debug.c -- Various debugging routines
  
   Copyright (C) 1986 Free Software Foundation
     Written by Jay Fenlason, December 1986
  
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "awk.h"



STATIC char *	NEAR PASCAL	get_builtin_name(NODE *ptr);
STATIC void	NEAR PASCAL	deal_with_curls(NODE *ptr);


#ifndef FAST

/* This is all debugging stuff.  Ignore it and maybe it'll go away. */

struct builtin_names
{
    NODE	*(PASCAL *pptr)(NODE *);
    char	*name;
};

static struct builtin_names	bnames[] =
				{
				    { do_atan2,   "atan2"   },
				    { do_close,   "close"   },
				    { do_cos,	  "cos"     },
				    { do_bp,	  "bp"	    },
				    { do_exp,	  "exp"     },
				    { do_getline, "getline" },
				    { do_gsub,	  "gsub"    },
				    { do_index,   "index"   },
				    { do_int,	  "int"     },
				    { do_length,  "length"  },
				    { do_log,	  "log"     },
				    { do_lower,   "lower"   },
				    { do_match,   "match"   },
				    { do_prvars,  "prvars"  },
				    { do_rand,	  "rand"    },
				    { do_reverse, "reverse" },
				    { do_sin,	  "sin"     },
				    { do_split,   "split"   },
				    { do_sprintf, "sprintf" },
				    { do_sqrt,	  "sqrt"    },
				    { do_srand,   "srand"   },
				    { do_sub,	  "sub"     },
				    { do_substr,  "substr"  },
				    { do_system,  "system"  },
				    { do_upper,   "upper"   }
				};

static char   *nnames[] = {
			    "ILLEGAL NODE",	       /*  0 */
			    "EXPONENTIAL",	       /*  1 */
			    "TIMES",		       /*  2 */
			    "DIVIDE",		       /*  3 */
			    "MOD",		       /*  4 */
			    "PLUS",		       /*  5 */
			    "MINUS",		       /*  6 */
			    "COND PAIR",	       /*  7 */
			    "SUBSCRIPT",	       /*  8 */
			    "CONCAT",		       /*  9 */
			    "++PRE",		       /* 10 */
			    "--PRE",		       /* 11 */
			    "POST++",		       /* 12 */
			    "POST--",		       /* 13 */
			    "UMINUS",		       /* 14 */
			    "FIELD",		       /* 15 */
			    "ASSIGN",		       /* 16 */
			    "^=",		       /* 17 */
			    "*=",		       /* 18 */
			    "/=",		       /* 19 */
			    "%=",		       /* 20 */
			    "+=",		       /* 21 */
			    "-=",		       /* 22 */
			    "AND",		       /* 23 */
			    "OR",		       /* 24 */
			    "EQUAL",		       /* 25 */
			    "!=",		       /* 26 */
			    "LESS",		       /* 27 */
			    "GREATER",		       /* 28 */
			    "<=",		       /* 29 */
			    ">=",		       /* 30 */
			    "NOT",		       /* 31 */
			    "MATCH",		       /* 32 */
			    "NOMATCH",		       /* 33 */
			    "STRING",		       /* 34 */
			    "TMPSTRING",	       /* 35 */
			    "NUMBER",		       /* 36 */
			    "RULE LIST",	       /* 37 */
			    "RULE NODE",	       /* 38 */
			    "STMT LIST",	       /* 39 */
			    "IF BRANCHES",	       /* 40 */
			    "EXP LIST", 	       /* 41 */
			    "BEGIN",		       /* 42 */
			    "END",		       /* 43 */
			    "IF",		       /* 44 */
			    "WHILE",		       /* 45 */
			    "FOR",		       /* 46 */
			    "ARRAY FOR",	       /* 47 */
			    "BREAK",		       /* 48 */
			    "CONTINUE", 	       /* 49 */
			    "PRINT",		       /* 50 */
			    "PRINTF",		       /* 51 */
			    "NEXT",		       /* 52 */
			    "EXIT",		       /* 53 */
			    "DELETE",		       /* 54 */
			    "GETLINE",		       /* 55 */
			    "REDIRECT <",	       /* 56 */
			    "REDIRECT >",	       /* 57 */
			    "REDIRECT >>",	       /* 58 */
			    "REDIRECT |",	       /* 59 */
			    "VARIABLE", 	       /* 60 */
			    "VARRAY",		       /* 61 */
			    "BUILTIN",		       /* 62 */
			    "LINE RANGE",	       /* 63 */
			    "COND EXP", 	       /* 64 */
			    "CONDEXP BRANCHES",        /* 65 */
			    "REGEXP",		       /* 66 */
			    "MEMBER COND"	       /* 67 */
			  };


void PASCAL ptree(NODE *n)
{
    print_parse_tree((NODE *) n);
    return;
}


void PASCAL pt(void)
{
    auto     NODE	*x;

    scanf("%p", (FPTR) &x);
    printf("0x%p\n", (FPTR) x);
    print_parse_tree(x);
    fflush(stdout);
    return;
}

static          depth = 0;

void PASCAL print_parse_tree(register NODE *ptr)
{
    register int	n;

    if (!ptr)
    {
	printf("NULL\n");
	return;
    }

    if (ptr->type < 0 || ptr->type > MAXDIM(nnames))
    {
	printf("(0x%p Type %d??)\n", (FPTR) ptr, ptr->type);
	return;
    }

    printf("(%d)%*s", depth, depth, "");
    switch (ptr->type)
    {
	case NODE_REGEXP:
	    printf("(0x%p REG EXPRESSION \"%.*s\"\n",
		   (FPTR) ptr, ptr->rereg->used, ptr->rereg->buffer);
	    return;
	case NODE_STRING:
	case NODE_TEMP_STRING:
	    printf("(0x%p STRING \"%.*s\")\n", (FPTR) ptr, ptr->stlen,
		   ptr->stptr);
	    return;
	case NODE_NUMBER:
	    printf("(0x%p NUMBER %g)\n", (FPTR) ptr, ptr->numbr);
	    return;
	case NODE_VAR_ARRAY:
	    printf("(0x%p ARRAY OF %d)\n", (FPTR) ptr, ptr->arrsiz);
	    for (n = 0; n < ptr->arrsiz; n++)
	    {
		printf("'");
		print_simple((ptr->array)[n * 2], stdout);
		printf("' is '");
		print_simple((ptr->array)[n * 2 + 1], stdout);
		printf("'\n");
	    }
	    return;
    }
    if (ptr->lnode)
	printf("0x%p = <--", (FPTR) ptr->lnode);
    else
	printf("NULL = <--");
    if (NODE_BUILTIN == ptr->type)
	printf("(0x%p %s(%s) [%d])", (FPTR) ptr, nnames[ptr->type],
				      get_builtin_name(ptr), ptr->type);
    else
	printf("(0x%p %s [%d])", (FPTR) ptr, nnames[ptr->type], ptr->type);
    if (ptr->rnode)
	printf("--> = 0x%p", (FPTR) ptr->rnode);
    else
	printf("--> = NULL");
    printf("\n");
    depth++;
    if (ptr->lnode)
	print_parse_tree(ptr->lnode);
    switch (ptr->type)
    {
	case NODE_LINE_RANGE:	  /* jfw */
	    break;
	case NODE_BUILTIN:
	    printf("BUILTIN: %p\n", (FPTR) ptr->proc);
	    break;
	case NODE_K_FOR:
	case NODE_K_ARRAYFOR:
	    printf("(%s:)\n", nnames[ptr->type]);
	    print_parse_tree(ptr->forloop->init);
	    printf("looping:\n");
	    print_parse_tree(ptr->forloop->cond);
	    printf("doing:\n");
	    print_parse_tree(ptr->forloop->incr);
	    break;
	default:
	    if (ptr->rnode)
		print_parse_tree(ptr->rnode);
	    break;
    }
    --depth;
}


STATIC char * NEAR PASCAL get_builtin_name(NODE *ptr)
{
    register int	n;

    for (n = MAXDIM(bnames) - 1; n >= 0; ++n)
    {
	if (ptr->proc == bnames[n].pptr)
	    break;
    }
    if (n >= 0)
       return(bnames[n].name);
    return("??");
}


/* print out all the variables in the world */

void PASCAL dump_vars(void)
{
    register int	 n;
    register HASHNODE	*buc;

    printf("Fields:");
    dump_fields();
    printf("Vars:\n");
    for (n = 0; n < HASHSIZE; n++)
    {
	for (buc = variables[n]; buc; buc = buc->next)
	{
	    printf("-----> '%.*s': ", buc->length, buc->name);
	    print_simple(buc->value->var_value, stdout);
	    printf(":");
	    print_parse_tree(buc->value->lnode);
	}
    }
    printf("End\n");
    return;
}


void PASCAL dump_fields(void)
{
    register NODE      **p;
    register int	 n;

    printf("%d fields\n", f_arr_siz);
    for (n = 0, p = &fields_arr[0]; n < f_arr_siz; n++, p++)
    {
	printf("$%d is '", n);
	print_simple(*p, stdout);
	printf("'\n");
    }
    return;
}


void print_debug(char *str, ...)
{
    auto     va_list	    ap;

    if (debugging)
    {
	va_start(ap, str);
	vprintf(str, ap);
	printf("\n");
	va_end(ap);
    }
    return;
}


int             indent = 0;

void PASCAL print_a_node(NODE *ptr)
{
    auto     NODE	    *p1;
    auto     char	    *str, *str2;
    auto     int	     n;
    auto     HASHNODE	    *buc;

    if (!ptr)
	return;			/* don't print null ptrs */
    switch (ptr->type)
    {
	case NODE_NUMBER:
	    printf("%g", ptr->numbr);
	    return;
	case NODE_STRING:
	    printf("\"%.*s\"", ptr->stlen, ptr->stptr);
	    return;
	case NODE_EXPONENTIAL:
	    str = "^";
	    goto pr_twoop;
	case NODE_TIMES:
	    str = "*";
	    goto pr_twoop;
	case NODE_QUOTIENT:
	    str = "/";
	    goto pr_twoop;
	case NODE_MOD:
	    str = "%";
	    goto pr_twoop;
	case NODE_PLUS:
	    str = "+";
	    goto pr_twoop;
	case NODE_MINUS:
	    str = "-";
	    goto pr_twoop;
	case NODE_CONCAT:
	    str = " ";
	    goto pr_twoop;
	case NODE_ASSIGN:
	    str = "=";
	    goto pr_twoop;
	case NODE_ASSIGN_EXPONENTIAL:
	    str = "^=";
	    goto pr_twoop;
	case NODE_ASSIGN_TIMES:
	    str = "*=";
	    goto pr_twoop;
	case NODE_ASSIGN_QUOTIENT:
	    str = "/=";
	    goto pr_twoop;
	case NODE_ASSIGN_MOD:
	    str = "%=";
	    goto pr_twoop;
	case NODE_ASSIGN_PLUS:
	    str = "+=";
	    goto pr_twoop;
	case NODE_ASSIGN_MINUS:
	    str = "-=";
	    goto pr_twoop;
	case NODE_AND:
	    str = "&&";
	    goto pr_twoop;
	case NODE_OR:
	    str = "||";
	    goto pr_twoop;
	case NODE_EQUAL:
	    str = "==";
	    goto pr_twoop;
	case NODE_NOTEQUAL:
	    str = "!=";
	    goto pr_twoop;
	case NODE_LESS:
	    str = "<";
	    goto pr_twoop;
	case NODE_GREATER:
	    str = ">";
	    goto pr_twoop;
	case NODE_LEQ:
	    str = "<=";
	    goto pr_twoop;
	case NODE_GEQ:
	    str = ">=";
pr_twoop:
	    print_a_node(ptr->lnode);
	    printf("%s", str);
	    print_a_node(ptr->rnode);
	    return;

	case NODE_NOT:
	    str = "!";
	    str2 = "";
	    goto pr_oneop;
	case NODE_FIELD_SPEC:
	    str = "$(";
	    str2 = ")";
	    goto pr_oneop;
	case NODE_POSTINCREMENT:
	    str = "";
	    str2 = "++";
	    goto pr_oneop;
	case NODE_POSTDECREMENT:
	    str = "";
	    str2 = "--";
	    goto pr_oneop;
	case NODE_PREINCREMENT:
	    str = "++";
	    str2 = "";
	    goto pr_oneop;
	case NODE_PREDECREMENT:
	    str = "--";
	    str2 = "";
	    goto pr_oneop;
pr_oneop:
	    printf(str);
	    print_a_node(ptr->subnode);
	    printf(str2);
	    return;
	case NODE_EXPRESSION_LIST:
	    print_a_node(ptr->lnode);
	    if (ptr->rnode)
	    {
		printf(",");
		print_a_node(ptr->rnode);
	    }
	    return;

	case NODE_VAR:
	    for (n = 0; n < HASHSIZE; n++)
	    {
		for (buc = variables[n]; buc; buc = buc->next)
		{
		    if (buc->value == ptr)
		    {
			printf("%.*s", buc->length, buc->name);
			n = HASHSIZE;
			break;
		    }
		}
	    }
	    return;
	case NODE_SUBSCRIPT:
	    print_a_node(ptr->lnode);
	    printf("[");
	    print_a_node(ptr->rnode);
	    printf("]");
	    return;
	case NODE_BUILTIN:
	    printf("some_builtin(");
	    print_a_node(ptr->subnode);
	    printf(")");
	    return;
	case NODE_STATEMENT_LIST:
	    printf("{\n");
	    indent++;
	    for (n = indent; n; --n)
		printf("  ");
	    while (ptr)
	    {
		print_maybe_semi(ptr->lnode);
		if (ptr->rnode)
		    for (n = indent; n; --n)
			printf("  ");
		ptr = ptr->rnode;
	    }
	    --indent;
	    for (n = indent; n; --n)
		printf("  ");
	    printf("}\n");
	    for (n = indent; n; --n)
		printf("  ");
	    return;
	case NODE_K_IF:
	    printf("if(");
	    print_a_node(ptr->lnode);
	    printf(") ");
	    ptr = ptr->rnode;
	    if (ptr->lnode->type == NODE_STATEMENT_LIST)
	    {
		printf("{\n");
		indent++;
		for (p1 = ptr->lnode; p1; p1 = p1->rnode)
		{
		    for (n = indent; n; --n)
			printf("  ");
		    print_maybe_semi(p1->lnode);
		}
		--indent;
		for (n = indent; n; --n)
		    printf("  ");
		if (ptr->rnode)
		{
		    printf("} else ");
		}
		else
		{
		    printf("}\n");
		    return;
		}
	    }
	    else
	    {
		print_maybe_semi(ptr->lnode);
		if (ptr->rnode)
		{
		    for (n = indent; n; --n)
			printf("  ");
		    printf("else ");
		}
		else
		    return;
	    }
	    if (!ptr->rnode)
		return;
	    deal_with_curls(ptr->rnode);
	    return;
	case NODE_K_FOR:
	    printf("for(");
	    print_a_node(ptr->forloop->init);
	    printf(";");
	    print_a_node(ptr->forloop->cond);
	    printf(";");
	    print_a_node(ptr->forloop->incr);
	    printf(") ");
	    deal_with_curls(ptr->forsub);
	    return;
	case NODE_K_ARRAYFOR:
	    printf("for(");
	    print_a_node(ptr->forloop->init);
	    printf(" in ");
	    print_a_node(ptr->forloop->incr);
	    printf(") ");
	    deal_with_curls(ptr->forsub);
	    return;
	case NODE_K_PRINTF:
	    printf("printf(");
	    print_a_node(ptr->lnode);
	    printf(")");
	    return;
	case NODE_K_PRINT:
	    printf("print(");
	    print_a_node(ptr->lnode);
	    printf(")");
	    return;
	case NODE_K_NEXT:
	    printf("next");
	    return;
	case NODE_K_BREAK:
	    printf("break");
	    return;
	default:
	    print_parse_tree(ptr);
    }
    return;
}


void PASCAL print_maybe_semi(NODE *ptr)
{
    print_a_node(ptr);
    switch (ptr->type)
    {
	case NODE_K_IF:
	case NODE_K_FOR:
	case NODE_K_ARRAYFOR:
	case NODE_STATEMENT_LIST:
	    break;
	default:
	    printf(";\n");
	    break;
    }
    return;
}


STATIC void NEAR PASCAL deal_with_curls(NODE *ptr)
{
    auto     int	n;

    if (ptr->type == NODE_STATEMENT_LIST)
    {
	printf("{\n");
	indent++;
	while (ptr)
	{
	    for (n = indent; n; --n)
		printf("  ");
	    print_maybe_semi(ptr->lnode);
	    ptr = ptr->rnode;
	}
	--indent;
	for (n = indent; n; --n)
	    printf("  ");
	printf("}\n");
    }
    else
	print_maybe_semi(ptr);
    return;
}


NODE * PASCAL do_prvars(NODE *dummy)
{
    dump_vars();
    return(Nnull_string);
}


NODE * PASCAL do_bp(NODE *dummy)
{
    return(Nnull_string);
}

#endif /* FAST */
