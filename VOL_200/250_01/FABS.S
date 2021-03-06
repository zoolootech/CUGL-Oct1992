	.ttl	"fabs.s"

* MC68020/MC68881 IEEE Format Single Precision Routines
*
* Written by: Edmund H. Ramm
*
* _fabs entry point for single prec. absolute, operand in 4(sp)
*
*	 sp     4
*	---------------
*	| ret  |  x   |


	.text
	.globl	_fabs


_fabs:
*
* on exit, d0 = fabs( 4(sp) )
*
*	fabs.s	4(sp),fp0		* fp0 <-- abs( 4(sp) )
	dc.w	$f22f,$4418,$0004
*	fmove.s	fp0,d0			* d0 <-- abs(x)
	dc.w	$f200,$6400
	rts

	.end
                                                                  