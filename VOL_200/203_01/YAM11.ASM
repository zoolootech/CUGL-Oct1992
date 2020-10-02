PAGE ,132
TITLE 'YAM11: YAM TERMIO ASSMELBY ROUTINES'
;DATE ( 7 nov 85)

; SMALL MODEL
.xlist
INCLUDE \LC\S\DOS.MAC
.list

; interrupt vectors
ser0		equ	0ch	;serial 0 interrupt
ser1		equ	0bh	;serial 1 interrupt
break		equ	01bh	;break interrupt
cntlc		equ	023h	;control-c handler
lpt		equ	017h	;printer status

; monitor functions
get_status	equ	02h	;get printer status

; dos functions
commint		equ	14h	;monitor serial I/O
dos		equ	21h	;ms-dos vector
dcio		equ	6h	;dos direct console i/o
get_vector	equ	35h	;get interrupt vector
set_vector	equ	25h	;set interrupt vector

; i/o port addresses
cntl8259	equ	20h	;control port
mask8259	equ	21h	;interrupt mask register

; offsets to rest of ports
intenreg	equ	1	;interrupt enable
intidreg	equ	2	;interrupt identication
lcreg		equ	3	;line control
modctlreg	equ	4	;modem control
linestreg	equ	5	;line status
modstreg	equ	6	;modem status

; masks and data equates
charrdy		equ	01h	;character ready
cr		equ	0dh
eoi		equ	20h	;non-specific eoi command
idisable	equ	0f7h	;serial int disable
ienable		equ	08H	;serial int enable
lf		equ	0ah
mask3		equ	08h	;int3 disable mask
mask4		equ	010h	;int4 disable mask
mon_conout	equ	14	;funtion code
page_0		equ	0	;page to output char to
rts		equ	02h	;rts on mask
rtsoff		equ	0fdh	;rts off mask
rxdmask		equ	01h	;interrupt on rx data ready
txrdy		equ	20h	;transmitter ready
unmask3		equ	0f7h	;int7 enable mask for 8259
unmask4		equ	0efh	;int4 enable mask for 8259
xoff		equ	013h	;^s

;=======================================================
	dseg

errflg	dw	0		;error flag for init comm
lowater	dw	0		;copy of LOWWATER
termflg	db	0		;true if in terminal mode

; global variables
extrn	Dport:word		;actual data port #
extrn	bufst:word		;pointer to buffer start
extrn	bufend:word		;pointer to buffer end
extrn	bufcq:word		;buffer pointer for modem
extrn	bufcdq:word		;buffer pointer for console
extrn	Ignrx:word		;Flag which says to throw away chars in rxnono
extrn	rxnono:word		;pointer to buffer of characters to ignore
extrn	Timeout:word		;timeout before next char rx
extrn	Nfree:word		;number of free bytes in buffer
extrn	overrun:word		;set if buffer overflows
extrn	Wrapped:word		;indicates buffer pointer has looped
extrn	Xoffflg:word		;flag indicating Xoff was sent
	endds

;=======================================================
	pseg

;storage locations in code segment
brk	dd	0		;break vector storage
ctlc	dd	0		;control-c vector storage 

;*******************************************************
; return used for break interrupt handlers
cc_hand	proc	far
	iret
cc_hand	endp


;*******************************************************
; cause an INT3 for entry into debuggers.
; calling sequence:
;	dbgint();

public  dbgint
dbgint	proc	near
	int	3
	ret
dbgint	endp


