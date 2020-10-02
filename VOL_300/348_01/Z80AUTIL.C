/*

This module contains the following utility packages:

	1)  symbol table building and searching

	2)  opcode and operator table searching

	3)  listing file output

	4)  hex file output

	5)  error flagging
*/

/*  Get global goodies:  */

#include "z80a.h"

/*  Make sure that MSDOS compilers using the large memory model know	*/
/*  that calloc() returns pointer to char as an MSDOS far pointer is	*/
/*  NOT compatible with the int type as is usually the case.		*/

char *calloc();

/*  Get access to global mailboxes defined in Z80F.C:			*/

extern char errcode, line[], title[], sub_tit[];
extern int eject, listhex, code, listok;
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
/*  NOTE: THIS TABLE MUST BE IN ALPHA ORDER!				*/

OPCODE *find_code(nam)
char *nam;
{
    OPCODE *bsearch();

    static OPCODE opctbl[] = {
	{ S_OP + INDEX,			0x88,	"ADC"	},
	{ ONE_ARG_L2,			0xED4A,	"ADCHL"	},
	{ ONE_ARG,			0xCE,	"ADCI"	},
	{ S_OP + INDEX,			0x80,	"ADD"	},
	{ L_OP,				0x09,	"ADDHL"	},
	{ ONE_ARG,			0xC6,	"ADDI"	},
	{ ONE_ARG_L2,			0xDD09,	"ADDIX"	},
	{ ONE_ARG_L2,			0xFD09,	"ADDIY"	},
	{ S_OP + INDEX,			0xA0,	"AND"	},
	{ ONE_ARG,			0xE6,	"ANDI"	},
	{ BIT + INDEX,			0xCB40,	"BIT"	},
	{ L_ARG2,			0xCD,	"CALL"	},
	{ CALL,				0xCD,	"CALLC"	},
	{ NO_ARG,			0x3F,	"CCF"	},
	{ S_OP + INDEX,			0xB8,	"CMP"	},
	{ ONE_ARG,			0xFE,	"CMPI"	},
	{ PSEUDO,			CODE,	"CODE"	},
	{ S_OP + INDEX,			0xB8,	"CP"	},
	{ NO_ARG_L,			0xEDA9,	"CPDEC"	},
	{ NO_ARG_L,			0xEDB9,	"CPDR"	},
	{ NO_ARG_L,			0xEDA1,	"CPINC"	},
	{ NO_ARG_L,			0xEDB1,	"CPIR"	},
	{ NO_ARG,			0x2F,	"CPL"	},
	{ PSEUDO,			CSECT,	"CSECT"	},
	{ PSEUDO,			DS,	"DA"	},
	{ NO_ARG,			0x27,	"DAA"	},
	{ PSEUDO,			DW,	"DAC"	},
	{ PSEUDO,			DB,	"DATA"	},
	{ PSEUDO,			DB,	"DBC"	},
	{ PSEUDO,			DB,	"DC"	},
	{ S_OP + 0x0300 + INDEX,	0x05,	"DEC"	},
	{ L_OP + INDEX,			0x0B,	"DEC16"	},
	{ NO_ARG,			0xF3,	"DI"	},
	{ JREL,				0x10,	"DJNZ"	},
	{ NO_ARG,			0xFB,	"EI"	},
	{ PSEUDO + ISIF,		ELSE,	"ELS"	},
	{ PSEUDO,			END,	"END"	},
	{ PSEUDO + ISIF,		ENDIF,	"ENF"	},
	{ PSEUDO,			EQU,	"EQU"	},
	{ NO_ARG,			0x08,	"EXAFAF"},
	{ NO_ARG,			0xEB,	"EXDEHL"},
	{ NO_ARG,			0xE3,	"EXSPHL"},
	{ NO_ARG_L,			0xDDE3,	"EXSPIX"},
	{ NO_ARG_L,			0xFDE3,	"EXSPIY"},
	{ NO_ARG,			0xD9,	"EXX"	},
	{ NO_ARG,			0x76,	"HALT"	},
	{ PSEUDO + ISIF,		IF,	"IF"	},
	{ NO_ARG_L,			0xED46,	"IM0"	},
	{ NO_ARG_L,			0xED56,	"IM1"	},
	{ NO_ARG_L,			0xED5E,	"IM2"	},
	{ ONE_ARG,			0xDB,	"IN"	},
	{ S_OP + 0x0300 + INDEX,	0x04,	"INC"	},
	{ L_OP + INDEX,			0x03,	"INC16"	},
	{ NO_ARG_L,			0xEDAA,	"INDEC"	},
	{ NO_ARG_L,			0xEDBA,	"INDR"	},
	{ NO_ARG_L,			0xEDA2,	"ININC"	},
	{ NO_ARG_L,			0xEDB2,	"INIR"	},
	{ XIO,				0xED40,	"INPUT"	},
	{ JUMP,				0xC3,	"JP"	},
	{ JREL,				0x18,	"JR"	},
	{ JREL,				0x18,	"JREL"	},
	{ L_ARG2,			0xC3,	"JUMP"	},
	{ ONE_ARG_L + INDEX,		0xE9,	"JUMPI"	},
	{ MOV + INDEX,			0x40,	"LD"	},
	{ L_ARG_L + INDEX,		0x2A,	"LD16"	},
	{ L_ARG2,			0x3A,	"LDA"	},
	{ NO_ARG,			0x0A,	"LDABC"	},
	{ NO_ARG,			0x1A,	"LDADE"	},
	{ NO_ARG_L,			0xED57,	"LDAI"	},
	{ NO_ARG_L,			0xED5F,	"LDAR"	},
	{ NO_ARG,			0x02,	"LDBCA"	},
	{ NO_ARG,			0x12,	"LDDEA"	},
	{ NO_ARG_L,			0xEDA8,	"LDDEC"	},
	{ NO_ARG_L,			0xEDB8,	"LDDR"	},
	{ ONE_ARG + 0x0300 + INDEX,	0x06,	"LDI"	},
	{ L_ARG + INDEX,		0x01,	"LDI16"	},
	{ NO_ARG_L,			0xED47,	"LDIA"	},
	{ NO_ARG_L,			0xEDA0,	"LDINC"	},
	{ NO_ARG_L,			0xEDB0,	"LDIR"	},
	{ NO_ARG_L,			0xED4F,	"LDRA"	},
	{ ONE_ARG_L + INDEX,		0xF9,	"LDSP"	},
	{ PSEUDO,			LIST,	"LIST"	},
	{ PSEUDO,			NCODE,	"NCODE"	},
	{ NO_ARG_L,			0xED44,	"NEG"	},
	{ NO_ARG,			0x00,	"NOP"	},
	{ S_OP + INDEX,			0xB0,	"OR"	},
	{ PSEUDO,			ORG,	"ORG"	},
	{ ONE_ARG,			0xF6,	"ORI"	},
	{ ONE_ARG,			0xD3,	"OUT"	},
	{ NO_ARG_L,			0xEDAB,	"OUTDEC"},
	{ NO_ARG_L,			0xEDBB,	"OUTDR"	},
	{ NO_ARG_L,			0xEDA3,	"OUTINC"},
	{ NO_ARG_L,			0xEDB3,	"OUTIR"	},
	{ XIO,				0xED41,	"OUTPUT"},
	{ PSEUDO,			PAGE,	"PAG"	},
	{ PSEUDO,			PAGT,	"PAGT"	},
	{ L_OP + INDEX,			0xC1,	"POP"	},
	{ L_OP + INDEX,			0xC5,	"PUSH"	},
	{ BIT + INDEX,			0xCB80,	"RES"	},
	{ RETURN,			0xC9,	"RET"	},
	{ NO_ARG_L,			0xED4D,	"RETI"	},
	{ NO_ARG_L,			0xED45,	"RETN"	},
	{ NO_ARG,			0xC9,	"RETURN"},
	{ ROTATE + INDEX,		0xCB10,	"RL"	},
	{ NO_ARG,			0x17,	"RLA"	},
	{ PSEUDO,			RLC,	"RLC"	},
	{ NO_ARG,			0x07,	"RLCA"	},
	{ NO_ARG_L,			0xED6F,	"RLD"	},
	{ ROTATE + INDEX,		0xCB00,	"ROLC"	},
	{ ROTATE + INDEX,		0xCB18,	"RR"	},
	{ NO_ARG,			0x1F,	"RRA"	},
	{ ROTATE + INDEX,		0xCB08,	"RRC"	},
	{ NO_ARG,			0x0F,	"RRCA"	},
	{ NO_ARG_L,			0xED67,	"RRD"	},
	{ RESTART,			0xC7,	"RST"	},
	{ S_OP + INDEX,			0x98,	"SBC"	},
	{ ONE_ARG_L2,			0xED42,	"SBCHL"	},
	{ ONE_ARG,			0xDE,	"SBCI"	},
	{ NO_ARG,			0x37,	"SCF"	},
	{ PSEUDO,			SET,	"SET" 	},
	{ BIT + INDEX,			0xCBC0,	"SETB"	},
	{ PSEUDO,			SKP,	"SKP"   },
	{ ROTATE + INDEX,		0xCB40,	"SLA"	},
	{ ROTATE + INDEX,		0xCB48,	"SRA"	},
	{ ROTATE + INDEX,		0xCB38,	"SRL"	},
	{ L_ARG_L2 + INDEX,		0x22,	"ST16"	},
	{ L_ARG2,			0x32,	"STA"	},
	{ S_OP + INDEX,			0x90,	"SUB"	},
	{ ONE_ARG,			0xD6,	"SUBI"	},
	{ PSEUDO,			TITL,	"TTL"	},
	{ PSEUDO,			TYP,	"TYP"   },
	{ PSEUDO,			ULIST,	"ULIST" },
	{ PSEUDO,			URLC,	"URLC"  },
	{ PSEUDO,			INCL,	"USE"   },
	{ S_OP + INDEX,			0xA8,	"XOR"	},
	{ ONE_ARG,			0xEE,	"XORI"	}
    };

    return bsearch(opctbl,opctbl + (sizeof(opctbl) / sizeof(OPCODE)),nam);
}

