/*
	HEADER:		CUG242;
	TITLE:		68HC11 Cross-Assembler (Portable);
	FILENAME:	A611.C;
	VERSION:	0.1;
	DATE:		08/27/1988;
	SEE-ALSO:	A611.H
	AUTHORS:	William C. Colley III;
*/

/*
		     68HC11 Cross-Assembler in Portable C

		Copyright (c) 1985,1987 William C. Colley, III

This file contains the main program and line assembly routines for the
assembler.  The main program parses the command line, feeds the source lines
to the line assembly routine, and sends the results to the listing and object
file output routines.  It also coordinates the activities of everything.  The
line assembly routines uses the expression analyzer and the lexical analyzer
to parse the source line and convert it into the object bytes that it
represents.
*/

/*  Get global goodies:  */

#include "a611.h"

/*  Define global mailboxes for all modules:				*/

char errcode, line[MAXLINE + 1], title[MAXLINE];
int pass = 0;
int eject, filesp, forwd, listhex;
unsigned  address, bytes, errors, listleft, obj[MAXLINE], pagelen, pc;
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

    printf("68HC11 Cross-Assembler (Portable) Ver 0.1\n");
    printf("Copyright (c) 1985, 1987 William C. Colley, III\n\n");

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
    SCRATCH unsigned attrib, operand, *o;
    static unsigned prebyte[2][4] = {
	{ 0x00, 0x00, 0x18, 0x1a },
	{ 0x18, 0xcd, 0x1a, 0xcd }
    };
    unsigned expr();
    TOKEN *lex();
    void do_label(), error(), s_error(), unlex();

    do_label();

    o = obj;
    if (operand = prebyte[0][(attrib = opcod -> attr) & PREBYTE])
	*o++ = operand;
    *o++ = opcod -> valu;
    while (attrib & ~PREBYTE) {
	switch (lex() -> attr & TYPE) {
	    case IMM:	if (!(attrib & IMMED)) { s_error();  return; }
		        operand = expr();
			if (attrib & IMM_16) *o++ = high(operand);
			else if (operand > 0xff && operand < 0xff80) {
			    error('V');  operand = 0;
			}
			*o++ = low(operand);  attrib = 0;  break;

	    case REG:	operand = 0;
do_indx:		if (!(attrib & INDX)) { s_error();  return; }
			if (token.valu == (*obj == 0x18 ? 'X' : 'Y')) {
			    if ((attrib & PREBYTE) < 2) *o++ = *obj;
			    *obj = prebyte[1][attrib & PREBYTE];
			}
			switch (*--o) {
			    case 0x12:
			    case 0x13:	*o++ += 0x0c;  break;

			    case 0x14:
			    case 0x15:	*o++ += 0x08;  break;
			
			    default:	*o++ += 0x20;  break;
			}
			if (operand > 0xff && operand < 0xff80) {
			    error('V');  operand = 0;
			}
			*o++ = low(operand);
			if ((attrib &= REL + MASK) & (REL + MASK)) {
			    if ((lex() -> attr & TYPE) != SEP) {
				s_error();  return;
			    }
			}
			break;

	    default:	unlex();  operand = expr();
			if ((token.attr & TYPE) == SEP) {
			    if ((lex() -> attr & TYPE) == REG) goto do_indx;
			    unlex();
			}
			switch (attrib & (DIR + EXT)) {
			    case DIR + EXT: if (!forwd && operand <= 0xff)
						goto do_dir;
					
			    case EXT:	    o[-1] += 0x30;
					    *o++ = high(operand);
					    *o++ = low(operand);
					    attrib = 0;
					    break;

			    case DIR:	    if (operand > 0xff) {
						error('V');  operand = 0;
					    }
do_dir:					    if (o[-1] >= 0x40) o[-1] += 0x10;
					    *o++ = low(operand);
					    attrib &= REL + MASK;
					    break;

			    default:	    if (attrib & MASK) {
						if (operand > 0xff) {
						    error('V');  operand = 0;
						}
						*o++ = low(operand);
						attrib &= REL;
					    }
					    else {
						if ((operand = word(operand -
						    (pc + (o + 1 - obj))))
						    > 0x7f &&
						    operand < 0xff80) {
						    error('B');
						    operand = obj - (o + 1);
						}
						*o++ = low(operand);
						attrib = 0;
					    }
					    break;
			}
	}
    }
    bytes = o - obj;  return;
}

void s_error()
{
    SCRATCH unsigned *o;

    error('S');  bytes = BIGINST;
    for (o = obj; o < obj + BIGINST; *o++ = NOP);
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
			*o++ = high(u);  *o++ = low(u);
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

	case TITLE: listhex = FALSE;  do_label();
		    if ((lex() -> attr & TYPE) == EOL) title[0] = '\0';
		    else if ((token.attr & TYPE) != STR) error('S');
		    else strcpy(title,token.sval);
		    break;
    }
    return;
}
