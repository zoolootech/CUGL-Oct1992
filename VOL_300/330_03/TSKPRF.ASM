;
;	--- Version 2.2 90-10-12 10:38 ---
;
;	CTask - Printf replacement
;
;	Public Domain Software written by
;		Thomas Wagner
;		Ferrari electronic Gmbh
;		Beusselstrasse 27
;		D-1000 Berlin 21
;		Germany
;
;	This file is new with Version 2.1. 
;
;	This module contains substitutes for the standard C printf
;	family formatting routines. It uses no helper routines, and
;	thus can be called from Assembler programs and is model
;	independent. It is smaller and faster (up to 5 times) than
;	the standard C printf, and supports all output options except 
;	floating point and some esoteric flags. 
;
;	Also supported is output to the second monitor in a dual-
;	monitor system.
;
;	The routines in this module are reentrant in the sense that nothing
;	will crash if routines are re-entered concurrently. No protection
;	against a garbled screen is built in, however, so external
;	resource request/release calls are still necessary to make
;	sure output to the display stays in proper order. The sprintf
;	routines need no protection.
;
;	No CTask specific stuff is used to allow use of the routines
;	even when CTask is not installed.
;
;	Note that the tsk_fprintf routine takes a different parameter
;	than the standard fprintf. You have to pass an integer file 
;	handle, not a FILE *.
;
;---------------------------------------------------------------------------
;
;	Conversion format:
;
;		% [flags] [width] [.precision] [indirection] [size] conversion
;
;	Flags supported:
;
;		-	Left-justify output
;
;		the +, blank, and # flags are not supported.
;
;	Width and precision are handled like in standard C, i.e.
;	the * specification is supported. The only exception is
;	that numeric output will not be suppressed for an explicit
;	0 precision.
;
;	Size modifiers supported:
;
;		N	Near pointer
;		F	Far pointer
;		h	Short
;		l	Long
;
;	Conversion characters supported:
;
;		c	single character
;		s	zero-terminated string
;		x	unsigned hex word, a-f
;		X	unsigned hex word, A-F
;		d	signed decimal int
;		u	unsigned decimal int
;		p	hex pointer, a-f
;		P	hex pointer, A-F
;		n	store converted length at int pointer
;
;	In addition, two indirection operators are supported. Both
;	can be repeated, but they should not normally be mixed.
;
;		^	Near indirection (param is DS-based near pointer)
;		@	Far indirection (param is far pointer)
;
;	Example:
;		%d	Displays the word parameter
;		%^d	Displays the word at DS:parameter
;		%@d	Displays the word at the far address 
;			given as parameter
;
;---------------------------------------------------------------------------
;
	name	tskprf
;
	include	tsk.mac
;
	.tsk_model
;
	public	tsk_regen		; far pointer to regen buffer
	public	tsk_regen_s		; regen buffer segment
	public	tsk_regen_o		; current regen buffer offset
	public	tsk_disport		; display controller I/O port
;
	Pubfunc	tsk_putc		; put char to screen
	Pubfunc	tsk_puts		; put string to screen
	Pubfunc	tsk_rputc		; put char to regen
	Pubfunc	tsk_rputs		; put string to regen
;
	CPubfnc	tsk_printf		; print to screen
	Pubfunc	tsk_vprintf		; print to screen with arg pointer
	CPubfnc	tsk_fprintf		; print to file handle
	Pubfunc	tsk_vfprintf		; print to file handle w. argptr
	CPubfnc	tsk_sprintf		; print to string
	Pubfunc	tsk_vsprintf		; print to string w. argptr
	CPubfnc	tsk_rprintf		; print to regen
	Pubfunc	tsk_vrprintf		; print to regen w. argptr
;
	Pubfunc	tsk_setpos		; set regen cursor position
	Pubfunc	tsk_set_regen		; set regen address
	Pubfunc	tsk_set_dualdis		; init secondary monitor
	Pubfunc	tsk_set_currdis		; init primary monitor
	Pubfunc	tsk_set_colour		; init colour monitor
	Pubfunc	tsk_set_mono		; init mono monitor
	Pubfunc	tsk_set_attr		; set display attribute
	Pubfunc	tsk_set_clreol		; CR clears to EOL when set
;
;
; Configuration options:
;
; FILE_BUFSIZE	Is the local buffer size for tsk_fprintf and tsk_vfprintf.
;		The buffer is allocated on the stack, so it should
;		not be chosen too large to avoid stack overflows.
;		It must be a power of 2, and less than or equal to 256.
;
FILE_BUFSIZE	=	64 - 1
;
;
; DFLT_FAR	If set, pointers (%p and %n) and strings (%s) are assumed
;		far by default, near pointers must use the N size
;		modifiers.
;		If clear, the default is near, so the F size modifier
;		must be used for far pointers.
;
DFLT_FAR	=	1	; by default, pointers and strings are Far
;
;
;	The local variables used in the main formatter.
;	Those variables are relative to BP, and are accessed by
;	several routines in this module (the local routines do not
;	use the standard stack discipline, they should be viewed as
;	Pascal-like nested routines within the scope of @disprintf).
;
dislocrec	struc
convbuf	db	11 dup(?)	; conversion buffer (hex/dec)
prflags	db	?		; formatting flags
prhexch	db	?		; Hex format offset for upper/lower case
prfcnt	db	?		; file output counter
procnt	dw	?		; output char counter
prwidth	dw	?		; field width
prec	dw	?		; precision
arglen	dw	?		; argument length in bytes
prproc	dw	?		; output routine offset
prargbx	dw	?		; output routine BX argument
prargdx	dw	?		; output routine DX argument
dislocrec	ends
;
;	formatting flags
;
F_LONGARG	=	01h	; argument is long/far (4 bytes)
F_SHORTARG	=	02h	; argument is single byte
F_PADZERO	=	04h	; pad with zeros
F_LEFTADJ	=	08h	; left adjust output
F_SIGNED	=	10h	; signed number
F_WIDTH		=	20h	; width is valid
F_PREC		=	40h	; precision is valid
F_INDIR		=	80h	; indirection was used
;
biosdata	segment at 40h
		org	4ah
