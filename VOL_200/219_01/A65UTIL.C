/*
	HEADER:		CUG219;
	TITLE:		6502 Cross-Assembler (Portable);
	FILENAME:	A65UTIL.C;
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

This module contains the following utility packages:

	1)  symbol table building and searching

	2)  opcode and operator table searching

	3)  listing file output

	4)  hex file output

	5)  error flagging
*/

/*  Get global goodies:  */

#include "a65.h"

/*  Make sure that MSDOS compilers using the large memory model know	*/
/*  that calloc() returns pointer to char as an MSDOS far pointer is	*/
/*  NOT compatible with the int type as is usually the case.		*/

char *calloc();

/*  Get access to global mailboxes defined in A65.C:			*/

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
	{ TWOOP,		0x61,	"ADC"	},
	{ TWOOP,		0x21,	"AND"	},
	{ LOGOP,		0x06,	"ASL"	},
	{ INHOP,		0x0a,	"ASLA"	},
	{ RELBR,		0x90,	"BCC"	},
	{ RELBR,		0xb0,	"BCS"	},
	{ RELBR,		0xf0,	"BEQ"	},
	{ BITOP,		0x24,	"BIT"	},
	{ RELBR,		0x30,	"BMI"	},
	{ RELBR,		0xd0,	"BNE"	},
	{ RELBR,		0x10,	"BPL"	},
	{ INHOP,		0x00,	"BRK"	},
	{ RELBR,		0x50,	"BVC"	},
	{ RELBR,		0x70,	"BVS"	},
	{ INHOP,		0x18,	"CLC"	},
	{ INHOP,		0xd8,	"CLD"	},
	{ INHOP,		0x58,	"CLI"	},
	{ INHOP,		0xb8,	"CLV"	},
	{ TWOOP,		0xc1,	"CMP"	},
	{ CPXY,			0xe0,	"CPX"	},
	{ CPXY,			0xc0,	"CPY"	},
	{ INCOP,		0xc6,	"DEC"	},
	{ INHOP,		0xca,	"DEX"	},
	{ INHOP,		0x88,	"DEY"	},
	{ PSEUDO + ISIF,	ELSE,	"ELSE"	},
	{ PSEUDO,		END,	"END"	},
	{ PSEUDO + ISIF,	ENDI,	"ENDI"	},
	{ TWOOP,		0x41,	"EOR"	},
	{ PSEUDO,		EQU,	"EQU"	},
	{ PSEUDO,		FCB,	"FCB"	},
	{ PSEUDO,		FCC,	"FCC"	},
	{ PSEUDO,		FDB,	"FDB"	},
	{ PSEUDO + ISIF,	IF,	"IF"	},
	{ INCOP,		0xe6,	"INC"	},
	{ PSEUDO,		INCL,	"INCL"	},
	{ INHOP,		0xe8,	"INX"	},
	{ INHOP,		0xc8,	"INY"	},
	{ JUMP,			0x4c,	"JMP"	},
	{ CALL,			0x20,	"JSR"	},
	{ TWOOP,		0xa1,	"LDA"	},
	{ LDXY,			0xa2,	"LDX"	},
	{ LDXY,			0xa0,	"LDY"	},
	{ LOGOP,		0x46,	"LSR"	},
	{ INHOP,		0x4a,	"LSRA"	},
	{ INHOP,		0xea,	"NOP"	},
	{ TWOOP,		0x01,	"ORA"	},
	{ PSEUDO,		ORG,	"ORG"	},
	{ PSEUDO,		PAGE,	"PAGE"	},
	{ INHOP,		0x48,	"PHA"	},
	{ INHOP,		0x08,	"PHP"	},
	{ INHOP,		0x68,	"PLA"	},
	{ INHOP,		0x28,	"PLP"	},
	{ PSEUDO,		RMB,	"RMB"	},
	{ LOGOP,		0x26,	"ROL"	},
	{ INHOP,		0x2a,	"ROLA"	},
	{ LOGOP,		0x66,	"ROR"	},
	{ INHOP,		0x6a,	"RORA"	},
	{ INHOP,		0x40,	"RTI"	},
	{ INHOP,		0x60,	"RTS"	},
	{ TWOOP,		0xe1,	"SBC"	},
	{ INHOP,		0x38,	"SEC"	},
	{ INHOP,		0xf8,	"SED"	},
	{ INHOP,		0x78,	"SEI"	},
	{ PSEUDO,		SET,	"SET"	},
	{ TWOOP,		0x81,	"STA"	},
	{ STXY,			0x86,	"STX"	},
	{ STXY,			0x84,	"STY"	},
	{ INHOP,		0xaa,	"TAX"	},
	{ INHOP,		0xa8,	"TAY"	},
	{ PSEUDO,		TITL,	"TITL"	},
	{ INHOP,		0xba,	"TSX"	},
	{ INHOP,		0x8a,	"TXA"	},
	{ INHOP,		0x9a,	"TXS"	},
	{ INHOP,		0x98,	"TYA"	}
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
	{ REG,				'A',		"A"	},
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
	{ REG,				'X',		"X"	},
	{ BINARY + LOG2  + OPR,		XOR,		"XOR"	},
	{ REG,				'Y',		"Y"	},
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
