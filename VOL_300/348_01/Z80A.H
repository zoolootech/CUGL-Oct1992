/*
	HEADER:		;
	TITLE:		Z80 Cross-Assembler (Portable);
	FILENAME:	Z80A.H;
	VERSION:	0.0;
	DATE:		10/11/1988;

	DESCRIPTION:	"This program lets you use your computer to assemble
			code for the Intel 8080/Z80 family of microprocessors.
			The program is written in portable C rather than BDS
			C.  All assembler features are supported except
			relocation, linkage, and macros.";

	KEYWORDS:	Software Development, Assemblers, Cross-Assemblers,
			Intel, Zilog, Z80;

	SYSTEM:		CP/M-80, CP/M-86, HP-UX, MSDOS, PCDOS, QNIX;
	COMPILERS:	Aztec C86, Aztec CII, CI-C86, Eco-C, Eco-C88, HP-UX,
			Lattice C, Microsoft C,	QNIX C;

	WARNINGS:	"This program has compiled successfully on 2 UNIX
			compilers, 5 MSDOS compilers, and 2 CP/M compilers.
			A port to Toolworks C is untried."

	AUTHORS:	William C. Colley III, Michael G. Panas;
*/

/*
		      z80 Cross-Assembler in Portable C

		Portions Copyright (c) 1985,1987 William C. Colley, III
		Portions Copyright (c) 1988  Michael G. Panas

Revision History:

Ver	Date		Description

0.0	SEPT 1988	Adapted from version 0.0 of Michael Panas portable
			QBAL Z80 cross-assembler by Michael G. Panas.

This header file contains the global constants and data type definitions for
all modules of the cross-assembler.  This also seems a good place to put the
compilation and linkage instructions for the animal.  This list currently
includes the following compilers:

	    Compiler Name		Op. Sys.	Processor

	1)  Aztec C86			CP/M-86		8086, 8088
					MSDOS/PCDOS

	2)  AZTEC C II			CP/M-80		8080, Z-80

	3)  Computer Innovations C86	MSDOS/PCDOS	8086, 8088

	4)  Eco-C			CP/M-80		Z-80

	5)  Eco-C88			MSDOS/PCDOS	8086, 8088

	6)  HP C			HP-UX		68000

	7)  Lattice C			MSDOS/PCDOS	8086, 8088

	8)  Microsoft C			MSDOS/PCDOS	8086, 8088

	9)  QNIX C			QNIX		8086, 8088

Further additions will be made to the list as users feed the information to
me.  This particularly applies to UNIX and IBM-PC compilers.

Compile-assemble-link instructions for this program under various compilers
and operating systems:

    1)	Aztec C86:

	A)  Uncomment out the "#define AZTEC_C" line and comment out all
	    other compiler names in Z80A.H.

	B)  Assuming that all files are on drive A:, run the following sequence
	    of command lines:

		A>cc Z80A
		A>cc Z80Aeval
		A>cc Z80Autil
		A>ln Z80A.o Z80Aeval.o Z80Autil.o -lc
		A>era Z80A*.o

    2)  Aztec CII (version 1.06B):

	A)  Uncomment out the "#define AZTEC_C" line and comment out all
	    other compiler names in Z80A.H.

	B)  Assuming the C compiler is called "CC.COM" and all files are
	    on drive A:, run the following sequence of command lines:

		A>cc Z80A
		A>as -zap Z80A
		A>cc Z80Aeval
		A>as -zap Z80Aeval
		A>cc Z80Autil
		A>as -zap Z80Autil
		A>ln Z80A.o Z80Aeval.o Z80Autil.o -lc
		A>era Z80A*.o

    3)  Computer Innovations C86:

	A)  Uncomment out the "#define CI_C86" line and comment out all
	    other compiler names in Z80A.H.

	B)  Compile the files Z80A.C, Z80AEVAL.C, and Z80AUTIL.C.  Link
	    according to instructions that come with the compiler.

    4)  Eco-C (CP/M-80 version 3.10):

	A)  Uncomment out the "#define ECO_C" line and comment out all
	    other compiler names in Z80A.H.

	B)  Assuming all files are on drive A:, run the following sequence of
	    command lines:

		A>cp Z80A -i -m
		A>cp Z80Aeval -i -m
		A>cp Z80Autil -i -m
		A>l80 Z80A,Z80Aeval,Z80Autil,Z80A/n/e
		A>era Z80A*.mac
		A>era Z80A*.rel

    5)  Eco-C88:

	A)  Uncomment out the "#define ECO_C" line and comment out all
	    other compiler names in Z80A.H.

	B)  Compile the files Z80A.C, Z80AEVAL.C, and Z80AUTIL.C.  Link
	    according to instructions that come with the compiler.

    6)  HP-UX (a UNIX look-alike running on an HP-9000 Series 200/500,
	68000-based machine):

	A)  Uncomment out the "#define HP_UX" line and comment out all
	    other compiler names in Z80A.H.

	B)  Run the following command line:

		. cc Z80A.c Z80Aeval.c Z80Autil.c

    7)  Lattice C:

	A)  Uncomment out the "#define LATTICE_C" line and comment out all
	    other compiler names in Z80A.H.

	B)  Compile the files Z80A.C, Z80AEVAL.C, and Z80AUTIL.C.  Link
	    according to instructions that come with the compiler.

    8)  Microsoft C (version 3.00 --> version 5.1):

	A)  Uncomment out the "#define MICROSOFT_C" line and comment out all
	    other compiler names in A68.H.

	B)  Run the following command line:

		C>cl Z80A.c Z80Aeval.c Z80Autil.c

    9)	QNIX C:

	A)  Uncomment out the "#define QNIX" line and comment out all other
	    compiler names in Z80A.H.

	B)  Run the following command line:

		. cc Z80A.c Z80Aeval.c Z80Autil.c

Note that, under CP/M-80, you can't re-execute a core image from a previous
assembly run with the "@.COM" trick.  This technique is incompatible with the
Aztec CII compiler, so I didn't bother to support it at all.
*/

