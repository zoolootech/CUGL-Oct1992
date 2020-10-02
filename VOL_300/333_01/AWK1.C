
/***************************************************************************/
/*									   */
/*     awk1 -- Expression tree constructors and main program for gawk.	   */
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
/*									   */
/* Extensive code restructuring and port to MSDOS and MS OS/2 by:	   */
/*									   */
/*			      Bob Withers				   */
/*			   649 Meadowbrook St.				   */
/*			   Allen, Texas 75002				   */
/*			    December 8, 1988				   */
/*									   */
/* The current state of gAWK (at least this version) is a subset of the    */
/* AWK langauge as released on Unix in 1985 and defined in the book "The   */
/* AWK Programming Langauge" by Aho, Kernighan, and Weinberger (1988).     */
/* Following are language elements which are not supported in this	   */
/* version:								   */
/*									   */
/* -  Actions								   */
/*									   */
/*    o  User defined functions are not supported			   */
/*									   */
/*    o  The "return [expression]" statement is not supported		   */
/*									   */
/*    o  The "delete array[i]" statement is not supported		   */
/*	 + Support added for "delete" by BW 01/02/89			   */
/*									   */
/* -  Input/Output							   */
/*									   */
/*    o  The "close(expr)" statement is not supported			   */
/*	 +  Support for close() added by BW 12/21/88			   */
/*	    -	close(exp)   ==> close file represented by "exp"	   */
/*	    -	close()      ==> close all redirected files		   */
/*	    -	returns 1 for success, 0 for file not found		   */
/*									   */
/*    o  The "system(cmd_line)" statement is not supported		   */
/*	 +  Support for "system" added by BW 12/6/88			   */
/*									   */
/*    o  Use of pipes is not supported                                     */
/*                                                                         */
/*    o  Output redirection to files is supported for print and printf     */
/*       but there is not support for input redirection via getline        */
/*									   */
/* -  Built-in Variables						   */
/*									   */
/*    o  The following built-in variables are not supported:		   */
/*									   */
/*	   ARGC 	 -- Support added by BW 12/26/88		   */
/*	   ARGV 	 -- Support added by BW 12/26/88		   */
/*	   FNR		 -- Support added by BW 12/01/88		   */
/*	   RLENGTH	 -- Support added by BW 12/21/88		   */
/*	   RSTART	 -- Support added by BW 12/21/88		   */
/*	   SUBSEP	 -- Support added by BW 12/26/88		   */
/*									   */
/* -  Built-in String Functions 					   */
/*									   */
/*    o  The following built-in string functions are not supported:	   */
/*									   */
/*	   gsub(r, s, t) -- Support added by BW 12/22/88		   */
/*	   match(s, r)	 -- Support added by BW 12/21/88		   */
/*	   sub(r, s, t)  -- Support added by BW 12/22/88		   */
/*									   */
/* -  Built-in Arithmetic Functions					   */
/*									   */
/*    o  The following built-in arithmetic functions are not supported:    */
/*									   */
/*	   atan2(y, x)	 -- Support added by BW 12/03/88		   */
/*	   cos(x)	 -- Support added by BW 12/03/88		   */
/*	   rand()	 -- Support added by BW 12/20/88		   */
/*	   sin(x)	 -- Support added by BW 12/03/88		   */
/*	   srand(x)	 -- Support added by BW 12/20/88		   */
/*									   */
/* -  Expression Operators						   */
/*									   */
/*    o  The match operator (~) and not match operator (!~) only work	   */
/*	 on hardcoded regular expressions.  They do not work on regular    */
/*	 expressions in a variable.					   */
/*	 + Support for matching regular expression stored in a variable    */
/*	   added by BW 01/06/88.  The variable is forced to a string and   */
/*	   compiled as a regular expression.  If it contains an invalid    */
/*	   regular expression execution is terminated with error msg.	   */
/*									   */
/*    o  The exponentiation operator (^) is not supported		   */
/*	 + Support for ^ added by BW 12/12/88.				   */
/*                                                                         */
/*    o  The conditional expression "exp1 ? exp2 : exp3" is not supported  */
/*	 + This has been fixed by BW on 12/20/88			   */
/*									   */
/* -  Misc								   */
/*									   */
/*    o  Source lines to be continued MUST be terminated by a backslash.   */
/*	 Unix AWK allows lines to be continued at a comma without a	   */
/*	 backslash, gAWK does not.					   */
/*	 + This was fixed by BW 12/13/88				   */
/*									   */
/*    o  Unix AWK allows a simple print statement to either use parens	   */
/*	 or not.  gAWK does not allow parens on simple print's.            */
/*	 + This was fixed by BW 12/13/88				   */
/*									   */
/*    o  gAWK only allowed a single level of subscripting on array names.  */
/*	 This was changed to support multi-level subscripts in the	   */
/*	 manner described in "The AWK Programming Language" which converts */
/*	 multidimension subscripts to a single subscript separated by	   */
/*	 the value of special variable SUBSEP.				   */
/*									   */
/*    o  The %f and %e format specifiers for printf() did not work	   */
/*	 properly.  This was fixed by BW.				   */
/*									   */
/*    o  Large number of assumptions that sizeof(int) == sizeof(char *)    */
/*	 were corrected by BW.						   */
/*									   */
/*    o  The split() function did not work properly when multiple field    */
/*	 separators appeared between fields.  This was fixed by BW.	   */
/*									   */
/*    o  The FS variable was enhanced to support regular expressions	   */
/*	 rather than the single character field seperator.  While this	   */
/*	 is not documented in "The AWK Programming Language" it is	   */
/*	 implemented in the latest AWK version on UNIX (called NAWK).	   */
/*	 The default value of FS was changed from " " to "[\t ]+".	   */
/*									   */
/* -  gAWK extensions - BW 12/20/88					   */
/*									   */
/*    o  upper function added to return the string value of its argument   */
/*	 converted to all upper case:					   */
/*									   */
/*		  $1 = upper($1)					   */
/*									   */
/*    o  lower function added to return the string value of its argument   */
/*	 converted to all lower case:					   */
/*									   */
/*		  $1 = lower($1)					   */
/*									   */
/*    o  reverse function added to return the string value of its	   */
/*	 argument reversed:						   */
/*									   */
/*		  $1 = reverse($1)					   */
/*									   */
/***************************************************************************/


