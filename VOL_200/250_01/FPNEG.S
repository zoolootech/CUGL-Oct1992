	.ttl	"fabs.s"

* MC68020/MC68881 IEEE Format Single Precision Routines
*
* Written by: Edmund H. Ramm
*
* _fpneg entry point for single prec. negation, operand in 4(sp)
*
*	 sp     4
*	---------------
*	| ret  |  x   |


	.text
	.globl	_fpneg,fpneg


_fpneg:
fpneg:
*
* on exit, d0 = -( 4(sp) )
*
	move.l	4(sp),d0		* d0 <-- operand
	bchg	#31,d0			* invert mantissa's sign
	rts				* faster than MC68881's fneg

	.end
                                                                   