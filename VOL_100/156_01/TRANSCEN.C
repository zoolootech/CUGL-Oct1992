#include iolib.h
#include float.h
#include transcen.h
#asm
;
;	transcendental floating point routines
;
;	History...
;		26 Jun 83	Added 'pow'.
;		25 Jun83	Added 'sinh', 'cosh',
;			and 'tanh'.
;		8 Nov 82	Added 'sqrt'
;		7 Nov 82	Added 'atan2'
;		17 Oct 82	removed constant pi.
;		11 Oct 82	atn => atan
;			EXP constant fixed, several labels
;			changed, references to 'int' changed
;			to 'qfloor', 'qpi' inserted.
;		23 Sept 82	Added colons after labels,
;			declaring all coefficients with DBs.
;		4 Sept 82	Separated from rest of
;			floating point routines.
;		8 Aug 82	AS.COM format source code.
;		31 Jul 82	Translated to Zilog mnemonics.
;		30 Jul 82	Disassembled from Xitan Disk
;			Basic.
;
ONE:	DW	0
	DW	0
	DW	8100H
LOGCOEF: DB	6
	DB	23H,85H,0ACH,0C3H,11H,7FH
	DB	53H,0CBH,9EH,0B7H,23H,7FH
	DB	0CCH,0FEH,0A6H,0DH,53H,7FH
	DB	0CBH,5CH,60H,0BBH,13H,80H
	DB	0DDH,0E3H,4EH,38H,76H,80H
	DB	5CH,29H,3BH,0AAH,38H,82H
;
QLOG10:	CALL	QLOG	
	LD	BC,7F5EH	; 1/ln(10)
	LD	IX,5BD8H
	LD	DE,0A938H
	JP	FMUL	
;
QLOG:	CALL	SGN	
	OR	A
	JP	PE,ILLFCT
	LD	HL,FA+5	
	LD	A,(HL)
	LD	BC,8035H	; 1/sqrt(2)
	LD	IX,04F3H
	LD	DE,33FAH
	SUB	B
	PUSH	AF
	LD	(HL),B
	PUSH	DE
	PUSH	IX
	PUSH	BC
	CALL	FADD	
	POP	BC
	POP	IX
	POP	DE
	INC	B
	CALL	FDIV	
	LD	HL,ONE	
	CALL	HLSUB	
	LD	HL,LOGCOEF
	CALL	EVENPOL
	LD	BC,8080H	; -0.5
	LD	IX,0
	LD	DE,0
	CALL	FADD	
	POP	AF
	CALL	L247E	
	LD	BC,8031H	; ln(2)
	LD	IX,7217H
	LD	DE,0F7D2H
	JP	FMUL	
;
L265F:	POP	BC
	POP	IX
	POP	DE
	JP	FMUL
;
;
QEXP:	LD	BC,8138H	;1.442695041
	LD	IX,0AA3BH
	LD	DE,295CH	
	CALL	FMUL	
	LD	A,(FA+5)
	CP	88H
	JP	NC,DIV17
	CALL	PUSHFA	
	CALL	QFLOOR	
	POP	BC
	POP	IX
	POP	DE
	PUSH	AF
	CALL	FSUB	
	LD	HL,EXPCOEF
	CALL	POLY	
	LD	HL,FA+5	
	POP	AF
	OR	A
	JP	M,EXP5	
	ADD	A,(HL)
	DB	1	;"ignore next 2 bytes"
EXP5:	ADD	A,(HL)
	CCF	
	LD	(HL),A
	RET	NC
	JP	DIV17	
;
EXPCOEF: DB	10
	DB	0CCH,0D5H,45H,56H,15H,6AH
	DB	0CFH,37H,0A0H,92H,27H,6DH
	DB	0F5H,95H,0EEH,93H,00H,71H
	DB	0D0H,0FCH,0A7H,78H,21H,74H
	DB	0B1H,21H,82H,0C4H,2EH,77H
	DB	82H,58H,58H,95H,1DH,7AH
	DB	6DH,0CBH,46H,58H,63H,7CH
	DB	0E9H,0FBH,0EFH,0FDH,75H,7EH
	DB	0D2H,0F7H,17H,72H,31H,80H
	DB	0,0,0,0,0,81H
