	page	60,132
;--------------------------------------------------------------------------
; 	e g a a . a s m
;	---------------
;	Low-level driver for the ega device.
;
;	update history
;	--------------
;	Jun, 17. 1987	"dos.mac" and macro-usage added.
;
;	Written by 	Rainer Gerhards
;			Petronellastr. 6
;			D-5112 Baesweiler
;			Phone (49) 2401 - 1601
;--------------------------------------------------------------------------

	include	dos.mac
	
	if1
	  %out	Info: EGA driver selected.
	endif

LONG	macro	bed, ziel		; conditional jump >  127 byte
	local	l1, l2
	bed	l1
	jmp	short l2
l1:	jmp	ziel
l2:	
	endm

	SETX

MAX_X	equ	640			; maximum pixel x
MAX_Y	equ	350			; maximum pixel y

	EXTERN	vidbase,word,VID_BASE

	PSEG
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
	push	es
	mov	ax, VID_BASE			; load base adr
	mov	es, ax
	mov	di, [bp]+X			; load x value
	mov	bx, [bp]+X+2			; load y value
	cmp	di, MAX_X			; x range ok?
	jae	sexit
	cmp	bx, MAX_Y			; y range ok?
	jae	sexit
;
	mov	cx, bx
	shl	bx, 1				;  2 *
	shl	bx, 1				;  4 *
	add	bx, cx				;  5 *
	shl	bx, 1				; 10 *
	shl	bx, 1				; 20 *
	shl	bx, 1				; 40 *
	shl	bx, 1				; 80 * = y-byteoffset
	mov	cx, di
	rept	3
	shr	cx, 1				; x / 8 = x-byteoffset
	endm
	add	bx, cx				; total byteoffset
;
	mov	cx, di				; load x value
	and	cl, 07h				; x mod 8
	xor	cl, 07h				; shiftcount = 7 - x mod 8
	mov	ch, 1				; mask
	shl	ch, cl				; shift mask to position

	mov	dx, 03ceh
	mov	al, 05
	out	dx, al				; mode register
	inc	dx
	mov	al, 2
	out	dx, al				; write mode 2

	dec	dx
	mov	al, 8
	out	dx, al				; bit mask register
	inc	dx
	mov	al, ch
	out	dx, al				; store mask

	mov	al, es:[bx]
	mov	ax, [bp]+X+4			; load color
	mov	es:[bx], al			; write pixel

	mov	dx, 03ceh
	mov	al, 5
	out	dx, al				; mode register
	inc	dx
	xor	al, al
	out	dx, al				; write mode 0

	dec	dx
	mov	al, 8
	out	dx, al				; bit mask register
	inc	dx
	mov	al, 0ffh			; allow access to all bits
	out	dx, al
sexit:	pop	es				; done!
	LEAVEF
	ENDFUNC	setpixel

	BEGIN	setewm2
;**
;	name		setwm2
;
;	synopsis	setwm2()
;
;	description	The ega knows 4 different write modes. The write-pixel
;			routine uses write mode 2. After changing the write
;			mode, die default mode (0) must be restored. So the
;			write pixel routine has first to change the write mode
;			to 2, write the pixel and then restore write-mode 0.
;			This is very slow, if you've to write a lot of pixel.
;			This routine (and rsestdwm) allows the higer-level
;			routine to set write mode 2 at the beginning of her
;			processing, to call qsetpix (which assumes that mode 2
;			is set) to write the pixel and then restore the write-
;			mode to the default by calling rsestdwm. Because
;			qsetpix must'nt perform mode-switches, it is much
;			faster than setpix.
;	caution		When using this method, the write mode isn't restored
;			until the higher-level drawing procedure hasn't
;			finished. If this procedure would be aborted, the BIOS
;			will not restore the ega registers. So the video-
;			output will destroyed (display only grabbage). To
;			recover from this situation, just restore the original
;			ega register contents. This can by done by a little
;			program, calling only rsestdwm.
;**
	mov	dx, 03ceh
	mov	al, 05
	out	dx, al				; mode register
	inc	dx
	mov	al, 2
	out	dx, al				; write mode 2
	ret
	ENDFUNC	setewm2

	BEGIN	rsestdwm
;**
;	name		rsestdwm
;
;	synopsis	rsestdwm()
;
;	description	This routine restores the ega standard write mode.
;			For a deteiled discussion see function setewm2.
;**
	mov	dx, 03ceh
	mov	al, 5
	out	dx, al				; mode register
	inc	dx
	xor	al, al
	out	dx, al				; write mode 0

	dec	dx
	mov	al, 8
	out	dx, al				; bit mask register
	inc	dx
	mov	al, 0ffh			; allow access to all bits
	out	dx, al
	ret
	ENDFUNC	rsestdwm

	BEGIN	qsetpix
;**
;	name		qsetpix
;
;	synopsis	qsetpix(x, y, color);
;			int x, y;	coordinate of the pixel
;			int color;	pixel-color
;
;	description	This function sets a pixel of color "color" at
;			the specified coordinate, switching no write modes.
;			For a deteiled discussion see function setewm2.
;**
	ENTERF
	push	es
	mov	ax, VID_BASE			; load base adr
	mov	es, ax
	mov	di, [bp]+X			; load x value
	mov	bx, [bp]+X+2			; load y value
	cmp	di, MAX_X			; x range ok?
	jae	qsexit
	cmp	bx, MAX_Y			; y range ok?
	jae	qsexit
