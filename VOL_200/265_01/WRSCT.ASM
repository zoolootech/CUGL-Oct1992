;
;	sector write routine
;	written by	Rainer Gerhards
;			Petronellastr. 6
;			D-5112 Baesweiler
;			West Germany
;
;	See rdsct.asm for segment and function names!
;
X	equ	4
;PROG	segment	byte public 'PROG'	; Lattice and Datalight
_TEXT	segment	byte public 'CODE'	; Microsoft and Borland
	assume	cs:_TEXT
	public	_wrsct			; without underline f. Lattice
_wrsct	proc	near			; and Datalight
	push	bp
	mov	bp, sp
	push	bx
	push	cx
	push	dx
	push	si
	push	di

	mov	dx, [bp]+X		; sector number
	mov	bx, [bp]+X+2		; buffer address (must be in DS:)
	mov	cx, 1			; always one sector
	mov	al, 0			; to drive A:
	int	26h			; write sector
	jc	err			; error?
	xor	ax, ax			; no, clear return value
	jmp	short return		; and exit
err:	mov	ax, 1			; indicate error

return:	popf				; leaved on stack (damned!)
	pop	di
	pop	si
	pop	dx
	pop	cx
	pop	bx
	pop	bp
	ret
_wrsct	endp
;PROG	ends
_TEXT	ends
	end
