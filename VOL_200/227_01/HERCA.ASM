	page	60,132
;--------------------------------------------------------------------------
; 	h e r c a . a s m
;       -----------------
;	Low-level driver for the hercules monochrome graphics card.
;
;	update history
;	--------------
;	May, 26. 1987	function "setbyte" added
;	August, 16 1987	xor functionality added
;
;	Written by 	Rainer Gerhards
;			Petronellastr. 6
;			D-5112 Baesweiler
;			Phone (49) 2401 - 1601
;--------------------------------------------------------------------------

	include	dos.mac
	
	if1
	  %out	Info: Hercules graphics card driver selected.
	endif

VIDBASE	equ	0b800h				; Videoram base address

MUL90	macro	srcreg, zwsp, addreg
	mov	zwsp, srcreg
	shl	srcreg, 1			; 2 *
	add	srcreg, zwsp			; 3 *
	shl	srcreg, 1			; 6 *
	ifnb	<addreg>
	sub	addreg, srcreg			; correct 96 * to 90 *
	else
	mov	zwsp, srcreg
	endif
	shl	srcreg, 1			; 12 *
	shl	srcreg, 1			; 24 *
	shl	srcreg, 1			; 48 *
	shl	srcreg, 1			; 96 *
	ifnb	<addreg>
	add	addreg, srcreg			; 6 * were subtracted before
	else
	sub	srcreg, zwsp			; 90 * !
	endif
	endm
;
YOFFSET	macro	yco, zwsp, adrreg
	mov	zwsp&h, yco&l			; select bank
	and	zwsp&x, 0300h
	rept	5
	shl	zwsp&h, 1			; 0000 2000 4000 6000
	endm
	mov	adrreg, zwsp&x
;
	shr	yco&x, 1			; 90 * (y mod 4)
	shr	yco&x, 1			; y mod 4
	MUL90	yco&x, zwsp&x, adrreg		; adrreg = adrreg + 90 * yco&x
	endm
;
XOFFSET	macro	xco, zwsp, adrreg
	ifdif	<zwsp>, <xco>
	mov	zwsp, xco			; x-coordinate
	endif
	rept	3
	shr	zwsp, 1
	endm
	ifnb	<adrreg>
	add	adrreg, zwsp			; pixel byteaddress
	endif
	endm
;
BYTEADR	macro	xco, yco, zwsp, adrreg
	YOFFSET	yco, zwsp, adrreg		; compute y-Offset
	XOFFSET xco, yco&x, adrreg		; add x-byteoffset
	endm

; ------------------- end macros ----------------------------------------

	SETX

MAX_X	equ	720			; maximum pixel x
MAX_Y	equ	348			; maximum pixel y
	PSEG

	BEGIN	clrgraph
;**
;	name		clrgraph
;
;	synopsis	clrgraph(mask);
;			int mask;    Color-mask used to clear the screen
;
;	description	Clears the graphics screen (page 1) with a
;			user-specified attribute (useally 0 or 0ffffh).
;**
	ENTERF
	push	es
	mov	dx, VIDBASE			; videoram-segment
	mov	es, dx
	xor	di, di				; offset zero
	cld					; count upward
	mov	ax, [bp]+X			; load attribute
	mov	cx, 4000h			; num. words in memory
rep	stosw					; clear the page!
	pop	es
	LEAVEF
	ENDFUNC	clrgraph			; done!

	BEGIN	intoff
;**
;	name		intoff
;
;	synopsis	intoff();
;
;	description	This function switches all interrupts (except NMI)
;			off.
;**
	cli					; only one instruction!
	ret
	ENDFUNC	intoff

	BEGIN	inton
;**
;	name		inton
;
;	synopsis	inton();
;
;	description	This function switches all interrupts on.
;**
	sti					; only one instruction!
	ret
	ENDFUNC	inton

	BEGIN	setpixel
;**
;	name		setpixel
;
;	synopsis	setpixel(x, y, color);
;			int x, y;	coordinate of the pixel
;			int color;	pixel-color
;
;	description	This function sets a pixel of color "color" at
;			the specified coordinate.
;**
	ENTERF
	mov	si, [bp]+X			; load x value
	cmp	si, MAX_X			; x range ok?
	jae	sexit
	mov	ax, [bp]+X+2			; load y value
	cmp	ax, MAX_Y			; y range ok?
	jae	sexit
	mov	bx, [bp]+x+4			; load color
;
	BYTEADR	si, a, c, di 			; byteoffset -> di
;
	mov	ax, bx				; color,
	mov	ah, 01h				; use only least significant
	and	al, ah				; bit!
	mov	cx, si				; x-coordinate
	and	cl, 7				; 7 - last 3 bit
	xor	cl, 7				; = Bitoffset
	shl	ax, cl				; shift color and mask
