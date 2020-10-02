;
;		      Test File for 6502 Cross-Assembler
;
; This file just contains all of the 6502 CPU's opcodes in numerical order.
;
ABS	EQU	$1234
IMM	EQU	$56
IND	EQU	$0078
ZP	EQU	$0090

;
; $00 - $0F
;
	BRK
	ORA	(IND, X)
	ORA	ZP
	ASL	ZP
	PHP
	ORA	#IMM
	ASL	A
	ORA	ABS
	ASL	ABS
;
; $10 - $1F
;
	BPL	*
	ORA	(IND), Y
	ORA	ZP, X
	ASL	ZP, X
	CLC
	ORA	ABS, Y
	ORA	ABS, X
	ASL	ABS, X
;
; $20 - $2F
;
	JSR	ABS
	AND	(IND, X)
	BIT	ZP
	AND	ZP
	ROL	ZP
	PLP
	AND	#IMM
	ROL	A
	BIT	ABS
	AND	ABS
	ROL	ABS
;
; $30 - $3F
;
	BMI	*
	AND	(IND), Y
	AND	ZP, X
	ROL	ZP, X
	SEC
	AND	ABS, Y
	AND	ABS, X
	ROL	ABS, X
;
; $40 - $4F
;
	RTI
	EOR	(IND, X)
	EOR	ZP
	LSR	ZP
	PHA
	EOR	#IMM
	LSR	A
	JMP	ABS
	EOR	ABS
	LSR	ABS
;
; $50 - $5F
;
	BVC	*
	EOR	(IND), Y
	EOR	ZP, X
	LSR	ZP, X
	CLI
	EOR	ABS, Y
	EOR	ABS, X
	LSR	ABS, X
;
; $60 - $6F
;
	RTS
	ADC	(IND, X)
	ADC	ZP
	ROR	ZP
	PLA
	ADC	#IMM
	ROR	A
	JMP	(ABS)
	ADC	ABS
	ROR	ABS
;
; $70 - $7F
;
	BVS	*
	ADC	(IND), Y
	ADC	ZP, X
	ROR	ZP, X
	SEI
	ADC	ABS, Y
	ADC	ABS, X
	ROR	ABS, X
;
; $80 - $8F
;
	STA	(IND, X)
	STY	ZP
	STA	ZP
	STX	ZP
	DEY
	TXA
	STY	ABS
	STA	ABS
	STX	ABS
;
; $90 - $9F
;
	BCC	*
	STA	(IND), Y
	STY	ZP, X
	STA	ZP, X
	STX	ZP, Y
	TYA
	STA	ABS, Y
	TXS
	STA	ABS, X
;
; $A0 - $AF
;
	LDY	#IMM
	LDA	(IND, X)
	LDX	#IMM
	LDY	ZP
	LDA	ZP
	LDX	ZP
	TAY
	LDA	#IMM
	TAX
	LDY	ABS
	LDA	ABS
	LDX	ABS
;
; $B0 - $BF
;
	BCS	*
	LDA	(IND), Y
	LDY	ZP, X
	LDA	ZP, X
	LDX	ZP, Y
	CLV
	LDA	ABS, Y
	TSX
	LDY	ABS, X
	LDA	ABS, X
	LDX	ABS, Y
;
; $C0 - $CF
;
	CPY	#IMM
	CMP	(IND, X)
	CPY	ZP
	CMP	ZP
	DEC	ZP
	INY
	CMP	#IMM
	DEX
	CPY	ABS
	CMP	ABS
	DEC	ABS
;
; $D0 - $DF
;
	BNE	*
	CMP	(IND), Y
	CMP	ZP, X
	DEC	ZP, X
	CLD
	CMP	ABS, Y
	CMP	ABS, X
	DEC	ABS, X
;
; $E0 - $EF
;
	CPX	#IMM
	SBC	(IND, X)
	CPX	ZP
	SBC	ZP
	INC	ZP
	INX
	SBC	#IMM
	NOP
	CPX	ABS
	SBC	ABS
	INC	ABS
;
; $F0 - $FF
;
	BEQ	*
	SBC	(IND), Y
	SBC	ZP, X
	INC	ZP, X
	SED
	SBC	ABS, Y
	SBC	ABS, X
	INC	ABS, X

	END

	SEI
	ADC	ABS, Y
	ADC	ABS, X
	ROR	ABS, X
;
; $80 - $8F
;
	STA	(IND, X)
	STY	ZP
	STA	ZP
	STX	ZP
