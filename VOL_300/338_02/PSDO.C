/* psdo.c, the psdo op code for the as68 assembler. (root part)
 * 
 *	(C) Copyright 1982 Steve Passe
 *	All Rights Reserved
 * 
 * version 1.00
 * created 11/25/82
 * 
 * version 1.01
 * 
 *    8/30/83 ver. 1.01 modified for Aztec ver. 1.05g smp
 *    12/26/83	added include psdo  smp
 * 
 */

/* begincode */
 
/* includes */
 
#include <stdio.h>
#include "as68.h"
 
/* externals */
 
extern char fatal;
extern char pass;
extern unsigned	line_count;
extern long loc_counter;
extern int loc_plus;
extern FLAG abs_long;
extern FLAG rorg;
extern FLAG do_label;
extern char label[];
extern char instr[];
extern char code[];
extern int src_level;
extern FILE *_src[];
extern char *opfld_ptr;
extern struct _oprnd op1;
extern struct _symbol *symtbl;
extern FLAG nolist;

/* types */

char *token();
 
/*** note - external pseudo-op table is defined	at the end of this file ***/

px_dc(psdo, arg)
char *psdo;
int arg;
{
    register int x;
    register int size, result, cx = 0;
    int dmp_typ	= DATA;
 
    for (x = 0; x < 12; code[x++] = 0)  	    /* clear code */
	;
    size = arg;
    x = 1;
top:
    result = eval(size,	cx);			/* while args are left */
    if (x++ > 10/size) {			/* line is full (code) */
	if (pass == 2) {
	    dump_code(dmp_typ, code, cx);	/* dump code */
	    for (x = 0; x < size; ) code[x++] = code[cx++]; /* save xtra */
	    while (x < 12) code[x++] = 0;	/* clear rest of code */
	    dmp_typ = MDATA;
	}
	x = 2;  				/* reset the indexes */
	cx = 0;
    }
    switch (result) {
    case _address:
	loc_plus += size;
	cx += size;				    /* bump code index */
	goto top;
    case NULL:  				    /* no more operands	*/
#ifdef AUTO_PAD
	if ((size == 1) && ((loc_counter + loc_plus) % 2)) { /* need to pad? */
	/** look ahead for another dc.b, o joy! */
	    if (!moredcb()) {
		++cx;				    /* pad code */
		++loc_plus;			    /* bump loc */
	    }
	}
#endif /*AUTO_PAD*/
	if (pass == 2) {
	    if (cx) {   			/* addition since last dump */
		dump_code(dmp_typ, code, cx);       /* dump code */
	    }
	    dump_code(FLUSH, code, 0);  	    /* flush line */
	}
	return;
    default:					    /* error of somesort */
	if (pass == 2) err_out(result);
	loc_plus += size;
	cx += size;				    /* bump code index */
	goto top;
    }
}
 
px_ds(psdo, arg)
char *psdo;
int arg;
{
    register int size;  		/* default to word size (2 bytes) */
    register int cx = 0;
 
    size = arg;
    if (eval(4,	cx) == _address) {
/**f4*/	if (pass == 2) dump_code(MSG, code, 4);
	if (label[0] && do_label) label_do();
	if (op1._addr) {
	    loc_counter	+= size * op1._addr;	/* add this much to loc */
	}
	else {  				/* must be 0 */
	    if (loc_counter % 2) ++loc_counter;	/* for alignment purposes */
	}
	obj_out(SYNC, 0, 0);    		/* flush and new */
	return OK;
    }
    else {
	if (pass == 2) {
	    err_out(OPRND_EVAL);
	    dump_code(MSG, code, 4);
	}
	return ERROR;
    }
}

px_orgx(psdo, arg)
char *psdo;
register int arg;
{
    register int size;
    long mask;
 
    size = (arg % 2) ? 2 : 4;   		/* long or short org */
    if (size == 2) mask = ~0xffffL;
    else mask = ~0xffffffL;
    if (pass == 2 && label[0]) {
	err_out(LABEL_USE);			/* labels not allowed */
    }
    if (eval(size, 0) == _address) {    	/* should be type _address */
	if (op1._addr & mask) { /* range */
	    if (pass == 2) {
		err_out(LBL_RANGE);		/* fails range test */
		dump_code(MSG, code, size);
	    }
	    return ERROR;
	}
	else {
	    loc_counter	= op1._addr;
	    if (pass == 2) {
		obj_out(SYNC, 0, 0);
		dump_code(MSG, code, size);
	    }
	    abs_long = (size == 2) ? NO : YES;
	    rorg = (arg > 2) ? YES : NO;
	    return OK;
	}
    }
    if (pass == 2) {
	err_out(OPRND_EVAL);
	dump_code(MSG, code, 0);
    }
    return ERROR;
}
 
px_end(psdo, arg)   /* close include file(s), rewind source */
char *psdo;
int arg;
{
    for ( ; src_level; fclose(_src[src_level--]));  /* close include files */
    fseek(_src[0], 0L, 0);			    /* rewind the source */
    if (pass == 2 && label[0]) err_out(LABEL_USE);
    if (pass++ == 2) {
	obj_out(CLOSE, 0, 0);
	dump_code(MSG, code, 0);
    }
    loc_counter	= loc_plus = line_count	= 0;
    abs_long = YES;
    rorg = NO;
}
 
px_incl(psdo, arg)
char *psdo;
int arg;
{
    char name[FNAME_SIZE];
 
    word_copy(name, FNAME_SIZE,	opfld_ptr);	    /* get filename */
    if (!(_src[++src_level] = fopen(name, "r"))) {
	--src_level;				    /* backup */
	err_out(INCL_OPEN);			    /* report it */
    }
    if (pass == 2) {
	dump_code(MSG, code, 0);
    }
}

