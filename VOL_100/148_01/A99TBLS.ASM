
TPALOC		EQU	0100H






base:	equ 0000h
fcb:	equ base+5ch
tbuff:	equ base+80h
bdos:	equ base+5
tpa:	equ base+100h
nfcbs:	equ 9
errorv:	equ 255
cccorg:	equ tpa


ram:	equ cccorg+52Bh
args:	equ ram+37

cr:	equ 0dh
lf:	equ 0ah
newlin:	equ lf
tab:	equ 9
bs:	equ 08h
cntrlc:	equ 3

ma1toh:	equ cccorg+20ah
ma2toh:	equ cccorg+213h
ma3toh:	equ ma2toh+6
ma4toh:	equ ma2toh+12
ma5toh:	equ ma2toh+18
ma6toh:	equ ma2toh+24
ma7toh:	equ ma2toh+30

arghak:	equ ma2toh+36

arg1:	equ args
arg2:	equ args+2
arg3:	equ args+4
arg4:	equ args+6
arg5:	equ args+8
arg6:	equ args+10
arg7:	equ args+12


		ORG	TPALOC+200H
		DB	0,0,0,0,0
GETOPC$BEG	EQU	$-TPALOC

NUM	EQU	ARG1
OPCODE	EQU	ARG2
VALUE	EQU	ARG3
ATTRIB1	EQU	ARG4

		DB	0
		DW	GETOPC$END-$-2
GETOPC$STRT	EQU	$
GETOPC$EF$GETOPC	EQU	GETOPC$STRT

GETOPC$STRTC	EQU	$
	CALL	ARGHAK
	LDA	NUM
	CPI	OPTBLL
	LXI	H, 0
	RNC

	PUSH	B
	PUSH	D
	MOV	L,A
	DAD	H
	MOV	C,L
	MOV	B,H
	DAD	H
	DAD	B
	MOV	C,A
	MVI	B,0	
	DAD	B
GETOPC$R000	EQU	$+1-GETOPC$STRT
	LXI 	B,GETOPC$L$OPCTBL
	DAD	B
	MOV	C,L
	MOV	B,H
	INX	H
	INX	H
	INX	H
	INX	H
	PUSH	H

	MVI	E, 4
	LHLD	OPCODE
GETOPC$L$MOVOPC		EQU	$-GETOPC$STRT
       	LDAX	B
	CPI	' '
GETOPC$R001	EQU	$+1-GETOPC$STRT
	JZ	GETOPC$L$ENDOPC
	INX	B
	MOV	M,A
	INX	H
	DCR	E
GETOPC$R002	EQU	$+1-GETOPC$STRT
	JNZ	GETOPC$L$MOVOPC

GETOPC$L$ENDOPC		EQU	$-GETOPC$STRT
       	MVI	M, 0
	LHLD	ATTRIB1
	POP	B
	LDAX	B
	INX	B
	MOV	M,A

	LHLD	VALUE
	LDAX	B
	INX	B
	MOV	M,A
	INX	H
	LDAX	B
	MOV	M,A

	LXI	H, 1
	POP	D
	POP	B
	RET

OPCTBL0	EQU	$

