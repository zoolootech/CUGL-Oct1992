	.ttl	"fpmul.s"

* MC68020/MC68881 IEEE Format Single Precision Routines
*
* Written by: Edmund H. Ramm
*
* fpmul entry for single prec. multiplication, operands in 4(sp) and 8(sp)
*
*	 sp     4      8
*	----------------------
*	| ret  |  a   |   b  |

	.text
	.globl	fpmul,_fpmul,fpmult,_fpmult


fpmul:
_fpmul:
fpmult:
_fpmult:
~~fpmul:
*
*	on exit, d0 = 4(sp) * 8(sp)
*
*	fmove.s	4(sp),fp0		* fp0 <-- "a"
	dc.w	$f22f,$4400,$0004
*	fsglmul.s	8(sp),fp0	* fp0 <-- "a" * "b"
	dc.w	$f22f,$4427,$0008
*	fmove.s	fp0,d0			* d0 <-- product
	dc.w	$f200,$6400
	rts

	.end
                                          