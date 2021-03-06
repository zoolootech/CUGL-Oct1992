	.ttl	"fpadd.s"

* MC68020/MC68881 IEEE Format Single Precision Routines
*
* Written by: Edmund H. Ramm
*
* fpadd entry point for single prec. addition, operands in 4(sp) and 8(sp)
*
*	 sp     4      8
*	----------------------
*	| ret  |  a   |   b  |


	.text
	.globl	fpadd,_fpadd


fpadd:
_fpadd:
~~fpadd:
*
* on exit, d0 = 4(sp) + 8(sp)
*
*	fmove.s	4(sp),fp0		* fp0 <-- "a"
	dc.w	$f22f,$4400,$0004
*	fadd.s	8(sp),fp0		* fp0 <-- "a" + "b"
	dc.w	$f22f,$4422,$0008
*	fmove.s	fp0,d0			* d0 <-- sum
	dc.w	$f200,$6400
	rts

	.end
                                                                                