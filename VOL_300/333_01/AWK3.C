/* awk3.c -- Builtin functions and various utility procedures
   Copyright (C) 1986,1987 Free  Software Foundation
   Written by Jay Fenlason, December 1986
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <errno.h>
#include "awk.h"



struct redirect
{
    int 	    flag;      /* type of redirection */
    NODE           *value;
    FILE           *fp;
};
typedef struct redirect 	REDIRECT;

static REDIRECT reds[20];	/* An arbitrary limit, surely, but there's an
				 * arbitrary limit on open files, too.  So it
				 * doesn't make much difference, does it? */


long            NR;
int             NF;

/* Set all the special variables to their initial values.  Also sets up
   the dumb[] array for force_string */

VOID PASCAL init_vars(void)
{
    register int	  i;
    auto     NODE	**tmp;

    FS_node	  = spc_var("FS", make_string("[\t ]+", 5));
    NF_node	  = spc_var("NF", make_number(0.0));
    RS_node	  = spc_var("RS", make_string("\n", 1));
    NR_node	  = spc_var("NR", make_number(0.0));
    FILENAME_node = spc_var("FILENAME", Nnull_string);
    OFS_node	  = spc_var("OFS", make_string(" ", 1));
    ORS_node	  = spc_var("ORS", make_string("\n", 1));
    OFMT_node	  = spc_var("OFMT", make_string("%.6g", 4));
    FNR_node	  = spc_var("FNR", make_number(0.0));
    RLENGTH_node  = spc_var("RLENGTH", make_number(0.0));
    RSTART_node   = spc_var("RSTART", make_number(0.0));
    SUBSEP_node   = spc_var("SUBSEP", make_string("\034", 1));
    ARGC_node	  = spc_var("ARGC", make_number(1.0));
    ARGV_node	  = variable("ARGV");
    assoc_clear(ARGV_node);
    tmp  = assoc_lookup(ARGV_node, tmp_number((AWKNUM) 0.0), ASSOC_CREATE);
    *tmp = make_string(myname, strlen(myname));

    /* This ugly hack is used by force_string to fake a call to sprintf */
    dumb[0].type  = NODE_EXPRESSION_LIST;
    dumb[0].lnode = OFMT_node;
    dumb[0].rnode = &dumb[1];
    dumb[1].type  = NODE_EXPRESSION_LIST;
    dumb[1].lnode = (NODE *) NULL; /* fill in the var here */
    dumb[1].rnode = (NODE *) NULL;

    for (i = 0; i < MAXDIM(reds); ++i)
	reds[i].flag = NODE_ILLEGAL;

    return;
}


/* OFMT is special because we don't dare use force_string on it for fear of
   infinite loops.  Thus, if it isn't a string, we return the default "%.6g"
   This may or may not be the right thing to do, but its the easiest */
/* This routine isn't used!  It should be.  */

char * PASCAL get_ofmt(void)
{
    register NODE      *tmp;

    tmp = *get_lhs(OFMT_node);
    if (tmp->type != NODE_STRING || tmp->stlen == 0)
	return("%.6g");
    return(tmp->stptr);
}


REPAT_BUFFER * PASCAL get_fs(void)
{
    register NODE	    *tmp;
    auto     char	    *err;
    auto     char	    *str;
    auto     int	     len;
    static   REPAT_BUFFER    fs_repat;
    static   char	     fs_fastmap[FASTMAP_SIZE];
    static   char	     fs_str[256] = "";

    tmp = force_string(FS_node->var_value);
    if (0 == tmp->stlen)
    {
	str = "[\t ]+";
	len = strlen(str);
    }
    else
    {
	str = tmp->stptr;
	len = tmp->stlen;
    }

    if (strcmp(str, fs_str))
    {
	strcpy(fs_str, str);
	fs_repat.fastmap	  = fs_fastmap;
	fs_repat.used		  = 0;
	fs_repat.fastmap_accurate = FALSE;
	fs_repat.can_be_null	  = 0;
	if (0 == fs_repat.allocated)
	{
	    fs_repat.allocated = 100;
	    if (NULL == (fs_repat.buffer = malloc(100)))
		panic("Out of memory for fs_repat buffer");
	}
	err = re_compile_pattern(str, len, &fs_repat);
	if (err)
	    panic("Invalid REGEXP(%s) in FS variable: %s", str, err);
    }

    return(&fs_repat);
}


VOID PASCAL set_fs(char *str)
{
    register NODE     **tmp;

    tmp = get_lhs(FS_node);
    do_deref();

    *tmp = make_string(str, -1);
    return;
}


VOID PASCAL set_rs(char *str)
{
    register NODE      **tmp;

    tmp = get_lhs(RS_node);
    do_deref();

    if (*str == 't')
	*str = '\t';

    *tmp = make_string(str, 1);
    return;
}


int PASCAL get_rs(void)
{
    register NODE  *tmp;

    tmp = force_string(RS_node->var_value);
    if (tmp->stlen == 0)
	return('\n');
    return(*(tmp->stptr));
}


/* Builtin functions */


