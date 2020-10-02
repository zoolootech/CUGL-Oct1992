
/***************************************************************************/
/*									   */
/*		       awk.h -- Definitions for gawk.			   */
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

#ifndef AWK_H
#define AWK_H


#define AWKNUM		      double

#include "regex.h"

#include <ctype.h>
#define is_identchar(c)       (isalnum(c) || (c) == '_')

#define obstack_chunk_alloc    malloc
#define obstack_chunk_free     free
#include "obstack.h"

#define PROFILER	       TRUE

#if PROFILER
#define STATIC
#else
#define STATIC		       static
#endif

#define EOS		       '\0'
#define FPTR		       void far *

#if defined(__POWERC)
#define NEAR
#define PASCAL
#define CDECL
#define VOID		       int
#else
#define NEAR		       near
#define PASCAL		       pascal
#define CDECL		       cdecl
#define VOID		       void
#endif

#define TABSTOPS	       8

#define MAXDIM(array)	       (sizeof(array) / sizeof(array[0]))

#define MAX_SUBSCRIPT_LEN      80

#ifndef TRUE
#define TRUE		       1
#define FALSE		       0
#endif

typedef int		       BOOL;

/*  illegal node entry							    */
#define NODE_ILLEGAL		0

/*  binary operators - lnode and rnode are the expressions to work on	    */
#define NODE_EXPONENTIAL	1
#define NODE_TIMES		2
#define NODE_QUOTIENT		3
#define NODE_MOD		4
#define NODE_PLUS		5
#define NODE_MINUS		6
#define NODE_COND_PAIR		7
#define NODE_SUBSCRIPT		8
#define NODE_CONCAT		9

/* unary operators - subnode is the expression to work on		    */
#define NODE_PREINCREMENT	10
#define NODE_PREDECREMENT	11
#define NODE_POSTINCREMENT	12
#define NODE_POSTDECREMENT	13
#define NODE_UNARY_MINUS	14
#define NODE_FIELD_SPEC 	15

/* assignment operators - lnode is the var to assign to rnode is the exp    */
#define NODE_ASSIGN		16
#define NODE_ASSIGN_EXPONENTIAL 17
#define NODE_ASSIGN_TIMES	18
#define NODE_ASSIGN_QUOTIENT	19
#define NODE_ASSIGN_MOD 	20
#define NODE_ASSIGN_PLUS	21
#define NODE_ASSIGN_MINUS	22

/*  boolean binary operators - lnode and rnode are expressions		    */
#define NODE_AND		23
#define NODE_OR 		24

/*  relational binary operators - lnode and rnode are exp to comapre	    */
#define NODE_EQUAL		25
#define NODE_NOTEQUAL		26
#define NODE_LESS		27
#define NODE_GREATER		28
#define NODE_LEQ		29
#define NODE_GEQ		30

/*  relational unary operators - subnode is the expression to operate on    */
#define NODE_NOT		31

/*  match operators - lnode is an exp, rnode is a regular expression	    */
#define NODE_MATCH		32
#define NODE_NOMATCH		33

/*  data items								    */
#define NODE_STRING		34  /* stptr and stlen			    */
#define NODE_TEMP_STRING	35  /* stptr and stlen			    */
#define NODE_NUMBER		36  /* numbr				    */

/*  program structures							    */
#define NODE_RULE_LIST		37  /* lnode is rule, rnode rest of list    */
#define NODE_RULE_NODE		38  /* lnode is cond, rnode is statement    */
#define NODE_STATEMENT_LIST	39  /* lnode is stmt, rnode is more list    */
#define NODE_IF_BRANCHES	40  /* lnode is TRUE side, rnode is FALSE   */
#define NODE_EXPRESSION_LIST	41  /* lnode is an exp, rnode is more list  */

