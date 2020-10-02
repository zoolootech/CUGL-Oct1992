;	Screen output routines for RED text editor.
;
;	Assemble this file with Microsoft MASM.
;
;	Source:	 redouta.c
;	Started: 12/21/89
;	Version: 1/18/90
;

;
; External variables
;
	EXTRN	_outx:word
	EXTRN	_outy:word

_TEXT	segment	byte public 'CODE'

DGROUP	group	_DATA
	assume ds:DGROUP
_TEXT	ends

_DATA	segment word public 'DATA'
d@	label	byte
d@w	label	word
_DATA	ends

;
; Character constants
;
space		equ 20h		;ascii space
normal		equ 07h		;normal video attribute

;
; Screen constants
;
maxcol		equ 80		;columns per line on CRT
maxcol1		equ 79		;maxcol - 1
maxrow		equ 25		;rows on CRT
maxrow1		equ 24		;maxrow - 1

_TEXT	SEGMENT	BYTE PUBLIC 'CODE'
		ASSUME cs:_TEXT

;
;	Output one printable character to the screen.
;
		PUBLIC _outchar

IFDEF NEAR
_outchar	proc near
ELSE
_outchar	proc far
ENDIF

	push	bp
	mov	bp,sp
	push	di
	push	si

	mov	ah,14		;write character as TTY

IFDEF NEAR
	mov	al,[bp+4]
ELSE
	mov	al,[bp+6]
ENDIF

	int	10h

	mov	al,byte ptr DGROUP:_outx	;increment outx
	add	al,1
	mov	byte ptr DGROUP:_outx,al

	pop	si
	pop	di
	pop	bp
	ret

_outchar	ENDP

;
;	Clear the screen.
;
		PUBLIC _outclr

IFDEF NEAR
_outclr		proc near
ELSE
_outclr		proc far
ENDIF

	push	di
	push	si

	mov	ah,15		;get video mode
	int	10h

	cmp	al,3		;already mode 3?
	je	outclr1		;yes -->

outclr1:
	mov	ah,0		;set video mode (Bios also clears the screen)
	mov	al,3		;mode 3 (High res, 80x25, CGA)
	int	10h
	jmp	outclr2		;-->

;
; Already in mode 3.  Clear the screen by scrolling it.
;
	
	mov	ah,6		;scroll window up
	mov	al,maxrow	;line count
	mov	bh,normal	;display attribute, not page
	mov	ch,0		;top row
	mov	cl,0		;left column
	mov	dh,maxrow1	;bottom row
	mov	dl,maxcol1	;right column
	int	10h

outclr2:
	mov	ah,5		;set display page
	mov	al,0		;set page 0
	int	10h

	pop	si
	pop	di
	ret

_outclr		ENDP

;
;	Delete the line on which the cursor rests.
;	Leave the cursor at the left margin.
;
		PUBLIC _outdelln

IFDEF NEAR
_outdelln	proc near
ELSE
_outdelln	proc far
ENDIF

	push	di
	push	si

	mov	ah,3		;read cursor position
	mov	bh,0		;page number
	int	10h		;row number is in dh for set cursor position

	mov	ah,2		;set cursor position
	mov	bh,0		;page number
	mov	dl,0		;set x = column number
	int	10h

	mov	ah,9		;write character and attribute
	mov	bh,0		;page number
	mov	al,space	;ascii space
	mov	bl,normal	;attribute
	mov	cx,maxcol1	;repeat count
	int 	10h

	pop	si
	pop	di
	ret

_outdelln	ENDP

;
;	Delete to end of line.
;	Assume the last column is blank.
;
		PUBLIC _outdeol

IFDEF NEAR
_outdeol	proc near
ELSE
_outdeol	proc far
ENDIF

	push	di
	push	si

	mov	ah,3		;read cursor position
	mov	bh,0		;page number
	int	10h		;put column number in dl

	mov	ah,9		;write character and attribute
	mov	bh,0		;page number
	mov	al,space	;ascii space
	mov	bl,normal	;attribute
	xor	cx,cx		;repeat count in cx
	mov	cl,maxcol
	sub	cl,dl
	int 	10h

	pop	si
	pop	di
	ret

_outdeol	ENDP

;
;	Hardware delete line.
;
		PUBLIC _outdel

IFDEF NEAR
_outdel		proc near
ELSE
_outdel		proc far
ENDIF

	push	di
	push	si

	mov	ah,3		;read cursor position
	mov	bh,0		;page number
	int	10h		;put row number in dh

	mov	ah,6		;scroll window up
	mov	al,1		;line count
	mov	bh,normal	;display attribute, not page
	mov	ch,dh		;top row
	mov	cl,0		;left column
	mov	dh,maxrow1	;bottom row
	mov	dl,maxcol1	;right column
	int	10h
	
	pop	si
	pop	di
	ret

_outdel		ENDP

;
;	Hardware insert line.  Leave cursor alone.
;
		PUBLIC _outins

IFDEF NEAR
_outins		proc near
ELSE
_outins		proc far
ENDIF

	push	di
	push	si

	mov	ah,3		;read cursor position
	mov	bh,0		;page number
	int	10h		;put row number in dh

	mov	ah,7		;scroll window down
	mov	al,1		;line count
	mov	bh,normal	;display attribute, not page
	mov	ch,dh		;top row
	mov	cl,0		;left column
	mov	dh,maxrow1	;bottom row
	mov	dl,maxcol1	;right column
	int	10h

	pop	si
	pop	di
	ret

_outins		ENDP

;
; Scrolling routines are not used
;
		PUBLIC _outsdn

IFDEF NEAR
_outsdn		proc near
ELSE
_outsdn		proc far
ENDIF
		ret		;not used

_outsdn		ENDP

		PUBLIC _outsup

IFDEF NEAR
_outsup		proc near
ELSE
_outsup		proc far
ENDIF

		ret		;not used

_outsup		ENDP

;
;	Put cursor at row,column.   0,0 is top of screen.
;
		PUBLIC _outxy

IFDEF NEAR
_outxy		proc near
ELSE
_outxy		proc far
ENDIF

	push	bp
	mov	bp,sp
	push	di
	push	si

	mov	ah,2		;set cursor position
	mov	bh,0		;page number

IFDEF NEAR
	mov	dl,[bp+4]			;set x = column number
	mov	byte ptr DGROUP:_outx,dl	;set global outx
	mov	dh,[bp+6]			;set y = row number
ELSE
	mov	dl,[bp+6]			;set x = column number
	mov	byte ptr DGROUP:_outx,dl	;set global outx
	mov	dh,[bp+8]			;set y = row number
ENDIF

	mov	byte ptr DGROUP:_outy,dh	;set global outy
	int	10h

	pop	si
	pop	di
	pop	bp
	ret

_outxy		ENDP

_TEXT	ENDS

		END
