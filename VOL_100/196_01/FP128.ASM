
TPALOC		EQU	0100H




	page 76

CPM:	EQU 1
MPM2:	EQU 0

	if not cpm
CCCORG:	EQU WHATEVER
RAM:	EQU WHATEVER2
BASE:	EQU WHATEVER3
	endif


	if cpm
base:	equ 0000h
fcb:	equ base+5ch
tbuff:	equ base+80h
bdos:	equ base+5
tpa:	equ base+100h
nfcbs:	equ 9
errorv:	equ 255
cccorg:	equ tpa


ram:	equ cccorg+52Bh
	endif


cr:	equ 0dh
lf:	equ 0ah
newlin:	equ lf
tab:	equ 9
bs:	equ 08h
cntrlc:	equ 3



fexitv:	equ cccorg+09h
error:	equ cccorg+1dh
exit:	equ error+3

	if cpm
close:	equ error+6
setfcb:	equ error+9
fgfd:	equ error+12
fgfcb:	equ error+15
setfcu:	equ error+18
setusr:	equ error+21
rstusr:	equ error+24
khack:	equ error+33
clrex:	equ error+36
	endif


eqwel:	equ cccorg+0e5h

smod:	equ cccorg+10fh
usmod:	equ cccorg+129h
smul:	equ cccorg+13fh
usmul:	equ cccorg+16bh
usdiv:	equ cccorg+189h
sdiv:	equ cccorg+1cbh

cmphd:	equ cccorg+1ddh
cmh:	equ cccorg+1fah
cmd:	equ cccorg+202h

ma1toh:	equ cccorg+20ah
ma2toh:	equ cccorg+213h
ma3toh:	equ ma2toh+6
ma4toh:	equ ma2toh+12
ma5toh:	equ ma2toh+18
ma6toh:	equ ma2toh+24
ma7toh:	equ ma2toh+30

arghak:	equ ma2toh+36



	org ram

	ds 20

errnum:	ds 1

pbase:	ds 2
ysize:	ds 2
xsize:	ds 2
psize:	ds 2

rseed:	ds 8

args:	ds 14

iohack:	ds 6

allocp: ds 2
alocmx:	ds 2

room:	ds 30

uroom:	ds 20



extrns:	equ cccorg+15h
cccsiz:	equ cccorg+17h
codend:	equ cccorg+19h
freram:	equ cccorg+1bh


arg1:	equ args
arg2:	equ args+2
arg3:	equ args+4
arg4:	equ args+6
arg5:	equ args+8
arg6:	equ args+10
arg7:	equ args+12

tmp:	equ room
tmp1:	equ room+1
tmp2:	equ room+2
tmp2a:	equ room+4
ungetl:	equ room+6
unused:	equ room+7
curusr:	equ room+8
usrnum:	equ room+9



mode:	equ room+10
freeze:	equ room+11
pending: equ room+12
pendch:	equ room+13
quitc:	equ room+14

echo:	equ 1
quit:	equ 2
flow:	equ 4
strip:	equ 8
expand:	equ 16
swap:	equ 32



	if cpm
conin:	equ 1
conout:	equ 2
lstout:	equ 5
dconio:	equ 6
pstrng:	equ 9
getlin:	equ 10
cstat:	equ 11
select:	equ 14
openc:	equ 15
closec:	equ 16
delc:	equ 19
reads:	equ 20
creatc:	equ 22
renc:	equ 23
sdma:	equ 26
gsuser:	equ 32
readr:	equ 33
writr:	equ 34
cfsizc:	equ 35
srrecc:	equ 36
	endif


BIASEXP	EQU	0400H
NBYTES	EQU	16



		ORG	TPALOC+200H
		DB	0,0,0,0,0
FP128$BEG	EQU	$-TPALOC
		DB	0
		DW	FP128$END-$-2
FP128$STRT	EQU	$
FP128$EF$FP128	EQU	FP128$STRT

FP128$STRTC	EQU	$
	call	arghak
	push	b
	lda	arg1
	ora	a
FP128$R000	EQU	$+1-FP128$STRT
	jz	FP128$L$FPTEST
	cpi	11
FP128$R001	EQU	$+1-FP128$STRT
	jz	FP128$L$FPIN
	cpi	255
FP128$R002	EQU	$+1-FP128$STRT
	jz	FP128$L$FPTST2
	lhld	arg2
	xchg
FP128$R003	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$AREG
FP128$R004	EQU	$+1-FP128$STRT
	call	FP128$L$UNPACK
	lda	arg1
	cpi	10
FP128$R005	EQU	$+1-FP128$STRT
	jz	FP128$L$FPCONV
	lhld	arg3
	xchg
FP128$R006	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$BREG
FP128$R007	EQU	$+1-FP128$STRT
	call	FP128$L$UNPACK
FP128$R008	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$EXITP
	push	h
	lda	arg1
	cpi	1
FP128$R009	EQU	$+1-FP128$STRT
	jz	FP128$L$FPMUL0
	cpi	2
FP128$R010	EQU	$+1-FP128$STRT
	jz	FP128$L$FPDIV0
	cpi	3
FP128$R011	EQU	$+1-FP128$STRT
	jz	FP128$L$FPADD0
	cpi	4
FP128$R012	EQU	$+1-FP128$STRT
	jz	FP128$L$FPSUB0
	pop	h
	pop	b
	lxi	h,0
	ret

FP128$L$EXITP		EQU	$-FP128$STRT
      	lhld	arg4
	xchg
FP128$R013	EQU	$+1-FP128$STRT
	call	FP128$L$PACK
FP128$R014	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$OVF
	xra	a
	ora	m
	inx	h
	ora	m
	inx	h
	ora	m
	inx	h
	ora	m
	mov	l,a
	mvi	h,0
	pop	b
	ret



FP128$L$FPDIV0		EQU	$-FP128$STRT
       	lxi	h,0
FP128$R015	EQU	$+1-FP128$STRT
	shld	FP128$L$OVF
FP128$R016	EQU	$+1-FP128$STRT
	shld	FP128$L$ZERO
FP128$R017	EQU	$+1-FP128$STRT
	lhld	FP128$L$BEXP
	mov	a,h
	ora	l
FP128$R018	EQU	$+1-FP128$STRT
	jz	FP128$L$OVRFW
FP128$R019	EQU	$+1-FP128$STRT
	lhld	FP128$L$AEXP
	mov	a,h
	ora	l
FP128$R020	EQU	$+1-FP128$STRT
	jz	FP128$L$SETZERO

FP128$L$FDIV1		EQU	$-FP128$STRT
      	lxi	h,0
FP128$R021	EQU	$+1-FP128$STRT
	shld	FP128$L$LA
FP128$R022	EQU	$+1-FP128$STRT
	shld	FP128$L$LA+2
FP128$R023	EQU	$+1-FP128$STRT
	shld	FP128$L$LA+4
FP128$R024	EQU	$+1-FP128$STRT
	shld	FP128$L$LA+6
FP128$R025	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$LA+NBYTES+NBYTES-1
	mvi	b,NBYTES+1
	xra	a
FP128$R026	EQU	$+1-FP128$STRT
	call	FP128$L$SFTR0
FP128$R027	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$BREG+NBYTES-1
	xra	a
FP128$R028	EQU	$+1-FP128$STRT
	call	FP128$L$SFTR
FP128$R029	EQU	$+1-FP128$STRT
	lhld	FP128$L$BEXP
	inx	h
FP128$R030	EQU	$+1-FP128$STRT
	shld	FP128$L$BEXP
	mvi	b,NBYTES*8
FP128$L$FDIV2		EQU	$-FP128$STRT
      	push	b
FP128$R031	EQU	$+1-FP128$STRT
	lxi	d,FP128$L$AREG+NBYTES-1
FP128$R032	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$BREG+NBYTES-1
FP128$R033	EQU	$+1-FP128$STRT
	call	FP128$L$ICMP
FP128$R034	EQU	$+1-FP128$STRT
	jc	FP128$L$FDIV3
FP128$R035	EQU	$+1-FP128$STRT
	lxi	d,FP128$L$AREG
FP128$R036	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$BREG
FP128$R037	EQU	$+1-FP128$STRT
	call	FP128$L$ISUB
	xra	a
FP128$L$FDIV3		EQU	$-FP128$STRT
      	cmc
FP128$R038	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$LA
FP128$R039	EQU	$+1-FP128$STRT
	call	FP128$L$SFTL
FP128$R040	EQU	$+1-FP128$STRT
	call	FP128$L$SFTL
	pop	b

	db	010h,0dch

FP128$R041	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$LA
FP128$R042	EQU	$+1-FP128$STRT
	lxi	d,FP128$L$AREG
	lxi	b,NBYTES
	ldir
FP128$R043	EQU	$+1-FP128$STRT
	lhld	FP128$L$AEXP
	lxi	d,BIASEXP+2
	dad	d
	xchg
FP128$R044	EQU	$+1-FP128$STRT
	lhld	FP128$L$BEXP
	xchg
FP128$R045	EQU	$+1-FP128$STRT
	jmp	FP128$L$EXPNRM




FP128$L$FPMUL0		EQU	$-FP128$STRT
       	lxi	h,0
FP128$R046	EQU	$+1-FP128$STRT
	shld	FP128$L$OVF
FP128$R047	EQU	$+1-FP128$STRT
	shld	FP128$L$ZERO
FP128$R048	EQU	$+1-FP128$STRT
	lhld	FP128$L$BEXP
	mov	a,h
	ora	l
FP128$R049	EQU	$+1-FP128$STRT
	jz	FP128$L$SETZERO
