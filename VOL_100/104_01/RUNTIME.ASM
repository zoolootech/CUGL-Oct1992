	title	runtime package for small "C"

ram	equ	0		;start of ram in system
	extrn	main

;********************************************************
;							*
;	run time libray for small c compiler		*
;							*
;		by Ron Cain				*
;							*
;********************************************************
;
;	fetch a single byte from stack pointer plus
;	offset of byte following call and sign extend
;	into hl
;
@gcharss:csect
	pop	b	
	ldax	b	
	mov	l,a	
	inx	b	
	mvi	h,0	
	dad	sp	
	push	b	
	mov	a,m	
	jmp	@sxt
;
;	fetch a single byte from stack plus a 2 byte offset
;	and sign extend byte into hl
;
@gcharsl:csect
	pop	b
	ldax	b
	mov	l,a	
	inx	b	
	ldax	b	
	mov	h,a	
	inx	h	
	dad	sp	
	push	b	
	mov	a,m	
	jmp	@sxt	
;
;	fetch a single byte from address in hl and
;	sign extend into hl
;
@gchar: csect
	mov	a,m
	jmp	@sxt
;
;	put the accum into hl and sign extend through h
;
@sxt:	csect
	mov	l,a
	rlc
	sbb	a
	mov	h,a
	ret
;
;	fetch a full 16-bit integer from offset plus stack pointer
;	into hl
;
@gintss:csect
	pop	b	
	ldax	b	
	mov	l,a	
	mvi	h,0	
	inx	b	
	dad	sp	
	push	b	
	mov	a,m	
	inx	h	
	mov	h,m	
	mov	l,a	
	ret		
;
;	fetch a 16-bit integer from stack pointer plus
;	a 16 bit offset and put value in hl
;
@gintsl:csect
	pop	b
	ldax	b
	mov	l,a
	inx	b
	ldax	b
	mov	h,a
	inx	b
	dad	sp
	push	b
	mov	a,m
	inx	h
	mov	h,m
	mov	l,a
	ret		
;
;	fetch a full 16-bit integer from the address in hl
;	into hl
;
@gint:	csect
	mov	a,m
	inx	h
	mov	h,m
	mov	l,a
	ret
;
;	store a byte stack plus 8 byte offset
;
@pcharss:csect
	xchg	
	pop	b
	ldax	b
	mov	l,a
	mvi	h,0
	inx	b
	dad	sp
	mov	m,e
	xchg
	push	b
	ret
;
;	store a byte at stack plus offset
;
@pcharsl:csect
	xchg
	pop	b
	ldax	b
	mov	l,a
	inx	b
	ldax	b
	mov	h,a
	inx	b
	dad	sp
	mov	m,e		
	xchg
	push	b
	ret
;
;	store 16-bit integer at offset plus stack 
;
@pintss:csect
	xchg
	pop	b
	ldax	b
	mov	l,a
	mvi	h,0
	inx	b
	dad	sp
	mov	m,e
	inx	h
	mov	m,d
	xchg
	push	b
	ret
;
;	store a 16 bit integer in hl at stack plus offset
;
@pintsl:csect
	xchg
	pop	b
	ldax	b
	mov	l,a
	inx	b
	ldax	b
	mov	h,a
	inx	b
	dad	sp
	mov	m,e
	inx	h
	mov	m,d
	push	b
	xchg
	ret
;
;	store a 16-bit integer in hl at the address in de
;
@pint:	csect
	mov	a,l
	stax	d
	inx	d
	mov	a,h
	stax	d
	ret
	page
@incdec:csect
;
;	take the address in hl and add value that follow and save back
;	address in hl
;
@preinc:
	call	@incdecl
	call	@inc
	nop
	ret	
;
;	take the address in hl and sub value that follow and
;	save it back in place
;
@predec:
	call	@incdecl
	call	@dec
	nop
	ret
;
;	take the address in hl and sub value that follow and save
;	back in address in hl and restore value to pre inc
;
@postinc:
	call	@incdecl
	push	d
	call	@inc
	pop	h
	nop
	ret
;
;	take the address in hl and add value that follow and
;	save back in address in hl restore value pre dec
;
@postdec:
	call	@incdecl
	push	d
	call	@dec
	pop	h
	nop
	ret	
