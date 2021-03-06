	.ttl	"ceil.s"

* MC68020/MC68881 IEEE Format Single Precision Routines
*
* Written by: Edmund H. Ramm
*
* _ceil entry point for single prec. ceil, operand in 4(sp)
*
*	 sp     4
*	---------------
*	| ret  |  x   |


	.text
	.globl	_ceil


_ceil:
*
* on exit, d0 = ceil( 4(sp) )
*
	moveq	#$30,d1
*	fmove.l	d1,fpcr			* select round to +infinity
	dc.w	$f201,$9000
*	fint.s	4(sp),fp0		* fp0 <-- ceil( 4(sp) )
	dc.w	$f22f,$4401,$0004
*	fmove.s	fp0,d0			* d0 <-- ceil(x)
	dc.w	$f200,$6400
	moveq	#0,d1
*	fmove.l	d1,fpcr			* re-select ieee defaults
	dc.w	$f201,$9000
	rts

	.end
                                