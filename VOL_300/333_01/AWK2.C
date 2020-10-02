/*
 * awk2 --- gawk parse tree interpreter
 *
 * Copyright (C) 1986 Free Software Foundation
 *   Written by Paul Rubin, August 1986
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <setjmp.h>
#include "awk.h"


/* More of that debugging stuff */
#ifdef FAST
#define DEBUG(X)
#else
#define DEBUG(X)		 print_debug X
#endif

/* longjmp return codes, must be nonzero */
/* Continue means either for loop/while continue, or next input record */
#define TAG_CONTINUE 1
/* Break means either for/while break, or stop reading input */
#define TAG_BREAK 2

/* the loop_tag_valid variable allows continue/break-out-of-context
 * to be caught and diagnosed (jfw) */

#define PUSH_BINDING(stack, x)			\
		    (memcpy((stack), (x), sizeof(jmp_buf)), loop_tag_valid++)

#define RESTORE_BINDING(stack, x)		\
		    (memcpy((x), (stack), sizeof(jmp_buf)), loop_tag_valid--)

/* for "for(iggy in foo) {" */
struct search
{
    int             numleft;
    AHASH         **arr_ptr;
    AHASH          *bucket;
    NODE           *symbol;
    NODE           *retval;
};

STATIC struct search *	NEAR PASCAL    assoc_scan(NODE *symbol);
STATIC struct search *	NEAR PASCAL    assoc_next(struct search *lookat);


/* Tree is a bunch of rules to run.
   Returns zero if it hit an exit() statement */