FP128$R050	EQU	$+1-FP128$STRT
	lhld	FP128$L$AEXP
	mov	a,h
	ora	l
FP128$R051	EQU	$+1-FP128$STRT
	jz	FP128$L$SETZERO

FP128$L$FMUL3		EQU	$-FP128$STRT
FP128$R052	EQU	$+1-FP128$STRT
      	lxi	h,FP128$L$AREG
FP128$R053	EQU	$+1-FP128$STRT
	lxi	d,FP128$L$LA
	lxi	b,nbytes
	ldir

FP128$R054	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$BREG
FP128$R055	EQU	$+1-FP128$STRT
	call	FP128$L$IMUL

FP128$R056	EQU	$+1-FP128$STRT
	lhld	FP128$L$AEXP
	xchg
FP128$R057	EQU	$+1-FP128$STRT
	lhld	FP128$L$BEXP
	dad	d
	lxi	d,BIASEXP

FP128$L$EXPNRM		EQU	$-FP128$STRT
       	ora	a
	dsbc	d
FP128$R058	EQU	$+1-FP128$STRT
	shld	FP128$L$AEXP
FP128$R059	EQU	$+1-FP128$STRT
	jc	FP128$L$UNDRFW
	mov	a,h
	cpi	BIASEXP/128
FP128$R060	EQU	$+1-FP128$STRT
	jnc	FP128$L$OVRFW
FP128$R061	EQU	$+1-FP128$STRT
	lda	FP128$L$ASIGN
FP128$R062	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$BSIGN
	xra	m
FP128$R063	EQU	$+1-FP128$STRT
	sta	FP128$L$ASIGN
FP128$R064	EQU	$+1-FP128$STRT
	jmp	FP128$L$FPNORM



FP128$L$FPSUB0		EQU	$-FP128$STRT
FP128$R065	EQU	$+1-FP128$STRT
       	lda	FP128$L$BSIGN
	xri	080h
FP128$R066	EQU	$+1-FP128$STRT
	sta	FP128$L$BSIGN

FP128$L$FPADD0		EQU	$-FP128$STRT
       	lxi	h,0
FP128$R067	EQU	$+1-FP128$STRT
	shld	FP128$L$OVF
FP128$R068	EQU	$+1-FP128$STRT
	shld	FP128$L$ZERO
FP128$R069	EQU	$+1-FP128$STRT
	lhld	FP128$L$AEXP
	mov	a,h
	ora	l
	xchg
FP128$R070	EQU	$+1-FP128$STRT
	jnz	FP128$L$FADD1
FP128$R071	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$BREG
FP128$R072	EQU	$+1-FP128$STRT
	lxi	d,FP128$L$AREG
	lxi	b,NBYTES+3
	ldir
FP128$R073	EQU	$+1-FP128$STRT
	jmp	FP128$L$FPNORM
FP128$L$FADD1		EQU	$-FP128$STRT
FP128$R074	EQU	$+1-FP128$STRT
      	lhld	FP128$L$BEXP
	mov	a,h
	ora	l
FP128$R075	EQU	$+1-FP128$STRT
	jz	FP128$L$FPNORM
	xchg
	dsbc	d
FP128$R076	EQU	$+1-FP128$STRT
	jz	FP128$L$FADD4
FP128$R077	EQU	$+1-FP128$STRT
	jnc	FP128$L$FADD2

FP128$R078	EQU	$+1-FP128$STRT
	lda	FP128$L$ASIGN
	mov	c,a
FP128$R079	EQU	$+1-FP128$STRT
	lda	FP128$L$BSIGN
FP128$R080	EQU	$+1-FP128$STRT
	sta	FP128$L$ASIGN
	mov	a,c
FP128$R081	EQU	$+1-FP128$STRT
	sta	FP128$L$BSIGN
FP128$R082	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$AREG
FP128$R083	EQU	$+1-FP128$STRT
	lxi	d,FP128$L$BREG
	mvi	b,nbytes+2
FP128$R084	EQU	$+1-FP128$STRT
	call	FP128$L$SWAP0

FP128$R085	EQU	$+1-FP128$STRT
	shld	FP128$L$BEXP
	xchg
FP128$R086	EQU	$+1-FP128$STRT
	shld	FP128$L$AEXP
	ora	a
	dsbc	d
FP128$L$FADD2		EQU	$-FP128$STRT
      	mov	a,h
	ora	a
FP128$R087	EQU	$+1-FP128$STRT
	jnz	FP128$L$FPNORM
	mov	a,l
	cpi	NBYTES*8-1
FP128$R088	EQU	$+1-FP128$STRT
	jnc	FP128$L$FPNORM
	mov	b,a
FP128$R089	EQU	$+1-FP128$STRT
	lhld	FP128$L$BEXP
	xchg
FP128$L$FADD3		EQU	$-FP128$STRT
      	push	b
	xra	a
FP128$R090	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$BREG+NBYTES-1
FP128$R091	EQU	$+1-FP128$STRT
	call	FP128$L$SFTR
	inx	d
	pop	b

	db	010h,0f4h
FP128$L$FADD4		EQU	$-FP128$STRT
      	xchg
FP128$R092	EQU	$+1-FP128$STRT
	shld	FP128$L$BEXP
FP128$R093	EQU	$+1-FP128$STRT
	lda	FP128$L$ASIGN
FP128$R094	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$BSIGN
	xra	m
FP128$R095	EQU	$+1-FP128$STRT
	jnz	FP128$L$FADD5

FP128$R096	EQU	$+1-FP128$STRT
	lxi	d,FP128$L$AREG
FP128$R097	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$BREG
FP128$R098	EQU	$+1-FP128$STRT
	call	FP128$L$IADD
FP128$R099	EQU	$+1-FP128$STRT
	jnc	FP128$L$FPNORM
FP128$R100	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$AREG+NBYTES-1
FP128$R101	EQU	$+1-FP128$STRT
	call	FP128$L$SFTR
FP128$R102	EQU	$+1-FP128$STRT
	lhld	FP128$L$AEXP
	inx	h
FP128$R103	EQU	$+1-FP128$STRT
	shld	FP128$L$AEXP
FP128$R104	EQU	$+1-FP128$STRT
	jmp	FP128$L$FPNORM

FP128$L$FADD5		EQU	$-FP128$STRT
FP128$R105	EQU	$+1-FP128$STRT
      	lxi	d,FP128$L$AREG
FP128$R106	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$BREG
FP128$R107	EQU	$+1-FP128$STRT
	call	FP128$L$ISUB
FP128$R108	EQU	$+1-FP128$STRT
	jnc	FP128$L$FPNORM
FP128$R109	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$AREG
FP128$R110	EQU	$+1-FP128$STRT
	call	FP128$L$INEG
FP128$R111	EQU	$+1-FP128$STRT
	lda	FP128$L$BSIGN
FP128$R112	EQU	$+1-FP128$STRT
	sta	FP128$L$ASIGN
FP128$R113	EQU	$+1-FP128$STRT
	call	FP128$L$FPNORM
	ret


FP128$L$UNPACK		EQU	$-FP128$STRT
       	xra	a
	mov	m,a
	inx	h
	push	h
	xchg
	lxi	b,NBYTES
	ldir
	pop	h
	xra	a
	mvi	b,nbytes
FP128$L$UNPCK1		EQU	$-FP128$STRT
       	rld
	inx	h

	db	010h,0fbh

	mov	c,a
	ani	00000111b
	mov	m,a
	mov	a,c
	ani	00001000b
FP128$R114	EQU	$+1-FP128$STRT
	jz	FP128$L$UNPCK2
	mvi	a,080h
FP128$L$UNPCK2		EQU	$-FP128$STRT
       	inx	h
	mov	m,a
	ret


FP128$L$PACK		EQU	$-FP128$STRT
     	push	d
FP128$R115	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$OVF
	mov	a,m
	inx	h
	ora	m
	inx	h
	ora	m
FP128$R116	EQU	$+1-FP128$STRT
	jnz	FP128$L$PACK1

FP128$R117	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$AREG+1
	mov	a,m
	ani	08h
FP128$R118	EQU	$+1-FP128$STRT
	cnz	FP128$L$INCA

FP128$L$PACK1		EQU	$-FP128$STRT
FP128$R119	EQU	$+1-FP128$STRT
      	lda	FP128$L$ASIGN
	ora	a
	mvi	c,0
FP128$R120	EQU	$+1-FP128$STRT
	jz	FP128$L$PACK2
	mvi	c,08h
FP128$L$PACK2		EQU	$-FP128$STRT
FP128$R121	EQU	$+1-FP128$STRT
      	lda	FP128$L$AEXP+1
	ani	00000111b
	ora	c
FP128$R122	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$AEXP
	mvi	b,nbytes
FP128$L$PACK3		EQU	$-FP128$STRT
      	rrd
	dcx	h

	db	010h,0fbh

	inx	h
	pop	d
	lxi	b,NBYTES
	ldir
	RET

FP128$L$INCA		EQU	$-FP128$STRT
     	mov	a,m
	ani	0f8h
	adi	08h
	mov	m,a
	rnc
	mvi	b,NBYTES-2
FP128$L$INCA1		EQU	$-FP128$STRT
      	inx	h
	inr	m
	rnz

	db	010h,0fbh

	stc
FP128$R123	EQU	$+1-FP128$STRT
	call	FP128$L$SFTR
FP128$R124	EQU	$+1-FP128$STRT
	lhld	FP128$L$AEXP
	inx	h