#define DRIVER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "awk.h"

#define VERSION 		    "3.00"

STATIC char *  NEAR PASCAL  extract_module_name(char *argv0);


static char		*pgm_storage = NULL;



main(int argc, char *argv[])
{
    register int	i;
    register NODE      *tmp;
    auto     NODE     **nptr;
    auto     int	j;
    auto     char     **ptr, *p;

#ifndef FAST
    /* Print out the parse tree.   For debugging */
    register int	dotree = 0;
    extern int		yydebug;
#endif

    extern char        *lexptr;
    extern char        *lexptr_begin;
    FILE	       *fp;

#if PROFILER
/** StartProfile("AWK"); **/
#endif
    --argc;
    myname = extract_module_name(*argv++);
    if (!argc)
	usage();

    /* Tell the regex routines how they should work. . . */
    re_set_syntax(RE_NO_BK_PARENS | RE_NO_BK_VBAR);

    /* Set up the stack for temporary strings */
    obstack_init(&temp_strings);
    ob_dummy = obstack_alloc(&temp_strings, 0);

    /* Set up the other stack for other things */
    obstack_init(&other_stack);
    /* initialize the null string */
    Nnull_string = make_string("", 0);

    /* initialize the temp regular expression buffer */
    memset(&wrk_repat, 0, sizeof(wrk_repat));
    wrk_repat.fastmap	= wrk_fastmap;
    wrk_repat.allocated = 200;
    if (NULL == (wrk_repat.buffer = malloc(200)))
	panic("Out of memory for repat work buffer");

    /* Set up the special variables */
    /* Note that this must be done BEFORE arg parsing else -R and -F break
     * horribly */

    init_vars();

    for (; *argv && **argv == '-'; argc--, argv++)
    {
	switch (argv[0][1])
	{
#ifndef FAST
	    case 'd':
		debugging++;
		dotree++;
		break;
	    case 'D':
		debugging++;
		yydebug = 2;
		break;
#endif
	    case 'R':	 /* This feature isn't in un*x awk, might be useful */
		set_rs(&argv[0][2]);
		break;
	    case 'F':
		set_fs(&argv[0][2]);
		break;

		/* It would be better to read the input file in as we parse
		 * it.  Its done this way for hysterical reasons.  Feel free
		 * to fix it.
		 */
	    case 'f':
		if (lexptr)
		    panic("Can only use one -f option");
		if ((fp = fopen(argv[1], "r")) == NULL)
		    er_panic(argv[1]);
		else
		{
		    auto     char      *curptr;
		    auto     int	siz, nread;

		    curptr = lexptr = malloc(2000);
		    if (curptr == NULL)
			panic("Memory exhausted");

		    siz = 2000;
		    i	= siz - 1;
		    while ((nread = fread(curptr, sizeof(char), i, fp)) > 0)
		    {
			curptr += nread;
			i -= nread;
			if (i == 0)
			{
			    lexptr = realloc(lexptr, siz * 2);
			    if (lexptr == NULL)
				panic("Memory exhausted");
			    curptr = lexptr + siz - 1;
			    i = siz;
			    siz *= 2;
			}
		    }
		    *curptr = EOS;
		    pgm_storage = lexptr;
		    fclose(fp);
		}
		argc--;
		argv++;
		break;
	    case EOS:	       /* A file */
		break;
	    default:
		panic("Unknown option %s", argv[0]);
	}
    }
#ifndef FAST
    if (debugging)
	setbuf(stdout, 0);	/* jfw: make debugging easier */
#endif
    /* No -f option, use next arg */
    if (!lexptr)
    {
	if (!argc)
	    usage();
	lexptr = *argv++;
	--argc;
    }

    /* Read in the program */
    lexptr_begin = lexptr;
    (void) yyparse();
    free(pgm_storage);	 /* BW: I see no reason to leave this allocated     */

    /* Anything allocated on the other_stack after here will be freed when  */
    /* the next input line is read.					    */
    parse_end = obstack_alloc(&other_stack, 0);

#ifndef FAST
    if (dotree)
    {
	printf("Parse tree before execution:\n");
	print_parse_tree(expression_value);
    }
#endif
    /* Set up the field variables */
    init_fields();

    if (argc == 0)
    {
	static char    *dumb[2] = { "-", NULL };

	argc = 1;
	argv = &dumb[0];
    }

    for (ptr = argv, i = argc, j = 0; i > 0; --i)
    {
	if (NULL != *ptr)
	{
	    ++j;
	    assign_number(&ARGC_node->var_value, (AWKNUM) (j + 1));
	    nptr  = assoc_lookup(ARGV_node, tmp_number((AWKNUM) j),
				 ASSOC_CREATE);
	    *nptr = make_string(*ptr, strlen(*ptr));
	}
	++ptr;
    }

    /* Look for BEGIN and END blocks.  Only one of each allowed */
    for (tmp = expression_value; tmp; tmp = tmp->rnode)
    {
	if (!tmp->lnode || !tmp->lnode->lnode)
	    continue;
	if (tmp->lnode->lnode->type == NODE_K_BEGIN)
	{
	    if (begin_block)
		panic("More than one BEGIN block found");
	    else
		begin_block = tmp->lnode->rnode;
	}
	else
	{
	    if (tmp->lnode->lnode->type == NODE_K_END)
	    {
		if (end_block)
		    panic("More than one END block found");
		else
		    end_block = tmp->lnode->rnode;
	    }
	}
    }
    if (begin_block && interpret(begin_block) == 0)
	exit(0);

    j = 0;
    while (TRUE)
    {
	i = (int) force_number(ARGC_node->var_value);
	if (++j >= i)
	    break;

	nptr = assoc_lookup(ARGV_node, tmp_number((AWKNUM) j), ASSOC_TEST);
	if (NULL == nptr)
	    panic("Invalid array element found at ARGV[%d]", j);

	tmp = force_string(*nptr);
	if (0 == tmp->stlen)
	    continue;
	strcpy(current_filename, tmp->stptr);

	p = strchr(current_filename, '=');
	if (p)
	{
	    *p++ = EOS;
	     variable(current_filename)->var_value = make_string(p, strlen(p));
	}
	else
	{
	    assign_number(&FNR_node->var_value, (AWKNUM) 0.0);
	    deref = FILENAME_node->var_value;
	    do_deref();
	    FILENAME_node->var_value =
		    make_string(current_filename, strlen(current_filename));

	    if (0 == strcmp(current_filename, "-"))
		input_file = stdin;
	    else
	    {
		input_file = fopen(current_filename, "r");
		if (NULL == input_file)
		    er_panic(current_filename);
	    }

	    if (inrec(input_file) == 0)
	    {
		do
		{
		    obstack_free(&temp_strings, ob_dummy);
		} while (interpret(expression_value)
				   && inrec(input_file) == 0);
	    }

	    deref = FILENAME_node->var_value;
	    do_deref();
	    if (input_file != stdin)
		fclose(input_file);
	}
    }
    if (end_block)
	(void) interpret(end_block);

#ifndef FAST
    if (dotree)
    {
	printf("\nParse tree after execution:\n");
	print_parse_tree(expression_value);
    }
#endif

    close_redirect_files();
#if PROFILER
/** EndProfile(); **/
#endif
    return(0);
}


