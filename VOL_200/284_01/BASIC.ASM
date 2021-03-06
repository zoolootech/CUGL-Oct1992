;
;	PALO ALTO TINY BASIC
;	"PORTABLE 8080" VERSION
;

;	MEMORY CONFIGURATION
ITOP	EQU	0000H
LTOP	EQU	1000H
VTOP	EQU	1300H
LBUF	EQU	VTOP+0037H
LBFSZ	EQU	80
MSTK	EQU	LBUF+LBFSZ
TMSTK	EQU	MSTK+30
STACK	EQU	1400H

;	MAIN PROGRAM
	ORG	ITOP

ENTRY:	DI
	LXI	H,OTOP
	SHLD	OBTM
START:	LXI	SP,STACK
	EI
	CALL	CRLF
	LXI	D,OKMES
	SUB	A
	CALL	MSG
	LXI	H,$+7
	SHLD	CLABL
	LXI	H,0000H
	SHLD	FCNTR
	SHLD	RSTCK

EDITR:	MVI	A,'>'
	CALL	GETL
	PUSH	D
	LXI	D,LBUF
	CALL	GINT
	CALL	SKPBL
	MOV	A,H
	ORA	L
	POP	B
	JZ	KWCPR
INSRT:	DCX	D
	MOV	A,H
	STAX	D
	DCX	D
	MOV	A,L
	STAX	D
	PUSH	B
	PUSH	D
	MOV	A,C
	SUB	E
	PUSH	PSW
	CALL	SRCH
	PUSH	D
	JNZ	MOVE
	PUSH	D
	CALL	SKIPL
	POP	B
	LHLD	OBTM
	CALL	TRNSF
	MOV	H,B
	MOV	L,C
	SHLD	OBTM
MOVE:	POP	B
	LHLD	OBTM
	POP	PSW
	PUSH	H
	CPI	03H
	JZ	START
	ADD	L
	MOV	L,A
	MVI	A,00H
	ADC	H
	MOV	H,A
	LXI	D,VTOP
	CALL	COMPR
	JNC	ERSRY
	SHLD	OBTM
	POP	D
	CALL	TR2
	POP	D
	POP	H
	CALL	TRNSF
	JMP	EDITR

GETL:	CALL	PUT
	LXI	D,LBUF
GETL1:	CALL	GET
	CPI	7FH
	JZ	CRUB
	CPI	08H
	JZ	CRUB
	CPI	18H
	JZ	CCAN
	CALL	PUT
	STAX	D
	INX	D
	CPI	0DH
	RZ
	MOV	A,E
	CPI	LBUFL+LBFSZ
	JNZ	GETL1
CRUB:	MOV	A,E
	CPI	LBUFL
	JZ	CCAN
	DCX	D
	MVI	A,08H
	CALL	PUT
	MVI	A,20H
	CALL	PUT
	MVI	A,08H
	CALL	PUT
	JMP	GETL1
CCAN:	CALL	CRLF
	MVI	A,5CH
	JMP	GETL
LBUFL	EQU	LBUF AND 0FFH

SRCH:	MOV	A,H
	ORA	A
	JM	ERHOW
	LXI	D,OTOP
SRCH1:	PUSH	H
	LHLD	OBTM
	DCX	H
	CALL	COMPR
	POP	H
	RC
	LDAX	D
	SUB	L
	MOV	B,A
	INX	D
	LDAX	D
	SBB	H
	JC	SKPL1
	DCX	D
	ORA	B
	RET

SKIPL:	INX	D
SKPL1:	INX	D
SKPL2:	LDAX	D
	CPI	0DH
	JNZ	SKPL1
	INX	D
	JMP	SRCH1

TRNSF:	CALL	COMPR
	RZ
	LDAX	D
	STAX	B
	INX	D
	INX	B
	JMP	TRNSF

TR2:	MOV	A,B
	SUB	D
	JNZ	TR2E
	MOV	A,C
	SUB	E
	RZ
TR2E:	DCX	D
	DCX	H
	LDAX	D
	MOV	M,A
	JMP	TR2

KWCPR:	LXI	H,CMDKW-1

NXTKW:	CALL	SKPBL
NXTK1:	PUSH	D
KWC1:	LDAX	D
	INX	D
	CPI	'.'
	JZ	KWSRT
	INX	H
	CMP	M
	JZ	KWC1
	DCX	D
	SUB	A
	CMP	M
	JZ	EXEQT
