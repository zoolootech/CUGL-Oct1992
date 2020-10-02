/*
	HEADER:		CUG219;
	TITLE:		8048 Cross-Assembler (Portable);
	FILENAME:	A48UTIL.C;
	VERSION:	0.2;
	DATE:		11/09/1988;

	DESCRIPTION:	"This program lets you use your computer to assemble
			code for the Intel 8048 family of microprocessors.
			The program is written in portable C rather than BDS
			C.  All assembler features are supported except
			relocation, linkage, and macros.";

	KEYWORDS:	Software Development, Assemblers, Cross-Assemblers,
			Intel, 8048;

	SYSTEM:		CP/M-80, CP/M-86, HP-UX, MSDOS, PCDOS, QNIX;
	COMPILERS:	Aztec C86, Aztec CII, CI-C86, Eco-C, Eco-C88, HP-UX,
			Lattice C, Microsoft C,	QNIX C;

	WARNINGS:	"This program has compiled successfully on 2 UNIX
			compilers, 5 MSDOS compilers, and 2 CP/M compilers.
			A port to Toolworks C is untried."

	AUTHORS:	William C. Colley III;
*/

/*
		      8048 Cross-Assembler in Portable C

		Copyright (c) 1985,1987 William C. Colley, III

Revision History:

Ver	Date		Description

0.0	APR 1987	Adapted from version 2.4 of my portable 1805A cross-
			assembler.  WCC3.

0.1	AUG 1988	Fixed a bug in the command line parser that puts it
			into a VERY long loop if the user types a command line
			like "A48 FILE.ASM -L".  WCC3 per Alex Cameron.

0.2	NOV 1988	Fixed a name conflict between a variable and a goto
			label that caused some compilers to choke.  WCC3.

This file contains the main program and line assembly routines for the
assembler.  The main program parses the command line, feeds the source lines
to the line assembly routine, and sends the results to the listing and object
file output routines.  It also coordinates the activities of everything.  The
line assembly routine uses the expression analyzer and the lexical analyzer to
parse the source line convert it into the object bytes that it represents.
*/

/*  Get global goodies:  */

#include "a48.h"

/*  Define global mailboxes for all modules:				*/

char errcode, line[MAXLINE + 1], title[MAXLINE];
int pass = 0;
int eject, filesp, forwd, listhex;
unsigned address, bytes, errors, listleft, obj[MAXLINE], pagelen, pc;
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

    printf("8048 Cross-Assembler (Portable) Ver 0.1\n");
    printf("Copyright (c) 1985,1987 William C. Colley, III\n\n");

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

