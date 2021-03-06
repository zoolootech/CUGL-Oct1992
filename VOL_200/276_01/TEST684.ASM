;
;	Test File Containing the MC68HC04's Opcodes in Numerical Order
;
IMMED	EQU	$CC

X_REG	EQU	$80
Y_REG	EQU	$81
V_REG	EQU	$82
W_REG	EQU	$83

DIRECT	EQU	$DD

;
; $00 - $0F
;
	BNE	* + 1
	BNE	* + 2
	BNE	* + 3
	BNE	* + 4
	BNE	* + 5
	BNE	* + 6
	BNE	* + 7
	BNE	* + 8
	BNE	* + 9
	BNE	* + 10
	BNE	* + 11
	BNE	* + 12
	BNE	* + 13
	BNE	* + 14
	BNE	* + 15
	BNE	* + 16

;
; $10 - $1F
;
	BNE	* - 15
	BNE	* - 14
	BNE	* - 13
	BNE	* - 12
	BNE	* - 11
	BNE	* - 10
	BNE	* - 9
	BNE	* - 8
	BNE	* - 7
	BNE	* - 6
	BNE	* - 5
	BNE	* - 4
	BNE	* - 3
	BNE	* - 2
	BNE	* - 1
	BNE	*
;
; $20 - $2F
;
	BEQ	* + 1
	BEQ	* + 2
	BEQ	* + 3
	BEQ	* + 4
	BEQ	* + 5
	BEQ	* + 6
	BEQ	* + 7
	BEQ	* + 8
	BEQ	* + 9
	BEQ	* + 10
	BEQ	* + 11
	BEQ	* + 12
	BEQ	* + 13
	BEQ	* + 14
	BEQ	* + 15
	BEQ	* + 16

;
; $30 - $3F
;
	BEQ	* - 15
	BEQ	* - 14
	BEQ	* - 13
	BEQ	* - 12
	BEQ	* - 11
	BEQ	* - 10
	BEQ	* - 9
	BEQ	* - 8
	BEQ	* - 7
	BEQ	* - 6
	BEQ	* - 5
	BEQ	* - 4
	BEQ	* - 3
	BEQ	* - 2
	BEQ	* - 1
	BEQ	*
;
; $40 - $4F
;
	BCC	* + 1
	BCC	* + 2
	BCC	* + 3
	BCC	* + 4
	BCC	* + 5
	BCC	* + 6
	BCC	* + 7
	BCC	* + 8
	BCC	* + 9
	BCC	* + 10
	BCC	* + 11
	BCC	* + 12
	BCC	* + 13
	BCC	* + 14
	BCC	* + 15
	BCC	* + 16

;
; $50 - $5F
;
	BCC	* - 15
	BCC	* - 14
	BCC	* - 13
	BCC	* - 12
	BCC	* - 11
	BCC	* - 10
	BCC	* - 9
	BCC	* - 8
	BCC	* - 7
	BCC	* - 6
	BCC	* - 5
	BCC	* - 4
	BCC	* - 3
	BCC	* - 2
	BCC	* - 1
	BCC	*
;
; $60 - $6F
;
	BCS	* + 1
	BCS	* + 2
	BCS	* + 3
	BCS	* + 4
	BCS	* + 5
	BCS	* + 6
	BCS	* + 7
	BCS	* + 8
	BCS	* + 9
	BCS	* + 10
	BCS	* + 11
	BCS	* + 12
	BCS	* + 13
	BCS	* + 14
	BCS	* + 15
	BCS	* + 16

;
; $70 - $7F
;
	BCS	* - 15
	BCS	* - 14
	BCS	* - 13
	BCS	* - 12
	BCS	* - 11
	BCS	* - 10
	BCS	* - 9
	BCS	* - 8
	BCS	* - 7
	BCS	* - 6
	BCS	* - 5
	BCS	* - 4
	BCS	* - 3
	BCS	* - 2
	BCS	* - 1
	BCS	*

