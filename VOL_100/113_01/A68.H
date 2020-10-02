/*
	HEADER:		CUG113;
	TITLE:		6800 Cross-Assembler (BDS C Version);
	FILENAME:	A68.H;
	VERSION:	2.6;
	DATE:		07/22/1985;

	DESCRIPTION:	"This program lets you use your CP/M-80-based computer
			to assemble code for the Motorola 6800, 6801, 6802,
			6803, 6808, and 68701 microprocessors.  The program is
			written in BDS C for the best possible performance on
			8-bit machines.  All assembler features are supported
			except relocation, linkage, listing control, and
			macros.";

	KEYWORDS:	Software Development, Assemblers, Cross-Assemblers,
			Motorola, MC6800, MC6801;

	SEE-ALSO:	CUG149, 6801 Cross-Assembler (Portable);

	SYSTEM:		CP/M-80;
	COMPILERS:	BDS C;

	WARNINGS:	"This package is specifically tailored to CP/M-80
			machines and the rather non-standard, but high-
			performance BDS C compiler.  For other environments,
			use the portable version of this package on CUG149.";

	AUTHORS:	William C. Colley III;
*/

/*
	6800/6801 Cross-Assembler  v. 2.6

	May, 1980

	July, 1980 -- Rev. 2.2 consisting of fixing the M errors that
		come from forward references in FDB and FCB pseudo-ops.

	October, 1980 -- Rev. 2.3 consisting of updating the assembly
		language file and I/O routines to match and take
		advantage of BDS C V1.4.

	October, 1983 -- Rev. 2.4 consisting of adding the CPU pseudo-op,
		adding the 6801 CPU's extra opcodes, and speeding up the
		code a bit.

	September, 1984 -- Rev. 2.5 consisting of fixing bugs with the symbol
		table sort, the writing of files to specified drives, and the
		handling of blank input lines.

	June, 1985 -- Rev. 2.6 consisting of fixing a bug in the IF block
		nesting mechanism.

	Copyright (c) 1980,83,84,85 William C. Colley, III.

File:	a68.h

Global macro substitutions and external variable declarations.
*/

/*  For the benefit of BDS C V1.4's library functions:			*/

#include <bdscio.h>
			/*  Note:  The cross assembler will run more
				efficiently and with less head racket
				if the number of sectors of disk buffer
				specified by BDSCIO.H is at least 8.	*/

/*  Set input line length:  */

#define	LINLEN	120	/* Length of input line in characters.		*/

/*  Define symbol table parameters:  */

#define	SYMLEN	8	/*  Length of labels (must be an even number).	*/
#define	SYMBOLS	500	/*  Number of symbols in symbol table.		*/
#define	PADDING	"       "	/*  SYMLEN - 1 blanks.			*/

/*  Number of if statements that can be nested:  */

#define	IFDEPTH	16

#define	ON	1
#define	OFF	-1

/*  BDOS functions called directly:  */

#define	GETDISK	25	/*  Get currently logged in disk.		*/

/*  Define flag values:  */

#define	SKIP	TRUE	/*  Used with skip flag in getchr.		*/
#define	NOSKIP	FALSE
#define	BIGST	TRUE	/*  Used to tell kind of string in getitem.	*/
#define	SMALST	FALSE
#define	NOCODE	0	/*  Used to tell the hex generator what to do.	*/
#define	PUTCODE	1
#define	FLUSH	2
#define	NOMORE	3

/*  File descriptors:  */

#define	NOFILE	-1	/*  No file specified.				*/
#define	CONO	1	/*  Console output.				*/
#define	LST	2	/*  List device.				*/
#define	LODISK	4	/*  Lowest numbered disk.			*/

/*  Items can have the following attributes:  */

#define	ALPHA	0	/*  Alphabetic character.			*/
#define	NUMERIC	1	/*  Numeric digit (0-9).			*/
#define	END_LIN	2	/*  End-of-Line marker (cr or ;).		*/
#define	COMMA	3	/*  Field separator (,).			*/
#define	OPERATR	4	/*  Operator (* - GE < ( SHR etc.).		*/
#define	BAS_DES	5	/*  Base designator ($ % @).			*/
#define	QUOTE	6	/*  String delimiter (" ').			*/
#define	IMMED	7	/*  Immediate mode indicator (#).		*/
#define	VALUE	8	/*  Evaluated expression.			*/
#define	REGISTR	9	/*  Register designator (A B X).		*/
#define	BLANK	10	/*  White space (spc tab).			*/
#define	TRASH	11	/*  Other characters.				*/

