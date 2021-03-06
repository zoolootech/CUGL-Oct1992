	.ttl	"atan.s"

* MC68020/MC68881 IEEE Format Single Precision Routines
*
* Written by: Edmund H. Ramm
*
* _atan entry point for single prec. arc tangent, operand in 4(sp)
*
*	 sp     4
*	---------------
*	| ret  |  x   |



	.text
	.globl	_atan


_atan:
*
* on exit, d0 = atan( 4(sp) )
*
*	fatan.s	4(sp),fp0		* fp0 <-- atan( 4(sp) )
	dc.w	$f22f,$440a,$0004
*	fmove.s	fp0,d0			* d0 <-- arc tangent
	dc.w	$f200,$6400
	rts

	.end
                                                      