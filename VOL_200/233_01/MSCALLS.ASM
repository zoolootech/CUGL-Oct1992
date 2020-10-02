;   VERSION 0001  (DATE: 17/05/87)  (TIME: 12:47)   ;
COMMENT *
	macros to be used in calling MicroSoft Fortran (v. 3.2)
	double precision transcendentals from Data Light C compiler
	
	file:	mscalls.asm
	date:	17.V.87
	author: J. W. Haefner

	use:	see "fsin.asm"
	requires: MACROS.ASM from DLC
*
INCLUDE	MACROS.ASM

callpro	MACRO	res
	push	BP		;save old frame ptr
	mov	BP,SP
	mov	BX,BP
	add	BX,p
	push	SS		;push segment of arg, reqd by MSFortran
	push	BX		;addr of sin arg on stack
	mov	BX,OFFSET res	;offset of temp var for return
	push	BX
	ENDM

callepi MACRO
;	mov	SP,BP		;MS routines do this already
	mov	DX,[temp+0]	;DLC expects LSB in DL
	mov	CX,[temp+2]
	mov	BX,[temp+4]
	mov	AX,[temp+6]	;DLC expects MSB in AH
	pop	BP		;regain old frame ptr
	ENDM