int PASCAL interpret(NODE *tree)
{
    register NODE  *t;		     /* temporary */

    auto jmp_buf    loop_tag_stack;  /* shallow binding stack for loop_tag */
    static jmp_buf  loop_tag;	     /* always the current binding */
    static int      loop_tag_valid = 0;	/* nonzero when loop_tag valid (jfw) */

    static jmp_buf  rule_tag;	/* tag the rule currently being run, for NEXT
				 * and EXIT statements.  It is static because
				 * there are no nested rules */

    register NODE **lhs;	/* lhs == Left Hand Side for assigns, etc */
    register struct search *l;	/* For array_for */

    /* clean up temporary strings created by evaluating expressions in
     * previous recursive calls */
    obstack_free(&temp_strings, ob_dummy);

    if (tree == NULL)
	return(1);
    switch (tree->type)
    {
#ifndef FAST
	    /* Can't run these! */
	case NODE_ILLEGAL:
	case NODE_RULE_NODE:
	case NODE_IF_BRANCHES:
	case NODE_EXPRESSION_LIST:
	case NODE_K_BEGIN:
	case NODE_K_END:
	case NODE_REDIRECT_OUTPUT:
	case NODE_REDIRECT_APPEND:
	case NODE_REDIRECT_PIPE:
	case NODE_VAR_ARRAY:
	case NODE_CONDEXP_BRANCHES:
	    panic("Illegal node type (%d) in interpret()", tree->type);
#endif
	case NODE_RULE_LIST:
	    for (t = tree; t != NULL; t = t->rnode)
	    {
		switch (setjmp(rule_tag))
		{
		    case 0:	/* normal non-jump */
			if (eval_condition(t->lnode->lnode))
			{
			    DEBUG(("Found a rule:%p", (FPTR)t->lnode->rnode));
			    if (t->lnode->rnode == NULL)
			    {
				/* special case: pattern with no action is
				 * equivalent to an action of {print} (jfw) */
				NODE            printnode;

				printnode.type = NODE_K_PRINT;
				printnode.lnode = NULL;
				printnode.rnode = NULL;
				hack_print_node(&printnode);
			    }
			    else
				(void) interpret(t->lnode->rnode);
			}
			break;
		    case TAG_CONTINUE:	/* NEXT statement */
			return(1);
		    case TAG_BREAK:
			return(0);
		}
	    }
	    break;
	case NODE_STATEMENT_LIST:
	    /* print_a_node(tree); */
	    /* because BEGIN and END do not have Node_rule_list nature, yet
	     * can have exits and nexts, we special-case a setjmp of rule_tag
	     * here. (jfw) */
	    if (tree == begin_block || tree == end_block)
	    {
		switch (setjmp(rule_tag))
		{
		    case TAG_CONTINUE:	/* next */
			panic("unexpected next");
			return(1);
		    case TAG_BREAK:
			return(0);
		}
	    }
	    for (t = tree; t != NULL; t = t->rnode)
	    {
		DEBUG(("Statements:%p", (FPTR) t->lnode));
		(void) interpret(t->lnode);
	    }
	    break;
	case NODE_K_IF:
	    DEBUG(("IF:%p", (FPTR) tree->lnode));
	    if (eval_condition(tree->lnode))
	    {
		DEBUG(("True:%p", (FPTR) tree->rnode->lnode));
		(void) interpret(tree->rnode->lnode);
	    }
	    else
	    {
		DEBUG(("False:%p", (FPTR) tree->rnode->rnode));
		(void) interpret(tree->rnode->rnode);
	    }
	    break;
	case NODE_K_WHILE:
	    PUSH_BINDING(loop_tag_stack, loop_tag);

	    DEBUG(("WHILE:%p", (FPTR) tree->lnode));
	    while (eval_condition(tree->lnode))
	    {
		switch (setjmp(loop_tag))
		{
		    case 0:	/* normal non-jump */
			DEBUG(("DO:%p", (FPTR) tree->rnode));
			(void) interpret(tree->rnode);
			break;
		    case TAG_CONTINUE:	/* continue statement */
			break;
		    case TAG_BREAK:	/* break statement */
			RESTORE_BINDING(loop_tag_stack, loop_tag);
			return(1);
#ifndef FAST
		    default:
			panic("Bad setjmp return (WHILE) - interpret()");
#endif
		}
	    }
	    RESTORE_BINDING(loop_tag_stack, loop_tag);
	    break;
	case NODE_K_FOR:
	    PUSH_BINDING(loop_tag_stack, loop_tag);
	    DEBUG(("FOR:%p", (FPTR) tree->forloop->init));
	    (void) interpret(tree->forloop->init);
	    DEBUG(("FOR.WHILE:%p", (FPTR) tree->forloop->cond));
	    while (eval_condition(tree->forloop->cond))
	    {
		switch (setjmp(loop_tag))
		{
		    case 0:	/* normal non-jump */
			DEBUG(("FOR.DO:%p", (FPTR) tree->lnode));
			(void) interpret(tree->lnode);
			/* fall through */
		    case TAG_CONTINUE:	/* continue statement */
			DEBUG(("FOR.INCR:%p", (FPTR)tree->forloop->incr));
			(void) interpret(tree->forloop->incr);
			break;
		    case TAG_BREAK:	/* break statement */
			RESTORE_BINDING(loop_tag_stack, loop_tag);
			return(1);
#ifndef FAST
		    default:
			panic("Bad setjmp return (FOR.WHILE) - interpret()");
#endif
		}
	    }
	    RESTORE_BINDING(loop_tag_stack, loop_tag);
	    break;
	case NODE_K_ARRAYFOR:
	    PUSH_BINDING(loop_tag_stack, loop_tag);
	    DEBUG(("AFOR.VAR:%p", (FPTR) tree->forloop->init));
	    lhs = get_lhs(tree->forloop->init);
	    do_deref();
	    for (l = assoc_scan(tree->forloop->incr); l; l = assoc_next(l))
	    {
		*lhs = dupnode(l->retval);
		DEBUG(("AFOR.NEXTIS:%p", (FPTR) *lhs));
		switch (setjmp(loop_tag))
		{
		    case 0:
			DEBUG(("AFOR.DO:%p", (FPTR) tree->lnode));
			(void) interpret(tree->lnode);
		    case TAG_CONTINUE:
			break;

		    case TAG_BREAK:
			RESTORE_BINDING(loop_tag_stack, loop_tag);
			return(1);
#ifndef FAST
		    default:
			panic("Bad setjmp return (AFOR.NEXT) - interpret()");
#endif
		}
	    }
	    RESTORE_BINDING(loop_tag_stack, loop_tag);
	    break;
	case NODE_K_BREAK:
	    DEBUG(("BREAK:"));
	    if (loop_tag_valid == 0)	/* jfw */
		panic("unexpected break or continue");
	    longjmp(loop_tag, TAG_BREAK);
	    break;
	case NODE_K_CONTINUE:
	    DEBUG(("CONTINUE:"));
	    if (loop_tag_valid == 0)	/* jfw */
		panic("unexpected break or continue");
	    longjmp(loop_tag, TAG_CONTINUE);
	    break;
	case NODE_K_PRINT:
	    DEBUG(("PRINT:%p", (FPTR) tree));
	    (void) hack_print_node(tree);
	    break;
	case NODE_K_PRINTF:
	    DEBUG(("PRINTF:%p", (FPTR) tree));
	    (void) do_printf(tree);
	    break;
	case NODE_K_NEXT:
	    DEBUG(("NEXT:"));
	    longjmp(rule_tag, TAG_CONTINUE);
	    break;
	case NODE_K_EXIT:
	    /* The unix awk doc says to skip the rest of the input.  Does
	     * that mean after performing all the rules on the current line?
	     * Unix awk quits immediately, so this does too. */
	    /* The UN*X exit can also take an optional arg return code.  We
	     * don't */
	    /* Well, we parse it, but never *DO* it */
	    DEBUG(("EXIT:"));
	    longjmp(rule_tag, TAG_BREAK);
	    break;
	case NODE_K_DELETE:
	    assoc_lookup(tree->lnode, tree->rnode, ASSOC_DELETE);
	    break;
	default:
	    /* Appears to be an expression statement.  Throw away the value. */
	    DEBUG(("Exp:"));
	    (void) tree_eval(tree);
	    break;
    }
    return(1);
}


/* evaluate a subtree, allocating strings on a temporary stack. */
/* This used to return a whole NODE, instead of a ptr to one, but that
   led to lots of obnoxious copying.  I got rid of it (JF) */

