;	GETSYS - STAND-ALONE CP/M SYSTEM EXTRACT PROGRAM
;	for standard CP/M format (8"1S) only

ESC	equ	0EDH
APE	equ	0EDH

SETDMA	equ	12
READ	equ	13
WRITE	equ	14

	org	0000H
ENTRY:
	lxi	sp, STACK + 128
	lxi	b, BUFFER
	db	ESC, APE, SETDMA

LOOP:
	lda	SDSK
	sta	XDSK
	lda	STRK
	sta	XTRK
	lda	SSEC
	mov	c, a
	mvi	b, 0
	lxi	h, XLAT
	dad	b
	mov	a, m
	sta	XSEC
	lxi	d, XDP
	db	ESC, APE, READ
	lxi	d, DDP
	mvi	c, 0
	db	ESC, APE, WRITE

	lxi	h, SSEC
	call	NEXTPOS
	lxi	h, DSEC
	call	NEXTPOS
	lxi	h, COUNT
	dcr	m
	jnz	LOOP

	hlt

NEXTPOS:
	inr	m
	mov	a, m
	cpi	26
	rnz
	mvi	m, 0
	dcx	h
	inr	m
	ret

XLAT:				; anti-skew table
	db	 0, 13,	 9, 22,	 5, 18
	db	 1, 14, 10, 23,	 6, 19
	db	 2, 15, 11, 24,	 7, 20
	db	 3, 16, 12, 25,	 8, 21
	db	 4, 17

COUNT:	db	44

SDP:
SDSK:	db	0
STRK:	db	0
SSEC:	db	1

DDP:
DDSK:	db	1
DTRK:	db	2
DSEC:	db	17

XDP:
XDSK:	ds	1
XTRK:	ds	1
XSEC:	ds	1

BUFFER: ds	128
STACK:	ds	128

	end	ENTRY
