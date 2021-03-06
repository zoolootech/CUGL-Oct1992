	.ttl	"cosh.s"

* MC68020/MC68881 IEEE Format Single Precision Routines
*
* Written by: Edmund H. Ramm
*
* _cosh entry point for single prec. hyperbolic cosine, operand in 4(sp)
*
*	 sp     4
*	---------------
*	| ret  |  x   |


	.text
	.globl	_cosh


_cosh:
*
* on exit, d0 = cosh( 4(sp) )
*
*	fcosh.s	4(sp),fp0		* fp0 <-- cosh( 4(sp) )
	dc.w	$f22f,$4419,$0004
*	fmove.s	fp0,d0			* d0 <-- hyp. cosine
	dc.w	$f200,$6400
	rts

	.end
                                                  