/*  keywords								    */
#define NODE_K_BEGIN		42  /* nothing on either side		    */
#define NODE_K_END		43  /* nothing on either side		    */
#define NODE_K_IF		44  /* lnode is cond, rnode is if_branches  */
#define NODE_K_WHILE		45  /* lnode is cond, rnode stuff to run    */
#define NODE_K_FOR		46  /* lnode is for_struct, rnode run stuff */
#define NODE_K_ARRAYFOR 	47  /* lnode is for_struct, rnode run stuff */
#define NODE_K_BREAK		48  /* nothing on either side		    */
#define NODE_K_CONTINUE 	49  /* nothing on either side		    */
#define NODE_K_PRINT		50  /* lnode is exp_list, rnode is redirect */
#define NODE_K_PRINTF		51  /* lnode is exp_list, rnode is redirect */
#define NODE_K_NEXT		52  /* nothing on either side		    */
#define NODE_K_EXIT		53  /* subnode is return value, or NULL     */
#define NODE_K_DELETE		54  /* lnode is symbol, rnode is subscript  */
#define NODE_GETLINE		55  /* lnode is var, rnode is redirect	    */

/*  I/O redirection for print statements				    */
#define NODE_REDIRECT_INPUT	56  /* subnode is where to redirect	    */
#define NODE_REDIRECT_OUTPUT	57  /* subnode is where to redirect	    */
#define NODE_REDIRECT_APPEND	58  /* subnode is where to redirect	    */
#define NODE_REDIRECT_PIPE	59  /* subnode is where to redirect	    */

/*  variables								    */
#define NODE_VAR		60  /* rnode is value, lnode array stuff    */
#define NODE_VAR_ARRAY		61  /* array is ptr to elements, asize = #  */

/*  builtin functions - subnode is exp_list, proc is function to call	    */
#define NODE_BUILTIN		62

/*  line range pattern							    */
#define NODE_LINE_RANGE 	63

/*  conditional expression						    */
#define NODE_CONDEXP		64  /* lnode is cond, rnode is cond_branches*/
#define NODE_CONDEXP_BRANCHES	65  /* lnode is exp if TRUE, rnode if FALSE */

/*  regular expression							    */
#define NODE_REGEXP		66

/*  membership conditional						    */
#define NODE_MEMBER_COND	67


typedef struct exp_node
{
    int 				      type;
    union
    {
	struct
	{
	    struct exp_node		      *lptr;
	    union
	    {
		struct exp_node 	      *rptr;
		struct exp_node 	   *(PASCAL *pptr)(struct exp_node *);
		REPAT_BUFFER		     *preg;
		struct for_loop_header	     *hd;
		struct ahash		    **av;
		int			      r_ent; /* range entered (jfw) */
	    } r;
	} nodep;
	struct
	{
	    struct exp_node		    **ap;
	    int 			      as;
	} ar;
	struct
	{
	    char			     *sp;
	    int 			      slen, sref;
	} str;
	AWKNUM				      fltnum;
  } sub;
} NODE;

#define lnode		   sub.nodep.lptr
#define rnode		   sub.nodep.r.rptr

#define subnode 	   lnode
#define proc		   sub.nodep.r.pptr

#define reexp		   lnode
#define rereg		   sub.nodep.r.preg

#define forsub		   lnode
#define forloop 	   sub.nodep.r.hd

#define array		   sub.ar.ap
#define arrsiz		   sub.ar.as

#define stptr		   sub.str.sp
#define stlen		   sub.str.slen
#define stref		   sub.str.sref

#define numbr		   sub.fltnum

#define var_value	   lnode
#define var_array	   sub.nodep.r.av

#define condpair	   lnode
#define triggered	   sub.nodep.r.r_ent

#define HASHSIZE	   101
#define ASSOC_HASHSIZE	   29
#define STIR_BITS(n)	   ((n) << 5 | (((n) >> 27) & 0x1f))
#define HASHSTEP(old, c)   ((old << 1) + c)
#define MAKE_POS(v)	   (((v) < 0) ? -(v) : (v))

/* the following defines are used in calls to assoc_lookup() to advise the  */
/* function of the type of action desired on the subscripted variable.	    */
#define ASSOC_CREATE	   0	/* if var doesn't exist, create it          */
#define ASSOC_TEST	   1	/* test if var exists, don't create         */
#define ASSOC_DELETE	   2	/* if var exists delete it		    */


typedef struct hashnode 	   HASHNODE;
struct hashnode
{
    HASHNODE *next;
    char *name;
    int length;
    NODE *value;
};


typedef struct ahash		    AHASH;
struct ahash
{
    AHASH	    *next;
    NODE	    *name,
		    *symbol,
		    *value;
};


