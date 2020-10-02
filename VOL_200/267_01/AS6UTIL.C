/*
	HEADER:		CUG267;
	TITLE:		S6 Cross-Assembler (Portable);
	FILENAME:	AS6UTIL.C;
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

This module contains the following utility packages:

	1)  symbol table building and searching

	2)  opcode and operator table searching

	3)  listing file output

	4)  hex file output

	5)  error flagging
*/

/*  Get global goodies:  */

#include "as6.h"

/*  Make sure that MSDOS compilers using the large memory model know	*/
/*  that calloc() returns pointer to char as an MSDOS far pointer is	*/
/*  NOT compatible with the int type as is usually the case.		*/

char *calloc();

/*  Get access to global mailboxes defined in AS6.C:			*/

extern char errcode, line[], title[];
extern int eject, listhex;
extern unsigned address, bytes, errors, listleft, obj[], pagelen;

/*  The symbol table is a binary tree of variable-length blocks drawn	*/
/*  from the heap with the calloc() function.  The root pointer lives	*/
/*  here:								*/

static SYMBOL *sroot = NULL;

/*  Add new symbol to symbol table.  Returns pointer to symbol even if	*/
/*  the symbol already exists.  If there's not enough memory to store	*/
/*  the new symbol, a fatal error occurs.				*/

SYMBOL *new_symbol(nam)
char *nam;
{
    SCRATCH int i;
    SCRATCH SYMBOL **p, *q;
    void fatal_error();

    for (p = &sroot; (q = *p) && (i = strcmp(nam,q -> sname)); )
	p = i < 0 ? &(q -> left) : &(q -> right);
    if (!q) {
	if (!(*p = q = (SYMBOL *)calloc(1,sizeof(SYMBOL) + strlen(nam))))
	    fatal_error(SYMBOLS);
	strcpy(q -> sname,nam);
    }
    return q;
}

/*  Look up symbol in symbol table.  Returns pointer to symbol or NULL	*/
/*  if symbol not found.						*/

SYMBOL *find_symbol(nam)
char *nam;
{
    SCRATCH int i;
    SCRATCH SYMBOL *p;

    for (p = sroot; p && (i = strcmp(nam,p -> sname));
	p = i < 0 ? p -> left : p -> right);
    return p;
}

/*  Opcode table search routine.  This routine pats down the opcode	*/
/*  table for a given opcode and returns either a pointer to it or	*/
/*  NULL if the opcode doesn't exist.					*/

OPCODE *find_code(nam)
char *nam;
{
    OPCODE *bsearch();

    static OPCODE opctbl[] = {
	{ ADD_OP + 2,		0x005f,	"ADD"	},
	{ ADDI_OP + 2,		0x0057,	"ADDI"	},
	{ ADD_OP + 2,		0x00bf,	"AND"	},
	{ ADDI_OP + 2,		0x00b7,	"ANDI"	},
	{ JP + 2,		0x0001,	"CALL"	},
	{ CLR + 2,		0x000d,	"CLR"	},
	{ SHIFT_OP + 1,		0x002d,	"COM"	},
	{ ADD_OP + 2,		0x003f,	"CP"	},
	{ ADDI_OP + 2,		0x0037,	"CPI"	},
	{ PSEUDO,		DB,	"DB"	},
	{ INC_DEC + 2,		0x1dff,	"DEC"	},
	{ PSEUDO,		DS,	"DS"	},
	{ PSEUDO,		DW,	"DW"	},
	{ PSEUDO + ISIF,	ELSE,	"ELSE"	},
	{ PSEUDO,		END,	"END"	},
	{ PSEUDO + ISIF,	ENDIF,	"ENDIF"	},
	{ PSEUDO,		EQU,	"EQU"	},
	{ PSEUDO + ISIF,	IF,	"IF"	},
	{ INC_DEC + 2,		0x157f,	"INC"	},
	{ PSEUDO,		INCL,	"INCL"	},
	{ JP + 2,		0x0009,	"JP"	},
	{ JR + 1,		0x0006,	"JRC"	},
	{ JR_SIGN + 3,		0x00f3,	"JRN"	},
	{ JR + 1,		0x0002,	"JRNC"	},
	{ JR + 1,		0x0000,	"JRNZ"	},
	{ JR_SIGN + 3,		0x00e3,	"JRP"	},
	{ JR_BIT + 3,		0x0003,	"JRR"	},
	{ JR_BIT + 3,		0x0013,	"JRS"	},
	{ JR + 1,		0x0004,	"JRZ"	},
	{ LD + 2,		0x351f,	"LD"	},
	{ LDI + 3,		0x000d,	"LDI"	},
	{ NO_ARG + 1,		0x0004,	"NOP"	},
	{ PSEUDO,		ORG,	"ORG"	},
	{ PSEUDO,		PAGE,	"PAGE"	},
	{ BIT_OP + 2,		0x000b,	"RES"	},
	{ NO_ARG + 1,		0x00cd,	"RET"	},
	{ NO_ARG + 1,		0x004d,	"RETI"	},
	{ SHIFT_OP + 1,		0x00ad,	"RLC"	},
	{ BIT_OP + 2,		0x001b,	"SET"	},
	{ SHIFT_OP + 2,		0xff5f,	"SLA"	},
	{ NO_ARG + 1,		0x006d,	"STOP"	},
	{ ADD_OP + 2,		0x00df,	"SUB"	},
	{ ADDI_OP + 2,		0x00d7,	"SUBI"	},
	{ PSEUDO,		TITLE,	"TITLE"	},
	{ NO_ARG + 1,		0x00ed,	"WAIT"	},
	{ PSEUDO,		VAR,	"VAR"	}
    };

    return bsearch(opctbl,opctbl + (sizeof(opctbl) / sizeof(OPCODE)),nam);
}

