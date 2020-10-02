/*
	HEADER:		CUG219;
	TITLE:		65C02 Cross-Assembler (Portable);
	FILENAME:	A65CEVAL.H;
	VERSION:	0.1;
	DATE:		08/27/1988;

	DESCRIPTION:	"This program lets you use your computer to assemble
			code for the Rockwell 65C02 microprocessors.  The
			program is written in portable C rather than BDS C.
			All assembler features are supported except relocation
			linkage, and macros.";

	KEYWORDS:	Software Development, Assemblers, Cross-Assemblers,
			Rockwell, 65C02;

	SYSTEM:		CP/M-80, CP/M-86, HP-UX, MSDOS, PCDOS, QNIX;
	COMPILERS:	Aztec C86, Aztec CII, CI-C86, Eco-C, Eco-C88, HP-UX,
			Lattice C, Microsoft C,	QNIX C;

	WARNINGS:	"This program is written in as portable C as possible.
			A port to BDS C would be extremely difficult, but see
			volume CUG113.  A port to Toolworks C is untried."

	AUTHORS:	William C. Colley III;
*/

/*
		      65C02 Cross-Assembler in Portable C

		   Copyright (c) 1986 William C. Colley, III

Revision History:

Ver	Date		Description

0.0	NOV 1986	Derived from my 6800/6801 cross-assembler.  WCC3.

0.1	AUG 1988	Fixed a bug in the command line parser that puts it
			into a VERY long loop if the user types a command line
			like "A65C FILE.ASM -L".  WCC3 per Alex Cameron.

This file contains the assembler's expression evaluator and lexical analyzer.
The lexical analyzer chops the input character stream up into discrete tokens
that are processed by the expression analyzer and the line assembler.  The
expression analyzer processes the token stream into unsigned results of
arithmetic expressions.
*/

/*  Get global goodies:  */

#include "a65c.h"

/*  Get access to global mailboxes defined in A65C.C:			*/

extern char line[];
extern int filesp, forwd, pass;
extern unsigned argattr, pc;
extern FILE *filestk[], *source;
extern TOKEN token;

/*  Machine opcode argument field parsing routine.  The token stream	*/
/*  from the lexical analyzer is processed to extract addressing mode	*/
/*  information and (possibly) an actual address that can be reduced to */
/*  an unsigned value.  If an error occurs during the evaluation, the	*/
/*  global flag forwd is set to indicate to the line assembler that it	*/
/*  should not base certain decisions on the result of the evaluation.	*/
/*  The address is passed back as the return value of the function.	*/
/*  The addressing mode information is passed back through the global	*/
/*  mailbox argattr.							*/

static int bad;

unsigned do_args()
{
    SCRATCH int c;
    SCRATCH unsigned u;
    TOKEN *lex();
    int popc();
    unsigned eval(), expr();
    void exp_error(), pushc(), trash(), unlex();

    argattr = ARGNUM;  u = 0;  bad = FALSE;
    switch (lex() -> attr & TYPE) {
	case REG:   if (token.valu == 'A') argattr = ARGA;
		    else exp_error('S');
		    break;

	case EOL:   argattr = NULL;  break;

	case IMM:   argattr = ARGIMM + ARGNUM;
		    return expr();

	case SEP:   u = 0;  goto have_number;

	case OPR:   if (token.valu == '(') {
			bad = FALSE;  u = eval(START2);
			switch (lex() -> attr & TYPE) {
			    case EOL:	exp_error('(');  return 0;

			    case SEP:	if ((lex() -> attr & TYPE) != REG ||
					    token.valu != 'X') exp_error('S');
					else if ((lex() -> attr & TYPE) != OPR
					    || token.valu != ')')
					    exp_error('(');
					else argattr += (ARGX + ARGIND);
					return bad ? 0 : u;

			    case OPR:	argattr = (ARGIND + ARGNUM);  trash();
					if ((c = popc()) == '\n') return u;
					if (c == ',') {
					    if (lex() -> attr & TYPE != REG
						|| token.valu != 'Y')
						exp_error('S');
					    else argattr += ARGY;
					    return bad ? 0 : u;
					}
					argattr = ARGNUM;  pushc(c);
					token.attr = VAL;  token.valu = u;
			}
		    }

	case VAL:
	case STR:   unlex();  u = eval(START);
		    if ((token.attr & TYPE) != SEP) {
			if ((token.attr & TYPE) != EOL) exp_error('S');
			break;
		    }

have_number:	    if ((lex() -> attr & TYPE) == REG) {
			switch (token.valu) {
			    case 'A':	exp_error('S');  break;

			    case 'X':	argattr += ARGX;  break;

			    case 'Y':	argattr += ARGY;  break;
			    }
		    }
		    else {
			if ((token.attr & TYPE) == EOL) exp_error('S');
		        unlex();
		    }
		    break;
    }
    return bad ? 0 : u;
}

