	.ttl	"acos.s"

* MC68020/MC68881 IEEE Format Single Precision Routines
*
* Written by: Edmund H. Ramm
*
* _acos entry point for single prec. arc cosine, operand in 4(sp)
*
*	 sp     4
*	---------------
*	| ret  |  x   |



	.text
	.globl	_acos


_acos:
*
* on exit, d0 = acos( 4(sp) )
*
*	facos.s	4(sp),fp0		* fp0 <-- acos( 4(sp) )
	dc.w	$f22f,$441c,$0004
*	fmove.s	fp0,d0			* d0 <-- arc cosine
	dc.w	$f200,$6400
	rts

	.end
                                                        