bios_cols	dw	?
		org	63h
bios_chipad	dw	?
		org	84h
bios_rows	db	?
biosdata	ends
;
	.tsk_data
;
tsk_regen	label	dword
tsk_regen_o	dw	0
tsk_regen_s	dw	0b000h		; default mono screen
;
tsk_disport	dw	3b4h		; default mono 6845
tsk_attrib	db	07h		; default white on black
;
sc_clreol	db	0		; default is don't clear
;
sc_cols	dw	80 * 2
sc_end	dw	25 * 80 * 2
;
sc_proc	dw	offset @nodis
;
	.tsk_edata
	.tsk_code
;
ctdataseg	dw	@CTASK_DATA
;
;	setcsr - local routine to set the cursor to the current position
;		 on the regen display.
;
@setcsr	proc	near
;
	push	dx
	mov	dx,tsk_disport
	or	dx,dx
	jz	no_cursor
	mov	al,0eh
	out	dx,al
	jmp	$+2
	mov	ax,tsk_regen_o
	shr	ax,1
	xchg	ah,al
	inc	dx
	out	dx,al
	jmp	$+2
	dec	dx
	mov	al,0fh
	out	dx,al
	jmp	$+2
	mov	al,ah
	inc	dx
	out	dx,al
;
no_cursor:
	pop	dx
	ret
;
@setcsr	endp
;
;
; disregen - local routine to write char directly into the display 
;	regen buffer. The following control characters 
;	are handled special:
;		08 \b (BS)  	backspace one position (no erase)
;		09 \t (TAB)	tab to next 8th column
;		0D \r (CR)	carriage return
;		0A \n (LF)	line feed
;				Note: the printf routines will not pass
;				a single line feed unchanged.
;		0B \v (VT)	clear to end of line
;		0C \f (FF)	clear to end of screen
;
;
;	Entry:	AL = char to display
;	Uses:	AX
;
@disregen	proc	near
;
	push	ds
	mov	ds,cs:ctdataseg		; allow access to local vars
;
;	First, check for control characters
;
	cmp	al,0eh			; 08-0D are special
	jae	no_special
	cmp	al,08h
	jb	no_special
	je	dis_bs			; 08 = Backspace
	cmp	al,0ah
	jb	dis_tab			; 09 = Tab
	je	dis_lf			; 0A = Linefeed
	cmp	al,0ch
	jb	dis_vt			; 0B = Clear to eol
	jne	dis_cr			; 0D = Carriage return
	jmp	dis_ff			; 0C = Clear screen
;
;	Carriage return
;
dis_cr:
	cmp	sc_clreol,0
	jne	dis_vt
discr1:
	push	dx
	cli
	mov	ax,tsk_regen_o		; current offset
	xor	dx,dx			; make into doubleword
	div	sc_cols			; divide by number of columns
	sub	tsk_regen_o,dx		; subtract remainder
	sti
	pop	dx
	jmp	disreg_end
;
no_special:
	jmp	disregen_ok
;
;	Backspace
;
dis_bs:
	cli
	cmp	tsk_regen_o,0		; Handle wraparound
	je	dis_bs2
	sub	tsk_regen_o,2
	sti
	jmp	disreg_end
dis_bs2:
	mov	ax,sc_end		; Wrap to end of screen
	sub	ax,2
	mov	tsk_regen_o,ax
	sti
	jmp	disreg_end
;
;	Tab
;
dis_tab:
	cli
	mov	ax,tsk_regen_o
	add	ax,16
	and	ax,0fff0h		; Tabs every 8 cols
	mov	tsk_regen_o,ax
	sti
	jmp	disreg_end
;
;	Linefeed
;
dis_lf:
	cli
	mov	ax,tsk_regen_o
	add	ax,sc_cols		; Add one line
	mov	tsk_regen_o,ax
	sti
	cmp	ax,sc_end		; did we get past the end?
	jae	dislf_sc
	jmp	disreg_end
;
dislf_sc:
	push	es			; then setup regs and go scroll
	push	di
	mov	es,tsk_regen_s
	mov	di,ax
	jmp	short disreg_scroll