EVENPOL: CALL	PUSHFA	
	LD	DE,L265F
	PUSH	DE
	PUSH	HL
	CALL	LDBCFA	
	CALL	FMUL	
	POP	HL
POLY:	CALL	PUSHFA	
	LD	A,(HL)
	INC	HL
	CALL	DLOAD	
	DB	0FEH	;"ignore next byte"
POL3:	POP	AF
	POP	BC
	POP	IX
	POP	DE
	DEC	A
	RET	Z
	PUSH	DE
	PUSH	IX
	PUSH	BC
	PUSH	AF
	PUSH	HL
	CALL	FMUL	
	POP	HL
	CALL	LDBCHL	
	PUSH	HL
	CALL	FADD	
	POP	HL
	JR	POL3	
;
QCOS:	LD	HL,HALFPI
	CALL	HLADD	
QSIN:	CALL	PUSHFA	
	LD	BC,08349H	;6.283185308... = 2*pi
	LD	IX,00FDAH
	LD	DE,0A222H
	CALL	LDFABC	
	POP	BC
	POP	IX
	POP	DE
	CALL	FDIV	
	CALL	PUSHFA	
	CALL	QFLOOR	
	POP	BC
	POP	IX
	POP	DE
	CALL	FSUB	
	LD	HL,QUARTER
	CALL	HLSUB	
	CALL	SGN	
	SCF	
	JP	P,SIN5	
	CALL	ADDHALF	
	CALL	SGN	
	OR	A
SIN5:	PUSH	AF
	CALL	P,MINUSFA
	LD	HL,QUARTER
	CALL	HLADD	
	POP	AF
	CALL	NC,MINUSFA
	LD	HL,SINCOEF
	JP	EVENPOL
;
EE:	DW	058A4H	;2.718281828... = e
	DW	0F854H
	DW	0822DH
HALFPI:	DB	022H,0A2H,0DAH,00FH,049H,081H ; pi/2
;	DW	0A222H
;	DW	00FDAH
;	DW	08149H
QUARTER: DW	0
	DW	0
	DW	7F00H
SINCOEF: DB	7
	DB	90H,0BAH,34H,76H,6AH,82H
	DB	0E4H,0E9H,0E7H,4BH,0F1H,84H
	DB	0B1H,4FH,7FH,3BH,28H,86H
	DB	31H,0B6H,64H,69H,99H,87H
	DB	0E4H,36H,0E3H,35H,23H,87H
	DB	24H,31H,0E7H,5DH,0A5H,86H
	DB	21H,0A2H,0DAH,0FH,49H,83H
QTAN:	CALL	PUSHFA	
	CALL	QSIN	
	POP	BC
	POP	IX
	POP	DE
	CALL	L2895	
	EX	DE,HL
	CALL	LDFABC	
	CALL	QCOS	
	JP	DIV1	
;
#endasm
pow(x,y)	/* x to the power y */
double x,y;
{	return exp(log(x)*y);
}
sinh(x) double x;
{	double e;
	e=exp(x);
	return .5*(e-1./e);
}
cosh(x) double x;
{	double e;
	e=exp(x);
	return .5*(e+1./e);
}
tanh(x) double x;
{	double e;
	e=exp(x);
	return (e-1./e)/(e+1./e);
}
#asm
;
QATAN:	CALL	SGN	
	CALL	M,ODD		;negate argument & answer
	LD	A,(FA+5)
	CP	81H
	JR	C,ATAN5		;c => argument less than 1
	LD	BC,8100H	;1.0
	LD	IX,0
	LD	D,C
	LD	E,C
	CALL	FDIV	
	LD	HL,HLSUB
	PUSH	HL	;will subtract answer from pi/2
ATAN5:	LD	HL,ATNCOEF
	CALL	EVENPOL
	LD	HL,HALFPI	;may use for subtraction
	RET	