;
;	load needed valut for inc and dec 
;
@incdecl:
	pop	d
	pop	b
	ldax	b
	inx	b
	push	b
	push	d
	dcx	b
	ora	a
	jp	@incdec1
	mov	e,m
	inx	h
	mov	d,m
	ret
@incdec1:
	mov	e,m
	mov	a,e
	rlc
	sbb	a
	mov	d,a
	ret
;
;	add value in a to de and save at address in hl
;
@inc:
	ani	07fh
	add	e
	mov	e,a
	mov	a,d
	aci	0
	mov	d,a
	jmp	@incdecs
;
;	subtract value in a to de and save at address in hl
;
@dec:
	ani	07fh
	cma
	inr	a
	add	e
	mov	e,a
	mov	a,d
	aci	0ffh
	mov	d,a
	jmp	@incdecs 
;
;	store value in de at address in hl
;
@incdecs:
	ldax	b
	ora	a
	jp	@incdec2
	mov	m,d
	dcx	h
	mov	m,e
	xchg
	mov	a,h
	ora	l
	ret
@incdec2:
	mov	m,e
	xchg
	mov	a,h
	ora	l
	ret	
	page
;
;	inclusive "or" hl and de into hl
;
@or:	csect
	pop	b
	pop	d
	push	b
	mov	a,l
	ora	e
	mov	l,a
	mov	a,h
	ora	d
	mov	h,a
	ora	l
	ret
;
;	excluseive "or" hl and de into hl
;
@xor:	csect
	pop	b
	pop	d
	push	b
	mov	a,l
	xra	e
	mov	l,a
	mov	a,h
	xra	d
	mov	h,a
	ora	l
	ret
;
;	"and" hl and de into hl
;
@and:	csect
	pop	b
	pop	d
	push	b
	mov	a,l
	ana	e
	mov	l,a
	mov	a,h
	ana	d
	mov	h,a
	ora	l
	ret
;
;	in all the following compare routines hl is set to 1 
;	if the condition is true, otherwise it is set to 0 
;
;	not current condition
;
@nlog:	csect
	mov	a,h
	ora	l
	jnz	@nlog1
	lxi	h,1
	mov	a,h
	ora	l
	ret
@nlog1:
	lxi	h,0
	mov	a,h
	ora	l
	ret
	page
@comp:	csect
;
;	test if hl equal to de
;
@eq:
	pop	b
	pop	d
	push	b
	lxi	b,@compret
	push	b
	call	@cmp
	rz	
	dcx	h
	ret
;
;	test if de to hl for not equal
;
@ne:
	pop	b
	pop	d
	push	b
	lxi	b,@compret
	push	b
	call	@cmp
	rnz
	dcx	h
	ret
;
;	test if de greater then hl (signed)
;
@gt:
	pop	b
	pop	d
	push	b
	lxi	b,@compret
	push	b
	xchg
	call	@cmp
	rc
	dcx	h
	ret
;
;	test if de less then hl (signed)
;
@lt:
	pop	b
	pop	d
	push	b
	lxi	b,@compret
	push	b
	call	@cmp
	rc
	dcx	h
	ret
;
;	test if de greater then or equal to hl (signed)
;
@ge:
	pop	b
	pop	d
	push	b
	lxi	b,@compret
	push	b
	call	@cmp
	rz
	rnc
	dcx	h
	ret
;
;	test if de less than or equal to hl (signed)
;
@le:
	pop	b
	pop	d
	push	b
	lxi	b,@compret
	push	b
	call	@cmp
	rz
	rc
	dcx	h
	ret
	page
;
;	test if de greater than hl (unsigned)
;
@ugt:
	pop	b
	pop	d
	push	b
	lxi	b,@compret
	push	b
	xchg
	call	@ucmp
	rc
	dcx	h
	ret
;
;	test if de less than hl(unsigned)
;
@ult:
	pop	b
	pop	d
	push	b
	lxi	b,@compret
	push	b
	call	@ucmp
	rc
	dcx	h
	ret
;
;	test if de greater than or equal to hl (unsigned)
;
@uge:
	pop	b
	pop	d
	push	b
	lxi	b,@compret
	push	b
	call	@ucmp
	rnc
	dcx	h
	ret
