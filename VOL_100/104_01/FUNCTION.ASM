	title	'string assebmle function for BDS C'
	page	60
;

	maclib	bds
	maclib	cmac
;
;
	direct
	define	STRLEN
	define	STRCPY
	define	STRCAT
	define	STRPOS
	enddir
;
	page
;
;	strlen
;
;	return lenght of string
;
	prelude	strlen
;

strlen:
	call	arghak		;get arg list
	lhld	arg1		;get point to string
	xchg			;put address in de
	lxi	h,0		;put a zero in hl (length)
strlen1:
	ldax	d		;get value of this character
	ora	a		;check to see if zero
	rz			;if zero end of string return
	inx	h		;add 1 to string lenght
	inx	d		;add 1 to pointer to string
	reloc	jmp,strlen1	;check next character
;
	postlude strlen
;
;
;
	prelude	strcpy
;
strcpy:
	call	arghak		;get arg list
	lhld	arg1		;get address of where to save string
	xchg			;put address in de
	lhld	arg2		;get address of string to copy
strcpy1:
	mov	a,m		;get value from source string
	stax	d		;save character a object string
	ora	a		;check to see if done
	rz			;if zero done return to caller
	inx	h		;add 1 to source pointer
	inx	d		;add 1 to object pointer
	reloc	jmp,strcpy1	;down till end of string
;
	postlude strcpy
;
;
;
	prelude	strcat
strcat:
	call	arghak	;get parm list
	lhld	arg1	;get address of frist parm
	xchg		;put it in hl
	lhld	arg2	;get second parm
strcat1:
	ldax	d		;get byte to see if zero
	ora	a		;set status flags
	reloc	jz,strcat2	;end of string 1 
	inx	d		;add 1 to pointer
	reloc	jmp,strcat1	;loop till end of string 1
strcat2:
	mov	a,m		;get char for source string
	stax	d		;save at end of second string
	ora	a		;check to see if zero
	rz			;return if end of string 2
	inx	h		;add 1 to source pointer
	inx	d		;add 1 to object pointer
	reloc	jmp,strcat2	;loop till end of source string
	postlude strcat
;
;
;
	prelude	strpos
;
strpos:
	call	arghak		;get parm list
	push	b		;save c stack on machine stack
	lhld	arg1		;get string that we are searching in
	xchg			;put address in de
	lxi	b,0		;zero postion 
strpos1:
	lhld	arg2		;get address of search for string
strpos2:
	ldax	d		;get character to check 
	ora	a		;check to see if end of string 1
	reloc	jz,strpos4	;end of string arg2 is not inside of arg1
	inx	b		;add 1 to postion
	inx	d		;add 1 top pointer of arg1
	cmp	m		;check to see if a match on first char
	reloc	jnz,strpos2	;loop till fisrt character match
	push	d		;save current postion on stack
	dcx	d		;move pointer back one
strpos3:
	inx	h		;add 1 to arg1
	inx	d		;add 1 to arg2
	mov	a,m		;check to see if end of arg2
	ora	a		;see if zero 
	reloc	jz,strpos5	;end of arg2 match found 
	ldax	d		;get arg1
	cmp	m		;check to see if match
	reloc	jz,strpos3	;loop to end of string or nomatch
	pop	d		;remove search address from stack
	reloc	jmp,strpos1	;loop till end of arg1
strpos4:
	pop	b		;restore c stack pointer
	lxi	h,0		;arg2 not found in arg1
	ret
strpos5:
	mov	h,b		;arg2 found in arg1
	mov	l,c
	pop	d		;remove junk from stack
	pop	b		;restore b stack pointer
	ret
;
;
	postlude strpos
	end
lude strpos
	end

end

	ret
;
;
	postlude strpos
	end