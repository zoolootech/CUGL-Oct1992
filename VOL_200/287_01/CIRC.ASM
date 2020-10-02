;
oa	equ	4

DGROUP	group	_DATA
_DATA	segment word public 'DATA'
	assume	ds:DGROUP

	extrn	_ARCSPEC:word,_DOTVALUE:word
	extrn	_LASTX:word, _LASTY:word, _ARCSTTX:word, _ARCSTTY:word
	extrn	_ARCENDX:word, _ARCENDY:word
	extrn	wrtvec:word
	extrn	_WINX1:word,_WINX2:word,_WINY1:word,_WINY2:word

	public	_cirsty
cirx	dw	?
ciry	dw	?
cird	dw	?
incr1	dw	?
incr2	dw	?
_cirsty dw	?
cirptr	dw	?
cirptra dw	?
oldx	dw	?
oldy	dw	?
_DATA	ends

_TEXT	segment byte public 'CODE'
	assume	cs:_TEXT,ds:DGROUP
	extrn	$calc:near

	public	_circ,_setcptr
	public	_seteptr,_wrtept,_wrtepta

_setcptr proc	 near	 ; public to c
	mov	bx,sp
	shr	word ptr SS:[bx+oa-2],1
	jnc	short scp1
	mov	cirptr,offset arc1pt
	mov	cirptra,offset arc1pt
	ret
scp1:	mov	cirptr,offset cir_pt
	mov	cirptra,offset arc1pt
	ret
_setcptr endp

_seteptr proc	 near	 ; public to c
	mov	bx,sp
	shr	word ptr SS:[bx+oa-2],1
	jnc	short sep1
	mov	cirptr,offset eac1pt
	mov	cirptra,offset eac1pt
	ret
sep1:	mov	cirptr,offset ell_pt
	mov	cirptra,offset eac1pt
	ret
_seteptr endp

_wrtept proc	near	; public to c
	jmp	cirptr
_wrtept endp

_wrtepta proc	 near	 ; public to c
	jmp	cirptra
_wrtepta endp

$lcp	proc	near
	add	ax,[bp+oa+4]
	add	bx,[bp+oa+6]
	mov	di,ax
	and	di,0fh
	call	$calc
	mov	es,ax
	shl	di,1
	mov	ax,_DOTVALUE[di]
	jmp	wrtvec
$lcp	endp

$lap	proc	near
	add	ax,[bp+oa+4]
	add	bx,[bp+oa+6]
	cmp	ax,_WINX1
	jb	short leave
	cmp	ax,_WINX2
	ja	short leave
	cmp	bx,_WINY1
	jb	short leave
	cmp	bx,_WINY2
	ja	short leave
	mov	di,ax
	and	di,0fh
	call	$calc
	mov	es,ax
	shl	di,1
	mov	ax,_DOTVALUE[di]
	jmp	wrtvec
leave:	ret
$lap	endp

cir_pt	proc	near	; public to c
	push	bp
	mov	bp,sp
	push	si
	push	di
	mov	ax,[bp+oa+2]
	mov	bx,[bp+oa]
	call	$lcp
	mov	ax,[bp+oa]
	mov	bx,[bp+oa+2]
	neg	bx
	call	$lcp
	mov	ax,[bp+oa]
	neg	ax
	mov	bx,[bp+oa+2]
	call	$lcp
	mov	ax,[bp+oa+2]
	neg	ax
	mov	bx,[bp+oa]
	neg	bx
	call	$lcp
	mov	ax,[bp+oa]
	mov	bx,[bp+oa+2]
	call	$lcp
	mov	ax,[bp+oa+2]
	mov	bx,[bp+oa]
	neg	bx
	call	$lcp
	mov	ax,[bp+oa]
	neg	ax
	mov	bx,[bp+oa+2]
	neg	bx
	call	$lcp
	mov	ax,[bp+oa+2]
	neg	ax
	mov	bx,[bp+oa]
	call	$lcp
	pop	di
	pop	si
	pop	bp
	ret
cir_pt	endp

arc1pt	proc	near	    ; public to c
	push	bp
	mov	bp,sp
	push	si
	push	di
	test	byte ptr _ARCSPEC,04h
	jz	short a1pt1
	mov	ax,[bp+oa+2]
	mov	bx,[bp+oa]
	call	$lap
a1pt1:	test	byte ptr _ARCSPEC,01h
	jz	short a1pt2
	mov	ax,[bp+oa]
	mov	bx,[bp+oa+2]
	neg	bx
	call	$lap
a1pt2:	test	byte ptr _ARCSPEC,10h
	jz	short a1pt3
	mov	ax,[bp+oa]
	neg	ax
	mov	bx,[bp+oa+2]
	call	$lap
a1pt3:	test	byte ptr _ARCSPEC,40h
	jz	short arc1p1
	mov	ax,[bp+oa+2]
	neg	ax
	mov	bx,[bp+oa]
	neg	bx
	call	$lap
arc1p1: test	byte ptr _ARCSPEC,08h
	jz	short a1pt4
	mov	ax,[bp+oa]
	mov	bx,[bp+oa+2]
	call	$lap
a1pt4:	test	byte ptr _ARCSPEC,02h
	jz	short a1pt5
	mov	ax,[bp+oa+2]
	mov	bx,[bp+oa]
	neg	bx
	call	$lap
a1pt5:	test	byte ptr _ARCSPEC,80h
	jz	short a1pt6
	mov	ax,[bp+oa]
	neg	ax
	mov	bx,[bp+oa+2]
	neg	bx
	call	$lap
