/*
	HEADER:		CUG242;
	TITLE:		68HC11 Cross-Assembler (Portable);
	FILENAME:	A611UTIL.C;
	VERSION:	0.1;
	DATE:		08/27/1988;
	SEE-ALSO:	A611.H
	AUTHORS:	William C. Colley III;
*/

/*
		     68HC11 Cross-Assembler in Portable C

		Copyright (c) 1985,1987 William C. Colley, III

This module contains the following utility packages:

	1)  symbol table building and searching

	2)  opcode and operator table searching

	3)  listing file output

	4)  hex file output

	5)  error flagging
*/

/*  Get global goodies:  */

#include "a611.h"

/*  Make sure that MSDOS compilers using the large memory model know	*/
/*  that calloc() returns pointer to char as an MSDOS far pointer is	*/
/*  NOT compatible with the int type as is usually the case.		*/

char *calloc();

/*  Get access to global mailboxes defined in A68.C:			*/

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
	{ 0,						0x1b,	"ABA"	},
	{ 0,						0x3a,	"ABX"	},
	{ 2,						0x3a,	"ABY"	},
	{ IMMED + DIR + EXT + INDX + 0,			0x89,	"ADCA"	},
	{ IMMED + DIR + EXT + INDX + 0,			0xc9,	"ADCB"	},
	{ IMMED + DIR + EXT + INDX + 0,			0x8b,	"ADDA"	},
	{ IMMED + DIR + EXT + INDX + 0,			0xcb,	"ADDB"	},
	{ IMM_16 + IMMED + DIR + EXT + INDX + 0,	0xc3,	"ADDD"	},
	{ IMMED + DIR + EXT + INDX + 0,			0x84,	"ANDA"	},
	{ IMMED + DIR + EXT + INDX + 0,			0xc4,	"ANDB"	},
	{ EXT + INDX + 0,				0x48,	"ASL"	},
	{ 0,						0x48,	"ASLA"	},
	{ 0,						0x58,	"ASLB"	},
	{ 0,						0x05,	"ASLD"	},
	{ EXT + INDX + 0,				0x47,	"ASR"	},
	{ 0,						0x47,	"ASRA"	},
	{ 0,						0x57,	"ASRB"	},
	{ REL + 0,					0x24,	"BCC"	},
	{ MASK + DIR + INDX + 0,			0x15,	"BCLR"	},
	{ REL + 0,					0x25,	"BCS"	},
	{ REL + 0,					0x27,	"BEQ"	},
	{ REL + 0,					0x2c,	"BGE"	},
	{ REL + 0,					0x2e,	"BGT"	},
	{ REL + 0,					0x22,	"BHI"	},
	{ REL + 0,					0x24,	"BHS"	},
	{ IMMED + DIR + EXT + INDX + 0,			0x85,	"BITA"	},
	{ IMMED + DIR + EXT + INDX + 0,			0xc5,	"BITB"	},
	{ REL + 0,					0x2f,	"BLE"	},
	{ REL + 0,					0x25,	"BLO"	},
	{ REL + 0,					0x23,	"BLS"	},
	{ REL + 0,					0x2d,	"BLT"	},
	{ REL + 0,					0x2b,	"BMI"	},
	{ REL + 0,					0x26,	"BNE"	},
	{ REL + 0,					0x2a,	"BPL"	},
	{ REL + 0,					0x20,	"BRA"	},
	{ REL + MASK + DIR + INDX + 0,			0x13,	"BRCLR"	},
	{ REL + 0,					0x21,	"BRN"	},
	{ REL + MASK + DIR + INDX + 0,			0x12,	"BRSET"	},
	{ MASK + DIR + INDX + 0,			0x14,	"BSET"	},
	{ REL + 0,					0x8d,	"BSR"	},
	{ REL + 0,					0x28,	"BVC"	},
	{ REL + 0,					0x29,	"BVS"	},
	{ 0,						0x11,	"CBA"	},
	{ 0,						0x0c,	"CLC"	},
	{ 0,						0x0e,	"CLI"	},
	{ EXT + INDX + 0,				0x4f,	"CLR"	},
	{ 0,						0x4f,	"CLRA"	},
	{ 0,						0x5f,	"CLRB"	},
	{ 0,						0x0a,	"CLV"	},
	{ IMMED + DIR + EXT + INDX + 0,			0x81,	"CMPA"	},
	{ IMMED + DIR + EXT + INDX + 0,			0xc1,	"CMPB"	},
	{ EXT + INDX + 0,				0x43,	"COM"	},
	{ 0,						0x43,	"COMA"	},
	{ 0,						0x53,	"COMB"	},
	{ IMM_16 + IMMED + DIR + EXT + INDX + 3,	0x83,	"CPD"	},
	{ IMM_16 + IMMED + DIR + EXT + INDX + 1,	0x8c,	"CPX"	},
	{ IMM_16 + IMMED + DIR + EXT + INDX + 2,	0x8c,	"CPY"	},
	{ 0,						0x19,	"DAA"	},
	{ EXT + INDX + 0,				0x4a,	"DEC"	},
	{ 0,						0x4a,	"DECA"	},
	{ 0,						0x5a,	"DECB"	},
	{ 0,						0x34,	"DES"	},
	{ 0,						0x09,	"DEX"	},
	{ 2,						0x09,	"DEY"	},
	{ PSEUDO + ISIF,				ELSE,	"ELSE"	},
	{ PSEUDO,					END,	"END"	},
	{ PSEUDO + ISIF,				ENDIF,	"ENDIF"	},
	{ IMMED + DIR + EXT + INDX + 0,			0x88,	"EORA"	},
	{ IMMED + DIR + EXT + INDX + 0,			0xc8,	"EORB"	},
	{ PSEUDO,					EQU,	"EQU"	},
	{ PSEUDO,					FCB,	"FCB"	},
	{ PSEUDO,					FCC,	"FCC"	},
	{ PSEUDO,					FDB,	"FDB"	},
	{ 0,						0x03,	"FDIV"	},
	{ 0,						0x02,	"IDIV"	},
	{ PSEUDO + ISIF,				IF,	"IF"	},
	{ EXT + INDX + 0,				0x4c,	"INC"	},
	{ 0,						0x4c,	"INCA"	},
	{ 0,						0x5c,	"INCB"	},
	{ PSEUDO,					INCL,	"INCL"	},
	{ 0,						0x31,	"INS"	},
	{ 0,						0x08,	"INX"	},
	{ 2,						0x08,	"INY"	},
	{ EXT + INDX + 0,				0x4e,	"JMP"	},
	{ DIR + EXT + INDX + 0,				0x8d,	"JSR"	},
	{ IMMED + DIR + EXT + INDX + 0,			0x86,	"LDAA"	},
	{ IMMED + DIR + EXT + INDX + 0,			0xc6,	"LDAB"	},
	{ IMM_16 + IMMED + DIR + EXT + INDX + 0,	0xcc,	"LDD"	},
	{ IMM_16 + IMMED + DIR + EXT + INDX + 0,	0x8e,	"LDS"	},
	{ IMM_16 + IMMED + DIR + EXT + INDX + 1,	0xce,	"LDX"	},
	{ IMM_16 + IMMED + DIR + EXT + INDX + 2,	0xce,	"LDY"	},
	{ EXT + INDX + 0,				0x48,	"LSL"	},
	{ 0,						0x48,	"LSLA"	},
	{ 0,						0x58,	"LSLB"	},
	{ 0,						0x05,	"LSLD"	},
	{ EXT + INDX + 0,				0x44,	"LSR"	},
	{ 0,						0x44,	"LSRA"	},
	{ 0,						0x54,	"LSRB"	},
	{ 0,						0x04,	"LSRD"	},
	{ 0,						0x3d,	"MUL"	},
	{ EXT + INDX + 0,				0x40,	"NEG"	},
	{ 0,						0x40,	"NEGA"	},
	{ 0,						0x50,	"NEGB"	},
	{ 0,						0x01,	"NOP"	},
	{ IMMED + DIR + EXT + INDX + 0,			0x8a,	"ORAA"	},
	{ IMMED + DIR + EXT + INDX + 0,			0xca,	"ORAB"	},
	{ PSEUDO,					ORG,	"ORG"	},
	{ PSEUDO,					PAGE,	"PAGE"	},
	{ 0,						0x36,	"PSHA"	},
	{ 0,						0x37,	"PSHB"	},
	{ 0,						0x3c,	"PSHX"	},
	{ 2,						0x3c,	"PSHY"	},
	{ 0,						0x32,	"PULA"	},
	{ 0,						0x33,	"PULB"	},
	{ 0,						0x38,	"PULX"	},
	{ 2,						0x38,	"PULY"	},
	{ PSEUDO,					RMB,	"RMB"	},
	{ EXT + INDX + 0,				0x49,	"ROL"	},
	{ 0,						0x49,	"ROLA"	},
	{ 0,						0x59,	"ROLB"	},
	{ EXT + INDX + 0,				0x46,	"ROR"	},
	{ 0,						0x46,	"RORA"	},
	{ 0,						0x56,	"RORB"	},
	{ 0,						0x3b,	"RTI"	},
	{ 0,						0x39,	"RTS"	},
	{ 0,						0x10,	"SBA"	},
	{ IMMED + DIR + EXT + INDX + 0,			0x82,	"SBCA"	},
	{ IMMED + DIR + EXT + INDX + 0,			0xc2,	"SBCB"	},
	{ 0,						0x0d,	"SEC"	},
	{ 0,						0x0f,	"SEI"	},
	{ PSEUDO,					SET,	"SET"	},
	{ 0,						0x0b,	"SEV"	},
	{ DIR + EXT + INDX + 0,				0x87,	"STAA"	},
	{ DIR + EXT + INDX + 0,				0xc7,	"STAB"	},
	{ DIR + EXT + INDX + 0,				0xcd,	"STD"	},
	{ 0,						0xcf,	"STOP"	},
	{ DIR + EXT + INDX + 0,				0x8f,	"STS"	},
	{ DIR + EXT + INDX + 1,				0xcf,	"STX"	},
	{ DIR + EXT + INDX + 2,				0xcf,	"STY"	},
	{ IMMED + DIR + EXT + INDX + 0,			0x80,	"SUBA"	},
	{ IMMED + DIR + EXT + INDX + 0,			0xc0,	"SUBB"	},
	{ IMM_16 + IMMED + DIR + EXT + INDX + 0,	0x83,	"SUBD"	},
	{ 0,						0x3f,	"SWI"	},
	{ 0,						0x16,	"TAB"	},
	{ 0,						0x06,	"TAP"	},
	{ 0,						0x17,	"TBA"	},
	{ 0,						0x00,	"TEST"	},
	{ PSEUDO,					TITLE,	"TITLE"	},
	{ 0,						0x07,	"TPA"	},
	{ EXT + INDX + 0,				0x4d,	"TST"	},
	{ 0,						0x4d,	"TSTA"	},
	{ 0,						0x5d,	"TSTB"	},
	{ 0,						0x30,	"TSX"	},
	{ 2,						0x30,	"TSY"	},
	{ 0,						0x35,	"TXS"	},
	{ 2,						0x35,	"TYS"	},
	{ 0,						0x3e,	"WAI"	},
	{ 0,						0x8f,	"XGDX"	},
	{ 2,						0x8f,	"XGDY"	}
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
	    fprintf(list,"%c ",errcode);
	    if (listhex) {
		fprintf(list,"%04x ",address);
		for (j = 5; j; --j) {
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