/*  Line assembly routine.  This routine gets expressions and tokens	*/
/*  from the source file using the expression evaluator and lexical	*/
/*  analyzer, respectively.  It fills a buffer with the machine code	*/
/*  bytes and returns nothing.						*/

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

    trash(); opcod = NULL;
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
    SCRATCH unsigned op, minreg, maxreg;
    int grab_a(), grab_comma();
    unsigned expr();
    TOKEN *lex();
    void do_label(), error(), s_error(), unlex();
    static unsigned fixup_table[] = {	0x00, 0x5e, 0x7e, 0x70, 0x00,
					0x08, 0x09, 0x0a, 0x0b, 0x0c,
					0x0d, 0x0e, 0x0f, 0x00, 0x01,
					0x02, 0x39, 0x3a, 0x08, 0x09,
					0x0a, 0x0c, 0x0d, 0x0e, 0x0f,
					0x00, 0x00, 0x10, 0x00, 0x20,
					0x00, 0x00, 0x20, 0x30, 0x00,
					0x10				};

    do_label();  bytes = 1;
    maxreg = ((opcod -> attr & N_TAGS) - 1) +
	(minreg = (opcod -> attr & MIN_TAG) >> 4);

    switch (opcod -> attr & OPTYPE) {
	case MOV:	op = lex() -> valu;
			if ((token.attr & TYPE) != REG || grab_comma()) {
			    s_error();  break;
			}
			switch (op) {
			    case A:	if ((lex() -> attr & TYPE) == REG) {
					    switch (op = token.valu) {
						case PSW: obj[0] = 0xc7;
							  break;

						case T:   obj[0] = 0x42;
							  break;

						default:  if (op < minreg ||
							    op > maxreg)
							    s_error();
							  else
							    obj[0] = 0xf0 +
							      fixup_table[op];
							  break;
					    }
					}
					else if ((token.attr & TYPE) == IMM) {
					    obj[0] = 0x23;  goto do_immediate;
					}
					else s_error();
					break;

			    case PSW:	if (grab_a()) s_error();
					else obj[0] = 0xd7;
					break;

			    case T:	if (grab_a()) s_error();
					else obj[0] = 0x62;
					break;

			    default:	if (op < minreg || op > maxreg)
					    s_error();
					else if (grab_a()) {
					    if ((token.attr & TYPE) != IMM)
						s_error();
					    else {
						obj[0] = 0xb0 +
						    fixup_table[op];
						goto do_immediate;
					    }
					}
					else obj[0] = 0xa0 + fixup_table[op];
					break;
			}
			break;

	case LNG_JMP:	if ((op = expr()) > 0xfff) error('V');
			else if (opcod -> valu == 0x14 &&
			    (pc & 0x7fe) == 0x7fe) error('B');
			else {
			    obj[0] = opcod -> valu + ((op & 0x700) >> 3);
			    obj[1] = low(op);
			}
			bytes = 2;  break;

	case REG_CND:	if ((lex() -> attr & TYPE) != REG ||
			    (op = token.valu) < minreg || op > maxreg ||
			    grab_comma()) {
			    s_error();  break;
			}
			obj[0] = opcod -> valu + fixup_table[op];
			goto do_conditional;

	case JMP_CND:	obj[0] = opcod -> valu;
do_conditional:		if ((op = expr()) > 0xfff) error('V');
			else if (((pc + 1) ^ op) & 0x700) error('B');
			else obj[1] = low(op);
			bytes = 2;  break;

	case LOG_OP:	if ((lex() -> attr & TYPE) == REG &&
			    (op = token.valu) >= BUS && op <= P2) {
			    if (grab_comma() ||
				(lex() -> attr & TYPE) != IMM) {
				s_error();  break;
			    }
			    else {
				obj[0] = opcod -> valu + 0x40 +
				    fixup_table[op];
				goto do_immediate;
			    }
			}
			unlex();

	case ADD_OP:	if (grab_a() || grab_comma()) {
			    s_error();  break;
			}
			if ((lex() -> attr & TYPE) != IMM) {
			    unlex();  goto one_arg;
			}
			if (opcod -> valu == 0x60 || opcod -> valu == 0x70)
			    obj[0] = opcod -> valu - 0x5d;
			else obj[0] = opcod -> valu + 0x03;
do_immediate:		if ((op = expr()) > 0xff && op < 0xff80) error('V');
			else obj[1] = low(op);
			bytes = 2;  break;

	case A_AT_A:	if (grab_a() || grab_comma()) {
			    s_error();  break;
			}
			goto one_arg;

	case OUTPUT:	if ((lex() -> attr & TYPE) != REG ||
			    (op = token.valu) < minreg || op > maxreg ||
			    grab_comma() || grab_a()) {
			    s_error();  break;
			}
			if (!opcod -> valu) op -= 3;
			goto fixup;

	case A_REG_2:	if ((lex() -> attr & TYPE) == REG &&
			    (op = token.valu) >= minreg && op <= maxreg) {
			    if (grab_comma() || grab_a()) s_error();
			    else obj[0] = (opcod -> valu ? 0x90 : 0x30)
				+ fixup_table[op];
			    break;
			}
			unlex();

a_reg_1:
	case A_REG_1:	if (grab_a() || grab_comma()) {
			    s_error();  break;
			}
			goto one_arg;

	case INC_DEC:	if (!grab_a()) {
			    obj[0] = (opcod -> valu == 0x10 ? 0x17 : 0x07);
			    break;
			}
			unlex();

one_arg:
	case ONE_ARG:	if ((lex() -> attr & TYPE) != REG ||
			    (op = token.valu) < minreg || op > maxreg) {
			    error ('S');  break;
			}
fixup:			obj[0] = opcod -> valu + fixup_table[op];  break;

	case NO_ARG:	obj[0] = opcod -> valu;  break;
    }
    return;
}

