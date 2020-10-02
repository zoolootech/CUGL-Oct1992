/*
	HEADER:		CUG219;
	TITLE:		65C02 Cross-Assembler (Portable);
	FILENAME:	A65C.H;
	VERSION:	0.1;
	DATE:		08/27/1988;

	DESCRIPTION:	"This program lets you use your computer to assemble
			code for the Rockwell 65C02 microprocessor.  The
			program is written in portable C rather than BDS C.
			All assembler features are supported except relocation
			linkage, and macros.";

	KEYWORDS:	Software Development, Assemblers, Cross-Assemblers,
			Rockwell, 65C02;

	SYSTEM:		CP/M-80, CP/M-86, HP-UX, MSDOS, PCDOS, QNIX;
	COMPILERS:	Aztec C86, Aztec CII, CI-C86, Eco-C, Eco-C88, HP-UX,
			Lattice C, Microsoft C,	QNIX C;

	WARNINGS:	"This program is written in as portable C as possible.
			A port to BDS C would be extremely difficult, but see
			volume CUG113.  A port to Toolworks C is untried."

	AUTHORS:	William C. Colley III;
*/

/*
		      65C02 Cross-Assembler in Portable C

		   Copyright (c) 1986 William C. Colley, III

Revision History:

Ver	Date		Description

0.0	NOV 1986	Derived from my 6800/6801 cross-assembler.  WCC3.

0.1	AUG 1988	Fixed a bug in the command line parser that puts it
			into a VERY long loop if the user types a command line
			like "A65C FILE.ASM -L".  WCC3 per Alex Cameron.

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

	A)  Uncomment out the "#define AZTEC_C 1" line and comment out all
	    other compiler names in A65C.H.

	B)  Assuming that all files are on drive A:, run the following sequence
	    of command lines:

		A>cc a65c
		A>cc a65ceval
		A>cc a65cutil
		A>ln a65c.o a65ceval.o a65cutil.o -lc
		A>era a65c*.o

    2)  Aztec CII (version 1.06B):

	A)  Uncomment out the "#define AZTEC_C 1" line and comment out all
	    other compiler names in A65C.H.

	B)  Assuming the C compiler is called "CC.COM" and all files are
	    on drive A:, run the following sequence of command lines:

		A>cc a65c
		A>as -zap a65c
		A>cc a65ceval
		A>as -zap a65ceval
		A>cc a65cutil
		A>as -zap a65cutil
		A>ln a65c.o a65ceval.o a65cutil.o -lc
		A>era a65c*.o

    3)  Computer Innovations C86:

	A)  Uncomment out the "#define CI_C86 1" line and comment out all
	    other compiler names in A65C.H.

	B)  Compile the files A65C.C, A65CEVAL.C, and A65CUTIL.C.  Link
	    according to instructions that come with the compiler.

    4)  Eco-C (CP/M-80 version 3.10):

	A)  Uncomment out the "#define ECO_C 1" line and comment out all
	    other compiler names in A65C.H.

	B)  Assuming all files are on drive A:, run the following sequence of
	    command lines:

		A>cp a65c -i -m
		A>cp a65ceval -i -m
		A>cp a65cutil -i -m
		A>l80 a65c,a65ceval,a65cutil,a65c/n/e
		A>era a65c*.mac
		A>era a65c*.rel

    5)  Eco-C88:

	A)  Uncomment out the "#define ECO_C 1" line and comment out all
	    other compiler names in A65C.H.

	B)  Compile the files A65C.C, A65CEVAL.C, and A65CUTIL.C.  Link
	    according to instructions that come with the compiler.

    6)  HP-UX (a UNIX look-alike running on an HP-9000 Series 200/500,
	68000-based machine):

	A)  Uncomment out the "#define HP_UX 1" line and comment out all
	    other compiler names in A65C.H.

	B)  Run the following command line:

		. cc a65c.c a65ceval.c a65cutil.c

    7)  Lattice C:

	A)  Uncomment out the "#define LATTICE_C 1" line and comment out all
	    other compiler names in A65C.H.

	B)  Compile the files A65C.C, A65CEVAL.C, and A65CUTIL.C.  Link
	    according to instructions that come with the compiler.

    8)  Microsoft C (version 3.00):

	A)  Uncomment out the "#define MICROSOFT_C 1" line and comment out
	    all other compiler names in A65C.H.

	B)  Run the following command line:

		C>cl a65c.c a65ceval.c a65cutil.c

    9)	QNIX C:

	A)  Uncomment out the "#define QNIX 1" line and comment out all other
	    compiler names in A65C.H.

	B)  Run the following command line:

		. cc a65c.c a65ceval.c a65cutil.c

Note that, under CP/M-80, you can't re-execute a core image from a previous
assembly run with the "@.COM" trick.  This technique is incompatible with the
Aztec CII compiler, so I didn't bother to support it at all.
*/