typedef struct for_loop_header
{
    NODE	    *init;
    NODE	    *cond;
    NODE	    *incr;
} FOR_LOOP_HEADER;


#define ADD_ONE_REFERENCE(s) ++(s)->stref
#define FREE_ONE_REFERENCE(s) {\
  if(s==Nnull_string) {\
    fprintf(stderr,"Free_Nnull_string %d",(s)->stref);\
  }\
  if (--(s)->stref == 0) {\
    free((char *)((s)->stptr));\
    free((char *)s);\
  }\
}

    /* %%% VANDYS:
     *  Can you say "do it with mirrors"?
     *  Sure.  I knew you could.
     *  There isn't exactly a 1:1 correspondence between the BSD
     *   routines & the Sys-V ones, but it's close enough.
     */
#ifndef BSD
#define index strchr
#define bcopy(src, dest, count) memcpy(dest, src, count)
#define bzero(ptr, count) memset(ptr, '\0', count)
#define bcmp memcmp
#endif /* BSD */

#if defined(__TURBOC__) || defined(__POWERC)
#define NEARDATA
#else
#define NEARDATA		NEAR
#endif

#ifdef DRIVER
#define VARCLASS
#define INIT(x) 		= x
#else
#define VARCLASS		extern
#define INIT(x)
#endif

VARCLASS    NODE *	 NEARDATA    expression_value	INIT(NULL);

VARCLASS    REPAT_BUFFER NEARDATA    wrk_repat;
VARCLASS    char	 NEARDATA    wrk_fastmap[FASTMAP_SIZE];

    /* This node is the cannonical null string, used everywhere */
VARCLASS    NODE *	 NEARDATA    Nnull_string	INIT(NULL);
    /* A block of gAWK code to be run before running the program */
VARCLASS    NODE *	 NEARDATA    begin_block	INIT(NULL);
    /* A block of gAWK code to be run after the last input file */
VARCLASS    NODE *	 NEARDATA    end_block		INIT(NULL);
    /* These nodes store all the special variables gAWK uses */
VARCLASS    NODE *	 NEARDATA    FILENAME_node	INIT(NULL);
VARCLASS    NODE *	 NEARDATA    FS_node		INIT(NULL);
VARCLASS    NODE *	 NEARDATA    NF_node		INIT(NULL);
VARCLASS    NODE *	 NEARDATA    RS_node		INIT(NULL);
VARCLASS    NODE *	 NEARDATA    NR_node		INIT(NULL);
VARCLASS    NODE *	 NEARDATA    OFS_node		INIT(NULL);
VARCLASS    NODE *	 NEARDATA    ORS_node		INIT(NULL);
VARCLASS    NODE *	 NEARDATA    OFMT_node		INIT(NULL);
VARCLASS    NODE *	 NEARDATA    FNR_node		INIT(NULL);  /* BW */
VARCLASS    NODE *	 NEARDATA    RLENGTH_node	INIT(NULL);  /* BW */
VARCLASS    NODE *	 NEARDATA    RSTART_node	INIT(NULL);  /* BW */
VARCLASS    NODE *	 NEARDATA    ARGC_node		INIT(NULL);  /* BW */
VARCLASS    NODE *	 NEARDATA    ARGV_node		INIT(NULL);  /* BW */
VARCLASS    NODE *	 NEARDATA    SUBSEP_node	INIT(NULL);  /* BW */

VARCLASS    FILE *	 NEARDATA    input_file 	INIT(NULL);
VARCLASS    char	 NEARDATA    current_filename[64] INIT("");

VARCLASS    HASHNODE *	 NEARDATA    variables[HASHSIZE];

    /* This dumb kludge is used by force_string to turn a floating point  */
    /* number into a string						  */
VARCLASS    NODE	 NEARDATA    dumb[2];

VARCLASS    NODE **	 NEARDATA    fields_arr 	INIT(NULL);
VARCLASS    NODE *	 NEARDATA    fields_nodes	INIT(NULL);
VARCLASS    int 	 NEARDATA    f_arr_siz		INIT(0);
VARCLASS    char	 NEARDATA    f_empty[2] 	INIT("");

    /* Temporary nodes are stored here.  ob_dummy is a dummy object used   */
    /* to keep the obstack library from free()ing up the entire stack.	   */