NODE * PASCAL do_match(NODE *tree)
{
    auto     int	      idx;
    auto     NODE	     *str, *reg_node;
    auto     char	     *err;
    auto     REPAT_BUFFER    *rp;
    auto     REREGS	      regs;

    get_two(tree, &str, &reg_node);
    str = force_string(str);
    if (NODE_REGEXP == reg_node->type)
	rp = reg_node->rereg;
    else
    {
	reg_node = force_string(reg_node);
	clear_wrk_repat();
	rp  = &wrk_repat;
	err = re_compile_pattern(reg_node->stptr, reg_node->stlen, rp);
	if (err)
	    panic("Invalid REGEXP(%s) in match(): %s", reg_node->stptr, err);
    }
    idx = re_search(rp, str->stptr, str->stlen, 0, str->stlen, &regs);
    if (idx < 0)
    {
	assign_number(&RSTART_node->var_value,	(AWKNUM) 0.0);
	assign_number(&RLENGTH_node->var_value, (AWKNUM) 0.0);
	idx = 0;
    }
    else
    {
	assign_number(&RSTART_node->var_value,	(AWKNUM) ++idx);
	assign_number(&RLENGTH_node->var_value,
		      (AWKNUM) (regs.end[0] - regs.start[0]));
    }
    return(tmp_number((AWKNUM) idx));
}


NODE * PASCAL do_sub(NODE *tree)
{
    auto     int	      idx, len;
    auto     int	      match_len;
    auto     NODE	     *regexp, *str1, *str2;
    auto     char	     *wrk, *pwrk;
    auto     REPAT_BUFFER    *rp;
    auto     REREGS	      regs;

    get_three(tree, &regexp, &str1, &str2);
    str1 = force_string(str1);
    str2 = force_string(str2);
    if (NODE_REGEXP == regexp->type)
	rp = regexp->rereg;
    else
    {
	regexp = force_string(regexp);
	clear_wrk_repat();
	rp  = &wrk_repat;
	wrk = re_compile_pattern(regexp->stptr, regexp->stlen, rp);
	if (wrk)
	    panic("Invalid REGEXP(%s) in sub(): %s", regexp->stptr, wrk);
    }

    idx  = re_search(rp, str2->stptr, str2->stlen, 0, str2->stlen, &regs);
    if (idx < 0)
	return(tmp_number((AWKNUM) 0.0));

    match_len = regs.end[0] - regs.start[0];
    wrk = malloc(str2->stlen - match_len + str1->stlen + 1);
    if (NULL == wrk)
	panic("Out of memory in do_sub()");

    pwrk = wrk;
    if (idx > 0)
    {
	memcpy(pwrk, str2->stptr, idx);
	pwrk += idx;
    }
    memcpy(pwrk, str1->stptr, str1->stlen);
    pwrk += str1->stlen;
    len = idx + match_len;
    if (len < str2->stlen)
    {
	memcpy(pwrk, str2->stptr + len, str2->stlen - len);
	pwrk += str2->stlen - len;
    }
    *pwrk = EOS;

    len  = str2->stlen - match_len + str1->stlen;
    str1 = tree->rnode->rnode->lnode;
    *get_lhs(str1) = dupnode(tmp_string(wrk, len));
    do_deref();
    free(wrk);

    /* If the modified string is a field variable we need to update the   */
    /* value of the field variables.  If $0 was changed we need to recalc */
    /* all the fields.	If an individual field was modified we need to	  */
    /* recalc $0. - BW 12/21/88 					  */
    if (NODE_FIELD_SPEC == str1->type)
	field_spec_changed(str1->lnode->numbr);

    return(tmp_number((AWKNUM) 1.0));
}


