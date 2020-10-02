	TITLE	'Machine Language Functions for 1805A Cross-Assembler V 1.2'
	PAGE	60
;
;	1805A Cross-Assembler Version 1.2
;
;	Copyright (c) 1980, 82, 83, 85 William C. Colley, III.
;
;	July 1982 -- Adapted from my 1802 cross-assembler.  WCC3.
;
;	Vers 1.0 -- March 1983 -- Added 1805A opcodes to the 1805 set.  WCC3.
;
;	Vers 1.1 -- March 1983 -- Added CPU pseudo-op to combine 1802 and 1805A
;			cross-assemblers into a single program.  WCC3.
;
;	Vers 1.2 -- June 1985 -- Fixed IF block nesting mechanism bug and bug
;			in 1805A SCAL opcode.  WCC3.
;
; File:	A15TBL2.ASM
;
; Machine Language Functions -- Module #2 of 2.
;
	MACLIB	CMAC
;
; Here I build the directory to the functions in this CRL file:
;
	DIRECT
	DEFINE	GETATTR		;First entry.
	DEFINE	GETOPR		;Second entry.
	DEFINE	NUMOPRS		;Third entry.
	ENDDIR
;
; Arguments are passed via offsets from the stack pointer.
; They are defined as follows:
;
ARG0	EQU	2		;First argument.
ARG1	EQU	4		;Second argument.
ARG2	EQU	6		;Third argument.
ARG3	EQU	8		;Fourth argument.
;
	PAGE