;
;	test if de less than or equal to hl (unsigned)
;
@ule:
	pop	b
	pop	d
	push	b
	lxi	b,@compret
	push	b
	call	@ucmp
	rz
	rc
	dcx	h
	ret
;
;	common routine to preform a signed compare of
;	de and hl 
;
;	de-hl and sets the conditions:
;		carry set means de < hl
;		zero/non-zero set according to equality
;
@cmp:
	mov	a,e
	sub	l
	mov	e,a
	mov	a,d
	sbb	h
	lxi	h,1
	jm	@cmp1
	ora	e
	ret
@cmp1:
	ora	e
	stc
	ret
;
;	common routine to perform unsinged compare
;
;	carry set if de less than hl
;	zero/non-zero set accordingly hl=de
;
@ucmp:
	mov	a,d
	cmp	h
	jnz	@ucmp1
	mov	a,e
	cmp	l
@ucmp1:
	lxi	h,1
	ret
;
;	set machine status for all comditional operators
;
@compret:
	mov	a,h
	ora	l
	ret
	page
;
;	shift de arithmetically right by hl return in hl
;
@asr:	csect
	pop	b
	pop	d
	push	b
@asr1:
	xchg
	mov	a,h
	ral
	mov	a,h
	rar
	mov	h,a
	mov	a,l
	rar
	mov	l,a
	dcr	e
	jnz	 @asr1
	ret
;
;	shift de arithmetically left by hl and return in hl
;
@asl:	csect
	pop	b
	pop	d
	push	b
@asl1:
	xchg
	dad	h
	dcr	e
	jnz	@asl1
	ret
;
;	subtract hl from de and return in hl
;
@sub:	csect
	pop	b
	pop	d
	push	b
	mov	a,e
	sub	l
	mov	l,a
	mov	a,d
	sbb	h
	mov	h,a
	ret
;
;	from the two's complement of hl
;	
@neg:	csect
	call	@com
	inx	h
	ret
;
;	from the one's complement of hl
;
@com:	csect
	mov	a,h
	cma
	mov	h,a
	mov	a,l
	cma
	mov	l,a
	ret
;
;	mutiply de by hl and return in hl (signed)
;
@mult:  csect
	pop	b
	pop	d
	push	b
	mov	b,h
	mov	c,l
	lxi	h,0
@mult1:
	mov	a,c
	rrc
	jnc	@mult2
	dad	d
@mult2:
	xra	a
	mov	a,b
	rar
 	mov	b,a
	mov	a,c
	rar
	mov	c,a
	ora	b
	rz
	xra	a
	mov	a,e
	ral
	mov	e,a
	mov	a,d
	ral
	mov	d,a
	ora	e
	rz
	jmp	@mult1
;
;	divide de by hl and return quotient in hl
;	 return remainder in de (signed)
;
@div:	csect
	pop	b
	pop	d
	push	b
	mov	b,h
	mov	c,l
	mov	a,d
	xra	b
	push	psw
	mov	a,d
	ora	a
	cm	@deneg
	mov	a,b
	ora	a
	cm	@bcneg
	mvi	a,16
	push	psw
	xchg
	lxi	d,0
@div1:
	dad	h
	call	@rdel
	jz	@div2
	call	@cmpbcde
	jm	@div2
	mov	a,l
	ori	1
	mov	l,a
	mov	a,e
	sub	c
	mov	e,a
	mov	a,d
	sbb	b
	mov	d,a
@div2:
	pop	psw
	dcr	a
	jz	@div3
	push	psw
	jmp	@div1
@div3:
	pop	psw
	rp
	call	@deneg
	xchg
	call	@deneg
	xchg
	ret
;
;	negate the integer in bc (used by divide only)
;
@deneg:
	mov	a,d
	cma
	mov	d,a
	mov	a,e
	cma
	mov	e,a
	inx	d
	ret
;
;	negate then integer in bc (used by divide only)
;
@bcneg:
	mov	a,b
	cma
	mov	b,a
	mov	a,c
	cma
	mov	c,a
	inx	b
	ret
;
;	roate de left one bit (used by divide only)
;
@rdel:
	mov	a,e
	ral
	mov	e,a
	mov	a,d
	ral
	mov	d,a
	ora	e
	ret
;
;	compare bc to de (used by divide only)
;
@cmpbcde: 
	mov	a,e
	sub	c
	mov	a,d
	sbb	b
	ret