#include <stdio.h>

/*  Comment out all but the line containing the name of your compiler:	*/

/* #define AZTEC_C	/*					*/
/* #define CI_C86	/*					*/
/* #define ECO_C	/*					*/
/* #define HP_UX	/* All UNIX's use this			*/
/* #define LATTICE_C	/*					*/
#define MICROSOFT_C	/* 3.0 4.0 5.1 				*/
/* #define QNIX		/*					*/

/*  Compiler dependencies:						*/

#ifdef	AZTEC_C
#define	getc(f)		agetc(f)
#define	putc(c,f)	aputc(c,f)
#endif

#ifndef	ECO_C
#define	FALSE		0
#define	TRUE		(!0)
#endif

#ifdef	LATTICE_C
#define	void		int
#endif

#ifdef	QNIX
#define	fprintf		tfprintf
#define	printf		tprintf
#endif

/*  On 8-bit machines, the static type is as efficient as the register	*/
/*  type and far more efficient than the auto type.  On larger machines	*/
/*  such as the 8086 family, this is not necessarily the case.  To	*/
/*  let you experiment to see what generates the fastest, smallest code	*/
/*  for your machine, I have declared internal scratch variables in	*/
/*  functions "SCRATCH int", "SCRATCH unsigned", etc.  A SCRATCH	*/
/*  varible is made static below, but you might want to try register	*/
/*  instead.								*/

#define	SCRATCH		static

/*  A slow, but portable way of cracking an unsigned into its various	*/
/*  component parts:							*/

#define	clamp(u)	((u) &= 0xffff)
#define	high(u)		(((u) >> 8) & 0xff)
#define	low(u)		((u) & 0xff)
#define	word(u)		((u) & 0xffff)

/*  The longest source line the assembler can hold without exploding:	*/

#define	MAXLINE		255

/*  The maximum number of source files that can be open simultaneously:	*/

#define	FILES		4

/*  The fatal error messages generated by the assembler:		*/

#define	ASMOPEN		"Source File Did Not Open"
#define	ASMREAD		"Error Reading Source File"
#define	DSKFULL		"Disk or Directory Full"
#define	FLOFLOW		"File Stack Overflow"
#define	HEXOPEN		"Object File Did Not Open"
#define	IFOFLOW		"If Stack Overflow"
#define	LSTOPEN		"Listing File Did Not Open"
#define	NOASM		"No Source File Specified"
#define	SYMBOLS		"Too Many Symbols"

