	page	66,132
comment !
/*
HEADER: 	;
TITLE:		PC com port driver front end;
VERSION:	1.0;

DESCRIPTION:	"Assembler front end (start up module) for COMX.C.
		Contains MS-DOS device driver interface and ISR
		prototype.  Defines DGROUP to coerce the linkage
		editor to produce a tiny memory model executable
		file.";

WARNINGS:	"Microsoft specific.  Start up code does NOT zero
		uninitialized static data.";
SYSTEM: 	MS-DOS v2 or later;
FILENAME:	COMXFE.ASM;

SEE-ALSO:	COMX, COMX.C, COMXBE.C;
AUTHORS:	Hugh Daschbach;
COMPILERS:	Microsoft v5.1;
*/
!
;-----------------------------------------------------------------------------;
;  dfe.asm:  The routines in this file provide an assembler front end
;  to a MS-DOS driver written in C.
;-----------------------------------------------------------------------------;
	.model	 small,c
dgroup	group	_TEXT,_DATA
	assume	cs:dgroup,ds:nothing,es:nothing,ss:nothing

;-----------------------------------------------------------------------------;
;	Define Device Header
;-----------------------------------------------------------------------------;
	.code
com1:	dw	offset dgroup : com2	; offset of next header
	dw	-1			; segment of next header
	dw	0c000h			; attribute: character, supports ioctl,
					; and generic ioctl
	dw	offset dgroup : dev_strategy
	dw	offset dgroup : dev_inter1
	db	'COM1	 '

com2:	dw	offset dgroup : com3	; offset of next header
	dw	-1			; segment of next header
	dw	0c000h			; attribute: character, supports ioctl,
					; and generic ioctl
	dw	offset dgroup : dev_strategy
	dw	offset dgroup : dev_inter2
	db	'COM2	 '

com3:	dw	offset dgroup : com4	; offset of next header
	dw	-1			; segment of next header
	dw	0c000h			; attribute: character, supports ioctl,
					; and generic ioctl
	dw	offset dgroup : dev_strategy
	dw	offset dgroup : dev_inter3
	db	'COM3	 '

com4:	dw	-1			; offset of next header
	dw	-1			; segment of next header
	dw	0c000h			; attribute: character, supports ioctl,
					; and generic ioctl
	dw	offset dgroup : dev_strategy
	dw	offset dgroup : dev_inter4
	db	'COM4	 '


	.data
save_sp dw	?			; register save space for stack seg
save_ss dw	?			; and pointer
					;-------------------------------------;
					; The stack and request block pointer
					; are defined adjacently to deliver
					; the request block and line number as
					; parameters to the driver() when it
					; is called from the interrpt entry
					; point.
					;-------------------------------------;
	db	16 dup ('stack	 ')	; local stack space
rb_off	dw	?			; request block offset
rb_seg	dw	?			; request block segment
line_no dw	?			; com line number (0 - 3)

	.code
					;-------------------------------------;
					; declare external reference to the
					; driver, resolve reference to C
					; startup module (_acrtused) with
					; dummy label
					;-------------------------------------;
	extrn	driver : near
	public	_acrtused
_acrtused =	0

;-----------------------------------------------------------------------------;
;	Device Strategy Entry Point
;-----------------------------------------------------------------------------;
dev_strategy proc far
	mov	cs:rb_seg,es		; save pointer to request block
	mov	cs:rb_off,bx
	ret
dev_strategy endp

;-----------------------------------------------------------------------------;
;	Device Interrut Entry Points
;-----------------------------------------------------------------------------;
dev_inter1:
	mov	cs:line_no,0
	jmp	dev_interrupt
dev_inter2:
	mov	cs:line_no,1
	jmp	dev_interrupt
dev_inter3:
	mov	cs:line_no,2
	jmp	dev_interrupt
dev_inter4:
	mov	cs:line_no,3
	jmp	dev_interrupt

;-----------------------------------------------------------------------------;
;	Device Interrupt Entry Point
;
;  Note: if this is to execute on first run 8088/8086 processors, interrupts
;  must be disabled while the stack registers are being manipulated.  Newer
;  processors gaurentee that interrupts are disabled for one instruction cycle
;  after the move to SS.
;-----------------------------------------------------------------------------;
dev_interrupt proc far

	push	es			; save registers that C does not
	push	ds
	push	bp
	push	di
	push	si
	push	dx
	push	cx
	push	bx
	push	ax
	mov	ax,cs			; establish data addressability
	mov	ds,ax
	assume	ds:dgroup
	mov	save_ss,ss		; connect to local stack.
	mov	save_sp,sp
	mov	ss,ax
	mov	sp,offset dgroup : rb_off
	call	driver			; call the driver
	mov	ss,save_ss		; restore stack
	mov	sp,save_sp
	pop	ax			; restore registers
	pop	bx
	pop	cx
	pop	dx
	pop	si
	pop	di
	pop	bp
	pop	ds
	pop	es
	ret				; and return
	assume	ds:nothing
dev_interrupt endp

;-----------------------------------------------------------------------------;
;  ISR prototype - This routine is copied to an isr structure by hookvect().
;  The target of the call and jump are adjusted before hooking the structure
;  to a hardware interrupt vector.  This routine assumes that the called
;  C routine saves and restores BP, SI, and DI.
;-----------------------------------------------------------------------------;
isr_prototype proc far
	push	es			; save registers that C does not
	push	ds
	push	dx
	push	cx
	push	bx
	push	ax
	mov	ax,cs			; get data addressability
	mov	ds,ax
	call	driver			; call the interrupt service routine
	or	ax,ax			; test return value
	pop	ax			; restore registers
	pop	bx
	pop	cx
	pop	dx
	pop	ds
	pop	es
	jz	ip01			; if return value no zero
	jmp	dword ptr cs:dummy	;     chain to old vector
ip01:					; else
	iret				;     return directly from interrupt
					; endif
dummy	dd	?
isr_prototype endp
;-----------------------------------------------------------------------------;
; int10(ax, bx) - video I/O service
;-----------------------------------------------------------------------------;
int10	proc	near a:word, b:word
	mov	ax,a
	mov	bx,b
	int	10h
	ret
int10	endp

	end
