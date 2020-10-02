/*
	pass 2 writes out those objects in the page list flagged 
	for WRITE and clears all flags except the BLANKED flag.
*/
	#ASM
FOROFF	EQU	0
NBYTES	EQU	32
FLGOFF	EQU	2*4
;pass2(mscan,pgscan,pscrn)
;int	*mscan,*pgscan,*pscrn;
;{
;int	pgflag;
pass2:
	ldx	4,s
	ldy	2,s	; x ->pgscan , y-> screen
p2wait:
	ldd	SCRADR
	bne	p2wait	; If display is still on previous page
pass20:
;	while(pgscan[FORWARD] !=NULL) {
	ldd	FOROFF,x
	cmpd	#0
	beq	pass29
;		pgflag=pgscan[FLAGS];
		ldd	FLGOFF,x
;		if(WRITE&pgflag) {
		andb	#32
		beq	pass21
;			writeobj(pscrn,NBYTES,pgscan[X],pgscan[Y],pgscan[OBJPTR]);
			pshs	x,y	; save pointers
			pshs	y
			ldd	#32
			pshs	d
			ldd	XOFF,x
			pshs	d
			ldd	YOFF,x
			pshs	d
			ldd	OBJOFF,x
			pshs	d
			jsr	writeobj
			leas	10,s
			puls	x,y
;	}
pass21:
;		pgscan[FLAGS]=pgflag&BLANKED;
		ldd	FLGOFF,x
		andb	#1		; mask off all except blanked
		std	FLGOFF,x
;		pgscan=pgscan[FORWARD];
		ldx	FOROFF,x
;	}
		bra	pass20
;}
pass29:
		rts
	#ENDASM
Õ„