FP128$R125	EQU	$+1-FP128$STRT
	shld	FP128$L$AEXP
	mov	a,h
	cpi	BIASEXP/128
	rc
	mvi	h,BIASEXP/128-1
FP128$R126	EQU	$+1-FP128$STRT
	shld	FP128$L$AEXP
	mvi	a,08h
FP128$R127	EQU	$+1-FP128$STRT
	sta	FP128$L$OVF
	ret



FP128$L$FPCONV		EQU	$-FP128$STRT
FP128$R128	EQU	$+1-FP128$STRT
       	lda	FP128$L$ASIGN
	ora	a
	mvi	a,' '
FP128$R129	EQU	$+1-FP128$STRT
	jz	FP128$L$CONV1
	mvi	a,'-'
FP128$L$CONV1		EQU	$-FP128$STRT
FP128$R130	EQU	$+1-FP128$STRT
      	sta	FP128$L$OUTSGN
FP128$R131	EQU	$+1-FP128$STRT
	lhld	FP128$L$AEXP
	mov	a,h
	ora	l
FP128$R132	EQU	$+1-FP128$STRT
	jz	FP128$L$CONV9

	xra	a
FP128$R133	EQU	$+1-FP128$STRT
	sta	FP128$L$ASIGN
	lxi	h,0
FP128$R134	EQU	$+1-FP128$STRT
	shld	FP128$L$EP
FP128$L$CONV20		EQU	$-FP128$STRT
       	lxi	h,256
FP128$R135	EQU	$+1-FP128$STRT
	shld	FP128$L$K2

FP128$L$CONV2		EQU	$-FP128$STRT
FP128$R136	EQU	$+1-FP128$STRT
      	lxi	d,FP128$L$AREG+NBYTES+1
FP128$R137	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$ONE +NBYTES+1
	mvi	b,nbytes+2
FP128$R138	EQU	$+1-FP128$STRT
	call	FP128$L$ICMP0
FP128$R139	EQU	$+1-FP128$STRT
	jc	FP128$L$MCONV

FP128$R140	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$TEN256
FP128$R141	EQU	$+1-FP128$STRT
	shld	FP128$L$T

FP128$L$PCONV1		EQU	$-FP128$STRT
       	lxi	d,NBYTES+1
	dad	d
FP128$R142	EQU	$+1-FP128$STRT
	lxi	d,FP128$L$AREG+NBYTES+1
	mvi	b,nbytes+2
FP128$R143	EQU	$+1-FP128$STRT
	call	FP128$L$ICMP0
FP128$R144	EQU	$+1-FP128$STRT
	jc	FP128$L$PCONV2

FP128$R145	EQU	$+1-FP128$STRT
	lhld	FP128$L$T
FP128$R146	EQU	$+1-FP128$STRT
	lxi	d,FP128$L$BREG
	lxi	b,NBYTES+3
	ldir
FP128$R147	EQU	$+1-FP128$STRT
	call	FP128$L$FPDIV0

FP128$R148	EQU	$+1-FP128$STRT
	lhld	FP128$L$K2
	xchg
FP128$R149	EQU	$+1-FP128$STRT
	lhld	FP128$L$EP
	dad	d
FP128$R150	EQU	$+1-FP128$STRT
	shld	FP128$L$EP

FP128$L$PCONV2		EQU	$-FP128$STRT
       
FP128$R151	EQU	$+1-FP128$STRT
	lhld	FP128$L$K2
	srlr	h
	rarr	l
FP128$R152	EQU	$+1-FP128$STRT
	shld	FP128$L$K2
	mov	a,h
	ora	l
FP128$R153	EQU	$+1-FP128$STRT
	jz	FP128$L$CONV3

FP128$R154	EQU	$+1-FP128$STRT
	lhld	FP128$L$T
	lxi	d,nbytes+3
	dad	d
FP128$R155	EQU	$+1-FP128$STRT
	shld	FP128$L$T
FP128$R156	EQU	$+1-FP128$STRT
	jmp	FP128$L$PCONV1


FP128$L$MCONV		EQU	$-FP128$STRT
FP128$R157	EQU	$+1-FP128$STRT
      	lxi	d,FP128$L$AREG+nbytes+1
FP128$R158	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$TENM1+nbytes+1
	mvi	b,nbytes+2
FP128$R159	EQU	$+1-FP128$STRT
	call	FP128$L$ICMP0
FP128$R160	EQU	$+1-FP128$STRT
	jnc	FP128$L$CONV3

FP128$R161	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$TENM128
FP128$R162	EQU	$+1-FP128$STRT
	shld	FP128$L$T

FP128$R163	EQU	$+1-FP128$STRT
	lxi	d,FP128$L$AREG+NBYTES+1
FP128$R164	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$TENM256+NBYTES+1
	mvi	b,nbytes+2
FP128$R165	EQU	$+1-FP128$STRT
	call	FP128$L$ICMP0
FP128$R166	EQU	$+1-FP128$STRT
	jnc	FP128$L$MCONV1
FP128$R167	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$TEN256
FP128$R168	EQU	$+1-FP128$STRT
	lxi	d,FP128$L$BREG
	lxi	b,NBYTES+3
	ldir
FP128$R169	EQU	$+1-FP128$STRT
	call	FP128$L$FPMUL0
FP128$R170	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$TEN256
FP128$R171	EQU	$+1-FP128$STRT
	lxi	d,FP128$L$BREG
	lxi	b,NBYTES+3
	ldir
FP128$R172	EQU	$+1-FP128$STRT
	call	FP128$L$FPMUL0

	lxi	h,-512
FP128$R173	EQU	$+1-FP128$STRT
	shld	FP128$L$EP
FP128$R174	EQU	$+1-FP128$STRT
	jmp	FP128$L$CONV20

FP128$L$MCONV1		EQU	$-FP128$STRT
FP128$R175	EQU	$+1-FP128$STRT
       	lhld	FP128$L$T
	lxi	d,nbytes+1
	dad	d
FP128$R176	EQU	$+1-FP128$STRT
	lxi	d,FP128$L$AREG+NBYTES+1
	mvi	b,nbytes+2
FP128$R177	EQU	$+1-FP128$STRT
	call	FP128$L$ICMP0
FP128$R178	EQU	$+1-FP128$STRT
	jc	FP128$L$MCONV2

FP128$R179	EQU	$+1-FP128$STRT
	lhld	FP128$L$K2
	srlr	h
	rarr	l
FP128$R180	EQU	$+1-FP128$STRT
	shld	FP128$L$K2

FP128$R181	EQU	$+1-FP128$STRT
	lhld	FP128$L$T
	lxi	d,nbytes+3
	dad	d
FP128$R182	EQU	$+1-FP128$STRT
	shld	FP128$L$T
FP128$R183	EQU	$+1-FP128$STRT
	jmp	FP128$L$MCONV1


FP128$L$MCONV2		EQU	$-FP128$STRT
FP128$R184	EQU	$+1-FP128$STRT
       	lhld	FP128$L$T
	dcx	h
FP128$R185	EQU	$+1-FP128$STRT
	lxi	d,FP128$L$BREG+NBYTES+2
	lxi	b,NBYTES+3
	lddr
FP128$R186	EQU	$+1-FP128$STRT
	call	FP128$L$FPDIV0

FP128$R187	EQU	$+1-FP128$STRT
	lhld	FP128$L$K2
	xchg
FP128$R188	EQU	$+1-FP128$STRT
	lhld	FP128$L$EP
	ora	a
	dsbc	d
FP128$R189	EQU	$+1-FP128$STRT
	shld	FP128$L$EP
FP128$R190	EQU	$+1-FP128$STRT
	jmp	FP128$L$CONV20


FP128$L$CONV3		EQU	$-FP128$STRT
FP128$R191	EQU	$+1-FP128$STRT
      	lxi	d,FP128$L$AREG+NBYTES+1
FP128$R192	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$ONE +NBYTES+1
	mvi	b,nbytes+2
FP128$R193	EQU	$+1-FP128$STRT
	call	FP128$L$ICMP0
FP128$R194	EQU	$+1-FP128$STRT
	jc	FP128$L$CONV4
FP128$R195	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$TEN1
FP128$R196	EQU	$+1-FP128$STRT
	lxi	d,FP128$L$BREG
	lxi	b,NBYTES+3
	ldir
FP128$R197	EQU	$+1-FP128$STRT
	call	FP128$L$FPDIV0
FP128$R198	EQU	$+1-FP128$STRT
	lhld	FP128$L$EP
	inx	h
FP128$R199	EQU	$+1-FP128$STRT
	shld	FP128$L$EP
FP128$L$CONV4		EQU	$-FP128$STRT
      	lxi	h,0
FP128$R200	EQU	$+1-FP128$STRT
	shld	FP128$L$AREG-1
FP128$R201	EQU	$+1-FP128$STRT
	lhld	FP128$L$AEXP
	lxi	d,BIASEXP
	xchg
	ora	a
	dsbc	d
FP128$R202	EQU	$+1-FP128$STRT
	jz	FP128$L$CONV6
	mov	b,l
FP128$L$CONV5		EQU	$-FP128$STRT
      	push	b
FP128$R203	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$AREG+NBYTES-1
	xra	a
	mvi	b,nbytes+1
FP128$R204	EQU	$+1-FP128$STRT
	call	FP128$L$SFTR0
	pop	b

	db	010h,0f3h
FP128$L$CONV6		EQU	$-FP128$STRT
      	mvi	b,34
FP128$R205	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$OUTBUF+2
FP128$L$CONV7		EQU	$-FP128$STRT
FP128$R206	EQU	$+1-FP128$STRT
      	call	FP128$L$TENTHA
	adi	'0'
	mov	m,a
	inx	h

	db	010h,0f7h

