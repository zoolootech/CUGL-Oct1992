	.ttl	"fsqr.s"

* MC68020/MC68881 IEEE Format Single Precision Routines
*
* Written by: Edmund H. Ramm
*
* _fsqr entry point for single prec. square, operand in 4(sp)
*
*	 sp     4
*	---------------
*	| ret  |  x   |


	.text
	.globl	_fsqr


_fsqr:
*
* on exit, d0 = fsqr( 4(sp) )
*
*	fmove.s	4(sp),fp0		* fp0 <-- operand
	dc.w	$f22f,$4400,$0004
*	fmul.x	fp0,fp0			* square operand
	dc.w	$f200,$0023
*	fmove.s	fp0,d0			* d0 <-- square
	dc.w	$f200,$6400
	rts

	.end
                