;
ATNCOEF: DB	13
	DB	14H,7H,0BAH,0FEH,62H,75H
	DB	51H,16H,0CEH,0D8H,0D6H,78H
	DB	4CH,0BDH,7DH,0D1H,3EH,7AH
	DB	1,0CBH,23H,0C4H,0D7H,7BH
	DB	0DCH,3AH,0AH,17H,34H,7CH
	DB	36H,0C1H,0A3H,81H,0F7H,7CH
	DB	0EBH,16H,61H,0AEH,19H,7DH
	DB	5DH,78H,8FH,60H,0B9H,7DH
	DB	0A2H,44H,12H,72H,63H,7DH
	DB	16H,62H,0FBH,47H,92H,7EH
	DB	0C0H,0F0H,0BFH,0CCH,4CH,7EH
	DB	7EH,8EH,0AAH,0AAH,0AAH,7FH
	DB	0F6H,0FFH,0FFH,0FFH,07FH,80H
;/*	arc tangent of y/x  */
;atan2(y,x) double x,y;
QATAN2:
;{	double a;
	PUSH BC
	PUSH BC
	PUSH BC
;	if(fabs(x)>=fabs(y))
	LD HL,8
	ADD HL,SP
	CALL DLOAD
	CALL DPUSH
	CALL QFABS
	POP BC
	POP BC
	POP BC
	PUSH HL
	LD HL,16
	ADD HL,SP
	CALL DLOAD
	CALL DPUSH
	CALL QFABS
	POP BC
	POP BC
	POP BC
	CALL CCGE
	LD A,H
	OR L
	JP Z,ATAN23
;		{a=atan(y/x);
	LD HL,0
	ADD HL,SP
	PUSH HL
	LD HL,16
	ADD HL,SP
	CALL DLOAD
	CALL DPUSH
	LD HL,16
	ADD HL,SP
	CALL DLOAD
	CALL DDIV
	CALL DPUSH
	CALL QATAN
	POP BC
	POP BC
	POP BC
	POP HL
	CALL DSTORE
;		if(x<0.)
	LD HL,8
	ADD HL,SP
	CALL DLOAD
	CALL DPUSH
	LD HL,ATAN27+0
	CALL DLOAD
	CALL DLT
	LD A,H
	OR L
	JR Z,ATAN22
;			{if(y>=0.) a=a+3.14159265358979;
	LD HL,14
	ADD HL,SP
	CALL DLOAD
	CALL DPUSH
	LD HL,ATAN27+6
	CALL DLOAD
	CALL DGE
	LD A,H
	OR L
	JR Z,ATAN20
	LD HL,0
	ADD HL,SP
	PUSH HL
	LD HL,2
	ADD HL,SP
	CALL DLOAD
	CALL DPUSH
	LD HL,ATAN27+12
	CALL DLOAD
	CALL DADD
	POP HL
	CALL DSTORE
;			else a=a-3.14159265358979;
	JR ATAN21
ATAN20:
	LD HL,0
	ADD HL,SP
	PUSH HL
	LD HL,2
	ADD HL,SP
	CALL DLOAD
	CALL DPUSH
	LD HL,ATAN27+18
	CALL DLOAD
	CALL DSUB
	POP HL
	CALL DSTORE
ATAN21:
;			}
;		}
ATAN22:
;	else
	JR ATAN26
ATAN23:
;		{a=-atan(x/y);
	LD HL,0
	ADD HL,SP
	PUSH HL
	LD HL,10
	ADD HL,SP
	CALL DLOAD
	CALL DPUSH
	LD HL,22
	ADD HL,SP
	CALL DLOAD
	CALL DDIV
	CALL DPUSH
	CALL QATAN
	POP BC
	POP BC
	POP BC
	CALL MINUSFA
	POP HL
	CALL DSTORE
;		if(y<0.) a=a-1.57079632679489;  /* pi/2 */
	LD HL,14
	ADD HL,SP
	CALL DLOAD
	CALL DPUSH
	LD HL,ATAN27+24
	CALL DLOAD
	CALL DLT
	LD A,H
	OR L
	JR Z,ATAN24
	LD HL,0
	ADD HL,SP
	PUSH HL
	LD HL,2
	ADD HL,SP
	CALL DLOAD
	CALL DPUSH
	LD HL,ATAN27+30
	CALL DLOAD
	CALL DSUB
	POP HL
	CALL DSTORE
;		else     a=a+1.57079632679489;
	JR ATAN25
ATAN24:
	LD HL,0
	ADD HL,SP
	PUSH HL
	LD HL,2
	ADD HL,SP
	CALL DLOAD
	CALL DPUSH
	LD HL,ATAN27+36
	CALL DLOAD
	CALL DADD
	POP HL
	CALL DSTORE
ATAN25:
;		}
ATAN26:
;	return a;
	LD HL,0
	ADD HL,SP
	CALL DLOAD
	POP BC
	POP BC
	POP BC
	RET
;}
ATAN27:	DB 0,0,0,0,0,0
	DB 0,0,0,0,0,0
	DB 33,162,218,15,73,130
	DB 33,162,218,15,73,130
	DB 0,0,0,0,0,0
	DB 34,162,218,15,73,129
	DB 34,162,218,15,73,129
