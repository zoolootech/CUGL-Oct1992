;
; llint.asm
;
; version of 25-Mar-84
;
; a component of lsup.c
;
; Copyright 1984 (c) A. Skjellum. All rights reserved.
; 
; these routines are setup for Aztec C86 v 1.05i
;
; all procedures are "near"
;

dseg	segment para public 'data'
dseg	ends

cseg	segment para public 'code'
	assume	cs:cseg,ds:dseg,es:dseg,ss:dseg

;
; Routines which do not merit calls to portable routines in llsup.asm
;

;
; ds = flptr(lptr,sptr)
; LPTR *lptr;
; char *sptr;
;
; form a long pointer from a "normal" ds relative short pointer (sptr)
; and store at lptr
;
; note: no portable segment (flptr) in llsup.asm since this
; is such a trivial routine.
;
; return value is also ds, should this prove useful
;
	public	flptr_
flptr_	proc	near
	mov	bx,sp		; prepare for argument load
	mov	ax,4[bx]	; get short pointer ds:ax
	mov	bx,2[bx]	; get address where to store long pointer
	mov	[bx],ax		; store low order
	mov	ax,ds
	mov	2[bx],ax	; store high order
	ret			; return value is also ds
flptr_	endp

;
; the following four routines are examples of what can
; be done to supplement general routines with specific
; (more efficient ones).  Many more variations are
; possible than the two presented here.  They follow
; directly from this basic idea:
;
;
; char lchr(lptr)
; LPTR *lptr;
;
; return character pointed to by lptr
;
	public	lchr_
lchr_	proc	near
	mov	bx,sp		; prepare for argument load
	push	ds		; save ds register
	mov	bx,2[bx]	; get address of lptr
	mov	ax,[bx]		; 
	mov	ds,2[bx]	; begin forming pointer
	mov	bx,ax		; ds:bx now is valid pointer
	sub	ax,ax		; zero whole acc.
	mov	al,[bx]		; get the character
	pop	ds
	ret			; exit with char in ax
lchr_	endp

;
; int lint(lptr)
; LPTR *lptr;
; 
; return integer or unsigned pointed to by lptr
;
	public	lint_
lint_	proc	near
	mov	bx,sp		; prepare for argument load
	push	ds		; save ds register
	mov	bx,2[bx]	; get address of lpr
	mov	ax,[bx]		; 
	mov	ds,2[bx]	; begin forming pointer
	mov	bx,ax		; ds:bx now is valid pointer
	mov	ax,[bx]		; get the integer or unsigned
	pop	ds		; recover old ds value
	ret			; and exit with char in ax
lint_	endp

;
; l_stchr(lptr,chr)
; LPTR *lptr;
; char chr;
;
; store character chr at address lptr
;
	public	l_stchr_
l_stchr_ proc	near
	mov	bx,sp		; prepare for argument load
	mov	cl,4[bx]	; get character
	mov	bx,2[bx]	; prepare for load of long pointer
	push	ds		; save ds segment register
	mov	ax,[bx]		; 
	mov	ds,2[bx]	; begin forming pointer
	mov	bx,ax		; ds:bx now is valid pointer
	mov	[bx],cl		; store byte
	pop	ds
	ret
l_stchr_ endp

;
; l_stint(lptr,val)
; LPTR *lptr;
; int val;
;
; store integer or unsigned at address lptr
;
	public	l_stint_
l_stint_ proc	near
	mov	bx,sp		; prepare for argument load
	mov	cx,4[bx]	; get integer to store
	mov	bx,2[bx]	; prepare to form ds:bx with correct
				; storage address
	push	ds		; save current ds
	mov	ax,[bx]		; 
	mov	ds,2[bx]	; begin forming pointer
	mov	bx,ax		; ds:bx now is valid pointer
	mov	[bx],cx		; store the integer
	pop	ds
	ret			; exit 
l_stint_ endp

;
; lload(dest,lptr,len)
; char *dest;
; LPTR *lptr;
; unsigned len;
;
; general purpose copy routine from long data storage to ds: relative
; storage
;
; we assume es = ds for Aztec C explicitly here
;
; due to convenient 8086 instructions, the portable function would
; consist merely of a "cld", "rep movsb" sequence followed by a return.
; Therefore, no portable lload is included in llsup.asm
;

	public	lload_
lload_	proc	near
	mov	bx,sp		; prepare for argument load
	push	ds		; save Aztec ds segment
	push	si
	push	di		; save source and destination indices
	;
	mov	cx,6[bx]	; get length for move
	mov	di,2[bx]	; es:di now has the destination address
	mov	bx,4[bx]	; prepare to load long ptr
	mov	si,[bx]		; get low order
	mov	ds,2[bx]	; and then high order
	cld
	rep	movsb		; do the move
	pop	di
	pop	si
	pop	ds
	ret
lload_	endp
 
;
; lstor(lptr,src,len)
; LPTR *lptr;
; char *src;
; unsigned len;
;
; Reverse of lload: this routine copies data from ds:src to lptr
; Once again, there is no llsup analog.
;
	public	lstor_