KWSK1:	INX	H
	CMP	M
	JNZ	KWSK1
	INX	H
	INX	H
	POP	D
	JMP	NXTK1

KWSRT:	SUB	A
KWSK2:	INX	H
	CMP	M
	JNZ	KWSK2

EXEQT:	INX	H
	MOV	A,M
	INX	H
	MOV	H,M
	MOV	L,A
	POP	PSW
	PCHL

TEST:	XTHL
	CALL	SKPBL
	CMP	M
	INX	H
	JZ	TSTEQ
	PUSH	B
	MOV	C,M
	MVI	B,00H
	DAD	B
	POP	B
	DCX	D
TSTEQ:	INX	D
	INX	H
	XTHL
	RET

SKPBL:	LDAX	D
	CPI	' '
	RNZ
	INX	D
	JMP	SKPBL

NEW:	CALL	TSCR2
	LXI	H,OTOP
	SHLD	OBTM
STOP:	CALL	TSCR2
	JMP	START

GOSUB:	CALL	PSHV
	CALL	EEXPR
	PUSH	D
	CALL	SRCH
	JNZ	ERHW1
	LHLD	CLABL
	PUSH	H
	LHLD	RSTCK
	PUSH	H
	LXI	H,0000H
	SHLD	FCNTR
	DAD	SP
	SHLD	RSTCK
	JMP	RUN2

RETURN: CALL	TSCR2
	LHLD	RSTCK
	MOV	A,H
	ORA	L
	JZ	ERWHT
	SPHL
	POP	H
	SHLD	RSTCK
	POP	H
	SHLD	CLABL
	POP	D
	CALL	POPV
	JMP	ENDL

FOR:	CALL	PSHV
	CALL	LTSUB
	DCX	H
	SHLD	FCNTR
	LXI	H,KWTO-1
	JMP	NXTKW
FORTO:	CALL	EEXPR
	SHLD	FTOV
	LXI	H,KWSTP-1
	JMP	NXTKW
FSTEP:	CALL	EEXPR
	JMP	FOR0
FSTP1:	LXI	H,0001H
FOR0:	SHLD	FSTPV
	LHLD	CLABL
	SHLD	FLABL
	XCHG
	SHLD	FOBJ
	LXI	B,000AH
	LHLD	FCNTR
	XCHG
	MOV	H,B
	MOV	L,B
	DAD	SP
	DB	3EH
FOR3:	DAD	B
	MOV	A,M
	INX	H
	ORA	M
	JZ	FOR10
	MOV	A,M
	DCX	H
	CMP	D
	JNZ	FOR3
	MOV	A,M
	CMP	E
	JNZ	FOR3
FOR5:	XCHG
	LXI	H,0000H
	DAD	SP
	MOV	B,H
	MOV	C,L
	LXI	H,000AH
	DAD	D
	CALL	TR2
	SPHL
FOR10:	LHLD	FOBJ
	XCHG
	JMP	ENDL

NEXT:	CALL	TSTV
	JC	ERWHT
	SHLD	NCNTR
NEXT1:	PUSH	D
	XCHG
	LHLD	FCNTR
	MOV	A,H
	ORA	L
	JZ	ERWT1
	CALL	COMPR
	JZ	NEXT2
	POP	D
	CALL	POPV
	LHLD	NCNTR
	JMP	NEXT1
NEXT2:	MOV	E,M
	INX	H
	MOV	D,M
	LHLD	FSTPV
	PUSH	H
	DAD	D
	XCHG
	LHLD	FCNTR
	MOV	M,E
	INX	H
	MOV	M,D
	LHLD	FTOV
	POP	PSW
	ORA	A
	JP	NEXT4
	XCHG
NEXT4:	CALL	CMINT
	POP	D
	JC	NEXT5
	LHLD	FLABL
	SHLD	CLABL
	LHLD	FOBJ
	XCHG
	JMP	ENDL
NEXT5:	CALL	POPV
	JMP	ENDL

PSHV:	LXI	H,TMSTK
	CALL	TWSCP
	POP	B
	DAD	SP
	JNC	ERSRY
	LHLD	FCNTR
	MOV	A,H
	ORA	L
	JZ	NPSH
	LHLD	FOBJ
	PUSH	H
	LHLD	FLABL
	PUSH	H
	LHLD	FTOV
	PUSH	H
	LHLD	FSTPV
	PUSH	H
	LHLD	FCNTR
