/*
	HEADER:		CUG267;
	TITLE:		2650 Cross-Assembler (Portable);
	FILENAME:	A26.C;
	VERSION:	0.1;
	DATE:		08/27/1988;
	SEE-ALSO:	A26.H;
	AUTHORS:	William C. Colley III;
*/

/*
		      2650 Cross-Assembler in Portable C

		Copyright (c) 1985,1987 William C. Colley, III

Revision History:

Ver	Date		Description

0.0	AUG 1987	Derived from version 3.4 of my portable 6800/6801
			cross-assembler.  WCC3.

0.1	AUG 1988	Fixed a bug in the command line parser that puts it
			into a VERY long loop if the user types a command line
			like "A26 FILE.ASM -L".  WCC3 per Alex Cameron.

This file contains the main program and line assembly routines for the
assembler.  The main program parses the command line, feeds the source lines
to the line assembly routine, and sends the results to the listing and object
file output routines.  It also coordinates the activities of everything.  The
line assembly routines uses the expression analyzer and the lexical analyzer
to parse the source line and convert it into the object bytes that it
represents.
*/

/*  Get global goodies:  */

#include "a26.h"

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

    printf("2650 Cross-Assembler (Portable) Ver 0.1\n");
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
	    pc = pc + bytes & 0x7fff;
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

#define	diff_page(a1,a2)	(((a1) ^ (a2)) & 0xe000)

static void normal_op()
{
    SCRATCH unsigned attrib, from, to;
    unsigned expr();
    TOKEN *lex();
    void do_label(), error(), unlex();

    do_label();  bytes = (attrib = opcod -> attr) & BYTES;
    if (pass < 2) return;
    obj[0] = opcod -> valu;  obj[1] = obj[2] = 0;
    if (diff_page(pc,pc + bytes)) error('F');

    if (attrib & COMMA)
	if ((lex() -> attr & TYPE) != SEP) { error('S');  return; }

    if (attrib & (REG + CC)) {
	if ((lex() -> attr & TYPE) != VAL) { error('S');  return; }
	switch (token.valu) {
	    case 3:	if (attrib & NOT_CC3) { error('C');  break; }

	    case 2:
	    case 1:	if ((attrib & (REG + INDEX)) == (REG + INDEX))
			    attrib &= ~INDEX;
			obj[0] |= token.valu;  break;

	    case 0:	if (attrib & ODD_R0) obj[0] = 0x60;
			if (attrib & NOT_R0) error('R');
			break;

	    default:	error(attrib & REG ? 'R' : 'C');  break;
	}
    }

    if (attrib & INDIR) {
	if ((lex() -> attr & TYPE) != OPR || token.valu != '*') unlex();
	else obj[1] |= 0x80;
    }

    switch (attrib & V_TYPE) {
	case ABS_15:	if ((to = expr()) & 0x8000) error('A');
			else {
			    obj[1] |= (to >> 8) & 0x7f;  obj[2] = low(to);
			}
			break;

	case ABS_13:	if (diff_page(pc,to = expr())) error('A');
			else {
			    obj[1] |= (to >> 8) & 0x1f;  obj[2] = low(to);
			}
			break;

	case SIGN_8:	if ((to = expr()) > 0xff && to < 0xff80) error('V');
			else obj[1] = low(to);
			break;

	case UNSGN_8:	if ((to = expr()) > 0xff) error('V');
			else obj[1] = low(to);
			break;

	case REL_7:	from = pc;  goto do_rel;

	case ZREL_7:	from = 0x1ffe;
do_rel:			if (diff_page(from,to = expr()) ||
			    ((to = to - (from + 2) & 0x1fff) > 0x3f &&
			    to < 0x1fc0)) {
			    error('A');  obj[1] |= 0x7e;
			}
			else obj[1] |= to & 0x7f;

	case NONE:	break;
    }

    if (attrib & INDEX) {
	if ((lex() -> attr & TYPE) != SEP) unlex();
	else if ((lex() -> attr & TYPE) == VAL) {
		obj[1] |= 0x60;
		if (token.valu > 3) error('R');
		else obj[0] |= token.valu;
		if ((lex() -> attr & TYPE) != SEP) unlex();
		else if ((lex() -> attr & TYPE) != OPR) error('S');
		else switch (token.valu) {
		    case '+':	obj[1] &= 0xbf;  break;

		    case '-':	obj[1] &= 0xdf;  break;

		    default:	error('S');  break;
		}
	}
	else error('S');
    }

    if (attrib & ONLY_R3)
	if ((lex() -> attr & TYPE) != SEP || (lex() -> attr & TYPE) != VAL ||
	    token.valu != 3) error('S');
}

static void pseudo_op()
{
    SCRATCH char *s;
    SCRATCH int c;
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
	case ACON:  do_label();
		    do {
			lex();  unlex();
			u = ((token.attr & TYPE) == SEP) ? 0 : expr();
			if (u > 0x7fff) { error('V');  u &= 0x7fff; }
			*o++ = high(u);  *o++ = low(u);  bytes += 2;
		    } while ((lex() -> attr & TYPE) == SEP);
		    break;

	case DATA:  do_label();
		    do {
			switch (lex() -> attr & TYPE) {
			    case SEP:	unlex();  u = 0;  goto save_byte;

			    case STR:	trash();  pushc(c = popc());
					if (c == ',' || c == '\n') {
					    for (s = token.sval; *s;
						*o++ = *s++) ++bytes;
					    break;
					}

			    default:	unlex();
					if ((u = expr()) > 0xff &&
					    u < 0xff80) {
					    u = 0;  error('V');
					}
save_byte:				*o++ = low(u);  ++bytes;  break;
			}
		    } while ((lex() -> attr & TYPE) == SEP);
		    break;

	case EJE:   listhex = FALSE;  do_label();
		    if ((lex() -> attr & TYPE) != EOL) {
			unlex();
			if ((pagelen = expr()) > 0 && pagelen < 3) {
			    pagelen = 0;  error('V');
			}
		    }
		    eject = TRUE;
		    break;

	case ELSE:  listhex = FALSE;
		    if (ifsp) off = (ifstack[ifsp] = -ifstack[ifsp]) != ON;
		    else error('I');
		    break;

	case END:   do_label();
		    if (filesp) { listhex = FALSE;  error('*'); }
		    else {
			done = eject = TRUE;
			if (pass == 2) {
			    if ((lex() -> attr & TYPE) != EOL) {
				unlex();  hseek(address = expr());
			    }
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

	case RES:   do_label();
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