a1pt6:	test	byte ptr _ARCSPEC,20h
	jz	short a1pt7
	mov	ax,[bp+oa+2]
	neg	ax
	mov	bx,[bp+oa]
	call	$lap
a1pt7:	pop	di
	pop	si
	pop	bp
	ret
arc1pt	endp

;	circ(centerx,centery,radius,style,pnt1,pnt2)
;	int	cneterx,centery,radius,style;
;	struct	cpoint *pnt1, *pnt2;
;
;	draw a circle or arc with center at (centerx,centery)
_circ	proc	near	; public to c
	push	bp
	mov	bp,sp
	mov	cx,[bp+oa+6]
	mov	_cirsty,cx
	mov	cx,0
	mov	cirx,cx
	mov	ax,[bp+oa+4]
	mov	ciry,ax
	push	[bp+oa+2]
	push	[bp+oa]
	push	ax
	push	cx
	shl	ax,1
	mov	bx,3
	sub	bx,ax
	mov	cird,bx
	mov	incr1,6
	shl	ax,1
	mov	bx,10
	sub	bx,ax
	mov	incr2,bx
	rol	_cirsty,1
	jnc	short cir1
	mov	cx,_ARCSPEC
	mov	[bp+oa],cx
	and	_ARCSPEC,055h
	call	cirptra
	mov	cx,[bp+oa]
	mov	_ARCSPEC,cx
	jmp	short cir1
cir4:	rol	_cirsty,1
	jnc	short cir1
	call	cirptr
cir1:	add	sp,4
	mov	ax,cirx
	mov	oldx,ax
	mov	ax,ciry
	mov	oldy,ax
	mov	ax,cird
	test	ax,ax
	jns	cir2
	add	ax,incr1
	add	incr2,4
	jmp	short cir3
cir2:	add	ax,incr2
	add	incr2,8
	dec	ciry
cir3:	add	incr1,4
	mov	cird,ax
	inc	cirx
;
	mov	ax,0ffffh
	mov	bx,[bp+oa+8]
	test	ax,[bx]
	jnz	acir1
	mov	cx,cirx
	mov	_ARCSTTX,cx
	mov	cx,ciry
	mov	_ARCSTTY,cx
	mov	cx,_ARCSPEC
	or	cx,[bx+4]
	mov	_ARCSPEC,cx
	test	ax,[bx+2]
	jz	acir2
	xor	cx,[bx+4]
	mov	_ARCSPEC,cx
	mov	cx,oldx
	mov	_ARCSTTX,cx
	mov	cx,oldy
	mov	_ARCSTTY,cx
acir2:
acir1:	dec	word ptr [bx]
	mov	bx,[bp+oa+10]
	test	ax,[bx]
	jnz	acir3
	mov	cx,cirx
	mov	_ARCENDX,cx
	mov	cx,ciry
	mov	_ARCENDY,cx
	mov	cx,_ARCSPEC
	or	cx,[bx+4]
	mov	_ARCSPEC,cx
	test	ax,[bx+2]
	jz	acir4
	xor	cx,[bx+4]
	mov	_ARCSPEC,cx
	mov	cx,oldx
	mov	_ARCENDX,cx
	mov	cx,oldy
	mov	_ARCENDY,cx
acir4:
acir3:	dec	word ptr [bx]
	mov	ax,cirx
	mov	bx,ciry
	push	bx
	push	ax
	cmp	ax,bx
	jb	tocir4
	ja	cir5
	mov	oldx,ax
	mov	oldy,bx
	rol	_cirsty,1
	jnc	short cir5
	mov	cx,_ARCSPEC
	mov	[bp+oa],cx
	and	_ARCSPEC,0aah
	call	cirptra
	mov	cx,[bp+oa]
	mov	_ARCSPEC,cx
cir5:	mov	ax,oldx
	mov	_LASTX,ax
	mov	ax,oldy
	mov	_LASTY,ax
	mov	sp,bp
	pop	bp
	ret
tocir4: jmp	cir4
_circ	endp

ell_pt	proc	near	; public to c
	push	bp
	mov	bp,sp
	push	si
	push	di
	mov	ax,[bp+oa]
	mov	bx,[bp+oa+2]
	neg	bx
	call	$lcp
	mov	ax,[bp+oa]
	neg	ax
	mov	bx,[bp+oa+2]
	call	$lcp
	mov	ax,[bp+oa]
	mov	bx,[bp+oa+2]
	call	$lcp
	mov	ax,[bp+oa]
	neg	ax
	mov	bx,[bp+oa+2]
	neg	bx
	call	$lcp
	pop	di
	pop	si
	pop	bp
	ret
ell_pt	endp

eac1pt	proc	near	; public to c
	push	bp
	mov	bp,sp
	push	si
	push	di
	rol	_cirsty,1
	jnc	ea1a2
	test	_ARCSPEC,01h
	jz	short eac11
	mov	ax,[bp+oa]
	mov	bx,[bp+oa+2]
	neg	bx
	call	$lap
eac11:	test	_ARCSPEC,10h
	jz	short ea1p1
	mov	ax,[bp+oa]
	neg	ax
	mov	bx,[bp+oa+2]
	call	$lap
ea1p1:	test	_ARCSPEC,08h
	jz	short ea1a1
	mov	ax,[bp+oa]
	mov	bx,[bp+oa+2]
	call	$lap
ea1a1:	test	_ARCSPEC,80h
	jz	short ea1a2
	mov	ax,[bp+oa]
	neg	ax
	mov	bx,[bp+oa+2]
	neg	bx
	call	$lap
ea1a2:	pop	di
	pop	si
	pop	bp
	ret
eac1pt	endp

_TEXT	ends
	end
