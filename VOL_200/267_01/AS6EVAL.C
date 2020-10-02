/*
	HEADER:		CUG267;
	TITLE:		S6 Cross-Assembler (Portable);
	FILENAME:	AS6EVAL.C;
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

This file contains the assembler's expression evaluator and lexical analyzer.
The lexical analyzer chops the input character stream up into discrete tokens
that are processed by the expression analyzer and the line assembler.  The
expression analyzer processes the token stream into unsigned results of
arithmetic expressions.
*/

/*  Get global goodies:  */

#include "as6.h"

/*  Get access to global mailboxes defined in AS6.C:			*/

extern char line[];
extern int filesp, forwd, pass;
extern unsigned pc;
extern FILE *filestk[], *source;
extern TOKEN token;

/*  Machine opcode argument field parsing routine.  The token stream	*/
/*  from the lexical analyzer is processed to extract addresses and	*/
/*  addressing mode information.  The resulting value is passed back	*/
/*  through the same token buffer that the lexical analyzer uses.  In	*/
/*  addition, the addressing mode information is returned as the	*/
/*  function return value.						*/

static int bad;

unsigned get_arg(num)
int num;
{
    SCRATCH int c;
    SCRATCH unsigned a, u;
    TOKEN *lex();
    int popc();
    unsigned eval(), expr();
    void exp_error(), pushc(), trash(), unlex();

    a = NULL;  u = 0;  bad = FALSE;
    switch (lex() -> attr & TYPE) {
	case EOL:
	case SEP:   exp_error('S');  break;

	case OPR:   if (token.valu == '(') {
			u = eval(LPREN);  trash();  pushc(c = popc());
			if (c == ',' || c == '\n') {
			    if (u < 0x80 || u > 0x81) exp_error('V');
			    else if (!forwd) a = IS_IND;
			    lex();  break;
			}
			token.attr = VAL;  token.valu = u;
		    }

	case VAL:
	case STR:   unlex();  u = eval(START);
		    if (!forwd) switch (u) {
			case 0xff:  a = IS_A;  break;

			case 0x80:
			case 0x81:
			case 0x82:
			case 0x83:  a = IS_XYVW;  break;

			default:    break;
		    }
		    break;
    }
    if (token.attr != (num ? EOL : SEP)) exp_error('S');
    token.valu = bad ? 0 : u;  return token.attr = a;
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
	 case SEP:   if (pre != START) unlex();
	 case EOL:   exp_error('E');  return;

	 case OPR:   if (!(token.attr & UNARY)) { exp_error('E');  break; }
		     u = eval((op == '+' || op == '-') ?
			   (unsigned) UOP1 : token.attr & PREC);
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
	    token.attr = o -> attr;  token.valu = o -> valu;
	}
	else {
	    token.attr = VAL;
	    if (!strcmp(token.sval,"$")) token.valu = pc;
	    else if (s = find_symbol(token.sval)) {
		token.valu = s -> valu;
		if (pass == 2 && s -> attr & FORWD) forwd = TRUE;
	    }
	    else { token.valu = 0;  exp_error('U'); }
	}
    }
    else if (isnum(c)) {
	pushc(c);  pops(token.sval);
	for (p = token.sval; p[1]; ++p);
	switch (toupper(*p)) {
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
    return (c >= '?' && c <= '~') || (c >= '#' && c <= '&') ||
	c == '!' || c == '.' || c == ':';
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
