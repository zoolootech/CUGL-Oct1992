	.ttl	"log.s"

* MC68020/MC68881 IEEE Format Single Precision Routines
*
* Written by: Edmund H. Ramm
*
* _log entry point for single prec. nat. logarithm, operand in 4(sp)
*
*	 sp     4
*	---------------
*	| ret  |  x   |


	.text
	.globl	_log


_log:
*
* on exit, d0 = log( 4(sp) )
*
*	flogn.s	4(sp),fp0		* fp0 <-- log( 4(sp) )
	dc.w	$f22f,$4414,$0004
*	fmove.s	fp0,d0			* d0 <-- nat. log
	dc.w	$f200,$6400
	rts

	.end
                                                              