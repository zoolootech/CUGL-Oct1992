/* bc2.c, code building function of as68 assembler (pass2 routines)
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
 
p2_mnem(mt)
struct _mtable *mt;
{
    register int x;     			    /* scratch */
    register int index;   			    /* index to mvalue */
    register int result;
 
    for (x = 0; x < 10; ++x) {  	/* init code array to 0 */
	code[x]	= 0;
    }
    if ((index = match(mt)) == ERROR) {
	dump_code(CODE,	code, 0);
	return ERROR;
    }
    code[0] = mvalue[index]._opcb1;			    /* get opcode */
    code[1] = mvalue[index]._opcb2;			    /* get opcode */
 
    if ((result	= (*mvalue[index]._p2_action)()) < 0) {     /* call funct */
	err_out(result);
	loc_plus = 0;
    }
    else loc_plus = ++result * 2;		    /* bytes created */
    dump_code(CODE, code, loc_plus);		    /* send them */
    return result;				    /* words created */
}


