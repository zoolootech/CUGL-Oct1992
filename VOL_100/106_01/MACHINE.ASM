;				machine.asm
;
;	Copyright (C) 1980, M J Maney
;
;	First created 8/28/80
;	Last revised 8/29/80 15:30
;
;	Tested and installed 8/29/80 16:15
;
;	This file contains some assembler functions for BDS C. These functions
;	are assorted little ditties for doing low-level operations such as
;	filling/moving/exchanging blocks of memory.
;
	maclib	crl
;
; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ;
;
;	movmem		xchmem		fillbyte	fillword
;	inp		outp		peek		poke
;	peekw		pokew
;
; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ;
;
;
;	movmem(src,des,n)
;	char *src,*des;
;	uint n;
;
;	Move a block of memory from src to des: the block is n bytes long.
;	This function allows for moves of overlapping blocks by moving in
;	either ascending or descending order as required.
;
	PROC	MOVMEM
	lhld	ARG3
	mov	b,h
	mov	c,l		;n to BC
	lhld	ARG2
	xchg			;des to DE
	lhld	ARG1		;src to HL
	BSR	movmem4		;test for direction to use in moving
	BC	movmem2
movmem1	;want to move from bottom to top
	mov	a,b
	ora	c		;while (n > 0) {
	rz
	mov	a,m
	inx	h
	stax	d
	inx	d		;	*des++ = *src++;
	dcx	b		;	n--;
	BRA	movmem1		;	}
				;return
movmem2	;want to move from top to bottom
	dad	b		;	des += n;
	xchg
	dad	b
	xchg			;	src += n;
movmem3	mov	a,b
	ora	c		;while (n > 0) {
	rz
	dcx	h
	mov	a,m
	dcx	d
	stax	d		;	*--des = *--src;
	dcx	b		;	n--;
	BRA	movmem3		;	}
				;return
movmem4	;a little internal subroutine
	mov	a,h
	cmp	d
	rc
	mov	a,l
	cmp	e
	ret
	PEND	MOVMEM
;
;
;	xchmem(src,des,n)
;	char *src,*des;
;	uint n;
;
;	Exchange the contents of two blocks of memory. NO special checks
;	are done for overlapping blocks, so the result in such a case may
;	be sort of strange. However, xchmem is very careful to handle a
;	count of zero properly.
;
	PROC	XCHMEM
	lhld	ARG3
	mov	b,h
	mov	c,l		;n to BC
	lhld	ARG2
	xchg			;des to DE
	lhld	ARG1		;src to HL
xchmem1	mov	a,b
	ora	c		;while (n > 0) {
	rz
	mov	a,m
	push	psw		;	temp=*src;
	ldax	d
	mov	m,a		;	*src=*des;
	pop	a
	stax	d		;	*des=temp;
	inx	h		;	src++;
	inx	d		;	des++;
	dcx	b		;	n--;
	BRA	xchmem1		;	}
	PEND	XCHMEM
;
;
;	fillbyte(data,des,n)
;	char data,*des;
;	uint n;
;
;	Fill n bytes of memory, starting at des, with data. The case of
;	a zero count is properly handled.
;
	PROC	FILLBYTE
	lhld	ARG3
	mov	b,h
	mov	c,l		;n to BC
	lhld	ARG1
	xchg			;data to E
	lhld	ARG2		;des to HL
fillb1	mov	a,b
	ora	c		;while (n > 0) {
	rz
	mov	m,e		;	*des=data;
	inx	h		;	des++;
	dcx	b		;	n--;
	BRA	fillb1		;	}
	PEND	FILLBYTE
;
;
;	fillword(data,des,n)
;	int data,*des;
;	uint n;
;
;	Fill n words of memory, starting at des, with data. Note that 2*n
;	bytes will be filled, and that the case n==0 is properly handled.
;
	PROC	FILLWORD
	lhld	ARG3
	mov	b,h
	mov	c,l		;n to BC
	lhld	ARG1
	xchg			;data to DE
	lhld	ARG2		;des to HL
fillw1:	mov	a,b
	ora	c		;while (n > 0) {
	rz
	mov	m,e
	inx	h
	mov	m,d
	inx	h		;	*des++ = data;
	dcx	b		;	n--;
	BRA	fillw1		;	}
	PEND	FILLWORD
;
;
;	inp(port)
;	char port;
;
;	Returns the value input from I/O port # port. Returned value
;	is an integer with high byte set to zero.
;
	PROC	INP
	lda	ARG1
	STAR	inp1+1
inp1:	in	0		;second byte of op is dynamically set
	mov	l,a
	mvi	h,0
	ret
	PEND	INP
;
;
;	outp(data,port)
;	char data,port;
;
;	Data is output to hardware port # port.
;
	PROC	OUTP
	lda	ARG2
	STAR	outp1+1
	lda	ARG1
outp1:	out	0		;second byte of op is dynamically set
	ret
	PEND	OUTP
;
;
;	peek(adr)
;	char *adr;
;
;	Returns the char pointed to by adr.
;
	PROC	PEEK
	lhld	ARG1
	mov	l,m
	mvi	h,0
	ret
	PEND	PEEK
;
;
;	poke(data,adr)
;	char data,*adr;
;
;	Stores data into memory pointed to by adr.
;
	PROC	POKE
	lda	ARG1
	lhld	ARG2
	mov	m,a
	ret
	PEND	POKE
;
;
;	peekw(adr)
;	int *adr;
;
;	Returns the integer pointed to by adr.
;
	PROC	PEEKW
	lhld	ARG1
	mov	a,m
	inx	h
	mov	h,m
	mov	l,a
	ret
	PEND	PEEKW
;
;
;	pokew(data,adr)
;	int data,*adr;
;
;	Stores data into memory pointed to by adr.
;
	PROC	POKEW
	lhld	ARG1
	xchg
	lhld	ARG2
	mov	m,e
	inx	h
	mov	m,d
	ret
	PEND	POKEW
;
;
	end