NODE * PASCAL tree_eval(NODE *tree)
{
    register NODE    *r, *t1, *t2; /* return value and temporary subtrees */
    register NODE   **lhs;
    auto     AWKNUM   x;	   /* Why are these static? */

    if (tree == NULL)
    {
	DEBUG(("NULL"));
	return(Nnull_string);
    }
    switch (tree->type)
    {
	    /* trivial data */
	case NODE_STRING:
	    DEBUG(("DATA:(string) %p [%s]", (FPTR) tree, force_string(tree)->stptr));
	    return(tree);
	case NODE_NUMBER:
	    DEBUG(("DATA:(number) %p [%g]", (FPTR) tree, force_number(tree)));
	    return(tree);
	case NODE_REGEXP:
	    DEBUG(("DATA:(regexp) %p", (FPTR) tree));
	    return(tree);

	    /* Builtins */
	case NODE_BUILTIN:
	    DEBUG(("builtin:%p", (FPTR) tree));
	    return((*tree->proc)(tree->subnode));

	case NODE_CONDEXP:
	    DEBUG(("condexp:%p", (FPTR) tree));
	    if (eval_condition(tree->lnode))
	    {
		DEBUG(("True:%p", (FPTR) tree->rnode->lnode));
		r = tree->rnode->lnode;
	    }
	    else
	    {
		DEBUG(("False:%p", (FPTR) tree->rnode->rnode));
		r = tree->rnode->rnode;
	    }
	    return(r);


	    /* unary operations */
	case NODE_VAR:
	case NODE_SUBSCRIPT:
	case NODE_FIELD_SPEC:
	    DEBUG(("var_type ref:%p", (FPTR) tree));
	    lhs = get_lhs(tree);
	    return(*lhs);

	case NODE_PREINCREMENT:
	case NODE_PREDECREMENT:
	    DEBUG(("+-X:%p", (FPTR) tree));
	    lhs = get_lhs(tree->subnode);
	    assign_number(lhs, force_number(*lhs) +
			       (tree->type == NODE_PREINCREMENT ? 1.0 : -1.0));
	    return(*lhs);

	case NODE_POSTINCREMENT:
	case NODE_POSTDECREMENT:
	    DEBUG(("X+-:%p", (FPTR) tree));
	    lhs = get_lhs(tree->subnode);
	    x = force_number(*lhs);
	    assign_number(lhs, x +
			      (tree->type == NODE_POSTINCREMENT ? 1.0 : -1.0));
	    return(tmp_number(x));

	case NODE_UNARY_MINUS:
	    DEBUG(("UMINUS:%p", (FPTR) tree));
	    return(tmp_number(-force_number(tree_eval(tree->subnode))));

	    /* assignments */
	case NODE_ASSIGN:
	    DEBUG(("ASSIGN:%p", (FPTR) tree));
	    r = tree_eval(tree->rnode);
	    lhs = get_lhs(tree->lnode);
	    *lhs = dupnode(r);
	    do_deref();
	    if (tree->lnode->type == NODE_FIELD_SPEC)
		field_spec_changed(tree->lnode->lnode->numbr);
	    return(r);

	    /* other assignment types are easier because they are numeric */
	case NODE_ASSIGN_EXPONENTIAL:
	    r = tree_eval(tree->rnode);
	    lhs = get_lhs(tree->lnode);
	    assign_number(lhs, pow(force_number(*lhs), force_number(r)));
	    do_deref();
	    if (tree->lnode->type == NODE_FIELD_SPEC)
		field_spec_changed(tree->lnode->lnode->numbr);
	    return(r);

	case NODE_ASSIGN_TIMES:
	    r = tree_eval(tree->rnode);
	    lhs = get_lhs(tree->lnode);
	    assign_number(lhs, force_number(*lhs) * force_number(r));
	    do_deref();
	    if (tree->lnode->type == NODE_FIELD_SPEC)
		field_spec_changed(tree->lnode->lnode->numbr);
	    return(r);

	case NODE_ASSIGN_QUOTIENT:
	    r = tree_eval(tree->rnode);
	    lhs = get_lhs(tree->lnode);
	    assign_number(lhs, force_number(*lhs) / force_number(r));
	    do_deref();
	    if (tree->lnode->type == NODE_FIELD_SPEC)
		field_spec_changed(tree->lnode->lnode->numbr);
	    return(r);

	case NODE_ASSIGN_MOD:
	    r = tree_eval(tree->rnode);
	    lhs = get_lhs(tree->lnode);
	    assign_number(lhs, (AWKNUM) (((int) force_number(*lhs)) % ((int) force_number(r))));
	    do_deref();
	    if (tree->lnode->type == NODE_FIELD_SPEC)
		field_spec_changed(tree->lnode->lnode->numbr);
	    return(r);

	case NODE_ASSIGN_PLUS:
	    r = tree_eval(tree->rnode);
	    lhs = get_lhs(tree->lnode);
	    assign_number(lhs, force_number(*lhs) + force_number(r));
	    do_deref();
	    if (tree->lnode->type == NODE_FIELD_SPEC)
		field_spec_changed(tree->lnode->lnode->numbr);
	    return(r);

	case NODE_ASSIGN_MINUS:
	    r = tree_eval(tree->rnode);
	    lhs = get_lhs(tree->lnode);
	    assign_number(lhs, force_number(*lhs) - force_number(r));
	    do_deref();
	    if (tree->lnode->type == NODE_FIELD_SPEC)
		field_spec_changed(tree->lnode->lnode->numbr);
	    return(r);
    }
    /* Note that if TREE is invalid, gAWK will probably bomb in one of these
     * tree_evals here.  */
    /* evaluate subtrees in order to do binary operation, then keep going */
    t1 = tree_eval(tree->lnode);
    t2 = tree_eval(tree->rnode);

    switch (tree->type)
    {
	case NODE_CONCAT:
	    t1 = force_string(t1);
	    t2 = force_string(t2);
	    r  = (NODE *) obstack_alloc(&temp_strings, sizeof(NODE));
	    r->type = NODE_TEMP_STRING;
	    r->stlen = t1->stlen + t2->stlen;
	    r->stref = 1;
	    r->stptr = (char *) obstack_alloc(&temp_strings, r->stlen + 1);
	    memcpy(r->stptr, t1->stptr, t1->stlen);
	    memcpy(r->stptr + t1->stlen, t2->stptr, t2->stlen);
	    r->stptr[r->stlen] = EOS;
	    return(r);

	case NODE_TIMES:
	    return(tmp_number(force_number(t1) * force_number(t2)));

	case NODE_EXPONENTIAL:
	    return(tmp_number(pow(force_number(t1), force_number(t2))));

	case NODE_QUOTIENT:
	    x = force_number(t2);
	    if (x == (AWKNUM) 0)
		return(tmp_number((AWKNUM) 0.0));
	    else
		return(tmp_number(force_number(t1) / x));

	case NODE_MOD:
	    x = force_number(t2);
	    if (x == (AWKNUM) 0)
		return(tmp_number((AWKNUM) 0.0));
	    return(tmp_number((AWKNUM)	/* uggh... */
			      (((int) force_number(t1)) % ((int) x))));

	case NODE_PLUS:
	    return(tmp_number(force_number(t1) + force_number(t2)));

	case NODE_MINUS:
	    return(tmp_number(force_number(t1) - force_number(t2)));

#ifndef FAST
	default:
	    panic("internal error: illegal numeric operation - tree_eval()");
#endif
    }
    return(0);
}