NPSH:	PUSH	H
	PUSH	B
	RET

POPV:	POP	B
	POP	H
	SHLD	FCNTR
	MOV	A,H
	ORA	L
	JZ	NPOP
	POP	H
	SHLD	FSTPV
	POP	H
	SHLD	FTOV
	POP	H
	SHLD	FLABL
	POP	H
	SHLD	FOBJ
NPOP:	PUSH	B
	RET

IFSTM:	CALL	EEXPR
	MOV	A,H
	ORA	L
	JNZ	NXTGO
REM:	LXI	H,0000H
	CALL	SKPL2
	JNC	RUN2
	JMP	START

LIST:	CALL	GINT
	CALL	TSCR2
	CALL	SRCH
LISTL:	JC	START
	CALL	WLINE
	CALL	BREAK
	CALL	SRCH1
	JMP	LISTL

WLINE:	LDAX	D
	MOV	L,A
	INX	D
	LDAX	D
	MOV	H,A
	INX	D
	MVI	C,04H
	CALL	WINT
	MVI	A,' '
	CALL	PUT
	SUB	A
	CALL	MSG
	RET

PRINT:	MVI	C,6
PRSC:	CALL	TEST
	DB	';'
	DB	PRCR-$-1
	CALL	CRLF
	JMP	NXTGO
PRCR:	CALL	TEST
	DB	0DH
	DB	PRSHA-$-1
	CALL	CRLF
	JMP	RUN1
PRSHA:	CALL	TEST
	DB	'#'
	DB	PRLTR-$-1
	CALL	EEXPR
	MOV	C,L
	JMP	PRCMA
PRLTR:	CALL	PRSUB
	JMP	PREXP
PRCMA:	CALL	TEST
	DB	','
	DB	PREND-$-1
	CALL	TSTSC
	JMP	PRSHA
PREND:	CALL	CRLF
	JMP	ENDL
PREXP:	CALL	EEXPR
	PUSH	B
	CALL	WINT
	POP	B
	JMP	PRCMA

PRSUB:	CALL	TEST
	DB	'"'
	DB	PR13-$-1
	MVI	A,'"'
	JMP	PR11
PR13:	CALL	TEST
	DB	27H
	DB	PR14-$-1
	MVI	A,27H
PR11:	CALL	MSG
	CPI	0DH
	POP	H
	JZ	RUN1
	JMP	PR12
PR14:	CALL	TEST
	DB	5FH
	DB	PR15-$-1
	MVI	A,8DH
	CALL	PUT
	CALL	PUT
	POP	H
PR12:	INX	H
	INX	H
	INX	H
	PCHL
PR15:	RET

WINT:	PUSH	D
LXI	D,000AH
	PUSH	D
	MOV	B,D
	DCR	C
	CALL	ABS
	JP	WINT1
	MVI	B,'-'
	DCR	C
WINT1:	PUSH	B
WINT2:	CALL	DIVID
	MOV	A,B
	ORA	C
	JZ	WINT3
	XTHL
	DCR	L
	PUSH	H
	MOV	H,B
	MOV	L,C
	JMP	WINT2
WINT3:	POP	B
WINT4:	DCR	C
	MOV	A,C
	ORA	A
	JM	WINT5
	MVI	A,' '
	CALL	PUT
	JMP	WINT4
WINT5:	MOV	A,B
	CALL	PUT
	MOV	E,L
WINT6:	MOV	A,E
	CPI	0AH
	POP	D
	RZ
	ADI	30H
	CALL	PUT
	JMP	WINT6

ERRIN:	LHLD	NCNTR
	SPHL
	POP	H
	SHLD	CLABL
	POP	D
	POP	D
INPUT:	PUSH	D
	CALL	PRSUB
	JMP	INPT2
INPT1:	CALL	TSTV
	JC	INPT6
	JMP	INPT4
INPT2:	PUSH	D
	CALL	TSTV
	JC	ERWHT
	LDAX	D
	MOV	C,A
	SUB	A
	STAX	D
	POP	D
	CALL	MSG
	MOV	A,C
	DCX	D
	STAX	D