NODE * PASCAL do_gsub(NODE *tree)
{
    auto     int	       idx, len, strlen;
    auto     int	       match_len;
    auto     int	       hits = 0;
    auto     int	       ofs  = 0;
    auto     NODE	      *regexp, *str1, *str2;
    auto     char	      *cur2, *new2, *p;
    auto     REPAT_BUFFER     *rp;
    auto     REREGS	       regs;

    get_three(tree, &regexp, &str1, &str2);
    str1 = force_string(str1);
    str2 = force_string(str2);
    if (NODE_REGEXP == regexp->type)
	rp = regexp->rereg;
    else
    {
	regexp = force_string(regexp);
	clear_wrk_repat();
	rp = &wrk_repat;
	p = re_compile_pattern(regexp->stptr, regexp->stlen, rp);
	if (p)
	    panic("Invalid REGEXP(%s) in gsub(): %s", regexp->stptr, p);
    }

    idx  = re_search(rp, str2->stptr, str2->stlen, 0, str2->stlen, &regs);
    if (idx < 0)
	return(tmp_number((AWKNUM) 0.0));

    match_len = regs.end[0] - regs.start[0];
    strlen    = str2->stlen;
    cur2      = malloc(strlen + 1);
    if (NULL == cur2)
	panic("Out of memory in gsub()");
    strcpy(cur2, str2->stptr);

    do
    {
	p = new2 = malloc(strlen - match_len + str1->stlen + 1);
	if (NULL == new2)
	    panic("Out of memory in gsub()");
	if (idx > 0)
	{
	    memcpy(p, cur2, idx);
	    p	+= idx;
	    ofs += idx;
	}
	if (str1->stlen > 0)
	{
	    memcpy(p, str1->stptr, str1->stlen);
	    p	+= str1->stlen;
	    ofs += str1->stlen;
	}
	len = idx + match_len;
	if (len < strlen)
	{
	    memcpy(p, cur2 + len, strlen - len);
	    p += strlen - len;
	}
	*p = EOS;
	free(cur2);
	cur2   = new2;
	strlen = strlen + str1->stlen - match_len;
	++hits;
	if (ofs > strlen)
	    break;
	idx = re_search(rp, cur2, strlen, ofs, strlen - ofs, &regs);
	match_len = regs.end[0] - regs.start[0];
    } while (idx > 0);

    str1 = tree->rnode->rnode->lnode;
    *get_lhs(str1) = dupnode(tmp_string(cur2, strlen));
    do_deref();
    free(cur2);

    /* If the modified string is a field variable we need to update the   */
    /* value of the field variables.  If $0 was changed we need to recalc */
    /* all the fields.	If an individual field was modified we need to	  */
    /* recalc $0. - BW 12/21/88 					  */
    if (NODE_FIELD_SPEC == str1->type)
	field_spec_changed(str1->lnode->numbr);

    return(tmp_number((AWKNUM) hits));
}


NODE * PASCAL do_exp(NODE *tree)
{
    auto     NODE      *tmp;

    get_one(tree, &tmp);
    return(tmp_number(exp(force_number(tmp))));
}


NODE * PASCAL do_cos(NODE *tree)
{
    auto     NODE	*tmp;

    get_one(tree, &tmp);
    return(tmp_number(cos(force_number(tmp))));
}


NODE * PASCAL do_sin(NODE *tree)
{
    auto     NODE	*tmp;

    get_one(tree, &tmp);
    return(tmp_number(sin(force_number(tmp))));
}


NODE * PASCAL do_atan2(NODE *tree)
{
    auto     NODE	*tmp1, *tmp2;

    get_two(tree, &tmp1, &tmp2);
    return(tmp_number(atan2(force_number(tmp1), force_number(tmp2))));
}


/* JF: I don't know what this should return. */
/* jfw: 1 if successful or by land, 0 if end of file or by sea */

NODE * PASCAL do_getline(NODE *tree)
{
    auto     NODE      *redir;
    auto     FILE      *fp;
    auto     char      *buf;
    auto     int	c, buflen, cnt;
    auto     NODE     **lhs;

    if (tree->rnode)
	fp = deal_redirect(tree->rnode);
    else
	fp = input_file;

    if (NULL == fp)
	return(tmp_number(-1.0));

    if (NULL == tree->lnode)
	return(tmp_number(inrec(fp) == 0 ? 1.0 : 0.0));

    buf = NULL;
    c	= read_a_record(fp, &buf, &buflen, &cnt);
    if (EOF == c)
    {
	free(buf);
	return(tmp_number(0.0));
    }
    lhs  = get_lhs(tree->lnode);
    *lhs = make_string(buf, cnt);
    free(buf);
    do_deref();
    if (tree->lnode && NODE_FIELD_SPEC == tree->lnode->type)
	field_spec_changed(tree->lnode->numbr);
    return(tmp_number(1.0));
}


NODE * PASCAL do_close(NODE *tree)
{
    register int	  i;
    auto     NODE	 *tmp;
    auto     int	  ret_val = 0;

    get_one(tree, &tmp);
    if (tmp == Nnull_string)
    {
	close_redirect_files();
	ret_val = 1;
    }
    else
    {
	tmp = force_string(tmp);
	for (i = 0; i < MAXDIM(reds); ++i)
	{
	    if (reds[i].fp && cmp_nodes(reds[i].value, tmp) == 0)
	    {
		fclose(reds[i].fp);
		reds[i].fp    = NULL;
		reds[i].flag  = NODE_ILLEGAL;
		ret_val       = 1;
		break;
	    }
	}
    }
    return(tmp_number((AWKNUM) ret_val));
}


NODE * PASCAL do_index(NODE *tree)
{
    register char	  *p1, *p2;
    register int	   l1, l2;
    auto     NODE	  *s1, *s2;

    get_two(tree, &s1, &s2);
    p1 = s1->stptr;
    p2 = s2->stptr;
    l1 = s1->stlen;
    l2 = s2->stlen;
    while (l1)
    {
	if (*p1 == *p2)        /* BW: speed up index() */
	{
	    if (0 == strncmp(p1, p2, l2))
		return(tmp_number((AWKNUM) (1 + s1->stlen - l1)));
	}
	l1--;
	p1++;
    }
    return(tmp_number(0.0));
}