FP128$R207	EQU	$+1-FP128$STRT
	call	FP128$L$TENTHA
	cpi	5
FP128$R208	EQU	$+1-FP128$STRT
	jc	FP128$L$CONV8

	mvi	b,34
FP128$L$CONV70		EQU	$-FP128$STRT
       	dcx	h
	mov	a,m
	inr	a
	mov	m,a
	cpi	'9'+1
FP128$R209	EQU	$+1-FP128$STRT
	jnz	FP128$L$CONV8
	mvi	a,'0'
	mov	m,a

	db	010h,0f2h

	mvi	a,'1'
FP128$R210	EQU	$+1-FP128$STRT
	sta	FP128$L$OUTBUF+2
FP128$R211	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$OUTBUF+3
FP128$R212	EQU	$+1-FP128$STRT
	lxi	d,FP128$L$OUTBUF+4
	lxi	b,33
	mvi	m,'0'
	ldir
FP128$R213	EQU	$+1-FP128$STRT
	lhld	FP128$L$EP
	inx	h
FP128$R214	EQU	$+1-FP128$STRT
	shld	FP128$L$EP

FP128$L$CONV8		EQU	$-FP128$STRT
      	mvi	a,0
FP128$R215	EQU	$+1-FP128$STRT
	sta	FP128$L$OUTBUF+36
	mvi	a,'0'
FP128$R216	EQU	$+1-FP128$STRT
	sta	FP128$L$OUTBUF
	mvi	a,'.'
FP128$R217	EQU	$+1-FP128$STRT
	sta	FP128$L$OUTBUF+1
FP128$R218	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$EP
	pop	b
	ret

FP128$L$CONV9		EQU	$-FP128$STRT
FP128$R219	EQU	$+1-FP128$STRT
      	lxi	h,FP128$L$OUTBUF+2
FP128$R220	EQU	$+1-FP128$STRT
	lxi	d,FP128$L$OUTBUF+3
	lxi	b,34
	mvi	m,'0'
	ldir
	mvi	m,0
FP128$R221	EQU	$+1-FP128$STRT
	jmp	FP128$L$CONV8

FP128$L$TENTHA		EQU	$-FP128$STRT
       	push	h
	push	d
	push	b
FP128$R222	EQU	$+1-FP128$STRT
 	lxi	h,FP128$L$AREG-1
FP128$R223	EQU	$+1-FP128$STRT
	lxi	d,FP128$L$BREG-1
	lxi	b,NBYTES+4
	LDIR
	stc
FP128$R224	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$AREG-1
	mvi	b,nbytes+1
FP128$R225	EQU	$+1-FP128$STRT
	call	FP128$L$SFTL0
	mvi	a,0
	ral

FP128$R226	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$AREG-1
	mvi	b,nbytes+1
FP128$R227	EQU	$+1-FP128$STRT
	call	FP128$L$SFTL0
	ral

	mov	c,a
FP128$R228	EQU	$+1-FP128$STRT
	lxi	d,FP128$L$AREG-1
FP128$R229	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$BREG-1
	mvi	b,nbytes+1
FP128$R230	EQU	$+1-FP128$STRT
	call	FP128$L$IADD0
	mvi	a,0
	adc	c

FP128$R231	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$AREG-1
	mvi	b,nbytes+1
FP128$R232	EQU	$+1-FP128$STRT
	call	FP128$L$SFTL0
	ral
	pop	b
	pop	d
	pop	h
	ret


FP128$L$FPIN		EQU	$-FP128$STRT
FP128$R233	EQU	$+1-FP128$STRT
     	call	FP128$L$CLEARA
	lxi	h,0
FP128$R234	EQU	$+1-FP128$STRT
	shld	FP128$L$EPX
	mvi	a,0
FP128$R235	EQU	$+1-FP128$STRT
	sta	FP128$L$SIGNX
FP128$R236	EQU	$+1-FP128$STRT
	sta	FP128$L$OUTSGN
	lhld	arg2
	xra	a
 	lxi	b,100
	ccir
FP128$R237	EQU	$+1-FP128$STRT
	jnz	FP128$L$FPIN15
	xchg
	lhld	arg2
	xchg
	ora	a
	dsbc	d
	push	h
	mov	b,h
	mov	c,l
	lhld	arg2
	mvi	a,'E'
	ccir
	pop	b
FP128$R238	EQU	$+1-FP128$STRT
	jz	FP128$L$FPIN1

	lhld	arg2
	mvi	a,'e'
	ccir
FP128$R239	EQU	$+1-FP128$STRT
	jnz	FP128$L$FPIN6

FP128$L$FPIN1		EQU	$-FP128$STRT
      	dcx	h
	mvi	m,0
	inx	h
	mov	a,m
	cpi	'-'
FP128$R240	EQU	$+1-FP128$STRT
	jnz	FP128$L$FPIN2
FP128$R241	EQU	$+1-FP128$STRT
	sta	FP128$L$SIGNX
FP128$R242	EQU	$+1-FP128$STRT
	jmp	FP128$L$FPIN3
FP128$L$FPIN2		EQU	$-FP128$STRT
      	cpi	'+'
FP128$R243	EQU	$+1-FP128$STRT
	jnz	FP128$L$FPIN4
FP128$L$FPIN3		EQU	$-FP128$STRT
      	inx	h
FP128$L$FPIN4		EQU	$-FP128$STRT
FP128$R244	EQU	$+1-FP128$STRT
      	call	FP128$L$CTOI
FP128$R245	EQU	$+1-FP128$STRT
	jc	FP128$L$FPIN5
	push	h
FP128$R246	EQU	$+1-FP128$STRT
	lhld	FP128$L$EPX
	mov	d,h
	mov	e,l
	dad	h
	dad	h
	dad	d
	dad	h
	mov	e,a
	mvi	d,0
	dad	d
FP128$R247	EQU	$+1-FP128$STRT
	shld	FP128$L$EPX
	pop	h
FP128$R248	EQU	$+1-FP128$STRT
	jmp	FP128$L$FPIN3

FP128$L$FPIN5		EQU	$-FP128$STRT
FP128$R249	EQU	$+1-FP128$STRT
      	lda	FP128$L$SIGNX
	cpi	'-'
FP128$R250	EQU	$+1-FP128$STRT
	jnz	FP128$L$FPIN6
FP128$R251	EQU	$+1-FP128$STRT
	lhld	FP128$L$EPX
	xchg
	lxi	h,0
	ora	a
	dsbc	d
FP128$R252	EQU	$+1-FP128$STRT
	shld	FP128$L$EPX

FP128$L$FPIN6		EQU	$-FP128$STRT
      	xra	a
FP128$R253	EQU	$+1-FP128$STRT
	sta	FP128$L$SIGNX
	lhld	arg2
	mov	a,m
	cpi	'+'
FP128$R254	EQU	$+1-FP128$STRT
	jz	FP128$L$FPIN7
	cpi	'-'
FP128$R255	EQU	$+1-FP128$STRT
	jnz	FP128$L$FPIN8
FP128$R256	EQU	$+1-FP128$STRT
	sta	FP128$L$OUTSGN
FP128$L$FPIN7		EQU	$-FP128$STRT
      	inx	h
FP128$L$FPIN8		EQU	$-FP128$STRT
      	mov	a,m
	cpi	'.'
FP128$R257	EQU	$+1-FP128$STRT
	jz	FP128$L$FPIN10
	cpi	'0'
FP128$R258	EQU	$+1-FP128$STRT
	jnz	FP128$L$FPIN11
FP128$R259	EQU	$+1-FP128$STRT
	jmp	FP128$L$FPIN7

FP128$L$FPIN10		EQU	$-FP128$STRT
       	inx	h
	mov	a,m
	cpi	'0'
FP128$R260	EQU	$+1-FP128$STRT
	jnz	FP128$L$FPIN13
	xchg
FP128$R261	EQU	$+1-FP128$STRT
	lhld	FP128$L$EPX
	dcx	h
FP128$R262	EQU	$+1-FP128$STRT
	shld	FP128$L$EPX
	xchg
FP128$R263	EQU	$+1-FP128$STRT
	jmp	FP128$L$FPIN10

FP128$L$FPIN11		EQU	$-FP128$STRT
FP128$R264	EQU	$+1-FP128$STRT
       	call	FP128$L$CTOI
FP128$R265	EQU	$+1-FP128$STRT
	jc	FP128$L$FPIN12
FP128$R266	EQU	$+1-FP128$STRT
	call	FP128$L$FPINX
	inx	h
FP128$R267	EQU	$+1-FP128$STRT
	jmp	FP128$L$FPIN11

FP128$L$FPIN12		EQU	$-FP128$STRT
       	cpi	'.'
FP128$R268	EQU	$+1-FP128$STRT
	jnz	FP128$L$FPIN14
	inx	h
FP128$L$FPIN13		EQU	$-FP128$STRT
FP128$R269	EQU	$+1-FP128$STRT
       	call	FP128$L$CTOI
FP128$R270	EQU	$+1-FP128$STRT
	jc	FP128$L$FPIN14
FP128$R271	EQU	$+1-FP128$STRT
	call	FP128$L$FPINX
	xchg
FP128$R272	EQU	$+1-FP128$STRT
	lhld	FP128$L$EPX
	dcx	h
FP128$R273	EQU	$+1-FP128$STRT
	shld	FP128$L$EPX
	xchg
	inx	h
FP128$R274	EQU	$+1-FP128$STRT
	jmp	FP128$L$FPIN13

