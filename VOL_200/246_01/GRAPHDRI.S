	.ttl	GRAPHIC SUPPORT ROUTINES (DRI-C)

*	(EHR)	851215, 860824

bdos		equ	2
lstout		equ	5


	.globl	_pputc,_bitset,_odd
	.text



_pputc:
	link	a6,#-4			* stack frame
	movem.l	d1-d7/a0-a6,-(sp)	* save regs
	move.w	8(a6),d1		* parameter (char)
	and.w	#$00ff,d1		* undo any sign extension
	moveq	#lstout,d0
	trap	#bdos			* send char to lst:
	movem.l	(sp)+,d1-d7/a0-a6	* restore regs
	unlk	a6
	rts

_bitset:
	move.l	4(sp),a0		* pointer
	moveq	#0,d0
	move.b	9(sp),d0		* bit #
	bset	d0,(a0)			* set bit
	rts

_odd:
	moveq	#1,d0			* mask
	and.b	5(sp),d0		* clr d0 if even
	rts
                                 