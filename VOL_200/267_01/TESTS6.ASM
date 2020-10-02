;
;	S6 Opcodes in Numerical Order
;
;	For Testing S6 Cross-Assembler
;
; First, some constant definitions:
;
RAM	EQU	12H
DATA	EQU	34H

;
; 000H - 00FH
;
	JRNZ	$ + 1
	CALL	0560H
	JRNC	$ + 1
	JRR	0, RAM, $ + 3
	JRZ	$ + 1
				; 005H IS UNUSED.
	JRC	$ + 1
	LD	A, (X)
	JRNZ	$ + 2
	JP	0560H
	JRNC	$ + 2
	RES	0, RAM
	JRZ	$ + 2
	LDI	RAM, DATA
	JRC	$ + 2
	LD	A, (Y)

;
; 001H - 01FH
;
	JRNZ	$ + 3
	CALL	0561H
	JRNC	$ + 3
	JRS	0, RAM, $ + 3
	JRZ	$ + 3
	INC	X
	JRC	$ + 3
	LDI	A, DATA
	JRNZ	$ + 4
	JP	0561H
	JRNC	$ + 4
	SET	0, RAM
	JRZ	$ + 4
	DEC	X
	JRC	$ + 4
	LD	A, RAM

;
; 020H - 02FH
;
	JRNZ	$ + 5
	CALL	0562H
	JRNC	$ + 5
	JRR	4, RAM, $ + 3
	JRZ	$ + 5
				; 025H IS UNUSED.
	JRC	$ + 5
	CP	A, (X)
	JRNZ	$ + 6
	JP	0562H
	JRNC	$ + 6
	RES	4, RAM
	JRZ	$ + 6
	COM	A
	JRC	$ + 6
	CP	A, (Y)

;
; 031H - 03FH
;
	JRNZ	$ + 7
	CALL	0563H
	JRNC	$ + 7
	JRS	4, RAM, $ + 3
	JRZ	$ + 7
	LD	A, X
	JRC	$ + 7
	CPI	A, DATA
	JRNZ	$ + 8
	JP	0563H
	JRNC	$ + 8
	SET	4, RAM
	JRZ	$ + 8
	LD	X, A
	JRC	$ + 8
	CP	A, RAM

;
; 041H - 04FH
;
	JRNZ	$ + 9
	CALL	0564H
	JRNC	$ + 9
	JRR	2, RAM, $ + 3
	JRZ	$ + 9
				; 045H IS UNUSED.
	JRC	$ + 9
	ADD	A, (X)
	JRNZ	$ + 10
	JP	0564H
	JRNC	$ + 10
	RES	2, RAM
	JRZ	$ + 10
	RETI
	JRC	$ + 10
	ADD	A, (Y)

;
; 051H - 05FH
;
	JRNZ	$ + 11
	CALL	0565H
	JRNC	$ + 11
	JRS	2, RAM, $ + 3
	JRZ	$ + 11
	INC	Y
	JRC	$ + 11
	ADDI	A, DATA
	JRNZ	$ + 12
	JP	0565H
	JRNC	$ + 12
	SET	2, RAM
	JRZ	$ + 12
	DEC	Y
	JRC	$ + 12
	ADD	A, RAM

;
; 061H - 06FH
;
	JRNZ	$ + 13
	CALL	0566H
	JRNC	$ + 13
	JRR	6, RAM, $ + 3
	JRZ	$ + 13
				; 065H IS UNUSED.
	JRC	$ + 13
	INC	(X)
	JRNZ	$ + 14
	JP	0566H
	JRNC	$ + 14
	RES	6, RAM
	JRZ	$ + 14
	STOP
	JRC	$ + 14
	INC	(Y)

;
; 071H - 07FH
;
	JRNZ	$ + 15
	CALL	0567H
	JRNC	$ + 15
	JRS	6, RAM, $ + 3
	JRZ	$ + 15
	LD	A, Y
	JRC	$ + 15
				; 077H IS UNUSED.
	JRNZ	$ + 16
	JP	0567H
	JRNC	$ + 16
	SET	6, RAM
	JRZ	$ + 16
	LD	Y, A
	JRC	$ + 16
	INC	RAM