FP128$L$FPIN14		EQU	$-FP128$STRT
FP128$R275	EQU	$+1-FP128$STRT
       	lhld	FP128$L$EPX
	mov	a,h
	ora	l
FP128$R276	EQU	$+1-FP128$STRT
	jz	FP128$L$FPIN15
FP128$R277	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$AREG
FP128$R278	EQU	$+1-FP128$STRT
	lxi	d,FP128$L$XX
	lxi	b,NBYTES+3
	ldir
FP128$R279	EQU	$+1-FP128$STRT
	call	FP128$L$EXP
FP128$R280	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$AREG
FP128$R281	EQU	$+1-FP128$STRT
	lxi	d,FP128$L$BREG
	lxi	b,NBYTES+3
	ldir
FP128$R282	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$XX
FP128$R283	EQU	$+1-FP128$STRT
	lxi	d,FP128$L$AREG
	lxi	b,NBYTES+3
	ldir

FP128$R284	EQU	$+1-FP128$STRT
	lda	FP128$L$SIGNX
	cpi	'-'
FP128$R285	EQU	$+1-FP128$STRT
	jnz	FP128$L$FPIN17
FP128$R286	EQU	$+1-FP128$STRT
	call	FP128$L$FPDIV0
FP128$R287	EQU	$+1-FP128$STRT
	jmp	FP128$L$FPIN15
FP128$L$FPIN17		EQU	$-FP128$STRT
FP128$R288	EQU	$+1-FP128$STRT
       	call	FP128$L$FPMUL0

FP128$L$FPIN15		EQU	$-FP128$STRT
FP128$R289	EQU	$+1-FP128$STRT
       	lda	FP128$L$OUTSGN
	ora	a
FP128$R290	EQU	$+1-FP128$STRT
	jz	FP128$L$FPIN16
	mvi	a,080h
FP128$R291	EQU	$+1-FP128$STRT
	sta	FP128$L$ASIGN
FP128$L$FPIN16		EQU	$-FP128$STRT
       	lhld	arg3
	xchg
FP128$R292	EQU	$+1-FP128$STRT
	call	FP128$L$PACK
	pop	b
	ret

FP128$L$CTOI		EQU	$-FP128$STRT
     	mov	a,m
FP128$R293	EQU	$+1-FP128$STRT
	call	FP128$L$ISDIGIT
	rc
	sui	'0'
	ret

FP128$L$FPINX		EQU	$-FP128$STRT
      	push	h
	push	psw
FP128$R294	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$TEN1
FP128$R295	EQU	$+1-FP128$STRT
	lxi	d,FP128$L$BREG
	lxi	b,NBYTES+3
	ldir
FP128$R296	EQU	$+1-FP128$STRT
	call	FP128$L$FPMUL0
	pop	psw
	mov	c,a
	add	a
	add	a
	add	c
	add	a
	add	a
	sub	c
	mov	c,a
	mvi	b,0
FP128$R297	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$NUM0
	dad	b
FP128$R298	EQU	$+1-FP128$STRT
	lxi	d,FP128$L$BREG
	lxi	b,NBYTES+3
	ldir
FP128$R299	EQU	$+1-FP128$STRT
	call	FP128$L$FPADD0
	pop	h
	ret

FP128$L$EXP		EQU	$-FP128$STRT
FP128$R300	EQU	$+1-FP128$STRT
    	lhld	FP128$L$EPX
	mov	a,h
	ora	a
FP128$R301	EQU	$+1-FP128$STRT
	jp	FP128$L$EXP0
	xchg
	lxi	h,0
	ora	a
	dsbc	d
FP128$R302	EQU	$+1-FP128$STRT
	shld	FP128$L$EPX
	mvi	a,'-'
FP128$R303	EQU	$+1-FP128$STRT
	sta	FP128$L$SIGNX

FP128$L$EXP0		EQU	$-FP128$STRT
FP128$R304	EQU	$+1-FP128$STRT
     	lxi	h,FP128$L$ONE
FP128$R305	EQU	$+1-FP128$STRT
	lxi	d,FP128$L$AREG
	lxi	b,NBYTES+3
	ldir
FP128$R306	EQU	$+1-FP128$STRT
	lhld	FP128$L$EPX
	mov	a,h
	ora	l
	rz
	mov	a,h
	ora	a
FP128$R307	EQU	$+1-FP128$STRT
	jz	FP128$L$EXP1
FP128$R308	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$TEN256
FP128$R309	EQU	$+1-FP128$STRT
	lxi	d,FP128$L$AREG
	lxi	b,NBYTES+3
	ldir
FP128$R310	EQU	$+1-FP128$STRT
	lhld	FP128$L$EPX
FP128$L$EXP1		EQU	$-FP128$STRT
     	mvi	c,128
	mvi	b,0
FP128$L$EXP2		EQU	$-FP128$STRT
     	mov	a,l
	ora	a
	rz
	sub	c
FP128$R311	EQU	$+1-FP128$STRT
	jc	FP128$L$EXP3
	mov	l,a
	push	b
	push	h
	mov	a,b
	add	a
	add	a
	add	b
	add	a
	add	a
	sub	b
	mov	c,a
	mvi	b,0
FP128$R312	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$TEN128
	dad	b
FP128$R313	EQU	$+1-FP128$STRT
	lxi	d,FP128$L$BREG
	lxi	b,NBYTES+3
	ldir
FP128$R314	EQU	$+1-FP128$STRT
	call	FP128$L$FPMUL0
	pop	h
	pop	b
FP128$L$EXP3		EQU	$-FP128$STRT
     	srlr	c
	inr	b
FP128$R315	EQU	$+1-FP128$STRT
	jmp	FP128$L$EXP2


FP128$L$SIGNX		EQU	$-FP128$STRT
     	ds	1
FP128$L$EPX		EQU	$-FP128$STRT
   	ds	2
FP128$L$T		EQU	$-FP128$STRT
 	ds	2
FP128$L$NUMLEN		EQU	$-FP128$STRT
      	ds	2

FP128$L$FPTEST		EQU	$-FP128$STRT
       	lhld	arg2
	push	h
	pop	d
	dad	h
	dad	h
	dad	d
	dad	h
	dad	h
	dsbc	d
FP128$R316	EQU	$+1-FP128$STRT
	lxi	d,FP128$L$TEN256
	dad	d
FP128$R317	EQU	$+1-FP128$STRT
	lxi	d,FP128$L$AREG
	lxi	b,NBYTES+3
	ldir
FP128$R318	EQU	$+1-FP128$STRT
	jmp	FP128$L$FPCONV


FP128$L$FPTST2		EQU	$-FP128$STRT
       	lhld	arg2
FP128$R319	EQU	$+1-FP128$STRT
	lxi	d,FP128$L$AREG
	lxi	b,NBYTES+3
	ldir
FP128$R320	EQU	$+1-FP128$STRT
	jmp	FP128$L$FPCONV


FP128$L$ISDIGIT		EQU	$-FP128$STRT
        cpi	'0'
	rc
	cpi	'9'+1
	cmc
	ret

FP128$L$CLEARA		EQU	$-FP128$STRT
       	push	h
FP128$R321	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$AREG
	mvi	b,nbytes+3
FP128$R322	EQU	$+1-FP128$STRT
	call	FP128$L$ICLR0
	pop	h
	ret

FP128$L$ICLR		EQU	$-FP128$STRT
     	mvi	b,nbytes
FP128$L$ICLR0		EQU	$-FP128$STRT
      	push	psw
	xra	a
FP128$L$ICLR1		EQU	$-FP128$STRT
      	mov	m,a
	inx	h

	db	010h,0fch
	pop	psw
	ret

FP128$L$IMUL		EQU	$-FP128$STRT
     	shld	tmp
FP128$R323	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$AREG
FP128$R324	EQU	$+1-FP128$STRT
	call	FP128$L$ICLR
	mvi	b,nbytes*8
FP128$L$LOOPML		EQU	$-FP128$STRT
       	push	b
FP128$R325	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$LA+nbytes-1
FP128$R326	EQU	$+1-FP128$STRT
	call	FP128$L$SFTR
FP128$R327	EQU	$+1-FP128$STRT
	jnc	FP128$L$JUMPML
	lhld	tmp
FP128$R328	EQU	$+1-FP128$STRT
	lxi	d,FP128$L$AREG
	ora	a
FP128$R329	EQU	$+1-FP128$STRT
	call	FP128$L$IADD
FP128$L$JUMPML		EQU	$-FP128$STRT
FP128$R330	EQU	$+1-FP128$STRT
       	lxi	h,FP128$L$AREG+nbytes-1
FP128$R331	EQU	$+1-FP128$STRT
	call	FP128$L$SFTR
	pop	b

	db	010h,0e3h
FP128$R332	EQU	$+1-FP128$STRT
	call	FP128$L$SFTR
	ret

FP128$L$IDIV		EQU	$-FP128$STRT
     	shld	tmp
FP128$R333	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$AREG
FP128$R334	EQU	$+1-FP128$STRT
	call	FP128$L$ICLR
	mvi	b,nbytes*8
FP128$R335	EQU	$+1-FP128$STRT
	lda	FP128$L$LA+nbytes-1
	bit	7,a
FP128$R336	EQU	$+1-FP128$STRT
	jnz	FP128$L$LOOPDV
FP128$L$DVCHKK		EQU	$-FP128$STRT
       
	db	010h,02h
	stc
	ret
FP128$L$DVCHK		EQU	$-FP128$STRT
      	push	b
