;	Fake bios for Portable CP/M System

SYSSIZE equ	20			; system size in KB
NDISKS	equ	8			; max. drive

;   *** Definitions

ESC	equ	0EDEDH			; special ESCAPE instruction
Icboot	equ	0
Iwboot	equ	1
Iconst	equ	2
Iconin	equ	3
Iconot	equ	4
Ilstot	equ	5
Iauxot	equ	6
Iauxin	equ	7
Idsdma	equ	12
Idread	equ	13
Idwrit	equ	14
Ilstst	equ	15

BIAS	equ	( SYSSIZE - 20 ) * 1024
BOOT	equ	00000H
STACK	equ	00100H
CCP	equ	03400H + BIAS
BDOS	equ	03C06H + BIAS
BIOS	equ	04A00H + BIAS

cdisk	equ	00004H			; current disk and user
iobyte	equ	00005H			; Intel I/O byte

;   *** Boot code

	org	BOOT
	jmp	BIOS

;   *** Bios body

	org	BIOS

JVECT:	jmp	CBOOT
	jmp	WBOOT
	jmp	CONST
	jmp	CONIN
	jmp	CONOT
	jmp	LSTOT
	jmp	AUXOT
	jmp	AUXIN
	jmp	DHOME
	jmp	DSDSK
	jmp	DSTRK
	jmp	DSSEC
	jmp	DSDMA
	jmp	DREAD
	jmp	DWRIT
	jmp	LSTST
	jmp	STRAN

CBOOT:	lxi	sp, STACK
	dw	ESC
	db	Icboot
	ora	a
	jnz	halt
	mvi	a, 0
	sta	cdisk
	mvi	a, 0
	sta	iobyte
	jmp	gocpm

WBOOT:	lxi	sp, STACK
	dw	ESC
	db	Iwboot
	ora	a
	jz	gocpm
halt:	hlt				; stop machine and exit

gocpm:	mvi	a, 0C3H
	sta	00000H
	sta	00005H
	lxi	h, JVECT + 3
	shld	00001H
	lxi	h, BDOS
	shld	00006H
	lxi	b, 00080H
	call	DSDMA
	lda	cdisk
	mov	c, a
	jmp	CCP

CONST:	dw	ESC
	db	Iconst
	ret

CONIN:	dw	ESC
	db	Iconin
	ret

CONOT:	dw	ESC
	db	Iconot
	ret

LSTOT:	dw	ESC
	db	Ilstot
	ret

AUXOT:	dw	ESC
	db	Iauxot
	ret

AUXIN:	dw	ESC
	db	Iauxin
	ret

DSDSK:	lxi	h, 0
	mov	a, c
	sta	dskno
	cpi	NDISKS
	rnc
	mov	l, c
	mvi	h, 0
	dad	h
	dad	h
	dad	h
	dad	h
	lxi	d, DPH
	dad	d
	ret

DHOME:	mvi	c, 0

DSTRK:	mov	a, c
	sta	trkno
	ret

DSSEC:	mov	a, c
	sta	secno
	ret

DSDMA:	dw	ESC
	db	Idsdma
	ret

DREAD:	lxi	d, dparam
	dw	ESC
	db	Idread
	ret

DWRIT:	lxi	d, dparam
	dw	ESC
	db	Idwrit
	ret

LSTST:	dw	ESC
	db	Ilstst
	ret

STRAN:	mov	h, b
	mov	l, c
	ret

;   *** Tables

DPH:
DPH0:	dw	00000H
	dw	0,0,0
	dw	DIRBUF
	dw	DPB
	dw	CSV0
	dw	ALV0
DPH1:	dw	00000H
	dw	0,0,0
	dw	DIRBUF
	dw	DPB
	dw	CSV1
	dw	ALV1
DPH2:	dw	00000H
	dw	0,0,0
	dw	DIRBUF
	dw	DPB
	dw	CSV2
	dw	ALV2
DPH3:	dw	00000H
	dw	0,0,0
	dw	DIRBUF
	dw	DPB
	dw	CSV3
	dw	ALV3
DPH4:	dw	00000H
	dw	0,0,0
	dw	DIRBUF
	dw	DPB
	dw	CSV4
	dw	ALV4
DPH5:	dw	00000H
	dw	0,0,0
	dw	DIRBUF
	dw	DPB
	dw	CSV5
	dw	ALV5
DPH6:	dw	00000H
	dw	0,0,0
	dw	DIRBUF
	dw	DPB
	dw	CSV6
	dw	ALV6
DPH7:	dw	00000H
	dw	0,0,0
	dw	DIRBUF
	dw	DPB
	dw	CSV7
	dw	ALV7

DPB:	; Compatible with 8'1S standard format ( without skew )
	dw	26			; SPT
	db	3			; BSH
	db	7			; BLM
	db	0			; EXM
	dw	242			; DSM
	dw	63			; DRM
	db	11000000B		; AL0
	db	00000000B		; AL1
	dw	16			; CKS
	dw	2			; OFF

;   *** Variables

dparam:
dskno:	ds	1
trkno:	ds	1
secno:	ds	1

;   *** BDOS work area

	org	( $ + 1 ) and 0FFFEH	; make it even
DIRBUF: ds	128
CSV0:	ds	16
CSV1:	ds	16
CSV2:	ds	16
CSV3:	ds	16
CSV4:	ds	16
CSV5:	ds	16
CSV6:	ds	16
CSV7:	ds	16
ALV0:	ds	31
ALV1:	ds	31
ALV2:	ds	31
ALV3:	ds	31
ALV4:	ds	31
ALV5:	ds	31
ALV6:	ds	31
ALV7:	ds	31

ENDBIOS equ	$			; last address

	end