NODE * PASCAL do_upper(NODE *tree)
{
    auto     NODE      *tmp;

    get_one(tree, &tmp);
    tmp = force_string(tmp);
    strupr(tmp->stptr);
    return(tmp_string(tmp->stptr, tmp->stlen));
}


NODE * PASCAL do_lower(NODE *tree)
{
    auto     NODE      *tmp;

    get_one(tree, &tmp);
    tmp = force_string(tmp);
    strlwr(tmp->stptr);
    return(tmp_string(tmp->stptr, tmp->stlen));
}


NODE * PASCAL do_reverse(NODE *tree)
{
    auto     NODE      *tmp;

    get_one(tree, &tmp);
    tmp = force_string(tmp);
    strrev(tmp->stptr);
    return(tmp_string(tmp->stptr, tmp->stlen));
}


NODE * PASCAL do_srand(NODE *tree)
{
    auto     NODE      *tmp;
    auto     unsigned	seed;

    get_one(tree, &tmp);
    if (tmp == Nnull_string)
	seed = (unsigned) force_number(tmp);
    else
	seed = (unsigned) time(NULL);
    srand(seed);
    return(tmp_number((AWKNUM) 0.0));
}


NODE * PASCAL do_rand(NODE *tree)
{
    auto     int       r_num;
    auto     AWKNUM    num;

    r_num = rand();
    if (0 == r_num)
	num = 0.0;
    else
    {
	while (1 == r_num)
	    r_num = rand();
	num = (AWKNUM) 1.0 / (AWKNUM) r_num;
    }
    return(tmp_number(num));
}


NODE * PASCAL do_int(NODE *tree)
{
    auto     NODE      *tmp;

    get_one(tree, &tmp);
    return(tmp_number(floor(force_number(tmp))));
}


NODE * PASCAL do_length(NODE *tree)
{
    auto     NODE	*tmp;

    get_one(tree, &tmp);
    return(tmp_number((AWKNUM) (force_string(tmp)->stlen)));
}


NODE * PASCAL do_log(NODE *tree)
{
    auto     NODE	*tmp;

    get_one(tree, &tmp);
    return(tmp_number(log(force_number(tmp))));
}


NODE * PASCAL do_printf(NODE *tree)
{
    register FILE	*fp;

    fp = deal_redirect(tree->rnode);
    print_simple(do_sprintf(tree->lnode), fp);
    return(Nnull_string);
}


NODE * PASCAL do_split(NODE *tree)
{
    register REPAT_BUFFER    *splitc;
    register int	      num;
    register char	     *ptr, *ttmp;
    auto     int	      tcnt, pos, new_pos;
    auto     int	      len;
    auto     REREGS	      reregs;
    auto     NODE	     *t1, *t2, *t3;

    if (a_get_three(tree, &t1, &t2, &t3) < 3)
	splitc = get_fs();
    else
    {
	if (NODE_REGEXP == t3->type)
	{
	    splitc   = t3->rereg;
	}
	else
	{
	    t3 = force_string(t3);
	    clear_wrk_repat();
	    splitc = &wrk_repat;
	    ptr = re_compile_pattern(t3->stptr, t3->stlen, splitc);
	    if (ptr)
		panic("Invalid REGEXP(%s) in split(): %s", t3->stptr, ptr);
	}
    }

    num  = 0;
    tree = force_string(t1);
    ptr  = tree->stptr;
    len  = tree->stlen;
    assoc_clear(t2);

    pos = 0;
    do
    {
	new_pos = re_search(splitc, ptr, len, pos, len - pos, &reregs);
	ttmp	= ptr + pos;
	if (new_pos >= 0)
	    tcnt = new_pos - pos;
	else
	    tcnt = len - pos;
	pos = reregs.end[0];
	*assoc_lookup(t2, make_number((AWKNUM) (++num)), ASSOC_CREATE) =
		      make_string(ttmp, tcnt);
    } while (new_pos >= 0);

    return(tmp_number((AWKNUM) num));
}


/* Note that the output buffer cannot be static because sprintf may get called
 * recursively by force_string.  Hence the wasteful malloc calls.
 */


/* %e and %f formats are not properly implemented.  Someone should fix them */

/* BW: %e and %f were not working because "fw" and "prec" were defined as
 *     long rather than int.  This has been fixed.
 */

NODE * PASCAL do_sprintf(NODE *tree)
{
#define bchunk(s,l) if(l) {\
    if((l)>ofre) {\
      char *tmp;\
      tmp=(char *)malloc(osiz*2);\
      if (NULL == tmp) panic("Out of memory in bchunk()");\
      memcpy(tmp,obuf,olen);\
      obuf=tmp;\
      ofre+=osiz;\
      osiz*=2;\
    }\
    memcpy(obuf+olen,s,(l));\
    olen+=(l);\
    ofre-=(l);\
  }

/* Is there space for something L big in the buffer? */
#define chksize(l)  if((l)>ofre) {\
    char *tmp;\
    tmp=(char *)malloc(osiz*2);\
    if (NULL == tmp) panic("Out of memory in chksize()");\
    memcpy(tmp,obuf,olen);\
    obuf=tmp;\
    ofre+=osiz;\
    osiz*=2;\
  }
