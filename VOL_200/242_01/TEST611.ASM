;
; This file contains the opcodes of the MC68HC11 microprocessor in numerical
; order.  Instructions with prebytes immediately follow their non-prebyte
; brethren.
;
IMM	EQU	$12
DIR	EQU	$34
EXT	EQU	$5678
INDX	EQU	$9A

	ORG	$100

;
; $00 - $0F
;
	TEST
	NOP
	IDIV
	FDIV
	LSRD
	ASLD
	LSLD
	TAP
	TPA
	INX
	INY
	DEX
	DEY
	CLV
	SEV
	CLC
	SEC
	CLI
	SEI

;
; $10 - $1F
;
	SBA
	CBA
	BRSET	DIR, IMM, *
	BRCLR	DIR, IMM, *
	BSET	DIR, IMM
	BCLR	DIR, IMM
	TAB
	TBA
				; $18 is one of the prefix bytes.
	DAA
				; $1A is one of the prefix bytes.
	ABA
	BSET	INDX, X, IMM
	BSET	INDX, Y, IMM
	BCLR	INDX, X, IMM
	BCLR	INDX, Y, IMM
	BRSET	INDX, X, IMM, *
	BRSET	INDX, Y, IMM, *
	BRCLR	INDX, X, IMM, *
	BRCLR	INDX, Y, IMM, *

;
; $20 - $2F
;
	BRA	*
	BRN	*
	BHI	*
	BLS	*
	BCC	*
	BHS	*
	BCS	*
	BLO	*
	BNE	*
	BEQ	*
	BVC	*
	BVS	*
	BPL	*
	BMI	*
	BGE	*
	BLT	*
	BGT	*
	BLE	*

;
; $30 - $3F
;
	TSX
	TSY
	INS
	PULA
	PULB
	DES
	TXS
	TYS
	PSHA
	PSHB
	PULX
	PULY
	RTS
	ABX
	ABY
	RTI
	PSHX
	PSHY
	MUL
	WAI
	SWI

;
; $40 - $4F
;
	NEGA
				; $41 is unused.
				; $42 is unused.
	COMA
	LSRA
				; $45 is unused.
	RORA
	ASRA
	ASLA
	LSLA
	ROLA
	DECA
				; $4B is unused.
	INCA
	TSTA
				; $4E is unused.
	CLRA

;
; $50 - $5F
;
	NEGB
				; $51 is unused.
				; $52 is unused.
	COMB
	LSRB
				; $55 is unused.
	RORB
	ASRB
	ASLB
	LSLB
	ROLB
	DECB
				; $5B is unused.
	INCB
	TSTB
				; $5E is unused.
	CLRB

;
; $60 - $6F
;
	NEG	INDX, X
	NEG	INDX, Y
				; $61 is unused.
				; $62 is unused.
	COM	INDX, X
	COM	INDX, Y
	LSR	INDX, X
	LSR	INDX, Y
				; $65 is unused.
	ROR	INDX, X
	ROR	INDX, Y
	ASR	INDX, X
	ASR	INDX, Y
	ASL	INDX, X
	ASL	INDX, Y
	LSL	INDX, X
	LSL	INDX, Y
	ROL	INDX, X
	ROL	INDX, Y
	DEC	INDX, X
	DEC	INDX, Y
					; $6B is unused.
	INC	INDX, X
	INC	INDX, Y
	TST	INDX, X
	TST	INDX, Y
	JMP	INDX, X
	JMP	INDX, Y
	CLR	INDX, X
	CLR	INDX, Y

;
; $70 - $7F
;
	NEG	EXT
					; $71 is unused.
					; $72 is unused.
	COM	EXT
	LSR	EXT
					; $75 is unused.
	ROR	EXT
	ASR	EXT
	ASL	EXT
	LSL	EXT
	ROL	EXT
	DEC	EXT
					; $7B is unused.
	INC	EXT
	TST	EXT
	JMP	EXT
	CLR	EXT

;
; $80 - $8F
;
	SUBA	#IMM
	CMPA	#IMM
	SBCA	#IMM
	SUBD	#IMM
	CPD	#IMM
	ANDA	#IMM
	BITA	#IMM
	LDAA	#IMM
					; $87 is unused.
	EORA	#IMM
	ADCA	#IMM
	ORAA	#IMM
	ADDA	#IMM
	CPX	#IMM
	CPY	#IMM
	BSR	*
	LDS	#IMM
	XGDX
	XGDY

