	.ttl	"asin.s"

* MC68020/MC68881 IEEE Format Single Precision Routines
*
* Written by: Edmund H. Ramm
*
* _asin entry point for single prec. arc sine, operand in 4(sp)
*
*	 sp     4
*	---------------
*	| ret  |  x   |


	.text
	.globl	_asin


_asin:
*
* on exit, d0 = asin( 4(sp) )
*
*	fasin.s	4(sp),fp0		* fp0 <-- asin( 4(sp) )
	dc.w	$f22f,$440c,$0004
*	fmove.d	fp0,d0			* d0 <-- arc sine
	dc.w	$f200,$6400
	rts

	.end
                                                              