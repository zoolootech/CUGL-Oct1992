;	copyall - stand alone disk copy program
;	for standard CP/M format (8"1S) only
;	note that this program does not use SECTRAN function

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

	mvi	a, 2
	sta	TRACK
TRKLOOP:

	mvi	a, 0
	sta	SECTER
SECLOOP:
	mvi	a, 0
	sta	DISK
	mvi	c, 0
	lxi	d, DPARAM
	db	ESC, APE, READ
	mvi	a, 1
	sta	DISK
	mvi	c, 0
	lxi	d, DPARAM
	db	ESC, APE, WRITE

	lda	SECTER
	inr	a
	sta	SECTER
	cpi	26
	jnz	SECLOOP

	lda	TRACK
	inr	a
	sta	TRACK
	cpi	77
	jnz	TRKLOOP

	hlt

;

DPARAM:
DISK:	ds	1
TRACK:	ds	1
SECTER: ds	1

BUFFER: ds	128
STACK:	ds	128

	end	ENTRY