/* This returns a POINTER to a node pointer.  *get_lhs(ptr) is the current  */
/* value of the var, or where to store the var's new value.                 */

NODE ** PASCAL get_lhs(NODE *ptr)
{
    register NODE     **aptr;
    register int	num;

#ifndef FAST
    if (ptr == NULL)
	panic("NULL pointer passed to get_lhs()");
#endif
    deref = NULL;
    switch (ptr->type)
    {
	case NODE_VAR:
	    deref = ptr->var_value;
	    return(&ptr->var_value);

	case NODE_FIELD_SPEC:
	    num = (int) force_number(tree_eval(ptr->lnode));
	    if (num < 0)
		num = 0;	/* JF what should I do? */
	    if (num > f_arr_siz)
		set_field(num, f_empty, 0);	/* jfw: so blank_strings can
						 * be simpler */
	    deref = NULL;
	    return(&fields_arr[num]);

	case NODE_SUBSCRIPT:
	    aptr  = assoc_lookup(ptr->lnode, ptr->rnode, ASSOC_CREATE);
	    deref = *aptr;
	    return(aptr);
    }
#ifndef FAST
    panic("Bad node type (%d) in get_lhs()", ptr->type);
#endif
    return(NULL);
}


VOID PASCAL do_deref(void)
{
    if (deref)
    {
	switch (deref->type)
	{
	    case NODE_STRING:
		if (deref != Nnull_string)
		    FREE_ONE_REFERENCE(deref);
		break;
	    case NODE_NUMBER:
		free((char *) deref);
		break;
#ifndef FAST
	    default:
		panic("Bad node type (%d) in do_deref()", deref->type);
#endif
	}
	deref = 0;
    }
}


/* This makes numeric operations slightly more efficient.
   Just change the value of a numeric node, if possible */

VOID PASCAL assign_number(NODE **ptr, AWKNUM value)
{
    switch ((*ptr)->type)
    {
	case NODE_STRING:
	    if (*ptr != Nnull_string)
		FREE_ONE_REFERENCE(*ptr);
	case NODE_TEMP_STRING:	      /* jfw: dont crash if we say $2 += 4 */
	    *ptr = make_number(value);
	    return;
	case NODE_NUMBER:
	    (*ptr)->numbr = value;
	    deref = 0;
	    break;
#ifndef FAST
	default:
	    panic("Bad node type (%d) in assign_number()", (*ptr)->type);
#endif
    }
}



/* Routines to deal with fields */
#define ORIG_F			    30