;
	mov	cx, bx
	shl	bx, 1				;  2 *
	shl	bx, 1				;  4 *
	add	bx, cx				;  5 *
	shl	bx, 1				; 10 *
	shl	bx, 1				; 20 *
	shl	bx, 1				; 40 *
	shl	bx, 1				; 80 * = y-byteoffset
	mov	cx, di
	rept	3
	shr	cx, 1				; x / 8 = x-byteoffset
	endm
	add	bx, cx				; total byteoffset
;
	mov	cx, di				; load x value
	and	cl, 07h				; x mod 8
	xor	cl, 07h				; shiftcount = 7 - x mod 8
	mov	ch, 1				; mask
	shl	ch, cl				; shift mask to position

	mov	dx, 03ceh
	mov	al, 8
	out	dx, al				; bit mask register
	inc	dx
	mov	al, ch
	out	dx, al				; store mask

	mov	al, es:[bx]
	mov	ax, [bp]+X+4			; load color
	mov	es:[bx], al			; write pixel

qsexit:	pop	es				; done!
	LEAVEF
	ENDFUNC	qsetpix

readbit	macro
	local	endmac
	mov	al, 4
	out	dx, al
	inc	dx
	mov	al, ah
	out	dx, al
	mov	al, es:[bx]
	shl	cl, 1
	and	al, ch
	jz	endmac
	or	cl, 1
endmac:
	endm

	BEGIN	qgetpix
;**
;	name		getpix
;
;	synopsis	color = qgetpix(x, y);
;			int x, y;	coordinate of the pixel
;			int color;	returned color of that pixel
;
;	description	This function reads ("gets") the color of a
;			specified pixel.
;**
	ENTERF
	push	es
	mov	ax, VID_BASE			; load base adr
	mov	es, ax
	mov	di, [bp]+X			; load x value
	mov	bx, [bp]+X+2			; load y value
	cmp	di, MAX_X			; x range ok?
	jl	nxt
	jmp	gexit
nxt:	cmp	bx, MAX_Y			; y range ok?
	jl	proces
	jmp	gexit
;
proces:	mov	cx, bx
	shl	bx, 1				;  2 *
	shl	bx, 1				;  4 *
	add	bx, cx				;  5 *
	shl	bx, 1				; 10 *
	shl	bx, 1				; 20 *
	shl	bx, 1				; 40 *
	shl	bx, 1				; 80 * = y-byteoffset
	mov	cx, di
	rept	3
	shr	cx, 1				; x / 8 = x-byteoffset
	endm
	add	bx, cx				; total byteoffset
;
	mov	cx, di				; load x value
	and	cl, 07h				; x mod 8
	xor	cl, 07h				; shiftcount = 7 - x mod 8
	mov	ch, 1				; mask
	shl	ch, cl				; shift mask to position

	mov	ah, 3
	mov	dx, 03ceh
;
	rept	4
	readbit
	dec	dx
	dec	ah
	endm
;
	and	cx, 000fh
	mov	ax, cx
gexit:	pop	es				; done, retval in ax!
	LEAVEF
	ENDFUNC	qgetpix

	BEGIN	setbyte
;**
;	name		setbyte
;
;	synopsis	setbyte(x, y, color, pattern);
;			int x;		x-coordinate of the byte, only
;					valid if multiple of 8
;			int y;		y-coordinate of the byte, may have
;					any y-value
;			int color;	pixel-color
;			int gr_byte;	graphics pattern to set
;
;	description	This function puts the graphics pattern contained
;			in gr_byte into the adapter memory. Gr_byte must
;			be a multiple of 8, all other values are invalid
;			and will be rounded. Gr_byte is defined as con-
;			secutive 8 pixles on the x-axis, which do all have
;			the SAME color.
;
;	warning		This function is only available with the ega driver. 
;			There is a nearly identic function in the hercules
;			driver, but It schould be treated as available 
;			internal to the grphics library only. If you have to 
;			use it, use conditional compilation, too. This 
;			function is only available when 
;			#ifdef EGAGRAF == true!
;**
	ENTERF
	push	es
	mov	ax, VID_BASE			; load base adr
	mov	es, ax
	mov	di, [bp]+X			; load x value
	mov	bx, [bp]+X+2			; load y value
	cmp	di, MAX_X			; x range ok?
	jae	sbexit
	cmp	bx, MAX_Y			; y range ok?
	jae	sbexit
;
	mov	cx, bx
	shl	bx, 1				;  2 *
	shl	bx, 1				;  4 *
	add	bx, cx				;  5 *
	shl	bx, 1				; 10 *
	shl	bx, 1				; 20 *
	shl	bx, 1				; 40 *
	shl	bx, 1				; 80 * = y-byteoffset
	mov	cx, di
	rept	3
	shr	cx, 1				; x / 8 = x-byteoffset
	endm
	add	bx, cx				; total byteoffset
;
	mov	dx, 03ceh
	mov	al, 8
	out	dx, al				; bit mask register
	inc	dx
	mov	al, [bp]+X+6			; bit pattern
	out	dx, al				; store mask

	mov	al, es:[bx]
	mov	ax, [bp]+X+4			; load color
	mov	es: [bx], al			; write pixel

sbexit:	pop	es				; done!
	LEAVEF
	ENDFUNC	setbyte
	ENDPS
	end