INPT4:	PUSH	D
	XCHG
	LHLD	CLABL
	PUSH	H
	LXI	H,INPUT
	SHLD	CLABL
	LXI	H,0000H
	DAD	SP
	SHLD	NCNTR
	PUSH	D
	MVI	A,':'
	CALL	GETL
	LXI	D,LBUF
	CALL	EEXPR
	POP	D
	XCHG
	MOV	M,E
	INX	H
	MOV	M,D
	POP	H
	SHLD	CLABL
	POP	D
INPT6:	POP	PSW
	CALL	TEST
	DB	','
	DB	ENDL-$-1
	JMP	INPUT

LETDF:	CALL	TSCR1
	CPI	0DH
	JZ	ENDL
LET:	CALL	LTSUB
	CALL	TEST
	DB	','
	DB	ENDL-$-1
	JMP	LET

LTSUB:	CALL	TSTV
	JC	ERWHT
	PUSH	H
	CALL	TEST
	DB	'='
	DB	LERR-$-1
	CALL	EEXPR
	MOV	B,H
	MOV	C,L
	POP	H
	MOV	M,C
	INX	H
	MOV	M,B
	RET

GOTO:	CALL	EEXPR
	PUSH	D
	CALL	TSCR2
	CALL	SRCH
	JNZ	ERHW1
	POP	PSW
	JMP	RUN2

RUN:	CALL	TSCR2
	LXI	D,OTOP
RUN1:	LXI	H,0000H
	CALL	SRCH1
	JC	START
RUN2:	XCHG
	SHLD	CLABL
	XCHG
	INX	D
	INX	D
NXTGO:	CALL	BREAK
	LXI	H,STMKW-1
	JMP	NXTKW

ENDL:	CALL	TSTSC
LERR:	JMP	ERWHT

TSTSC:	CALL	TEST
	DB	';'
	DB	TSCR1-$-1
	POP	PSW
	JMP	NXTGO

TSCR1:	CALL	TEST
	DB	0DH
	DB	TSRTN-$-1
	POP	PSW
	JMP	RUN1
TSRTN:	RET

TSCR2:	CALL	SKPBL
	CPI	0DH
	RZ
	JMP	ERWHT

EEXPR:	CALL	EXPR
	PUSH	H
	LXI	H,ROPKW-1
	JMP	NXTKW
GE:	CALL	IFEXQ
	RC
	MOV	L,A
	RET
NE:	CALL	IFEXQ
	RZ
	MOV	L,A
	RET
GT:	CALL	IFEXQ
	RZ
	RC
	MOV	L,A
	RET
LE:	CALL	IFEXQ
	MOV	L,A
	RZ
	RC
	MOV	L,H
	RET
EQ:	CALL	IFEXQ
	RNZ
	MOV	L,A
	RET
LT:	CALL	IFEXQ
	RNC
	MOV	L,A
	RET
NOROP:	POP	H
	RET

IFEXQ:	MOV	A,C
	POP	H
	POP	B
	PUSH	H
	PUSH	B
	MOV	C,A
	CALL	EXPR
	XCHG
	XTHL
	CALL	CMINT
	POP	D
	LXI	H,0000H
	MVI	A,01H
	RET

EXPR:	CALL	TEST
	DB	'-'
	DB	EXPR1-$-1
	LXI	H,0000H
	JMP	NEGA1
EXPR1:	CALL	TEST
	DB	'+'
	DB	EXPR3-$-1
EXPR3:	CALL	TERM
EXPR2:	CALL	TEST
	DB	'+'
	DB	NEGA0-$-1
	PUSH	H
	CALL	TERM
	JMP	ADDBL
NEGA0:	CALL	TEST
	DB	'-'
	DB	EXPRT-$-1
NEGA1:	PUSH	H
	CALL	TERM
	CALL	TWSCP
ADDBL:	XCHG
	XTHL
	MOV	A,H
	XRA	D
	MOV	A,D
	DAD	D
	POP	D
	JM	EXPR2
	XRA	H
	JP	EXPR2
	JMP	ERHOW

TERM:	CALL	FACTR
MULT:	CALL	TEST
	DB	'*'
	DB	DIV-$-1
	PUSH	H
	CALL	FACTR
	MVI	B,00H
	CALL	ABS
	XCHG
	XTHL
	CALL	ABS
	MOV	A,H
	ORA	A
	JZ	MULT1
	MOV	A,D
	ORA	D
	XCHG
	JNZ	ERHW1