;
; This function removes the parity bit from its argument and looks it up in
; a table.  The table classifies the character into one of the following bins
; returning the appropriate code.
;
ALPHA	EQU	0	;Alphabetic.
NUMERC	EQU	1	;Numeric (0-9).
ENDLIN	EQU	2	;End of line markers (CR ;).
COMMA	EQU	3	;Field separators (,).
OPERAT	EQU	4	;Operators (* + / - > < = ( )).
QUOTE	EQU	5	;String delimiters (' ").
BLANK	EQU	7	;White space character (SPC TAB).
TRASH	EQU	8	;Other control characters.
;
; The function is called as follows:
;
;	getattr(byte);
;
;	byte	Character value to be looked up.
;
BYTE	EQU	ARG0
;
	PRELUDE	GETATTR
;
	LXI	H, BYTE		;Get byte to look up.
	DAD	SP
	MOV	A,M
	ANI	7FH		;Strip parity bit.
;
	RELOC	<LXI H,>,ATTTBL	;Index into table.
	ADD	L
	MOV	L,A
	MVI	A, 0
	ADC	H
	MOV	H,A
;
	MOV	L,M		;Return result.
	MVI	H, 0
	RET
;
; Attribute table itself:
;
;	First a fill macro to shorten the source code:
;
BLOCK	MACRO	VALU,SIZE
	REPT	SIZE
	DB	VALU
	ENDM
	ENDM
;
ATTTBL:	BLOCK	TRASH,9		; ^@ thru ^H
	DB	BLANK		; TAB
	DB	ENDLIN		; LF
	DB	TRASH		; ^K
	DB	TRASH		; FF
	DB	TRASH		; CR
	BLOCK	TRASH,18	; All other control characters
	DB	BLANK		; SPC
	DB	ALPHA		; Exclamation point.
	DB	QUOTE		; "
	DB	ALPHA		; #
	DB	ALPHA		; $
	DB	ALPHA		; %
	DB	ALPHA		; &
	DB	QUOTE		; '
	DB	OPERAT		; (
	DB	OPERAT		; )
	DB	OPERAT		; *
	DB	OPERAT		; +
	DB	COMMA		; ,
	DB	OPERAT		; -
	DB	ALPHA		; .
	DB	OPERAT		; /
	BLOCK	NUMERC,10	; 0 thru 9
	DB	ALPHA		; :
	DB	ENDLIN		; ;
	DB	OPERAT		; <
	DB	OPERAT		; =
	DB	OPERAT		; >
	DB	ALPHA		; ?
	DB	ALPHA		; @
	BLOCK	ALPHA,26	; A thru Z
	DB	ALPHA		; [
	DB	ALPHA		; \
	DB	ALPHA		; ]
	DB	ALPHA		; ^
	DB	ALPHA		; _
	DB	ALPHA		; `
	BLOCK	ALPHA,26	; small A thru small Z
	DB	ALPHA		; {
	DB	ALPHA		; |
	DB	ALPHA		; }
	DB	ALPHA		; ~
	DB	TRASH		; RUB
;
	POSTLUDE GETATTR
;
	PAGE
;
; This function gets operator number num from the operator table.  The function
; returns 0 if the operator was not found, 1 if it was.
;
; Function is called as follows:
;
;	getopr(num,opratr,bs,token);
;
;	num	Character containing the number of the desired entry.
;			0 represents the first entry in the table.  The
;			table is in alphabetical order for the benefit
;			of the binary searching routine (written in C).
;	opratr	Pointer to a 5-character array which will receive the
;			operator's name (null terminated).
;	bs	A dud argument to maintain compatibility with getopc.
;	token	A pointer to a character that will receive the operator's
;			token byte.
;
NUM	EQU	ARG0
OPRATR	EQU	ARG1
BS	EQU	ARG2
TOKEN	EQU	ARG3
;
	PRELUDE	GETOPR
;
	LXI	H, NUM		;Get entry number.
	DAD	SP
	MOV	A,M
	CPI	OPRTLL		;Entry in table?
	LXI	H, 0		;If not, return 0.
	RNC
;
	MOV	L,A		;Find entry by computing
	ADD	A		;(5 * num) + table base.
	ADD	A
	ADD	L
	MOV	L,A
	RELOC	<LXI D,>,OPRTBL
	DAD	D
	MOV	E,L
	MOV	D,H
;
	LXI	H, OPRATR	;Find operator return area.
	DAD	SP
	MOV	A,M
	INX	H
	MOV	H,M
	MOV	L,A
;
	PUSH	B		;Move operator name to return area.
	MVI	C, 4
MOVOPR:	LDAX	D
	INX	D
	MOV	M,A
	INX	H
	DCR	C
	RELOC	JNZ,MOVOPR
	POP	B
;
	MVI	M, 0		;Terminate operator name.
;
	LXI	H, TOKEN	;Find token return area.
	DAD	SP
	MOV	A,M
	INX	H
	MOV	H,M
	MOV	L,A
;
	LDAX	D		;Move token to return area.
	MOV	M,A
;
	LXI	H, 1		;Return 1 for successful get.
	RET
;
; The operator table itself:
;
;	The operator tokens are as follows:
;
GETKN	EQU	1
NETKN	EQU	2
LETKN	EQU	3
ANDTKN	EQU	4
ORTKN	EQU	5
XORTKN	EQU	6
NOTTKN	EQU	7
MODTKN	EQU	8
SHLTKN	EQU	9
SHRTKN	EQU	10
HITKN	EQU	11
LOWTKN	EQU	12
GTTKN	EQU	'>'
EQTKN	EQU	'='
LTTKN	EQU	'<'
;
OPRTBL:	DB	'AND', 0,	ANDTKN
	DB	'EQ', 0, 0,	EQTKN
	DB	'GE', 0, 0,	GETKN
	DB	'GT', 0, 0,	GTTKN
	DB	'HIGH',		HITKN
	DB	'LE', 0, 0,	LETKN
	DB	'LOW', 0,	LOWTKN
	DB	'LT', 0, 0,	LTTKN
	DB	'MOD', 0,	MODTKN
	DB	'NE', 0, 0,	NETKN
	DB	'NOT', 0,	NOTTKN
	DB	'OR', 0, 0,	ORTKN
	DB	'SHL', 0,	SHLTKN
	DB	'SHR', 0,	SHRTKN
	DB	'XOR', 0,	XORTKN
;
OPRTLL	EQU	($-OPRTBL)/5	;Calculate length of table.
;
	POSTLUDE GETOPR
;
	PAGE
;
; This function returns the number of operators in the operator table
; for the benefit of the binary searching routine (written in C).
;
; This function is called as follows:
;
;	numoprs();
;
	PRELUDE	NUMOPRS
;
	LXI	H, OPRTLL	;Return number of opcodes.
	RET
;
	POSTLUDE NUMOPRS
;
; End of package:
;
	END
itself:
;
;	The operator tokens are as follows:
;
GETKN	EQU	1
NETKN	EQU	2
LETKN	EQU	3
ANDTKN	EQU	4
ORT