;
; $80 - $8F
;
	JSR	$0BB
	JSR	$1BB
	JSR	$2BB
	JSR	$3BB
	JSR	$4BB
	JSR	$5BB
	JSR	$6BB
	JSR	$7BB
	JSR	$8BB
	JSR	$9BB
	JSR	$ABB
	JSR	$BBB
	JSR	$CBB
	JSR	$DBB
	JSR	$EBB
	JSR	$FBB
;
; $90 - $9F
;
	JMP	$0BB
	JMP	$1BB
	JMP	$2BB
	JMP	$3BB
	JMP	$4BB
	JMP	$5BB
	JMP	$6BB
	JMP	$7BB
	JMP	$8BB
	JMP	$9BB
	JMP	$ABB
	JMP	$BBB
	JMP	$CBB
	JMP	$DBB
	JMP	$EBB
	JMP	$FBB

;
; $A0 - $AF
;
				; $A0 is unused.
				; $A1 is unused.
				; $A2 is unused.
				; $A3 is unused.
				; $A4 is unused.
				; $A5 is unused.
				; $A6 is unused.
				; $A7 is unused.
	INC	X_REG
	INC	Y_REG
	INC	V_REG
	INC	W_REG
	LDA	X_REG
	LDA	Y_REG
	LDA	V_REG
	LDA	W_REG
;
; $B0 - $BF
;
	MVI	DIRECT, IMMED
				; $B1 is unused.
	RTI
	RTS
	COMA
	ROLA
	STOP
	WAIT
	DEC	X_REG
	DEC	Y_REG
	DEC	V_REG
	DEC	W_REG
	STA	X_REG
	STA	Y_REG
	STA	V_REG
	STA	W_REG

;
; $C0 - $CF
;
	BRCLR	0, DIRECT, *
	BRCLR	1, DIRECT, *
	BRCLR	2, DIRECT, *
	BRCLR	3, DIRECT, *
	BRCLR	4, DIRECT, *
	BRCLR	5, DIRECT, *
	BRCLR	6, DIRECT, *
	BRCLR	7, DIRECT, *
	BRSET	0, DIRECT, *
	BRSET	1, DIRECT, *
	BRSET	2, DIRECT, *
	BRSET	3, DIRECT, *
	BRSET	4, DIRECT, *
	BRSET	5, DIRECT, *
	BRSET	6, DIRECT, *
	BRSET	7, DIRECT, *
;
; $D0 - $DF
;
	BCLR	0, DIRECT
	BCLR	1, DIRECT
	BCLR	2, DIRECT
	BCLR	3, DIRECT
	BCLR	4, DIRECT
	BCLR	5, DIRECT
	BCLR	6, DIRECT
	BCLR	7, DIRECT
	BSET	0, DIRECT
	BSET	1, DIRECT
	BSET	2, DIRECT
	BSET	3, DIRECT
	BSET	4, DIRECT
	BSET	5, DIRECT
	BSET	6, DIRECT
	BSET	7, DIRECT

;
; $E0 - $EF
;
	LDA	X
	STA	X
	ADD	X
	SUB	X
	CMP	X
	AND	X
	INC	X
	DEC	X
	LDA	#IMMED
				; $E9 is unused.
	ADD	#IMMED
	SUB	#IMMED
	CMP	#IMMED
	AND	#IMMED
				; $EE is unused.
				; $EF is unused.

;
; $F0 - $FF
;
	LDA	Y
	STA	Y
	ADD	Y
	SUB	Y
	CMP	Y
	AND	Y
	INC	Y
	DEC	Y
	LDA	DIRECT
	STA	DIRECT
	ADD	DIRECT
	SUB	DIRECT
	CMP	DIRECT
	AND	DIRECT
	INC	DIRECT
	DEC	DIRECT

;
; Now, let's try out all of the built-in macros:
;
	ASLA
	BHS	* + 1
	BLO	* + 1
	CLRA
	CLRX
	CLRY
	DECA
	DECX
	DECY
	INCA
	INCX
	INCY
	LDXI	IMMED
	LDYI	IMMED
	NOP
	TAX
	TAY
	TXA
	TYA

	END
�=+ u�?�w=