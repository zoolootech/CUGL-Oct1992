/*
	HEADER:		CUG242;
	TITLE:		8051 Cross-Assembler (Portable);
	FILENAME:	A51UTIL.C;
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

This module contains the following utility packages:

	1)  symbol table building and searching

	2)  opcode and operator table searching

	3)  listing file output

	4)  hex file output

	5)  error flagging
*/

/*  Get global goodies:  */

#include "a51.h"

/*  Make sure that MSDOS compilers using the large memory model know	*/
/*  that calloc() returns pointer to char as an MSDOS far pointer is	*/
/*  NOT compatible with the int type as is usually the case.		*/

char *calloc();

/*  Get access to global mailboxes defined in A51.C:			*/

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
	{ AJMP,					0x11,	"ACALL"	},
	{ ADD + ((R7 - A) << 5) + A,		0x24,	"ADD"	},
	{ ADD + ((R7 - A) << 5) + A,		0x34,	"ADDC"	},
	{ AJMP,					0x01,	"AJMP"	},
	{ ORL + ((R7 - AT_R0) << 5) + AT_R0,	0x54,	"ANL"	},
	{ PSEUDO,				BIT,	"BIT"	},
	{ CJNE,					0xb4,	"CJNE"	},
	{ CPL,					0xc3,	"CLR"	},
	{ CPL,					0xb3,	"CPL"	},
	{ SWAP + A,				0xd4,	"DA"	},
	{ PSEUDO,				DB,	"DB"	},
	{ DEC + ((R7 - A) << 5) + A,		0x14,	"DEC"	},
	{ SWAP + AB,				0x74,	"DIV"	},
	{ DJNZ + ((R7 - R0) << 5) + R0,		0xd4,	"DJNZ"	},
	{ PSEUDO,				DS,	"DS"	},
	{ PSEUDO,				DW,	"DW"	},
	{ PSEUDO + ISIF,			ELSE,	"ELSE"	},
	{ PSEUDO,				END,	"END"	},
	{ PSEUDO + ISIF,			ENDIF,	"ENDIF"	},
	{ PSEUDO,				EQU,	"EQU"	},
	{ PSEUDO + ISIF,			IF,	"IF"	},
	{ INC + ((R7 - A) << 5) + A,		0x04,	"INC"	},
	{ PSEUDO,				INCL,	"INCL"	},
	{ JBIT,					0x20,	"JB"	},
	{ JBIT,					0x10,	"JBC"	},
	{ SJMP,					0x40,	"JC"	},
	{ JMP + DPTR,				0x73,	"JMP"	},
	{ JBIT,					0x30,	"JNB"	},
	{ SJMP,					0x50,	"JNC"	},
	{ SJMP,					0x70,	"JNZ"	},
	{ SJMP,					0x60,	"JZ"	},
	{ LJMP,					0x12,	"LCALL"	},
	{ LJMP,					0x02,	"LJMP"	},
	{ MOV + ((R7 - AT_R0) << 5) + AT_R0,	0,	"MOV"	},
	{ MOVC + ((DPTR - PC) << 5) + PC,	0x93,	"MOVC"	},
	{ MOVX + ((AT_R1-AT_R0) << 5) + AT_R0,	0xe0,	"MOVX"	},
	{ SWAP + AB,				0x94,	"MUL"	},
	{ RET,					0x00,	"NOP"	},
	{ PSEUDO,				ORG,	"ORG"	},
	{ ORL + ((R7 - AT_R0) << 5) + AT_R0,	0x44,	"ORL"	},
	{ PSEUDO,				PAGE,	"PAGE"	},
	{ POP,					0xd0,	"POP"	},
	{ POP,					0xc0,	"PUSH"	},
	{ PSEUDO,				REGI,	"REG"	},
	{ RET,					0x22,	"RET"	},
	{ RET,					0x32,	"RETI"	},
	{ SWAP + A,				0x23,	"RL"	},
	{ SWAP + A,				0x33,	"RLC"	},
	{ SWAP + A,				0x03,	"RR"	},
	{ SWAP + A,				0x13,	"RRC"	},
	{ PSEUDO,				SET,	"SET"	},
	{ SETB,					0xd3,	"SETB"	},
	{ SJMP,					0x80,	"SJMP"	},
	{ ADD + ((R7 - A) << 5) + A,		0x94,	"SUBB"	},
	{ SWAP + A,				0xc4,	"SWAP"	},
	{ PSEUDO,				TITL,	"TITL"	},
	{ XCH + ((R7 - AT_R0) << 5) + AT_R0,	0xc4,	"XCH"	},
	{ XCHD + ((AT_R1-AT_R0) << 5) + AT_R0,	0xd4,	"XCHD"	},
	{ XRL + ((R7 - AT_R0) << 5) + AT_R0,	0x64,	"XRL"	}
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
	{ UNARY  + UOP1  + OPR,		'$',		"$"		},
	{ REG,				A,		"A"		},
	{ REG,				AB,		"AB"		},
	{ BVAL,				0xd6,		"AC"		},
	{ VAL,				0xe0,		"ACC"		},
	{ BINARY + LOG1  + OPR,		AND,		"AND"		},
	{ VAL,				0xf0,		"B"		},
	{ REG,				C,		"C"		},
	{ BVAL,				0xd7,		"CY"		},
	{ VAL,				0x83,		"DPH"		},
	{ VAL,				0x82,		"DPL"		},
	{ REG,				DPTR,		"DPTR"		},
	{ BVAL,				0xaf,		"EA"		},
	{ BINARY + RELAT + OPR,		'=',		"EQ"		},
	{ BVAL,				0xac,		"ES"		},
	{ BVAL,				0xa9,		"ET0"		},
	{ BVAL,				0xab,		"ET1"		},
	{ BVAL,				0xa8,		"EX0"		},
	{ BVAL,				0xaa,		"EX1"		},
	{ BVAL,				0xd5,		"F0"		},
	{ BINARY + RELAT + OPR,		GE,		"GE"		},
	{ BINARY + RELAT + OPR,		'>',		"GT"		},
	{ UNARY  + UOP3  + OPR,		HIGH,		"HIGH"		},
	{ VAL,				0xa8,		"IE"		},
	{ BVAL,				0x89,		"IE0"		},
	{ BVAL,				0x8b,		"IE1"		},
	{ VAL,				0xb8,		"IP"		},
	{ BVAL,				0x88,		"IT0"		},
	{ BVAL,				0x8a,		"IT1"		},
	{ BINARY + RELAT + OPR,		LE,		"LE"		},
	{ UNARY  + UOP3  + OPR,		LOW,		"LOW"		},
	{ BINARY + RELAT + OPR,		'<',		"LT"		},
	{ BINARY + MULT  + OPR,		MOD,		"MOD"		},
	{ BINARY + RELAT + OPR,		NE,		"NE"		},
	{ UNARY  + UOP2  + OPR,		NOT,		"NOT"		},
	{ BINARY + LOG2  + OPR,		OR,		"OR"		},
	{ BVAL,				0xd2,		"OV"		},
	{ BVAL,				0xd0,		"P"		},
	{ VAL,				0x80,		"P0"		},
	{ VAL,				0x90,		"P1"		},
	{ VAL,				0xa0,		"P2"		},
	{ VAL,				0xb0,		"P3"		},
	{ REG,				PC,		"PC"		},
	{ VAL,				0x87,		"PCON"		},
	{ VAL,				0xd0,		"PSW"		},
	{ BVAL,				0xb9,		"PT0"		},
	{ BVAL,				0xbb,		"PT1"		},
	{ BVAL,				0xb8,		"PX0"		},
	{ BVAL,				0xba,		"PX1"		},
	{ REG,				R0,		"R0"		},
	{ REG,				R1,		"R1"		},
	{ REG,				R2,		"R2"		},
	{ REG,				R3,		"R3"		},
	{ REG,				R4,		"R4"		},
	{ REG,				R5,		"R5"		},
	{ REG,				R6,		"R6"		},
	{ REG,				R7,		"R7"		},
	{ BVAL,				0x9a,		"RB8"		},
	{ BVAL,				0x9c,		"REN"		},
	{ BVAL,				0x98,		"RI"		},
	{ BVAL,				0xd3,		"RS0"		},
	{ BVAL,				0xd4,		"RS1"		},
	{ VAL,				0x99,		"SBUF"		},
	{ VAL,				0x98,		"SCON"		},
	{ BINARY + MULT  + OPR,		SHL,		"SHL"		},
	{ BINARY + MULT  + OPR,		SHR,		"SHR"		},
	{ BVAL,				0x9f,		"SM0"		},
	{ BVAL,				0x9e,		"SM1"		},
	{ BVAL,				0x9d,		"SM2"		},
	{ VAL,				0x81,		"SP"		},
	{ BVAL,				0x9b,		"TB8"		},
	{ VAL,				0x88,		"TCON"		},
	{ BVAL,				0x8d,		"TF0"		},
	{ BVAL,				0x8f,		"TF1"		},
	{ VAL,				0x8c,		"TH0"		},
	{ VAL,				0x8d,		"TH1"		},
	{ BVAL,				0x99,		"TI"		},
	{ VAL,				0x8a,		"TL0"		},
	{ VAL,				0x8b,		"TL1"		},
	{ VAL,				0x89,		"TMOD"		},
	{ BVAL,				0x8c,		"TR0"		},
	{ BVAL,				0x8e,		"TR1"		},
	{ BINARY + LOG2  + OPR,		XOR,		"XOR"		}
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
    putb(low(-sum));  putc('\n',hex);

    ad