/* Get the next arg to be formatted.  If we've run out of args, return
   "" (Null string) */
#define parse_next_arg() {\
  if(!carg) arg= Nnull_string;\
  else {\
  	get_one(carg,&arg);\
	carg=carg->rnode;\
  }\
 }

    auto     char	    *obuf;
    auto     int	     osiz, ofre, olen;
    static   char	     chbuf[] = "0123456789abcdef";
    static   char	    sp[] = " ";
    auto     char	    *s0, *s1;
    auto     int	     n0;
    auto     NODE	    *sfmt, *arg;
    register NODE	    *carg;
    auto     int	     fw, prec;
    auto     int	    *cur;
    auto     long	     lj, alt, big;
    auto     long	     val;
    auto     unsigned long   uval;
    auto     int	     sgn;
    auto     int	     base;
    auto     char	     cpbuf[30]; /* if we have numbers bigger than 30 */
					/* chars we lose, but seems unlikely */
    auto     char	    *cend = &cpbuf[30];
    auto     char	    *cp;
    auto     char	    *fill;
    auto     double	     tmpval;
    auto     char	    *pr_str;

    obuf = (char *) malloc(120);
    if (NULL == obuf)
	panic("Out of memory in function do_sprintf()");
    osiz = 120;
    ofre = osiz;
    olen = 0;
    get_one(tree, &sfmt);
    sfmt = force_string(sfmt);
    carg = tree->rnode;
    for (s0 = s1 = sfmt->stptr, n0 = sfmt->stlen; n0-- > 0;)
    {
	if (*s1 != '%')
	{
	    s1++;
	    continue;
	}
	bchunk(s0, s1 - s0);
	s0   = s1;
	cur  = &fw;
	fw   = 0;
	prec = 0;
	lj   = alt = big = 0;
	fill = sp;
	cp   = cend;
	s1++;

retry:
	--n0;
	switch (*s1++)
	{
	    case '%':
		bchunk("%", 1);
		s0 = s1;
		break;
	    case '0':
		if (fill != sp || lj)
		    goto lose;
		fill = "0";			 /* FALL through */
	    case '1':
	    case '2':
	    case '3':
	    case '4':
	    case '5':
	    case '6':
	    case '7':
	    case '8':
	    case '9':
		if (cur == 0)
		    goto lose;
		*cur = s1[-1] - '0';
		while (n0 > 0 && *s1 >= '0' && *s1 <= '9')
		{
		    --n0;
		    *cur = *cur * 10 + *s1++ - '0';
		}
		goto retry;
	    case '-':
		if (lj || fill != sp)
		    goto lose;
		lj++;
		goto retry;
	    case '.':
		if (cur != &fw)
		    goto lose;
		cur = &prec;
		goto retry;
	    case '#':
		if (alt)
		    goto lose;
		alt++;
		goto retry;
	    case 'l':
		if (big)
		    goto lose;
		big++;
		goto retry;
	    case '*':
		if (cur == 0)
		    goto lose;
		parse_next_arg();
		*cur = (int) arg;
		goto retry;
	    case 'c':
		parse_next_arg();
		if (arg->type == NODE_NUMBER)
		{
		    uval     = (unsigned long) arg->numbr;
		    cpbuf[0] = uval;
		    prec     = 1;
		    pr_str   = cpbuf;
		    goto dopr_string;
		}
		if (!prec || prec > arg->stlen)
		    prec = arg->stlen;
		pr_str = cpbuf;
		goto dopr_string;
	    case 's':
		parse_next_arg();
		arg = force_string(arg);
		if (!prec || prec > arg->stlen)
		    prec = arg->stlen;
		pr_str = arg->stptr;

	dopr_string:
		if (fw > prec && !lj)
		{
		    while (fw > prec)
		    {
			bchunk(sp, 1);
			fw--;
		    }
		}
		bchunk(pr_str, (int) prec);
		if (fw > prec)
		{
		    while (fw > prec)
		    {
			bchunk(sp, 1);
			fw--;
		    }
		}
		s0 = s1;
		break;
	    case 'd':
		parse_next_arg();
		val = (long) force_number(arg);
		if (val < 0)
		{
		    sgn = 1;
		    val = -val;
		}
		else
		    sgn = 0;
		do
		{
		    *--cp = '0' + val % 10;
		    val /= 10;
		} while (val);
		if (sgn)
		    *--cp = '-';
		prec = cend - cp;
		if (fw > prec && !lj)
		{
		    if (fill != sp && *cp == '-')
		    {
			bchunk(cp, 1);
			cp++;
			prec--;
			fw--;
		    }
		    while (fw > prec)
		    {
			bchunk(fill, 1);
			fw--;
		    }
		}
		bchunk(cp, (int) prec);
		if (fw > prec)
		{
		    while (fw > prec)
		    {
			bchunk(fill, 1);
			fw--;
		    }
		}
		s0 = s1;
		break;
	    case 'u':
		base = 10;
		goto pr_unsigned;
	    case 'o':
		base = 8;
		goto pr_unsigned;
	    case 'x':
		base = 16;
		goto pr_unsigned;
pr_unsigned:
		parse_next_arg();
		uval = (unsigned long) force_number(arg);
		do
		{
		    *--cp = chbuf[uval % base];
		    uval /= base;
		} while (uval);
		prec = cend - cp;
		if (fw > prec && !lj)
		{
		    while (fw > prec)
		    {
			bchunk(fill, 1);
			fw--;
		    }
		}
		bchunk(cp, (int) prec);
		if (fw > prec)
		{
		    while (fw > prec)
		    {
			bchunk(fill, 1);
			fw--;
		    }
		}
		s0 = s1;
		break;
	    case 'g':
		parse_next_arg();
		tmpval = force_number(arg);
		if (prec == 0)
		    prec = 13;

		/** gcvt(tmpval,prec,cpbuf); **//* BW */
		sprintf(cpbuf, "%g", tmpval);	/* BW */

		prec = strlen(cpbuf);
		cp = cpbuf;
		if (fw > prec && !lj)
		{
		    if (fill != sp && *cp == '-')
		    {
			bchunk(cp, 1);
			cp++;
			prec--;
		    }		/* Deal with .5 as 0.5 */
		    if (fill == sp && *cp == '.')
		    {
			--fw;
			while (--fw >= prec)
			{
			    bchunk(fill, 1);
			}
			bchunk("0", 1);
		    }
		    else
			while (fw-- > prec)
			    bchunk(fill, 1);
		}
		else
		{		/* Turn .5 into 0.5 */
		    /* FOO */
		    if (*cp == '.' && fill == sp)
		    {
			bchunk("0", 1);
			--fw;
		    }
		}
		bchunk(cp, (int) prec);
		if (fw > prec)
		    while (fw-- > prec)
			bchunk(fill, 1);
		s0 = s1;
		break;
	    case 'f':
		parse_next_arg();
		tmpval = force_number(arg);
		chksize(fw + prec + 5);	/* 5==slop */
		cp = cpbuf;
		*cp++ = '%';
		if (lj)
		    *cp++ = '-';
		if (fill != sp)
		    *cp++ = '0';
		if (prec != 0)
		{
		    strcpy(cp, "*.*f");
		    sprintf(obuf + olen, cpbuf, fw, prec, tmpval);
		}
		else
		{
		    strcpy(cp, "*f");
		    sprintf(obuf + olen, cpbuf, fw, tmpval);
		}
		cp = obuf + olen;
		ofre -= strlen(obuf + olen);
		olen += strlen(obuf + olen);	/* There may be nulls */
		s0 = s1;
		break;
	    case 'e':
		parse_next_arg();
		tmpval = force_number(arg);
		chksize(fw + prec + 5);	/* 5==slop */
		cp = cpbuf;
		*cp++ = '%';
		if (lj)
		    *cp++ = '-';
		if (fill != sp)
		    *cp++ = '0';
		if (prec != 0)
		{
		    strcpy(cp, "*.*e");
		    sprintf(obuf + olen, cpbuf, fw, prec, (double) tmpval);
		}
		else
		{
		    strcpy(cp, "*e");
		    sprintf(obuf + olen, cpbuf, fw, (double) tmpval);
		}
		cp = obuf + olen;
		ofre -= strlen(obuf + olen);
		olen += strlen(obuf + olen);	/* There may be nulls */
		s0 = s1;
		break;
	    default:
lose:
		break;
	}
    }
    bchunk(s0, s1 - s0);
    sfmt = tmp_string(obuf, olen);
    free(obuf);
    return(sfmt);
}


