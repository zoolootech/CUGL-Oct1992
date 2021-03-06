	.ttl	"tan.s"

* MC68020/MC68881 IEEE Format Single Precision Routines
*
* Written by: Edmund H. Ramm
*
* _tan entry point for single prec. tangent, operand in 4(sp)
*
*	 sp     4
*	---------------
*	| ret  |   x  |



	.text
	.globl	_tan


_tan:
*
* on exit, d0 = tan( 4(sp) )
*
*	ftan.s	4(sp),fp0		* fp0 <-- tan( 4(sp) )
	dc.w	$f22f,$440f,$0004
*	fmove.s	fp0,d0			* d0 <-- tangent
	dc.w	$f200,$6400
	rts

	.end
                                                                     