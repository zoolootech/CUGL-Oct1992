/* bc1.c, code building function of as68 assembler (pass1 routines)
 * 
 *	(C) Copyright 1982 Steve Passe
 *	All Rights Reserved
 *
 * version 1.00
 * created 10/21/82
 *
 * version 1.01
 *
 *   8/30/83 ver. 1.01 modified for Aztec ver. 1.05g smp
 *   10/12/87  split into root, pass1 and pass2 parts for overlay on pdp-11
 */
 
/* begincode */
 
/* includes */
 
#include <stdio.h>
#include "as68.h"
 
/* externals */
 
extern char pass;			    /* present pass number, 1 or 2 */
extern unsigned	line_count;		    /* line number of source file */
extern long loc_counter;		    /* address to assemble obj code */
extern int loc_plus;			    /* increment to loc counter	*/
extern FLAG abs_long;			    /* default to absolute long add.*/
extern FLAG rorg;			    /* in pc relative mode */
extern char label[32];			/* buffer for label from preparse */
extern char instr[33];			/* buffer for mnem, psdo or macro */
 
extern struct _mtable mtable[];		    /* mnemonic	lookup table */
extern struct _mvalue mvalue[];		    /* mnemonic	code table */
extern struct _oprnd op1, op2;  	    /* structs to hold operand val */
extern char code[];			    /* code array */
 
p1_mnem(mt)
register struct _mtable *mt;
{
    register int result, index;
 
    if ((index = match(mt)) == ERROR) {
/** flush error stack someday? */
	return ERROR;
    }
    switch (result = mvalue[index]._opc_len) {  /* determine code length... */
    case 1:
    case 2:
    case 3:
	break;
    case 12:    			    /* _imd not contained in opcode */
    case 13:
	result -= 10;   		    /* strip '10' flag */
	result += p1_codelen(&op2);	    /* add second operand length */
	break;
    case 0:     				/* gotta figure it out */
	if (/**rorg**/ op1._rel_lbl && mvalue[index]._optyp1 == _sadr
	    && (op1._typ == _address || op1._typ == _d16_ani)) {
	    result = 2 + p1_codelen(&op2);	/* 1 for opcode, 1 for raddr */
	}
	else result = p1_codelen(&op1) + p1_codelen(&op2) + 1; /* 1 for code */
	break;
    }
    loc_plus = result * 2;
    return result;
}
 
p1_codelen(op)
register struct _oprnd *op;
{
    register int result;
 
    switch (op->_typ) {
    case _address:
	result = (op->_long_val) ? 2 : 1;
	break;
    case _d16_ani:
    case _d8_anx:
    case _labeli:
    case _reglst:
    case _label:    /* needed? */
	result = 1;
	break;
    default:
	result = 0;
    }
    return result;
}


