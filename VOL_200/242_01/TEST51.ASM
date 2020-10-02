;
; Test file with all of the Intel 8051's opcodes in numerical order.....
;

DIR	EQU	78H
IMM	EQU	9AH

BIT	BIT	PSW.7
BIT1	BIT	CY
BIT2	BIT	PSW + 7

;
; 000H - 000FH
;
	NOP
	AJMP	0012H
	LJMP	3456H
	RR	A
	INC	A
	INC	DIR
	INC	@R0
	INC	@R1
	INC	R0
	INC	R1
	INC	R2
	INC	R3
	INC	R4
	INC	R5
	INC	R6
	INC	R7

;
; 010H - 01FH
;
	JBC	BIT, $
	ACALL	0012H
	LCALL	3456H
	RRC	A
	DEC	A
	DEC	DIR
	DEC	@R0
	DEC	@R1
	DEC	R0
	DEC	R1
	DEC	R2
	DEC	R3
	DEC	R4
	DEC	R5
	DEC	R6
	DEC	R7

;
; 020H - 02FH
;
	JB	BIT, $
	AJMP	0112H
	RET
	RL	A
	ADD	A, #IMM
	ADD	A, DIR
	ADD	A, @R0
	ADD	A, @R1
	ADD	A, R0
	ADD	A, R1
	ADD	A, R2
	ADD	A, R3
	ADD	A, R4
	ADD	A, R5
	ADD	A, R6
	ADD	A, R7

;
; 030H - 03FH
;
	JNB	BIT, $
	ACALL	0112H
	RETI
	RLC	A
	ADDC	A, #IMM
	ADDC	A, DIR
	ADDC	A, @R0
	ADDC	A, @R1
	ADDC	A, R0
	ADDC	A, R1
	ADDC	A, R2
	ADDC	A, R3
	ADDC	A, R4
	ADDC	A, R5
	ADDC	A, R6
	ADDC	A, R7

;
; 040H - 04FH
;
	JC	$
	AJMP	0212H
	ORL	DIR, A
	ORL	DIR, #IMM
	ORL	A, #IMM
	ORL	A, DIR
	ORL	A, @R0
	ORL	A, @R1
	ORL	A, R0
	ORL	A, R1
	ORL	A, R2
	ORL	A, R3
	ORL	A, R4
	ORL	A, R5
	ORL	A, R6
	ORL	A, R7

;
; 050H - 05FH
;
	JNC	$
	ACALL	0212H
	ANL	DIR, A
	ANL	DIR, #IMM
	ANL	A, #IMM
	ANL	A, DIR
	ANL	A, @R0
	ANL	A, @R1
	ANL	A, R0
	ANL	A, R1
	ANL	A, R2
	ANL	A, R3
	ANL	A, R4
	ANL	A, R5
	ANL	A, R6
	ANL	A, R7

;
; 060H - 06FH
;
	JZ	$
	AJMP	0312H
	XRL	DIR, A
	XRL	DIR, #IMM
	XRL	A, #IMM
	XRL	A, DIR
	XRL	A, @R0
	XRL	A, @R1
	XRL	A, R0
	XRL	A, R1
	XRL	A, R2
	XRL	A, R3
	XRL	A, R4
	XRL	A, R5
	XRL	A, R6
	XRL	A, R7

;
; 070H - 07FH
;
	JNZ	$
	ACALL	0312H
	ORL	C, BIT
	JMP	@A + DPTR
	MOV	A, #IMM
	MOV	DIR, #IMM
	MOV	@R0, #IMM
	MOV	@R1, #IMM
	MOV	R0, #IMM
	MOV	R1, #IMM
	MOV	R2, #IMM
	MOV	R3, #IMM
	MOV	R4, #IMM
	MOV	R5, #IMM
	MOV	R6, #IMM
	MOV	R7, #IMM

;
; 080H - 08FH
;
	SJMP	$
	AJMP	0412H
	ANL	C, BIT
	MOVC	A, @A + PC
	DIV	AB
	MOV	DIR, DIR
	MOV	DIR, @R0
	MOV	DIR, @R1
	MOV	DIR, R0
	MOV	DIR, R1
	MOV	DIR, R2
	MOV	DIR, R3
	MOV	DIR, R4
	MOV	DIR, R5
	MOV	DIR, R6
	MOV	DIR, R7