GETOPC$L$OPCTBL		EQU	$-GETOPC$STRT
      	DB	'A   ',09H		;MNEMONIC FOLLOWED BY ATTRRIBUTE BYTE
	DW	0A000H
	DB	'AB  ',09H
	DW	0B000H
	DB	'ABS ',01H
	DW	0740H	
	DB	'AI  ', 35H
	DW	0220H
	DB	'ANDI', 35H
	DW	0240H
	DB	'B   ',01H
	DW	0440H
	DB	'BIND',01H		;TMS99000 INSTRUCTION
	DW	0140H
	DB	'BL  ',01H
	DW	0680H
	DB	'BLSK',35H		;TMS99000 INSTRUCTION
	DW	00B0H
	DB	'BLWP',01H
	DW	0400H
	DB	'BSS ', 80H
	DW	0005H
	DB	'BYTE', 80H
	DW	0002H
	DB	'C   ', 09H
	DW	8000H
	DB	'CB  ', 09H
	DW	9000H
	DB	'CI  ',35H
	DW	0280H
	DB	'CKOF',00H
	DW	03C0H
	DB	'CKON',00H
	DW	03A0H
	DB	'CLR ',01H
	DW	04C0H
	DB	'COC ',19H
	DW	2000H
	DB	'CZC ',19H
	DW	2400H
	DB	'DEC ',01H
	DW	0600H
	DB	'DECT',01H
	DW	0640H
	DB	'DIV ',19H
	DW	3C00H
	DB	'DIVS',01H		;TMS99000 INSTRUCTION
	DW	0180H
	DB	'DXOP',0C0H		; Define Extended Operation 
	DW	000CH
	DB	'ELSE', 0C0H
	DW	0007H
	DB	'END ', 80H
	DW	0006H
	DB	'ENDI', 0C0H
	DW	0008H
	DB	'EQU ', 80H
	DW	0001H
	DB	'EVEN',80H
	DW	000BH
	DB	'IDLE',00H
	DW	0340H
	DB	'IF  ',0C0H
	DW	0009H
	DB	'INC ',01H
	DW	0580H
	DB	'INCT',01H
	DW	05C0H
	DB	'INV ',01H
	DW	0540H
	DB	'JEQ ',02H
	DW	1300H
	DB	'JGT ',02H
	DW	1500H
	DB	'JH  ',02H
	DW	1B00H
	DB	'JHE ',02H
	DW	1400H
	DB	'JL  ',02H
	DW	1A00H
	DB	'JLE ',02H
	DW	1200H
	DB	'JLT ',02H
	DW	1100H
	DB	'JMP ',02H
	DW	1000H
	DB	'JNC ',02H
	DW	1700H
	DB	'JNE ',02H
	DW	1600H
	DB	'JNO ',02H
	DW	1900H
	DB	'JOC ',02H
	DW	1800H
	DB	'JOP ',02H
	DW	1C00H
	DB	'LDCR',19H
	DW	3000H
	DB	'LI  ',35H
	DW	0200H
	DB	'LIMI',06H
	DW	0300H
	DB	'LREX',00H
	DW	03E0H
	DB	'LST ',05H		;TMS99000 INSTRUCTION
	DW	0080H
	DB	'LWP ',05H		;TMS99000 INSTRUCTION
	DW	0090H
	DB	'LWPI',06H
	DW	02E0H
	DB	'MOV ',09H
	DW	0C000H
	DB	'MOVB',09H
	DW	0D000H
	DB	'MPY ',19H
	DW	3800H
	DB	'MPYS',01H		;TMS99000 INSTRUCTION
	DW	01C0H
	DB	'NEG ',01H
	DW	0500H
	DB	'NOP ',00H
	DW	1000H
	DB	'ORG ',80H
	DW	0000H
	DB	'ORI ',35H
	DW	0260H
	DB	'RSET',00H
	DW	0360H
	DB	'RT  ',00H
	DW	045BH
	DB	'RTWP',00H
	DW	0380H
	DB	'S   ',09H
	DW	6000H
	DB	'SB  ',09H
	DW	7000H
	DB	'SBO ',04H
	DW	1D00H
	DB	'SBZ ',04H
	DW	1E00H
	DB	'SET ',80H
	DW	000AH
	DB	'SETO',01H
	DW	0700H
	DB	'SLA ',2DH
	DW	0A00H
	DB	'SOC ',09H
	DW	0E000H
	DB	'SOCB',09H
	DW	0F000H
	DB	'SRA ',2DH
	DW	0800H
	DB	'SRC ',2DH
	DW	0B00H
	DB	'SRL ',2DH
	DW	0900H
	DB	'STCR',09H
	DW	3400H
	DB	'STST',05H
	DW	02C0H
	DB	'STWP',05H
	DW	02A0H
	DB	'SWPB',01H
	DW	06C0H
	DB	'SZC ',09H
	DW	4000H
	DB	'SZCB',09H
	DW	5000H
	DB	'TB  ',04H
	DW	1F00H
	DB	'TCMB',07H		;TMS99105 INSTRUCTION
	DW	0C0AH			
	DB	'TEXT', 80H
	DW	0004H
	DB	'TMB ',07H		;TMS99105 INSTRUCTION
	DW	0C09H
	DB	'TROF',00H
	DW	03C0H
	DB	'TRON',00H
	DW	03E0H
	DB	'TSMB',07H		;TMS99105 INSTRUCTION
	DW	0C0BH
	DB	'WORD',80H
	DW	0003H
	DB	'WREN',00H
	DW	03A0H
	DB	'X   ',01H
	DW	0480H
	DB	'XOP ',19H
	DW	2C00H
	DB	'XOR ',19H
	DW	2800H