eval(size, cx)
int size;
register int cx;
{
    static FLAG ascii_mode = NO;
    register int x, y;
    int result;
 
top:
    if (!*opfld_ptr || *opfld_ptr == '\n') return NULL;
    if (!ascii_mode) {
	switch (*opfld_ptr) {
	case '\t':
	case ' ':
	    return NULL;
	case '\'':		/* ' starts an ASCII string */
	    ++opfld_ptr;
	    ascii_mode = YES;
	    break;
	case ',':
	    ++opfld_ptr;
	    goto top;
	}
    }
    if (ascii_mode) {
	op1._addr = 0L;
	op1._rel_lbl = NO;
	for (y = 0; y < size; ++y) {
	    if (*opfld_ptr == '\'') {
		++opfld_ptr;			/* pass it */
		if (*opfld_ptr != '\'') {	/* end of ascii str */
		    ascii_mode = NO;
		    if (y == 0) goto top;	/* first char was end */
		    return _address;
		}
	    }
	    op1._addr *= 256;
	    op1._addr += *opfld_ptr;
	    code[cx++] = *opfld_ptr++;
	}
	return _address;
    }
    switch (result = op_eval(&op1)) {
    case _address:
	switch (size) {
	case 4:
	    code[cx++] = op1._addr >> 24;
	    code[cx++] = op1._addr >> 16;
	case 2:
	    code[cx++] = op1._addr >> 8;
	case 1:
	    code[cx] = op1._addr;
	}
	return _address;
    case _none:
	return NULL;
    default:
	return OPRND_EVAL;
    }
}

#ifdef AUTO_PAD 	/* This is REALLY messy - can't do it in the
			 * general case, and you really don't need it
			 * anyway. -srd 11-04-87
			 */
/** look at next line to determine if it is a 'dc.b' statement
    if "dc.b" or "dcb.b" exists in comment field this will fail,
    also, if at end of included	file and next statement	of next
    layer is a dc.b it will fail...
*/
 
moredcb()
{
    long place, ftell();
    int answer = NO;
    char tbuf[STMNT_SIZE];
 
    place = ftell(_src[src_level]);		/* present position */
    if (fgets(tbuf, STMNT_SIZE,	_src[src_level])) {/* get next statement */
	if (token(tbuf,	"dc.b")	|| token(tbuf, "dcb.b")) answer = YES;
    }
    fseek(_src[src_level], place, 0);   	/* get back to last line */
    return answer;
}
#endif /*AUTO_PAD*/
 
char *token(str, tkn)
register char *str;
char *tkn;
{
    register char *x;
    register char *y;
 
    for ( ; *str; ++str) {
	if (tolower(*str) == *tkn) {	/* a clumsy 'strcmp' */
	    x = str+1;
	    y = tkn+1;
	    while (TRUE) {
		if (!*y) return str;
		if (tolower(*x) != *y) break;
		x++;
		y++;
	    }
	}
    }
    return NULL;
}
 
#define	PTSIZE 21		/* number of structures	in pseudo-op table */

p1_endc(), p2_endc(),		/* declare the fcns moved to overlays */
p1_equ(), p2_equ(),
p1_ifeq(), p2_ifeq(),
p1_ifne(), p2_ifne(),
no_action(), p2_xlst(),
p1_set(), p2_set();
 
struct _ptable ptable[PTSIZE] = {
     "dc",	px_dc,	    px_dc,	2,
     "dc.b",	px_dc,	    px_dc,	1,
     "dc.l",	px_dc,	    px_dc,	4,
     "dc.w",	px_dc,	    px_dc,	2,
/**
     "dcb",	px_dcb,	    px_dcb,	2,
     "dcb.b",	px_dcb,	    px_dcb,	1,
     "dcb.l",	px_dcb,	    px_dcb,	4,
     "dcb.w",	px_dcb,	    px_dcb,	2,
**/
     "ds",	px_ds,	    px_ds,	2,
     "ds.b",	px_ds,	    px_ds,	1,
     "ds.l",	px_ds,	    px_ds,	4,
     "ds.w",	px_ds,	    px_ds,	2,
     "end",	px_end,	    px_end,	NULL,
     "endc",	p1_endc,    p2_endc,	NULL,
     "equ",	p1_equ,	    p2_equ,	NULL,
     "ifeq",	p1_ifeq,    p2_ifeq,	NULL,
     "ifne",	p1_ifne,    p2_ifne,	NULL,
     "include",	px_incl,    px_incl,	NULL,
     "list",	no_action,  p2_xlst,	NULL,
     "nolist",	no_action,  p2_xlst,	NULL,
     "org",	px_orgx,    px_orgx,	1,
     "org.l",	px_orgx,    px_orgx,	2,
     "rorg",	px_orgx,    px_orgx,	3,
     "rorg.l",	px_orgx,    px_orgx,	4,
     "set",	p1_set,	    p2_set,	NULL
};
 
/** search the pseudo-op table for "psdo"
    return pointer to struct of type _ptable.
*/
 
struct _ptable *
psdosearch(psdo)
char *psdo;
{
    register int result;
    register struct _ptable *bottom = &ptable[0];
    register struct _ptable *top = &ptable[PTSIZE - 1];
    register struct _ptable *middle;
 
    if (strlen(psdo) > 7) return NULL;
 
    while (bottom <= top) {
	middle = bottom + (top - bottom) / 2;
	if ((result = a1strcmp(psdo, middle->_psdo)) < 0)
	    top = middle - 1;
	else if (result	> 0)
	    bottom = middle + 1;
	else return middle;
    }
    return NULL;
}
/* endcode */

