/*
	HEADER:		CUG276;
	TITLE:		PIC1650 Cross-Assembler (Portable);
	FILENAME:	A16UTIL.C;
	VERSION:	0.0;
	DATE:		06/0151988;
	SEE-ALSO:	A16.H;
	AUTHORS:	William C. Colley III;
*/

/*
		     PIC1650 Cross-Assembler in Portable C

	     Copyright (c) 1985,1987,1989 William C. Colley, III

Revision History:

Ver	Date		Description

0.0	JUNE 1989	Derived from version 0.2 of my portable 8085 cross-
			assembler.  WCC3.

This module contains the following utility packages:

	1)  symbol table building and searching

	2)  opcode and operator table searching

	3)  listing file output

	4)  hex file output

	5)  error flagging
*/

/*  Get global goodies:  */

#include "a16.h"

/*  Make sure that MSDOS compilers using the large memory model know	*/
/*  that calloc() returns pointer to char as an MSDOS far pointer is	*/
/*  NOT compatible with the int type as is usually the case.		*/

char *calloc();

/*  Get access to global mailboxes defined in A16.C:			*/

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
	{ REG_5 + DIR + 1,			0x1c0,	"ADDWF"	    },
	{ IMMED_8 + 1,				0xe00,	"ANDLW"	    },
	{ REG_5 + DIR + 1,			0x140,	"ANDWF"	    },
	{ ADDR_9 + 1,				0xa00,	"B"	    },
	{ ADDR_9 + 2,				0x603,	"BC"	    },
	{ REG_5 + BIT_3 + 1,			0x400,	"BCF"	    },
	{ ADDR_9 + 2,				0x623,	"BDC"	    },
	{ ADDR_9 + 2,				0x703,	"BNC"	    },
	{ ADDR_9 + 2,				0x723,	"BNDC"	    },
	{ ADDR_9 + 2,				0x743,	"BNZ"	    },
	{ REG_5 + BIT_3 + 1,			0x500,	"BSF"	    },
	{ REG_5 + BIT_3 + 1,			0x600,	"BTFSC"	    },
	{ REG_5 + BIT_3 + 1,			0x700,	"BTFSS"	    },
	{ ADDR_9 + 2,				0x643,	"BZ"	    },
	{ ADDR_8 + 1,				0x900,	"CALL"	    },
	{ NONE + 1,				0x403,	"CLRC"	    },
	{ NONE + 1,				0x423,	"CLRDC"	    },
	{ REG_5 + 1,				0x060,	"CLRF"	    },
	{ NONE + 1,				0x040,	"CLRW"	    },
	{ NONE + 1,				0x004,	"CLRWDT"    },
	{ NONE + 1,				0x443,	"CLRZ"	    },
	{ REG_5 + DIR + 1,			0x240,	"COMF"	    },
	{ REG_5 + DIR + 1,			0x0c0,	"DECF"	    },
	{ REG_5 + DIR + 1,			0x2c0,	"DECFSZ"    },
	{ PSEUDO,				DS,	"DS"	    },
	{ PSEUDO,				DW,	"DW"	    },
	{ PSEUDO + ISIF,			ELSE,	"ELSE"	    },
	{ PSEUDO,				END,	"END"	    },
	{ PSEUDO + ISIF,			ENDIF,	"ENDIF"	    },
	{ PSEUDO,				EQU,	"EQU"	    },
	{ ADDR_9 + 1,				0xa00,	"GOTO"	    },
	{ PSEUDO + ISIF,			IF,	"IF"	    },
	{ REG_5 + DIR + 1,			0x280,	"INCF"	    },
	{ REG_5 + DIR + 1,			0x3c0,	"INCFSZ"    },
	{ PSEUDO,				INCL,	"INCL"	    },
	{ IMMED_8 + 1,				0xd00,	"IORLW"	    },
	{ REG_5 + DIR + 1,			0x100,	"IORWF"	    },
	{ REG_5 + DIR + 1,			0x200,	"MOVF"	    },
	{ REG_5 + 1,				0x200,	"MOVFW"	    },
	{ IMMED_8 + 1,				0xc00,	"MOVLW"	    },
	{ REG_5 + 1,				0x020,	"MOVWF"	    },
	{ NONE + 1,				0x000,	"NOP"	    },
	{ NONE + 1,				0x002,	"OPTION"    },
	{ PSEUDO,				ORG,	"ORG"	    },
	{ PSEUDO,				PAGE,	"PAGE"	    },
	{ IMMED_8 + 1,				0x800,	"RETLW"	    },
	{ REG_5 + DIR + 1,			0x340,	"RLF"	    },
	{ REG_5 + DIR + 1,			0x300,	"RRF"	    },
	{ PSEUDO,				SET,	"SET"	    },
	{ NONE + 1,				0x503,	"SETC"	    },
	{ NONE + 1,				0x523,	"SETDC"	    },
	{ NONE + 1,				0x543,	"SETZ"	    },
	{ NONE + 1,				0x703,	"SKPC"	    },
	{ NONE + 1,				0x723,	"SKPDC"	    },
	{ NONE + 1,				0x603,	"SKPNC"	    },
	{ NONE + 1,				0x623,	"SKPNDC"    },
	{ NONE + 1,				0x643,	"SKPNZ"	    },
	{ NONE + 1,				0x743,	"SKPZ"	    },
	{ NONE + 1,				0x003,	"SLEEP"	    },
	{ REG_5 + DIR + 1,			0x080,	"SUBWF"	    },
	{ REG_5 + DIR + 1,			0x380,	"SWAPF"	    },
	{ PSEUDO,				TITLE,	"TITLE"	    },
	{ TRIS_REG + 1,				0x000,	"TRIS"	    },
	{ REG_5 + 1,				0x220,	"TSTF"	    },
	{ IMMED_8 + 1,				0xf00,	"XORLW"	    },
	{ REG_5 + DIR + 1,			0x180,	"XORWF"	    }
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
	{ VAL,						0,	"@FSR"	},
	{ BINARY + LOG1  + OPR,				AND,	"AND"	},
	{ BINARY + RELAT + OPR,				'=',	"EQ"	},
	{ VAL,						1,	"F"	},
	{ VAL,						0,	"F0"	},
	{ VAL,						1,	"F1"	},
	{ VAL,						10,	"F10"	},
	{ VAL,						11,	"F11"	},
	{ VAL,						12,	"F12"	},
	{ VAL,						13,	"F13"	},
	{ VAL,						14,	"F14"	},
	{ VAL,						15,	"F15"	},
	{ VAL,						16,	"F16"	},
	{ VAL,						17,	"F17"	},
	{ VAL,						18,	"F18"	},
	{ VAL,						19,	"F19"	},
	{ VAL,						2,	"F2"	},
	{ VAL,						20,	"F20"	},
	{ VAL,						21,	"F21"	},
	{ VAL,						22,	"F22"	},
	{ VAL,						23,	"F23"	},
	{ VAL,						24,	"F24"	},
	{ VAL,						25,	"F25"	},
	{ VAL,						26,	"F26"	},
	{ VAL,						27,	"F27"	},
	{ VAL,						28,	"F28"	},
	{ VAL,						29,	"F29"	},
	{ VAL,						3,	"F3"	},
	{ VAL,						30,	"F30"	},
	{ VAL,						31,	"F31"	},
	{ VAL,						4,	"F4"	},
	{ VAL,						5,	"F5"	},
	{ VAL,						6,	"F6"	},
	{ VAL,						7,	"F7"	},
	{ VAL,						8,	"F8"	},
	{ VAL,						9,	"F9"	},
	{ VAL,						4,	"FSR"	},
	{ BINARY + RELAT + OPR,				GE,	"GE"	},
	{ BINARY + RELAT + OPR,				'>',	"GT"	},
	{ UNARY  + UOP3  + OPR,				HIGH,	"HIGH"	},
	{ BINARY + RELAT + OPR,				LE,	"LE"	},
	{ UNARY  + UOP3  + OPR,				LOW,	"LOW"	},
	{ BINARY + RELAT + OPR,				'<',	"LT"	},
	{ BINARY + MULT  + OPR,				MOD,	"MOD"	},
	{ BINARY + RELAT + OPR,				NE,	"NE"	},
	{ UNARY  + UOP2  + OPR,				NOT,	"NOT"	},
	{ BINARY + LOG2  + OPR,				OR,	"OR"	},
	{ VAL,						2,	"PC"	},
	{ VAL,						5,	"PORTA"	},
	{ VAL,						6,	"PORTB"	},
	{ VAL,						7,	"PORTC"	},
	{ VAL,						1,	"RTCC"	},
	{ BINARY + MULT  + OPR,				SHL,	"SHL"	},
	{ BINARY + MULT  + OPR,				SHR,	"SHR"	},
	{ VAL,						3,	"SW"	},
	{ VAL,						0,	"W"	},
	{ BINARY + LOG2  + OPR,				XOR,	"XOR"	}
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
		fprintf(list,"%04x   ",address);
		for (j = 3; j; --j) {
		    if (i) { --i;  ++address;  fprintf(list,"%03x ",*o++); }
		    else fprintf(list,"    ");
		}
	    }
	    else fprintf(list,"%19s","");
	    fprintf(list,"  %s",line);  strcpy(line,"\n");
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
	buf[cnt++] = high(c);  buf[cnt++] = low(c);
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
	addr = a << 1;
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