/*  Expression analysis routine.  The token stream from the lexical	*/
/*  analyzer is processed as an arithmetic expression and reduced to an	*/
/*  unsigned value.  If an error occurs during the evaluation, the	*/
/*  global flag	forwd is set to indicate to the line assembler that it	*/
/*  should not base certain decisions on the result of the evaluation.	*/

unsigned expr()
{
    SCRATCH unsigned u;
    unsigned eval();

    bad = FALSE;
    u = eval(START);
    return bad ? 0 : u;
}

static unsigned eval(pre)
unsigned pre;
{
   register unsigned op, u, v;
   TOKEN *lex();
   void exp_error(), unlex();

   for (;;) {
      u = op = lex() -> valu;
      switch (token.attr & TYPE) {
	 case REG:
	 case IMM:   exp_error('S');  break;

	 case SEP:   if (pre != START) unlex();
	 case EOL:   exp_error('E');  return;

	 case OPR:   if (!(token.attr & UNARY)) { exp_error('E');  break; }
		     u = (op == '*' ? pc :
			eval((op == '+' || op == '-') ?
			   (unsigned) UOP1 : token.attr & PREC));
		     switch (op) {
			case '-':   u = word(-u);  break;

			case NOT:   u ^= 0xffff;  break;

			case HIGH:  u = high(u);  break;

			case LOW:   u = low(u);  break;
		     }

	 case VAL:	
	 case STR:   for (;;) {
			op = lex() -> valu;
			switch (token.attr & TYPE) {
			   case REG:
			   case IMM:   exp_error('S');  break;

			   case SEP:   if (pre != START) unlex();
			   case EOL:   if (pre == LPREN) exp_error('(');
				       return u;

			   case STR:
			   case VAL:   exp_error('E');  break;

			   case OPR:   if (!(token.attr & BINARY)) {
					  exp_error('E');  break;
				       }
				       if ((token.attr & PREC) >= pre) {
					  unlex();  return u;
				       }
				       if (op != ')')
					  v = eval(token.attr & PREC);
				       switch (op) {
					  case '+':   u += v;  break;

					  case '-':   u -= v;  break;

					  case '*':   u *= v;  break;

					  case '/':   u /= v;  break;

					  case MOD:   u %= v;  break;

					  case AND:   u &= v;  break;

					  case OR:    u |= v;  break;

					  case XOR:   u ^= v;  break;

					  case '<':   u = u < v;  break;

					  case LE:    u = u <= v;  break;

					  case '=':   u = u == v;  break;

					  case GE:    u = u >= v;  break;

					  case '>':   u = u > v;  break;

					  case NE:    u = u != v;  break;

					  case SHL:   if (v > 15)
							 exp_error('E');
						      else u <<= v;
						      break;

					  case SHR:   if (v > 15)
							 exp_error('E');
						      else u >>= v;
						      break;

					  case ')':   if (pre == LPREN)
							 return u;
						      exp_error('(');
						      break;
				       }
				       clamp(u);
				       break;
			}
		     }
		     break;
      }
   }
}

static void exp_error(c)
char c;
{
    forwd = bad = TRUE;  error(c);
}

/*  Lexical analyzer.  The source input character stream is chopped up	*/
/*  into its component parts and the pieces are evaluated.  Symbols are	*/
/*  looked up, operators are looked up, etc.  Everything gets reduced	*/
/*  to an attribute word, a numeric value, and (possibly) a string	*/
/*  value.								*/

static int oldt = FALSE;
static int quote = FALSE;