STATIC char * NEAR PASCAL extract_module_name(register char *argv0)
{
    register char	*p;

    p = argv0 + strlen(argv0);
    while (--p > argv0)
    {
	if ('.' == *p)
	    *p = EOS;
	else
	    if ('\\' == *p || '/' == *p)
		return(++p);
    }
    return(p);
}


void PASCAL clear_wrk_repat()
{
    wrk_repat.used	       = 0;
    wrk_repat.fastmap_accurate = FALSE;
    wrk_repat.can_be_null      = 0;
    return;
}


/* These exit values are arbitrary */

VOID panic(char *str, ...)
{
    auto     va_list	    va;

    fprintf(stderr, "%s: ", myname);

    va_start(va, str);
    vfprintf(stderr, str, va);
    va_end(va);

    fprintf(stderr, "\n");
    exit(12);
}



VOID PASCAL er_panic(char *str)
{
    fprintf(stderr, "%s: ", myname);
    perror(str);
    exit(15);
}



VOID PASCAL usage(void)
{
    fprintf(stderr, "\nGNU AWK Version %s\n", VERSION);
    fprintf(stderr, "Extensive modification and enhancement by Bob Withers\n");
    fprintf(stderr, "Permission to freely copy as provided by the ");
    fprintf(stderr, "GNU Project charter\n\n");
    fprintf(stderr,
	"%s: usage: %s {-f progfile | program } [-F{c} -R{c}] file . . .\n",
	myname, myname);
    exit(11);
}