VOID PASCAL init_fields(void)
{
    register NODE     **tmp;
    register NODE      *xtmp;

    f_arr_siz = ORIG_F;
    fields_arr = (NODE **) malloc(ORIG_F * sizeof(NODE *));
    fields_nodes = (NODE *) malloc(ORIG_F * sizeof(NODE));
    tmp = &fields_arr[f_arr_siz];
    xtmp = &fields_nodes[f_arr_siz];
    while (--tmp >= &fields_arr[0])
    {
	--xtmp;
	*tmp = xtmp;
	xtmp->type = NODE_TEMP_STRING;
	xtmp->stlen = 0;
	xtmp->stref = 1;
	xtmp->stptr = f_empty;
    }
    return;
}


VOID PASCAL blank_fields(void)
{
    register NODE	 **tmp;

    tmp = &fields_arr[f_arr_siz];
    while (--tmp >= &fields_arr[0])
    {
	switch ((*tmp)->type)
	{
	    case NODE_NUMBER:
		free((char *) *tmp);
		*tmp = &fields_nodes[tmp - fields_arr];
		break;
	    case NODE_STRING:
		if (*tmp != Nnull_string)
		    FREE_ONE_REFERENCE(*tmp);
		*tmp = &fields_nodes[tmp - fields_arr];
		break;
	    case NODE_TEMP_STRING:
		break;
#ifndef FAST
	    default:
		panic("Invalid node type (%d) in blank_fields()",
		      tmp[0]->type);
#endif
	}
	if ((*tmp)->stptr != f_empty)
	{			/* jfw */
	    /* Then it was assigned a string with set_field */
	    /* out of a private buffer to inrec, so don't free it */
	    (*tmp)->stptr = f_empty;
	    (*tmp)->stlen = 0;
	    (*tmp)->stref = 1;
	}
    }
    return;
}


/* Danger!  Must only be called for fields we know have just been blanked,
   or fields we know don't exist yet.  */

VOID PASCAL set_field(int n, char *str, int len)
{
    if (n > f_arr_siz)
    {
	int             t;

	fields_arr = (NODE **) realloc((char *) fields_arr,
				       (n + 1) * sizeof(NODE *));
	fields_nodes = (NODE *) realloc((char *) fields_nodes,
					(n + 1) * sizeof(NODE));
	if (NULL == fields_arr || NULL == fields_nodes)
	    panic("Out of memory in set_field()");
	for (t = f_arr_siz; t <= n; t++)
	{
	    fields_arr[t] = &fields_nodes[t];
	    fields_nodes[t].type = NODE_TEMP_STRING;
	    fields_nodes[t].stlen = 0;
	    fields_nodes[t].stref = 1;
	    fields_nodes[t].stptr = f_empty;
	}
	f_arr_siz = n + 1;
    }
    fields_nodes[n].stlen = len;
    if (n == 0)
    {
	fields_nodes[n].stptr = (char *) obstack_alloc(&other_stack, len + 1);
	memcpy(fields_nodes[n].stptr, str, len);
	fields_nodes[n].stptr[len] = EOS;
    }
    else
    {
	fields_nodes[n].stptr = str;
	str[len] = EOS;
    }
    return;
}


/* Nodes created with this will go away when the next input line is read */

NODE * PASCAL field_string(char *s, int len)
{
    register NODE  *r;

    r = (NODE *) obstack_alloc(&other_stack, sizeof(NODE));
    r->type = NODE_TEMP_STRING;
    r->stref = 1;
    r->stlen = len;
    r->stptr = (char *) obstack_alloc(&other_stack, len + 1);
    memcpy(r->stptr, s, len);
    r->stptr[len] = EOS;

    return(r);
}


/* Someone assigned a value to $(something).  Fix up $0 to be right */

VOID PASCAL fix_fields(void)
{
    register int    tlen;
    register NODE  *tmp;
    NODE           *ofs;
    char           *ops;
    register char  *cops;
    register NODE **ptr, **maxp;

    maxp = NULL;
    tlen = 0;
    ofs = force_string(*get_lhs(OFS_node));
    ptr = &fields_arr[f_arr_siz];
    while (--ptr > &fields_arr[0])
    {
	tmp = force_string(*ptr);
	tlen += tmp->stlen;
	if (tmp->stlen && !maxp)
	    maxp = ptr;
    }
    if (!maxp)
    {
	if (fields_arr[0] != fields_nodes)
	    FREE_ONE_REFERENCE(fields_arr[0]);
	fields_arr[0] = Nnull_string;
	return;
    }

    tlen += ((maxp - fields_arr) - 1) * ofs->stlen;
    ops = (char *) malloc(tlen + 1);
    cops = ops;
    for (ptr = &fields_arr[1]; ptr <= maxp; ptr++)
    {
	tmp = force_string(*ptr);
	memcpy(cops, tmp->stptr, tmp->stlen);
	cops += tmp->stlen;
	if (ptr != maxp)
	{
	    memcpy(cops, ofs->stptr, ofs->stlen);
	    cops += ofs->stlen;
	}
    }
    tmp = newnode(NODE_STRING);
    tmp->stptr = ops;
    tmp->stlen = tlen;
    tmp->stref = 1;
    tmp->stptr[tlen] = EOS;

    /* don't free unless it's new */
    if (fields_arr[0] != fields_nodes)
	FREE_ONE_REFERENCE(fields_arr[0]);
    fields_arr[0] = tmp;
    return;
}



