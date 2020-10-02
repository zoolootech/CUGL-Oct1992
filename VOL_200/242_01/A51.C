/*
	HEADER:		CUG242;
	TITLE:		8051 Cross-Assembler (Portable);
	FILENAME:	A51.C;
	VERSION:	0.4;
	DATE:		11/09/1988;
	SEE-ALSO:	A51.H;
	AUTHORS:	William C. Colley III;
*/

/*
		      8051 Cross-Assembler in Portable C

		Copyright (c) 1985,1987 William C. Colley, III

Revision History:

Ver	Date		Description

0.0	JUL 1987	Adapted from version 0.0 of my portable 8048 cross-
			assembler.  WCC3.

0.1	OCT 1987	Changed the order of DW constants and made trailing
			colons on labels get trashed.  WCC3.

0.2	AUG 1988	Fixed bug that swapped the machine codes for CPL A and
			CLR A.  WCC3.

0.3	AUG 1988	Fixed a bug in the command line parser that puts it
			into a VERY long loop if the user types a command line
			like "A51 FILE.ASM -L".  WCC3 per Alex Cameron.

0.4	NOV 1988	Fixed a bug that made DJNZ direct,relative generate
			the wrong opcode.  WCC3.

This file contains the main program and line assembly routines for the
assembler.  The main program parses the command line, feeds the source lines
to the line assembly routine, and sends the results to the listing and object
file output routines.  It also coordinates the activities of everything.  The
line assembly routine uses the expression analyzer and the lexical analyzer to
parse the source line convert it into the object bytes that it represents.
*/

/*  Get global goodies:  */