FP128$R337	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$LA
FP128$R338	EQU	$+1-FP128$STRT
	call	FP128$L$SFTL
	pop	b
FP128$R339	EQU	$+1-FP128$STRT
	jp	FP128$L$DVCHKK
FP128$L$LOOPDV		EQU	$-FP128$STRT
       	push	b
FP128$R340	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$LA
FP128$R341	EQU	$+1-FP128$STRT
	call	FP128$L$SFTL
FP128$R342	EQU	$+1-FP128$STRT
	call	FP128$L$SFTL
	lhld	tmp
FP128$R343	EQU	$+1-FP128$STRT
	lxi	d,FP128$L$AREG
	ora	a
FP128$R344	EQU	$+1-FP128$STRT
	call	FP128$L$ISUB
FP128$R345	EQU	$+1-FP128$STRT
	jnc	FP128$L$JUMPDV
	lhld	tmp
FP128$R346	EQU	$+1-FP128$STRT
	lxi	d,FP128$L$AREG
	ora	a
FP128$R347	EQU	$+1-FP128$STRT
	call	FP128$L$IADD
FP128$L$JUMPDV		EQU	$-FP128$STRT
       	cmc	
	pop	b

	db	010h,0dbh
FP128$R348	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$LA
FP128$R349	EQU	$+1-FP128$STRT
	call	FP128$L$SFTL
	ana	a
	ret

FP128$L$IADD		EQU	$-FP128$STRT
     	mvi	b,nbytes
FP128$L$IADD0		EQU	$-FP128$STRT
      	ldax	d
	adc	m
	stax	d
	inx	h
	inx	d

	db	010h,0f9h
	ret

FP128$L$ISUB		EQU	$-FP128$STRT
     	mvi	b,nbytes
FP128$L$ISUB1		EQU	$-FP128$STRT
      	ldax	d
	sbb	m
	stax	d
	inx	d
	inx	h

	db	010h,0f9h
	ret

FP128$L$ICMP		EQU	$-FP128$STRT
     	mvi	b,nbytes
FP128$L$ICMP0		EQU	$-FP128$STRT
      	ldax	d
	sub	m
	rnz
	dcx	d
	dcx	h

	db	010h,0f9h
	ret

FP128$L$INEG		EQU	$-FP128$STRT
     	mvi	b,nbytes
FP128$L$INEG0		EQU	$-FP128$STRT
      	mov	a,m
	cma
	adi	1
	mov	m,a
	dcr	b
FP128$L$INEG1		EQU	$-FP128$STRT
      	inx	h
	mov	a,m
	cma
	aci	0
	mov	m,a

	db	010h,0f8h
	ret

FP128$L$SFTL		EQU	$-FP128$STRT
     	mvi	b,nbytes
FP128$L$SFTL0		EQU	$-FP128$STRT
      	ralr	m
	inx	h

	db	010h,0fbh
	ret

FP128$L$SFTR		EQU	$-FP128$STRT
     	mvi	b,nbytes
FP128$L$SFTR0		EQU	$-FP128$STRT
      	rarr	m
	dcx	h

	db	010h,0fbh
	ret

FP128$L$TENTH		EQU	$-FP128$STRT
      	push	b
	shld	tmp
FP128$R350	EQU	$+1-FP128$STRT
	lxi	d,FP128$L$LLWORK
	lxi	b,nbytes
	ldir
	xra	a
	lhld	tmp
FP128$R351	EQU	$+1-FP128$STRT
	call	FP128$L$SFTL
	ral
	ora	a
	lhld	tmp
FP128$R352	EQU	$+1-FP128$STRT
	call	FP128$L$SFTL
	ral
	mov	c,a
	lhld	tmp
FP128$R353	EQU	$+1-FP128$STRT
	lxi	d,FP128$L$LLWORK
	xchg
FP128$R354	EQU	$+1-FP128$STRT
	call	FP128$L$IADD
	mvi	a,0
	adc	c
	ora	a
	lhld	tmp
FP128$R355	EQU	$+1-FP128$STRT
	call	FP128$L$SFTL
	ral
	pop	b
	ret

FP128$L$SWAP		EQU	$-FP128$STRT
     	mvi	b,nbytes
FP128$L$SWAP0		EQU	$-FP128$STRT
      	mov	c,m
	ldax	d
	mov	m,a
	mov	a,c
	stax	d
	inx	h
	inx	d

	db	010h,0f7h
	ret


FP128$L$FPNORM		EQU	$-FP128$STRT
FP128$R356	EQU	$+1-FP128$STRT
       	lhld	FP128$L$AEXP
	xchg
	lxi	b,1
FP128$L$FPNRM1		EQU	$-FP128$STRT
FP128$R357	EQU	$+1-FP128$STRT
       	lda	FP128$L$AREG+NBYTES-1
	ral
FP128$R358	EQU	$+1-FP128$STRT
	jc	FP128$L$FPNRM2
FP128$R359	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$LA
FP128$R360	EQU	$+1-FP128$STRT
	call	FP128$L$SFTL
FP128$R361	EQU	$+1-FP128$STRT
	call	FP128$L$SFTL
	xchg
	dsbc	b
	xchg
FP128$R362	EQU	$+1-FP128$STRT
	jnc	FP128$L$FPNRM1
FP128$R363	EQU	$+1-FP128$STRT
	jmp	FP128$L$UNDRFW

FP128$L$FPNRM2		EQU	$-FP128$STRT
       	xchg
	mov	a,h
	cpi	BIASEXP/128
FP128$R364	EQU	$+1-FP128$STRT
	jnc	FP128$L$OVRFW
FP128$R365	EQU	$+1-FP128$STRT
	shld	FP128$L$AEXP
FP128$R366	EQU	$+1-FP128$STRT
	jmp	FP128$L$EXTNRM


FP128$L$OVRFW		EQU	$-FP128$STRT
FP128$R367	EQU	$+1-FP128$STRT
      	lxi	h,FP128$L$AREG
FP128$R368	EQU	$+1-FP128$STRT
	call	FP128$L$ICLR
	mvi	h,BIASEXP/128-1
FP128$R369	EQU	$+1-FP128$STRT
	shld	FP128$L$AEXP
	mvi	a,08h
FP128$R370	EQU	$+1-FP128$STRT
	sta	FP128$L$OVF
	xra	a
FP128$R371	EQU	$+1-FP128$STRT
	sta	FP128$L$ZERO
FP128$R372	EQU	$+1-FP128$STRT
	jmp	FP128$L$EXTNRM

FP128$L$UNDRFW		EQU	$-FP128$STRT
FP128$R373	EQU	$+1-FP128$STRT
       	lxi	h,FP128$L$AREG
	mvi	b,nbytes+2
FP128$R374	EQU	$+1-FP128$STRT
	call	FP128$L$ICLR0
	mvi	a,04h
FP128$R375	EQU	$+1-FP128$STRT
	sta	FP128$L$UNF
	xra	a
FP128$R376	EQU	$+1-FP128$STRT
	sta	FP128$L$ZERO
FP128$L$EXTNRM		EQU	$-FP128$STRT
FP128$R377	EQU	$+1-FP128$STRT
       	lda	FP128$L$ASIGN
	ora	a
FP128$R378	EQU	$+1-FP128$STRT
	jz	FP128$L$EXTNM2
	mvi	a,1
FP128$L$EXTNM2		EQU	$-FP128$STRT
FP128$R379	EQU	$+1-FP128$STRT
       	sta	FP128$L$MINUS
	ret

FP128$L$SETZERO		EQU	$-FP128$STRT
        
FP128$R380	EQU	$+1-FP128$STRT
	lxi	h,FP128$L$AREG
	mvi	b,nbytes+3
FP128$R381	EQU	$+1-FP128$STRT
	call	FP128$L$ICLR0
	lxi	h,0
FP128$R382	EQU	$+1-FP128$STRT
	shld	FP128$L$OVF
	mvi	a,020h
FP128$R383	EQU	$+1-FP128$STRT
	sta	FP128$L$ZERO
	xra	a
FP128$R384	EQU	$+1-FP128$STRT
	sta	FP128$L$MINUS
	ret


FP128$L$LLWORK		EQU	$-FP128$STRT
      	ds	nbytes+3


FP128$L$K2		EQU	$-FP128$STRT
  	ds	2
FP128$L$EP		EQU	$-FP128$STRT
  	ds	2
FP128$L$OUTSGN		EQU	$-FP128$STRT
      	ds	1
FP128$L$OUTBUF		EQU	$-FP128$STRT
      	ds	48

FP128$L$OVF		EQU	$-FP128$STRT
   	ds	1
FP128$L$UNF		EQU	$-FP128$STRT
   	ds	1
FP128$L$ZERO		EQU	$-FP128$STRT
    	ds	1
FP128$L$MINUS		EQU	$-FP128$STRT
     	ds	1

FP128$L$TEMPW		EQU	$-FP128$STRT
     	ds	nbytes+8
FP128$L$UU		EQU	$-FP128$STRT
  	ds	nbytes+3
FP128$L$VV		EQU	$-FP128$STRT
  	ds	nbytes+3
FP128$L$WW		EQU	$-FP128$STRT
  	ds	nbytes+3
FP128$L$XX		EQU	$-FP128$STRT
  	ds	nbytes+3
FP128$L$YY		EQU	$-FP128$STRT
  	ds	nbytes+3

FP128$L$LA		EQU	$-FP128$STRT
  	DS	NBYTES
FP128$L$AREG		EQU	$-FP128$STRT
    	DS	NBYTES
FP128$L$AEXP		EQU	$-FP128$STRT
    	DS	2