/* Is TREE true or false?  Returns 0==false, non-zero==true */

int PASCAL eval_condition(NODE *tree)
{
    register int	       di;
    register NODE	      *t1, *t2;
    auto     NODE	     **tmp;
    auto     char	      *err;

    if (tree == NULL)		/* Null trees are the easiest kinds */
	return(1);
    switch (tree->type)
    {
	    /* Maybe it's easy; check and see. */
	    /* BEGIN and END are always false */
	case NODE_K_BEGIN:
	    return(0);

	case NODE_K_END:
	    return(0);

	case NODE_MEMBER_COND:
	    tmp = assoc_lookup(tree->lnode, tree->rnode, ASSOC_TEST);
	    if (NULL == tmp)
		return(FALSE);
	    return(TRUE);

	case NODE_AND:
	    return(eval_condition(tree->lnode)
		       && eval_condition(tree->rnode));

	case NODE_OR:
	    return(eval_condition(tree->lnode)
		       || eval_condition(tree->rnode));

	case NODE_NOT:
	    return(!eval_condition(tree->lnode));

	    /* Node_line_range is kind of like Node_match, EXCEPT: the lnode
	     * field (more properly, the condpair field) is a node of a
	     * Node_cond_pair; whether we evaluate the lnode of that node or
	     * the rnode depends on the triggered word.  More precisely:  if
	     * we are not yet triggered, we tree_eval the lnode; if that
	     * returns true, we set the triggered word.  If we are triggered
	     * (not ELSE IF, note), we tree_eval the rnode, clear triggered
	     * if it succeeds, and perform our action (regardless of success
	     * or failure).  We want to be able to begin and end on a single
	     * input record, so this isn't an ELSE IF, as noted above. This
	     * feature was implemented by John Woods, jfw@eddie.mit.edu,
	     * during a rainy weekend. */
	case NODE_LINE_RANGE:
	    if (!tree->triggered)
		if (!eval_condition(tree->condpair->lnode))
		    return(0);
		else
		    tree->triggered = 1;
	    /* Else we are triggered */
	    if (eval_condition(tree->condpair->rnode))
		tree->triggered = 0;
	    return(1);
    }

    /* Could just be J.random expression. in which case, null and 0 are
     * false, anything else is true */

    switch (tree->type)
    {
	case NODE_REGEXP:
	case NODE_MATCH:
	case NODE_NOMATCH:
	case NODE_EQUAL:
	case NODE_NOTEQUAL:
	case NODE_LESS:
	case NODE_GREATER:
	case NODE_LEQ:
	case NODE_GEQ:
	    break;

	default:		/* This is so 'if(iggy)', etc, will work */
	    /* Non-zero and non-empty are true */
	    t1 = tree_eval(tree);
	    switch (t1->type)
	    {
		case NODE_NUMBER:
		    return(t1->numbr != 0.0);
		case NODE_STRING:
		case NODE_TEMP_STRING:
		    return(t1->stlen != 0);
#ifndef FAST
		default:
		    panic("Bad node type (%d) in eval_condition()", t1->type);
#endif
	    }
    }
    /* couldn't fob it off recursively, eval left subtree and see if it's a
     * pattern match operation */

    if (NODE_REGEXP == tree->type)
    {
	return(re_search(tree->rereg, WHOLELINE->stptr, WHOLELINE->stlen,
			 0, WHOLELINE->stlen, NULL) != -1);
    }

    t1 = tree_eval(tree->lnode);

    if (tree->type == NODE_MATCH || tree->type == NODE_NOMATCH)
    {
	t1 = force_string(t1);
	if (NODE_REGEXP == tree->rnode->type)
	{
	    return(re_search(tree->rnode->rereg, t1->stptr,
			      t1->stlen, 0, t1->stlen,
			      NULL) == -1)
			^ (tree->type == NODE_MATCH);
	}
	t2 = tree_eval(tree->rnode);
	t2 = force_string(t2);
	clear_wrk_repat();
	err = re_compile_pattern(t2->stptr, t2->stlen, &wrk_repat);
	if (err)
	    panic("Illegal REGEXP(%s) in condition: %s", t2->stptr, err);
	di = re_search(&wrk_repat, t1->stptr, t1->stlen, 0, t1->stlen, NULL);
	return((-1 == di) ^ (NODE_MATCH == tree->type));
    }

    /* still no luck--- eval the right subtree and try binary ops */

    t2 = tree_eval(tree->rnode);

    di = cmp_nodes(t1, t2);

    switch (tree->type)
    {
	case NODE_EQUAL:
	    return(di == 0);
	case NODE_NOTEQUAL:
	    return(di != 0);
	case NODE_LESS:
	    return(di < 0);
	case NODE_GREATER:
	    return(di > 0);
	case NODE_LEQ:
	    return(di <= 0);
	case NODE_GEQ:
	    return(di >= 0);
#ifndef FAST
	default:
	    panic("unknown conditonal node (%d) in eval_condition()",
		  tree->type);
#endif
    }
    return(0);
}