NODE * PASCAL do_sqrt(NODE *tree)
{
    auto     NODE	 *tmp;

    get_one(tree, &tmp);
    return(tmp_number(sqrt(force_number(tmp))));
}


NODE * PASCAL do_substr(NODE *tree)
{
    auto     NODE	 *t1, *t2, *t3;
    register int	  n1, n2;

    if (get_three(tree, &t1, &t2, &t3) < 3)
	n2 = 32000;
    else
	n2 = (int) force_number(t3);
    n1	   = (int) force_number(t2) - 1;
    tree   = force_string(t1);
    if (n1 < 0 || n1 >= tree->stlen || n2 <= 0)
	return(Nnull_string);
    if (n1 + n2 > tree->stlen)
	n2 = tree->stlen - n1;
    return(tmp_string(tree->stptr + n1, n2));
}


NODE * PASCAL do_system(NODE *tree)
{
    auto     NODE	*tmp;
    auto     int	 ret_val = 0;

    get_one(tree, &tmp);
    tmp     = force_string(tmp);
    if (system(tmp->stptr));
	ret_val = errno;
    return(tmp_number((AWKNUM) ret_val));
}


/* The print command.  Its name is historical (or hysterical?) */

VOID PASCAL hack_print_node(NODE *tree)
{
    register FILE	 *fp;

#ifndef FAST
    if (!tree || tree->type != NODE_K_PRINT)
	panic("Invalid or NULL node passed to hack_print_node()");
#endif

    fp = deal_redirect(tree->rnode);
    tree = tree->lnode;
    if (!tree)
	tree = WHOLELINE;
    if (tree->type != NODE_EXPRESSION_LIST)
	print_simple(tree, fp);
    else
    {
	while (tree)
	{
	    print_simple(tree_eval(tree->lnode), fp);
	    tree = tree->rnode;
	    if (tree)
		print_simple(OFS_node->var_value, fp);
	}
    }
    print_simple(ORS_node->var_value, fp);
    return;
}