;
;	Vertical tab (clear to end of line)
;
dis_vt:
	push	es
	push	di
	push	cx
	push	ax
	les	di,tsk_regen
	mov	cx,sc_cols		; number of columns
	mov	ax,di			; current offset
	xor	dx,dx			; make into doubleword
	div	cx			; divide by number of columns
	sub	cx,dx			; subtract remainder
	shr	cx,1			; number of words
	mov	ah,tsk_attrib
	mov	al,' '
	rep stosw
	pop	ax
	pop	cx
	cmp	al,0dh
	jne	disreg_ok
	pop	di
	pop	es
	jmp	discr1
;
;	Formfeed (clear to end of screen)
;
dis_ff:
	push	es
	push	di
	push	cx
	les	di,tsk_regen
	mov	cx,sc_end		; total number of chars
	sub	cx,di			; minus current offset
	shr	cx,1			; number of words
	mov	ah,tsk_attrib
	mov	al,' '
	rep stosw
	pop	cx
	jmp	short disreg_ok
;
;	Normal character output
;
disregen_ok:
	push	es
	push	di
	cli
	les	di,tsk_regen		; load current regen buffer addr
	mov	ah,tsk_attrib
	stosw
	mov	tsk_regen_o,di		; new offset
	sti
	cmp	di,sc_end		; past the end?
	jb	disreg_ok
;
;	Scroll up one line
;
disreg_scroll:
	sub	di,sc_cols		; one line up
	mov	tsk_regen_o,di
	push	cx
	push	si
	push	ds
	mov	si,sc_cols		; second line
	mov	cx,sc_end
	sub	cx,si			; screen len minus one line
	shr	cx,1			; no. of words
	mov	di,es
	mov	ds,di
	mov	di,0			; first line
	rep movsw
	pop	ds
;
	mov	cx,sc_cols		; number of columns
	shr	cx,1			; number of words
	mov	ah,tsk_attrib
	mov	al,' '
	rep stosw
;
	pop	si
	pop	cx
;
disreg_ok:
	pop	di
	pop	es
;
disreg_end:
	call	@setcsr
	pop	ds
	ret
;
@disregen	endp
;
;
; nodis - local dummy output routine.
;	This routine is called if the regen buffer adddress has not
;	been set, or if there is no secondary monitor.
;
@nodis	proc	near
	ret
@nodis	endp
;
;
; dischar - local screen output routine.
;	This routine uses INT 10 TTY output to display the character.
;	Entry:	AL = char to display
;	Uses:	AX
;
@dischar	proc	near
	IF	IBM
	push	bx
	push	bp
        mov	bl,7
        mov     ah,14
	int	10h
	pop	bp
	pop	bx
	ELSE
	push	dx
	mov	dl,al
	mov	ah,2
	int	21h
	pop	dx
	ENDIF
	ret
@dischar	endp
;
;
; filechar - local file output routine.
;	Write character to file
;	Entry:	AL = char to display
;	Uses:	AX
;
@filechar	proc	near
	push	bx
	push	si
	mov	si,prargdx[bp]
	mov	bl,ss:[si]
	xor	bh,bh
	inc	bx
	mov	ss:[si+bx],al
	cmp	bx,FILE_BUFSIZE - 1
	jb	fcharend
	xchg	si,dx
	inc	dx
	push	ds
	push	ax
	push	cx
	mov	cx,bx
	mov	bx,prargbx[bp]
	mov	ax,ss
	mov	ds,ax
	mov	ah,40h
	int	21h
	pop	cx
	pop	ax
	pop	ds
	xchg	dx,si
	dec	si
	xor	bl,bl
fcharend:
	mov	ss:[si],bl
	pop	si
	pop	bx
	ret
@filechar	endp
;
;
; stringchar - local string output routine.
;	Entry:	AL = char to write to string
;		prargbx/dx set to point to destination
;
;	Uses:	AX
;
@stringchar		proc	near
	push	di
	push	es
	les	di,dword ptr prargbx[bp]
	stosb
	mov	prargbx[bp],di
	pop	es
	pop	di
	ret
@stringchar		endp
;
;
; hexdigbp - put hex digit in AL to SS:DI, increment DI and BL.
;
@hexdigbp	proc	near
;
	push	ax
        and	al,0fh
	add	al,'0'
        cmp	al,'9'
        jbe	hexbp1
        add	al,prhexch[bp]
hexbp1:
	mov	byte ptr ss:[di],al
	inc	di
	inc	bl
	pop	ax
	ret
;
@hexdigbp	endp
;
;
;	hexconv - convert hex
;	Entry:	DX,AX = number
;		SS:DI = Buffer pointer
;	Exit:	BX    = converted length
;		SS:DI = string start + 1
;	Uses:	CX
;
@hexconv	proc	near
	xor	bl,bl
	mov	cl,4
	mov	bh,4
	or	dx,dx
	jnz	hchiword
;
hclp1:
	call	@hexdigbp
	shr	ax,cl
	jz	hcend
	dec	bh
	jnz	hclp1
;
hcend:
	xor	bh,bh
	ret
;
hchiword:
	call	@hexdigbp
	shr	ax,cl
	dec	bh
	jnz	hchiword
	mov	ax,dx
	jmp	hclp1
;
@hexconv	endp
;
;
;	decconv - convert decimal
;
;	Entry:	DX,AX = number
;		SS:DI = Buffer top pointer
;	Exit:	BX    = converted length
;		SS:DI = string start + 1
;	Uses:	CX
;
@decconv	proc	near
	xor	bx,bx
	mov	cx,10
	or	dx,dx
	jnz	dchiword