/* This allocates a new node of type ty.  Note that this node will not go
   away unless freed, so don't use it for tmp storage */

NODE * PASCAL newnode(int ty)
{
    register NODE      *r;

    r = (NODE *) malloc(sizeof(NODE));
    if (r == NULL)
	panic("Out of memory in function newnode()");
    r->type = ty;
    return(r);
}


/* Duplicate a node.  (For global strings, "duplicate" means crank up	   */
/* the reference count.)  This creates global nodes. . .		   */

NODE * PASCAL dupnode(NODE *n)
{
    register NODE      *r;

    if (n->type == NODE_STRING)
    {
	n->stref++;
	return(n);
    }
    else
    {
	if (n->type == NODE_TEMP_STRING)
	{
	    r	     = newnode(NODE_STRING);
	    r->stlen = n->stlen;
	    r->stref = 1;
	    r->stptr = malloc(n->stlen + 1);
	    if (r->stptr == NULL)
		panic("Out of memory in function dupnode()");
	    memcpy(r->stptr, n->stptr, n->stlen);
	    r->stptr[r->stlen] = EOS;	   /* JF for hackval */
	    return(r);
	}
	else
	{
	    r  = newnode(NODE_ILLEGAL);
	    *r = *n;
	    return(r);
	}
    }
}


/* This allocates a node with defined lnode and rnode. */
/* This should only be used by yyparse+co while
   reading in the program */

NODE * PASCAL node(NODE *left, int op, NODE *right)
{
    register NODE  *r;

    r	     = (NODE *) obstack_alloc(&other_stack, sizeof(NODE));
    r->type  = op;
    r->lnode = left;
    r->rnode = right;
    return(r);
}


/* This allocates a node with defined subnode and proc */
/* Otherwise like node() */

NODE * PASCAL snode(NODE *subn, int op, NODE *(PASCAL *procp)(NODE *))
{
    register NODE	 *r;

    r	       = (NODE *) obstack_alloc(&other_stack, sizeof(NODE));
    r->type    = op;
    r->subnode = subn;
    r->proc    = procp;
    return(r);
}


/* (jfw) This allocates a Node_line_range node
 * with defined condpair and zeroes the trigger word
 * to avoid the temptation of assuming that calling
 * 'node( foo, Node_line_range, 0)' will properly initialize 'triggered'.
 */
/* Otherwise like node() */