/* Get the arguments to functions.  No function cares if you give it
   too many args (they're ignored).  Only a few fuctions complain
   about being given too few args.  The rest have defaults */

VOID PASCAL get_one(NODE *tree, NODE **res)
{
    if (!tree)
    {
	*res = WHOLELINE;
	return;
    }

#ifndef FAST
    if (tree->type != NODE_EXPRESSION_LIST)
	panic("Invalid node type (%d) in get_one()", tree->type);
#endif

    *res = tree_eval(tree->lnode);
    return;
}


VOID PASCAL get_two(NODE *tree, NODE **res1, NODE **res2)
{
    if (!tree)
    {
	*res1 = WHOLELINE;
	return;
    }

#ifndef FAST
    if (tree->type != NODE_EXPRESSION_LIST)
	panic("Invalid node1 type (%d) in get_two()", tree->type);
#endif

    *res1 = tree_eval(tree->lnode);
    if (!tree->rnode)
	return;
    tree = tree->rnode;

#ifndef FAST
    if (tree->type != NODE_EXPRESSION_LIST)
	panic("Invalid node2 type (%d) in get_two()", tree->type);
#endif

    *res2 = tree_eval(tree->lnode);
    return;
}


int PASCAL get_three(NODE *tree, NODE **res1, NODE **res2, NODE **res3)
{
    if (!tree)
    {
	*res1 = WHOLELINE;
	return(0);
    }

#ifndef FAST
    if (tree->type != NODE_EXPRESSION_LIST)
	panic("Invalid node1 type (%d) in get_three()", tree->type);
#endif

    *res1 = tree_eval(tree->lnode);
    if (!tree->rnode)
	return(1);
    tree = tree->rnode;

#ifndef FAST
    if (tree->type != NODE_EXPRESSION_LIST)
	panic("Invalid node2 type (%d) in get_three()", tree->type);
#endif

    *res2 = tree_eval(tree->lnode);
    if (!tree->rnode)
	return(2);
    tree = tree->rnode;

#ifndef FAST
    if (tree->type != NODE_EXPRESSION_LIST)
	panic("Invalid node3 type (%d) in get_three()", tree->type);
#endif

    *res3 = tree_eval(tree->lnode);
    return(3);
}


int PASCAL a_get_three(NODE *tree, NODE **res1, NODE **res2, NODE **res3)
{
    if (!tree)
    {
	*res1 = WHOLELINE;
	return(0);
    }

#ifndef FAST
    if (tree->type != NODE_EXPRESSION_LIST)
	panic("Invalid node1 type (%d) in a_get_three()", tree->type);
#endif

    *res1 = tree_eval(tree->lnode);
    if (!tree->rnode)
	return(1);
    tree = tree->rnode;

#ifndef FAST
    if (tree->type != NODE_EXPRESSION_LIST)
	panic("Invalid node2 type (%d) in a_get_three()", tree->type);
#endif

    *res2 = tree->lnode;
    if (!tree->rnode)
	return(2);
    tree = tree->rnode;

#ifndef FAST
    if (tree->type != NODE_EXPRESSION_LIST)
	panic("Invalid node3 type (%d) in a_get_three()", tree->type);
#endif

    *res3 = tree_eval(tree->lnode);
    return(3);
}


/* FOO this should re-allocate the buffer if it isn't big enough.
   Also, it should do RMS style only-parse-enough stuff. */
/* This reads in a line from the input file */

int PASCAL inrec(FILE *fp)
{
    register int      c;
    static   char    *buf = NULL, *buf_end = NULL;
    static   int      bsz = 0;
    auto     int      cnt;

    if (fp == input_file)
	++NR;
    blank_fields();
    obstack_free(&other_stack, parse_end);

    c = read_a_record(fp, &buf, &bsz, &cnt);

    set_field(0, buf, cnt);
    if (c == EOF && cnt == 0)
    {
	assign_number(&NF_node->var_value, (AWKNUM) 0.0);
	free(buf);
	buf = NULL;
	return(1);
    }

    if (fp == input_file)
    {
	assign_number(&NR_node->var_value,
		      (AWKNUM) 1.0 + force_number(NR_node->var_value));
	assign_number(&FNR_node->var_value,
		      (AWKNUM) 1.0 + force_number(FNR_node->var_value));
    }

    split_out_fields(FALSE);

    return(0);
}


