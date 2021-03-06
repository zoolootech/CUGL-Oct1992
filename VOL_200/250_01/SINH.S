	.ttl	"sinh.s"

* MC68020/MC68881 IEEE Format Single Precision Routines
*
* Written by: Edmund H. Ramm
*
* _sinh entry point for single prec. hyperbolic sine, operand in 4(sp)
*
*	 sp     4
*	---------------
*	| ret  |  x   |


	.text
	.globl	_sinh


_sinh:
*
*	on exit, d0 = sinh( 4(sp) )
*
*	fsinh.s	4(sp),fp0		* fp0 <-- sinh( 4(sp) )
	dc.w	$f22f,$4402,$0004
*	fmove.s	fp0,d0			* d0 <-- hyp. sine
	dc.w	$f200,$6400
	rts

	.end
                                                      