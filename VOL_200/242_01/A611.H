/*
	HEADER:		CUG242;
	TITLE:		68HC11 Cross-Assembler (Portable);
	FILENAME:	A611.H;
	VERSION:	0.1;
	DATE:		08/27/1988;

	DESCRIPTION: 	"This program lets you use your computer to assemble
			code for the Motorola 68HC11 microprocessor.  The
			program is written in portable C rather than BDS C.
			All assembler features are supported except
			relocation, linkage, and macros.";

	KEYWORDS:	Software Development, Assemblers, Cross-Assemblers,
			Motorola, MC68HC11;

	SYSTEM:		CP/M-80, CP/M-86, HP-UX, MSDOS, PCDOS, QNIX;
	COMPILERS:	Aztec C86, Aztec CII, CI-C86, Eco-C, Eco-C88, HP-UX,
			Lattice C, Microsoft C,	QNIX C;

	WARNINGS:	"This program has compiled successfully on 2 UNIX
			compilers, 5 MSDOS compilers, and 2 CP/M compilers.
			A port to BDS C would be extremely difficult.  A port
			to Toolworks C is untried."

	AUTHORS:	William C. Colley III;
*/

/*
		     68HC11 Cross-Assembler in Portable C

		Copyright (c) 1985,1987 William C. Colley, III

Revision History:

Ver	Date		Description

0.0	JUN 1987	Adapted from version 0.2 of the portable 6805 cross-
			assembler.  WCC3.

0.1	AUG 1988	Fixed a bug in the command line parser that puts it
			into a VERY long loop if the user types a command line
			like "A611 FILE.ASM -L".  WCC3 per Alex Cameron.

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

	A)  Uncomment out the "#define AZTEC_C" line and comment out all other
	    compiler names in A611.H.

	B)  Assuming that all files are on drive A:, run the following sequence
	    of command lines:

		A>cc a611
		A>cc a611eval
		A>cc a611util
		A>ln a611.o a611eval.o a611util.o -lc
		A>era a611*.o

    2)  Aztec CII (version 1.06B):

	A)  Uncomment out the "#define AZTEC_C" line and comment out all other
	    compiler names in A611.H.

	B)  Assuming the C compiler is called "CC.COM" and all files are
	    on drive A:, run the following sequence of command lines:

		A>cc a611
		A>as -zap a611
		A>cc a611eval
		A>as -zap a611eval
		A>cc a611util
		A>as -zap a611util
		A>ln a611.o a611eval.o a611util.o -lc
		A>era a611*.o

    3)  Computer Innovations C86:

	A)  Uncomment out the "#define CI_C86" line and comment out all other
	    compiler names in A611.H.

	B)  Compile the files A611.C, A611EVAL.C, and A611UTIL.C.  Link
	    according to instructions that come with the compiler.

    4)  Eco-C (CP/M-80 version 3.10):

	A)  Uncomment out the "#define ECO_C" line and comment out all other
	    compiler names in A611.H.

	B)  Assuming all files are on drive A:, run the following sequence of
	    command lines:

		A>cp a611 -i -m
		A>cp a611eval -i -m
		A>cp a611util -i -m
		A>l80 a611,a611eval,a611util,a611/n/e
		A>era a611*.mac
		A>era a611*.rel

    5)  Eco-C88:

	A)  Uncomment out the "#define ECO_C" line and comment out all other
	    compiler names in A611.H.

	B)  Compile the files A611.C, A611EVAL.C, and A611UTIL.C.  Link
	    according to instructions that come with the compiler.

    6)  HP-UX (a UNIX look-alike running on an HP-9000 Series 200/500,
	68000-based machine):

	A)  Uncomment out the "#define HP_UX" line and comment out all other
	    compiler names in A611.H.

	B)  Run the following command line:

		. cc a611.c a611eval.c a611util.c

    7)  Lattice C:

	A)  Uncomment out the "#define LATTICE_C" line and comment out all
	    other compiler names in A611.H.

	B)  Compile the files A611.C, A611EVAL.C, and A611UTIL.C.  Link
	    according to instructions that come with the compiler.

    8)  Microsoft C (version 3.00):

	A)  Uncomment out the "#define MICROSOFT_C" line and comment out all
	    other compiler names in A68.H.

	B)  Run the following command line:

		C>cl a611.c a611eval.c a611util.c

    9)	QNIX C:

	A)  Uncomment out the "#define QNIX" line and comment out all other
	    compiler names in A611.H.

	B)  Run the following command line:

		. cc a611.c a611eval.c a611util.c

Note that, under CP/M-80, you can't re-execute a core image from a previous
assembly run with the "@.COM" trick.  This technique is incompatible with the
Aztec CII compiler, so I didn't bother to support it at all.
*/