lstor_	proc	near
	mov	bx,sp		; prepare for argument load
	push	es
	push	di
	push	si		; save registers as required by Aztec C.
	mov	cx,6[bx]	; get length of move
	mov	si,4[bx]	; ds:si now contains source index
	mov	bx,2[bx]	; prepare to form es:di
	mov	di,[bx]
	mov	es,2[bx]
	rep	movsb		; move the data
	pop	si
	pop	di
	pop	es
	ret			; restore registers and exit
lstor_	endp

;
; -----------------------------------------------------
;
; routines which call portable subroutines in llsup.asm
;
; -----------------------------------------------------
;
; linc(lptr)
; LPTR *lptr;
;
; increment a long pointer by 1
;
	public	linc_
linc_	proc	near
	extrn	linc:near
	mov	bx,sp		; prepare for argument load
	push	es		; save es value from caller
	mov	bx,2[bx]
	push	bx		; address where answer will go
	mov	es,2[bx]	; get segment
	mov	bx,[bx]		; and address
	call	linc		; do the work
	pop	ax
	xchg	ax,bx
	mov	[bx],ax
	mov	2[bx],es	; store the value
	pop	es		; recover the old value
	ret			; and exit
linc_	endp

;
; ldec(lptr)
; LPTR *lptr;
;
; decrement a long pointer by 1
;
	public	ldec_
ldec_	proc	near
	extrn	ldec:near
	mov	bx,sp		; prepare for argument load
	push	es		; preserve es
	mov	bx,2[bx]	; get address ds:bx
	push	bx		; address where answer will go
	mov	ax,[bx]		;
	mov	es,2[bx]
	mov	bx,ax
	call	ldec		; do the decrement
	pop	ax
	xchg	ax,bx
	mov	[bx],ax
	mov	2[bx],es	; store the value
	pop	es		; recover it for sake of caller
	ret			; and then exit
ldec_	endp

;
; ladd(lptr,offset)
; LPTR *lptr;
; unsigned offset;
;
; add unsigned offset to a long pointer
;
	public	ladd_
ladd_	proc	near
	extrn	ladd:near
	mov	bx,sp		; prepare for argument load
	push	es		; save es value of caller
	mov	ax,4[bx]	; get the offset to ax
	mov	bx,2[bx]
	push	bx		; address where answer will go too.
	mov	cx,[bx]
	mov	es,2[bx]
	mov	bx,cx
	call	ladd		; do the addition
	pop	ax
	xchg	ax,bx
	mov	[bx],ax
	mov	2[bx],es	; store the value
	pop	es		; recover old es value
	ret			; and then exit
ladd_	endp

;
; lsub(lptr,offset)
; LPTR *lptr;
; unsigned offset;
;
; subtract unsigned offset from a long pointer
;
	public	lsub_
lsub_	proc	near
	extrn	lsub:near
	mov	bx,sp		; prepare for argument load
	push	es		; preserve es 
	mov	ax,4[bx]	; get the offset
	mov	bx,2[bx]
	push	bx		; store answer at this addr. too.
	mov	cx,[bx]
	mov	es,2[bx]
	mov	bx,cx
	call	lsub		; do the subtraction
	pop	ax
	xchg	ax,bx
	mov	[bx],ax
	mov	2[bx],es	; store the value
	pop	es		; restore es
	ret			; and then exit
lsub_	endp

;
; lsum(lptr,offset)
; LPTR *lptr;
; int offset;
;
; add signed offset to a long pointer
;
	public	lsum_
lsum_	proc	near
	extrn	lsum:near
	mov	bx,sp		; prepare for argument load
	push	es		; preserve caller's es
	mov	ax,4[bx]	; get the signed offset
	mov	bx,2[bx]
	push	bx
	mov	cx,[bx]
	mov	es,2[bx]
	mov	bx,cs
	call	lsum		; do the signed addition
	pop	ax
	xchg	ax,bx
	mov	[bx],ax
	mov	2[bx],es	; store the value
	pop	es		;
	ret			; exit.
lsum_	endp

;
; lcopy(dest,src,len)
; LPTR *dest;
; LPTR *src;
; long len;	(treated as a long unsigned quantity)
;
; 	copy from src to dest, len bytes.
;
; note this routine can be used to copy arbitrarily large chunks of memory
;
	public	lcopy_
lcopy_	proc	near
	extrn	lcopy:near
	mov	bx,sp		; prepare for argument load
	push	ds
	push	es		; save segment registers
	push	di
	push	si		; save these registers for Aztec C
	;
	mov	cx,6[bx]	; get length (low order)
	mov	dx,8[bx]	; high order of length
	mov	ax,2[bx]	; get ds:ax as pointer to dest.
	xchg	ax,bx
	mov	di,[bx]
	mov	es,2[bx]
	mov	bx,ax
	mov	bx,4[bx]	; get ds:bx as pointer for dest.
	mov	si,[bx]
	mov	ds,2[bx]	; get long pointer
	;
	call	lcopy
	;
	pop	si
	pop	di
	pop	es
	pop	ds
	ret
lcopy_	endp	

cseg	ends
	end
