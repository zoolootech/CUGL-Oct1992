	.ttl	"sin.s"

* MC68020/MC68881 IEEE Format Single Precision Routines
*
* Written by: Edmund H. Ramm
*
* _sin entry point for single prec. sine, operand in 4(sp)
*
*	 sp     4
*	---------------
*	| ret  |  x   |


	.text
	.globl	_sin


_sin:
*
*	on exit, d0 = sin( 4(sp) )
*
*	fsin.s	4(sp),fp0		* fp0 <-- sin( 4(sp) )
	dc.w	$f22f,$440e,$0004
*	fmove.d	fp0,d0			* d0 <-- sine
	dc.w	$f200,$6400
	rts

	.end
                                                                             