OPTBLL	EQU	($-OPCTBL0)/7

GETOPC$END	EQU	$
		DW	3
		DW	GETOPC$R000
		DW	GETOPC$R001
		DW	GETOPC$R002


	
NUMOPCS$BEG	EQU	$-TPALOC

		DB	0
		DW	NUMOPCS$END-$-2
NUMOPCS$STRT	EQU	$
NUMOPCS$EF$NUMOPCS	EQU	NUMOPCS$STRT

NUMOPCS$STRTC	EQU	$
	LXI	H,OPTBLL
	RET
	
NUMOPCS$END	EQU	$
		DW	0


ALPHA	EQU	0
NUMERC	EQU	1
ENDLIN	EQU	2
COMMA	EQU	3
OPERAT	EQU	4
BASDES	EQU	5
QUOTE	EQU	6
BLANK	EQU	10
TRASH	EQU	11
BLOCK	MACRO	VALU,SIZE
	REPT SIZE
	DB	VALU
	ENDM
	ENDM

GETATTR$BEG	EQU	$-TPALOC

		DB	0
		DW	GETATTR$END-$-2
GETATTR$STRT	EQU	$
GETATTR$EF$GETATTR	EQU	GETATTR$STRT

GETATTR$STRTC	EQU	$
	CALL	MA1TOH
	ANI	7FH
GETATTR$R000	EQU	$+1-GETATTR$STRT
	LXI 	H,GETATTR$L$ATTTBL
	ADD	L
	MOV	L,A
	MVI	A, 0
	ADC	H
	MOV	H,A

	MOV	L,M
	MVI	H, 0
	RET

GETATTR$L$ATTTBL		EQU	$-GETATTR$STRT
       	BLOCK	TRASH,9
	DB	BLANK
	DB	ENDLIN
	DB	TRASH
	DB	TRASH
	DB	TRASH
	BLOCK	TRASH,18
	DB	BLANK
	DB	ALPHA
	DB	QUOTE
	DB	BASDES
	DB	OPERAT
	DB	BASDES
	DB	ALPHA
	DB	QUOTE
	DB	OPERAT
	DB	OPERAT
	DB	OPERAT
	DB	OPERAT
	DB	COMMA
	DB	OPERAT
	DB	ALPHA
	DB	OPERAT
	BLOCK	NUMERC,10
	DB	ALPHA
	DB	ENDLIN
	DB	OPERAT
	DB	OPERAT
	DB	OPERAT
	DB	ALPHA
	DB	OPERAT
	BLOCK	ALPHA,26
	DB	ALPHA
	DB	ALPHA
	DB	ALPHA
	DB	ALPHA
	DB	ALPHA
	DB	ALPHA
	BLOCK	ALPHA,26
	DB	ALPHA
	DB	ALPHA
	DB	ALPHA
	DB	ALPHA
	DB	TRASH
