	.ttl	"cos.s"

* MC68020/MC68881 IEEE Format Single Precision Routines
*
* Written by: Edmund H. Ramm
*
* _cos entry point for single prec. cosine, operand in 4(sp)
*
*	 sp     4
*	---------------
*	| ret  |  x   |


	.text
	.globl	_cos


_cos:
*
* on exit, d0 = cos( 4(sp) )
*
*	fcos.s	4(sp),fp0		* fp0 <-- cos( 4(sp) )
	dc.w	$f22f,$441d,$0004
*	fmove.s	fp0,d0			* d0 <-- cosine
	dc.w	$f200,$6400
	rts

	.end
                                                                         