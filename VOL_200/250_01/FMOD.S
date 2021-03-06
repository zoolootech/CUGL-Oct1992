	.ttl	"fmod.s"

* MC68020/MC68881 IEEE Format Single Precision Routines
*
* Written by: Edmund H. Ramm
*
* _fmod entry point for s.p. modulo, ops. in 4(sp) and 8(sp)
*
*	 sp     4        8
*	-------------------------
*	| ret  |dividend|divisor|


	.text
	.globl	_fmod


_fmod:
*
*	on exit, d0 = fmod( 4(sp), 8(sp) )
*	remainder = dividend fmod divisor
*
*	fmove.s	4(sp),fp0		* fp0 <-- dividend
	dc.w	$f22f,$4400,$0004
*	fmod.s	8(sp),fp0		* fp0 <-- 4(sp) fmod 8(sp)
	dc.w	$f22f,$4421,$0008
*	fmove.s	fp0,d0			* d0 <-- remainder
	dc.w	$f200,$6400
	rts

	.end
                                                   