;
;	used by switch to search table
;	calling format form compiler
;	d-pointer to table
;	h-where to go if value not in table
;	b- number of  entry in table
;
@switch:csect
	xthl			;get value of expresstion to check
	xchg			;put value in de
@switch1:
	mov	a,e
	cmp	m		;check to see if low byte match
	inx	h		;move pointer to next byte of data
	jnz	@switch2	;no nov to next entry
	mov	a,d		;move high byte of data
	cmp	m		;check to see if match
	jnz	@switch2	;yes jump to address
	inx	h
	mov	e,m		;get low byte of address
	inx	h
	mov	d,m		;get high byte of address
	xchg			;put address in hl
	pop	b		;remove entry from stack
	pchl			;jump to needed case statement
@switch2:
	inx	h		;move pointer to next entry
	inx	h
	inx	h
	dcr	b		;check to see if done with scan
	jnz	@switch1
	xthl			;get address of where to go when done
	pop	b		;remove entry from stack
	pchl			;go there
;
;	call a bios routine with be loaded
;
bios:	csect
	pop	h
	pop	b
	pop	d
	push	d
	push	b
	push	h
	lhld	1
	dcx	h
 	dcx	h
	dcx	h
	mvi	d,0
	dad	d
	dad	d
	dad	d
	call	bios1
	mov	l,a
	mvi	h,0
	ret
bios1:	pchl
;
;	call bods to do a cpm function bods(c,de)
;
bdos:	csect
	pop	h
	pop	d
	pop	b
	push	b
	push	d
	push	h
	jmp	5
;
;	exit back to system
;
exit:	csect
	mvi	c,0
	call	5
;
;	take command buffer and build pointer list

@init:	csect
	lhld	6+ram			;get pointer to high memory
	dcx	h			;move to just below bdos
	sphl				;make it current stack
	lxi	h,80h+ram		;pointer to start of command buffer
	mov	e,m			;number of bytes in command line
	mvi	d,0			;make it 16 bit value
	dad	d			;point to end of text
	inx	h			;move past end of buffer
	mvi	m,0			;make end of text
	lxi	d,cpointer		;place where pointer should place
	lxi	h,80h+ram		;address of text string
	lxi	b,1			;number of arg
@init2:
	inx	h			;move pointer to next byte
	mov	a,m			;check for end of buffer
	ora	a			;set machine status
	jz	@init10			;yes call main line
	cpi	' '			;is there a leading space
	jz	@init2			;yes
	mov	a,l			;save pointer value
	stax	d
	mov	a,h
	inx	d
	stax	d
	inx	d			
	inr	c			;add 1 to arg count
@init3:
	inx	h			;move to next byte
	mov	a,m			;get byte into a
	ora	a			;see if end of buffer
	jz	@init10			;yes...
	cpi	' '			;check for space
	jnz	@init3			;no keep looking
	mvi	m,0			;mark as end of parm
	jmp	@init2			;loop for next command
@init10:
	lxi	h,cpointer-2		;get address of pointers
	push	b			;push argc on stack
	push	h			;push argv on stack
	call	main			;execute main line
	jmp 	0			;it just return from main exit to cpm
	dw	@ccnull
cpointer:
	dw	@ccnull,@ccnull,@ccnull,@ccnull,@ccnull
	dw	@ccnull,@ccnull,@ccnull,@ccnull,@ccnull
	dw	@ccnull,@ccnull,@ccnull,@ccnull,@ccnull
	dw	@ccnull,@ccnull,@ccnull,@ccnull,@ccnull
	dw	@ccnull,@ccnull,@ccnull,@ccnull,@ccnull
@ccnull:
	db	'noname',0	
	page
@oeq	equ	@eq-@comp
@one	equ	@ne-@comp
@ogt	equ	@gt-@comp
@olt	equ	@lt-@comp
@oge	equ	@ge-@comp
@ole	equ	@le-@comp

@ougt	equ	@ugt-@comp
@oult	equ	@ult-@comp
@ouge	equ	@uge-@comp
@oule	equ	@ule-@comp

@oprei	equ	@preinc-@incdec
@opred	equ	@predec-@incdec
@oposi	equ	@postinc-@incdec
@oposd	equ	@postdec-@incdec

	end
