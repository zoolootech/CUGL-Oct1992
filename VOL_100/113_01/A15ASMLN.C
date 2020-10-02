/*
	HEADER:		CUG113;
	TITLE:		1802 Cross-Assembler (BDS C Version);
	FILENAME:	A15ASMLN.C;
	VERSION:	1.2;
	DATE:		07/22/1985;

	DESCRIPTION:	"This program lets you use your CP/M-80-based computer
			to assemble code for the RCA 1802, 1804, 1805, 1805A,
			1806, AND 1806A microprocessors.  The program is
			written in BDS C for the best possible performance on
			8-bit machines.  All assembler features are supported
			except relocation, linkage, listing control, and
			macros.";

	KEYWORDS:	Software Development, Assemblers, Cross-Assemblers,
			RCA, CDP1802, CDP1805A;

	SEE-ALSO:	CUG149, 1805A Cross-Assembler (Portable);

	SYSTEM:		CP/M-80;
	COMPILERS:	BDS C;

	WARNINGS:	"This package is specifically tailored to CP/M-80
			machines and the rather non-standard, but high-
			performance BDS C compiler.  For other environments,
			use the portable version of this package on CUG149.";

	AUTHORS:	William C. Colley III;
*/

/*
	1805A Cross-Assembler  v 1.2

	Copyright (c) 1980, 82, 83, 85 William C. Colley, III.

	July 1982 -- Adapted from my 1802 cross-assembler.  WCC3.

	Vers 1.0 -- March 1983 -- Added 1805A opcodes to the 1805 set.  WCC3.

	Vers 1.1 -- March 1983 -- Added CPU pseudo-op to combine 1802 and 1805A
			cross-assemblers into a single program.  WCC3.

	Vers 1.2 -- June 1985 -- Fixed IF block nesting mechanism bug and bug
			in 1805A SCAL opcode.  WCC3.

File:	a15asmln.c

Line assembly routine.
*/

/*  Get globals:  */

#include "a15.h"

/*
This function is the workhorse of the assembler.  The routine
gets any labels off the line and processes them, gets the opcode
and builds the binary output as it evaluates the operand field.
*/

asmline()
{
    char c, label[2*SYMLEN], opcode, opattr, getchr();

    nbytes = 0;  address = pc;  directok = TRUE;
    evalerr = FALSE;  hexflg = NOCODE;  label[0] = '\0';
    binbuf[0] = binbuf[1] = binbuf[2] = binbuf[3] = 0xc4;

    switch (getchr(&c,NOSKIP)) {
	case ALPHA:	backchr;  getname(label);
			if (strcmp(label,"$") == 0 || chkoprat(label) !=
			    NO_OPR) { markerr('L');  label[0] = '\0'; }
			break;

	case END_LIN:	return;

	case BLANK:	break;

	default:	markerr('L');
			while((c = getchr(&c,NOSKIP)) != BLANK && c !=
			    END_LIN);
			backchr;
    }

    switch (getopcod(&opcode,&opattr)) {
	case 0:	    nbytes = 4;

	case -1:    opcode = 255;		/*  Set nil pseudo-op.	*/
		    opattr = 0x80;

	case 1:     if ((opattr & (PSOP | IFGROUP)) == (PSOP | IFGROUP)) {
			if (label[0] != '\0') markerr('L');
		    }
		    else {
			if (ifstack[ifsp] != ON) return;
			if (label[0] != '\0') {
			    strcat(label,PADDING);
			    if (pass == 1) {
				if (addsym(label)) {
				    sympoint->symvalu = pc;
				    if ((opattr & PSOP) && opcode == 10)
					sympoint->symname[1] |= 0x80;
				}
			    }
			    else {
				if (slookup(label)) {
				    markerr('P');  label[0] = '\0';
				}
				else {
				    if (sympoint->symvalu != pc ||
					(sympoint->symname[1] & 0x80))
					markerr('M');
				    sympoint->symname[0] &= 0x7f;
				}
			    }
			}
		    }
		    hexflg = PUTCODE;
		    if (opattr & PSOP) _psop(opcode,label);
		    else _normop(opcode,opattr);
		    return;
    }
}

/*
Internal function to process pseudo opcodes.
*/