;
	not	ah				; Mask pixel off
	mov	cx, VIDBASE			; load base adr
	mov	si, es				; save ES
	mov	es, cx
	mov	cl, es:[di]			; read pixel
	cmp	bx, -1				; xor?
	jne	wrtpx
	xor	cl, al
	jmp	short rwrt			; rewrite to display
wrtpx:	and	cl, ah	  			; clear old color
	or	cl, al				; set new color
rwrt:	mov	es: [di], cl			; back to display memory! 
	mov	es, si				; restore saved ES
sexit:	LEAVEF					; done!
	ENDFUNC	setpixel

	BEGIN	getpixel
;**
;	name		getpixel
;
;	synopsis	color = getpixel(x, y);
;			int x, y;	coordinate of the pixel
;			int color;	returned color of that pixel
;
;	description	This function reads ("gets") the color of a
;			specified pixel.
;**
	ENTERF
	mov	si, [bp]+X			; load x coordinate
	cmp	si, MAX_X			; x range ok?
	jae	gexit
	mov	ax, [bp]+X+2			; load y coordinate
	cmp	ax, MAX_Y			; y range ok?
	jae	gexit
;
	BYTEADR	si, a, c, di 			; byteoffset -> di
;
	mov	ax, VIDBASE			; load segment graphic page 1
	mov	cx, es				; save ES
	mov	es, ax
	mov	bl, es:[di]			; this byte contains the pixel
	mov	es, cx				; restore saved ES
	or	bl, bl				; any pixel set?
	jz	goret				; no, done!
	mov	cx, si				; x-coordinate
	and	cl, 7				; 7 - last 3 bit
	xor	cl, 7				; = shiftcount
	shr	bl, cl				; shift to position 0
	and	bl, 01				; last bit only
;
goret:	mov	ax, bx				; function result must 
gexit:	LEAVEF					; be in ax
	ENDFUNC	getpixel

	BEGIN	setbyte
;**
;	name		setbyte
;
;	synopsis	setbyte(x, y, gr_byte);
;			int x;		x-coordinate of the byte, only
;					valid if multiple of 8
;			int y;		y-coordinate of the byte, may have
;					any valid value
;			int gr_byte;	byte of graphics to set
;					if (gr_byte >> 8) == 0xff then
;					xor vidmem with gr_byte & 0xff!
;
;	description	This function puts the graphics pattern contained
;			in gr_byte into the adapter memory. Gr_byte must
;			be a multiple of 8, all other values are invalid
;			and will be rounded. Gr_byte is defined as con-
;			secutive 8 pixles on the x-axis.
;
;	warning		This function is only available with the hercules
;			driver. It schould be treated as available internal
;			to the grphics library only. If you have to use it,
;			use conditional compilation, too. This function is
;			only available when #ifdef HERCGRAF == true!
;**
	ENTERF
	mov	si, [bp]+X			; load x value
	cmp	si, MAX_X			; x range ok?
	jae	bexit
	mov	ax, [bp]+X+2			; load y value
	cmp	ax, MAX_Y			; y range ok?
	jae	bexit
;
;	everything ok - can perform my job.
;
	mov	bx, [bp]+X+4			; load graphics information
	BYTEADR	si, a, c, di 			; byteoffset -> di
	mov	cx, VIDBASE			; load base adr of page 1
	mov	si, es				; save ES
	mov	es, cx
	cmp	bh, 0ffh			; xor byte?
	jne	endbyt
	xor	es: [di], bl
	jmp	short endsb
endbyt:	mov	es: [di], bl			; set screen memory
endsb:	mov	es, si				; restore saved ES
bexit:	LEAVEF					; done!
	ENDFUNC	setbyte

	BEGIN	getbyte
;**
;	name		getbyte
;
;	synopsis	gr_byte = getbyte(x, y);
;			int x;		x-coordinate of the byte, only
;					valid if multiple of 8
;			int y;		y-coordinate of the byte, may have
;					any valid value
;			int gr_byte;	byte of graphics to read
;
;	description	This function reads the whole byte of graphics at
;			the specified position. That means, it read 8
;			consecutive pixel.
;
;	warning		This function is only available with the hercules
;			driver. It schould be treated as available internal
;			to the grphics library only. If you have to use it,
;			use conditional compilation, too. This function is
;			only available when #ifdef HERCGRAF == true!
;
;	NOT FULLY TESTED!
;
;**
	ENTERF
	mov	si, [bp]+X			; load x value
	cmp	si, MAX_X			; x range ok?
	jae	gbexit
	mov	ax, [bp]+X+2			; load y value
	cmp	ax, MAX_Y			; y range ok?
	jae	gbexit
;
	BYTEADR	si, a, c, di 			; byteoffset -> di
	mov	cx, VIDBASE			; load base adr of page 1
	mov	si, es				; save ES
	mov	es, cx
	mov	al, es: [di]			; get 8 pixel
	and	ax, 0ffh			; mask off high byte
	mov	es, si				; restore saved ES
gbexit:	LEAVEF					; done!
	ENDFUNC	getbyte
	ENDPS
	end
	END
