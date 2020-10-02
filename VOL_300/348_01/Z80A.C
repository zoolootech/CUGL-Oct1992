/*

This file contains the main program and line assembly routines for the
assembler.  The main program parses the command line, feeds the source lines
to the line assembly routine, and sends the results to the listing and object
file output routines.  It also coordinates the activities of everything.  The
line assembly routine uses the expression analyzer and the lexical analyzer to
parse the source line convert it into the object bytes that it represents.
*/

#define YES 1
#define NO  0

/*  Get global goodies:  */

#include "z80a.h"

/*  Define global mailboxes for all modules:				*/

char errcode, line[MAXLINE + 1], title[MAXLINE], sub_tit[MAXLINE];
int pass = 0;
int code = TRUE;
int listok = TRUE;
int eject, filesp, forwd, listhex;
unsigned address, bytes, errors, listleft, obj[MAXLINE], pc;
unsigned pagelen = 60;
FILE *filestk[FILES], *source;
TOKEN token;

/*  Mainline routine.  This routine parses the command line, sets up	*/
/*  the assembler at the beginning of each pass, feeds the source text	*/
/*  to the line assembler, feeds the result to the listing and hex file	*/
/*  drivers, and cleans everything up at the end of the run.		*/

static int done, ifsp, off;

main(argc,argv)
int argc;
char **argv;
{
    SCRATCH unsigned *o;
    int newline();
    void asm_line();
    void lclose(), lopen(), lputs();
    void hclose(), hopen(), hputc();
    void error_s(), fatal_error(), warning();

    printf("Z80 Cross-Assembler (Portable Z80ASM version) Ver 0.0\n");
    printf("Copyright (c) 1988 Michael G. Panas\n\n");

    while (--argc) {
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
	errors = filesp = ifsp = pc = 0;  title[0] = '\0';
	while (!done) {
	    errcode = ' ';
	    if (newline()) {
		error_s('*');
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
    void error_s(), pops(), pushc(), trash(), trash_lex();
#ifdef MICROSOFT_C
    void put_len(unsigned);
#else
    void put_len();
#endif

    address = pc;  bytes = 0;  eject = forwd = listhex = FALSE;
    for (i = 0; i < BIGINST; obj[i++] = NOP);

    label[0] = '\0';

    if ((i = popc()) != ' ' && i != '\n') {
	if (isalph(i)) {
	    pushc(i);  pops(label);
	    if (find_operator(label)) { label[0] = '\0';  error_s('L'); }
	}
	else {
	    error_s('L');
	    while ((i = popc()) != ' ' && i != '\n');
	}
    }

    trash(); opcod = NULL;
    if ((i = popc()) != '\n') {
	if (!isalph(i)) error_s('S');
	else {
	    pushc(i);  pops(token.sval);
	    if (!(opcod = find_code(token.sval))) error_s('O');
	}
	if (!opcod) { listhex = TRUE;  bytes = BIGINST; }
    }

    if (opcod && opcod -> attr & ISIF) { if (label[0]) error_s('L'); }
    else if (off) { listhex = FALSE;  flush();  return; }

    if (!opcod) { do_label();  flush(); }
    else {
	listhex = TRUE;
	trash_lex();		/* space allowed here - don't eat '*' */
	if (opcod -> attr & PSEUDO) {
		pseudo_op();
		if (bytes) put_len(bytes);
	} else {
		normal_op();
		put_len(0);
	}
	flush(); /* throw rest of line away */
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
    unsigned pin, st;
    SYMBOL *find_symbol(), *new_symbol();
    void error_s();

    if (label[0]) {
	listhex = TRUE;
	if (label[0] == '@') {
		pin = PIN;
		st = 1;
	} else {
		pin = st = 0;
	}
	if (pass == 1) {
	    if (!((l = new_symbol(&label[st])) -> attr)) {
		l -> attr = FORWD + VAL + pin;
		l -> valu = pc;
		l -> len = 2; /* preset length */
	    }
	}
	else {
	    if (l = find_symbol(&label[st])) {
		l -> attr = VAL + pin;
		if (l -> valu != pc) error_s('M');
	    }
	    else error_s('P');
	}
    }
}

static void put_len(length)
unsigned length;
{
    unsigned st = 0;
    SCRATCH SYMBOL *l;
    SYMBOL *find_symbol();
    
    if (label[0]) {
	if (label[0] == '@')	st = 1;
	if (l = find_symbol(&label[st])) {
		l -> len = length;
	}
    }
}
static void put_attr(attrv)
unsigned attrv;
{
    unsigned st = 0;
    SCRATCH SYMBOL *l;
    SYMBOL *find_symbol();
    
    if (label[0]) {
	if (label[0] == '@')	st = 1;
	if (l = find_symbol(&label[st])) {
		l -> attr |= attrv;
	}
    }
}

unsigned get_len(sym)
char *sym;
{
    SCRATCH SYMBOL *l;
    SYMBOL *find_symbol();
    
    if (pass == 1) return 1;
    if (l = find_symbol(sym)) {
    	if (l -> len == 0) return 2;
    	return l -> len;
    }
    return 0;
}

static void normal_op()
{
    SCRATCH unsigned t, opcode, op, left, index, long_op;
    int grab_comma();
    unsigned expr(), grab_index();
    TOKEN *lex(), *lex_op();
    void do_label(), error_s(), s_error(), unlex();

    opcode = opcod -> valu;
    do_label();
    left = ((opcod -> attr & SHIFT) >> 8);
    long_op = NO;
    
    switch (opcod -> attr & OPTYPE) {
	case NO_ARG:
			bytes = 1;
			break;

	case NO_ARG_L:	long_op = YES;
			bytes = 2;
			break;

	case ONE_ARG:	/* all 8 bit IMM mode instuctions */
			if (left) { /* LDI case */
				op = (lex() -> valu);
				switch (token.attr & TYPE) {
					case MIX:
					case REG: break;
					default:  goto error;
				}
				if (token.attr & INDEX) { /* IX or IY */
					opcode += ((op << 8)+0x30); /* set prefix */
					if (grab_comma()) goto error;
					op = grab_index();
				/* SEP eaten by grab_index() */
					op += (expr() & 0xff) << 8;
					long_op = YES;
					bytes = 4;
					break;
				}
				if (grab_sep()) goto error; /* no semicolon */
				opcode += (op << left);
			}
			op = expr();
			bytes = 2;
			break;

	case ONE_ARG_L: /* opcode is one or two bytes with arg */
			/* JUMPI and LDSP only */
			op = lex() -> valu;
			if ((token.attr & TYPE) != REG) {
				goto error;
			}
			if (token.attr & INDEX) { /* IX or IY */
				opcode += (op << 8); /* set prefix */
				long_op = YES;
				bytes = 2;
				break;
			}
			if (op != HL) goto error; /* only HL allowed */
			bytes = 1;
			break;
	
	case ONE_ARG_L2: /* opcode is two bytes with arg type 2 */
			/* ADCHL, ADDIY, ADDIX, SBCHL only */
			op = lex() -> valu;
			if ((token.attr & TYPE) != REG) {
				goto error;
			}
			switch (op) {
				case BC: op = 0; break;
				case DE: op = 1; break;
				case X: op = 2;
					if (opcode != 0xdd09) goto error;
					break;
				case Y: op = 2;
					if (opcode != 0xfd09) goto error;
					break;
				case HL: op = 2;
					if (opcode == 0xdd09 || opcode == 0xfd09)
						goto error;
					break;
				case SP: op = 3; break;
				default: goto error;
			}
			opcode += (op << 4);
			long_op = YES;
			bytes = 2;
			break;			

	case CALL:
	case JUMP:	op = lex() -> valu;
			if ((t = token.attr & TYPE) == CON || (t == MIX)) {
				switch (op) {
					case C:	 op = 0x03;
						 break;
					case M:  op = 0x07;
						 break;
					default: op -= 0x10;
				}
				if (grab_sep()) goto error;
				if (opcode == 0xcd) {
					opcode = 0xc4;
				} else {
					opcode--;
				}
				opcode += (op << 3);
				op = expr();
				bytes = 3;
				break;
			}
			unlex();
			op = expr();
			bytes = 3;
			break;

	case RETURN:	op = lex_op() -> valu;
			if ((t = token.attr & TYPE) == CON || (t == MIX)) {
				switch (op) {
					case C:	 op = 0x03;
						 break;
					case M:  op = 0x07;
						 break;
					default: op -= 0x10;
				}
				opcode = 0xc0;
				opcode += (op << 3);
			}
			unlex();
			bytes = 1;
			break;
	
	case JREL:	op = lex() -> valu;
			if ((t = token.attr & TYPE) == CON || (t == MIX)) {
				switch (op) {
					case C:	 opcode = 0x38;
						 break;
					case NC: opcode = 0x30;
						 break;
					case Z:  opcode = 0x28;
						 break;
					case NZ: opcode = 0x20;
						 break;
					default: goto error;
				}
				if (grab_sep()) goto error;
				goto jrel;
			}
			unlex();
jrel:
			op = expr();
			op -= pc + 2;
			if ((clamp(op) > 0x007f) && (op < 0xff80))
				error_s('R');
			bytes = 2;
			break;

	case L_ARG:	/* LDI16 - only BC, DE, HL, IX, IY and SP allowed as operands */
			op = lex() -> valu;
			switch (token.attr & TYPE) {
				case MIX:
				case REG: break;
				default:  goto error;
			}
			if (token.attr & INDEX) { /* IX or IY */
				opcode = ((op << 8)+0x21); /* set prefix */
				if (grab_sep()) goto error;
				op = expr();
				long_op = YES;
				bytes = 4;
				break;
			}
			switch (op) {
				case BC: op = 0; break;
				case DE: op = 1; break;
				case HL: op = 2; break;
				case SP: op = 3; break;
				default: goto error;
			}
			opcode += (op << 4);
			if (grab_sep()) goto error;
l_arg2:
	case L_ARG2:
			op = expr();
l_arg:
			bytes = 3;
			break;			

	case L_ARG_L:	/* LD16 only 		*/
			op = lex() -> valu;
			if ((token.attr & TYPE) != REG) {
				goto error;
			}
			if (token.attr & INDEX) { /* IX or IY */
				opcode += (op << 8); /* set prefix */
				if (grab_sep()) goto error;
				op = expr();
				long_op = YES;
				bytes = 4;
				break;
			}
			switch (op) {
				case BC: op = 0; break;
				case DE: op = 1; break;
				case HL: if (grab_sep()) goto error; 
					 goto l_arg2;
				case SP: op = 3; break;
				default: goto error;
			}
			opcode = ((op << 4) + 0xed4b);
			if (grab_sep()) goto error;
			op = expr();
			long_op = YES;
			bytes = 4;
			break;			
			
	case L_ARG_L2:	/* ST16 only 		*/
			op = expr();
			/* expr() eats SEP */
			t = lex() -> valu;
			if ((token.attr & TYPE) != REG) {
				goto error;
			}
			if (token.attr & INDEX) { /* IX or IY */
				opcode += (t << 8); /* set prefix */
				long_op = YES;
				bytes = 4;
				break;
			}
			switch (t) {
				case BC: t = 0; break;
				case DE: t = 1; break;
				case HL: goto l_arg;
				case SP: t = 3; break;
				default: goto error;
			}
			opcode = ((t << 4) + 0xed43);
			long_op = YES;
			bytes = 4;
			break;			
			
	case L_OP:	/* ADDHL, DEC16, INC16, PUSH and POP */
			op = lex() -> valu;
			switch (token.attr & TYPE) {
				case MIX:
				case REG: break;
				default:  goto error;
			}
			if (token.attr & INDEX) { /* IX or IY */
				opcode += ((op << 8)+0x20); /* set prefix */
				long_op = YES;
				bytes = 2;
				break;
			}
			if ((opcode == 0xC1 || opcode == 0xC5) && op == SP) {
error:				s_error(); break;
			}
			if (op == AF) {
				switch (opcode) {
					case 0x09:
					case 0x0B:
					case 0x03: goto error;
				}
			}
			switch (op) {
				case BC: op = 0; break;
				case DE: op = 1; break;
				case HL: op = 2; break;
				case AF:
				case SP: op = 3; break;
				default: goto error;
			}
			opcode += (op << 4);
			bytes = 1;
			break;			

	case MOV:	/* LD - no value checking */
			op = (lex() -> valu);
			switch (token.attr & TYPE) {
				case MIX:
				case REG: break;
				default:  goto error;
			}
			if (token.attr & INDEX) { /* first operand IX or IY */
				opcode += ((op << 8)+0x30); /* set prefix */
				if (grab_comma()) goto error;
				index = grab_index();
				/* SEP eaten by grab_index() */
				/* get second operand */
				op = (lex() -> valu);
				switch (token.attr & TYPE) {
					case MIX:
					case REG: break;
					default:  goto error;
				}
				if (token.attr & INDEX) { /* second operand IX or IY */
					goto error;
				}
				opcode += op;
				op = low(index);
				long_op = YES;
				bytes = 3;
				break;
			}
			if (grab_sep()) goto error; /* no comma */
			opcode += (op << 3);
			/* get second operand ( first was not IX or IY) */
			op = (lex() -> valu);
			switch (token.attr & TYPE) {
				case MIX:
				case REG: break;
				default:  goto error;
			}
			if (token.attr & INDEX) { /* second operand IX or IY */
				opcode += ((op << 8)+0x06); /* set prefix */
				if (grab_comma()) goto error;
				op = low(grab_index());
				long_op = YES;
				bytes = 3;
				break;
			}
			opcode += op;
			bytes = 1;
			break;			
	case RESTART:
			op = expr();
			op >>= 3;
			if (op > 7) goto error;
			opcode += (op << 3);
			bytes = 1;
			break;
	
	case S_OP:	
			op = (lex() -> valu);
			switch (token.attr & TYPE) {
				case MIX:
				case REG: break;
				default:  goto error;
			}
			if (token.attr & INDEX) { /* operand IX or IY */
				opcode += ((op << 8)+(0x06 << left)); /* set prefix */
				if (grab_comma()) goto error;
				op = grab_index();
				long_op = YES;
				bytes = 3;
				break;
			}
			opcode += (op << left);
			bytes = 1;
			break;

	case ROTATE:
			op = (lex() -> valu);
			switch (token.attr & TYPE) {
				case MIX:
				case REG: break;
				default:  goto error;
			}
			if (token.attr & INDEX) { /* operand IX or IY */
				op <<= 8; op += high(opcode);
				index = opcode; /* temp only */
				opcode = op;
				if (grab_comma()) goto error;
				op = (low(index) + 0x06) << 8;
				op += grab_index();
				long_op = YES;
				bytes = 4;
				break;
			}
			opcode += op;
			long_op = YES;
			bytes = 2;
			break;			

	case XIO:
			op = (lex() -> valu);
			switch (token.attr & TYPE) {
				case MIX:
				case REG: break;
				default:  goto error;
			}
			switch (op) {
				case X:
				case Y:
				case M:
				case AF:
				case BC:
				case DE:
				case HL:
				case SP: goto error;
			}
			opcode += (op << 3);
			long_op = YES;
			bytes = 2;
			break;
	case BIT:
			op = expr();
			if (op > 7 || op < 0) goto error;
			opcode += (op << 3);
			/* if (grab_sep()) goto error; */
			op = (lex() -> valu);
			switch (token.attr & TYPE) {
				case MIX:
				case REG: break;
				default:  goto error;
			}
			if (token.attr & INDEX) { /* operand IX or IY */
				op <<= 8; op += high(opcode);
				index = opcode; /* temp only */
				opcode = op;
				op = (low(index) + 0x06) << 8;
				if (grab_comma()) goto error;
				op += grab_index();
				long_op = YES;
				bytes = 4;
				break;
			}
			opcode += op;
			long_op = YES;
			bytes = 2;
			break;			


    }
    if (long_op) {
    	obj[3] = high(op); obj[2] = low(op);
    	obj[1] = low(opcode); obj[0] = high(opcode);
    	return;
    } else {
    	obj[2] = high(op);  obj[1] = low(op);
    	obj[0] = low(opcode);
   	return;
    }
}

static int grab_sep()
{
    TOKEN *lex();

    return (lex() -> attr & TYPE) != SEP;
}

static int grab_comma()
{
    TOKEN *lex();

    return (lex() -> attr & TYPE) != SIZE;
}

static unsigned grab_index()
{
	SCRATCH unsigned index;
	unsigned expr();
	
    if ((index = expr()) > 0x7f && index < 0xff80) error_s('V');
    return low(index);
}
    
static void s_error()
{
    void error_s();

    bytes = 2;  error_s('S');
}

static void pseudo_op()
{
    SCRATCH char *s;
    SCRATCH unsigned *o, u, t, c, cnt;
    SCRATCH SYMBOL *l;
    int popc(), ishex(), isnum();
    unsigned expr();
    SYMBOL *find_symbol(), *new_symbol();
    TOKEN *lex();
    void do_label(), error_s(), fatal_error();
    void hseek(), pushc(), trash(), unlex();
#ifdef MICROSOFT_C
    void put_attr(unsigned), put_len(unsigned);
#else
    void put_attr(), put_len();
#endif

    for (o = obj; o != obj+MAXLINE; *o++ = 0); /* zero array */
    o = obj;
    switch (opcod -> valu) {
	case DB:    do_label();
		    do {
			/*if ((lex() -> attr & TYPE) == SEP) u = 0;*/
more_db:
			pushc(t = popc());
		    	/*if (token.attr == STR) { */
		    	if (t == '"' || t == '\'') {
				lex();
				pushc(u = popc());
				if (u == ';' || u == '\n' || u == ' ') {
			    		for (s = token.sval; *s; *o++ = *s++)
			    		++bytes;
			    		if ((token.attr & TYPE) == SEP) goto more_db;
			    		/*lex();*/  continue;
				}
		    	}
		    	/* unlex();*/
		    	u = popc();
		    	if (u == '$') {
		    		cnt = 0;
		    		while (pushc(u = popc()), ishex(u) && u != '\n') {
		    			for (c = t = 0; c != 2; c++) {
		    				if (ishex(u = popc())) {
	    				  		t = (t*16)+(toupper(u)
		    					- (isnum(u) ? '0' : 'A'-10));
		    				} else { pushc(u); break; }
		    			}
		    			*o++ = low(t); bytes++; cnt++;
		    		}
		    		if ( u == ',') {
		    			grab_comma();
		    			t = expr();
		    			if (t > MAXLINE) t = MAXLINE;
		    			if ((u = t - cnt) < 0) {
		    				u = 0; error_s('d');
		    			}
		    			bytes += u;
					if ((token.attr & TYPE) == SEP) goto more_db;
		    		}
		    	} else {
		    		pushc(u);
		    		u = expr();
		    		if ((token.attr & TYPE) == SIZE) t = expr();
		    		else t = 1;
		    		if (t > MAXLINE) {
		    			t = MAXLINE;
		    			error_s('r');
		    		}
				if (t > 1) {
					*o++ = low(u); *o++ = high(u); bytes += t;
				   } else {
					*o++ = low(u);  ++bytes;
				}
				if ((token.attr & TYPE) == SEP) goto more_db;
		    	}
		    /*} while ((token.attr & TYPE) == SEP);*/
		    } while (pushc(t = popc()), t == ';');
		    put_attr(PIN_VAL);
		    break;

	case DS:    do_label();  u = word(pc + (t = expr()));
		    if (forwd) error_s('P');
		    else {
			pc = u;
			if (pass == 2) hseek(pc);
		    }
		    put_len(t);
		    put_attr(PIN_VAL);
		    break;

	case DW:    do_label();
		    do {
			if ((lex() -> attr & TYPE) == SEP) u = 0;
			else { unlex();  u = expr(); }
			*o++ = high(u);  *o++ = low(u);
			bytes += 2;
		    } while ((token.attr & TYPE) == SEP);
		    put_attr(PIN_VAL);
		    break;

	case ELSE:  listhex = FALSE;
		    if (ifsp) off = (ifstack[ifsp] = -ifstack[ifsp]) != ON;
		    else error_s('I');
		    break;

	case END:   do_label();
		    if (filesp) { listhex = FALSE;  error_s('*'); }
		    else {
			done = eject = TRUE;
			if (pass == 2 && (lex() -> attr & TYPE) != EOL) {
			    unlex();  hseek(address = expr());
			}
			if (ifsp) error_s('I');
		    }
		    break;

	case ENDIF: listhex = FALSE;
		    if (ifsp) off = ifstack[--ifsp] != ON;
		    else error_s('I');
		    break;

	case EQU:   if (label[0]) {
			if (pass == 1) {
			    if (!((l = new_symbol(label)) -> attr)) {
				l -> attr = FORWD + VAL;  address = expr();
				if (!forwd) l -> valu = address;
		    		if ((token.attr & TYPE) == SIZE) {
		    			l -> len = expr();
		    			if (forwd) l -> len = 2;
		    		} else l -> len = 2; /* preset length */
			    }
			}
			else {
			    if (l = find_symbol(label)) {
				l -> attr = VAL;  address = expr();
				if (forwd) error_s('P');
				if (l -> valu != address) error_s('M');
			    }
			    else error_s('P');
			}
		    }
		    else error_s('L');
		    break;

	case IF:    if (++ifsp == IFDEPTH) fatal_error(IFOFLOW);
		    address = expr();
		    if (forwd) { error_s('P');  address = TRUE; }
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
			    --filesp;  error_s('V');
			}
		    }
		    else error_s('S');
		    break;

	case RLC:
	case ORG:   u = expr();
		    if (forwd) error_s('P');
		    else {
			pc = address = u;
			if (pass == 2) hseek(pc);
		    }
	case CSECT: do_label();
		    break;

	case PAGT:  listhex = FALSE;  do_label();
		    if ((lex() -> attr & TYPE) == EOL) sub_tit[0] = '\0';
		    else if ((token.attr & TYPE) != STR) error_s('S');
		    else strcpy(sub_tit,token.sval);
		    eject = TRUE;
		    break;

	case PAGE:  listhex = FALSE;  do_label();
		    if ((lex() -> attr & TYPE) != EOL) {
			unlex();  pagelen = expr();
			if (pagelen > 0 && pagelen < 3) {
			    pagelen = 0;  error_s('V');
			}
			break; /* no eject on value */
		    }
		    eject = TRUE;
		    break;

	case SET:   if (label[0]) {
			if (pass == 1) {
			    if (!((l = new_symbol(label)) -> attr)
				|| (l -> attr & SOFT)) {
				l -> attr = FORWD + SOFT + VAL;
				address = expr();
				if (!forwd) l -> valu = address;
		    		if ((token.attr & TYPE) == SIZE) {
		    			l -> len = expr();
		    			if (forwd) l -> len = 2;
		    		} else l -> len = 2; /* preset length */
			    }
			}
			else {
			    if (l = find_symbol(label)) {
				address = expr();
				if (forwd) error_s('P');
				else if (l -> attr & SOFT) {
				    l -> attr = SOFT + VAL;
				    l -> valu = address;
				}
				else error_s('M');
			    }
			    else error_s('P');
			}
		    }
		    else error_s('L');
		    break;

	case LIST:
	case ULIST: break;
	case SKP:   listok = FALSE;
		    break;
	case TYP:   listok = TRUE;
		    break;
	case URLC:  break;

	case CODE:  code = TRUE;
		    break;
	case NCODE: code = FALSE;
		    break;

	case TITL:  listhex = FALSE;  do_label();
		    if ((lex() -> attr & TYPE) == EOL) title[0] = '\0';
		    else if ((token.attr & TYPE) != STR) error_s('S');
		    else strcpy(title,token.sval);
		    break;

    }
    return;
}