/*  Operator table search routine.  This routine pats down the		*/
/*  operator table for a given operator and returns either a pointer	*/
/*  to it or NULL if the opcode doesn't exist.				*/

OPCODE *find_operator(nam)
char *nam;
{
    OPCODE *bsearch();

    static OPCODE oprtbl[] = {
	{ VAL,				0xff,		"A"	},
	{ BINARY + LOG1  + OPR,		AND,		"AND"	},
	{ BINARY + RELAT + OPR,		'=',		"EQ"	},
	{ BINARY + RELAT + OPR,		GE,		"GE"	},
	{ BINARY + RELAT + OPR,		'>',		"GT"	},
	{ UNARY  + UOP3  + OPR,		HIGH,		"HIGH"	},
	{ BINARY + RELAT + OPR,		LE,		"LE"	},
	{ UNARY  + UOP3  + OPR,		LOW,		"LOW"	},
	{ BINARY + RELAT + OPR,		'<',		"LT"	},
	{ BINARY + MULT  + OPR,		MOD,		"MOD"	},
	{ BINARY + RELAT + OPR,		NE,		"NE"	},
	{ UNARY  + UOP2  + OPR,		NOT,		"NOT"	},
	{ BINARY + LOG2  + OPR,		OR,		"OR"	},
	{ BINARY + MULT  + OPR,		SHL,		"SHL"	},
	{ BINARY + MULT  + OPR,		SHR,		"SHR"	},
	{ VAL,				0x82,		"V"	},
	{ VAL,				0x83,		"W"	},
	{ VAL,				0x80,		"X"	},
	{ BINARY + LOG2  + OPR,		XOR,		"XOR"	},
	{ VAL,				0x81,		"Y"	}
    };

    return bsearch(oprtbl,oprtbl + (sizeof(oprtbl) / sizeof(OPCODE)),nam);
}

static OPCODE *bsearch(lo,hi,nam)
OPCODE *lo, *hi;
char *nam;
{
    SCRATCH int i;
    SCRATCH OPCODE *chk;

    for (;;) {
	chk = lo + (hi - lo) / 2;
	if (!(i = ustrcmp(chk -> oname,nam))) return chk;
	if (chk == lo) return NULL;
	if (i < 0) lo = chk;
	else hi = chk;
    }
}

static int ustrcmp(s,t)
char *s, *t;
{
    SCRATCH int i;

    while (!(i = toupper(*s++) - toupper(*t)) && *t++);
    return i;
}

/*  Buffer storage for line listing routine.  This allows the listing	*/
/*  output routines to do all operations without the main routine	*/
/*  having to fool with it.						*/

static FILE *list = NULL;

/*  Listing file open routine.  If a listing file is already open, a	*/
/*  warning occurs.  If the listing file doesn't open correctly, a	*/
/*  fatal error occurs.  If no listing file is open, all calls to	*/
/*  lputs() and lclose() have no effect.				*/

void lopen(nam)
char *nam;
{
    FILE *fopen();
    void fatal_error(), warning();

    if (list) warning(TWOLST);
    else if (!(list = fopen(nam,"w"))) fatal_error(LSTOPEN);
    return;
}

/*  Listing file line output routine.  This routine processes the	*/
/*  source line saved by popc() and the output of the line assembler in	*/
/*  buffer obj into a line of the listing.  If the disk fills up, a	*/
/*  fatal error occurs.							*/

void lputs()
{
    SCRATCH int i, j;
    SCRATCH unsigned *o;
    void check_page(), fatal_error();

    if (list) {
	i = bytes;  o = obj;
	do {
	    fprintf(list,"%c  ",errcode);
	    if (listhex) {
		fprintf(list,"%04x  ",address);
		for (j = 4; j; --j) {
		    if (i) { --i;  ++address;  fprintf(list," %02x",*o++); }
		    else fprintf(list,"   ");
		}
	    }
	    else fprintf(list,"%18s","");
	    fprintf(list,"   %s",line);  strcpy(line,"\n");
	    check_page();
	    if (ferror(list)) fatal_error(DSKFULL);
	} while (listhex && i);
    }
    return;
}

