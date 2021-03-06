	.ttl	"tanh.s"

* MC68020/MC68881 IEEE Format Single Precision Routines
*
* Written by: Edmund H. Ramm
*
* _tanh entry point for single prec. hyperbolic tangent, op. in 4(sp)
*
*	 sp     4
*	---------------
*	| ret  |  x   |


	.text
	.globl	_tanh


_tanh:
*
*	on exit, d0, = tanh( 4(sp) )
*
*	ftanh.s	4(sp),fp0		* fp0 <-- tanh( 4(sp) )
	dc.w	$f22f,$4409,$0004
*	fmove.s	fp0,d0			* d0 <-- hyp. tangent
	dc.w	$f200,$6400
	rts

	.end
                                                   