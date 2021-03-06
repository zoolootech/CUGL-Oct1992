	.ttl	"sqrt.s"

* MC68020/MC68881 IEEE Format Single Precision Routines
*
* Written by: Edmund H. Ramm
*
* _sqrt entry point for single prec. square root, operand in 4(sp)
*
*	 sp     4
*	---------------
*	| ret  |  x   |


	.text
	.globl	_sqrt


_sqrt:
*
*	on exit, d0 = sqrt( 4(sp) )
*
*	fsqrt.s	4(sp),fp0		* fp0 <-- sqrt( 4(sp) )
	dc.w	$f22f,$4404,$0004
*	fmove.s	fp0,d0			* d0 <-- square root
	dc.w	$f200,$6400
	rts

	.end
                                                        