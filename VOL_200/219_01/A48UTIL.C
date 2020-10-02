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

This module contains the following utility packages:

	1)  symbol table building and searching

	2)  opcode and operator table searching

	3)  listing file output

	4)  hex file output

	5)  error flagging
*/

/*  Get global goodies:  */

#include "a48.h"

/*  Make sure that MSDOS compilers using the large memory model know	*/
/*  that calloc() returns pointer to char as an MSDOS far pointer is	*/
/*  NOT compatible with the int type as is usually the case.		*/

char *calloc();

/*  Get access to global mailboxes defined in A18.C:			*/

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
	{ ADD_OP + (R0 << 4) + 10,		0x60,	"ADD"	},
	{ ADD_OP + (R0 << 4) + 10,		0x70,	"ADDC"	},
	{ LOG_OP + (R0 << 4) + 10,		0x50,	"ANL"	},
	{ OUTPUT + (P4 << 4) + 4,		0x90,	"ANLD"	},
	{ LNG_JMP,				0x14,	"CALL"	},
	{ ONE_ARG + (F0 << 4) + 4,		0x27,	"CLR"	},
	{ ONE_ARG + (F0 << 4) + 4,		0x37,	"CPL"	},
	{ ONE_ARG + (A << 4) + 1,		0x57,	"DA"	},
	{ PSEUDO,				DB,	"DB"	},
	{ INC_DEC + (R0 << 4) + 8,		0xc0,	"DEC"	},
	{ ONE_ARG + (TCNTI << 4) + 2,		0x15,	"DIS"	},
	{ REG_CND + (R0 << 4) + 8,		0xe0,	"DJNZ"	},
	{ PSEUDO,				DS,	"DS"	},
	{ PSEUDO,				DW,	"DW"	},
	{ PSEUDO + ISIF,			ELSE,	"ELSE"	},
	{ ONE_ARG + (TCNTI << 4) + 2,		0x05,	"EN"	},
	{ PSEUDO,				END,	"END"	},
	{ PSEUDO + ISIF,			ENDIF,	"ENDIF"	},
	{ ONE_ARG + (CLK << 4) + 1,		0x75,	"ENT0"	},
	{ PSEUDO,				EQU,	"EQU"	},
	{ PSEUDO + ISIF,			IF,	"IF"	},
	{ A_REG_1 + (P1 << 4) + 2,		0x00,	"IN"	},
	{ INC_DEC + (R0 << 4) + 10,		0x10,	"INC"	},
	{ PSEUDO,				INCL,	"INCL"	},
	{ A_REG_1 + (BUS << 4) + 1,		0x00,	"INS"	},
	{ JMP_CND,				0x12,	"JB0"	},
	{ JMP_CND,				0x32,	"JB1"	},
	{ JMP_CND,				0x52,	"JB2"	},
	{ JMP_CND,				0x72,	"JB3"	},
	{ JMP_CND,				0x92,	"JB4"	},
	{ JMP_CND,				0xb2,	"JB5"	},
	{ JMP_CND,				0xd2,	"JB6"	},
	{ JMP_CND,				0xf2,	"JB7"	},
	{ JMP_CND,				0xf6,	"JC"	},
	{ JMP_CND,				0xb6,	"JF0"	},
	{ JMP_CND,				0x76,	"JF1"	},
	{ LNG_JMP,				0x04,	"JMP"	},
	{ ONE_ARG + (AT_A << 4) + 1,		0xb3,	"JMPP"	},
	{ JMP_CND,				0xe6,	"JNC"	},
	{ JMP_CND,				0x86,	"JNI"	},
	{ JMP_CND,				0x26,	"JNT0"	},
	{ JMP_CND,				0x46,	"JNT1"	},
	{ JMP_CND,				0x96,	"JNZ"	},
	{ JMP_CND,				0x36,	"JT0"	},
	{ JMP_CND,				0x56,	"JT1"	},
	{ JMP_CND,				0x16,	"JTF"	},
	{ JMP_CND,				0xc6,	"JZ"	},
	{ MOV + (R0 << 4) + 10,			0x00,	"MOV"	},
	{ A_REG_2 + (P4 << 4) + 4,		0x00,	"MOVD"	},
	{ A_AT_A + (AT_A << 4) + 1,		0xa3,	"MOVP"	},
	{ A_AT_A + (AT_A << 4) + 1,		0xe3,	"MOVP3"	},
	{ A_REG_2 + (AT_R0 << 4) + 2,		0x80,	"MOVX"	},
	{ NO_ARG,				0x00,	"NOP"	},
	{ PSEUDO,				ORG,	"ORG"	},
	{ LOG_OP + (R0 << 4) + 10,		0x40,	"ORL"	},
	{ OUTPUT + (P4 << 4) + 4,		0x80,	"ORLD"	},
	{ OUTPUT + (BUS << 4) + 3,		0x00,	"OUTL"	},
	{ PSEUDO,				PAGE,	"PAGE"	},
	{ PSEUDO,				REGI,	"REG"	},
	{ NO_ARG,				0x83,	"RET"	},
	{ NO_ARG,				0x93,	"RETR"	},
	{ ONE_ARG + (A << 4) + 1,		0xe7,	"RL"	},
	{ ONE_ARG + (A << 4) + 1,		0xf7,	"RLC"	},
	{ ONE_ARG + (A << 4) + 1,		0x77,	"RR"	},
	{ ONE_ARG + (A << 4) + 1,		0x67,	"RRC"	},
	{ ONE_ARG + (MB0 << 4) + 4,		0xc5,	"SEL"	},
	{ PSEUDO,				SET,	"SET"	},
	{ ONE_ARG + (TCNT << 4) + 1,		0x65,	"STOP"	},
	{ ONE_ARG + (CNT << 4) + 2,		0x45,	"STRT"	},
	{ ONE_ARG + (A << 4) + 1,		0x47,	"SWAP"	},
	{ PSEUDO,				TITL,	"TITL"	},
	{ A_REG_1 + (R0 << 4) + 10,		0x20,	"XCH"	},
	{ A_REG_1 + (AT_R0 << 4) + 2,		0x30,	"XCHD"	},
	{ ADD_OP + (R0 << 4) + 10,		0xd0,	"XRL"	}
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
	{ UNARY  + UOP1  + OPR,		'$',		"$"	},
	{ REG,				A,		"A"	},
	{ BINARY + LOG1  + OPR,		AND,		"AND"	},
	{ REG,				BUS,		"BUS"	},
	{ REG,				C,		"C"	},
	{ REG,				CLK,		"CLK"	},
	{ REG,				CNT,		"CNT"	},
	{ BINARY + RELAT + OPR,		'=',		"EQ"	},
	{ REG,				F0,		"F0"	},
	{ REG,				F1,		"F1"	},
	{ BINARY + RELAT + OPR,		GE,		"GE"	},
	{ BINARY + RELAT + OPR,		'>',		"GT"	},
	{ UNARY  + UOP3  + OPR,		HIGH,		"HIGH"	},
	{ REG,				I,		"I"	},
	{ BINARY + RELAT + OPR,		LE,		"LE"	},
	{ UNARY  + UOP3  + OPR,		LOW,		"LOW"	},
	{ BINARY + RELAT + OPR,		'<',		"LT"	},
	{ REG,				MB0,		"MB0"	},
	{ REG,				MB1,		"MB1"	},
	{ BINARY + MULT  + OPR,		MOD,		"MOD"	},
	{ BINARY + RELAT + OPR,		NE,		"NE"	},
	{ UNARY  + UOP2  + OPR,		NOT,		"NOT"	},
	{ BINARY + LOG2  + OPR,		OR,		"OR"	},
	{ REG,				P1,		"P1"	},
	{ REG,				P2,		"P2"	},
	{ REG,				P4,		"P4"	},
	{ REG,				P5,		"P5"	},
	{ REG,				P6,		"P6"	},
	{ REG,				P7,		"P7"	},
	{ REG,				PSW,		"PSW"	},
	{ REG,				R0,		"R0"	},
	{ REG,				R1,		"R1"	},
	{ REG,				R2,		"R2"	},
	{ REG,				R3,		"R3"	},
	{ REG,				R4,		"R4"	},
	{ REG,				R5,		"R5"	},
	{ REG,				R6,		"R6"	},
	{ REG,				R7,		"R7"	},
	{ REG,				RB0,		"RB0"	},
	{ REG,				RB1,		"RB1"	},
	{ BINARY + MULT  + OPR,		SHL,		"SHL"	},
	{ BINARY + MULT  + OPR,		SHR,		"SHR"	},
	{ REG,				T,		"T"	},
	{ REG,				TCNT,		"TCNT"	},
	{ REG,				TCNTI,		"TCNTI"	},
	{ BINARY + LOG2  + OPR,		XOR,		"XOR"	}
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
    };
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

/*  Listing file close routine.  THe symbol table is appended to the	*/
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
	if ((sp -> attr & TYPE) == REG)
	    fprintf(list," R%01x   %-10s",sp -> valu - R0,sp -> sname);
	else fprintf(list,"%04x  %-10s",sp -> valu,sp -> sname);
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
	if (title[0]) { fprintf(list,"%s\n\n",title);  listleft -= 2; }
    }
    return;
}

/*  Buffer storage for hex output file.  This allows this module to	*/
/*  do all of the required buffering and record forming without the	*/
/*  main routine having to fool with it.				*/

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

    putc(digit[b >> 4],hex);
    putc(digit[b & 0x0f],hex);
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
);
    putb