public init_com
init_com proc near
;*******************************************************
; initialze interrupt vectors for serial port.  This will ensure
; all characters from the serial port are not missed.  It also resets
; the control-c and break vector so they will not cause an abort back
; to the operating system, leaving interrupt vectors in a unknown state.
; calling sequence:
;	init_comm(LOWWATER)
;		where LOWWATER is the number of bytes left in the buffer
;		at which XOFF is set to the modem.

	cld			;auto increment
	push	bp		;save bp
	mov	bp,sp
	mov	ax,[bp+4]	;LOWWATER is parmeter to this routine
	mov	lowater,ax
	cmp	si,bx		;check for buffer overflow
	pop	bp		;done with passed parameters
	push	es		;save es for caller(required by lattice c or
				;very strange things result
	push	ds		;we muck with ds
	cli			;disable interrupts when setting vectors

; save the break interrupt vector.  Save area is in code
; segment, thus the segment over-rides.
	mov	ah,get_vector
	mov	al,break
	int	dos
	mov	word ptr cs:brk,bx	;vector returned in es:bx
	mov	word ptr cs:brk+2,es

; save the contol-c interrupt vector
	mov	ah,get_vector
	mov	al,cntlc
	int	dos
	mov	word ptr cs:ctlc,bx	;vector returned in es:bx
	mov	word ptr cs:ctlc+2,es

; modify old break vector to point to our handler.  It must
; return with an iret instruction.  Since ds is changed to cs,
; mov will function properly with no segment override.
	push	cs
	pop	ds			;set ds=code segment
	mov	ah,set_vector
	mov	al,break		;break interrupt vector
	mov	dx,offset cc_hand	;pointer in ds:dx
	int	dos

; modify old control-c vector to point to our handler.
	mov	ah,set_vector
	mov	al,cntlc		;break interrupt vector
	mov	dx,offset cc_hand	;pointer in ds:dx
	int	dos

	pop	ds
	pop	es		;restore es for caller
	sti			;re-enable system interrupts
	ret	2		;one passed parmeter
init_com endp


public setport
setport	proc	near
;*******************************************************
; set the interrupt vector for the requested comm port.
; only ports 0 and 1 are supported due to they are the only
; ones which support interrupts on most systems.
; calling sequence:
;	setport(port_num);

	cli
	push	bp		;save bp
	mov	bp,sp
	mov	bx,[bp+4]	;get modem port number
	push	ds
	push	bx

; modify serial interrupt vector to point to its handler
	push	cs
	pop	ds		;vector set to ds:dx
	mov	ah,set_vector
	cmp	bx,1		;serial port #1 is at int 3
	jz	por1
	mov	al,ser0		;serial #0 vector
	jmp	short setvec
por1:	mov	al,ser1		;serial #1 interrupt vector
setvec:	mov	dx,offset ser_int
	int	dos

;enable 8259 mask
	in	al,mask8259	;get mask
	pop	bx		;check port #
	cmp	bx,1		;serial port #1 is at int 3
	jz	epor1
	and	al,unmask4	;enable int4
	jmp	short emask
epor1:	and	al,unmask3	;enable int3
emask:	out	mask8259,al
	pop	ds
	pop	bp
	sti			;re-enable system interrupts
	ret	2		;one passed parmeter
setport endp


public resport
resport proc	near
;*******************************************************
; reset the interrupt vector for the requested comm port.
; only ports 0 and 1 are supported due to they are the only
; ones which support interrupts on most systems.
; calling sequence:
;	resport(port_num);

	cli
	push	bp		;save bp
	mov	bp,sp
	mov	bx,[bp+4]	;get modem port number

; mask comm interrupt
	in	al,mask8259	;get mask
	cmp	bx,1		;serial port #1 is at int 3
	jz	dpor1
	or	al,mask4	;disable int4
	jmp	short dmask
dpor1:	or	al,mask3	;disable int3
dmask:	out	mask8259,al

; disable trisate buffer for comm interrupt
	mov	dx,[Dport]
	add	dx,modctlreg
	in	al,dx
	and	al,idisable and rtsoff
	out	dx,al		;set out2 line false

	pop	bp
	sti			;re-enable system interrupts
	ret	2		;one passed parmeter
resport endp


public enblcm
enblcm	proc	near
;*******************************************************
; unmask comm interrupt
; enable bit for receive data interrupt and tristate buffer to allow
; interrupts from ACIA.  Some of the MSDOS commands such as INT 14
; seem to muck with these bits.  Mode is true if in terminal mode otherwise 0.
; Mode sets a flag used by the interrupt routine.
; calling sequence:
;	enblcm(mode);

	cli
	push	bp		;save bp
	mov	bp,sp
	mov	al,[bp+4]	;get termflag
	mov	termflg,al
	mov	dx,[Dport]
	add	dx,intenreg	;enable serial interrupts
	mov	al,rxdmask
	out	dx,al

; enable trisate buffer so interrupts can get to bus
	mov	dx,[Dport]
	add	dx,modctlreg
	in	al,dx
	or	al,ienable or rts
	out	dx,al		;set out2 line true
	pop	bp
	sti
	ret	2		;one passed parameter
enblcm	endp


public dsblcm
dsblcm	proc	near
;*******************************************************
; reset interrupt enable register of acia
; calling sequence:
;	dsblcm();

	cli
	mov	dx,[Dport]
	add	dx,intenreg	;offset to enable register
	mov	al,0		;disable serial interrupts
	out	dx,al
	sti
	ret
dsblcm	endp


public res_comm
res_comm proc	near
;*******************************************************
; reset interrupt vectors set up in init_comm
; calling sequence:
;	res_comm();

	cld			;auto increment
	cli			;disable interrupts when resetting vectors
	push	ds		;we muck with ds again

; restore break vector
	mov	ah,set_vector
	mov	al,break	;break interrupt vector
	mov	dx,word ptr brk	;pointer in ds:dx
	mov	ds,word ptr brk+2
	int	dos

; restore control-c vector
	mov	ah,set_vector
	mov	al,cntlc	;contol-c interrupt vector
	mov	dx,word ptr ctlc;pointer in ds:dx
	mov	ds,word ptr ctlc+2
	int	dos

; re-enable system interrupts
	pop	ds
	sti
	ret
res_comm endp


public ser_int
ser_int proc	far
;*******************************************************
; serial interrupt routine.  This routine gets characters
; from the modem and stuffs them into a circular buffer.
; Global flag Wrapped is set if buffer pointer wrap around
; calling sequence:
;	none.  this is an interrupt routine

	push	ax
	push	dx
	push	di
	push	ds
	mov	ax,dgroup	;ds may be wrong
	mov	ds,ax
	mov	dx,[Dport]
	add	dx,linestreg
	in	al,dx		;line status
	test	al,charrdy	;check data ready
	jz	seoi		;nope, so long
	mov	dx,[Dport]
	in	al,dx		;read character
	mov	di,ds:bufcq	;get bufferpointer
	mov	[di],al		;save char
	inc	di		;bump pointer
	cmp	termflg,0	;terminal mode does more
	jz	noterm
	call	termstuff

; if(bufcq >= bufend)
; {
; 	Wrapped=TRUE;
;	bufcq=bufst;
; }
noterm:	cmp	di,ds:bufend	;check for end of buffer
	jl	serdone
	mov	di,ds:bufst	;bufcq=bufst
	mov	ds:Wrapped,0ffh	;Wrapped = true
serdone:mov	ds:bufcq,di	;update pointer

seoi:	mov	al,eoi
	out	cntl8259,al	;set eoi to controller
	pop	ds
	pop	di
	pop	dx
	pop	ax
intret:	sti			;re-enable interrupts
	iret
ser_int	endp


termstuff proc	near
;*******************************************************
; this procedure contains stuff to do in the terminal mode.
; in file transfer mode, this should not be called.
; on entry ax contains last char read, and di contains pointer to
; next free buffer location.

;if (!Ignrx)
;	bufcq++;
;else
;if(!index(*bufcq, rxnono))
;	bufcq++;
;
	push	si		;part of interrupt save
	cmp	ds:Ignrx,0
	jz	tnfree		;go test Nfree
	mov	si,offset ds:rxnono
tloop:	mov	ah,[si]		;scan for match
	cmp	ah,0		;end of string
	jz	tnfree
	cmp	ah,al
	jz	found
	inc	si		;next char
	jmp	tloop
found:	dec	di		;set free buffer pointer back one

; Timeout=0;
; if(--Nfree == LOWWATER)
; 	MOCHAR(Xoffflg=TRUE);
; else if(Nfree==0)
; 	overrun = TRUE;
;
tnfree:	mov	ds:timeout,0	;Timeout = 0
	dec	ds:Nfree	;number of free bytes -= 1
	mov	ax,ds:Nfree	;check for < LOWWATER
	cmp	ax,0		;test overflow
	jnz	chklow
	mov	ds:overrun,1	;overflow = true

chklow:	cmp	ax,ds:lowater
	jne	done		;Nfree grows down
	mov	ds:Xoffflg,1	;set flag indicating xoff was sent
	mov	ax,XOFF		;send Xoff only Nfree=lowater
	push	ax
	call	mochr		;send Xoff to modem
done:	pop	si
	ret
termstuff	endp



public mirdy
mirdy	proc	near
;*******************************************************
; if character has come in, bufcq and bufcdq will be different
; if not, zero will be returned in ax, with the zero flag set.
; calling sequence:
;	if (mirdy());

	mov	ax,ds:bufcq
	sub	ax,[ds:bufcdq]
	ret
mirdy	endp


public michr
michr 	proc	near
;*******************************************************
; get a character out of the modem input buffer and return in
; ax register.  Pointer will be wrapped around to start of buffer
; if it goes past the end.
; calling sequence:
;	ch = michr();

	mov	si,ds:bufcdq	;char out pointer
	mov	al,[si]		;get char
	mov	ah,0		;make integer
	inc	si		;next buffer location
	cmp	si,ds:bufend	;check for end of buffer
	jl	midone		;dont reset pointer
	mov	si,ds:bufst	;pointer wrapped around
midone:	mov	ds:bufcdq,si	;bufcdq++
	ret
michr	endp


public cnsrdy
cnsrdy	proc	near
;*******************************************************
; console ready function.  If a key has been pressed, the zero flag will
; be reset.  If not, clear ax and return.  This routine is used to
; bypass the DOS ^C handling.
; calling sequence:
;	if (cnsrdy());

	mov	ah,1		;console status function
	int	16h		;key board i/o thru monitor
	jnz	char		;char rx
nochar:	xor	ax,ax		;clear ax and set zero flag
char:	ret
cnsrdy	endp


public 	cnsin
cnsin	proc	near
;*******************************************************
; get a char from the console.  char is returned in al, with
; with keycode in ah.
; calling sequence:
;	ch = cnsin();

	mov	ah,0
	int	16h
	ret
cnsin	endp


public	mochr
mochr	proc	near
;*******************************************************
; output char modem port.  Char is passed on stack as 
; first argument.  This routine is used rather than the
; monitor because the stupid IBM bios resets the interrupt enable
; line to the interrupt driver.  
; calling sequence:
;	mochr(ch);
	
	push	bp		;save bp
	mov	bp,sp
	mov	dx,[Dport]	;wait for txrdy
	add	dx,linestreg
moloop:	in	al,dx
	test	al,txrdy	;wait for hardware
	jz	moloop
	mov	al,[bp+4]	;get char to output
	mov	dx,[Dport]	;transmit port
	out	dx,al
	pop	bp
	ret	2		;clean stack
mochr	endp


public	cochar
cochar	proc	near
;*******************************************************
; output char on stack to console.  Use direct console output
; to bypass control-c, while allowing ansi driver to function
; calling sequence:
;	cochar(ch);

	push	bp		;save bp
	mov	bp,sp
	mov	dl,[bp+4]	;get char to output
	mov	ah,dcio		;direct console i/o
	int	dos
	pop	bp
	ret	2		;clean stack
cochar	endp

public	prtrdy
prtrdy	proc	near
;*******************************************************
; returns with ax=0 if printer is busy, otherwise ax <> 0
; calling sequence:
;	if prtrdy();

	mov	ah,get_status	;function code
	mov	dx,0		;printer port
	int	lpt		;monitor printer status function
	and	ah,11111001b	;mask off unused bits
	cmp	ah,10010000b	;test for ready and select
	je	rdy		;if =, printer is ready
	mov	ax,0		;if not ready, return false
rdy:	ret
prtrdy	endp

	endps

	end
