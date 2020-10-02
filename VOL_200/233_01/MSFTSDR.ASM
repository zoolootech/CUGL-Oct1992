;   VERSION 0011  (DATE: 18/05/87)  (TIME: 15:02)   ;
COMMENT *
  CALL MSFORTRAN DOUBLE PRECISION TRANSCENDENTALS FROM DLC
	date: 15 May 1987
	file: MSFTSDR.ASM
	use:  E.G.:	double y,x;
	      		y=sin(x);

	mods: 17 May 1987: use macros for repetitious parts
*

INCLUDE	MSCALLS.ASM

begdata

temp	DW	4 DUP (0)	;to hold real*8 returned
				;MS FORTRAN puts LSB of IEEE REAL*8 in
				;temp[0] and MSB in temp[7]
enddata

	;assume MS compiled as large model
	EXTRN	SNDRQQ:FAR	;SIN
	EXTRN	CNDRQQ:FAR	;COSINE
	EXTRN	TNDRQQ:FAR	;TANGET
	EXTRN	ASDRQQ:FAR	;ARC SINE
	EXTRN	ACDRQQ:FAR	;ARC COSINE
	EXTRN	ATDRQQ:FAR	;ARC TANGENT
	EXTRN	A2DRQQ:FAR	;ARC TANGENT2
	EXTRN	SHDRQQ:FAR	;HYPERBOLIC SINE
	EXTRN	CHDRQQ:FAR	;    "      COSINE
	EXTRN	THDRQQ:FAR	;    "      TANGENT
	EXTRN	EXDRQQ:FAR	;EXPONENTIATION
	EXTRN	LNDRQQ:FAR	;NATURAL LOG
	EXTRN	LDDRQQ:FAR	;COMMON LOG
	EXTRN	SRDRQQ:FAR	;SQ.ROOT
	EXTRN	PIDRQQ:FAR	;POWER: REAL BASE TO INTEGER
	EXTRN	PRDRQQ:FAR	;POWER: REAL BASE TO REAL

begcode
	PUBLIC	sin		;make sin func global
	PUBLIC	cos
	PUBLIC	tan
	PUBLIC	asin
	PUBLIC	acos
	PUBLIC	atan
	PUBLIC	sinh
	PUBLIC	cosh
	PUBLIC	tanh
	PUBLIC	exp
	PUBLIC	log
	PUBLIC	log10
	PUBLIC	sqrt
	PUBLIC  atan2
	PUBLIC  pow
	PUBLIC  ipow		;real raised to integer

func	sin
	callpro temp		;macro: sin result into temp
	call	FAR PTR SNDRQQ	;MSFORTRAN double precision DSIN
	callepi			;macro: xfer temp to AX,BX,CX,DX
	ret
sin	endp

func	cos
	callpro temp
	call	FAR PTR CNDRQQ	;MSFORTRAN DCOS
	callepi
	ret
cos	endp

func	tan
	callpro temp
	call	FAR PTR TNDRQQ	;MSFORTRAN double prec. DTAN
	callepi
	ret
tan	endp

func	asin
	callpro temp
	call	FAR PTR ASDRQQ	;MSFORTRAN DASIN
	callepi
	ret
asin	endp

func	acos
	callpro temp
	call	FAR PTR ACDRQQ	;MSFORTRAN DACOS
	callepi
	ret
acos	endp

func	atan
	callpro	temp
	call	FAR PTR ATDRQQ	;MSFORTRAN DATAN
	callepi
	ret
atan	endp

func	sinh
	callpro	temp
	call	FAR PTR SHDRQQ	;MSFORTRAN DSINH
	callepi
	ret
sinh	endp

func	cosh
	callpro	temp
	call	FAR PTR CHDRQQ	;MSFORTRAN DCOSH
	callepi
	ret
cosh	endp

func	tanh
	callpro	temp
	call	FAR PTR THDRQQ	;MSFORTRAN DTANH
	callepi
	ret
tanh	endp

func	exp
	callpro	temp
	call	FAR PTR EXDRQQ	;MSFORTRAN DEXP
	callepi
	ret
exp	endp

func	log
	callpro	temp
	call	FAR PTR LNDRQQ	;MSFORTRAN DLOG
	callepi
	ret
log	endp

func	log10
	callpro	temp
	call	FAR PTR LDDRQQ	;MSFORTRAN DLOG10
	callepi
	ret
log10	endp

func	sqrt
	callpro temp
	call	FAR PTR SRDRQQ	;MSFORTRAN DSQRT
	callepi
	ret
sqrt	endp

func	atan2			;has 2 args
	push	BP		;save old frame ptr
	mov	BP,SP
	mov	BX,BP
	add	BX,p		;span ret addr
	push	SS		;push segment of arg, reqd by MSFortran
	push	BX		;addr of numerator on stack
	add	BX,8		;span first C arg
	push	SS
	push	BX		;addr of denominator on stack
	mov	BX,OFFSET temp	;offset of temp var for return
	push	BX
	call	FAR PTR A2DRQQ	;MSFORTRAN DATAN2
	callepi
	ret
atan2	endp

func	pow
	push	BP		;save old frame ptr
	mov	BP,SP
	mov	BX,BP
	add	BX,p
	push	SS		;push segment of arg, reqd by MSFortran
	push	BX		;addr of double base arg on stack
	add	BX,8		;bytes in double
	push	SS
	push	BX		;addr of double exponent on stack
	mov	BX,OFFSET temp	;offset of temp var for return
	push	BX
	call	FAR PTR PRDRQQ	;MSFORTRAN X**Y
	callepi
	ret
pow	endp

func	ipow			;double x,y; long i; y=ipow(x,i)
	push	BP		;save old frame ptr
	mov	BP,SP
	mov	BX,BP
	add	BX,p
	push	SS		;push segment of arg, reqd by MSFortran
	push	BX		;addr of double base on stack
	add	BX,8		;bytes in double
	push	SS
	push	BX		;addr of long exponent on stack
	mov	BX,OFFSET temp	;offset of temp var for return
	push	BX
	call	FAR PTR PIDRQQ	;MSFORTRAN X**I
	callepi
	ret
ipow	endp

endcode
	END
