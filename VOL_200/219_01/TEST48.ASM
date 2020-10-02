;			8048 Opcodes in Numerical Order
;
; This serves as a test file for my Portable 8048 Cross-Assembler.
;
IMMED	EQU	12H

;
; 000H - 00FH
;
	NOP
				;001H is unused.
	OUTL	BUS, A
	ADD	A, #IMMED
	JMP	0034H
	EN	I
				;006H is unused.
	DEC	A
	INS	A, BUS
	IN	A, P1
	IN	A, P2
				;00BH is unused.
	MOVD	A, P4
	MOVD	A, P5
	MOVD	A, P6
	MOVD	A, P7

;
; 010H - 01FH.
;
	INC	@R0
	INC	@R1
	JB0	$
	ADDC	A, #IMMED
	CALL	0034H
	DIS	I
	JTF	$
	INC	A
	INC	R0
	INC	R1
	INC	R2
	INC	R3
	INC	R4
	INC	R5
	INC	R6
	INC	R7

;
; 020H - 02FH.
;
	XCH	A, @R0
	XCH	A, @R1
				;022H is unused.
	MOV	A, #IMMED
	JMP	0134H
	EN	TCNTI
	JNT0	$
	CLR	A
	XCH	A, R0
	XCH	A, R1
	XCH	A, R2
	XCH	A, R3
	XCH	A, R4
	XCH	A, R5
	XCH	A, R6
	XCH	A, R7

;
; 030H - 03FH.
;
	XCHD	A, @R0
	XCHD	A, @R1
	JB1	$
				;033H is unused.
	CALL	0134H
	DIS	TCNTI
	JT0	$
	CPL	A
				;038H is unused.
	OUTL	P1, A
	OUTL	P2, A
				;03BH is unused.
	MOVD	P4, A
	MOVD	P5, A
	MOVD	P6, A
	MOVD	P7, A

;
; 040H - 04FH.
;
	ORL	A, @R0
	ORL	A, @R1
	MOV	A, T
	ORL	A, #IMMED
	JMP	0234H
	STRT	CNT
	JNT1	$
	SWAP	A
	ORL	A, R0
	ORL	A, R1
	ORL	A, R2
	ORL	A, R3
	ORL	A, R4
	ORL	A, R5
	ORL	A, R6
	ORL	A, R7

;
; 050H - 05FH.
;
	ANL	A, @R0
	ANL	A, @R1
	JB2	$
	ANL	A, #IMMED
	CALL	0234H
	STRT	T
	JT1	$
	DA	A
	ANL	A, R0
	ANL	A, R1
	ANL	A, R2
	ANL	A, R3
	ANL	A, R4
	ANL	A, R5
	ANL	A, R6
	ANL	A, R7

;
; 060H - 06FH.
;
	ADD	A, @R0
	ADD	A, @R1
	MOV	T, A
				;063H is unused.
	JMP	0334H
	STOP	TCNT
				;066H is unused.
	RRC	A
	ADD	A, R0
	ADD	A, R1
	ADD	A, R2
	ADD	A, R3
	ADD	A, R4
	ADD	A, R5
	ADD	A, R6
	ADD	A, R7

;
; 070H - 07FH.
;
	ADDC	A, @R0
	ADDC	A, @R1
	JB3	$
				;073H is unused.
	CALL	0334H
	ENT0	CLK
	JF1	$
	RR	A
	ADDC	A, R0
	ADDC	A, R1
	ADDC	A, R2
	ADDC	A, R3
	ADDC	A, R4
	ADDC	A, R5
	ADDC	A, R6
	ADDC	A, R7

;
; 080H - 08FH.
;
	MOVX	A, @R0
	MOVX	A, @R1
				;082H is unused.
	RET
	JMP	0434H
	CLR	F0
	JNI	$
				;087H is unused.
	ORL	BUS, #IMMED
	ORL	P1, #IMMED
	ORL	P2, #IMMED
				;08BH is unused.
	ORLD	P4, A
	ORLD	P5, A
	ORLD	P6, A
	ORLD	P7, A

;
; 090H - 09FH.
;
	MOVX	@R0, A
	MOVX	@R1, A
	JB4	$
	RETR
	CALL	0434H
	CPL	F0
	JNZ	$
	CLR	C
	ANL	BUS, #IMMED
	ANL	P1, #IMMED
	ANL	P2, #IMMED
				;09BH is unused.
	ANLD	P4, A
	ANLD	P5, A
	ANLD	P6, A
	ANLD	P7, A

;
; 0A0H - 0AFH.
;
	MOV	@R0, A
	MOV	@R1, A
				;0A2H is unused.
	MOVP	A, @A
	JMP	0534H
	CLR	F1
				;0A6H is unused.
	CPL	C
	MOV	R0, A
	MOV	R1, A
	MOV	R2, A
	MOV	R3, A
	MOV	R4, A
	MOV	R5, A
	MOV	R6, A
	MOV	R7, A

;
; 0B0H - 0BFH.
;
	MOV	@R0, #IMMED
	MOV	@R1, #IMMED
	JB5	$
	JMPP	@A
	CALL	0534H
	CPL	F1
	JF0	$
				;0B7H is unused.
	MOV	R0, #IMMED
	MOV	R1, #IMMED
	MOV	R2, #IMMED
	MOV	R3, #IMMED
	MOV	R4, #IMMED
	MOV	R5, #IMMED
	MOV	R6, #IMMED
	MOV	R7, #IMMED

;
; 0C0H - 0CFH.
;
				;0C0H is unused.
				;0C1H is unused.
				;0C2H is unused.
				;0C3H is unused.
	JMP	0634H
	SEL	RB0
	JZ	$
	MOV	A, PSW
	DEC	R0
	DEC	R1
	DEC	R2
	DEC	R3
	DEC	R4
	DEC	R5
	DEC	R6
	DEC	R7

;
; 0D0H - 0DFH.
;
	XRL	A, @R0
	XRL	A, @R1
	JB6	$
	XRL	A, #IMMED
	CALL	0634H
	SEL	RB1
				;0D6H is unused.
	MOV	PSW, A
	XRL	A, R0
	XRL	A, R1
	XRL	A, R2
	XRL	A, R3
	XRL	A, R4
	XRL	A, R5
	XRL	A, R6
	XRL	A, R7

;
; 0E0H - 0EFH.
;
				;0E0H is unused.
				;0E1H is unused.
				;0E2H is unused.
	MOVP3	A, @A
	JMP	0734H
	SEL	MB0
	JNC	$
	RL	A
	DJNZ	R0, $
	DJNZ	R1, $
	DJNZ	R2, $
	DJNZ	R3, $
	DJNZ	R4, $
	DJNZ	R5, $
	DJNZ	R6, $
	DJNZ	R7, $

;
; 0F0H - 0FFH
;
	MOV	A, @R0
	MOV	A, @R1
	JB7	$
				;0F3H is unused.
	CALL	0734H
	SEL	MB1
	JC	$
	RLC	A
	MOV	A, R0
	MOV	A, R1
	MOV	A, R2
	MOV	A, R3
	MOV	A, R4
	MOV	A, R5
	MOV	A, R6
	MOV	A, R7

	END

	MOV	R2, #IMMED
	MOV	R3, #IMMED
	MOV	R4