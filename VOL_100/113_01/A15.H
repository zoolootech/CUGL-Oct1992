/*
	HEADER:		CUG113;
	TITLE:		1802 Cross-Assembler (BDS C Version);
	FILENAME:	A15.H;
	VERSION:	1.2;
	DATE:		07/22/1985;

	DESCRIPTION:	"This program lets you use your CP/M-80-based computer
			to assemble code for the RCA 1802, 1804, 1805, 1805A,
			1806, AND 1806A microprocessors.  The program is
			written in BDS C for the best possible performance on
			8-bit machines.  All assembler features are supported
			except relocation, linkage, listing control, and
			macros.";

	KEYWORDS:	Software Development, Assemblers, Cross-Assemblers,
			RCA, CDP1802, CDP1805A;

	SEE-ALSO:	CUG149, 1805A Cross-Assembler (Portable);

	SYSTEM:		CP/M-80;
	COMPILERS:	BDS C;

	WARNINGS:	"This package is specifically tailored to CP/M-80
			machines and the rather non-standard, but high-
			performance BDS C compiler.  For other environments,
			use the portable version of this package on CUG149.";

	AUTHORS:	William C. Colley III;
*/

/*
	1805A Cross-Assembler  v 1.2

	Copyright (c) 1980, 82, 83, 85 William C. Colley, III.

	July 1982 -- Adapted from my 1802 cross assembler.  WCC3.

	Vers 1.0 -- March 1983 -- Added 1805A opcodes to the 1805 set.  WCC3.

	Vers 1.1 -- March 1983 -- Added CPU pseudo-op to combine 1802 and 1805A
			cross-assemblers into a single program.  WCC3.

	Vers 1.2 -- June 1985 -- Fixed IF block nesting mechanism bug and bug
			in 1805A SCAL opcode.  WCC3.

File:	a15.h

Global macro substitutions and external variable declarations.
*/

/*  For the benefit of the library's I/O drivers:  */

#include <bdscio.h>

/*  Set input line length:  */

#define	LINLEN	120

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
#define	DEFEXT	FALSE	/*  Used to set default value of extend.	*/
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
#define	QUOTE	5	/*  String delimiter (" ').			*/
#define	VALUE	6	/*  Evaluated expression.			*/
#define	BLANK	7	/*  White space (spc tab).			*/
#define	TRASH	8	/*  Other characters.				*/

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
#define	IFGROUP	0x40	/*  Pseudo-op is IF, ELSE, ENDI.		*/
#define	XOPC	0x40	/*  Machine opcode is 2 bytes.			*/
#define	PROCNUM	0x38	/*  Mask to get processor number for opcode.	*/
#define	BYTES	0x07	/*  Mask to get number of bytes in opcode.	*/

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
int ifstack[IFDEPTH+1];

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
char extend;		/*  Flag to tell getopcod() whether the 1805A
				extended opcodes are allowed or not.	*/

/*  Some trivial functions:  */

#define	backchr	linptr--		/*  Push back a character.	*/
, linbuf[LINLEN];

/*  Buffer to hold the code generated by a line.  */

char nbytes, binbuf[255];

/*  Buffers for the 