/* FOO this doesn't properly compare "12.0" and 12.0 etc */
/* or "1E1" and 10 etc */
/* Perhaps someone should fix it.  */
/* Consider it fixed (jfw) */

/* strtod() would have been better, except (1) real awk is needlessly
 * restrictive in what strings it will consider to be numbers, and
 * (2) I couldn't find the public domain version anywhere handy.
 *
 * does the string str have pure-numeric syntax?  don't convert it,
 * assume that atof is better.
 */

int PASCAL is_a_number(char *str)
{
    if (*str == 0)
	return(1);		 /* null string has numeric value of 0 */
    /* This is still a bug: in real awk, an explicit "" string is not treated
     * as a number.  Perhaps it is only variables that, when empty, are also
     * 0s.  This bug-lette here at least lets uninitialized variables to
     * compare equal to zero like they should. */
    if (*str == '-')
	str++;
    if (*str == 0)
	return(0);
    /* must be either . or digits (.4 is legal) */
    if (*str != '.' && !isdigit(*str))
	return(0);
    while (isdigit(*str))
	str++;
    if (*str == '.')
    {
	str++;
	while (isdigit(*str))
	    str++;
    }
    /* curiously, real awk DOESN'T consider "1E1" to be equal to 10! Or even
     * equal to 1E1 for that matter!  For a laugh, try: awk 'BEGIN {if ("1E1"
     * == 1E1) print "eq"; else print "neq";exit}' Since this behavior is
     * QUITE curious, I include the code for the adventurous.  One might also
     * feel like skipping leading whitespace (awk doesn't) and allowing a
     * leading + (awk doesn't). #ifdef Allow_Exponents if (*str == 'e' ||
     * *str == 'E') { str++; if (*str == '+' || *str == '-') str++; if
     * (!isdigit(*str)) return 0; while (isdigit(*str)) str++; } #endif /* if
     * we have digested the whole string, we are successful */
    return(*str == 0);
}


int PASCAL cmp_nodes(NODE *t1, NODE *t2)
{
    register int    di;
    register AWKNUM d;


    if (t1 == t2)
    {
	return(0);
    }

#ifndef FAST
    if (!t1 || !t2)
	panic("NULL pointer passed to cmp_nodes()");
#endif

    if (t1->type == NODE_NUMBER && t2->type == NODE_NUMBER)
    {
	d = t1->numbr - t2->numbr;
	if (d < 0.0)
	    return(-1);
	if (d > 0.0)
	    return(1);
	return(0);
    }
    t1 = force_string(t1);
    t2 = force_string(t2);

    /* "real" awk treats things as numbers if they both "look" like numbers. */
    if (*t1->stptr && *t2->stptr && is_a_number(t1->stptr)
				 && is_a_number(t2->stptr))
    {
	d = atof(t1->stptr) - atof(t2->stptr);
	if (d < 0.0)
	    return(-1);
	if (d > 0.0)
	    return(1);
	return(0);
    }

    di = strncmp(t1->stptr, t2->stptr, min(t1->stlen, t2->stlen));
    if (di == 0)
	di = t1->stlen - t2->stlen;
    if (di > 0)
	return(1);
    if (di < 0)
	return(-1);
    return(0);
}


/* routines for associative arrays.  SYMBOL is the address of the node
   (or other pointer) being dereferenced.  SUBS is a number or string
   used as the subscript. */


/* Flush all the values in symbol[] before doing a split() */

VOID PASCAL assoc_clear(NODE *symbol)
{
    register int	 i;
    auto     AHASH	*bucket, *next;

    if (symbol->var_array == NULL)
	return;
    for (i = 0; i < ASSOC_HASHSIZE; ++i)
    {
	for (bucket = symbol->var_array[i]; bucket; bucket = next)
	{
	    next = bucket->next;
	    deref = bucket->name;
	    do_deref();
	    deref = bucket->value;
	    do_deref();
	    free((void *) bucket);
	}
	symbol->var_array[i] = NULL;
    }
    return;
}


/* Find SYMBOL[SUBS] in the assoc array.  Install it with value "" if it    */
/* isn't there.  Returns a pointer ala get_lhs to where its value is stored */

