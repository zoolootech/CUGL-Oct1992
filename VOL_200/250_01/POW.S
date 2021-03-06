	.ttl	"pow.s"

* MC68020/MC68881 IEEE Format Single Precision Routines
*
* Written by: Edmund H. Ramm
*
* _pow entry point for s.p. x^y, ops. in 4(sp) and 8(sp)
*
*	 sp     4        8
*	--------------------------
*	| ret  |mantissa|exponent|


	.text
	.globl	_pow


_pow:
*
*	on exit, d0 = pow( 4(sp), 8(sp) )

	moveq	#0,d0
	btst	#7,4(sp)		* negative mantissa?
	bne.s	L_pow1			* yes, exit with result = 0.0
*	flogn.s	4(sp),fp0		* fp0 <-- log(mantissa)
	dc.w	$f22f,$4414,$0004
*	fmul.s	8(sp),fp0		* perform x^y
	dc.w	$f22f,$4423,$0008
*	fetox.x	fp0			* fp0 <-- exp(log(x) * y)
	dc.w	$f200,$0010
*	fmove.s	fp0,d0			* d0 <-- result
	dc.w	$f200,$6400
L_pow1:
	rts

	.end
                                                                