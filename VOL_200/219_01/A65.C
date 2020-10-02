/*
	HEADER:		CUG219;
	TITLE:		6502 Cross-Assembler (Portable);
	FILENAME:	A65.C;
	VERSION:	0.1;
	DATE:		08/27/1988;

	DESCRIPTION:	"This program lets you use your computer to assemble
			code for the MOS Technology 6502 microprocessors.  The
			program is written in portable C rather than BDS C.
			All assembler features are supported except relocation
			linkage, and macros.";

	KEYWORDS:	Software Development, Assemblers, Cross-Assemblers,
			MOS Technology, 6502;

	SYSTEM:		CP/M-80, CP/M-86, HP-UX, MSDOS, PCDOS, QNIX;
	COMPILERS:	Aztec C86, Aztec CII, CI-C86, Eco-C, Eco-C88, HP-UX,
			Lattice C, Microsoft C,	QNIX C;

	WARNINGS:	"This program is written in as portable C as possible.
			A port to BDS C would be extremely difficult, but see
			volume CUG113.  A port to Toolworks C is untried."

	AUTHORS:	William C. Colley III;
*/

/*
		      6502 Cross-Assembler in Portable C

		   Copyright (c) 1986 William C. Colley, III

Revision History:

Ver	Date		Description

0.0	NOV 1986	Derived from my 6800/6801 cross-assembler.  WCC3.

0.1	AUG 1988	Fixed a bug in the command line parser that puts it
			into a VERY long loop if the user types a command line
			like "A65 FILE.ASM -L".  WCC3 per Alex Cameron.

This file contains the main program and line assembly routines for the
assembler.  The main program parses the command line, feeds the source lines to
the line assembly routine, and sends the results to the listing and object file
output routines.  It also coordinates the activities of everything.  The line
assembly routines uses the expression analyzer and the lexical analyzer to
parse the source line and convert it into the object bytes that it represents.
*/

/*  Get global goodies:  */

#include "a65.h"

/*  Define global mailboxes for all modules:				*/

char errcode, line[MAXLINE + 1], title[MAXLINE];
int pass = 0;
int eject, filesp, forwd, listhex;
unsigned address, argattr, bytes, errors, listleft, obj[MAXLINE], pagelen, pc;
FILE *filestk[FILES], *source;
TOKEN token;

/*  Mainline routine.  This routine parses the command line, sets up	*/
/*  the assembler at the beginning of each pass, feeds the source text	*/
/*  to the line assembler, feeds the result to the listing and hex file	*/
/*  drivers, and cleans everything up at the end of the run.		*/

static int done, ifsp, off;

void main(argc,argv)
int argc;
char **argv;
{
    SCRATCH unsigned *o;
    int newline();
    void asm_line();
    void lclose(), lopen(), lputs();
    void hclose(), hopen(), hputc();
    void error(), fatal_error(), warning();

    printf("6502 Cross-Assembler (Portable) Ver 0.1\n");
    printf("Copyright (c) 1986 William C. Colley, III\n\n");

    while (--argc > 0) {
	if (**++argv == '-') {
	    switch (toupper(*++*argv)) {
		case 'L':   if (!*++*argv) {
				if (!--argc) { warning(NOLST);  break; }
				else ++argv;
			    }
			    lopen(*argv);
			    break;

		case 'O':   if (!*++*argv) {
				if (!--argc) { warning(NOHEX);  break; }
				else ++argv;
			    }
			    hopen(*argv);
			    break;

		default:    warning(BADOPT);
	    }
	}
	else if (filestk[0]) warning(TWOASM);
	else if (!(filestk[0] = fopen(*argv,"r"))) fatal_error(ASMOPEN);
    }
    if (!filestk[0]) fatal_error(NOASM);

    while (++pass < 3) {
	fseek(source = filestk[0],0L,0);  done = off = FALSE;
	errors = filesp = ifsp = pagelen = pc = 0;  title[0] = '\0';
	while (!done) {
	    errcode = ' ';
	    if (newline()) {
		error('*');
		strcpy(line,"\tEND\n");
		done = eject = TRUE;  listhex = FALSE;
		bytes = 0;
	    }
	    else asm_line();
	    pc = word(pc + bytes);
	    if (pass == 2) {
		lputs();
		for (o = obj; bytes--; hputc(*o++));
	    }
	}
    }

    fclose(filestk[0]);  lclose();  hclose();

    if (errors) printf("%d Error(s)\n",errors);
    else printf("No Errors\n");

    exit(errors);
}

/*  Line assembly routine.  This routine gets the contents of the	*/
/*  argument field from the source file using the expression evaluator	*/
/*  and lexical analyzer.  It makes all validity checks on the		*/
/*  arguments validity, fills a buffer with the machine code bytes and	*/
/*  returns nothing.							*/

static char label[MAXLINE];
static int ifstack[IFDEPTH] = { ON };

static OPCODE *opcod;