#include <stdio.h>

/*  Comment out all but the line containing the name of your compiler:	*/

#define		AZTEC_C
/* #define	CI_C86							*/
/* #define	ECO_C							*/
/* #define	HP_UX							*/
/* #define	LATTICE_C						*/
/* #define	MICROSOFT_C						*/
/* #define	QNIX							*/

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

/*  Line assembler (A611.C) constants:					*/

#define	BIGINST		5		/*  longest instruction length	*/
#define	IFDEPTH		16		/*  maximum IF nesting level	*/
#define	NOP		0x01		/*  processor's NOP opcode	*/
#define	ON		1		/*  assembly turned on		*/
#define	OFF		-1		/*  assembly turned off		*/

/*  Line assembler (A611.C) opcode attribute word flag masks:		*/

#define	PSEUDO		0x8000	/*  is pseudo op			*/
#define	ISIF		0x4000	/*  is IF, ELSE, or ENDI		*/
#define	REL		0x0100	/*  has relative address operand	*/
#define	MASK		0x0080	/*  has bit mask operand		*/
#define	IMM_16		0x0040	/*  immediate operands are words	*/
#define	IMMED		0x0020	/*  can use immediate addressing	*/
#define	DIR		0x0010	/*  can use direct addressing		*/
#define	EXT		0x0008	/*  can use extended addressing		*/
#define	INDX		0x0004	/*  can use indexed addressing		*/
#define	PREBYTE		0x0003	/*  prebyte structure (0-3)		*/

/*  Line assembler (A611.C) pseudo-op opcode token values:		*/

#define	ELSE		1
#define	END		2
#define	ENDIF		3
#define	EQU		4
#define	FCB		5
#define	FCC		6
#define	FDB		7
#define	IF		8
#define	INCL		9
#define	ORG		10
#define	PAGE		11
#define	RMB		12
#define	SET		13
#define	TITLE		14

/*  Lexical analyzer (A611EVAL.C) token buffer and stream pointer:	*/

typedef struct {
    unsigned attr;
    unsigned valu;
    char sval[MAXLINE + 1];
} TOKEN;

/*  Lexical analyzer (A611EVAL.C) token attribute values:		*/

#define	EOL		0	/*  end of line				*/
#define	SEP		1	/*  field separator			*/
#define	OPR		2	/*  operator				*/
#define	STR		3	/*  character string			*/
#define	VAL		4	/*  value				*/
#define	IMM		5	/*  immediate designator		*/
#define	REG		6	/*  register designator			*/

/*  Lexical analyzer (A611EVAL.C) token attribute word flag masks:	*/

#define	BINARY		0x8000	/*  Operator:	is binary operator	*/
#define	UNARY		0x4000	/*		is unary operator	*/
#define	PREC		0x0f00	/*		precedence		*/

#define	FORWD		0x8000	/*  Value:	is forward referenced	*/
#define	SOFT		0x4000	/*		is redefinable		*/

#define	TYPE		0x000f	/*  All:	token type		*/

/*  Lexical analyzer (A611EVAL.C) operator token values (unlisted ones	*/
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

/*  Lexical analyzer (A611EVAL.C) operator precedence values:		*/

#define	UOP1		0x0000	/*  unary +, unary -			*/
#define	MULT		0x0100	/*  *, /, MOD, SHL, SHR			*/
#define	ADDIT		0x0200	/*  binary +, binary -			*/
#define	RELAT		0x0300	/*  >, >=, =, <=, <, <>			*/
#define	UOP2		0x0400	/*  NOT					*/
#define	LOG1		0x0500	/*  AND					*/
#define	LOG2		0x0600	/*  OR, XOR				*/
#define	UOP3		0x0700	/*  HIGH, LOW				*/
#define	RPREN		0x0800	/*  )					*/
#define	LPREN		0x0900	/*  (					*/
#define	ENDEX		0x0a00	/*  end of expression			*/
#define	START		0x0b00	/*  beginning of expression		*/

/*  Utility package (A611UTIL.C) symbol table routines:			*/

struct _symbol {
    unsigned attr;
    unsigned valu;
    struct _symbol *left, *right;
    char sname[1];
};

typedef struct _symbol SYMBOL;

#define	SYMCOLS		4

/*  Utility package (A611UTIL.C) opcode/operator table routines:	*/

typedef struct {
    unsigned attr;
    unsigned valu;
    char oname[6];
} OPCODE;

/*  Utility package (A611UTIL.C) hex file output routines:		*/

#define	HEXSIZE		32