#include "a51.h"

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

    printf("8051 Cross-Assembler (Portable) Ver 0.4\n");
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
    SCRATCH char *p;
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
	    for (p = label; *p; ++p);
	    if (*--p == ':') *p = '\0';
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
    SCRATCH unsigned *o, rmax, rmin, u;
    int grab_a(), grab_comma(), grab_reg();
    unsigned bit_expr(), dir_expr(), expr(), imm_expr();
    TOKEN *lex();
    void do_label(), error(), pushc(), s_error(), unlex();

    do_label();  o = obj;  *o = opcod -> valu;
    rmax = (rmin = opcod -> attr & RARG) + ((opcod -> attr & RARGS) >> 5);
    switch (opcod -> attr & OTYPE) {
	case MOV:	if (!grab_reg()) {
			    u = token.valu;
			    if (grab_comma()) { s_error();  return; }
			    switch (u) {
				case A:	    if ((lex() -> attr & TYPE)
						== IMM) {
						*o = 0x74;  *++o = imm_expr();
						break;
					    }
					    *o = 0xe4;  unlex();  goto dec;
						    
				case C:	    *o = 0xa2;  *++o = bit_expr();
					    break;

				case DPTR:  if ((lex() -> attr & TYPE)
						== IMM) {
						*o = 0x90;  goto ljmp;
					    }
					    s_error();  return;

				default:    if (u >= AT_R0 && u <= R7) {
						if (!grab_a()) *o = 0xf4 + u;
						else if ((token.attr & TYPE)
						    == IMM) {
						    *o = 0x74 + u;
						    *++o = imm_expr();
						}
						else {
						    *o = 0xa4 + u;  unlex();
						    *++o = dir_expr();
						}
						break;
					    }
					    s_error();  return;
			    }
			}
			else if ((token.attr & TYPE) == BVAL) {
			    unlex();  u = bit_expr();
move_bit:		    if ((token.attr & TYPE) != SEP || grab_reg() ||
				token.valu != C) {
				s_error();  return;
			    }
			    *o = 0x92;  *++o = low(u);
			}
			else{
			    unlex();  u = dir_expr();
			    if ((token.attr & TYPE) == DOT) {
				token.attr = VAL;  token.valu = u;  unlex();
				pushc('.');  u = bit_expr();  goto move_bit;
			    }
			    if ((token.attr & TYPE) != SEP) {
				s_error();  return;
			    }
			    if (!grab_a()) { *o = 0xf5, *++o = u; }
			    else if ((token.attr & TYPE) == IMM) {
				*o = 0x75;  *++o = u;  *++o = imm_expr();
			    }
			    else if ((token.attr & TYPE) != REG) {
				unlex();  *o++ = 0x85;  *++o = u;
				o[-1] = dir_expr();
			    }
			    else if (token.valu >= AT_R0 &&
				token.valu <= R7) {
				*o = 0x84 + token.valu;  *++o = u;
			    }
			    else if (token.valu == C) {
				*o = 0x92;  *++o = u;
			    }
			    else { s_error();  return; }
			}
			break;

	case MOVX:	if (grab_reg()) { s_error();  return; }
			switch (token.valu) {
			    case A:	    if (!grab_comma() &&
						!grab_reg()) {
						if (token.valu == AT_DPTR)
						    break;
						unlex();  goto swap;
					    }
					    s_error();  return;

			    case AT_R0:
			    case AT_R1:	    *o += token.valu;

			    case AT_DPTR:   *o += 0x10;
					    if (!grab_comma() && !grab_a())
						break;

			    default:	    s_error();  return;
			}
			break;

	case ORL:	if (grab_reg() || token.valu != C) {
			    unlex();  goto xrl;
			}
			if (grab_comma()) { s_error();  return; }
			*o += 0x2e;
			if ((lex() -> attr & TYPE) == OPR &&
			     token.valu == '/') *o += 0x2e;
			else unlex();
			*++o = bit_expr();  break;

xrl:	case XRL:	if (!grab_a()) goto do_add;
			unlex(); --*o;
			*++o = dir_expr();
			if ((token.attr & TYPE) == SEP) {
			    if (!grab_a()) { --obj[0];  break; }
			    if ((token.attr & TYPE) == IMM) {
				*++o = imm_expr();  break;
			    }
			}
			s_error();  return;

	case ADD:	if (grab_a()) { s_error();  return; }
do_add:			if (grab_comma()) { s_error();  return; }
			switch (lex() -> attr & TYPE) {
			    case IMM:	*++o = imm_expr();  break;
					
			    case REG:	unlex();  goto swap;
					
			    default:	++*o;  unlex();
					*++o = dir_expr();  break;
			}
			break;

	case DJNZ:	if (!grab_reg()) {
			    if ((u = token.valu) >= R0 && u <= R7 &&
				!grab_comma()) {
				*o += u;  goto sjmp;
			    }
			}
			else {
			    unlex();  ++*o;  *++o = dir_expr();
			    if ((token.attr & TYPE) == SEP) goto sjmp;
			}
			s_error();  return;

	case CJNE:	if (!grab_reg() && (u = token.valu) <= R7 &&
			    !grab_comma()) {
			    if ((lex() -> attr & TYPE) != IMM) {
				if (u != A) { s_error();  return; }
				++*o;  unlex();  *++o = dir_expr();
			    }
			    else { *o += u;  *++o = imm_expr(); }
			    if ((token.attr & TYPE) == SEP) goto sjmp;
			}
			s_error();  return;

	case XCH:	if (grab_a() || grab_comma()) { s_error();  return; }
			goto dec;

	case INC:	if (!grab_reg() && token.valu == DPTR) {
			    *o = 0xa3;  break;
			}
			unlex();
			
dec:	case DEC:	if (!grab_reg()) { unlex();  goto swap; }
			unlex();  ++*o;

	case POP:	*++o = dir_expr();  break;

	case JBIT:	*++o = bit_expr();
			if ((token.attr & TYPE) != SEP) {
			    s_error();  return;
			}

sjmp:	case SJMP:	if ((u = word(expr() - (pc + (o != obj ? 3 : 2))))
			    > 0x7f && u < 0xff80) {
			    error('B');  u = (o != obj ? 0xfd : 0xfe);
			}
			*++o = low(u);  break;

	case AJMP:	if (((u = expr()) ^ (pc + 2)) & 0xf800) {
			    error('B');  u = 0;
			}
			*o += (u >> 3) & 0xe0;  *++o = low(u);  break;

ljmp:	case LJMP:	*++o = high(u = expr());  *++o = low(u);  break;
	
	case CPL:	if (!grab_a()) {
			    *o = (*o == 0xb3 ? 0xf4 : 0xe4);  break;
			}
			unlex();

	case SETB:	if (!grab_reg()) {
			    if (token.valu != C) { s_error();  return; }
			}
			else { --*o;  unlex();  *++o = bit_expr(); }
			break;

	case MOVC:	if (grab_a() || grab_comma()) { s_error();  return; }

	case JMP:	if (grab_reg() || token.valu != AT_A ||
			    (lex() -> attr & TYPE) != OPR ||
			    token.valu != '+' || grab_reg() ||
			    token.valu < rmin || token.valu > rmax) {
			    s_error();  return;
			}
			if (token.valu == PC) *o -= 0x10;
			break;

	case XCHD:	if (grab_a() || grab_comma()) { s_error();  return; }

swap:	case SWAP:	if (grab_reg() || token.valu < rmin ||
			    token.valu > rmax) {
			    s_error();  return;
			}
			*o += token.valu;  break;

	case RET:	break;
    }
    bytes = o - obj + 1;  return;
}

static unsigned dir_expr()
{
    unsigned u, expr();
    void error();
    
    if ((u = expr()) > 0xff) { error('V');  return 0; }
    return u;
}

static unsigned imm_expr()
{
    unsigned u, expr();
    void error();
    
    if ((u = expr()) > 0xff && u < 0xff80) { error('V');  return 0; }
    return low(u);
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

static int grab_reg()
{
    TOKEN *lex();

    return (lex() -> attr & TYPE) != REG;
}

static void s_error()
{
    unsigned *o;
    void error();

    bytes = BIGINST;  error('S');
    for (o = obj; o < obj + BIGINST; *o++ = NOP);
}

static void pseudo_op()
{
    SCRATCH char *s;
    SCRATCH unsigned *o, u;
    SCRATCH SYMBOL *l;
    int popc();
    unsigned bit_expr(), expr();
    SYMBOL *find_symbol(), *new_symbol();
    TOKEN *lex();
    void do_label(), error(), fatal_error();
    void hseek(), pushc(), trash(), unlex();

    o = obj;
    switch (opcod -> valu) {
	case BIT:   if (label[0]) {
			if (pass == 1) {
			    if (!((l = new_symbol(label)) -> attr)) {
				l -> attr = FORWD + BVAL;
				address = l -> valu = bit_expr();
			    }
			}
			else {
			    if (l = find_symbol(label)) {
				l -> attr = BVAL;  address = bit_expr();
				if (l -> valu != address) error('M');
			    }
			    else error('P');
			}
		    }
		    else error('L');
		    break;

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
  if 