static int grab_a()
{
    TOKEN *lex();

    return (lex() -> attr & TYPE) != REG || token.valu != A;
}

static int grab_comma()
{
    TOKEN *lex();

    return (lex() -> attr & TYPE) != SEP;
}

static void s_error()
{
    void error();

    bytes = 2;  error('S');
}

static void pseudo_op()
{
    SCRATCH char *s;
    SCRATCH unsigned *o, u;
    SCRATCH SYMBOL *l;
    int popc();
    unsigned expr();
    SYMBOL *find_symbol(), *new_symbol();
    TOKEN *lex();
    void do_label(), error(), fatal_error();
    void hseek(), pushc(), trash(), unlex();

    o = obj;
    switch (opcod -> valu) {
	case DB:    do_label();
		    do {
			if ((lex() -> attr & TYPE) == SEP) u = 0;
			else {
			    if (token.attr == STR) {
				trash();  pushc(u = popc());
				if (u == ',' || u == '\n') {
				    for (s = token.sval; *s; *o++ = *s++)
					++bytes;
				    lex();  continue;
				}
			    }
			    unlex();
			    if ((u = expr()) > 0xff && u < 0xff80) {
				u = 0;  error('V');
			    }
			}
			*o++ = low(u);  ++bytes;
		    } while ((token.attr & TYPE) == SEP);
		    break;

	case DS:    do_label();  u = word(pc + expr());
		    if (forwd) error('P');
		    else {
			pc = u;
			if (pass == 2) hseek(pc);
		    }
		    break;

	case DW:    do_label();
		    do {
			if ((lex() -> attr & TYPE) == SEP) u = 0;
			else { unlex();  u = expr(); }
			*o++ = low(u);  *o++ = high(u);
			bytes += 2;
		    } while ((token.attr & TYPE) == SEP);
		    break;

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

	case ENDIF: listhex = FALSE;
		    if (ifsp) off = ifstack[--ifsp] != ON;
		    else error('I');
		    break;

	case EQU:   if (label[0]) {
			if (pass == 1) {
			    if (!((l = new_symbol(label)) -> attr)) {
				l -> attr = FORWD + VAL;  address = expr();
				if (!forwd) l -> valu = address;
			    }
			}
			else {
			    if (l = find_symbol(label)) {
				l -> attr = VAL;  address = expr();
				if (forwd) error('P');
				if (l -> valu != address) error('M');
			    }
			    else error('P');
			}
		    }
		    else error('L');
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

	case REGI:  if (label[0]) {
			if ((lex() -> attr & TYPE) != REG || forwd ||
			    (u = token.valu) < R0 || u > R7) {
			    error('S');  u = R0;
			}
			if (pass == 1) {
			    if (!((l = new_symbol(label)) -> attr)) {
				l -> attr = FORWD + REG;
				address = (l -> valu = u) - R0;
			    }
			}
			else {
			    if (l = find_symbol(label)) {
				l -> attr = REG;  address = u - R0;
				if (forwd) error('P');
				if (l -> valu != u) error('M');
			    }
			    else error('P');
			}
		    }
		    else error('L');
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
  if (l = find_symbol(label)) {
				l -> attr = REG;  address = u - R0;
				if (forwd) error('P');
				if 