;
dclp1:
	xor	dx,dx
	div	cx
	add	dl,'0'
	mov	byte ptr ss:[di],dl
	inc	di
	inc	bx
	or	ax,ax
	jnz	dclp1
	ret
;
dchiword:
	push	si
	mov	si,dx
;
dchilp:
	xchg	ax,si
	xor	dx,dx
	div	cx
	xchg	ax,si
	div	cx
	add	dl,'0'
	mov	byte ptr ss:[di],dl
	inc	di
	inc	bx
	or	si,si
	jnz	dchilp
;
	pop	si
	jmp	dclp1
;
@decconv	endp
;
;
; getint - read integer at DS:SI
;	Entry: AL = first char
;	Exit:  AX = integer
;	Uses:  BX,DX
;
@getint	proc	near
;
	and	ax,0fh
	mov	bl,10
getintlp:
	xchg	ax,dx
	lodsb
	cmp	al,'0'
	jb	getint_end
	cmp	al,'9'
	ja	getint_end
	and	ax,0fh
	xchg	ax,dx
	mul	bl
	add	ax,dx
	jmp	getintlp
;
getint_end:
	dec	si
	mov	ax,dx
	ret
;
@getint	endp
;
;
;	padleft - local routine to pad output on the left side.
;
@padleft	proc	near
;
	push	cx
	push	dx
	xor	dx,dx
	test	prflags[bp],F_PREC
	jz	padleft_noprec
	cmp	bx,prec[bp]
	jae	padleft_noprec
;
	mov	dx,prec[bp]
	sub	dx,bx
	test	prflags[bp],F_WIDTH
	jz	padleft_noprec
	sub	prwidth[bp],dx
	jnc	padleft_noprec
	and	prflags[bp],NOT F_WIDTH
;
padleft_noprec:
	test	prflags[bp],F_LEFTADJ
	jnz	padleft_end
	test	prflags[bp],F_WIDTH
	jz	padleft_end
	cmp	bx,prwidth[bp]
	jae	padleft_end
;
	mov	cx,prwidth[bp]
	sub	cx,bx
	add	procnt[bp],cx
	mov	al,' '
	test	prflags[bp],F_PADZERO
	jz	padleftlp
	mov	al,'0'
;
padleftlp:
	push	ax
	call	prproc[bp]
	pop	ax
	loop	padleftlp
;
padleft_end:
	mov	cx,dx
	jcxz	padleft_exit
	add	procnt[bp],cx
;
padleftprec:
	mov	al,'0'
	call	prproc[bp]
	loop	padleftprec
;
padleft_exit:
	pop	dx
	pop	cx
	ret
;
@padleft	endp
;
;	padright - local routine to pad output on the right side
;
@padright	proc	near
;
	test	prflags[bp],F_LEFTADJ
	jz	padright_end
	test	prflags[bp],F_WIDTH
	jz	padright_end
	cmp	bx,prwidth[bp]
	jae	padright_end
;
	push	cx
	mov	cx,prwidth[bp]
	sub	cx,bx
	add	procnt[bp],cx
;
padrightlp:
	mov	al,' '
	call	prproc[bp]
	loop	padrightlp
	pop	cx
;
padright_end:
	ret
;
@padright	endp
;
;
; disprintf: display formatted string.
;
; 	Entry:	DS:SI = format string
; 		ES:DI = parameter pointer
;		CX    = output routine address
;		BX,DX = output routine parameter
;
;	Uses:	AX,BX,DX
;
;	Usually, the parameter pointer will point to the first
;	parameter on the stack. For added flexibility, ES:DI is
;	used here, so that the parameter list may be anywhere in
;	memory. This allows vprintf-style parameters.
;
;
@disprintf	proc	near
	push	bp
	sub	sp,TYPE dislocrec
	mov	bp,sp
	mov	prproc[bp],cx
	mov	prargbx[bp],bx
	mov	prargdx[bp],dx
	mov	procnt[bp],0
;
disploop:
	lodsb
	or	al,al
	jz	dispend			; end of string
	cmp	al,'%'
	je	dispformat		; jump if format character
	cmp	al,0ah
	jne	dispchar
	mov	al,0dh
	call	prproc[bp]		; translate LF into CR+LF
	inc	procnt[bp]
	mov	al,0ah
;
dispchar:
	call	prproc[bp]
	inc	procnt[bp]
	jmp	disploop
;
dispend:
	mov	bx,prargbx[bp]
	add	sp,TYPE dislocrec
	pop	bp
	ret
;
disploop1:
	pop	di
	pop	es
	add	di,arglen[bp]
	jmp	disploop
;
;	Format character found, process conversion
;
dispformat:
	lodsb
	cmp	al,'%'		; %% means print single %
	je	dispchar
;
	push	es
	push	di		; save parameter pointer
	mov	prflags[bp],0
	mov	arglen[bp],2
;
	cmp	al,'-'
	jne	no_flags
	or	prflags[bp],F_LEFTADJ
	lodsb
;
no_flags:
	cmp	al,'0'
	jne	no_zero
	or	prflags[bp],F_PADZERO
	lodsb
;
no_zero:
	cmp	al,'*'
	je	loadwidth
	cmp	al,'9'
	ja	no_width
	cmp	al,'0'
	jb	no_width
	call	@getint
	jmp	short haswidth
