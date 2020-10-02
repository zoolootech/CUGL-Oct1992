;
;	absolute sector read routine
;	----------------------------
;	written by	Rainer Gerhards
;			Petronellastr. 6
;			D-5112 Baesweiler
;			West Germany
;
;	This Module supports only the small memory model of any compiler!
;
X	equ	4
_TEXT	segment	byte public 'CODE'	; Turbo C (and MSC?)
;PROG	segment	byte public 'PROG'	; Lattice and Dataligth C
	assume	cs:_TEXT
	public	_rdsct
_rdsct	proc	near			; use _rdsct for Turbo and MS C
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
	mov	al, 0			; from drive A:
	int	25h			; read sector
	jc	err			; error?
	xor	ax, ax			; no, clear ruturn value
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
_rdsct	endp				; _rdsct for Turbo and MS C
_TEXT	ends				; _TEXT for Turbo and MS C
;PROG	ends
	end
