/* psdo.c, the psdo op code for the as68 assembler.
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

p2_equ(psdo, arg)
char *psdo;
int arg;
{
    register int x;
    char acode[8];
 
    do_label = NO;			/* don't process label in main */
    if (x = symsearch(label)) {
	if (symtbl[x]._atr & 4) {		/* redefined */
	    err_out(REDEF);
	}
    }
    else {
	err_out(UNDEF_SYMBOL);	/* label never defined */
    }
    code[0] = symtbl[x]._val >> 24;		/* evaluate operand */
    code[1] = symtbl[x]._val >> 16;
    code[2] = symtbl[x]._val >> 8;
    code[3] = symtbl[x]._val;
    dump_code(MSG, code, 4);    		/* a long value */
}

p2_set(psdo, arg)
char *psdo;
int arg;
{
    register int x;
    char acode[8];
 
    do_label = NO;			/* don't process label in main */
    if (eval(4,	0) != _address)	err_out(SET_EVAL);	/* get a long value */
    else {
	if (x = symsearch(label)) {
	    if (symtbl[x]._atr & 1) {   	    /* redefined */
		err_out(REDEF);
	    }
	    else {
		symtbl[x]._val = op1._addr;
		symtbl[x]._atr |= (op1._rel_lbl) ? 0x02 : 0;
	    }
	}
	else {
	    err_out(UNDEF_SYMBOL);  /* label never defined */
	}
    }
    code[0] = symtbl[x]._val >> 24;		/* evaluate operand */
    code[1] = symtbl[x]._val >> 16;
    code[2] = symtbl[x]._val >> 8;
    code[3] = symtbl[x]._val;
    dump_code(MSG, code, 4);    		/* a long value */
}

p2_ifeq(psdo, arg)
char *psdo;
int arg;
{
err_out(UNREC_INSTR);
    if (label[0]) err_out(LABEL_USE);
    dump_code(MSG, code, 0);
}
 
p2_ifne(psdo, arg)
char *psdo;
int arg;
{
err_out(UNREC_INSTR);
    if (label[0]) err_out(LABEL_USE);
    dump_code(MSG, code, 0);
}
 
p2_endc(psdo, arg)
char *psdo;
int arg;
{
err_out(UNREC_INSTR);
    if (label[0]) err_out(LABEL_USE);
    dump_code(MSG, code, 0);
}
 
p2_xlst(psdo, arg)
char *psdo;
int arg;
{
    nolist = (*psdo == 'n');
}

/* endcode */