NODE * PASCAL mkrangenode(NODE *cpair)
{
    register NODE	*r;

    r		 = (NODE *) obstack_alloc(&other_stack, sizeof(NODE));
    r->type	 = NODE_LINE_RANGE;
    r->condpair  = cpair;
    r->triggered = 0;
    return(r);
}


/*  count the number of arguments in an expression list 		*/

int PASCAL count_arguments(register NODE *exp_list)
{
    auto     int	 cnt = 0;

    while (exp_list)
    {
	++cnt;
	exp_list = exp_list->rnode;
    }
    return(cnt);
}


/* this allocates a node with defined numbr */
/* This creates global nodes! */

NODE * PASCAL make_number(AWKNUM x)
{
    register NODE	*r;

    r	     = newnode(NODE_NUMBER);
    r->numbr = x;
    return(r);
}


/* This creates temporary nodes.  They go away quite quickly, so
   don't use them for anything important */

NODE * PASCAL tmp_number(AWKNUM x)
{
    auto     NODE      *r;

    r	     = (NODE *) obstack_alloc(&temp_strings, sizeof(NODE));
    r->type  = NODE_NUMBER;
    r->numbr = x;
    return(r);
}


/* Make a string node.  If len==0, the string passed in S is supposed to end
   with a double quote, but have had the beginning double quote
   already stripped off by yylex.
   If LEN!=0, we don't care what s ends with.  This creates a global node */

NODE * PASCAL make_string(char *s, int len)
{
    register NODE	*r;
    register char	*pf, *pt;
    register int	 c;

    /* the aborts are impossible because yylex is supposed to have already
     * checked for unterminated strings */
    if (len == -1)
    {				/* Called from yyparse, find our own len */
	for (pf = pt = s; *pf != EOS && *pf != '\"';)
	{
	    c = *pf++;
	    switch (c)
	    {
		case '\\':
#ifndef FAST
		    if (*pf == EOS)
			panic("EOS following \\ in make_string()");
#endif
		    c = *pf++;
		    switch (c)
		    {
			case '\\':	/* no massagary needed */
			case '\'':
			case '\"':
			    break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			    c -= '0';
			    while (*pf && *pf >= '0' && *pf <= '7')
			    {
				c = c * 8 + *pf++ - '0';
			    }
			    break;
			case 'b':
			    c = '\b';
			    break;
			case 'f':
			    c = '\f';
			    break;
			case 'n':
			    c = '\n';
			    break;
			case 'r':
			    c = '\r';
			    break;
			case 't':
			    c = '\t';
			    break;
			case 'v':
			    c = '\v';
			    break;
			default:
			    *pt++ = '\\';
			    break;
		    }
		    /* FALL THROUGH */
		default:
		    *pt++ = c;
		    break;
	    }
	}
	len = pt - s;
    }

    r = newnode(NODE_STRING);
    r->stptr = (char *) malloc(len + 1);
    if (r->stptr == NULL)
	panic("Out of memory in make_string()");
    r->type = NODE_STRING;
    r->stlen = len;
    r->stref = 1;
    memcpy(r->stptr, s, len);
    r->stptr[len] = EOS;

    return(r);
}


/* This should be a macro for speed, but the C compiler chokes. */
/* Read the warning under tmp_number */

NODE * PASCAL tmp_string(char *s, int len)
{
    register NODE     *r;

    r	     = (NODE *) obstack_alloc(&temp_strings, sizeof(NODE));
    r->stptr = (char *) obstack_alloc(&temp_strings, len + 1);
    r->type  = NODE_TEMP_STRING;
    r->stlen = len;
    r->stref = 1;
    memcpy(r->stptr, s, len);
    r->stptr[len] = EOS;       /* JF a hack */

    return(r);
}


/* Generate compiled regular expressions */

