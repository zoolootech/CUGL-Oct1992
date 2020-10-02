;
;	--- Version 2.2 90-10-12 10:37 ---
;
;	CTask - BIOS INT 15 interrupt handler (IBM AT specific)
;
;	Public Domain Software written by
;		Thomas Wagner
;		Ferrari electronic Gmbh
;		Beusselstrasse 27
;		D-1000 Berlin 21
;		Germany
;
;	NOTE:   Support for printer output busy has been dropped in
;		version 1.2. Depending on the speed of the driving
;		program, the old algorithm (which used a timed delay)
;		could cause the printer to slow down to an unbearable
;		crawl, and there is little chance for a clean solution
;		here, since there is not enough info on the printer
;		passed to INT 15.
;		Instead, an INT 17 replacement that schedules while 
;		waiting on the printer has been added (module tskint17.asm).
;
;	CAUTION: This module can only be installed in the primary kernel.
;		 It is not ROMable.
;
	name	tskbios
;
	include	tsk.mac
;
	.tsk_model
;
	Pubfunc	tsk_install_bios
	Pubfunc	tsk_remove_bios
;
	Globext	clear_flag
	Globext	wait_flag_set
	Globext	set_flag
        Globext	create_flag
        Globext	delete_flag
        Globext	t_delay
	Locext	tsk_switch_stack
;
	extrn	tsk_glob_rec: word	; global_rec
;
;
intseg	segment at 0
;
		org	15h*4
pintofs		dw	?		; interrupt entry
pintseg		dw	?

intseg	ends
;
;
	.tsk_data
;
floppy  flag    <>
fdisk   flag    <>
kbd     flag    <>
;
flag_tab	label	word
	dw	offset fdisk
	dw	offset floppy
	dw	offset kbd
;
	IF	TSK_NAMEPAR
fflname	db	"WFLEXDSK",0
ffdname	db	"WHARDDSK",0
fkbname	db	"WKBDPOST",0
	ENDIF
;
	.tsk_edata
	.tsk_code
;
;
post_save	label	dword		; in CSEG to allow addressing
psofs		dw	?
psseg		dw	?
;
;----------------------------------------------------------------------
;
;	interrupt handler
;
@bios_int	proc	far
        pushf
	sti
        cmp     ah,90h			; WAIT
        jb      bios_pass
        cmp     ah,91h			; POST
        ja	bios_pass
	cmp	al,2			; fdisk/floppy/keyboard
        jbe	process_bios
	cmp	al,0fdh			; floppy motor
        je	process_bios
;
bios_pass:
	popf
	jmp	cs:post_save
;
;
process_bios:
	popf
	call	tsk_switch_stack
	stc_saved			; Carry set on return
;
	cmp	al,2
	jbe	flag_ops
;
;	Wait for diskette motor start (1 sec.)
;
wait_motor:
	IF	CLOCK_MSEC
	callp	t_delay,<<0,1000>>
	ELSE
	callp	t_delay,<<0,tsk_glob_rec.ticks_per_sec>>
	ENDIF
	iret
;
;	Floppy/Fdisk/Keyboard wait/post
;
flag_ops:
	xor	bh,bh
	mov	bl,al	
	add	bx,bx
	mov	bx,flag_tab[bx]		; load appropriate flag
	cmp	ah,90h
	je	fl_wait			; jump if wait op
	callp	set_flag,<<ds,bx>>	; set flag: complete
	clc_saved			; carry clear on return
	iret
;
; Note: Version 2.1 includes the following fix provided by S. Worthington:
;
; When using a very fast hard disk controller (especially one with
; onboard cache or the new AT bus type controllers), it is possible to
; get the hard disk ready interrupt "posted" BEFORE the call to "wait"
; reaches the "call clear_flag_wait_set"  instruction.  This results in
; a long disk timeout and an error return.  The fix is to use a
; "wait_flag_set" followed by a "clear_flag" to replace the
; "clear_flag_wait_set".  Even though there is still a gap between the
; wait and clear, it is not possible to get a "post" in this gap since
; "upper_dos" is still locked, preventing the start of a new disk I/O.
;	
fl_wait:
	xor	dx,dx
	cmp	al,2
	je	fl_dowait		; no timeout if AL=2 (Keyboard)
	IF	CLOCK_MSEC
	mov	dx,1000
	ELSE
	mov	dx,tsk_glob_rec.ticks_per_sec
	ENDIF
	add	dx,dx			; two seconds
	cmp	al,1
	je	fl_dowait		; 2 sec timeout if AL=1 (Diskette)
	mov	cx,dx
	add	dx,cx
	add	dx,cx			; 6 sec timeout for Harddisk
fl_dowait:
	push	bx
	callp	wait_flag_set,<<ds,bx>,<0,dx>> ; Wait for event
	or	ax,ax
	jnz	flw_ret			; keep carry set if timeout
	clc_saved			; else return with carry clear
flw_ret:
	pop	bx
	callp	clear_flag,<<ds,bx>>
	iret
;
@bios_int       endp
;
;------------------------------------------------------------------------
;
;	void far tsk_remove_bios (void)
;
;	This routine un-installs the int handler.
;
Localfunc tsk_remove_bios
;
	IFDEF	LOAD_DS
	push	ds
	mov	ax,@CTASK_DATA
	mov	ds,ax
	ENDIF
;
	push	es
	xor	ax,ax
	mov	es,ax
	assume	es:intseg
        cli
	mov	ax,cs:psofs		; restore vector
	mov	pintofs,ax
	mov	ax,cs:psseg
	mov	pintseg,ax
        sti
	assume	es:nothing
	pop	es
;
        callp   delete_flag,<<ds,#floppy>>
        callp   delete_flag,<<ds,#fdisk>>
        callp   delete_flag,<<ds,#kbd>>
;
	IFDEF	LOAD_DS
	pop	ds
	ENDIF
        ret
;
tsk_remove_bios	endp
;
;----------------------------------------------------------------------
;
;	void far tsk_install_bios (void)
;
;	This routine installs the int handler.
;
Localfunc tsk_install_bios
;
	IFDEF	LOAD_DS
	push	ds
	mov	ax,@CTASK_DATA
	mov	ds,ax
	ENDIF
;
	IF	TSK_NAMEPAR
        callp   create_flag,<<ds,#floppy>,<ds,#fflname>>
	ELSE
        callp   create_flag,<<ds,#floppy>>
	ENDIF
;
	IF	TSK_NAMEPAR
        callp   create_flag,<<ds,#fdisk>,<ds,#ffdname>>
	ELSE
        callp   create_flag,<<ds,#fdisk>>
	ENDIF
;
	IF	TSK_NAMEPAR
        callp   create_flag,<<ds,#kbd>,<ds,#fkbname>>
	ELSE
        callp   create_flag,<<ds,#kbd>>
	ENDIF
;
        push    es
	xor	ax,ax
	mov	es,ax			; establish addressing for intseg
	assume	es:intseg
;
	mov	ax,pintofs		; save old timer int addr
	mov	psofs,ax
	mov	ax,pintseg
	mov	psseg,ax
	cli
	mov	pintofs,offset @bios_int ; set new timer int addr
	mov	pintseg,cs
	sti
	assume	es:nothing
        pop     es
	IFDEF	LOAD_DS
	pop	ds
	ENDIF
	ret
;
tsk_install_bios	endp
;
	.tsk_ecode
	end

