;
; llsup.asm
;
; a component of lsup.c
;
; Copyright 1984 (c) A. Skjellum. All rights reserved.
; 
; version of 25-Mar-84
;
; This routine makes no assumptions about the behavior of the
; C compiler in use.
;
; all procedures are "near"
;

dseg	segment para public 'data'
dseg	ends

cseg	segment para public 'code'
	assume	cs:cseg,ds:dseg

;
; linc:		increment a long pointer by 1 byte
;
; expects:	es:bx with long pointer to increment
; returns:	pointer incremented.
; consumes:	es, bx, f, ax
;
	public	linc
linc	proc	near
	inc	bx		; increment low part of word
	or	bx,bx		; is it zero now?
	jnz	linc_exit	; no, we are done
	mov	ax,es
	add	ax,1000h	; another 64k of paragraphs
	mov	es,ax		; store back to es
linc_exit:
	ret			; return
linc	endp

;
; ldec:		decrement a long pointer by 1 byte
;
; expects:	es:bx with long pointer to decrement
; returns:	pointer decremented.
; consumes:	es, bx, f, ax
;
	public	ldec
ldec	proc	near
	or	bx,bx		; zero currently ?
	dec	bx		; decrement it
	jnz	ldec_exit	; just decrement low end and exit...
	mov	ax,es		; get segment register
	sub	ax,1000h	; remove 64k of paragraphs
	mov	es,ax		; store back to es
ldec_exit:
	ret			; return
ldec	endp

;
; ladd:		add a constant to a long pointer
;
; expects:	es:bx with long pointer's original value
;		ax with unsigned constant to be added
; returns:	pointer with constant added
; consumes:	es, bx, f, ax
;
	public	ladd
ladd	proc	near
	add	bx,ax		; add in offset
	jnc	ladd_exit	; no carry, so we are done.
	mov	ax,es
	add	ax,1000h	; add 64k of paragraphs
	mov	es,ax		; and store back to es
ladd_exit:
	ret
ladd	endp

;
; lsub:		subtract a constant from a long pointer
;
; expects:	es:bx with long pointer's original value
;		ax with unsigned constant to be subtracted
; returns:	pointer with constant subtracted
; consumes:	es, bx, f, ax
;
	public	lsub
lsub	proc	near
	sub	bx,ax		; subtract offset
	jnb	lsub_exit	; no borrow, so we are done.
	mov	ax,es
	sub	ax,1000h	; subtract 64k of paragraphs
	mov	es,ax		; and store back to es
lsub_exit:
	ret
lsub	endp

;
; lsum:		add a signed offset to a long pointer
;
; expects:	es:bx with long pointer
;		ax with signed offset
; returns:	pointer with constant added (signed)
; consumes:	es, bx, f, ax
;
	public	lsum
lsum	proc	near
	or	ax,ax		; negative?
	jm	lsum_neg
	call	ladd		; do addition
	ret			; and exit
lsum_neg:
	and	ax,07fffh	; and out sign flag
	jnz	lsum_neg_ok
	mov	ax,8000h	; -32768 value (don't treat as 0)
lsum_neg_ok:
	call	lsub
	ret
lsum	endp

;
; lcopy:	copy from one long pointer to another,
;		up to 1024k bytes of data
;
; expects:	ds:si with src  address
;		es:di with dest address
;		ds|cx with length (dx is high order, cx is low order)
;		
; returns:	block copied
;		ds, es intact
; consumes:	ax, cx, f
;
;
; 	this routine uses a copy downward method, to produce
;	correct copying for overlapping regions
;
	public	lcopy
lcopy	proc	near
	;
	; convert dx into segment form:
	;
	push	dx		; save original form of dx
	push	cx		; save low order of long count
	and	dx,15		; smallest meaningful value
	xchg	dh,dl		; switch upper and lower parts
	mov	cl,4
	shl	dh,cl		; effect is shift left by 12 bits
	pop	cx		; and recover low order of long count
	;
	mov	ax,es
	add	ax,dx
	mov	es,ax
	mov	ax,ds
	add	ax,dx
	mov	ds,ax		; gross adjustment of segments
	pop	dx		; recover original form of dx
	;	
	add	di,cx		; adjust dest. ptr to end of area
	jnc	no_dest_adj	
	mov	ax,es
	add	ax,1000h	; add offset
	mov	es,ax		; and store back to segment register
no_dest_adj:
	;
	; do same work for source pointer:
	;
	add	si,cx		; do the addition
	jnc	no_mor_adj	; no more adjustment needed if no carry
	mov	ax,ds
	add	ax,1000h	; do the adjustment
	mov	ds,ax		; and store back to ds
	;
	; at this stage: 
	;
	;	es:di is at the last byte of the dest. area
	;	ds:si is at the last byte of the src.  area
	;
no_mor_adj:
	std			; set direction flag for moves
lc_loop:
	or	si,si		; is si zero ?
	lodsb 			; get byte ds:[si], decrement si
	jnz	no_ds_adj	; no need to adjust if non-zero at start
	mov	ax,ds
	sub	ax,1000h
	mov	ds,ax		; adjust pointer for next load
no_ds_adj:
	or	di,di		; is di zero ?
	stosb			; set byte es:[di] = al, decrement di
	jnz	no_es_adj	; no need to adjust if non-zero at start
	mov	ax,es
	sub	ax,1000h	
	mov	es,ax		; adjust pointer for next store
no_es_adj:
	loop	lc_loop		; copy whole block (--cx, jnz lc_loop)
	dec	dx		; work on outer loop
	or	dx,dx
	jnz	lc_loop		; loop over dx counts too
	;
	; we are done
	;
	inc	si
	inc	di		; restore to original calling values
	ret			; exit
lcopy	endp

cseg	ends
	end