/*  Operator table search routine.  This routine pats down the		*/
/*  operator table for a given operator and returns either a pointer	*/
/*  to it or NULL if the opcode doesn't exist.				*/
/*	MUST BE IN ALPHA ORDER!						*/

OPCODE *find_operator(nam)
char *nam;
{
    OPCODE *bsearch();

    static OPCODE oprtbl[] = {
	{ BINARY + OPR,			SIZE,		"."	},
	{ REG,				A,		"A"	},
	{ REG,				AF,		"AF"	},
	{ BINARY + LOG1  + OPR,		AND,		"AND"	},
	{ REG,				B,		"B"	},
	{ REG,				BC,		"BC"	},
	{ MIX,				C,		"C"	},
	{ REG,				D,		"D"	},
	{ REG,				DE,		"DE"	},
	{ REG,				E,		"E"	},
	{ CON,				Z,		"EQ"	},
	{ BINARY + RELAT + OPR,		GE,		"GE"	},
	{ BINARY + RELAT + OPR,		'>',		"GT"	},
	{ REG,				H,		"H"	},
	{ UNARY  + UOP3  + OPR,		HIGH,		"HIGH"	},
	{ REG,				HL,		"HL"	},
	{ REG + INDEX,			X,		"IX"	},
	{ REG + INDEX,			Y,		"IY"	},
	{ REG,				L,		"L"	},
	{ BINARY + RELAT + OPR,		LE,		"LE"	},
	{ UNARY  + UOP3  + OPR,		LOW,		"LOW"	},
	{ BINARY + RELAT + OPR,		'<',		"LT"	},
	{ MIX,				M,		"M"	},
	{ BINARY + MULT  + OPR,		MOD,		"MOD"	},
	{ CON,				NC,		"NC"	},
	{ CON,				NZ,		"NE"	},
	{ UNARY  + UOP2  + OPR,		NOT,		"NOT"	},
	{ CON,				NZ,		"NZ"	},
	{ BINARY + LOG2  + OPR,		OR,		"OR"	},
	{ CON,				P,		"P"	},
	{ CON,				PE,		"PE"	},
	{ CON,				PO,		"PO"	},
	{ BINARY + MULT  + OPR,		SHL,		"SHL"	},
	{ BINARY + MULT  + OPR,		SHR,		"SHR"	},
	{ REG,				SP,		"SP"	},
	{ BINARY + LOG2  + OPR,		XOR,		"XOR"	},
	{ CON,				Z,		"Z"	}
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
    	if (!listok) return;
	i = bytes;  o = obj;
	do {
	    fprintf(list,"%c  ",errcode);
	    if (listhex) {
		fprintf(list,"%04X  ",address);
		for (j = 4; j; --j) {
		    if (i) { --i;  ++address;  fprintf(list," %02X",*o++); }
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
	if (errors) fprintf(list,"\n%d Error(s)",errors);
	else fprintf(list,"\nNo Errors");
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
	fprintf(list,"%04X,%04X  %-10s",sp -> valu,((sp -> len) ? sp -> len : 2),sp -> sname);
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
	if (title[0]) { 
		fprintf(list,"\n%s\n",title);  listleft -= 4;
		if (sub_tit[0])	{
			fprintf(list,"%s\n\n",sub_tit);
		} else fprintf(list,"\n\n");
	}
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
    	if (code) {
    		buf[cnt++] = c;
		if (cnt == HEXSIZE) record(0);
	}
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
    	if (code) {
    		if (cnt) record(0);
		addr = a;
	}
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

void error_s(code)
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