MULT1:	MOV	A,L
	MVI	L,0
	ORA	A
	JZ	TERM1
MULTL:	DAD	D
	JC	ERHW1
	DCR	A
	JNZ	MULTL
	JMP	TERM1
DIV:	CALL	TEST
	DB	'/'
	DB	EXPRT-$-1
	PUSH	H
	CALL	FACTR
	MVI	B,00H
	CALL	ABS
	XCHG
	XTHL
	CALL	ABS
	MOV	A,D
	ORA	E
	JZ	ERHW1
	PUSH	B
	CALL	DIVID
	MOV	H,B
	MOV	L,C
	POP	B
TERM1:	POP	D
	MOV	A,H
	ORA	A
	JM	ERHOW
	MOV	A,B
	ORA	A
	CM	TWSCP
	JMP	MULT

DIVID:	PUSH	H
	MOV	L,H
	MVI	H,00H
	CALL	DIVSB
	MOV	B,C
	MOV	A,L
	POP	H
	MOV	H,A
DIVSB:	MVI	C,0FFH
DIVS1:	INR	C
	CALL	DIFF
	JNC	DIVS1
	DAD	D
EXPRT:	RET

DIFF:	MOV	A,L
	SUB	E
	MOV	L,A
	MOV	A,H
	SBB	D
	MOV	H,A
	RET

ABS:	MOV	A,H
	ORA	A
	RP
TWSCP:	MOV	A,H
	CMA
	MOV	H,A
	MOV	A,L
	CMA
	MOV	L,A
	INX	H
	MOV	A,B
	XRI	80H
	MOV	B,A
	RET

CMINT:	MOV	A,H
	XRA	D
	JP	COMPR
	XCHG
COMPR:	MOV	A,H
	CMP	D
	RNZ
	MOV	A,L
	CMP	E
	RET

FACTR:	LXI	H,FNKW-1
	JMP	NXTKW
RND:	CALL	EXPR0
	MOV	A,H
	ORA	A
	JM	ERHOW
	ORA	L
	JZ	ERHOW
	PUSH	D
	PUSH	H
	LHLD	RWRK
;;	INX	H
	MOV	A,H
	ANI	07H
	MOV	H,A
;;	DCR	L
;	LXI	D,ITOP
;	DAD	D
RND1:	MOV	E,M
	INX	H
	MOV	D,M
	SHLD	RWRK
	POP	H
	XCHG
	PUSH	B
	CALL	DIVID
	POP	B
	POP	D
	INX	H
	RET

FNABS:	CALL	EXPR0
	CALL	ABS
	MOV	A,H
	ORA	H
	JM	ERHOW
	RET

RSIZE:	LHLD	OBTM
	PUSH	D
	XCHG
	LXI	H,VTOP
	CALL	DIFF
	POP	D
	RET

VAR:	CALL	TSTV
	JC	NUMB
	MOV	A,M
	INX	H
	MOV	H,M
	MOV	L,A
	RET

TSTV:	CALL	SKPBL
	SUI	'@'
	RC
	JNZ	TSTV1
	INX	D
	CALL	EXPR0
	DAD	H
	JC	ERHOW
	PUSH	D
	XCHG
	CALL	RSIZE
	CALL	COMPR
	JC	ERSY1
	LXI	H,VTOP
	CALL	DIFF
	POP	D
	RET
TSTV1:	CPI	'Z'+1-'@'
	CMC
	RC
	INX	D
	LXI	H,VTOP
	RLC
	ADD	L
	MOV	L,A
	MVI	A,00H
	ADC	H
	MOV	H,A
	RET

GINT:	LXI	H,0000H
	MOV	B,H
	CALL	SKPBL
GETI1:	CPI	'0'
	RC
	CPI	'9'+1
	RNC
	MVI	A,0F0H
	ANA	H
	JNZ	ERHOW
	INR	B
	PUSH	B
	MOV	B,H
	MOV	C,L
	DAD	H
	DAD	H
	DAD	B
	DAD	H
	LDAX	D
	INX	D
	ANI	0FH
	ADD	L
	MOV	L,A
	MVI	A,00H
	ADC	H
	MOV	H,A
	POP	B
	LDAX	D
	JM	ERHOW
	JMP	GETI1