FP128$L$ASIGN		EQU	$-FP128$STRT
     	DS	1

FP128$L$LB		EQU	$-FP128$STRT
   	DS	NBYTES
FP128$L$BREG		EQU	$-FP128$STRT
     	DS	NBYTES
FP128$L$BEXP		EQU	$-FP128$STRT
     	DS	2
FP128$L$BSIGN		EQU	$-FP128$STRT
      	DS	1

FP128$L$TEN256		EQU	$-FP128$STRT
      	db	0b7h,0eah,0feh,098h,01bh,090h,0bbh,0ddh
	db	08dh,0deh,0f9h,09dh,0fbh,0ebh,07eh,0aah
	dw	biasexp+353h
	db	000h
FP128$L$TEN128		EQU	$-FP128$STRT
      	db	037h,001h,0b1h,036h,06ch,033h,06fh,0c6h
	db	0dfh,08ch,0e9h,080h,0c9h,047h,0bah,093h
	dw	biasexp+1aah
	db	000h
FP128$L$TEN64		EQU	$-FP128$STRT
     	db	0fbh,025h,06bh,0c7h,071h,06bh,0bfh,03ch
	db	0d5h,0a6h,0cfh,0ffh,049h,01fh,078h,0c2h
	dw	biasexp+0d5h
	db	000h
FP128$L$TEN32		EQU	$-FP128$STRT
     	db	000h,000h,000h,000h,000h,000h,020h,0f0h
	db	09dh,0b5h,070h,02bh,0a8h,0adh,0c5h,09dh
	dw	biasexp+06bh
	db	000h
FP128$L$TEN16		EQU	$-FP128$STRT
     	db	000h,000h,000h,000h,000h,000h,000h,000h
	db	000h,000h,000h,004h,0bfh,0c9h,01bh,08eh
	dw	biasexp+036h
	db	000h
FP128$L$TEN8		EQU	$-FP128$STRT
    	db	000h,000h,000h,000h,000h,000h,000h,000h
	db	000h,000h,000h,000h,000h,020h,0bch,0beh
	dw	biasexp+01bh
	db	000h
FP128$L$TEN4		EQU	$-FP128$STRT
    	db	000h,000h,000h,000h,000h,000h,000h,000h
	db	000h,000h,000h,000h,000h,000h,040h,09ch
	dw	biasexp+00eh
	db	000h
FP128$L$TEN2		EQU	$-FP128$STRT
    	db	000h,000h,000h,000h,000h,000h,000h,000h
	db	000h,000h,000h,000h,000h,000h,000h,0c8h
	dw	biasexp+007h
	db	000h
FP128$L$TEN1		EQU	$-FP128$STRT
    	db	000h,000h,000h,000h,000h,000h,000h,000h
	db	000h,000h,000h,000h,000h,000h,000h,0a0h
	dw	biasexp+004h
	db	000h
FP128$L$ONE		EQU	$-FP128$STRT
   	db	000h,000h,000h,000h,000h,000h,000h,000h
	db	000h,000h,000h,000h,000h,000h,000h,080h
	dw	biasexp+001h
	db	000h

FP128$L$TENM256		EQU	$-FP128$STRT
       	db	008h,053h,0fbh,0feh,055h,011h,091h,0fah
	db	039h,019h,07ah,063h,025h,043h,031h,0c0h
	dw	biasexp-352h
	db	000h
FP128$L$TENM128		EQU	$-FP128$STRT
       	db	0deh,0dbh,05dh,0d0h,0f6h,0b3h,07ch,0ach
	db	0a0h,0e4h,0bch,064h,07ch,046h,0d0h,0ddh
	dw	biasexp-1a9h
	db	000h
FP128$L$TENM64		EQU	$-FP128$STRT
      	db	024h,062h,0b3h,047h,0d7h,098h,023h,03fh
	db	0a5h,0e9h,039h,0a5h,027h,0eah,07fh,0a8h
	dw	biasexp-0d4h
	db	000h
FP128$L$TENM32		EQU	$-FP128$STRT
      	db	0f2h,04ah,081h,0a5h,0edh,018h,0deh,067h
	db	0bah,094h,039h,045h,0adh,01eh,0b1h,0cfh
	dw	biasexp-06ah
	db	000h
FP128$L$TENM16		EQU	$-FP128$STRT
      	db	0b3h,0a9h,089h,079h,068h,0beh,02eh,04ch
	db	05bh,0e1h,04dh,0c4h,0beh,094h,095h,0e6h
	dw	biasexp-035h
	db	000h
FP128$L$TENM8		EQU	$-FP128$STRT
     	db	03dh,07ch,0bah,036h,02bh,00dh,0c2h,0fdh
	db	0fch,0ceh,061h,084h,011h,077h,0cch,0abh
	dw	biasexp-01ah
	db	000h
FP128$L$TENM4		EQU	$-FP128$STRT
     	db	0a8h,0a4h,04eh,040h,013h,061h,0c3h,0d3h
	db	02bh,065h,019h,0e2h,058h,017h,0b7h,0d1h
	dw	biasexp-00dh
	db	000h
FP128$L$TENM2		EQU	$-FP128$STRT
     	db	0a3h,070h,03dh,00ah,0d7h,0a3h,070h,03dh
	db	00ah,0d7h,0a3h,070h,03dh,00ah,0d7h,0a3h
	dw	biasexp-006h
	db	000h
FP128$L$TENM1		EQU	$-FP128$STRT
     	db	0cdh,0cch,0cch,0cch,0cch,0cch,0cch,0cch
	db	0cch,0cch,0cch,0cch,0cch,0cch,0cch,0cch
	dw	biasexp-003h
	db	000h

FP128$L$NUM0		EQU	$-FP128$STRT
    	db	000h,000h,000h,000h,000h,000h,000h,000h
	db	000h,000h,000h,000h,000h,000h,000h,000h
	dw	000h
	db	000h
FP128$L$NUM1		EQU	$-FP128$STRT
    	db	000h,000h,000h,000h,000h,000h,000h,000h
	db	000h,000h,000h,000h,000h,000h,000h,080h
	dw	biasexp+001h
	db	000h
FP128$L$NUM2		EQU	$-FP128$STRT
    	db	000h,000h,000h,000h,000h,000h,000h,000h
	db	000h,000h,000h,000h,000h,000h,000h,080h
	dw	biasexp+002h
	db	000h
FP128$L$NUM3		EQU	$-FP128$STRT
    	db	000h,000h,000h,000h,000h,000h,000h,000h
	db	000h,000h,000h,000h,000h,000h,000h,0c0h
	dw	biasexp+002h
	db	000h
FP128$L$NUM4		EQU	$-FP128$STRT
    	db	000h,000h,000h,000h,000h,000h,000h,000h
	db	000h,000h,000h,000h,000h,000h,000h,080h
	dw	biasexp+003h
	db	000h
FP128$L$NUM5		EQU	$-FP128$STRT
    	db	000h,000h,000h,000h,000h,000h,000h,000h
	db	000h,000h,000h,000h,000h,000h,000h,0a0h
	dw	biasexp+003h
	db	000h
FP128$L$NUM6		EQU	$-FP128$STRT
    	db	000h,000h,000h,000h,000h,000h,000h,000h
	db	000h,000h,000h,000h,000h,000h,000h,0c0h
	dw	biasexp+003h
	db	000h
FP128$L$NUM7		EQU	$-FP128$STRT
    	db	000h,000h,000h,000h,000h,000h,000h,000h
	db	000h,000h,000h,000h,000h,000h,000h,0e0h
	dw	biasexp+003h
	db	000h
FP128$L$NUM8		EQU	$-FP128$STRT
    	db	000h,000h,000h,000h,000h,000h,000h,000h
	db	000h,000h,000h,000h,000h,000h,000h,080h
	dw	biasexp+004h
	db	000h
FP128$L$NUM9		EQU	$-FP128$STRT
    	db	000h,000h,000h,000h,000h,000h,000h,000h
	db	000h,000h,000h,000h,000h,000h,000h,090h
	dw	biasexp+004h
	db	000h

FP128$L$PAI		EQU	$-FP128$STRT
   	db	000h,020h,0dah,080h,08bh,062h,0c6h,0c4h
	db	034h,0c2h,068h,021h,0a2h,0dah,00fh,0c9h
	dw	biasexp+002h
	db	000h
FP128$L$PAI2		EQU	$-FP128$STRT
    	db	000h,020h,0dah,080h,08bh,062h,0c6h,0c4h
	db	034h,0c2h,068h,021h,0a2h,0dah,00fh,0c9h
	dw	biasexp+003h
	db	000h
FP128$L$PAID2		EQU	$-FP128$STRT
     	db	000h,020h,0dah,080h,08bh,062h,0c6h,0c4h
	db	034h,0c2h,068h,021h,0a2h,0dah,00fh,0c9h
	dw	biasexp+001h
	db	000h
FP128$L$PAID4		EQU	$-FP128$STRT
     	db	000h,020h,0dah,080h,08bh,062h,0c6h,0c4h
	db	034h,0c2h,068h,021h,0a2h,0dah,00fh,0c9h
	dw	biasexp+000h
	db	000h