int PASCAL read_a_record(FILE *fp, char **buf, int *buflen, int *cnt)
{
    register char     *cur;
    register int       c;
    auto     int       len = 0;
    auto     int       rs  = get_rs();
    auto     char     *buf_end;

    if (!(*buf))
    {
	*buf = malloc(80);
	if (NULL == *buf)
	    panic("Out of memory in function read_a_record()");
	*buflen = 80;
	buf_end = *buf + *buflen;
    }

    cur     = *buf;
    buf_end = *buf + *buflen;

    while ((c = getc(fp)) != EOF)
    {
	if (c == rs)
	    break;
	*cur++ = c;
	++len;
	if (cur == buf_end)
	{
	    *buf = realloc(*buf, *buflen * 2);
	    if (NULL == *buf)
		panic("Out of memory in function read_a_record()");
	    cur     = *buf + *buflen;
	    *buflen *= 2;
	    buf_end = *buf + *buflen;
	}
    }
    *cur = EOS;
    *cnt = len;

#ifndef FAST
    if (debugging)
	printf("Read a record:<%s>\n", buf);
#endif

    return(c);
}


VOID PASCAL field_spec_changed(AWKNUM fld_no)
{
    if ((AWKNUM) 0.0 == fld_no)
	split_out_fields(TRUE);
    else
	fix_fields();
    return;
}


VOID PASCAL split_out_fields(int blank_em)
{
    register char	      *ttmp;
    register int	       pos, new_pos, max_pos;
    auto     REPAT_BUFFER     *fs;
    auto     int	       tcnt;
    auto     REREGS	       reregs;
    auto     NODE	      *fld_0_node;
    static   char	      *fld_0_ptr = NULL;
    static   int	       fld_0_len = 0;

    fs = get_fs();
    NF = 0;

    fld_0_node = fields_arr[0];
    tcnt = fld_0_node->stlen;
    if (NULL == fld_0_ptr)
    {
	fld_0_ptr = malloc(tcnt + 1);
	fld_0_len = tcnt;
    }
    if (fld_0_ptr && fld_0_len < tcnt)
    {
	fld_0_ptr = realloc(fld_0_ptr, tcnt + 1);
	fld_0_len = tcnt;
    }
    if (NULL == fld_0_ptr)
	panic("Out of memory in split_out_fields()");
    strcpy(fld_0_ptr, fld_0_node->stptr);

    if (blank_em)
    {
	blank_fields();
	set_field(0, fld_0_ptr, tcnt);
    }

    pos     = 0;
    max_pos = tcnt;
    do
    {
	new_pos = re_search(fs, fld_0_ptr, max_pos, pos,
			    max_pos - pos, &reregs);
	ttmp = fld_0_ptr + pos;
	if (new_pos >= 0)
	    tcnt = new_pos - pos;
	else
	    tcnt = max_pos - pos;
	pos  = reregs.end[0];
	set_field(++NF, ttmp, tcnt);
#ifndef FAST
	if (debugging)
	    printf("Split out field %d:<%.*s> Len(%d)\n",
		   NF, tcnt, ttmp, tcnt);
#endif
    } while (new_pos >= 0);

    assign_number(&(NF_node->var_value), (AWKNUM) NF);
    return;
}


/* Redirection for printf and print commands */

FILE * PASCAL deal_redirect(NODE *tree)
{
    register NODE	      *tmp;
    register REDIRECT	      *rp;
    register char	      *str;
    register FILE	      *fp;
    auto     int	       tflag;

    if (!tree)
	return(stdout);
    tflag = tree->type;
    tmp   = tree_eval(tree->subnode);
    for (rp = reds; rp->flag != 0 && rp < &reds[MAXDIM(reds)]; rp++)
    {
	if (rp->flag == tflag && cmp_nodes(rp->value, tmp) == 0)
	    break;
    }
    if (rp == &reds[MAXDIM(reds)])
    {
	panic("Too many redirections");
	return(NULL);
    }
    if (rp->flag != NODE_ILLEGAL)
	return(rp->fp);
    rp->flag  = tflag;
    rp->value = dupnode(tmp);
    str       = force_string(tmp)->stptr;
    switch (tflag)
    {
	case NODE_REDIRECT_INPUT:
	    fp = fopen(str, "r");
	    break;
	case NODE_REDIRECT_OUTPUT:
	    fp = fopen(str, "w");
	    break;
	case NODE_REDIRECT_APPEND:
	    fp = fopen(str, "a");
	    break;
	case NODE_REDIRECT_PIPE:
	    fp = NULL;
	    break;
    }
    if (fp == NULL)
	panic("can't redirect to '%s'\n", str);
    rp->fp = fp;
    return(fp);
}


VOID PASCAL close_redirect_files(void)
{
    register int       i;

    for (i = 0; i < MAXDIM(reds); ++i)
    {
	if (reds[i].fp)
	{
	    fclose(reds[i].fp);
	    reds[i].fp	 = NULL;
	    reds[i].flag = 0;
	}
    }
    return;
}


VOID PASCAL print_simple(NODE *tree, FILE *fp)
{
    tree = force_string(tree);
    fwrite(tree->stptr, sizeof(char), tree->stlen, fp);
    return;
}