NUMB:	CALL	GINT
	MOV	A,B
	ORA	A
	RNZ

EXPR0:	CALL	TEST
	DB	'('
	DB	ERWHT-$-1
	CALL	EEXPR
	CALL	TEST
	DB	')'
	DB	ERWHT-$-1
	RET

ERWHT:	PUSH	D
ERWT1:	LXI	D,WHTMS
	JMP	LERMS
ERHOW:	PUSH	D
ERHW1:	LXI	D,HOWMS
	JMP	LERMS
ERSRY:	PUSH	D
ERSY1:	LXI	D,SRYMS
LERMS:	SUB	A
	CALL	MSG
	POP	D
	LDAX	D
	PUSH	PSW
	SUB	A
	STAX	D
	LHLD	CLABL
	PUSH	H
	MOV	A,M
	INX	H
	ORA	M
	POP	D
	JZ	START
	MOV	A,M
	ORA	A
	JM	ERRIN
	CALL	WLINE
	DCX	D
	POP	PSW
	STAX	D
	MVI	A,3FH
	CALL	PUT
	SUB	A
	CALL	MSG
	JMP	START

MSG:	MOV	B,A
MSG1:	LDAX	D
	INX	D
	CMP	B
	RZ
	CALL	PUT
	CPI	0DH
	JNZ	MSG1
	RET

CRLF:	MVI	A,0DH
	JMP	PUT

;	KEY WORD TABLES

CMDKW:	DB	'LIST',0
	DW		LIST
	DB	'RUN',0
	DW		RUN
	DB	'NEW',0
	DW		NEW
STMKW:	DB	'NEXT',0
	DW		NEXT
	DB	'LET',0
	DW		LET
	DB	'INPUT',0
	DW		INPUT
	DB	'IF',0
	DW		IFSTM
	DB	'GOTO',0
	DW		GOTO
	DB	'GOSUB',0
	DW		GOSUB
	DB	'RETURN',0
	DW		RETURN
	DB	'REM',0
	DW		REM
	DB	'FOR',0
	DW		FOR
	DB	'PRINT',0
	DW		PRINT
	DB	'STOP',0
	DW		STOP
	DB	0
	DW		LETDF

KWTO:	DB	'TO',0
	DW		FORTO
	DB	0
	DW		ERWHT

KWSTP:	DB	'STEP',0
	DW		FSTEP
	DB	0
	DW		FSTP1

ROPKW:	DB	'>=',0
	DW		GE
	DB	'#',0
	DW		NE
	DB	'>',0
	DW		GT
	DB	'=',0
	DW		EQ
	DB	'<=',0
	DW		LE
	DB	'<',0
	DW		LT
	DB	0
	DW		NOROP

FNKW:	DB	'RND',0
	DW		RND
	DB	'ABS',0
	DW		FNABS
	DB	'SIZE',0
	DW		RSIZE
	DB	0
	DW		VAR

;	MESSAGES

OKMES:	DB	'OK'
	DB	0DH
HOWMS:	DB	'HOW?'
	DB	0DH
WHTMS:	DB	'WHAT?'
	DB	0DH
SRYMS:	DB	'SORRY'
	DB	0DH

;	I/O ROUTINES FOR PORTABLE 8080

;	SPECIAL ESCAPE VALUE
ESC	EQU	0EDH
APE	EQU	0EDH
CONST	EQU	2
CONIN	EQU	3
CONOT	EQU	4

;	INPUT/OUTPUT ROUTINE
PUT:	PUSH	B
	MOV	C,A
	DB	ESC, APE, CONOT
	CPI	0DH
	JNZ	PUT1
	MVI	C,0AH
	DB	ESC, APE, CONOT
PUT1:	POP	B
	RET

BREAK:	DB	ESC, APE, CONST
	ORA	A
	RZ
GET:	DB	ESC, APE, CONIN
	CPI	03H
	JZ	START
	CPI	1AH
	RNZ
	HLT

;	SYSTEM WORK AREAD
	ORG	LTOP
CLABL:	DS	2
RSTCK:	DS	2
NCNTR:	DS	2
FCNTR:	DS	2
FSTPV:	DS	2
FTOV:	DS	2
FLABL:	DS	2
FOBJ:	DS	2
RWRK:	DS	2
OBTM:	DS	2
;
OTOP	EQU	$
;
	END