;
; 090H - 09FH
;
	MOV	DPTR, #3456H
	ACALL	0412H
	MOV	BIT, C
	MOVC	A, @A + DPTR
	SUBB 	A, #IMM
	SUBB	A, DIR
	SUBB	A, @R0
	SUBB	A, @R1
	SUBB	A, R0
	SUBB	A, R1
	SUBB	A, R2
	SUBB	A, R3
	SUBB	A, R4
	SUBB	A, R5
	SUBB	A, R6
	SUBB	A, R7

;
; 0A0H - 0AFH
;
	ORL	C, /BIT
	AJMP	0512H
	MOV	C, BIT
	INC	DPTR
	MUL	AB
				; 0A5H IS UNUSED.
	MOV	@R0, DIR
	MOV	@R1, DIR
	MOV	R0, DIR
	MOV	R1, DIR
	MOV	R2, DIR
	MOV	R3, DIR
	MOV	R4, DIR
	MOV	R5, DIR
	MOV	R6, DIR
	MOV	R7, DIR

;
; 0B0H - 0BFH
;
	ANL	C, /BIT
	ACALL	0512H
	CPL	BIT
	CPL	C
	CJNE	A, #IMM, $
	CJNE	A, DIR, $
	CJNE	@R0, #IMM, $
	CJNE	@R1, #IMM, $
	CJNE	R0, #IMM, $
	CJNE	R1, #IMM, $
	CJNE	R2, #IMM, $
	CJNE	R3, #IMM, $
	CJNE	R4, #IMM, $
	CJNE	R5, #IMM, $
	CJNE	R6, #IMM, $
	CJNE	R7, #IMM, $

;
; 0C0H - 0CFH
;
	PUSH	DIR
	AJMP	0612H
	CLR	BIT
	CLR	C
	SWAP	A
	XCH	A, DIR
	XCH	A, @R0
	XCH	A, @R1
	XCH	A, R0
	XCH	A, R1
	XCH	A, R2
	XCH	A, R3
	XCH	A, R4
	XCH	A, R5
	XCH	A, R6
	XCH	A, R7

;
; 0D0H - 0DFH
;
	POP	DIR
	ACALL	0612H
	SETB	BIT
	SETB	C
	DA	A
	DJNZ	DIR, $
	XCHD	A, @R0
	XCHD	A, @R1
	DJNZ	R0, $
	DJNZ	R1, $
	DJNZ	R2, $
	DJNZ	R3, $
	DJNZ	R4, $
	DJNZ	R5, $
	DJNZ	R6, $
	DJNZ	R7, $

;
; 0E0H - 0EFH
;
	MOVX	A, @DPTR
	AJMP	0712H
	MOVX	A, @R0
	MOVX	A, @R1
	CLR	A
	MOV	A, DIR
	MOV	A, @R0
	MOV	A, @R1
	MOV	A, R0
	MOV	A, R1
	MOV	A, R2
	MOV	A, R3
	MOV	A, R4
	MOV	A, R5
	MOV	A, R6
	MOV	A, R7

;
; 0F0H - 0FFH
;
	MOVX	@DPTR, A
	ACALL	0712H
	MOVX	@R0, A
	MOVX	@R1, A
	CPL	A
	MOV	DIR, A
	MOV	@R0, A
	MOV	@R1, A
	MOV	R0, A
	MOV	R1, A
	MOV	R2, A
	MOV	R3, A
	MOV	R4, A
	MOV	R5, A
	MOV	R6, A
	MOV	R7, A

;
; Now horse around with some bit operations.
;
BYTE	EQU	021H
BITNO	BIT	BYTE + 1.2

	MOV	C, BYTE + 1.2
	MOV	C, BITNO
	MOV	C, ((BYTE - 20H) SHL 3) + 2

	MOV	BYTE + 1.2, C
	MOV	BITNO, C
	MOV	((BYTE - 20H) SHL 3) + 2, C

	ORL	C, BYTE + 1.2
	ORL	C, BITNO
	ORL	C, ((BYTE - 20H) SHL 3) + 2

	ORL	C, /BYTE + 1.2
	ORL	C, /BITNO
	ORL	C, /((BYTE - 20H) SHL 3) + 2

	CLR	BYTE + 1.2
	CLR	BITNO
	CLR	((BYTE - 20H) SHL 3) + 2

	END
D	A, @R0
	XCHD	A, @R1
	DJNZ	R0, $
	DJNZ	R1, $
	DJNZ	R2, $
	DJNZ	R3, $
	DJNZ	R4, $
	DJNZ	R5, $
	DJNZ	R6, $
	DJNZ	R