#endasm
#asm
;double extra; /* current approximate root */
;sqrt(x) double x;
QSQRT:
;{	char *px,	/* points to x */
	PUSH BC
;	*pextra;		/* points to extra */
	PUSH BC
;	int i;		/* loop counter */
	PUSH BC
;	if(x==0.)return 0.;
	LD HL,8
	ADD HL,SP
	CALL DLOAD
	CALL DPUSH
	LD HL,SQRT10
	CALL DLOAD
	CALL DEQ
	LD A,H
	OR L
	JR Z,SQRT2
	LD HL,SQRT10
	JP SQRT9
;	if(x<0.)
SQRT2:
	LD HL,8
	ADD HL,SP
	CALL DLOAD
	CALL DPUSH
	LD HL,SQRT10
	CALL DLOAD
	CALL DLT
	LD A,H
	OR L
	JR Z,SQRT4
;		{err("ill sqrt"); return 0.;
	LD HL,SQRT12
	PUSH HL
	CALL QERR
	POP BC
	LD HL,SQRT10
	JP SQRT9
;		}
;	px=&x; pextra=&extra;	/* set the pointers */
SQRT4:
	LD HL,4
	ADD HL,SP
	PUSH HL
	LD HL,10
	ADD HL,SP
	CALL CCPINT
	LD HL,2
	ADD HL,SP
	PUSH HL
	LD HL,EXTRA
	CALL CCPINT
;	extra=.707;	/* initialize fraction at sqrt(.5) */
	LD HL,SQRT14
	CALL DLOAD
	LD HL,EXTRA
	CALL DSTORE
;	pextra[5]=(px[5]>>1)^64; /* answer exponent is half
;				of "x" exponent */
	LD HL,2
	ADD HL,SP
	CALL CCGINT
	PUSH HL
	LD HL,5
	POP DE
	ADD HL,DE
	PUSH HL
	LD HL,6
	ADD HL,SP
	CALL CCGINT
	PUSH HL
	LD HL,5
	POP DE
	ADD HL,DE
	CALL CCGCHAR
	PUSH HL
	LD HL,1
	POP DE
	CALL CCASR
	PUSH HL
	LD HL,64
	CALL CCXOR
	POP DE
	LD A,L
	LD (DE),A
;	i=5;	/* 5 iterations of Newton's algorithm */
	LD HL,0
	ADD HL,SP
	PUSH HL
	LD HL,5
	CALL CCPINT
;	while(i--)
SQRT6:
	LD HL,0
	ADD HL,SP
	PUSH HL
	CALL CCGINT
	DEC HL
	CALL CCPINT
	INC HL
	LD A,H
	OR L
	JR Z,SQRT8
;		{extra=(extra+x/extra);
	LD HL,EXTRA
	CALL DLOAD
	CALL DPUSH
	LD HL,14
	ADD HL,SP
	CALL DLOAD
	CALL DPUSH
	LD HL,EXTRA
	CALL DLOAD
	CALL DDIV
	CALL DADD
	LD HL,EXTRA
	CALL DSTORE
;		pextra[5]--;	/* /2 */
	LD HL,2
	ADD HL,SP
	CALL CCGINT
	PUSH HL
	LD HL,5
	POP DE
	ADD HL,DE
	PUSH HL
	CALL CCGCHAR
	DEC HL
	POP DE
	LD A,L
	LD (DE),A
	INC HL
;		}
	JR SQRT6
SQRT8:
;	return extra;
	LD HL,EXTRA
SQRT9:	CALL DLOAD
	POP BC
	POP BC
	POP BC
	RET
;}
SQRT10:	DB 0,0,0,0,0,0
SQRT12:	DB 'ill sqrt',0
SQRT14:	DB 70,182,243,253,52,128
#endasm
                                           