/*  The warning messages generated by the assembler:			*/

#define	BADOPT		"Illegal Option Ignored"
#define	NOHEX		"-o Option Ignored -- No File Name"
#define	NOLST		"-l Option Ignored -- No File Name"
#define	TWOASM		"Extra Source File Ignored"
#define	TWOHEX		"Extra Object File Ignored"
#define	TWOLST		"Extra Listing File Ignored"

/*  Line assembler (Z80A.C) constants:					*/

#define	BIGINST		4		/* longest instruction length	*/
#define	IFDEPTH		16		/* maximum IF nesting level	*/
#define	NOP		0x00		/* processor's NOP opcode	*/
#define	ON		1		/* assembly turned on		*/
#define	OFF		-1		/* assembly turned off		*/

/*  Line assembler (Z80A.C) opcode attribute word flag masks:		*/

#define	PSEUDO		0x8000		/* is pseudo-op			*/
#define	ISIF		0x4000		/* is IF, ELSE, or ENDI		*/

#define	SHIFT		0x0f00		/* operand shift left count	*/
#define	OPTYPE		0x001f		/* opcode type mask		*/

/*  Line assembler (Z80A.C) opcode types:				*/

#define	NO_ARG		0		/*	no arg inst		*/
#define	ONE_ARG		1		/*	one (8 bit) arg		*/
					/*	all 8 bit immediate inst*/
#define	JUMP		2		/*	jump			*/
#define	CALL		3		/*	call 			*/
#define	JREL		4		/*	jump relative		*/
#define	RETURN		5		/*	return			*/
#define	L_ARG		6		/*	Long (16 bit) arg	*/
					/*	  LDI16			*/
#define	L_ARG2		7		/*	Long (16 bit) arg type 2*/
					/*	  			*/
#define	L_OP		8		/*	long operation		*/
					/*	  ADDHL, DEC16, INC16,	*/
					/*	  PUSH, POP		*/
#define	MOV		10		/*	LD instruction		*/
#define	RESTART		11		/*	restart instruction	*/
#define	S_OP		12		/*	short operation		*/
					/*	  ADD, AND, CMP, DEC,	*/
					/*	  INC, OR, SBC, SUB,	*/
					/*	  XOR ... needs SHIFT	*/
					/*	  value			*/
#define	NO_ARG_L	13		/* opcode is 2 bytes long	*/
#define	ONE_ARG_L	14		/* opcode is 2 bytes with arg	*/
					/*	JUMPI, LDSP		*/
#define	ONE_ARG_L2	15		/* opcode is 2 bytes w/arg type 2*/
					/*	ADCHL, ADDIX, ADDIY, 	*/
					/*	SBCHL			*/
#define	ROTATE		16		/* 	all extened rotate inst	*/
#define	XIO		17		/*	all extened I/O inst	*/
#define	BIT		18		/*	all bit type inst	*/
#define	L_ARG_L		19		/* opcode is 2 bytes w/ 2 byte arg */
					/*	LD16			*/
#define	L_ARG_L2	20		/* opcode 2bytes w/2byte arg #2 */
					/*	ST16			*/

/*  Line assembler (Z80A.C) pseudo-op opcode token values:		*/

#define	DB		0
#define	DS		1
#define	DW		2
#define	ELSE		3
#define	END		4
#define	ENDIF		5
#define	EQU		6
#define	IF		7
#define	INCL		8
#define	ORG		9
#define	PAGE		10
#define	SET		11
#define	TITL		12
#define	PAGT		13
#define	CODE		14
#define	NCODE		15
#define	CSECT		16
#define	LIST		17
#define	ULIST		18
#define	SKP		19
#define	TYP		20
#define	URLC		21
#define	RLC		22
#define	MACRO		23
#define	ENDM		24

/*  Lexical analyzer (Z80AEVAL.C) token buffer and stream pointer:	*/

typedef struct {
    unsigned attr;
    unsigned valu;
    char sval[MAXLINE + 1];
} TOKEN;

/*  Lexical analyzer (Z80AEVAL.C) token attribute values:		*/

