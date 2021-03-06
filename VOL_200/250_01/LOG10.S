	.ttl	"log10.s"

* MC68020/MC68881 IEEE Format Single Precision Routines
*
* Written by: Edmund H. Ramm
*
* _log10 entry point for single prec. dec. logarithm, operand in 4(sp)
*
*	 sp     4
*	---------------
*	| ret  |  x   |


	.text
	.globl	_log10


_log10:
*
*	on exit, d0 = log10( 4(sp) )
*
*	flog10.s	4(sp),fp0	* fp0 <-- log10( 4(sp) )
	dc.w	$f22f,$4415,$0004
*	fmove.s	fp0,d0			* d0 <-- dec. log
	dc.w	$f200,$6400
	rts

	.end
                                                  