VARCLASS    OBSTACK	 NEARDATA    temp_strings;
VARCLASS    char *	 NEARDATA    ob_dummy		INIT(NULL);

    /* The parse tree and field nodes are stored here.	Parse_end is a	   */
    /* dummy item used to free up unneeded fields without freeing the	   */
    /* program being run						   */
VARCLASS    OBSTACK	 NEARDATA    other_stack;
VARCLASS    char *	 NEARDATA    parse_end		INIT(NULL);

    /* The name the program was invoked under, for error messages	   */
VARCLASS    char *	 NEARDATA    myname		INIT(NULL);

    /* We can't dereference a variable until after we've given it	   */
    /* its new value. This variable points to the value we have to free up */
VARCLASS    NODE *	 NEARDATA    deref		INIT(NULL);

    /* Switches used for debugging */
VARCLASS    int 	 NEARDATA    debugging		INIT(0);

VARCLASS    char	 NEARDATA    re_syntax_table[256];

#define WHOLELINE		     fields_arr[0]


#undef VARCLASS
#undef INIT


/*  AWK1.C  */


void		 PASCAL clear_wrk_repat(void);
void			panic(char *str, ...);
void		 PASCAL er_panic(char *str);
void		 PASCAL usage(void);
NODE *		 PASCAL newnode(int ty);
NODE *		 PASCAL dupnode(NODE *n);
NODE *		 PASCAL node(NODE *left, int op, NODE *right);
NODE *		 PASCAL snode(NODE *subn, int op,
			      NODE *(PASCAL *procp)(NODE *));
NODE *		 PASCAL mkrangenode(NODE *cpair);
int		 PASCAL count_arguments(NODE *exp_list);
NODE *		 PASCAL make_number(AWKNUM x);
NODE *		 PASCAL tmp_number(AWKNUM x);
NODE *		 PASCAL make_string(char *s, int len);
NODE *		 PASCAL tmp_string(char *s, int len);
REPAT_BUFFER *	 PASCAL make_regexp(char *s);
FOR_LOOP_HEADER * PASCAL make_for_loop(NODE *init, NODE *cond, NODE *incr);
NODE *		 PASCAL variable(char *name);
NODE *		 PASCAL spc_var(char *name, NODE *value);
NODE *		 PASCAL install(HASHNODE **table, char *name, NODE *value);
NODE *		 PASCAL lookup(HASHNODE **table, char *name);
int		 PASCAL hashf(char *name, int len, int hashsize);
NODE *		 PASCAL append_right(NODE *list, NODE *new);

/*  AWK2.C  */

int		 PASCAL interpret(NODE *tree);
NODE *		 PASCAL tree_eval(NODE *tree);
NODE ** 	 PASCAL get_lhs(NODE *ptr);
void		 PASCAL do_deref(void);
void		 PASCAL assign_number(NODE **ptr, AWKNUM value);
void		 PASCAL init_fields(void);
void		 PASCAL blank_fields(void);
void		 PASCAL set_field(int n, char *str, int len);
NODE *		 PASCAL field_string(char *s, int len);
void		 PASCAL fix_fields(void);
int		 PASCAL eval_condition(NODE *tree);
int		 PASCAL is_a_number(char *str);
int		 PASCAL cmp_nodes(NODE *t1, NODE *t2);
void		 PASCAL assoc_clear(NODE *symbol);
NODE ** 	 PASCAL assoc_lookup(NODE *symbol, NODE *subs, int operation);
NODE *		 PASCAL strforce(NODE *n);
AWKNUM		 PASCAL force_number(NODE *n);
NODE *		 PASCAL force_string(NODE *s);

/*  AWK3.C  */