void asm_line()
{
    SCRATCH int i;
    int isalph(), popc();
    OPCODE *find_code(), *find_operator();
    void do_label(), flush(), normal_op(), pseudo_op();
    void error(), pops(), pushc(), trash();

    address = pc;  bytes = 0;  eject = forwd = listhex = FALSE;
    for (i = 0; i < BIGINST; obj[i++] = NOP);

    label[0] = '\0';
    if ((i = popc()) != ' ' && i != '\n') {
	if (isalph(i)) {
	    pushc(i);  pops(label);
	    if (find_operator(label)) { label[0] = '\0';  error('L'); }
	}
	else {
	    error('L');
	    while ((i = popc()) != ' ' && i != '\n');
	}
    }

    trash();  opcod = NULL;
    if ((i = popc()) != '\n') {
	if (!isalph(i)) error('S');
	else {
	    pushc(i);  pops(token.sval);
	    if (!(opcod = find_code(token.sval))) error('O');
	}
	if (!opcod) { listhex = TRUE;  bytes = BIGINST; }
    }

    if (opcod && opcod -> attr & ISIF) { if (label[0]) error('L'); }
    else if (off) { listhex = FALSE;  flush();  return; }

    if (!opcod) { do_label();  flush(); }
    else {
	listhex = TRUE;
	if (opcod -> attr & PSEUDO) pseudo_op();
	else normal_op();
	while ((i = popc()) != '\n') if (i != ' ') error('T');
    }
    source = filestk[filesp];
    return;
}

static void flush()
{
    while (popc() != '\n');
}

static void do_label()
{
    SCRATCH SYMBOL *l;
    SYMBOL *find_symbol(), *new_symbol();
    void error();

    if (label[0]) {
	listhex = TRUE;
	if (pass == 1) {
	    if (!((l = new_symbol(label)) -> attr)) {
		l -> attr = FORWD + VAL;
		l -> valu = pc;
	    }
	}
	else {
	    if (l = find_symbol(label)) {
		l -> attr = VAL;
		if (l -> valu != pc) error('M');
	    }
	    else error('P');
	}
    }
}

static void normal_op()
{
    SCRATCH unsigned opcode, operand;
    unsigned do_args();
    void do_label(), error();

    opcode = opcod -> valu;  bytes = BIGINST;
    do_label();  operand = do_args();
    switch (opcod -> attr) {
	case CPXY:  if (argattr & ARGIMM) goto do_immediate;
		    opcode += 0x04;

	case BITOP: if (argattr != ARGNUM) { error('A');  return; }
		    goto do_zero_page;

	case INHOP: if (argattr) error('T');
	    	    bytes = 1;  break;

	case JUMP:  if (argattr == (ARGIND + ARGNUM)) {
			opcode += 0x20;  break;
		    }

	case CALL:  if (argattr != ARGNUM) { error('A');  return; }
		    break;

	case LOGOP: if (!(argattr & ARGA)) goto do_inc_op;
		    opcode += 0x04;  bytes = 1;  break;

	case RELBR: if (argattr != ARGNUM) { error('A');  return; }
	    	    bytes = 2;  operand -= pc + 2;
		    if (clamp(operand) > 0x007f && operand < 0xff80) {
			error('B');  operand = 0xfffe;
		    }
		    break;

	case STXY:  if ((argattr & (opcode == 0x86 ? ~ARGY : ~ARGX)) !=
			ARGNUM) { error ('A');  return; }
		    if (argattr & (ARGX + ARGY)) {
			if (operand > 0x00ff) {	error('V');  operand = 0; }
			opcode += 0x10;  bytes = 2;  break;
		    }
		    goto do_zero_page;

	case TWOOP: if (!(argattr & ARGNUM)) { error('A');  return; }
		    if (argattr & ARGIMM) {
			if (opcode == 0x81) { error('A');  return; }
			opcode += 0x08;  goto do_immediate;
		    }
		    if (argattr & ARGIND) {
			if (argattr & ARGY) opcode += 0x10;
			else if (!(argattr & ARGX)) { error('A');  return; }
			if (operand > 0x00ff) { error('V');  operand = 0; }
			bytes = 2;
		    }
		    else if (argattr & ARGY) opcode += 0x18;
		    else { opcode += 0x04;  goto do_indexed_x; }
		    break;

	case LDXY:  if (!(argattr & ARGIMM)) {
			if (opcode == 0xa2 && (argattr & ARGY))
			    argattr ^= (ARGX + ARGY);
			opcode += 0x04;  goto do_inc_op;
		    }
do_immediate:	    if (operand > 0x00ff && operand < 0xff80) {
			error('V');  operand = 0;
		    }
		    bytes = 2;  break;

do_inc_op:
	case INCOP: if ((argattr & ~ARGX) != ARGNUM) { error('A');  return; }
do_indexed_x:	    if (argattr & ARGX) opcode += 0x10;
do_zero_page:	    if (!forwd && operand <= 0x00ff) bytes = 2;
		    else opcode += 0x08;
		    break;
    }
    obj[2] = high(operand);  obj[1] = low(operand);  obj[0] = opcode;
    return;
}

