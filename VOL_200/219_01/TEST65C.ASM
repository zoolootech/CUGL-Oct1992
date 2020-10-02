;
;		      Test File for 65C02 Cross-Assembler
;
; This file just contains all of the 65C02 CPU's opcodes in numerical order.
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


	TSB	ZP
	ORA	ZP
	ASL	ZP
	RMB0	ZP
	PHP
	ORA	#IMM
	ASL	A

	TSB	ABS
	ORA	ABS
	ASL	ABS
	BBR0	ZP, *
;
; $10 - $1F
;
	BPL	*
	ORA	(IND), Y
	ORA	(IND)

	TRB	ZP
	ORA	ZP, X
	ASL	ZP, X
	RMB1	ZP
	CLC
	ORA	ABS, Y
	INC	A

	TRB	ABS
	ORA	ABS, X
	ASL	ABS, X
	BBR1	ZP, *
;
; $20 - $2F
;
	JSR	ABS
	AND	(IND, X)


	BIT	ZP
	AND	ZP
	ROL	ZP
	RMB2	ZP
	PLP
	AND	#IMM
	ROL	A

	BIT	ABS
	AND	ABS
	ROL	ABS
	BBR2	ZP, *
;
; $30 - $3F
;
	BMI	*
	AND	(IND), Y
	AND	(IND)

	BIT	ZP, X
	AND	ZP, X
	ROL	ZP, X
	RMB3	ZP
	SEC
	AND	ABS, Y
	DEC	A

	BIT	ABS, X
	AND	ABS, X
	ROL	ABS, X
	BBR3	ZP, *
;
; $40 - $4F
;
	RTI
	EOR	(IND, X)



	EOR	ZP
	LSR	ZP
	RMB4	ZP
	PHA
	EOR	#IMM
	LSR	A

	JMP	ABS
	EOR	ABS
	LSR	ABS
	BBR4	ZP, *
;
; $50 - $5F
;
	BVC	*
	EOR	(IND), Y
	EOR	(IND)


	EOR	ZP, X
	LSR	ZP, X
	RMB5	ZP
	CLI
	EOR	ABS, Y
	PHY


	EOR	ABS, X
	LSR	ABS, X
	BBR5	ZP, *
;
; $60 - $6F
;
	RTS
	ADC	(IND, X)


	STZ	ZP
	ADC	ZP
	ROR	ZP
	RMB6	ZP
	PLA
	ADC	#IMM
	ROR	A

	JMP	(ABS)
	ADC	ABS
	ROR	ABS
	BBR6	ZP, *
;
; $70 - $7F
;
	BVS	*
	ADC	(IND), Y
	ADC	(IND)

	STZ	ZP, X
	ADC	ZP, X
	ROR	ZP, X
	RMB7	ZP
	SEI
	ADC	ABS, Y
	PLY

	JMP	(ABS, X)
	ADC	ABS, X
	ROR	ABS, X
	BBR7	ZP, *
;
; $80 - $8F
;
	BRA	*
	STA	(IND, X)


	STY	ZP
	STA	ZP
	STX	ZP
	SMB0	ZP
	DEY
	BIT	#IMM
	TXA

	STY	ABS
	STA	ABS
	STX	ABS
	BBS0	ZP, *
;
; $90 - $9F
;
	BCC	*
	STA	(IND), Y
	STA	(IND)

	STY	ZP, X
	STA	ZP, X
	STX	ZP, Y
	SMB1	ZP
	TYA
	STA	ABS, Y
	TXS

	STZ	ABS
	STA	ABS, X
	STZ	ABS, X
	BBS1	ZP, *
;
; $A0 - $AF
;
	LDY	#IMM
	LDA	(IND, X)
	LDX	#IMM

	LDY	ZP
	LDA	ZP
	LDX	ZP
	SMB2	ZP
	TAY
	LDA	#IMM
	TAX

	LDY	ABS
	LDA	ABS
	LDX	ABS
	BBS2	ZP, *
;
; $B0 - $BF
;
	BCS	*
	LDA	(IND), Y
	LDA	(IND)

	LDY	ZP, X
	LDA	ZP, X
	LDX	ZP, Y
	SMB3	ZP
	CLV
	LDA	ABS, Y
	TSX

	LDY	ABS, X
	LDA	ABS, X
	LDX	ABS, Y
	BBS3	ZP, *
;
; $C0 - $CF
;
	CPY	#IMM
	CMP	(IND, X)


	CPY	ZP
	CMP	ZP
	DEC	ZP
	SMB4	ZP
	INY
	CMP	#IMM
	DEX

	CPY	ABS
	CMP	ABS
	DEC	ABS
	BBS4	ZP, *
;
; $D0 - $DF
;
	BNE	*
	CMP	(IND), Y
	CMP	(IND)


	CMP	ZP, X
	DEC	ZP, X
	SMB5	ZP
	CLD
	CMP	ABS, Y
	PHX


	CMP	ABS, X
	DEC	ABS, X
	BBS5	ZP, *
;
; $E0 - $EF
;
	CPX	#IMM
	SBC	(IND, X)


	CPX	ZP
	SBC	ZP
	INC	ZP
	SMB6	ZP
	INX
	SBC	#IMM
	NOP

	CPX	ABS
	SBC	ABS
	INC	ABS
	BBS6	ZP, *
;
; $F0 - $FF
;
	BEQ	*
	SBC	(IND), Y
	SBC	(IND)


	SBC	ZP, X
	INC	ZP, X
	SMB7	ZP
	SED
	SBC	ABS, Y
	PLX


	SBC	ABS, X
	INC	ABS, X
	BBS7	ZP, *

	END
argv) {