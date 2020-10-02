conino:	call	test	;a<--- iconin (console input character)
conin:	in 	cstat	;a<--- console status
	ani	imask
	jz	conino	;jump to memory test if no character ready
	in 	cdata	; get character into A
	ani	7fh	; mask parity bit out
	ret

; *****************************************************************************
; This is the memory test routine that gets called if the computer is         *
; idle, that is, waiting for character input from the console.                *
; Taken from Lifelines Volume I No. 12 (May 1981)                             *
; Patched in by Barry A. Dobyns, 12 June 1981                                 *
; *****************************************************************************

test:	push 	h
	lhld	tstpt	;HL = test address
	inx 	h	;HL++
	mov	a,h
	cpi	$/100h
	jnz	test1	; jump if below current location
	lxi	h,0	; re-initialize test pointer
test1:	shld	tstpt	; increment memory pointer
	mov	a,m	; test memory @ tstpt
	cma
	mov 	m,a
	cmp 	m
	jz 	test2	;jump if memory ok at tstpt
	xra	m	;a = bad bits
	push 	psw
	call 	prtstr
	db	cr,lf,'*** ',0
	pop	psw	;a = bad bits (still)
	push 	psw
	call	prthx	;display bad bits
	call	prtstr
	db	' Bits BAD at: ',0
	mov	a,h
	call	prthx   ;display bad address
	mov	a,l
	call	prthx
	pop     psw
test2:  cma
	mov	m,a
	pop	h
	ret

prthx:  push	psw     ;prthx (print a-reg in hex)
	rrc
	rrc
	rrc
	rrc
	call 	prthx1	;print most significant nibble
	pop	psw
prthx1: ani	0fh
	cpi	10
	jc	prthx2
	adi	7
prthx2: adi	'0'	;(zero)

output: push	b
	mov	c,a
	call	conout	;output char to console
	pop	b
	ret

prtstr:	xthl		;print chars in string following call
	push	psw	;to prtstr	
	call	prt
	pop	psw
	xthl
	ret
prt:	mov	a,m
	inx	h
	ana	a
	rz
	call	output	;output char
	jmp	prt

tstpt:	dw	0	;current address of test point 

	
	current location
	lxi	h,0	; re-initialize test pointer