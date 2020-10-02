/*
	HEADER:		CUG267;
	TITLE:		2650 Cross-Assembler (Portable);
	FILENAME:	A26UTIL.C;
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

This module contains the following utility packages:

	1)  symbol table building and searching

	2)  opcode and operator table searching

	3)  listing file output

	4)  hex file output

	5)  error flagging
*/

/*  Get global goodies:  */

#include "a26.h"

/*  Make sure that MSDOS compilers using the large memory model know	*/
/*  that calloc() returns pointer to char as an MSDOS far pointer is	*/
/*  NOT compatible with the int type as is usually the case.		*/

char *calloc();

/*  Get access to global mailboxes defined in A26.C:			*/

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
	{ PSEUDO,					ACON,	"ACON"	},
	{ COMMA + REG + INDIR + ABS_13 + INDEX + 3,	0x8c,	"ADDA"	},
	{ COMMA + REG + SIGN_8 + 2,			0x84,	"ADDI"	},
	{ COMMA + REG + INDIR + REL_7 + 2,		0x88,	"ADDR"	},
	{ REG + NONE + 1,				0x80,	"ADDZ"	},
	{ COMMA + REG + INDIR + ABS_13 + INDEX + 3,	0x4c,	"ANDA"	},
	{ COMMA + REG + SIGN_8 + 2,			0x44,	"ANDI"	},
	{ COMMA + REG + INDIR + REL_7 + 2,		0x48,	"ANDR"	},
	{ REG + NOT_R0 + NONE + 1,			0x40,	"ANDZ"	},
	{ COMMA + CC + NOT_CC3 + INDIR + ABS_15 + 3,	0x9c,	"BCFA"	},
	{ COMMA + CC + NOT_CC3 + INDIR + REL_7 + 2,	0x98,	"BCFR"	},
	{ COMMA + CC + INDIR + ABS_15 + 3,		0x1c,	"BCTA"	},
	{ COMMA + CC + INDIR + REL_7 + 2,		0x18,	"BCTR"	},
	{ COMMA + REG + INDIR + ABS_15 + 3,		0xfc,	"BDRA"	},
	{ COMMA + REG + INDIR + REL_7 + 2,		0xf8,	"BDRR"	},
	{ COMMA + REG + INDIR + ABS_15 + 3,		0xdc,	"BIRA"	},
	{ COMMA + REG + INDIR + REL_7 + 2,		0xd8,	"BIRR"	},
	{ COMMA + REG + INDIR + ABS_15 + 3,		0x5c,	"BRNA"	},
	{ COMMA + REG + INDIR + REL_7 + 2,		0x58,	"BRNR"	},
	{ COMMA + CC + NOT_CC3 + INDIR + ABS_15 + 3,	0xbc,	"BSFA"	},
	{ COMMA + CC + NOT_CC3 + INDIR + REL_7 + 2,	0xb8,	"BSFR"	},
	{ COMMA + REG + INDIR + ABS_15 + 3,		0x7c,	"BSNA"	},
	{ COMMA + REG + INDIR + REL_7 + 2,		0x78,	"BSNR"	},
	{ COMMA + CC + INDIR + ABS_15 + 3,		0x3c,	"BSTA"	},
	{ COMMA + CC + INDIR + REL_7 + 2,		0x38,	"BSTR"	},
	{ INDIR + ABS_15 + ONLY_R3 + 3,			0xbf,	"BSXA"	},
	{ INDIR + ABS_15 + ONLY_R3 + 3,			0x9f,	"BXA"	},
	{ COMMA + REG + INDIR + ABS_13 + INDEX + 3,	0xec,	"COMA"	},
	{ COMMA + REG + SIGN_8 + 2,			0xe4,	"COMI"	},
	{ COMMA + REG + INDIR + REL_7 + 2,		0xe8,	"COMR"	},
	{ REG + NONE + 1,				0xe0,	"COMZ"	},
	{ UNSGN_8 + 2,					0x75,	"CPSL"	},
	{ UNSGN_8 + 2,					0x74,	"CPSU"	},
	{ COMMA + REG + NONE + 1,			0x94,	"DAR"	},
	{ PSEUDO,					DATA,	"DATA"	},
	{ PSEUDO,					EJE,	"EJE"	},
	{ PSEUDO + ISIF,				ELSE,	"ELSE"	},
	{ PSEUDO,					END,	"END"	},
	{ PSEUDO + ISIF,				ENDI,	"ENDI"	},
	{ COMMA + REG + INDIR + ABS_13 + INDEX + 3,	0x2c,	"EORA"	},
	{ COMMA + REG + SIGN_8 + 2,			0x24,	"EORI"	},
	{ COMMA + REG + INDIR + REL_7 + 2,		0x28,	"EORR"	},
	{ REG + NONE + 1,				0x20,	"EORZ"	},
	{ PSEUDO,					EQU,	"EQU"	},
	{ NONE + 1,					0x40,	"HALT"	},
	{ PSEUDO + ISIF,				IF,	"IF"	},
	{ PSEUDO,					INCL,	"INCL"	},
	{ COMMA + REG + INDIR + ABS_13 + INDEX + 3,	0x6c,	"IORA"	},
	{ COMMA + REG + SIGN_8 + 2,			0x64,	"IORI"	},
	{ COMMA + REG + INDIR + REL_7 + 2,		0x68,	"IORR"	},
	{ REG + NONE + 1,				0x60,	"IORZ"	},
	{ COMMA + REG + INDIR + ABS_13 + INDEX + 3,	0x0c,	"LODA"	},
	{ COMMA + REG + SIGN_8 + 2,			0x04,	"LODI"	},
	{ COMMA + REG + INDIR + REL_7 + 2,		0x08,	"LODR"	},
	{ REG + ODD_R0 + NONE + 1,			0x00,	"LODZ"	},
	{ NONE + 1,					0x93,	"LPSL"	},
	{ NONE + 1,					0x92,	"LPSU"	},
	{ NONE + 1,					0xc0,	"NOP"	},
	{ PSEUDO,					ORG,	"ORG"	},
	{ UNSGN_8 + 2,					0x77,	"PPSL"	},
	{ UNSGN_8 + 2,					0x76,	"PPSU"	},
	{ COMMA + REG + NONE + 1,			0x30,	"REDC"	},
	{ COMMA + REG + NONE + 1,			0x70,	"REDD"	},
	{ COMMA + REG + UNSGN_8 + 2,			0x54,	"REDE"	},
	{ PSEUDO,					RES,	"RES"	},
	{ COMMA + CC + NONE + 1,			0x14,	"RETC"	},
	{ COMMA + CC + NONE + 1,			0x34,	"RETE"	},
	{ COMMA + REG + NONE + 1,			0xd0,	"RRL"	},
	{ COMMA + REG + NONE + 1,			0x50,	"RRR"	},
	{ PSEUDO,					SET,	"SET"	},
	{ NONE + 1,					0x13,	"SPSL"	},
	{ NONE + 1,					0x12,	"SPSU"	},
	{ COMMA + REG + INDIR + ABS_13 + INDEX + 3,	0xcc,	"STRA"	},
	{ COMMA + REG + INDIR + REL_7 + 2,		0xc8,	"STRR"	},
	{ REG + NOT_R0 + NONE + 1,			0xc0,	"STRZ"	},
	{ COMMA + REG + INDIR + ABS_13 + INDEX + 3,	0xac,	"SUBA"	},
	{ COMMA + REG + SIGN_8 + 2,			0xa4,	"SUBI"	},
	{ COMMA + REG + INDIR + REL_7 + 2,		0xa8,	"SUBR"	},
	{ REG + NONE + 1,				0xa0,	"SUBZ"	},
	{ PSEUDO,					TITL,	"TITL"	},
	{ COMMA + REG + UNSGN_8 + 2,			0xf4,	"TMI"	},
	{ UNSGN_8 + 2,					0xb5,	"TPSL"	},
	{ UNSGN_8 + 2,					0xb4,	"TPSU"	},
	{ COMMA + REG + NONE + 1,			0xb0,	"WRTC"	},
	{ COMMA + REG + NONE + 1,			0xf0,	"WRTD"	},
	{ COMMA + REG + UNSGN_8 + 2,			0xd4,	"WRTE"	},
	{ INDIR + ZREL_7 + 2,				0x9b,	"ZBRR"	},
	{ INDIR + ZREL_7 + 2,				0xbb,	"ZBSR"	}
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
	{ BINARY + LOG1  + OPR,				AND,	"AND"	},
	{ BINARY + RELAT + OPR,				'=',	"EQ"	},
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
	{ BINARY + MULT  + OPR,				SHL,	"SHL"	},
	{ BINARY + MULT  + OPR,				SHR,	"SHR"	},
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
	if (cnt == HEXSIZE || addr + cnt > 0x7fff) record(0);
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
	addr = a & 0x7fff;
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