#include <stdio.h>

/*  Comment out all but the line containing the name of your compiler:	*/

#define		AZTEC_C		1
/* #define	CI_C86		1					*/
/* #define	ECO_C		1					*/
/* #define	HP_UX		1					*/
/* #define	LATTICE_C	1					*/
/* #define	MICROSOFT_C	1					*/
/* #define	QNIX		1					*/

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

/*  Line assembler (A65C.C) constants:					*/

#define	BIGINST		3		/*  longest instruction length	*/
#define	IFDEPTH		16		/*  maximum IF nesting level	*/
#define	NOP		0xea		/*  processor's NOP opcode	*/
#define	ON		1		/*  assembly turned on		*/
#define	OFF		-1		/*  assembly turned off		*/

/*  Line assembler (A65C.C) opcode attribute word flag masks:		*/

#define	PSEUDO		0x80	/*  is pseudo op			*/
#define	ISIF		0x40	/*  is IF, ELSE, or ENDIF		*/

/*  Line assembler (A65C.C) pseudo-op opcode token values:		*/

#define	ELSE		1
#define	END		2
#define	ENDI		3
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
#define	TITL		14

/*  Line assembler (A65C.C) machine opcode attribute values:		*/

#define	RMBOP		0
#define	CPXY		1
#define	TSBOP		2
#define	INHOP		3
#define	JUMP		4
#define	CALL		5
#define	BBOP		6
#define	RELBR		7
#define	STXY		8
#define	TWOOP		9
#define	BIT		10
#define	LOGOP		11
#define	LDXY		12
#define	INCOP		13
#define	STZ		14

/*  Opcode argument field parser (A65CEVAL.C) attribute bit masks:	*/

#define	ARGA		040	/*  A register specified.		*/
#define	ARGX		020	/*  X register specified.		*/
#define	ARGY		010	/*  Y register specified.		*/
#define	ARGIND		004	/*  Argument field in parentheses.	*/
#define	ARGIMM		002	/*  Immediate addressing specified.	*/
#define	ARGNUM		001	/*  Argument value present.		*/

/*  Lexical analyzer (A65CEVAL.C) token buffer and stream pointer:	*/

typedef struct {
    unsigned attr;
    unsigned valu;
    char sval[MAXLINE + 1];
} TOKEN;

/*  Lexical analyzer (A65CEVAL.C) token attribute values:		*/

#define	EOL		0	/*  end of line				*/
#define	SEP		1	/*  field separator			*/
#define	OPR		2	/*  operator				*/
#define	STR		3	/*  character string			*/
#define	VAL		4	/*  value				*/
#define	IMM		5	/*  immediate designator		*/
#define	REG		6	/*  register designator			*/

/*  Lexical analyzer (A65CEVAL.C) token attribute word flag masks:	*/

#define	BINARY		0x8000	/*  Operator:	is binary operator	*/
#define	UNARY		0x4000	/*		is unary operator	*/
#define	PREC		0x0f00	/*		precedence		*/

#define	FORWD		0x8000	/*  Value:	is forward referenced	*/
#define	SOFT		0x4000	/*		is redefinable		*/

#define	TYPE		0x000f	/*  All:	token type		*/

/*  Lexical analyzer (A65CEVAL.C) operator token values (unlisted ones	*/
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

/*  Lexical analyzer (A65CEVAL.C) operator precedence values:		*/

#define	UOP1		0x0000	/*  unary +, unary -			*/
#define	MULT		0x0100	/*  *, /, MOD, SHL, SHR			*/
#define	ADDIT		0x0200	/*  binary +, binary -			*/
#define	RELAT		0x0300	/*  >, >=, =, <=, <, <>			*/
#define	UOP2		0x0400	/*  NOT					*/
#define	LOG1		0x0500	/*  AND					*/
#define	LOG2		0x0600	/*  OR, XOR				*/
#define	UOP3		0x0700	/*  HIGH, LOW				*/
#define	START2		0x0800	/*  beginning of subexpression		*/
#define	RPREN		0x0900	/*  )					*/
#define	LPREN		0x0a00	/*  (					*/
#define	ENDEX		0x0b00	/*  end of expression			*/
#define	START		0x0c00	/*  beginning of expression		*/

/*  Utility package (A65CUTIL.C) symbol table routines:			*/

struct _symbol {
    unsigned attr;
    unsigned valu;
    struct _symbol *left, *right;
    char sname[1];
};

typedef struct _symbol SYMBOL;

#define	SYMCOLS		4

/*  Utility package (A65CUTIL.C) opcode/operator table routines:	*/

typedef struct {
    unsigned attr;
    unsigned valu;
    char oname[5];
} OPCODE;

/*  Utility package (A65CUTIL.C) hex file output routines:		*/

#define	HEXSIZE		32
