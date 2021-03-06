	#ASM
pass0:
	ldx	4,s	; x -> pgscan
	ldy	6,s	; y -> mscan
;pass0(mptr,pgptr,pscrn)
;int *mptr,*pgptr,*pscrn;
;{
;int *mscan,*pgscan;
;int	mflag,pgflag;
	;mscan=mptr;
	;pgscan =pgptr;
pass00:
	ldd	FLGOFF,y	; Test for update required
	bpl	pass05		; If no update
	bita	8,s		; Test for update this page
	beq	pass05		; If not
	eora	8,s		; Turn off the bit
	bita	#60H		; Was this the last update ?
	bne	pass01		; If another page
	anda	#7FH		; Mask out the master update flag
pass01:
	std	FLGOFF,Y	; Restore master page flags
	ldd	OBJOFF,Y
	cmpd	OBJOFF,x	; test for new object
	beq	pass02		; if the same object
		ldd	2,s	; pscrn
		ldu	4,s	; pgptr = ptop
		pshs	x,y
		pshs	y	; mscan
		pshs	x	; pgscan
		pshs	d	; pscrn
		pshs	u	; pgptr = ptop
		bsr	new
		leas	8,s
		puls	x,y	
		bra	pass05
pass02:
	; test for moved object
	ldd	XOFF,y
	cmpd	XOFF,x
	bne	pass04		; moved
	ldd	YOFF,y
	cmpd	YOFF,x
	bne	pass04		; moved
; object is fixed to here
		ldd	2,s	; pscrn
		ldu	4,s	; pgptr
		pshs	x,y
		pshs	y	; mscan
		pshs	x	; pgscan
		pshs	d	; pscrn
		pshs	u	; pgptr
		lbsr	fixed
		leas	8,s
		puls	x,y
		bra	pass05
pass04:	; object has moved to here
	ldd	2,s
	ldu	4,s		; d = pscrn, u = pgptr top
	pshs	x,y
	pshs	y
	pshs	x
	pshs	d
	pshs	u
	lbsr	moved
	leas	8,s
	puls	x,y
pass05:
	ldy	FOROFF,y	; advance mscan  pointer
	beq	pass0x		; if null, exit
	ldx	FOROFF,x	; pgscan advance
	lbne	pass00		; if not = null, continue
pass0x:
	rts			; exit pass0
;new(mptr,pgptr,screen,ptop)
;int	*mptr,*pgptr,*screen,*ptop;
;{
;int	mflag,pgflag;
new:
	ldx	6,s	; x -> pgptr
	ldy	8,s	; y -> mscan
	ldd	FLGOFF,y
	bitb	#1	; test for blanked
	bne	new2	; object is currently off, change at will
		ldd	FLGOFF,x	; pgscan flag
		bitb	#1
		bne	new1		; if pglist off, flag for write
			ldd	4,s	; screen
			pshs	x,y
			pshs	d	; screen
			ldd	#32
			pshs	d	; NBYTEs
			ldd	XOFF,x
			pshs	d
			ldd	YOFF,x
			pshs	d
			ldd	OBJOFF,x
			pshs	d
			lbsr	eraseobj
			leas	10,s
			puls	x,y
			ldd	FLGOFF,x
			orb 	#64		; show erased
			std	FLGOFF,x
new1:
		ldd	FLGOFF,x	; Or in write request
		orb	#32
		std	FLGOFF,x
new2:
	ldd	OBJOFF,y
	std	OBJOFF,x		; new object pointer
	rts				; exit new
	#ENDASM
/*
	fixed	sets the erases and sets for write those objects
	which are both the same and have not moved relative to the
	master list.
*/
	#ASM
;fixed(mscan,pgscan,pscrn,pgptr)
;int	*mscan,*pgscan,*pscrn,*pgptr;
;{
;int	mflag,pgflag;
fixmfg	RMB	2
fixpgf	RMB	2	; Temp for pgflag
fixed:
;	mflag	= mscan[FLAGS];
	ldx	8,s
	ldy	6,s	; Y -> pgscan , X -> mscan
	ldd	FLGOFF,x	; get mflag
	std	fixmfg
	ldd	FLGOFF,y	; get pgflag
	std	fixpgf
;	pgflag	= pgscan[FLAGS];
;	if((mflag&BLANKED)==0){ /* mlist object is on */
	ldd	fixmfg
	andb	#1		; test for blanked
	bne	fixed1		; if object is blanked
;		if(pgflag&BLANKED) {
		ldd	fixpgf
		bitb	#1
		beq	fixed2	; if object is already on
;			pgflag=(pgflag&(~BLANKED))+WRITE;
			orb	#32	; Turn on write
			andb	#0FEH	; Turn off blanked
			std	fixpgf
			bra	fixed2
;		}
;	}
;		/* mlist is blanked, test pglist */
;	else 	if((pgflag&BLANKED)==0) {
fixed1:
	ldd	fixpgf
	bitb	#1		; test for not blanked
	bne	fixed2
;			pgflag=pgflag+BLANKED+ERASE;
		orb	#(1 + 64)
		std	fixpgf
;			eraseobj(pscrn,NBYTES,pgscan[X],pgscan[Y],pgscan[OBJPTR]);
		ldd	4,s	; d = pscrn
		pshs	y	; save pgscan incase it is blown
		pshs	d
		ldd	#32	; Nbytes
		pshs	d
		ldd	XOFF,y
		pshs	d
		ldd	YOFF,y
		pshs	d
		ldd	OBJOFF,y
		pshs	d
		jsr	eraseobj
		leas	10,s
		puls	y
;		}
;	pgscan[FLAGS]=pgflag;
fixed2:
	ldd	fixpgf
	std	FLGOFF,y
;}
	rts
	#ENDASM
/*
	moved 	erases and flags for re-write those objects which
	have moved relative to the master list.
*/
	#ASM
;moved(mscan,pgscan,pscrn,pgptr)
;int	*mscan,*pgscan,*pscrn,*pgptr;
;{
;int	pgflag,mflag;
movmfg	RMB	2
movpfg	RMB	2	; pgflag
moved:
;	pgflag	=pgscan[FLAGS];
	ldx	6,s	; X -> pgscan
	ldy	8,s	; Y -> mscan
	ldd	FLGOFF,x
	std	movpfg
;	mflag	= mscan[FLAGS];
	ldd	FLGOFF,y
	std	movmfg
;	if((mflag&BLANKED) ==0) { /* object is on */
	ldd	movmfg
	bitb	#1
	bne	moved2	; branch if mlist object is off
;		if(pgflag&BLANKED) { /* list object off */
		ldd	movpfg
		bitb	#1
		beq	moved1	; if list object off
;			pgflag=(pgflag&(~BLANKED))+MOVER+WRITE;
			orb	#(128 + 32)
			andb	#0FEH
			std	movpfg
			bra	moved3
;		}
;		else {	pgflag=pgflag+MOVER+WRITE+ERASE;
moved1:
		ldd	movpfg
		orb	#(128 + 64 +32)
		std	movpfg
;			eraseobj(pscrn,NBYTES,pgscan[X],pgscan[Y],pgscan[OBJPTR]);
			ldd	4,s	; pscrn
			pshs	x,y	; save pgscan pointer
			pshs	d	; pscrn on stack
			ldd	#32
			pshs	d
			ldd	XOFF,x
			pshs	d
			ldd	YOFF,x
			pshs	d
			ldd	OBJOFF,x
			pshs	d
			jsr	eraseobj
			leas	10,s
			puls	x,y
			bra	moved3
;		     }
;	}
;		/* mlist object is off */
;	else	if((pgflag&BLANKED)==0) { /* pglist is on */
moved2:
		ldd	movpfg
		bitb	#1
		bne	moved3
;			pgflag=pgflag+ERASE+BLANKED;
			orb	#(64 + 1)
			std	movpfg
;			eraseobj(pscrn,NBYTES,pgscan[X],pgscan[Y],pgscan[OBJPTR]);
			ldd	4,s	; d = pscrn
			pshs	x,y	; save pgscan pointer
			pshs	d
			ldd	#32
			pshs	d
			ldd	XOFF,x
			pshs	d
			ldd	YOFF,x
			pshs	d
			ldd	OBJOFF,x
			pshs	d
			jsr	eraseobj
			leas	10,s
			puls	x,y
			bra	moved3
;		}
moved3:
;	pgscan[X]=mscan[X];
	ldd	XOFF,y
	std	XOFF,x
;	pgscan[Y]=mscan[Y];
	ldd	YOFF,y
	std	YOFF,x
;	pgscan[FLAGS]=pgflag;
	ldd	movpfg
	std	FLGOFF,x
;}
	rts
	#ENDASM
h*>A�h*:9�*>V2K3Ë*>S�h*>V�h*�>Z�v*G!K3> ��\pË*2K3��"9�"9�"9>29�\��2�(2�(2�(> 2�(2�(>�2