TOKEN *lex()
{
    SCRATCH char c, *p;
    SCRATCH unsigned b;
    SCRATCH OPCODE *o;
    SCRATCH SYMBOL *s;
    OPCODE *find_operator();
    SYMBOL *find_symbol();
    void exp_error(), make_number(), pops(), pushc(), trash();

    if (oldt) { oldt = FALSE;  return &token; }
    trash();
    if (isalph(c = popc())) {
	pushc(c);  pops(token.sval);
	if (o = find_operator(token.sval)) {
	    token.attr = o -> attr;
	    token.valu = o -> valu;
	}
	else {
	    token.attr = VAL;  token.valu = 0;
	    if (s = find_symbol(token.sval)) {
		token.valu = s -> valu;
		if (pass == 2 && s -> attr & FORWD) forwd = TRUE;
	    }
	    else exp_error('U');
	}
    }
    else if (isnum(c)) {
	pushc(c);  pops(token.sval);
	for (p = token.sval; *p; ++p);
	switch (toupper(*--p)) {
	    case 'B':	b = 2;  break;

	    case 'O':
	    case 'Q':	b = 8;  break;

	    default:	++p;
	    case 'D':	b = 10;  break;

	    case 'H':	b = 16;  break;
	}
	*p = '\0';  make_number(b);
    }
    else switch (c) {
	case '%':   b = 2;  goto num;

	case '@':   b = 8;  goto num;

	case '$':   b = 16;
num:		    pops(token.sval);
		    make_number(b);
		    break;

	case '#':   token.attr = IMM;
		    break;

	case '(':   token.attr = UNARY + LPREN + OPR;
		    goto opr1;

	case ')':   token.attr = BINARY + RPREN + OPR;
		    goto opr1;

	case '+':   token.attr = BINARY + UNARY + ADDIT + OPR;
		    goto opr1;

	case '-':   token.attr = BINARY + UNARY + ADDIT + OPR;
		    goto opr1;

	case '*':   token.attr = BINARY + UNARY + MULT + OPR;
		    goto opr1;

	case '/':   token.attr = BINARY + MULT + OPR;
opr1:		    token.valu = c;
		    break;

	case '<':   token.valu = c;
		    if ((c = popc()) == '=') token.valu = LE;
		    else if (c == '>') token.valu = NE;
		    else pushc(c);
		    goto opr2;

	case '=':   token.valu = c;
		    if ((c = popc()) == '<') token.valu = LE;
		    else if (c == '>') token.valu = GE;
		    else pushc(c);
		    goto opr2;

	case '>':   token.valu = c;
		    if ((c = popc()) == '<') token.valu = NE;
		    else if (c == '=') token.valu = GE;
		    else pushc(c);
opr2:		    token.attr = BINARY + RELAT + OPR;
		    break;

	case '\'':
	case '"':   quote = TRUE;  token.attr = STR;
		    for (p = token.sval; (*p = popc()) != c; ++p)
			if (*p == '\n') { exp_error('"');  break; }
		    *p = '\0';  quote = FALSE;
		    if ((token.valu = token.sval[0]) && token.sval[1])
			token.valu = (token.valu << 8) + token.sval[1];
		    break;

	case ',':   token.attr = SEP;
		    break;

        case '\n':  token.attr = EOL;
		    break;
    }
    return &token;
}

static void make_number(base)
unsigned base;
{
    SCRATCH char *p;
    SCRATCH unsigned d;
    void exp_error();

    token.attr = VAL;
    token.valu = 0;
    for (p = token.sval; *p; ++p) {
	d = toupper(*p) - (isnum(*p) ? '0' : 'A' - 10);
	token.valu = token.valu * base + d;
	if (!ishex(*p) || d >= base) { exp_error('D');  break; }
    }
    clamp(token.valu);
    return;
}

int isalph(c)
char c;
{
    return (c >= 'A' && c <= '~') || c == '!' ||
	c == '&' || c == '.' || c == ':' || c == '?';
}

static int isnum(c)
char c;
{
    return c >= '0' && c <= '9';
}

static int ishex(c)
char c;
{
    return isnum(c) || ((c = toupper(c)) >= 'A' && c <= 'F');
}

static int isalnum(c)
char c;
{
    return isalph(c) || isnum(c);
}

/*  Push back the current token into the input stream.  One level of	*/
/*  pushback is supported.						*/

void unlex()
{
    oldt = TRUE;
    return;
}

/*  Get an alphanumeric string into the string value part of the	*/
/*  current token.  Leading blank space is trashed.			*/

void pops(s)
char *s;
{
    void pushc(), trash();

    trash();
    for (; isalnum(*s = popc()); ++s);
    pushc(*s);  *s = '\0';
    return;
}

/*  Trash blank space and push back the character following it.		*/

void trash()
{
    SCRATCH char c;
    void pushc();

    while ((c = popc()) == ' ');
    pushc(c);
    return;
}

/*  Get character from input stream.  This routine does a number of	*/
/*  other things while it's passing back characters.  All control	*/
/*  characters except \t and \n are ignored.  \t is mapped into ' '.	*/
/*  Semicolon is mapped to \n.  In addition, a copy of all input is set	*/
/*  up in a line buffer for the benefit of the listing.			*/

static int oldc, eol;
static char *lptr;

int popc()
{
    SCRATCH int c;

    if (oldc) { c = oldc;  oldc = '\0';  return c; }
    if (eol) return '\n';
    for (;;) {
	if ((c = getc(source)) != EOF && (c &= 0377) == ';' && !quote) {
	    do *lptr++ = c;
	    while ((c = getc(source)) != EOF && (c &= 0377) != '\n');
	}
	if (c == EOF) c = '\n';
	if ((*lptr++ = c) >= ' ' && c <= '~') return c;
	if (c == '\n') { eol = TRUE;  *lptr = '\0';  return '\n'; }
	if (c == '\t') return quote ? '\t' : ' ';
    }
}

/*  Push character back onto input stream.  Only one level of push-back	*/
/*  supported.  \0 cannot be pushed back, but nobody would want to.	*/

void pushc(c)
char c;
{
    oldc = c;
    return;
}

/*  Begin new line of source input.  This routine returns non-zero if	*/
/*  EOF	has been reached on the main source file, zero otherwise.	*/

int newline()
{
    void fatal_error();

    oldc = '\0';  lptr = line;
    oldt = eol = FALSE;
    while (feof(source)) {
	if (ferror(source)) fatal_error(ASMREAD);
	if (filesp) {
	    fclose(source);
	    source = filestk[--filesp];
	}
	else return TRUE;
    }
    return FALSE;
}