/*  Some tokens for composite operators:  */

#define	NO_OPR	0	/*  No operator.				*/
#define	GRTEQ	1	/*  Greater than or equal to.			*/
#define	NOTEQ	2	/*  Not equal to.				*/
#define	LESEQ	3	/*  Less than or equal to.			*/
#define	AND	4	/*  And.					*/
#define	OR	5	/*  Or.						*/
#define	XOR	6	/*  Exclusive or.				*/
#define	NOT	7	/*  1's complement.				*/
#define	MOD	8	/*  Mod--divide and return remainder.		*/
#define	SHL	9	/*  Shift left.					*/
#define	SHR	10	/*  Shift right.				*/
#define	HIGH	11	/*  High byte of.				*/
#define	LOW	12	/*  Low byte of.				*/

/*  Operator precedence values:  */

#define	UOP1	0	/*  Unary +, unary -.				*/
#define	MULT	1	/*  *, /, MOD, SHL, SHR.			*/
#define	ADDIT	2	/*  Binary +, binary -.				*/
#define	RELAT	3	/*  >, =, <, >=, <>, <=.			*/
#define	UOP2	4	/*  NOT.					*/
#define	LOG1	5	/*  AND.					*/
#define	LOG2	6	/*  OR, XOR.					*/
#define	UOP3	7	/*  HIGH, LOW.					*/
#define	RPREN	8	/*  ).						*/
#define	LPREN	9	/*  (.						*/
#define	ENDEX	10	/*  CR, ;, ,.					*/
#define	START	11	/*  Start of expression.			*/

/*  Bits of opcode attribute byte.  */

#define	PSOP	0x80	/*  Pseudo-op.					*/
#define	DIROK	0x40	/*  Non-pseudo-op is OK for direct addressing.	*/
#define	IFGROUP	0x40	/*  Pseudo-op is IF, ELSE, ENDI.		*/
#define	REL	0x20	/*  Relative addressing required.		*/
#define	REGREQ	0x10	/*  A, B must be specified.			*/
#define	IMM16	0x08	/*  16-bit immediate addressing OK.		*/
#define	IMM8	0x04	/*  8-bit immediate addressing OK.		*/
#define	INDEX	0x02	/*  Indexed or extended addressing OK.		*/
#define	REGOPT	0x01	/*  A, B optional.				*/
#define	IS6801	0x11	/*  6801 opcode.				*/

/*  Set up disk I/O buffers:  */

struct _buf _source, _list, _hex, *source, *list, *hex;

/*  Set up the symbol table: */

struct symbtbl
{
	char symname[SYMLEN];
	unsigned symvalu;
}
symtbl[SYMBOLS], *symend, *sympoint;

/*  If stack and stack pointer.  */

char ifsp;
unsigned ifstack[IFDEPTH+1];

/*  Buffer to hold current line:  */

char *linptr, linbuf[LINLEN];

/*  Buffer to hold the code generated by a line.  */

char nbytes, binbuf[255];

/*  Buffers for the hex generator.  */

char chksum, hxbytes, *hxlnptr, hxlnbuf[44];

/*  Miscellanious mailboxes:  */

char errcode;		/*  Error records.				*/
int errcount;
char evalerr;		/*  Flag to tell of error in eval.		*/
char backflg, oldattr;	/*  Item push-back buffer.			*/
unsigned oldvalu;
char curdrive;		/*  Place to save drive that was current
			        disk when assembly started.		*/
char hexflg;		/*  Flag for asmline to tell hex
				generator what to do.			*/
char directok;		/*  All symbols on line pre-defined.		*/
unsigned pc;		/*  Assembly program counter.			*/
unsigned address;	/*  Address to be put into listed line.		*/
char pass;		/*  Which pass the assembly is in.		*/
char extend;		/*  6801 extensions enabled/disabled.		*/

/*  Some trivial functions:  */

#define	backchr	--linptr		/*  Push back a character.	*/

uffer to ho