;				charfun.asm
;
;	Copyright (C) 1980, M J Maney
;
;	First created 8/16/80
;	Last revised 8/28/80 14:45
;
;	Tested and installed 8/28/80 14:55
;
;	This file contains the definitions for some character
;	functions, written in assembler for compacteness and performance.
;	The macros from the "crl.lib" file are used to create these
;	functions in the BDS "crl" format with minimum pain.
;
	maclib	crl
;
;
BLANK	equ	20H
TAB	equ	09H
NEWLINE	equ	0AH
;
;
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;
;		functions that operate on a byte and return
;			a small integer or a character
;
;	isalpha		isupper		islower		isdigit
;	isspace		toupper		tolower
;
;		functions that operate on a pair of bytes and
;			return an integer
;
;	atob
;
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;
;
	PROC	ISALPHA
	lda	ARG1
	lxi	h,0
	cpi	'A'
	rc			;less than 'A'
	cpi	'z'+1
	rnc			;greater than 'z'
	inx	h
	cpi	'a'
	rnc			;lower-case alpha, return TRUE
	cpi	'Z'+1
	rc			;upper-case alpha, return TRUE
	dcx	h
	ret			;else return FALSE
	PEND	ISALPHA
;
;
	PROC	ISUPPER
	lda	ARG1
	lxi	h,0
	cpi	'A'
	rc			;less than 'A'
	cpi	'Z'+1
	rnc			;greater than 'Z'
	inx	h
	ret			;else is upper-case
	PEND	ISUPPER
;
;
	PROC	ISLOWER
	lda	ARG1
	lxi	h,0
	cpi	'a'
	rc			;less than 'a'
	cpi	'z'+1
	rnc			;greater than 'z'
	inx	h
	ret			;else is lower case
	PEND	ISLOWER
;
;
	PROC	ISDIGIT
	lda	ARG1
	lxi	h,0
	cpi	'0'
	rc			;less than '0'
	cpi	'9'+1
	rnc			;greater than '9'
	inx	h
	ret			;else is decimal digit
	PEND	ISDIGIT
;
;
	PROC	ISSPACE
	lda	ARG1
	lxi	h,1
	cpi	BLANK
	rz			;equals ' '
	cpi	TAB
	rz			;equals tab
	cpi	NEWLINE
	rz			;equals newline
	dcx	h
	ret			;else is non-space
	PEND	ISSPACE
;
;
	PROC	TOUPPER
	lda	ARG1
	mov	l,a
	mvi	h,0		;setup to return character unchanged
	cpi	'a'
	rc			;less than 'a'
	cpi	'z'+1
	rnc			;greater than 'z'
	adi	'A'-'a'		;else is lower-case, convert it
	mov	l,a
	ret
	PEND	TOUPPER
;
;
	PROC	TOLOWER
	lda	ARG1
	mov	l,a
	mvi	h,0		;setup to return character unchanged
	cpi	'A'
	rc			;less than 'A'
	cpi	'Z'+1
	rnc			;greater than 'Z'
	adi	'a'-'A'		;else is upper-case, convert it
	mov	l,a
	ret
	PEND	TOLOWER
;
;
	PROC	ATOB
	lda	ARG1
	sbi	'0'
	BC	nogood		;illegal char below '0'
	cpi	10
	BC	gotit		;character between '0' and '9'
	sbi	'A' - '0'
	BC	nogood		;illegal char between '9' and 'A'
	cpi	26
	BC	gotalph		;character between 'A' and 'Z'
	sbi	'a' - 'A'
	BC	nogood		;illegal char between 'Z' and 'a'
	cpi	26
	BNC	nogood		;illegal char above 'z'
gotalph:;have valid alpha, must be offset by 10
	adi	10
gotit:	;have valid alpha-numeric char, converted value in A
	lxi	h,ARG2
	cmp	m
	BNC	nogood		;illegal char for specified base
	mov	l,a
	mvi	h,0
	ret
;
nogood:	;illegal character or out of range for specified base
	lxi	h,-1
	ret
	PEND	ATOB
;
;
	end
