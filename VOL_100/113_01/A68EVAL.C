/*
	HEADER:		CUG113;
	TITLE:		6800 Cross-Assembler (BDS C Version);
	FILENAME:	A68EVAL.C;
	VERSION:	2.6;
	DATE:		07/22/1985;

	DESCRIPTION:	"This program lets you use your CP/M-80-based computer
			to assemble code for the Motorola 6800, 6801, 6802,
			6803, 6808, and 68701 microprocessors.  The program is
			written in BDS C for the best possible performance on
			8-bit machines.  All assembler features are supported
			except relocation, linkage, listing control, and
			macros.";

	KEYWORDS:	Software Development, Assemblers, Cross-Assemblers,
			Motorola, MC6800, MC6801;

	SEE-ALSO:	CUG149, 6801 Cross-Assembler (Portable);

	SYSTEM:		CP/M-80;
	COMPILERS:	BDS C;

	WARNINGS:	"This package is specifically tailored to CP/M-80
			machines and the rather non-standard, but high-
			performance BDS C compiler.  For other environments,
			use the portable version of this package on CUG149.";

	AUTHORS:	William C. Colley III;
*/

/*
	6800/6801 Cross-Assembler  v. 2.6

	May, 1980

	July, 1980 -- Rev. 2.2 consisting of fixing the M errors that
		come from forward references in FDB and FCB pseudo-ops.

	October, 1980 -- Rev. 2.3 consisting of updating the assembly
		language file and I/O routines to match and take
		advantage of BDS C V1.4.

	October, 1983 -- Rev. 2.4 consisting of adding the CPU pseudo-op,
		adding the 6801 CPU's extra opcodes, and speeding up the
		code a bit.

	September, 1984 -- Rev. 2.5 consisting of fixing bugs with the symbol
		table sort, the writing of files to specified drives, and the
		handling of blank input lines.

	June, 1985 -- Rev. 2.6 consisting of fixing a bug in the IF block
		nesting mechanism.

	Copyright (c) 1980,83,84,85 William C. Colley, III.

File:	a68eval.c

Routines to crunch on source text chunks and give back
evaluated expressions, opcode parameters, etc.
*/

/*  Get Globals:  */

#include "a68.h"

/*
Function to evaluate the next expression on the present source line.
Function returns the value of the expression.  In addition, a global
flag, evalerr, is set to TRUE if an error occurred anywhere in the
evaluation process.  The precidence parameter, prec, is for the benefit
of the recursion.  In the first call to the routine, use START.
*/

eval(prec)
char prec;
{
    char getitem(), c;
    unsigned valu1, valu2;

    for (;;) {
        switch (getitem(&valu1,SMALST)) {
            case REGISTR:
            case IMMED:	  evalerr = TRUE;  markerr('S');  break;

            case COMMA:	  if (prec != START) backitem(0,COMMA);
                          goto enderr;

            case END_LIN: backitem(0,END_LIN);
            enderr:       evalerr = TRUE;  markerr('E');  return 0;

            case OPERATR: if (valu1 == NOT) valu1 = ~eval(UOP2);
                          else if (valu1 == '-') valu1 = -eval(UOP1);
                          else if (valu1 == '+') valu1 = eval(UOP1);
                          else if (valu1 == '(') valu1 = eval(LPREN);
                          else if (valu1 == HIGH) valu1 = eval(UOP3) >> 8;
                          else if (valu1 == LOW) valu1 = eval(UOP3) & 0xff;
                          else if (valu1 == '*') valu1 = pc;
                          else { evalerr = TRUE;  markerr('E');  break; }

            case VALUE:   for (;;) {
                              switch (getitem(&valu2,SMALST)) {
                                  case REGISTR:
                                  case IMMED:   evalerr = TRUE;  markerr('S');
                                                break;

                                  case COMMA:   if (prec != START) backitem(0,COMMA);
                                                goto endeval;

                                  case END_LIN: backitem(0,END_LIN);
                                  endeval:      if (prec == LPREN) {
                                                    evalerr = TRUE;
                                                    markerr('(');
                                                }
                                                return (evalerr) ? 0 : valu1;

                                  case VALUE:   evalerr = TRUE;  markerr('E');
                                                break;

                                  case OPERATR: if (valu2 == '(' || valu2 == NOT || valu2 == HIGH || valu2 == LOW) {
                                                    evalerr = TRUE;
						    markerr('E');
                                                    break;
                                                }
                                                if (prec <= getprec(valu2)) {
						    backitem(valu2,OPERATR);
                                                    return valu1;
                                                }
                                                switch (c = valu2) {
                                                    case '+':   valu1 += eval(ADDIT); break;

                                                    case '-':   valu1 -= eval(ADDIT); break;

                                                    case '*':   valu1 *= eval(MULT); break;

                                                    case '/':   valu1 /= eval(MULT); break;

                                                    case MOD:   valu1 %= eval(MULT); break;

                                                    case AND:	valu1 &= eval(LOG1); break;

                                                    case OR:    valu1 |= eval(LOG2); break;

                                                    case XOR:   valu1 ^= eval(LOG2); break;

                                                    case '>':   valu1 = (valu1 > eval(RELAT)) ? 0xffff : 0; break;

                                                    case GRTEQ: valu1 = (valu1 >= eval(RELAT)) ? 0xffff : 0; break;

                                                    case '=':   valu1 = (valu1 == eval(RELAT)) ? 0xffff : 0; break;

                                                    case NOTEQ: valu1 = (valu1 != eval(RELAT)) ? 0xffff : 0; break;

                                                    case '<':   valu1 = (valu1 < eval(RELAT)) ? 0xffff : 0; break;

                                                    case LESEQ: valu1 = (valu1 <= eval(RELAT)) ? 0xffff : 0; break;

                                                    case SHL:   if ((valu2 = eval(MULT)) > 15) {
                                                                    evalerr = TRUE;
                                                                    markerr('E');
                                                                    break;
                                                                }
                                                                valu1 = valu1 << valu2;
                                                                break;

                                                    case SHR:   if ((valu2 = eval(MULT)) > 15) {
                                                                    evalerr = TRUE;
                                                                    markerr('E');
                                                                    break;
                                                                }
                                                                valu1 = valu1 >> valu2;
                                                                break;

                                                    case ')':   if (prec == LPREN) return valu1;
                                                                else {
                                                                    evalerr = TRUE;
                                                                    markerr('(');
                                                                    break;
                                                                }
                                                }
                              }
                          }
        }
    }
}

/*
Function to get the precedence of a binary operator.
Function returns the precedence value.
*/

getprec(operator)
char operator;
{
    switch (operator) {
	case '*':
	case '/':
	case MOD:
	case SHL:
	case SHR:   return MULT;

	case '+':
	case '-':   return ADDIT;

	case '>':
	case '=':
	case '<':
	case GRTEQ:
	case NOTEQ:
	case LESEQ: return RELAT;

	case AND:   return LOG1;

	case OR:
	case XOR:   return LOG2;

	case HIGH:
	case LOW:   return UOP3;

	case ')':   return RPREN;
    }
}

/*
Function to mark up an error in the line if one isn't
already marked up.  The error code letter comes in
through letter.
*/

markerr(letter)
char letter;
{
    if (errcode == ' ') { errcode = letter;  ++errcount; }
}
                              