;				iofun.asm
;
;	Copyright (C) 1980, M J Maney
;
;	First created 8/30/80
;	Last revised 9/26/80 15:25
;
;	Tested and installed xx/xx/xx
;
;	Since I/O is such a big part of most programs, it would seem logical
;	to desire your I/O functions to be as efficent as possible. This
;	especially applies to the buffered character I/O that C uses for
;	disk communication. These routines can dominate the run-time of
;	programs that don't do extensive computation, and may affect the
;	speed of some that are rather heavily computational. Thus they are
;	being coded here in assembler.
;*
;*	I have removed the 'overlapping' code reffered to in read.me,
;*	but this is still entirely untested. You have been warned.
;
;
	maclib	crl
;
;
;	getc(iobuf)
;	FILBUF *iobuf;
;
;	This is the basic C character I/O function. It returns one byte from
;	the buffered file described by the FILBUF that iobuf points to,
;	and takes care of the buffer management. The structure of the
;	file buffer is:
;
;	byte offset		what's there
;
;	0,1			file's 'file descriptor', used in calls
;				to read, etc
;	2,3			nleft, the number of bytes left before
;				the buffer is depleted and must be filled
;	4,5			nextp, the pointer into the buffer
;	6,133			buff, the 128 byte buffer
;
;	This function is operationally identical to the C version in the
;	"stdlib1.c" file distributed with the 1.32 release of the BDS
;	compiler, and except for a little re-arrangement at the very end,
;	it is a straight hand translation from that source.
;
	PROC	GETC,<GETCHAR,READ>
	lhld	ARG1
	mov	a,h
	ora	l
	BNZ	getc1		;if (iobuf == 0)
	BSR	GETCHAR
	ret			;	return getchar();
getc1	inx	h
	inx	h
	mov	e,m
	inx	h
	mov	d,m
	push	d
	dcx	d
	mov	m,d
	dcx	h
	mov	m,e
	pop	d
	mov	a,d
	ora	e
	BZ	getc2		;if (iobuf->nleft-- .NE. 0)
	inx	h
	inx	h
	mov	e,m
	inx	h
	mov	d,m
	push	d
	inx	d
	mov	m,d
	dcx	h
	mov	m,e
	pop	h
	mov	l,m
	mvi	h,0
	ret			;	return *iobuf->nleft--;
getc2	dcx	h
	mov	d,m
	dcx	h
	mov	e,m
	push	h
	xchg
	shld	ARG1
	pop	h
	push	h
	lxi	d,6
	dad	d
	shld	ARG2
	lxi	h,1
	shld	ARG3
	BSR	READ
	mov	a,h
	ora	a
	BM	getc3
	ora	l
	BNZ	getc4		;if (read(iobuf->fd,iobuf->buff,1) <= 0)
getc3	pop	h
	lxi	h,-1
	ret			;	return ERR;
getc4	pop	h
	inx	h
	inx	h
	lxi	d,127
	mov	m,e
	inx	h
	mov	m,d		;iobuf->nleft = 127;
	inx	h
	inx	h
	inx	h
	mov	d,h
	mov	e,l
	mov	a,m		;a = *iobuf->buff;
	inx	d
	dcx	h
	mov	m,d
	dcx	h
	mov	m,e		;iobuf->nextp = iobuf->buff - 1;
	mov	l,a
	mvi	h,0
	ret			;return a;
	PEND	GETC
