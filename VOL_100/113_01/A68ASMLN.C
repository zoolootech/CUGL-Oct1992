/*
	HEADER:		CUG113;
	TITLE:		6800 Cross-Assembler (BDS C Version);
	FILENAME:	A68ASMLN.C;
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

File:	a68asmln.c

Line assembly routine.
*/

/*  Get globals:  */

#include "a68.h"

/*
This function is the workhorse of the assembler.  The routine
gets any labels off the line and processes them, gets the opcode
and builds the binary output as it evaluates the operand field.
*/

asmline()
{
    char c, label[2*SYMLEN], opcode, opattr;

    nbytes = 0;  address = pc;  directok = TRUE;
    evalerr = FALSE;  hexflg = NOCODE;  label[0] = '\0';
    binbuf[0] = binbuf[1] = binbuf[2] = 0x01;

    switch (getchr(&c,NOSKIP)) {
	case ALPHA:	backchr;  getname(label);
			if ((!label[1] && ((c = toupper(label[0])) == 'A' ||
			    c == 'B' || c == 'X')) || chkoprat(label) !=
			    NO_OPR) { markerr('L');  label[0] = '\0'; }
			break;

	case END_LIN:	return;

	case BLANK:	break;

	default:	while((c = getchr(&c,NOSKIP)) != BLANK &&
			    c != END_LIN);
			markerr('L');  backchr;
    }

    switch (getopcod(&opcode,&opattr)) {
	case 0:     nbytes = 3;

	case -1:    opcode = 255;  opattr = 0x80; /* Set nil pseudo-op.	*/

	case 1:     if ((opattr & (PSOP | IFGROUP)) == (PSOP | IFGROUP)) {
			if (label[0] != '\0') markerr('L');
		    }
		    else {
			if (ifstack[ifsp] != ON) return;
			if (label[0]) {
			    strcat(label,PADDING);
			    if (pass == 1) {
				if (addsym(label)) {
				    sympoint -> symvalu = pc;
				    if ((opattr & PSOP) && opcode == 10)
					sympoint -> symname[1] |= 0x80;
				}
			    }
			    else {
				if (slookup(label)) {
				    markerr('P');  label[0] = '\0';
				}
				else {
				    if (sympoint -> symvalu != pc ||
					(sympoint -> symname[1] & 0x80))
					markerr('M');
				    sympoint -> symname[0] &= 0x7f;
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
    unsigned t;
    struct symbtbl *sptr;

    sptr = sympoint;  bptr = binbuf;
    switch (opcode) {
	case 0:     t = eval(START);				/* ORG	*/
		    if (evalerr || !directok) { markerr('P');  return; }
		    address = pc = t;  hexflg = FLUSH;
		    if (!label[0]) goto chkargs;
		    backitem(t,VALUE);  backchr;

	case 1:     if (!label[0]) {				/* EQU	*/
			markerr('L');  return;
		    }
		    if (!(sptr -> symname[1] & 0x80) && errcode == 'M') {
			errcode = ' ';  --errcount;
		    }
		    t = eval(START);
		    if (evalerr || !directok) { markerr('P');  return; }
		    address = sptr -> symvalu;
		    if (!directok) markerr('P');
		    if (address != (sptr -> symvalu = t)) markerr('M');
		    goto chkargs;

	case 2:							/* FCB	*/
		    while ((c = getitem(&t,SMALST)) != END_LIN) {
			if (c == COMMA) t = 0;
			else {
			    backitem(t,c);
			    if ((t = eval(START)) > 0xff && t < 0xff80) {
				markerr('V');  t = 0;
			    }
			}
			*bptr++ = t;  ++nbytes;
		    }
		    return;

	case 3:							/* FDB	*/
		    while ((c = getitem(&t,SMALST)) != END_LIN) {
			if (c == COMMA) t = 0;
			else { backitem(t,c);  t = eval(START); }
			*bptr++ = t >> 8;  *bptr++ = t;  nbytes += 2;
		    }
		    return;

	case 4:     while ((c = getchr(&d,SKIP)) != END_LIN) {	/* FCC	*/
			if (c == COMMA) continue;
			if (c != QUOTE) { markerr('"');  return; }
			while ((c = *linptr++) != d) {
			    if (c == '\n') { markerr('"');  return; }
			    *bptr++ = c;  ++nbytes;
			}
		    }
		    return;

	case 5:     t = eval(START);				/* RMB	*/
		    if (evalerr || !directok) { markerr('P');  return; }
		    pc += t;  hexflg = FLUSH;  goto chkargs;

	case 6:     if (pass == 1) --pass;			/* END	*/
		    else {
			++pass;
			if (ifsp) markerr('I');
		    }
		    hexflg = NOMORE;  goto chkargs;

	case 7:							/* ELSE	*/
	case 8:     hexflg = NOCODE;				/* ENDI	*/
		    if (!ifsp) { markerr('I');  return; }
		    if (opcode == 8) --ifsp;
		    else ifstack[ifsp] = -ifstack[ifsp];
		    goto chkargs;

	case 9:     address = eval(START);			/* IF	*/
		    if (evalerr || !directok) {
			markerr('P');  address = TRUE;
		    }
		    if (ifsp == IFDEPTH - 1) wipeout("\nIf stack overflow.\n");
		    if (ifstack[ifsp++] != ON) {
			hexflg = NOCODE;  ifstack[ifsp] = NULL;
		    }
		    else ifstack[ifsp] = address ? ON : OFF;
		    goto chkargs;

	case 10:    if (!label[0]) {				/* SET	*/
			markerr('L');  return;
		    }
		    if ((sptr -> symname[1] & 0x80) && errcode == 'M') {
			errcode = ' ';  --errcount;
		    }
		    address = eval(START);
		    if (evalerr) return;
		    sptr -> symvalu = address;
		    if (!directok) markerr('P');
		    goto chkargs;

	case 11:    hexflg = NOCODE;				/* CPU	*/
		    t = eval(START);
		    if (evalerr || !directok) { markerr('P');  return; }
		    if (t != 6800 && t != 6801) { markerr('V');  return; }
		    extend = t == 6801;

	chkargs:    if (getitem(&c,SMALST) != END_LIN) markerr('T');
		    return;

	case 255:   if (!nbytes) hexflg = NOCODE;	/*  NO OPCODE	*/
		    return;
    }
}

/*
Internal function to process normal (non-pseudo) opcodes.
*/

_normop(opcode,attr)
char opcode,attr;
{
    unsigned value, operand;
    char numcntl, c;

    nbytes = 1;
    numcntl = operand = 0;

    if (extend && opcode == 0x8d) attr |= DIROK;
    if ((attr & IS6801) == IS6801) {
	attr &= ~IS6801;
	if (!extend) { nbytes = 3;  markerr('O');  return; }
    }

    for (;;) {
	switch (c = getitem(&value,SMALST)) {
	    case REGISTR:   switch (value) {
				case 'B':   if (opcode < 0x40) ++opcode;
					    else opcode |= opcode < 0x80 ?
						0x10 : 0x40;

				case 'A':   if (attr & REGOPT) {
						attr = 0;  numcntl = 2;
						break;
					    }
					    if (attr & REGREQ) {
						attr &= ~REGREQ;  break;
					    }
					    nbytes = 3;  markerr('R');
					    return;

				case 'X':   nbytes = 3;
					    if (!(attr & INDEX)) {
						markerr('A');  return;
					    }
					    if (numcntl == 2 &&
						operand > 0xff) {
						markerr('V');  return;
					    }
					    numcntl = 1;  nbytes = 2;
					    opcode = (opcode | 0x20) & 0xef;
					    attr &= REGREQ;  break;
			    }
			    if ((c = getitem(&value,SMALST)) != COMMA)
				backitem(value,c);
			    break;

	    case IMMED:     nbytes = 3;
			    if (!(attr & (IMM8 | IMM16))) {
				markerr('A');  return;
			    }
			    operand = eval(START);
			    if (evalerr) return;
			    if (attr & IMM8) {
				if (operand > 0xff && operand < 0xff80) {
				    markerr('V');  return;
				}
				--nbytes;
			    }
			    attr &= REGREQ;  break;

	    case OPERATR:
	    case VALUE:     backitem(value,c);
			    operand = eval(START);
			    if (attr & REL) {
				numcntl = nbytes = 2;  operand -= pc + 2;
				if (evalerr || (operand > 0x7f &&
				    operand < 0xff80)) 	{
				    markerr('B');  binbuf[0] = opcode;
				    binbuf[1] = 0xfe;  return;
				}
				attr = 0;  break;
			    }

	    case COMMA:     nbytes = 3;
			    if (evalerr) return;
			    if (numcntl == 1) {
				if (operand > 0xff && operand < 0xff80) {
				    markerr('V');  return;
				}
				numcntl = nbytes = 2;  break;
			    }
			    if (numcntl == 2 || !(attr & INDEX)) {
				markerr('S');  return;
			    }
			    numcntl = 2;  opcode |= 0x30;
			    attr &= (INDEX | REGREQ | DIROK);  break;

	    case END_LIN:   if (attr & (~(DIROK | INDEX))) {
				markerr(attr & REGREQ || (attr & (REGOPT |
				    INDEX)) == (REGOPT | INDEX) ? 'R' : 'S');
				nbytes = 3;  return;
			    }
			    if (attr & DIROK && directok && operand <= 0xff) {
				nbytes = 2;  opcode ^= 0x20;
			    }
			    binbuf[0] = opcode;
			    if (nbytes == 2) binbuf[1] = operand;
			    else if (nbytes == 3) {
				binbuf[1] = operand >> 8;
				binbuf[2] = operand;
			    }
			    return;
	}
    }
}
0)) 	{
				    markerr('B');  binbuf[0] = opcode;
				    binbuf[1] = 0xfe;  return;
				}
				attr