;
loadwidth:
	mov	ax,word ptr es:[di]
	add	di,2
	add	arglen[bp],2
;
haswidth:
	or	prflags[bp],F_WIDTH
	mov	prwidth[bp],ax
	lodsb
;
no_width:
	cmp	al,'.'
	jne	no_prec
	lodsb
	cmp	al,'*'
	je	loadprec
	cmp	al,'9'
	ja	no_prec
	cmp	al,'0'
	jb	no_prec
	call	@getint
	jmp	short hasprec
;
loadprec:
	mov	ax,word ptr es:[di]
	add	di,2
	add	arglen[bp],2
;
hasprec:
	or	prflags[bp],F_PREC
	mov	prec[bp],ax
	lodsb
;
;	Process indirection.
;
no_prec:
	mov	dx,ds
	cmp	al,'^'
	je	indir_near
	cmp	al,'@'
	jne	no_indir
	add	arglen[bp],2
;
indir_near:
	or	prflags[bp],F_INDIR
;
indir_loop:
	cmp	al,'^'
	je	is_indir
	cmp	al,'@'
	jne	no_indir
	mov	dx,word ptr es:2[di]
is_indir:
	mov	di,word ptr es:[di]
	mov	es,dx
	lodsb			; next conversion char
	jmp	indir_loop
;
;	Indirection finished, check for size modification
;
no_indir:
	IF	DFLT_FAR
	cmp	al,'s'
	je	dfltsize0
	cmp	al,'n'
	je	dfltsize0
	cmp	al,'p'
	je	dfltsize0
	cmp	al,'P'
	jne	dfltsize1
;
dfltsize0:
	or	prflags[bp],F_LONGARG
	jmp	short no_sizemod
;
dfltsize1:
	ENDIF
	cmp	al,'F'
	je	sizemodl
	cmp	al,'N'
	jne	sizemod2
	lodsb
	jmp	short no_sizemod
;
sizemod2:
	cmp	al,'h'
	jne	sizemod3
	or	prflags[bp],F_SHORTARG
	lodsb
	jmp	short no_sizemod
;
sizemod3:
	cmp	al,'l'
	jne	no_sizemod
;
sizemodl:
	or	prflags[bp],F_LONGARG
	lodsb
;
no_sizemod:
	xor	dx,dx
	test	prflags[bp],F_LONGARG
	jz	loadsingle
	mov	dx,word ptr es:2[di]
	test	prflags[bp],F_INDIR
	jnz	loadsingle
	add	arglen[bp],2
;
loadsingle:
	mov	bl,al
	mov	ax,word ptr es:[di]
	test	prflags[bp],F_SHORTARG
	jz	noshort
	xor	ah,ah
;
noshort:
	cmp	bl,'n'
	jne	convc0
	test	prflags[bp],F_LONGARG
	jz	gostorelen
	mov	ds,dx
gostorelen:
	mov	bx,ax
	mov	ax,procnt[bp]
	mov	[bx],ax
	jmp	disploop1
;
convc0:
	lea	di,convbuf[bp]
;
	cmp	bl,'c'
	jne	convc1
	mov	byte ptr ss:[di],al
	inc	di
	mov	bx,1
	and	prflags[bp],NOT (F_PREC OR F_PADZERO)
	jmp	out_conv
;
convc1:
	cmp	bl,'s'
	jne	convc2
	test	prflags[bp],F_LONGARG
	jnz	gofmtstring
	mov	dx,ds
gofmtstring:
	and	prflags[bp],NOT F_PADZERO
	jmp	fmtstring
;
convc2:
	cmp	bl,'x'
	je	fmthex
	cmp	bl,'X'
	je	fmthex
	cmp	bl,'p'
	je	fmtptr
	cmp	bl,'P'
	je	fmtptr
	cmp	bl,'u'
	je	fmtdec
	cmp	bl,'d'
	jne	badconv
	test	prflags[bp],F_LONGARG
	jnz	signedlong
	test	prflags[bp],F_SHORTARG
	jz	signedint
	cbw
;
signedint:
	test	ah,80h
	jz	fmtdec
	or	prflags[bp],F_SIGNED
	neg	ax
	jmp	short fmtdec
;
signedlong:
	test	dh,80h
	jz	fmtdec
	or	prflags[bp],F_SIGNED
	mov	cx,dx
	xor	dx,dx
	neg	ax
	sbb	dx,cx
	jmp	short fmtdec
;
;	Invalid conversion. Reset parameter pointer and output the
;	char.
;
badconv:
	pop	di
	pop	es
	mov	al,bl
	jmp	dispchar
;
;
fmtdec:
	call	@decconv
	test	prflags[bp],F_SIGNED
	jz	fmtdnosign
	mov	byte ptr ss:[di],'-'
	inc	di
	inc	bx
fmtdnosign:
	jmp	short out_conv
;
fmthex:
	mov	prhexch[bp],'a'-10-'0'
	cmp	bl,'x'
	je	fmthex1
	mov	prhexch[bp],'A'-10-'0'
fmthex1:
	call	@hexconv
	jmp	short out_conv
;
fmtptr:
	mov	prhexch[bp],'a'-10-'0'
	cmp	bl,'p'
	je	fmtptr1
	mov	prhexch[bp],'A'-10-'0'
