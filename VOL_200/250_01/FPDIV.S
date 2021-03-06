	.ttl	"fpdiv.s"

* MC68020/MC68881 IEEE Format Single Precision Routines
*
* Written by: Edmund H. Ramm
*
* fpdiv entry point for single prec. div, operands in 4(sp) and 8(sp)
*
*	 sp     4      8
*	----------------------
*	| ret  |  a   |   b  |


	.text
	.globl	fpdiv,_fpdiv


fpdiv:
_fpdiv:
~~fpdiv:
*
*	on exit, d0 = 4(sp) / 8(sp)
*	quotient = dividend / divisor
*	c = a / b
*
*	fmove.s	4(sp),fp0		* fp0 <-- "a"
	dc.w	$f22f,$4400,$0004
*	fsgldiv.s	8(sp),fp0	* divide by "b"
	dc.w	$f22f,$4424,$0008
*	fmove.s	fp0,d0			* d0 <-- quotient
	dc.w	$f200,$6400
	rts

	.end
                                    