void		 PASCAL init_vars(void);
char *		 PASCAL get_ofmt(void);
REPAT_BUFFER *	 PASCAL get_fs(void);
void		 PASCAL set_fs(char *str);
void		 PASCAL set_rs(char *str);
int		 PASCAL get_rs(void);
NODE *		 PASCAL do_match(NODE *tree);
NODE *		 PASCAL do_sub(NODE *tree);
NODE *		 PASCAL do_gsub(NODE *tree);
NODE *		 PASCAL do_exp(NODE *tree);
NODE *		 PASCAL do_cos(NODE *tree);
NODE *		 PASCAL do_sin(NODE *tree);
NODE *		 PASCAL do_atan2(NODE *tree);
NODE *		 PASCAL do_getline(NODE *tree);
NODE *		 PASCAL do_close(NODE *tree);
NODE *		 PASCAL do_index(NODE *tree);
NODE *		 PASCAL do_upper(NODE *tree);
NODE *		 PASCAL do_lower(NODE *tree);
NODE *		 PASCAL do_reverse(NODE *tree);
NODE *		 PASCAL do_srand(NODE *tree);
NODE *		 PASCAL do_rand(NODE *tree);
NODE *		 PASCAL do_int(NODE *tree);
NODE *		 PASCAL do_length(NODE *tree);
NODE *		 PASCAL do_log(NODE *tree);
NODE *		 PASCAL do_printf(NODE *tree);
NODE *		 PASCAL do_split(NODE *tree);
NODE *		 PASCAL do_sprintf(NODE *tree);
NODE *		 PASCAL do_sqrt(NODE *tree);
NODE *		 PASCAL do_substr(NODE *tree);
NODE *		 PASCAL do_system(NODE *tree);
void		 PASCAL hack_print_node(NODE *tree);
void		 PASCAL get_one(NODE *tree, NODE **res);
void		 PASCAL get_two(NODE *tree, NODE **res1, NODE **res2);
int		 PASCAL get_three(NODE *tree, NODE **res1,
				  NODE **res2, NODE **res3);
int		 PASCAL a_get_three(NODE *tree, NODE **res1,
				    NODE **res2, NODE **res3);
int		 PASCAL inrec(FILE *fp);
int		 PASCAL read_a_record(FILE *fp, char **buf,
				      int *buflen, int *cnt);
void		 PASCAL field_spec_changed(AWKNUM fld_no);
void		 PASCAL split_out_fields(int blank_em);
FILE *		 PASCAL deal_redirect(NODE *tree);
void		 PASCAL close_redirect_files(void);
void		 PASCAL print_simple(NODE *tree, FILE *fp);

/*  OBSTACK.C  */

void		 PASCAL _obstack_begin(struct obstack *h, void *(*chunkfun)());
void		 PASCAL _obstack_newchunk(struct obstack *h,
					  void *(*chunkfun)(), int length);
void		 PASCAL _obstack_free(struct obstack *h,
				      void (*freechunkfun)(), char *obj);
void		 PASCAL obstack_begin(OBSTACK *h, int try_length);
void		 PASCAL obstack_grow(OBSTACK *h, char *where, int length);
void		 PASCAL obstack_grow0(OBSTACK *h, char *where, int length);
void		 PASCAL obstack_1grow(OBSTACK *h, int datum);
void		 PASCAL obstack_blank(OBSTACK *h, int length);
void *		 PASCAL obstack_finish(OBSTACK *h);
void		 PASCAL obstack_free(OBSTACK *h, char *obj);

#ifndef FAST
/*  DEBUG.C  */

void		 PASCAL ptree(NODE *n);
void		 PASCAL pt(void);
void		 PASCAL print_parse_tree(NODE *ptr);
void		 PASCAL dump_vars(void);
void		 PASCAL dump_fields(void);
void			print_debug(char *str, ...);
void		 PASCAL print_a_node(NODE *ptr);
void		 PASCAL print_maybe_semi(NODE *ptr);
NODE *		 PASCAL do_prvars(NODE *);
NODE *		 PASCAL do_bp(NODE *);
#endif /* FAST */

/*  REGEX1.C  */

int		 PASCAL re_set_syntax(int syntax);
char *		 PASCAL re_compile_pattern(char *pattern, int size,
					   REPAT_BUFFER *bufp);
void		 PASCAL re_compile_fastmap(REPAT_BUFFER *bufp);
char *		 PASCAL re_comp(char *s);
int		 PASCAL re_exec(char *s);

/*  REGEX2.C  */

int		 PASCAL re_search(REPAT_BUFFER *pbufp, char *string, int size,
				  int startpos, int range, REREGS *regs);
int		 PASCAL re_match(REPAT_BUFFER *pbufp, char *string, int size,
				 int pos, REREGS *regs);

/*  AWKTAB.C  */

int		 yyparse(void);
void		 yyerror(char *mesg, ...);

#endif	    /* AWK_H */