GETATTR$END	EQU	$
		DW	1
		DW	GETATTR$R000


OPRATR	EQU	ARG2
TOKEN	EQU	ARG4


GETOPR$BEG	EQU	$-TPALOC

		DB	0
		DW	GETOPR$END-$-2
GETOPR$STRT	EQU	$
GETOPR$EF$GETOPR	EQU	GETOPR$STRT

GETOPR$STRTC	EQU	$
	CALL	ARGHAK
	LDA	NUM
	CPI	OPRTLL
	LXI	H, 0
	RNC

	PUSH	B
	PUSH	D
	MOV	L,A
	ADD	A
	ADD	A
	ADD	L
	MOV	L,A
GETOPR$R000	EQU	$+1-GETOPR$STRT
	LXI 	b,GETOPR$L$OPRTBL
	DAD	B
	MOV	C,L
	MOV	B,H
	INX	H
	INX	H
	INX	H
	INX	H
	PUSH	H

	MVI	E, 4
	LHLD	OPRATR
GETOPR$L$MOVOPR		EQU	$-GETOPR$STRT
       	LDAX	B
	CPI	' '
GETOPR$R001	EQU	$+1-GETOPR$STRT
	JZ	GETOPR$L$ENDOPR
	INX	B
	MOV	M,A
	INX	H
	DCR	E
GETOPR$R002	EQU	$+1-GETOPR$STRT
	JNZ	GETOPR$L$MOVOPR

GETOPR$L$ENDOPR		EQU	$-GETOPR$STRT
       	MVI	M, 0
	LHLD	TOKEN
	POP	B
	LDAX	B
	MOV	M,A

	LXI	H, 1
	POP	D
	POP	B
	RET

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

OPRTBL0	equ	$
GETOPR$L$OPRTBL		EQU	$-GETOPR$STRT
       	DB	'AND ', ANDTKN
	DB	'EQ  ', EQTKN
	DB	'GE  ', GETKN
	DB	'GT  ', GTTKN
	DB	'HIGH', HITKN
	DB	'LE  ', LETKN
	DB	'LOW ', LOWTKN
	DB	'LT  ', LTTKN
	DB	'MOD ', MODTKN
	DB	'NE  ', NETKN
	DB	'NOT ', NOTTKN
	DB	'OR  ', ORTKN
	DB	'SHL ', SHLTKN
	DB	'SHR ', SHRTKN
	DB	'XOR ', XORTKN

OPRTLL	EQU	($-OPRTBL0)/5
GETOPR$END	EQU	$
		DW	3
		DW	GETOPR$R000
		DW	GETOPR$R001
		DW	GETOPR$R002


	
NUMOPRS$BEG	EQU	$-TPALOC

		DB	0
		DW	NUMOPRS$END-$-2
NUMOPRS$STRT	EQU	$
NUMOPRS$EF$NUMOPRS	EQU	NUMOPRS$STRT

NUMOPRS$STRTC	EQU	$
NUMOPRS$L$STBOD5		EQU	$-NUMOPRS$STRT
       	LXI	H, OPRTLL
	RET


NUMOPRS$END	EQU	$
		DW	0



END$CRL		EQU	$-TPALOC

		ORG	TPALOC

; Directory:
		DB	'GETOP','C'+80H
		DW	GETOPC$BEG
		DB	'NUMOPC','S'+80H
		DW	NUMOPCS$BEG
		DB	'GETATT','R'+80H
		DW	GETATTR$BEG
		DB	'GETOP','R'+80H
		DW	GETOPR$BEG
		DB	'NUMOPR','S'+80H
		DW	NUMOPRS$BEG
		DB	80H
		DW	END$CRL
		END
KN	EQU	'='
LTTKN	EQU	'<'

OPRTBL0	equ	$
GETOPR$L$OPRTBL		EQU	$-GETOPR$STRT
       	DB	'AND ', ANDTKN
	DB	'EQ  ', 