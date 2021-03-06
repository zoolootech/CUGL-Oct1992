	.ttl	fpcmp.s

* MC68020/MC68881 IEEE Format Single Precision Routines
*
* Written by: Edmund H. Ramm
*
* dbcmp: compares two single precision numbers: "x" and "y"
* Input: 4(sp)   first  single precision number  ("x")
*        8(sp)   second single precision number  ("y")
*
*	 sp     4      8
*	----------------------
*	| ret  |  x   |   y  |
*
* Output: d0 =  0 if x == y
*            =  1 if x > y
*            = -1 if x < y
*
* Function:
* This function compares two single precision numbers and returns the
* result of this comparison in d0
*

	.text
	.globl	fpcmp,_fpcmp


fpcmp:
_fpcmp:
~~fpcmp:
*	fmove.s	8(sp),fp0		* fp0 <-- "y"
	dc.w	$f22f,$4400,$0008
*	fcmp.s	4(sp),fp0		* compare with "x"
	dc.w	$f22f,$4438,$0004
	moveq	#0,d0			* assume equal
*	fbeq	L_dbcmp1		* return with d0 = 0
	dc.w	$f281,$000a
	moveq	#1,d0			* assume x > y
*	fbolt	L_dbcmp1		* return with d0 = 1
	dc.w	$f284,$0004
	moveq	#-1,d0			* must be x < y
L_dbcmp1:
	rts

	.end
                         