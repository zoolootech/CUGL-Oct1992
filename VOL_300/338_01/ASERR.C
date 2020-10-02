/* aserr.c - report the error to all open error channels */
#include <stdio.h>
#include "as68.h"

/* externs */

extern unsigned line_count;
extern unsigned list_count;
extern FLAG econ;
extern FLAG elst;
extern FLAG efile;
extern FLAG elfile;
extern FILE *err_f;
extern FILE *lst_d;
extern FILE *lst_f;
extern int src_level;
extern char pass;

/* text for errors */

static char *err_msg[] = {
    "?",					/* no error #0 */
    "Parsing error - totally confused!!",	/* #1 */
    "Bad character in mnemonic/pseudo-op",	/* #2 */
    "Unknown instruction or pseudo-op",
    "Bad character in macro",			/* Macros not implimented */
    "Macro not known",
    "Improper use of label",
    "Can't evaluate operand",
    "Can't evaluete EQU operand",
    "Can't evaluate SET operand",
    "Can't redefine permanent symbol",
    "Symbol table full",
    "Unrecognized operand",
    "Symbol not defined/forward ref?",
    "Label out of range",
    "Operand 1 not valid for this opcode",
    "Operand 2 not valid for this opcode",
    "Operand 1 not correctly formed",
    "Operand 2 not correctly formed",
    "Can't build code?",
    "3-bit immediate out of bounds",
    "8-bit specifier out of range",
    "32-bit specifier out of range",
    "Can't generate bit-field specifier",
    "Count operand out of range",
    "25 destination register out of range",
    "Illegal register specifier",
    "Can't generate register mask list",
    "Doesn't evaluate to effective address",
    "Illegal effective address",
    "Illegal destination address",
    "Illegal multiple destination address",
    "Illegal jump address",
    "4-bit vector out of range",
    "Address displacement doesn't evaluate",
    "8-bit displacement out of range",
    "16-bit displacement out of range",
    "Extent word doesn't evaluate correctly",
    "8-bit operand out of range",
    "8-bit extension word out of range",
    "16-bit extension word out of range",
    "32-bit extension word overflow",
    "16-bit displacement failed - overflow?",
    "8-bit displacement failed - overflow?",
    "Include error - can't open file",
    "Phase error - label value different on pass 2"
    };

err_out(err_num)
int err_num;
{
    register int x, y;
    char buf[85];
    extern int err_stk[MAXERR];	/* these two used to be 'static', moved to */
    extern int err_sp;		/*  root for overlay purposes */
    extern FLAG any_errors;	/* just to speed things up in overlay env */

    switch (err_num) {
    case NULL:
	for (x = MAXERR; --x >= err_sp; ) {
	    y = -(err_stk[x]) - 100;
	    if (pass == 1) {
		sprintf(buf, "Pass 1 error #%d at %d\n", y, line_count);
	    } else {
			if( line_count != 1 ) {
				sprintf(buf, "Error #%d at %d (%s)\n", y, line_count, err_msg[y]);
				printf("Error on Line %d: %s\n", line_count, err_msg[y]);
			}
			else
				sprintf(buf, "");
		}
		if (econ) {	puts(buf); }
		if (elst) { fputs(buf, lst_d); }
		if (elfile)	{ fputs(buf, lst_f); }
		if (efile)
		{   fprintf(err_f, ">> level: %d, line: %d\n", src_level + 1, line_count);
			fputs(buf, err_f);
	    }
	}			/* fall through to FLUSH */
    case FLUSH:
	err_sp = MAXERR;    				/* reset */
	any_errors = NO;
	break;
    default:
	any_errors = YES;
	if (err_sp) {
	    err_stk[--err_sp] = err_num;
	}
    }
    return err_num;
}