#define	EOL		0	/*  end of line				*/
#define	SEP		1	/*  field separator			*/
#define	OPR		2	/*  operator				*/
#define	STR		3	/*  character string			*/
#define	VAL		4	/*  value				*/
#define	REG		5	/*  register or other tag		*/
#define	IMM		6	/*  immediate mode flag 		*/
#define	CON		7	/*  conditional flag			*/
#define	MIX		8	/*  mixed - register or conditional	*/

/*  Lexical analyzer (Z80AEVAL.C) token attribute word flag masks:	*/

#define	BINARY		0x8000	/*  Operator:	is binary operator	*/
#define	UNARY		0x4000	/*		is unary operator	*/
#define	INDEX		0x1000	/* 		is index reg		*/
#define	PREC		0x0f00	/*		precedence		*/

#define	FORWD		0x8000	/*  Value:	is forward referenced	*/
#define	SOFT		0x4000	/*		is redefinable		*/
#define	PIN		0x2000	/*		is PIN addressed	*/
#define	PIN_VAL		0x1000	/*		is PIN valued		*/

#define	TYPE		0x000f	/*  All:	token type		*/

/*  Lexical analyzer (Z80AEVAL.C) register token values:		*/
#define	A		0x07	/*  A					*/
#define	B		0x00	/*  B					*/
#define	C		0x01	/*  C reg or C carry jump		*/
#define	D		0x02	/*  D					*/
#define	E		0x03	/*  E					*/
#define	H		0x04	/*  H					*/
#define	L		0x05	/*  L					*/
#define	M		0x06	/*  M reg or M minus jump		*/

#define	X		0xdd	/*  IX					*/
#define	Y		0xfd	/*  IY					*/

#define	AF		0x08	/*  AF (PSW)				*/
#define	SP		0x09	/*  SP					*/
#define	BC		0x0a	/*  BC					*/
#define	DE		0x0b	/*  DE					*/
#define	HL		0x0c	/*  HL					*/

/*  Lexical analyzer (Z80AEVAL.C) conditional token values (+$10)	*/

#define	NZ		0x10	/*  NZ non-zero jump			*/
#define	Z		0x11	/*  Z zero jump				*/
#define	NC		0x12	/*  NC no-carry jump (C already used)	*/
				/*  C  carry ( 0x03)			*/
#define	PO		0x14	/*  PO parity odd jump			*/
#define	PE		0x15	/*  PE parity even jump			*/
#define	P		0x16	/*  P  plus jump (M already used)	*/
				/*  M  minus ( 0x07)			*/
				
/*  Lexical analyzer (Z80AEVAL.C) operator token values (unlisted ones	*/
/*  use ASCII characters):						*/

#define	AND		0
#define	GE		1
#define	HIGH		2
#define	LE		3
#define	LOW		4
#define	MOD		5
#define	NE		6
#define	NOT		7
#define	OR		8
#define	SHR		9
#define	SHL		10
#define	XOR		11
#define	SIZE		12
#define	LENOF		13

/*  Lexical analyzer (Z80AEVAL.C) operator precedence values:		*/

#define	UOP1		0x0000	/*  unary +, unary -			*/
#define	MULT		0x0100	/*  *, /, MOD, SHL, SHR			*/
#define	ADDIT		0x0200	/*  binary +, binary -			*/
#define	RELAT		0x0300	/*  >, >=, =, <=, <, <>			*/
#define	UOP2		0x0400	/*  NOT					*/
#define	LOG1		0x0500	/*  AND					*/
#define	LOG2		0x0600	/*  OR, XOR				*/
#define	UOP3		0x0700	/*  HIGH, LOW, .			*/
#define	RPREN		0x0800	/*  )					*/
#define	LPREN		0x0900	/*  (					*/
#define	ENDEX		0x0a00	/*  end of expression			*/
#define	START		0x0b00	/*  beginning of expression		*/

/*  Utility package (Z80AUTIL.C) symbol table routines:			*/

struct _symbol {
    unsigned attr;
    unsigned valu;
    unsigned len;
    struct _symbol *left, *right;
    char sname[1];
};

typedef struct _symbol SYMBOL;

#define	SYMCOLS		4

/*  Utility package (Z80AUTIL.C) opcode/operator table routines:		*/

typedef struct {
    unsigned attr;
    unsigned valu;
    char oname[8];
} OPCODE;

/*  Utility package (Z80AUTIL.C) hex file output routines:		*/

#define	HEXSIZE		32