fmtptr1:
	and	prflags[bp],NOT (F_PREC OR F_PADZERO)
	mov	cl,4
	mov	bh,cl
	xor	bl,bl
;
fmtplo:
	call	@hexdigbp
	shr	ax,cl
	dec	bh
	jnz	fmtplo
	test	prflags[bp],F_LONGARG
	jz	out_conv
;
	mov	byte ptr ss:[di],':'
	inc	di
	inc	bl
	mov	bh,cl
	mov	ax,dx
;
fmtphilp:
	call	@hexdigbp
	shr	ax,cl
	dec	bh
	jnz	fmtphilp
;
;
out_conv:
	mov	cx,bx
	call	@padleft
	add	procnt[bp],cx
outc_out:
	dec	di
	mov	al,byte ptr ss:[di]
	call	prproc[bp]
	loop	outc_out
	call	@padright
	jmp	disploop1
;
;
fmtstring:
	push	si
	push	ds
	mov	ds,dx
	mov	si,ax
;
	mov	es,dx
	mov	di,ax
	xor	ax,ax
	mov	cx,-1
	repne scasb
	not	cx
	dec	cx
	test	prflags[bp],F_PREC
	jz	fmtstr_np
	cmp	cx,prec[bp]
	jbe	fmtstr_np
	mov	cx,prec[bp]
;
fmtstr_np:
	and	prflags[bp],NOT F_PREC
	mov	bx,cx
	call	@padleft
	jcxz	fmtstr_end
	add	procnt[bp],cx
fmtstr_out:
	lodsb
	call	prproc[bp]
	loop	fmtstr_out
fmtstr_end:
	call	@padright
	pop	ds
	pop	si
	jmp	disploop1
;
;
@disprintf	endp
;
;
; void tsk_printf (char far *format, ...)
;
;	Formatted output to console.
;
CGlobalfunc tsk_printf,<uses ds si di, fmt: far ptr, varg: word>
;
	mov	ax,ss
	mov	es,ax
	lds	si,fmt
	lea	di,varg
	mov	cx,offset @dischar
	call	@disprintf
	ret
tsk_printf	endp
;
;
; void tsk_vprintf (char far *format, void far *arg)
;
;	Formatted output to console.
;
Globalfunc tsk_vprintf,<uses ds si di, fmt: far ptr, varg: far ptr>
;
	lds	si,fmt
	les	di,varg
	mov	cx,offset @dischar
	call	@disprintf
	ret
tsk_vprintf	endp
;
;
; void tsk_fprintf (int handle, char far *format, ...)
;
;	Formatted output to file.
;
CGlobalfunc tsk_fprintf	<uses ds si di, handle: word, fmt: far ptr, varg: word>
;
	mov	ax,ss
	mov	es,ax
	lds	si,fmt
	lea	di,varg
	sub	sp,FILE_BUFSIZE
	mov	dx,sp
	mov	bx,sp
	mov	byte ptr ss:[bx],0
	mov	cx,offset @filechar
	mov	bx,handle
	call	@disprintf
	mov	si,sp
	mov	cl,ss:[si]
	xor	ch,ch
	jcxz	fprintfend
	mov	dx,si
	inc	dx
	mov	bx,handle
	mov	ax,ss
	mov	ds,ax
	mov	ah,40h
	int	21h
fprintfend:
	add	sp,FILE_BUFSIZE
	ret
tsk_fprintf	endp
;
;
; void tsk_vfprintf (int handle, char far *format, void far *arg)
;
;	Formatted output to file.
;
Globalfunc tsk_vfprintf,<uses ds si di, handle: word, fmt: far ptr, varg: far ptr>
;
	lds	si,fmt
	les	di,varg
	sub	sp,FILE_BUFSIZE
	mov	dx,sp
	mov	bx,sp
	mov	byte ptr ss:[bx],0
	mov	cx,offset @filechar
	mov	bx,handle
	call	@disprintf
	mov	si,sp
	mov	cl,ss:[si]
	xor	ch,ch
	jcxz	vfprintfend
	mov	dx,si
	inc	dx
	mov	bx,handle
	mov	ax,ss
	mov	ds,ax
	mov	ah,40h
	int	21h
vfprintfend:
	add	sp,FILE_BUFSIZE
	ret
tsk_vfprintf	endp
;
;
; void tsk_sprintf (char far *dest, char far *format, ...)
;
;	Formatted output to string.
;
CGlobalfunc tsk_sprintf,<uses ds si di, dest: far ptr, fmt: far ptr, varg: word>
;
	mov	ax,ss
	mov	es,ax
	lds	bx,dest
	mov	dx,ds
	lds	si,fmt
	lea	di,varg
	mov	cx,offset @stringchar
	call	@disprintf
	sub	bx,word ptr(dest)
	mov	ax,bx
	ret
tsk_sprintf	endp
;
;
; void tsk_vsprintf (char far *dest, char far *format, void far *arglist)
;
;	Formatted output to string.
;
Globalfunc tsk_vsprintf,<uses ds si di, dest: far ptr, fmt: far ptr, varg: far ptr>
;
	lds	bx,dest
	mov	dx,ds
	lds	si,fmt
	les	di,varg
	mov	cx,offset @stringchar
	call	@disprintf
	sub	bx,word ptr(dest)
	mov	ax,bx
	ret
