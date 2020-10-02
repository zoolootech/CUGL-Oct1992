/***
	header file for as68 assembler
*/

/*		(C) Copyright 1982 Steve Passe		*/
/*		All Rights Reserved					*/

/* version 1.00 */

/* version 1.01

	8/30/83	ver. 1.01 modified for Aztec ver. 1.05g	smp

*/

/* statement field types */

#define MNEMONIC		2
#define PSEUDO			3
#define OPERAND			4
#define OPRND_1			5
#define OPRND_2			6
#define COMMENT			7
#define MACRO			8

/* error report numbers */

#define PREPARSE		-101	/* statement preparse error */
#define BAD_INSTR		-102	/* bad char in mnemonic-psdo field */
#define UNREC_INSTR		-103	/* instr not found in mnem or pseudo table */
#define BAD_MACRO		-104	/* bad char in macro field */
#define UNREC_MACRO		-105	/* macro not found in macro table(s) */
#define LABEL_USE		-106	/* improper use of label */
#define OPRND_EVAL		-107	/* can't evaluate operand */
#define EQU_EVAL		-108	/* can't evaluate equ operand */
#define SET_EVAL		-109	/* can't evaluate set operand */
#define REDEF			-110	/* attempt to redefine a permanent symbol */
#define SYMTFULL		-111	/* symbol table full */
#define UNREC_OPRND		-112	/* unrecognized operand */
#define UNDEF_SYMBOL	-113	/* symbol not defined in symbol table */
#define LBL_RANGE		-114	/* label out of range for current add. mode */
#define ILGL_OP1		-115	/* op 1 is not valid for instruction type */
#define ILGL_OP2		-116	/* op 2 is not valid for instruction type */
#define BAD_OP1			-117	/* op 1 is not correctly formed */
#define BAD_OP2			-118	/* op 2 is not correctly formed */
#define CODE_BUILD		-119	/* code building function failed */

/* bit fiddle error types */

#define ERR_BBB			-120
#define ERR_BX3			-121
#define ERR_BX5			-122
#define ERR_BX			-123
#define ERR_CCC			-124
#define ERR_DDD			-125
#define ERR_RXS			-126
#define ERR_MK			-127
#define ERR_EFGHJ		-128
#define ERR_F			-129
#define ERR_G			-130
#define ERR_H			-131
#define ERR_J			-132
#define ERR_V			-133
#define ERR_XXX			-134
#define ERR_XX			-135
#define ERR_XXXX		-136
#define ERR_OYZ			-137
#define ERR_Y			-138
#define ERR_OY			-139
#define ERR_YY			-140
#define ERR_YYZZ		-141
#define ERR_D16			-142
#define ERR_D8I			-143
#define INCL_OPEN		-144

/* commands to _out files */

#define CODE	1
#define DATA	2
#define MDATA	3
#define FLUSH	4
#define MSG		5
#define OPEN	6
#define CLOSE	7
#define SYNC	8

/* object size definitions */

#define MTSIZE			205
#define SYMTSIZE		2000			/* size of symbol table in bytes */
#define FNAME_SIZE		15				/* max chars in a file name string */
#define STMNT_SIZE		140				/* chars in a statement line */
#define SRCLEVELS		8				/* levels of nesting of source files */

/* pseudo-op table class masks */

#define NO_LABEL 0x01

/* mnemonic table class masks */


/* symbol table class masks */

#define PERMANENT		0x01
#define RELOCATABLE		0x02

/* misc. */

#define FLAG		char
#define YES			1
#define NO			0
/** defined in stdio.h -> #define OK			0	*/
/** defined in stdio.h -> #define ERROR		-1		*/

#define CURR_DISK	25

/* defines for operand typing, mvalue table */

#define _imd		0x01		/* 0-31 bits of immediate data */
#define _an			0x02		/* address register where n == 0-7 */
#define _dn			0x03		/* data register where n == 0-7 */
#define _ccr		0x04		/* cond. code reg., low byte of status reg. */
#define _sr			0x05		/* status register, 6 bits */
#define _usp		0x06		/* user stack pointer, another name for a7 */
#define _reglst		0x07		/* list of an and/or dn. range: xn-xn,
								'/' separating individual reg. specifiers. */
#define _none		0x08

#define _sp			0x09		/* another name for a7 */
#define _anw		0x0a		/* address register where n == 0-7 */
#define _anl		0x0b		/* address register where n == 0-7 */
#define _dnw		0x0c		/* data register where n == 0-7 */
#define _dnl		0x0d		/* data register where n == 0-7 */
#define _pc			0x0e
#define _an_inx		0x0f

#define _pd_ani		0x31		/* add. reg. indirect with predecrement */
#define _ani_pi		0x32		/* add. reg. indirect with postincrement */
#define _ani		0xf3		/* address register indirect */
#define _d16_ani	0xf4		/* add. reg. indirect with displacement */
#define _d8_anx		0xf5		/* add. reg. ind. with displ. and index */
#define _address	0xf6		/* address */
#define _label		0x57		/* address label */
#define _labeli		0x58		/* address label, indexed */

#define _sadr		0x10		/* effective address: */
#define _dadr		0x20		/* effective address: */
#define _jadr		0x40		/* effective address: */
#define _madr		0x80		/* effective address: */

/*
	the op eval structure:
*/

struct _oprnd {
	int _typ;							/* operand type */
	char _reg;							/* register # if present */
	char _regtyp;						/* type of reg., a or d */
	char _ireg;							/* index reg. # if present */
	char _iregtyp;						/* type of index reg., a or d */
	FLAG _inxl;							/* short or long index value */
	FLAG _rel_lbl;						/* relative label forced (* op) */
	int _displ;							/* displacement if present */
	unsigned _reg_list;					/* reg. list if present */
	FLAG _long_val;						/* data is long if TRUE */
	long _data;							/* immediate data if present */
	long _addr;							/* address if present */
};

struct _mtable {
	char _mnem[8];					/* ascii mnemonic for opcode */
	int _mvi;						/* index into mvalue */
	char _mvc;						/* number of arrays belonging to mnem */
};

struct _mvalue {
	char _optyp1;					/* type of first operand */
	char _optyp2;					/* type of second operand */
	char _opcb1;					/* first byte of opcode */
	char _opcb2;					/* second byte of opcode */
	char _opc_len;					/* number of ext words */
	int (*_p2_action)();			/* code building function */
};

struct _ptable {
	char _psdo[8];
	int (*_p1)();
	int (*_p2)();
	int _arg;
};

struct _symbol {							/* structure of symbol table */
	char *_sym;
	long _val;
	char _atr;
};
/* short or long index value */
	FLAG _rel_lbl;						/* relative label forced (* op) */
	int _displ;			