_psop(opcode,label)
char opcode, *label;
{
    char *bptr, c, d;
    unsigned t, t2;
    struct symbtbl *sptr;

    sptr = sympoint;  bptr = binbuf;

    switch (opcode) {
	case 0:      t = eval(START);				/* ORG.	*/
		     if (evalerr || !directok) { markerr('P');  return; }
		     address = pc = t;  hexflg = FLUSH;
		     if (label[0] == '\0') goto chkargs;
		     backitem(t,VALUE);  backchr;

	equlabl:
	case 1:     if (label[0] == '\0') {			/* EQU.	*/
			markerr('L');  return;
		    }
		    if (!(sptr->symname[1] & 0x80) && errcode == 'M') {
			errcode = ' ';  errcount--;
		    }
		    t = eval(START);
		    if (evalerr || !directok) {
			markerr('P');  return;
		    }
		    address = sptr->symvalu;  sptr->symvalu = t;
		    if (address != t) markerr('M');
		    goto chkargs;

	case 2:						/* BYTE.	*/
		    while ((c = getitem(&t,SMALST)) != END_LIN) {
			if (c == COMMA) {
			    *bptr++ = 0;  nbytes++;  continue;
			}
			backitem(t,c);  t = eval(START);
			if (t > 0xff && t < 0xff80) { markerr('V');  t = 0; }
			*bptr++ = t;  nbytes++;
		    }
		    return;

	case 3:						/* WORD.	*/
		    while ((c = getitem(&t,SMALST)) != END_LIN) {
			nbytes += 2;
			if (c == COMMA) *bptr++ = *bptr++ = 0;
			else {
			    backitem(t,c);  t = eval(START);
			    *bptr++ = t >> 8;  *bptr++ = t;
			}
		    }
		    return;

	case 4:						/* TEXT.	*/
		    while ((c = getchr(&d,SKIP)) != END_LIN) {
			if (c == COMMA) continue;
			if (c != QUOTE) { markerr('"');  return; }
			while ((c = *linptr++) != d) {
			    if (c == '\n') { markerr('"');  return; }
			    *bptr++ = c;  nbytes++;
			}
		    }
		    return;

	case 5:     t = eval(START);				/* BLK.	*/
		    if (evalerr || !directok) { markerr('P');  return; }
		    pc += t;  hexflg = FLUSH;  goto chkargs;

	case 6:     if (pass == 1) pass--;			/* END.	*/
		    else { pass++;  if (ifsp != 0) markerr('I'); }
		    hexflg = NOMORE;  goto chkargs;

	case 7:						/*  ELSE.	*/
	case 8:     hexflg = NOCODE;			/*  ENDI.	*/
		    if (ifsp == 0) { markerr('I');  return; }
		    if (opcode == 8) ifsp--;
		    else ifstack[ifsp] = -ifstack[ifsp];
		    goto chkargs;

	case 9:     address = eval(START);			/* IF.	*/
		    if (evalerr || !directok) {
			markerr('P');  address = TRUE;
		    }
		    if (ifsp == IFDEPTH - 1) wipeout("\nIf stack overflow.\n");
		    if (ifstack[ifsp++] != ON) {
			hexflg = NOCODE;  ifstack[ifsp] = NULL;
		    }
		    else ifstack[ifsp] = address ? ON : OFF;
		    goto chkargs;

	case 10:    if (label[0] == '\0') {			/* SET.	*/
			markerr('L');  return;
		    }
		    if ((sptr->symname[1] & 0x80) && errcode == 'M') {
			errcode = ' ';  errcount--;
		    }
		    address = eval(START);
		    if (evalerr) return;
		    sptr->symvalu = address;
		    if (!directok) markerr('P');

	chkargs:    if (getitem(&c,SMALST) != END_LIN) markerr('T');
		    return;

	case 11:					/*  PAGE.	*/
		    address = pc = pc + 255 & 0xff00;  hexflg = FLUSH;
		    if (label[0] == '\0') goto chkargs;
		    while (getitem(&c,SMALST) != END_LIN) markerr('T');
		    backchr;  backitem(pc,VALUE);  goto equlabl;

	case 12:    nbytes = 6;				/*  LOAD.	*/
		    binbuf[0] = binbuf[3] = 0xf8;  binbuf[1] = binbuf[4] = 0;
		    binbuf[2] = 0xb0;  binbuf[5] = 0xa0;
		    t = eval(START);  t2 = eval(START);
		    if (t > 15) { markerr('R');  t = 0; }
		    binbuf[1] = t2 >> 8;  binbuf[4] = t2 & 0xff;
		    binbuf[2] |= t;  binbuf[5] |= t;  goto chkargs;

	case 13:    						/* CPU. */
		    if (((t = eval(START)) != 1802 && t != 1805) || evalerr)
			markerr('V');
		    else extend = t == 1805;
		    goto chkargs;

	case 255:   if (nbytes == 0) hexflg = NOCODE;	/*  NO OPCODE.	*/
		    return;
    }
}

/*
Internal function to process normal (non-pseudo) opcodes.
*/

_normop(opcode,attr)
char opcode,attr;
{
    unsigned bindx, operand;

    nbytes = attr & BYTES;
    if (pass == 1) return;
    bindx = 0;
    if (attr & XOPC) binbuf[bindx++] = 0x68;
    if (attr & PROCNUM) operand = eval(START);

    switch (attr & PROCNUM) {
	case 8:					/*  Register Group.	*/
		    if (operand < 16) opcode |= operand;
		    else markerr('R');
		    if (opcode == 0) { markerr('R');  opcode++; }

	case 0:     break;			/*  No-argument Group.	*/

	case 16:				/*  Immediate Group.	*/
		    if (operand > 0xff && operand < 0xff80) {
			markerr('V');  operand = 0;
		    }
		    binbuf[bindx+1] = operand;  break;

	case 24:				/*  Short Branch Group.	*/
		    if (evalerr || (operand & 0xff00) !=
			(pc + (attr & XOPC ? 2 : 1) & 0xff00)) {
			operand = pc;  markerr('B');
		    }
		    binbuf[bindx+1] = operand & 0xff;  break;

	case 56:    if (operand < 16) opcode |= operand;      /*  DBNZ  */
		    else markerr('R');
		    operand = eval(START);

	case 32:    if (evalerr) operand = pc;	/*  Long Branch Group.	*/
		    binbuf[bindx+1] = operand >> 8;
		    binbuf[bindx+2] = operand & 0xff;  break;

	case 40:				/*  Input/Output Group.	*/
		    if (operand == 0 || operand > 7) {
			markerr('V');  operand = 1;
		    }
		    opcode |= operand;  break;

	case 48:    if (operand < 16) opcode |= operand;      /*  RLDI  */
		    else markerr('R');
		    binbuf[bindx+1] = (operand = eval(START)) >> 8;
		    binbuf[bindx+2] = operand & 0xff;  break;
    }

    binbuf[bindx] = opcode;
    if (getitem(&bindx,SMALST) != END_LIN) markerr('T');
    return;
}

		    binbuf[bindx+1] = operand;  break;

	case 24:				/*  Short Branch Group.	*/
		    if 