static void pseudo_op()
{
    SCRATCH char *s;
    SCRATCH unsigned *o, u;
    SCRATCH SYMBOL *l;
    unsigned expr();
    SYMBOL *find_symbol(), *new_symbol();
    TOKEN *lex();
    void do_label(), error(), fatal_error(), hseek(), unlex();

    o = obj;
    switch (opcod -> valu) {
	case ELSE:  listhex = FALSE;
		    if (ifsp) off = (ifstack[ifsp] = -ifstack[ifsp]) != ON;
		    else error('I');
		    break;

	case END:   do_label();
		    if (filesp) { listhex = FALSE;  error('*'); }
		    else {
			done = eject = TRUE;
			if (pass == 2 && (lex() -> attr & TYPE) != EOL) {
			    unlex();  hseek(address = expr());
			}
			if (ifsp) error('I');
		    }
		    break;

	case ENDI:  listhex = FALSE;
		    if (ifsp) off = ifstack[--ifsp] != ON;
		    else error('I');
		    break;

	case EQU:   if (label[0]) {
			if (pass == 1) {
			    if (!((l = new_symbol(label)) -> attr)) {
				l -> attr = FORWD + VAL;
				address = expr();
				if (!forwd) l -> valu = address;
			    }
			}
			else {
			    if (l = find_symbol(label)) {
				l -> attr = VAL;
				address = expr();
				if (forwd) error('P');
				if (l -> valu != address) error('M');
			    }
			    else error('P');
			}
		    }
		    else error('L');
		    break;

	case FCB:   do_label();
		    do {
			if ((lex() -> attr & TYPE) == SEP) u = 0;
			else {
			    unlex();
			    if ((u = expr()) > 0xff && u < 0xff80) {
				u = 0;  error('V');
			    }
			}
			*o++ = low(u);  ++bytes;
		    } while ((token.attr & TYPE) == SEP);
		    break;

	case FCC:   do_label();
		    while ((lex() -> attr & TYPE) != EOL) {
			if ((token.attr & TYPE) == STR) {
			    for (s = token.sval; *s; *o++ = *s++)
				++bytes;
			    if ((lex() -> attr & TYPE) != SEP) unlex();
			}
			else error('S');
		    }
		    break;

	case FDB:   do_label();
		    do {
			if ((lex() -> attr & TYPE) == SEP) u = 0;
			else { unlex();  u = expr(); }
			*o++ = low(u);  *o++ = high(u);
			bytes += 2;
		    } while ((token.attr & TYPE) == SEP);
		    break;

	case IF:    if (++ifsp == IFDEPTH) fatal_error(IFOFLOW);
		    address = expr();
		    if (forwd) { error('P');  address = TRUE; }
		    if (off) { listhex = FALSE;  ifstack[ifsp] = NULL; }
		    else {
			ifstack[ifsp] = address ? ON : OFF;
			if (!address) off = TRUE;
		    }
		    break;

	case INCL:  listhex = FALSE;  do_label();
		    if ((lex() -> attr & TYPE) == STR) {
			if (++filesp == FILES) fatal_error(FLOFLOW);
			if (!(filestk[filesp] = fopen(token.sval,"r"))) {
			    --filesp;  error('V');
			}
		    }
		    else error('S');
		    break;

	case ORG:   u = expr();
		    if (forwd) error('P');
		    else {
			pc = address = u;
			if (pass == 2) hseek(pc);
		    }
		    do_label();
		    break;

	case PAGE:  listhex = FALSE;  do_label();
		    if ((lex() -> attr & TYPE) != EOL) {
			unlex();  pagelen = expr();
			if (pagelen > 0 && pagelen < 3) {
			    pagelen = 0;  error('V');
			}
		    }
		    eject = TRUE;
		    break;

	case RMB:   do_label();
		    u = word(pc + expr());
		    if (forwd) error('P');
		    else {
			pc = u;
			if (pass == 2) hseek(pc);
		    }
		    break;

	case SET:   if (label[0]) {
			if (pass == 1) {
			    if (!((l = new_symbol(label)) -> attr)
				|| (l -> attr & SOFT)) {
				l -> attr = FORWD + SOFT + VAL;
				address = expr();
				if (!forwd) l -> valu = address;
			    }
			}
			else {
			    if (l = find_symbol(label)) {
				address = expr();
				if (forwd) error('P');
				else if (l -> attr & SOFT) {
				    l -> attr = SOFT + VAL;
				    l -> valu = address;
				}
				else error('M');
			    }
			    else error('P');
			}
		    }
		    else error('L');
		    break;

	case TITL:  listhex = FALSE;  do_label();
		    if ((lex() -> attr & TYPE) == EOL) title[0] = '\0';
		    else if ((token.attr & TYPE) != STR) error('S');
		    else strcpy(title,token.sval);
		    break;
    }
    return;
}