REPAT_BUFFER * PASCAL make_regexp(char *s)
{
    auto     REPAT_BUFFER   *rp;
    auto     char	    *p, *pout, *err;
    auto     int	     c, len = 0;

    rp = (REPAT_BUFFER *) obstack_alloc(&other_stack, sizeof(REPAT_BUFFER));
    memset(rp, 0, sizeof(REPAT_BUFFER));
    rp->buffer = (char *) malloc(100);	/* JF I'd obstack allocate it, except
					 * the regex routines try to
					 * realloc() it, which fails. */
    rp->allocated = 100;
    rp->fastmap   = (char *) obstack_alloc(&other_stack, FASTMAP_SIZE);

    for (pout = wrk_fastmap, p = s; *p != EOS; p++)
    {
	if (*p == '\\')
	{
	    c = *(++p);
	    switch (c)
	    {
		case 'b':
		    c = '\b';
		    break;
		case 't':
		    c = '\t';
		    break;
		case 'f':
		    c = '\f';
		    break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		    c -= '0';
		    while (*p && *p >= '0' && *p <= '7')
			c = c * 8 + *p++ - '0';
		    break;
		case 'n':
		    c = '\n';
		    break;
		case 'r':
		    c = '\r';
		    break;
	    }
	    *pout++ = c;
	    ++len;
	}
	else
	{
	    if (*p == '/')
		break;
	    else
	    {
		*pout++ = *p;
		++len;
	    }
	}
    }
#ifndef FAST
    if (*p != '/')
	panic("REGEXP doesn't end with / in make_regexp()");
#endif

    if ((err = re_compile_pattern(wrk_fastmap, len, rp)) != NULL)
    {
	fprintf(stderr, "illegal regexp: ");
	yyerror(err);		/* fatal */
    }

    return(rp);
}


/* Build a for loop */

FOR_LOOP_HEADER * PASCAL make_for_loop(NODE *init, NODE *cond, NODE *incr)
{
    register FOR_LOOP_HEADER	   *r;

    r = (FOR_LOOP_HEADER *) obstack_alloc(&other_stack,
					  sizeof(FOR_LOOP_HEADER));
    r->init = init;
    r->cond = cond;
    r->incr = incr;
    return(r);
}


/* Name points to a variable name.  Make sure its in the symbol table */

NODE * PASCAL variable(char *name)
{
    register NODE      *r;

    if ((r = lookup(variables, name)) == NULL)
    {
	r = install(variables, name,
		    node(Nnull_string, NODE_VAR, (NODE *) NULL));
    }
    return(r);
}


/* Create a special variable */

NODE * PASCAL spc_var(char *name, NODE *value)
{
    register NODE    *r;

    if ((r = lookup(variables, name)) == NULL)
	r = install(variables, name, node(value, NODE_VAR, (NODE *) NULL));
    return(r);
}


/*
 * Install a name in the hash table specified, even if it is already there.
 * Name stops with first non alphanumeric.
 * Caller must check against redefinition if that is desired.
 */

NODE * PASCAL install(HASHNODE **table, char *name, NODE *value)
{
    register HASHNODE	   *hp;
    register int	    i, len, bucket;
    register char	   *p;

    len = 0;
    p	= name;
    while (is_identchar(*p))
	p++;
    len = p - name;

    i		  = sizeof(HASHNODE) + len + 1;
    hp		  = (HASHNODE *) obstack_alloc(&other_stack, i);
    bucket	  = hashf(name, len, HASHSIZE);
    hp->next	  = table[bucket];
    table[bucket] = hp;
    hp->length	  = len;
    hp->value	  = value;
    hp->name	  = ((char *) hp) + sizeof(HASHNODE);
    hp->length	  = len;
    memcpy(hp->name, name, len);
    return(hp->value);
}


/*
 * find the most recent hash node for name name (ending with first
 * non-identifier char) installed by install
 */

NODE * PASCAL lookup(HASHNODE **table, char *name)
{
    register char	 *bp;
    register HASHNODE	 *bucket;
    register int	  len;

    for (bp = name; is_identchar(*bp); bp++)
	;
    len    = bp - name;
    bucket = table[hashf(name, len, HASHSIZE)];
    while (bucket)
    {
	if (bucket->length == len && strncmp(bucket->name, name, len) == 0)
	    return(bucket->value);
	bucket = bucket->next;
    }
    return(NULL);
}


/*
 * return hash function on name.  must be compatible with the one
 * computed a step at a time, elsewhere  (JF: Where?  I can't find it!)
 *					 (BW: Neither can I!)
 */

int PASCAL hashf(register char *name, register int len, int hashsize)
{
    register int	r = 0;

    while (len--)
	r = HASHSTEP(r, *name++);

    return(MAKE_POS(r) % hashsize);
}


/* Add new to the rightmost branch of LIST.  This uses n^2 time, but
   doesn't get used enough to make optimizing worth it. . . */
/* You don't believe me?  Profile it yourself! */

NODE * PASCAL append_right(NODE *list, NODE *new)
{
    register NODE	 *oldlist;

    oldlist = list;
    while (list->rnode != NULL)
	list = list->rnode;
    list->rnode = new;
    return(oldlist);
}