tsk_vsprintf	endp
;
;
; void tsk_rprintf (char far *format, ...)
;
;	Formatted output to regen buffer (second display).
;
CGlobalfunc tsk_rprintf,<uses ds si di, fmt: far ptr, varg: word>
;
	IFDEF	LOAD_DS
	mov	ds,cs:ctdataseg
	ENDIF
	mov	cx,sc_proc
	mov	ax,ss
	mov	es,ax
	lds	si,fmt
	lea	di,varg
	call	@disprintf
	ret
tsk_rprintf	endp
;
;
; void tsk_vrprintf (char far *format, void far *arglist)
;
;	Formatted output to regen buffer (second display).
;
Globalfunc tsk_vrprintf,<uses ds si di, fmt: far ptr, varg: far ptr>
;
	IFDEF	LOAD_DS
	mov	ds,cs:ctdataseg
	ENDIF
	mov	cx,sc_proc
	lds	si,fmt
	les	di,varg
	call	@disprintf
	ret
tsk_vrprintf	endp
;
;
; void tsk_putc (char c)
;
;	Output character to console.
;    
Globalfunc tsk_putc,<chr: word>
	mov	ax,chr
	call	@dischar
	ret
tsk_putc	endp
;
;
; void tsk_puts (char far *s)
;
;	Output string to console.
;    
Globalfunc tsk_puts,<uses ds si, dstr: far ptr>
;
	lds	si,dstr
putslp:
	lodsb
	or	al,al
	jz	putsend
	call	@dischar
	jmp	putslp
;
putsend:
	mov	al,0dh
	call	@dischar
	mov	al,0ah
	call	@dischar
	ret
tsk_puts	endp
;
;
; void tsk_rputc (char c)
;
;	Output character to regen buffer (second display).
;    
Globalfunc tsk_rputc,<chr: word>
;
	mov	ax,chr
	IFDEF	LOAD_DS
	push	ds
	mov	ds,cs:ctdataseg
	call	sc_proc
	pop	ds
	ELSE
	call	sc_proc
	ENDIF
	ret
tsk_rputc	endp
;
;
; void tsk_rputs (char far *s)
;
;	Output string to regen buffer.
;    
Globalfunc tsk_rputs,<uses si, dstr: far ptr>
;
	IFDEF	LOAD_DS
	push	ds
	mov	ds,cs:ctdataseg
	ENDIF
	les	si,dstr
rputslp:
	lods	es:prflags
	or	al,al
	jz	rputsend
	call	sc_proc
	jmp	rputslp
;
rputsend:
	mov	al,0dh
	call	sc_proc
	mov	al,0ah
	call	sc_proc
	IFDEF	LOAD_DS
	pop	ds
	ENDIF
	ret
tsk_rputs	endp
;
;
; void tsk_setpos (int row, int col)
;
;	Set cursor position for regen output.
;	First position on screen is (0, 0).
;
Globalfunc tsk_setpos,<row: word, col: word>
;
	IFDEF	LOAD_DS
	push	ds
	mov	ds,cs:ctdataseg
	ENDIF
	mov	ax,row
	mul	sc_cols
	add	ax,col
	add	ax,col
	mov	tsk_regen_o,ax
	call	@setcsr
	IFDEF	LOAD_DS
	pop	ds
	ENDIF
	ret
tsk_setpos	endp
;
;
; void tsk_set_regen (int segment, int port, int rows, int cols)
;
;	Set regen buffer address and size, and display controller port.
;	This routine can be used to force regen output to the
;	primary screen for single-screen systems, and to modify
;	the output parameters (number of rows and columns).
;
Globalfunc tsk_set_regen,<reg: word, port: word, rows: word, cols: word>
;
	IFDEF	LOAD_DS
	push	ds
	mov	ds,cs:ctdataseg
	ENDIF
	mov	ax,reg
	mov	tsk_regen_s,ax
	mov	ax,port
	mov	tsk_disport,ax
	mov	tsk_regen_o,0
	mov	ax,cols
	add	ax,ax
	mov	sc_cols,ax
	mul	rows
	mov	sc_end,ax
	mov	sc_proc,offset @disregen
	IFDEF	LOAD_DS
	pop	ds
	ENDIF
	ret
tsk_set_regen	endp
;
;
; int tsk_set_dualdis (void)
;
;	Determine regen buffer address on second monitor.
;	This routine first checks the current video mode. If it's
;	mode 7, the regen buffer is set to B800 (the colour screen).
;	For all other modes, the regen buffer is B000 (mono).
;	Then a check is done to make sure that the secondary card
;	exists. If it does, the regen output is initialized. Otherwise,
;	any output through the regen routines will be discarded.
;
;	Returns 0 if there is no secondary monitor, else the
;	regen buffer address.
;
;	No attempt is made to initialize the secondary monitor,
;	the monitor must be in alpha mode.
;
Globalfunc tsk_set_dualdis
;
	IFDEF	LOAD_DS
	push	ds
	mov	ds,cs:ctdataseg
	ENDIF
	mov	ah,0fh
	push	bp
	int	10h		; get video mode
	pop	bp
	mov	bx,0b800h	; colour regen buffer
	mov	dx,3d4h		; CGA 6845 I/O addr
	cmp	al,7		; only mode 7 has regen at B000
	je	ddis_checks
	mov	bx,0b000h	; mono regen buffer
	mov	dx,3b4h		; mono 6845 I/O addr