FP128$END	EQU	$
		DW	385
		DW	FP128$R000
		DW	FP128$R001
		DW	FP128$R002
		DW	FP128$R003
		DW	FP128$R004
		DW	FP128$R005
		DW	FP128$R006
		DW	FP128$R007
		DW	FP128$R008
		DW	FP128$R009
		DW	FP128$R010
		DW	FP128$R011
		DW	FP128$R012
		DW	FP128$R013
		DW	FP128$R014
		DW	FP128$R015
		DW	FP128$R016
		DW	FP128$R017
		DW	FP128$R018
		DW	FP128$R019
		DW	FP128$R020
		DW	FP128$R021
		DW	FP128$R022
		DW	FP128$R023
		DW	FP128$R024
		DW	FP128$R025
		DW	FP128$R026
		DW	FP128$R027
		DW	FP128$R028
		DW	FP128$R029
		DW	FP128$R030
		DW	FP128$R031
		DW	FP128$R032
		DW	FP128$R033
		DW	FP128$R034
		DW	FP128$R035
		DW	FP128$R036
		DW	FP128$R037
		DW	FP128$R038
		DW	FP128$R039
		DW	FP128$R040
		DW	FP128$R041
		DW	FP128$R042
		DW	FP128$R043
		DW	FP128$R044
		DW	FP128$R045
		DW	FP128$R046
		DW	FP128$R047
		DW	FP128$R048
		DW	FP128$R049
		DW	FP128$R050
		DW	FP128$R051
		DW	FP128$R052
		DW	FP128$R053
		DW	FP128$R054
		DW	FP128$R055
		DW	FP128$R056
		DW	FP128$R057
		DW	FP128$R058
		DW	FP128$R059
		DW	FP128$R060
		DW	FP128$R061
		DW	FP128$R062
		DW	FP128$R063
		DW	FP128$R064
		DW	FP128$R065
		DW	FP128$R066
		DW	FP128$R067
		DW	FP128$R068
		DW	FP128$R069
		DW	FP128$R070
		DW	FP128$R071
		DW	FP128$R072
		DW	FP128$R073
		DW	FP128$R074
		DW	FP128$R075
		DW	FP128$R076
		DW	FP128$R077
		DW	FP128$R078
		DW	FP128$R079
		DW	FP128$R080
		DW	FP128$R081
		DW	FP128$R082
		DW	FP128$R083
		DW	FP128$R084
		DW	FP128$R085
		DW	FP128$R086
		DW	FP128$R087
		DW	FP128$R088
		DW	FP128$R089
		DW	FP128$R090
		DW	FP128$R091
		DW	FP128$R092
		DW	FP128$R093
		DW	FP128$R094
		DW	FP128$R095
		DW	FP128$R096
		DW	FP128$R097
		DW	FP128$R098
		DW	FP128$R099
		DW	FP128$R100
		DW	FP128$R101
		DW	FP128$R102
		DW	FP128$R103
		DW	FP128$R104
		DW	FP128$R105
		DW	FP128$R106
		DW	FP128$R107
		DW	FP128$R108
		DW	FP128$R109
		DW	FP128$R110
		DW	FP128$R111
		DW	FP128$R112
		DW	FP128$R113
		DW	FP128$R114
		DW	FP128$R115
		DW	FP128$R116
		DW	FP128$R117
		DW	FP128$R118
		DW	FP128$R119
		DW	FP128$R120
		DW	FP128$R121
		DW	FP128$R122
		DW	FP128$R123
		DW	FP128$R124
		DW	FP128$R125
		DW	FP128$R126
		DW	FP128$R127
		DW	FP128$R128
		DW	FP128$R129
		DW	FP128$R130
		DW	FP128$R131
		DW	FP128$R132
		DW	FP128$R133
		DW	FP128$R134
		DW	FP128$R135
		DW	FP128$R136
		DW	FP128$R137
		DW	FP128$R138
		DW	FP128$R139
		DW	FP128$R140
		DW	FP128$R141
		DW	FP128$R142
		DW	FP128$R143
		DW	FP128$R144
		DW	FP128$R145
		DW	FP128$R146
		DW	FP128$R147
		DW	FP128$R148
		DW	FP128$R149
		DW	FP128$R150
		DW	FP128$R151
		DW	FP128$R152
		DW	FP128$R153
		DW	FP128$R154
		DW	FP128$R155
		DW	FP128$R156
		DW	FP128$R157
		DW	FP128$R158
		DW	FP128$R159
		DW	FP128$R160
		DW	FP128$R161
		DW	FP128$R162
		DW	FP128$R163
		DW	FP128$R164
		DW	FP128$R165
		DW	FP128$R166
		DW	FP128$R167
		DW	FP128$R168
		DW	FP128$R169
		DW	FP128$R170
		DW	FP128$R171
		DW	FP128$R172
		DW	FP128$R173
		DW	FP128$R174
		DW	FP128$R175
		DW	FP128$R176
		DW	FP128$R177
		DW	FP128$R178
		DW	FP128$R179
		DW	FP128$R180
		DW	FP128$R181
		DW	FP128$R182
		DW	FP128$R183
		DW	FP128$R184
		DW	FP128$R185
		DW	FP128$R186
		DW	FP128$R187
		DW	FP128$R188
		DW	FP128$R189
		DW	FP128$R190
		DW	FP128$R191
		DW	FP128$R192
		DW	FP128$R193
		DW	FP128$R194
		DW	FP128$R195
		DW	FP128$R196
		DW	FP128$R197
		DW	FP128$R198
		DW	FP128$R199
		DW	FP128$R200
		DW	FP128$R201
		DW	FP128$R202
		DW	FP128$R203
		DW	FP128$R204
		DW	FP128$R205
		DW	FP128$R206
		DW	FP128$R207
		DW	FP128$R208
		DW	FP128$R209
		DW	FP128$R210
		DW	FP128$R211
		DW	FP128$R212
		DW	FP128$R213
		DW	FP128$R214
		DW	FP128$R215
		DW	FP128$R216
		DW	FP128$R217
		DW	FP128$R218
		DW	FP128$R219
		DW	FP128$R220
		DW	FP128$R221
		DW	FP128$R222
		DW	FP128$R223
		DW	FP128$R224
		DW	FP128$R225
		DW	FP128$R226
		DW	FP128$R227
		DW	FP128$R228
		DW	FP128$R229
		DW	FP128$R230
		DW	FP128$R231
		DW	FP128$R232
		DW	FP128$R233
		DW	FP128$R234
		DW	FP128$R235
		DW	FP128$R236
		DW	FP128$R237
		DW	FP128$R238
		DW	FP128$R239
		DW	FP128$R240
		DW	FP128$R241
		DW	FP128$R242
		DW	FP128$R243
		DW	FP128$R244
		DW	FP128$R245
		DW	FP128$R246
		DW	FP128$R247
		DW	FP128$R248
		DW	FP128$R249
		DW	FP128$R250
		DW	FP128$R251
		DW	FP128$R252
		DW	FP128$R253
		DW	FP128$R254
		DW	FP128$R255
		DW	FP128$R256
		DW	FP128$R257
		DW	FP128$R258
		DW	FP128$R259
		DW	FP128$R260
		DW	FP128$R261
		DW	FP128$R262
		DW	FP128$R263
		DW	FP128$R264
		DW	FP128$R265
		DW	FP128$R266
		DW	FP128$R267
		DW	FP128$R268
		DW	FP128$R269
		DW	FP128$R270
		DW	FP128$R271
		DW	FP128$R272
		DW	FP128$R273
		DW	FP128$R274
		DW	FP128$R275
		DW	FP128$R276
		DW	FP128$R277
		DW	FP128$R278
		DW	FP128$R279
		DW	FP128$R280
		DW	FP128$R281
		DW	FP128$R282
		DW	FP128$R283
		DW	FP128$R284
		DW	FP128$R285
		DW	FP128$R286
		DW	FP128$R287
		DW	FP128$R288
		DW	FP128$R289
		DW	FP128$R290
		DW	FP128$R291
		DW	FP128$R292
		DW	FP128$R293
		DW	FP128$R294
		DW	FP128$R295
		DW	FP128$R296
		DW	FP128$R297
		DW	FP128$R298
		DW	FP128$R299
		DW	FP128$R300
		DW	FP128$R301
		DW	FP128$R302
		DW	FP128$R303
		DW	FP128$R304
		DW	FP128$R305
		DW	FP128$R306
		DW	FP128$R307
		DW	FP128$R308
		DW	FP128$R309
		DW	FP128$R310
		DW	FP128$R311
		DW	FP128$R312
		DW	FP128$R313
		DW	FP128$R314
		DW	FP128$R315
		DW	FP128$R316
		DW	FP128$R317
		DW	FP128$R318
		DW	FP128$R319
		DW	FP128$R320
		DW	FP128$R321
		DW	FP128$R322
		DW	FP128$R323
		DW	FP128$R324
		DW	FP128$R325
		DW	FP128$R326
		DW	FP128$R327
		DW	FP128$R328
		DW	FP128$R329
		DW	FP128$R330
		DW	FP128$R331
		DW	FP128$R332
		DW	FP128$R333
		DW	FP128$R334
		DW	FP128$R335
		DW	FP128$R336
		DW	FP128$R337
		DW	FP128$R338
		DW	FP128$R339
		DW	FP128$R340
		DW	FP128$R341
		DW	FP128$R342
		DW	FP128$R343
		DW	FP128$R344
		DW	FP128$R345
		DW	FP128$R346
		DW	FP128$R347
		DW	FP128$R348
		DW	FP128$R349
		DW	FP128$R350
		DW	FP128$R351
		DW	FP128$R352
		DW	FP128$R353
		DW	FP128$R354
		DW	FP128$R355
		DW	FP128$R356
		DW	FP128$R357
		DW	FP128$R358
		DW	FP128$R359
		DW	FP128$R360
		DW	FP128$R361
		DW	FP128$R362
		DW	FP128$R363
		DW	FP128$R364
		DW	FP128