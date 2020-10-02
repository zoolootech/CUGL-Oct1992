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

p1_equ(psdo, arg)
char *psdo;
int arg;
{
    do_label = NO;			/* don't process label in main */
/** need work here to signal second pass of problem */
    if (eval(4,	0) != _address)	return EQU_EVAL;	/* get a long value */
/* determine whether to enter as absolute or relocatable from result */
    if (symenter(label,	op1._addr, (op1._rel_lbl) ? 'r' : 'a')
	== ERROR) {
	puts("\007\nsymbol table FULL!\007");
	fatal = TRUE;			/* mark a fatal system ERROR */
	return SYMTFULL;
    }
    return OK;
}

p1_set(psdo, arg)
char *psdo;
int arg;
{
    do_label = NO;			/* don't process label in main */
/** need work here to signal second pass of problem */
    if (eval(4,	0) != _address)	return SET_EVAL;	/* get a long value */
/* determine whether to enter as absolute or relocatable from result */
    if (symadd(label, op1._addr, (op1._rel_lbl)	? 0x02 : 0, YES)
	== ERROR) {
	puts("\007\nsymbol table FULL!\007");
	fatal = TRUE;			/* mark a fatal system ERROR */
	return SYMTFULL;
    }
    return OK;
}

p1_ifeq(psdo, arg)
char *psdo;
int arg;
{
}
 
p1_ifne(psdo, arg)
char *psdo;
int arg;
{
}
 
p1_endc(psdo, arg)
char *psdo;
int arg;
{
}
 
no_action(psdo,	arg)
char *psdo;
int arg;
{
    return OK;  /* no action required! */
}

/* endcode */