/*  Listing file close routine.  The symbol table is appended to the	*/
/*  listing in alphabetic order by symbol name, and the listing file is	*/
/*  closed.  If the disk fills up, a fatal error occurs.		*/

static int col = 0;

void lclose()
{
    void fatal_error(), list_sym();

    if (list) {
	if (sroot) {
	    list_sym(sroot);
	    if (col) fprintf(list,"\n");
	}
	fprintf(list,"\f");
	if (ferror(list) || fclose(list) == EOF) fatal_error(DSKFULL);
    }
    return;
}

static void list_sym(sp)
SYMBOL *sp;
{
    void check_page();

    if (sp) {
	list_sym(sp -> left);
	fprintf(list,"%04x  %-10s",sp -> valu,sp -> sname);
	if (col = ++col % SYMCOLS) fprintf(list,"    ");
	else {
	    fprintf(list,"\n");
	    if (sp -> right) check_page();
	}
	list_sym(sp -> right);
    }
    return;
}

static void check_page()
{
    if (pagelen && !--listleft) eject = TRUE;
    if (eject) {
	eject = FALSE;  listleft = pagelen;  fprintf(list,"\f");
	if (title[0]) { listleft -= 2;  fprintf(list,"%s\n\n",title); }
    }
    return;
}

/*  Buffer storage for hex output file.  This allows the hex file	*/
/*  output routines to do all of the required buffering and record	*/
/*  forming without the	main routine having to fool with it.		*/

static FILE *hex = NULL;
static unsigned cnt = 0;
static unsigned addr = 0;
static unsigned sum = 0;
static unsigned buf[HEXSIZE];

/*  Hex file open routine.  If a hex file is already open, a warning	*/
/*  occurs.  If the hex file doesn't open correctly, a fatal error	*/
/*  occurs.  If no hex file is open, all calls to hputc(), hseek(), and	*/
/*  hclose() have no effect.						*/

void hopen(nam)
char *nam;
{
    FILE *fopen();
    void fatal_error(), warning();

    if (hex) warning(TWOHEX);
    else if (!(hex = fopen(nam,"w"))) fatal_error(HEXOPEN);
    return;
}

/*  Hex file write routine.  The data byte is appended to the current	*/
/*  record.  If the record fills up, it gets written to disk.  If the	*/
/*  disk fills up, a fatal error occurs.				*/

void hputc(c)
unsigned c;
{
    void record();

    if (hex) {
	buf[cnt++] = c;
	if (cnt == HEXSIZE) record(0);
    }
    return;
}

/*  Hex file address set routine.  The specified address becomes the	*/
/*  load address of the next record.  If a record is currently open,	*/
/*  it gets written to disk.  If the disk fills up, a fatal error	*/
/*  occurs.								*/

void hseek(a)
unsigned a;
{
    void record();

    if (hex) {
	if (cnt) record(0);
	addr = a;
    }
    return;
}

/*  Hex file close routine.  Any open record is written to disk, the	*/
/*  EOF record is added, and file is closed.  If the disk fills up, a	*/
/*  fatal error occurs.							*/

void hclose()
{
    void fatal_error(), record();

    if (hex) {
	if (cnt) record(0);
	record(1);
	if (fclose(hex) == EOF) fatal_error(DSKFULL);
    }
    return;
}

static void record(typ)
unsigned typ;
{
    SCRATCH unsigned i;
    void fatal_error(), putb();

    putc(':',hex);  putb(cnt);  putb(high(addr));
    putb(low(addr));  putb(typ);
    for (i = 0; i < cnt; ++i) putb(buf[i]);
    putb(low(-sum));  putc('\n',hex);

    addr += cnt;  cnt = 0;

    if (ferror(hex)) fatal_error(DSKFULL);
    return;
}

static void putb(b)
unsigned b;
{
    static char digit[] = "0123456789ABCDEF";

    putc(digit[b >> 4],hex);  putc(digit[b & 0x0f],hex);
    sum += b;  return;
}

/*  Error handler routine.  If the current error code is non-blank,	*/
/*  the error code is filled in and the	number of lines with errors	*/
/*  is adjusted.							*/

void error(code)
char code;
{
    if (errcode == ' ') { errcode = code;  ++errors; }
    return;
}

/*  Fatal error handler routine.  A message gets printed on the stderr	*/
/*  device, and the program bombs.					*/

void fatal_error(msg)
char *msg;
{
    printf("Fatal Error -- %s\n",msg);
    exit(-1);
}

/*  Non-fatal error handler routine.  A message gets printed on the	*/
/*  stderr device, and the routine returns.				*/

void warning(msg)
char *msg;
{
    printf("Warning -- %s\n",msg);
    return;
}