NODE ** PASCAL assoc_lookup(NODE *symbol, NODE *subs, int operation)
{
    register int	   hash1 = 0, i;
    auto     AHASH	  *bucket, *prev_bucket;
    auto     NODE	  *tmp, *tmp1;
    auto     NODE	  *subsep;
    auto     char	   wrk[MAX_SUBSCRIPT_LEN], *pwrk;

    switch (subs->type)
    {
	case NODE_NUMBER:
	    i	 = (int) subs->numbr;
	    subs = tmp_string(wrk, sprintf(wrk, "%d", i));
	    break;

	case NODE_EXPRESSION_LIST:
	    subsep = force_string(SUBSEP_node->var_value);
	    pwrk   = wrk;
	    tmp    = subs;
	    while (tmp)
	    {
		tmp1	= tree_eval(tmp->lnode);
		if (NULL == tmp1)
		    panic("Invalid subscript expression in assoc_lookup()");
		tmp1	= force_string(tmp1);
		memcpy(pwrk, tmp1->stptr, tmp1->stlen);
		pwrk   += tmp1->stlen;
		if (subsep->stlen > 0)
		{
		    memcpy(pwrk, subsep->stptr, subsep->stlen);
		    pwrk += subsep->stlen;
		}
		tmp	= tmp->rnode;
	    }
	    if (pwrk == wrk)
		*pwrk = EOS;
	    else
		*(pwrk - subsep->stlen) = EOS;
	    subs  = tmp_string(wrk, strlen(wrk));
					   /* intentional fall thru - BW */
	default:
	    subs = force_string(subs);
	    break;
    }
    for (i = 0; i < subs->stlen; i++)
	hash1 = HASHSTEP(hash1, subs->stptr[i]);

    hash1 = STIR_BITS(hash1);
    hash1 = MAKE_POS(hash1) % ASSOC_HASHSIZE;

    /* this table really should grow dynamically */
    if (symbol->var_array == NULL)
    {
	if (ASSOC_TEST == operation || ASSOC_DELETE == operation)
	    return(NULL);
	symbol->var_array = (AHASH **) malloc(sizeof(AHASH *) * ASSOC_HASHSIZE);
	for (i = 0; i < ASSOC_HASHSIZE; i++)
	    symbol->var_array[i] = NULL;
    }
    else
    {
	bucket	    = symbol->var_array[hash1];
	prev_bucket = NULL;
	while (bucket)
	{
	    if (0 == cmp_nodes(bucket->name, subs))
	    {
		if (ASSOC_DELETE == operation)
		{
		    if (prev_bucket)
			prev_bucket->next = bucket->next;
		    else
			symbol->var_array[hash1] = NULL;
		    deref = bucket->name;
		    do_deref();
		    deref = bucket->value;
		    do_deref();
		    free(bucket);
		    return(NULL);
		}
		return(&(bucket->value));
	    }
	    prev_bucket = bucket;
	    bucket	= bucket->next;
	}
#if 0
	for (bucket = symbol->var_array[hash1]; bucket; bucket = bucket->next)
	{
	    if (0 == cmp_nodes(bucket->name, subs))
		return(&(bucket->value));
	}
#endif
    }
    if (ASSOC_TEST == operation)
	return(NULL);
    bucket = (AHASH *) malloc(sizeof(AHASH));
    if (NULL == bucket)
	panic("Out of memory in function assoc_lookup()");
    bucket->symbol = symbol;
    bucket->name   = dupnode(subs);
    bucket->value  = Nnull_string;
    bucket->next   = symbol->var_array[hash1];
    symbol->var_array[hash1] = bucket;
    return(&(bucket->value));
}


STATIC struct search * NEAR PASCAL assoc_scan(NODE *symbol)
{
    auto     struct search    *lookat;

    if (!symbol->var_array)
	return(NULL);
    lookat = (struct search *) obstack_alloc(&other_stack,
					     sizeof(struct search));
    /* lookat->symbol = symbol; */
    lookat->numleft = ASSOC_HASHSIZE;
    lookat->arr_ptr = symbol->var_array;
    lookat->bucket  = symbol->var_array[0];
    return(assoc_next(lookat));
}


STATIC struct search * NEAR PASCAL assoc_next(struct search *lookat)
{
    for (; lookat->numleft; lookat->numleft--)
    {
	while (lookat->bucket != 0)
	{
	    lookat->retval = lookat->bucket->name;
	    lookat->bucket = lookat->bucket->next;
	    return(lookat);
	}
	lookat->bucket = *++(lookat->arr_ptr);
    }
    return(NULL);
}


AWKNUM PASCAL force_number(NODE *n)
{
    if (n)
    {
	switch (n->type)
	{
	    case NODE_NUMBER:
		return(n->numbr);
	    case NODE_STRING:
	    case NODE_TEMP_STRING:
		return(atof(n->stptr));
	}
    }
    panic("Bad node type (%d) in force_number()", n->type);
}


NODE * PASCAL force_string(NODE *s)
{
    auto     int       num;

    if (s)
    {
	switch (s->type)
	{
	    case NODE_STRING:
	    case NODE_TEMP_STRING:
		return(s);
	    case NODE_NUMBER:
		if ((*get_lhs(OFMT_node))->type != NODE_STRING)
		    panic("Insane value for OFMT in force_string()");
		dumb[1].lnode = s;
		return(do_sprintf(&dumb[0]));
	    case NODE_FIELD_SPEC:
		num = (int) force_number(tree_eval(s->lnode));
		if (num > f_arr_siz)
		    set_field(num, f_empty, 0);
		return(force_string(fields_arr[num]));
	}
    }
    panic("Bad node type (%d) in force_string()", s->type);
}