;
ddis_checks:
	mov	tsk_regen_s,bx
	mov	tsk_regen_o,0
	mov	tsk_disport,dx
	mov	sc_cols,80 * 2
	mov	sc_end,25 * 80 * 2
	mov	al,0fh
	out	dx,al
	inc	dx
	in	al,dx
	mov	ah,al
	mov	al,56h
	out	dx,al
	mov	cx,100h
ddis_wait:
	loop	ddis_wait
	in	al,dx
	xchg	ah,al
	out	dx,al
	cmp	ah,56h
	je	ddis_ok
	mov	sc_proc,offset @nodis
	xor	ax,ax
	mov	tsk_regen_s,ax
	jmp	short ddis_end
;
ddis_ok:
	mov	sc_proc,offset @disregen
	mov	ax,bx
;
ddis_end:
	IFDEF	LOAD_DS
	pop	ds
	ENDIF
	ret
;
tsk_set_dualdis	endp
;
;
; int tsk_set_currdis (void)
;
;	Determine regen buffer address on current monitor.
;	This routine checks the current video mode. If it's
;	mode 7, the regen buffer is set to B000 (the mono screen).
;	For all other modes, the regen buffer is B800 (colour).
;
;	Returns the regen buffer address.
;
;	No attempt is made to initialize the monitor, or to check for
;	special modes with different regen address. The monitor must 
;	be in alpha mode.
;
Globalfunc tsk_set_currdis
;
	IFDEF	LOAD_DS
	push	ds
	mov	ds,cs:ctdataseg
	ENDIF
	mov	ah,0fh
	push	bp
	int	10h		; get video mode
	pop	bp
	mov	bx,0b800h	; colour regen buffer
	cmp	al,7		; only mode 7 has regen at B000
	jne	curdis_ready
	mov	bx,0b000h	; mono regen buffer
;
curdis_ready:
	mov	tsk_regen_s,bx
	mov	tsk_regen_o,0
	mov	ax,SEG biosdata
	mov	es,ax
	assume	es:biosdata
	mov	ax,bios_chipad
	mov	tsk_disport,ax
	mov	ax,bios_cols
	add	ax,ax
	mov	sc_cols,ax
	mov	dl,bios_rows
	xor	dh,dh
	mul	dx
	mov	sc_end,ax
	mov	sc_proc,offset @disregen
	mov	ax,bx
	IFDEF	LOAD_DS
	pop	ds
	ENDIF
	ret
;
tsk_set_currdis	endp
;
;
; void tsk_set_colour (int rows, int cols)
;
;	Set regen buffer address to colour monitor.
;
Globalfunc tsk_set_colour,<rows: word, cols: word>
;
	IFDEF	LOAD_DS
	push	ds
	mov	ds,cs:ctdataseg
	ENDIF
	mov	tsk_regen_s,0b800h	; colour regen buffer
	mov	tsk_regen_o,0
	mov	tsk_disport,3d4h	; CGA 6845 I/O addr
	mov	ax,cols
	add	ax,ax
	mov	sc_cols,ax
	mul	rows
	mov	sc_end,ax
	mov	sc_proc,offset @disregen
	IFDEF	LOAD_DS
	pop	ds
	ENDIF
	ret
;
tsk_set_colour	endp
;
;
; void tsk_set_mono (int rows, int cols)
;
;	Set regen buffer address to monochrome monitor.
;
Globalfunc tsk_set_mono,<rows: word, cols: word>
;
	IFDEF	LOAD_DS
	push	ds
	mov	ds,cs:ctdataseg
	ENDIF
	mov	tsk_regen_s,0b000h	; mono regen buffer
	mov	tsk_regen_o,0
	mov	tsk_disport,3b4h	; mono 6845 I/O addr
	mov	ax,cols
	add	ax,ax
	mov	sc_cols,ax
	mul	rows
	mov	sc_end,ax
	mov	sc_proc,offset @disregen
	IFDEF	LOAD_DS
	pop	ds
	ENDIF
	ret
;
tsk_set_mono	endp
;
;
; void tsk_set_attr (int attr)
;
;	Set regen display attributes
;
Globalfunc tsk_set_attr,<attr: word>
;
	IFDEF	LOAD_DS
	push	ds
	mov	ds,cs:ctdataseg
	ENDIF
	mov	ax,attr
	mov	tsk_attrib,al
	IFDEF	LOAD_DS
	pop	ds
	ENDIF
	ret
;
tsk_set_attr	endp
;
;
; void tsk_set_clreol (int clr)
;
;	Set special handling of CR. If "clr" is nonzero, CR will clear
;	to end of line before returning to the home position.
;
Globalfunc tsk_set_clreol,<clr: word>
;
	IFDEF	LOAD_DS
	push	ds
	mov	ds,cs:ctdataseg
	ENDIF
	mov	ax,clr
	mov	sc_clreol,al
	IFDEF	LOAD_DS
	pop	ds
	ENDIF
	ret
;
tsk_set_clreol	endp
;
	.tsk_ecode
	end

