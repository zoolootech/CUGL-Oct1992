	.ttl	"exp.s"

* MC68020/MC68881 IEEE Format Single Precision Routines
*
* Written by: Edmund H. Ramm
*
* _exp entry point for single prec. e^x, operand in 4(sp)
*
*	 sp     4
*	---------------
*	| ret  |  x   |


	.text
	.globl	_exp


_exp:
*
* on exit, d0 = e^( 4(sp) )
*
*	fetox.s	4(sp),fp0		* fp0 <-- exp( 4(sp) )
	dc.w	$f22f,$4410,$0004
*	fmove.s	fp0,d0			* d0 <-- e to x
	dc.w	$f200,$6400
	rts

	.end
                                                                            