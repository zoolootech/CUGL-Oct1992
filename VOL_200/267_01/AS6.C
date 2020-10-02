/*
	HEADER:		CUG267;
	TITLE:		S6 Cross-Assembler (Portable);
	FILENAME:	AS6.C;
	VERSION:	0.2;
	DATE:		08/27/1988;
	SEE-ALSO:	AS6.H;
	AUTHORS:	William C. Colley III;
*/

/*
		       S6 Cross-Assembler in Portable C

		Copyright (c) 1986, 1987 William C. Colley, III

Revision History:

Ver	Date		Description

0.0	NOV 1987	Derived from my 6502 cross-assembler.  WCC3.

0.1	JUNE 1988	Fixed glitches in the relative jumps and in the bit
			operations to make the assembler conform to the
			machine rather than to the machine's documentation.
			WCC3.

0.2	AUG 1988	Fixed a bug in the command line parser that puts it
			into a VERY long loop if the user types a command line
			like "AS6 FILE.ASM -L".  WCC3 per Alex Cameron.

This file contains the main program and line assembly routines for the
assembler.  The main program parses the command line, feeds the source lines
to the line assembly routine, and sends the results to the listing and object
file output routines.  It also coordinates the activities of everything.  The
line assembly routines uses the expression analyzer and the lexical analyzer
to parse the source line and convert it into the object bytes that it
represents.
*/

/*  Get global goodies:  */

#include "as6.h"

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

    printf("S6 Cross-Assembler (Portable) Ver 0.2\n");
    printf("Copyright (c) 1986, 1987 William C. Colley, III\n\n");

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
    SCRATCH unsigned opcode;
    SCRATCH unsigned op1, op2;

    static unsigned reverse3[] = {
	0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0
    };

    unsigned get_arg();
    void do_direct(), do_immediate(), do_label(), error();

    bytes = opcod -> attr & BYTES;  opcode = opcod -> valu;
    op1 = op2 = 0;  do_label();
    switch (opcod -> attr & OPTYPE) {
	case LDI:	if (get_arg(0) & IS_A) {
			    opcode = 0x17;  bytes = 2;
			    get_arg(1);  do_immediate(&op1);
			}
			else {
			    do_direct(&op1);  get_arg(1);  do_immediate(&op2);
			}
			break;

	case ADDI_OP:	if (!(get_arg(0) & IS_A)) error('S');
			else { get_arg(1);  do_immediate(&op1); }
			break;

	case JR_SIGN:	op1 = 0xff;  goto do_rel8;

	case JR_BIT:	get_arg(0);
			if (token.valu > 7) error('V');
			else opcode |= reverse3[token.valu];
			get_arg(0);  do_direct(&op1);
do_rel8:		get_arg(1);
			if ((op2 = token.valu - (pc + 3)) > 0x7f &&
			    op2 < 0xff80) {
			    error('B');  op2 = 0;
			}
			break;

	case JR:	get_arg(1);
			if ((op1 = token.valu - (pc+ 1)) > 0x0f &&
			    op1 < 0xfff0) error('B');
			else opcode |= (op1 << 3) & 0xf8;
			break;

	case JP:	get_arg(1);
			if ((op1 = token.valu) > 0x0fff) {
			    error('V');  op1 = 0;
			}
			opcode |= (op1 << 4) & 0x00f0;  op1 >>= 4;
			break;

	case LD:	if (get_arg(0) & IS_A) goto do_inc;
			if (token.attr & IS_XYVW) {
			    opcode = 0x3d + ((token.valu & 0x03) << 6);
			    bytes = 1;
			}
			else if (token.attr & IS_IND) {
			    opcode = (token.valu == 0x80) ? 0x87 : 0x8f;
			    bytes = 1;
			}
			else { opcode = 0x9f;  do_direct(&op1); }
			if (!(get_arg(1) & IS_A)) error('S');
			break;

	case CLR:	if (get_arg(1) & IS_A) { opcode = 0xdf;  op1 = 0xff; }
			else { bytes = 3;  do_direct(&op1); }
			break;

	case BIT_OP:	get_arg(0);
			if (token.valu > 7) error('V');
			else opcode |= reverse3[token.valu];
			get_arg(1);  do_direct(&op1);
			break;

do_inc:	case INC_DEC:	if (get_arg(1) & IS_XYVW) {
			    opcode = (opcode >> 8) +
				((token.valu & 0x03) << 6);
			    bytes = 1;  break;
			}
			goto do_add;

	case ADD_OP:	if (!(get_arg(0) & IS_A)) { error('S');  break; }
			get_arg(1);
do_add:			if (token.attr & IS_IND) {
			    opcode -= (token.valu == 0x80) ? 0x18 : 0x10;
			    bytes = 1;
			}
			else do_direct(&op1);
			break;

	case SHIFT_OP:	if (!(get_arg(1) & IS_A)) error('S');

	case NO_ARG:	op1 = opcode >> 8;
			break;
    }
    if (bytes > 2) obj[2] = low(op2);
    if (bytes > 1) obj[1] = low(op1);
    obj[0] = low(opcode);  return;
}

static void do_direct(buf)
unsigned *buf;
{
    void error();

    if ((*buf = token.valu) > 0x00ff) { error('V');  *buf = 0; }
    return;
}

static void do_immediate(buf)
unsigned *buf;
{
    void error();

    if ((*buf = token.valu) > 0x00ff && *buf < 0xff80) {
	error('V');  *buf = 0;
    }
    return;
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
    void do_label(), error(), fatal_error(), hseek();
    void pushc(), trash(), unlex();

    o = obj;
    switch (opcod -> valu) {
	case DB:    do_label();
		    do {
			if ((lex() -> attr & TYPE) == SEP) {
			    *o++ = 0;  ++bytes;
			}
			else if (token.attr == STR) {
			    trash();  pushc(u = popc());
			    if (u != ',' && u != '\n') goto do_byte;
			    for (s = token.sval; *s; *o++ = *s++) ++bytes;
			    lex();
			}
			else {
do_byte:		    unlex();
			    if ((u = expr()) > 0xff && u < 0xff80) {
				u = 0;  error('V');
			    }
			    *o++ = low(u);  ++bytes;
			}
		    } while ((token.attr & TYPE) == SEP);
		    break;

	case DS:    do_label();
		    u = word(pc + expr());
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
			*o++ = high(u);  *o++ = low(u);
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

	case TITLE: listhex = FALSE;  do_label();
		    if ((lex() -> attr & TYPE) == EOL) title[0] = '\0';
		    else if ((token.attr & TYPE) != STR) error('S');
		    else strcpy(title,token.sval);
		    break;

	case VAR:   if (label[0]) {
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
    }
    return;
}