;
; 081H - 08FH
;
	JRNZ	$ - 15
	CALL	0568H
	JRNC	$ - 15
	JRR	1, RAM, $ + 3
	JRZ	$ - 15
				; 085H IS UNUSED.
	JRC	$ - 15
	LD	(X), A
	JRNZ	$ - 14
	JP	0568H
	JRNC	$ - 14
	RES	1, RAM
	JRZ	$ - 14
				; 08DH IS UNUSED.
	JRC	$ - 14
	LD	(Y), A

;
; 091H - 09FH
;
	JRNZ	$ - 13
	CALL	0569H
	JRNC	$ - 13
	JRS	1, RAM, $ + 3
	JRZ	$ - 13
	INC	V
	JRC	$ - 13
				; 097H IS UNUSED.
	JRNZ	$ - 12
	JP	0569H
	JRNC	$ - 12
	SET	1, RAM
	JRZ	$ - 12
	DEC	V
	JRC	$ - 12
	LD	RAM, A

;
; 0A1H - 0AFH
;
	JRNZ	$ - 11
	CALL	056AH
	JRNC	$ - 11
	JRR	5, RAM, $ + 3
	JRZ	$ - 11
				; 0A5H IS UNUSED.
	JRC	$ - 11
	AND	A, (X)
	JRNZ	$ - 10
	JP	056AH
	JRNC	$ - 10
	RES	5, RAM
	JRZ	$ - 10
	RLC	A
	JRC	$ - 10
	AND	A, (Y)

;
; 0B1H - 0BFH
;
	JRNZ	$ - 9
	CALL	056BH
	JRNC	$ - 9
	JRS	5, RAM, $ + 3
	JRZ	$ - 9
	LD	A, V
	JRC	$ - 9
	ANDI	A, DATA
	JRNZ	$ - 8
	JP	056BH
	JRNC	$ - 8
	SET	5, RAM
	JRZ	$ - 8
	LD	V, A
	JRC	$ - 8
	AND	A, RAM

;
; 0C1H - 0CFH
;
	JRNZ	$ - 7
	CALL	056CH
	JRNC	$ - 7
	JRR	3, RAM, $ + 3
	JRZ	$ - 7
				; 0C5H IS UNUSED.
	JRC	$ - 7
	SUB	A, (X)
	JRNZ	$ - 6
	JP	056CH
	JRNC	$ - 6
	RES	3, RAM
	JRZ	$ - 6
	RET
	JRC	$ - 6
	SUB	A, (Y)

;
; 0D1H - 0DFH
;
	JRNZ	$ - 5
	CALL	056DH
	JRNC	$ - 5
	JRS	3, RAM, $ + 3
	JRZ	$ - 5
	INC	W
	JRC	$ - 5
	SUBI	A, DATA
	JRNZ	$ - 4
	JP	056DH
	JRNC	$ - 4
	SET	3, RAM
	JRZ	$ - 4
	DEC	W
	JRC	$ - 4
	SUB	A, RAM

;
; 0E1H - 0EFH
;
	JRNZ	$ - 3
	CALL	056EH
	JRNC	$ - 3
	JRR	7, RAM, $ + 3
	JRZ	$ - 3
				; 0E5H IS UNUSED.
	JRC	$ - 3
	DEC	(X)
	JRNZ	$ - 2
	JP	056EH
	JRNC	$ - 2
	RES	7, RAM
	JRZ	$ - 2
	WAIT
	JRC	$ - 2
	DEC	(Y)

;
; 0F1H - 0FFH
;
	JRNZ	$ - 1
	CALL	056FH
	JRNC	$ - 1
	JRS	7, RAM, $ + 3
	JRZ	$ - 1
	LD	A, W
	JRC	$ - 1
				; 0F7H IS UNUSED.
	JRNZ	$ + 0
	JP	056FH
	JRNC	$ + 0
	SET	7, RAM
	JRZ	$ + 0
	LD	W, A
	JRC	$ + 0
	DEC	RAM

;
; Now, let's run the built-in macros:
;
	NOP			; JRZ $ + 1

	CLR	X		; LDI X, 0
	CLR	Y		; LDI Y, 0
	CLR	V		; LDI V, 0
	CLR	W		; LDI W, 0

	SLA	A		; ADD A, A

	CLR	A		; SUB A, A

	JRP	$		; JRR 7, 0FFH, $
	JRN	$		; JRS 7, 0FFH, $

	END