;
; $90 - $9F
;
	SUBA	DIR
	CMPA	DIR
	SBCA	DIR
	SUBD	DIR
	CPD	DIR
	ANDA	DIR
	BITA	DIR
	LDAA	DIR
	STAA	DIR
	EORA	DIR
	ADCA	DIR
	ORAA	DIR
	ADDA	DIR
	CPX	DIR
	CPY	DIR
	JSR	DIR
	LDS	DIR
	STS	DIR

;
; $A0 - $AF
;
	SUBA	INDX, X
	SUBA	INDX, Y
	CMPA	INDX, X
	CMPA	INDX, Y
	SBCA	INDX, X
	SBCA	INDX, Y
	SUBD	INDX, X
	SUBD	INDX, Y
	CPD	INDX, X
	CPD	INDX, Y
	ANDA	INDX, X
	ANDA	INDX, Y
	BITA	INDX, X
	BITA	INDX, Y
	LDAA	INDX, X
	LDAA	INDX, Y
	STAA	INDX, X
	STAA	INDX, Y
	EORA	INDX, X
	EORA	INDX, Y
	ADCA	INDX, X
	ADCA	INDX, Y
	ORAA	INDX, X
	ORAA	INDX, Y
	ADDA	INDX, X
	ADDA	INDX, Y
	CPX	INDX, X
	CPY	INDX, Y
	CPY	INDX, X
	CPX	INDX, Y
	JSR	INDX, X
	JSR	INDX, Y
	LDS	INDX, X
	LDS	INDX, Y
	STS	INDX, X
	STS	INDX, Y

;
; $B0 - $BF
;
	SUBA	EXT
	CMPA	EXT
	SBCA	EXT
	SUBD	EXT
	CPD	EXT
	ANDA	EXT
	BITA	EXT
	LDAA	EXT
	STAA	EXT
	EORA	EXT
	ADCA	EXT
	ORAA	EXT
	ADDA	EXT
	CPX	EXT
	CPY	EXT
	JSR	EXT
	LDS	EXT
	STS	EXT

;
; $C0 - $CF
;
	SUBB	#IMM
	CMPB	#IMM
	SBCB	#IMM
	ADDD	#IMM
	ANDB	#IMM
	BITB	#IMM
	LDAB	#IMM
					; $C7 is unused.
	EORB	#IMM
	ADCB	#IMM
	ORAB	#IMM
	ADDB	#IMM
	LDD	#IMM
					; $CD is one of the prefix bytes.
	LDX	#IMM
	LDY	#IMM
	STOP

;
; $D0 - $DF
;
	SUBB	DIR
	CMPB	DIR
	SBCB	DIR
	ADDD	DIR
	ANDB	DIR
	BITB	DIR
	LDAB	DIR
	STAB	DIR
	EORB	DIR
	ADCB	DIR
	ORAB	DIR
	ADDB	DIR
	LDD	DIR
	STD	DIR
	LDX	DIR
	LDY	DIR
	STX	DIR
	STY	DIR

;
; $E0 - $EF
;
	SUBB	INDX, X
	SUBB	INDX, Y
	CMPB	INDX, X
	CMPB	INDX, Y
	SBCB	INDX, X
	SBCB	INDX, Y
	ADDD	INDX, X
	ADDD	INDX, Y
	ANDB	INDX, X
	ANDB	INDX, Y
	BITB	INDX, X
	BITB	INDX, Y
	LDAB	INDX, X
	LDAB	INDX, Y
	STAB	INDX, X
	STAB	INDX, Y
	EORB	INDX, X
	EORB	INDX, Y
	ADCB	INDX, X
	ADCB	INDX, Y
	ORAB	INDX, X
	ORAB	INDX, Y
	ADDB	INDX, X
	ADDB	INDX, Y
	LDD	INDX, X
	LDD	INDX, Y
	STD	INDX, X
	STD	INDX, Y
	LDX	INDX, X
	LDY	INDX, Y
	LDY	INDX, X
	LDX	INDX, Y
	STX	INDX, X
	STY	INDX, Y
	STY	INDX, X
	STX	INDX, Y

;
; $F0 - $FF
;
	SUBB	EXT
	CMPB	EXT
	SBCB	EXT
	ADDD	EXT
	ANDB	EXT
	BITB	EXT
	LDAB	EXT
	STAB	EXT
	EORB	EXT
	ADCB	EXT
	ORAB	EXT
	ADDB	EXT
	LDD	EXT
	STD	EXT
	LDX	EXT
	LDY	EXT
	STX	EXT
	STY	EXT

	END
