;
;	--- Version 2.2 90-10-12 10:38 ---
;
;	CTask - BIOS INT 17 (Printer support) interrupt handler
;
;	Public Domain Software written by
;		Thomas Wagner
;		Ferrari electronic Gmbh
;		Beusselstrasse 27
;		D-1000 Berlin 21
;		Germany
;
;	This file is new with version 1.2.
;
;	This interrupt handler completely replaces the standard BIOS
;	printer output driver. It assumes full IBM compatibility.
;	The advantage of this driver is that it schedules while waiting
;	for the printer to get ready, so other tasks are not 
;	unnecessarily delayed.
;
;	CAUTION: This module can only be installed in the primary kernel.
;		 It is not ROMable.
;
;
	name	tskint17
;
	include	tsk.mac
;
	.tsk_model
;
	Pubfunc	tsk_install_int17
	Pubfunc	tsk_remove_int17
;
	Globext	yield
	Globext	create_ticker
	Globext	delete_ticker
;
intseg	segment at 0
;
		org	17h*4
prnoff		dw	?		; interrupt entry
prnseg		dw	?

intseg	ends
;
biosdata 	segment at 40h
;
		org	8h
pr_base		dw	4 dup(?)
		org	78h
pr_timeout	db	4 dup(?)
;
biosdata	ends
;
	.tsk_data
;
extrn		tsk_glob_rec: byte
;
psav_off	dw	?
psav_seg	dw	?
;
ticker		tick_rec	<>
;
	.tsk_edata
	.tsk_code
;
;------------------------------------------------------------------------
;
;	void near tsk_remove_int17 (void)
;
;	This routine un-installs the int handler.
;
Localfunc tsk_remove_int17
;
	IFDEF	LOAD_DS
	push	ds
	mov	ax,@CTASK_DATA
	mov	ds,ax
	ENDIF
;
	callp	delete_ticker,<<ds,#ticker>>
	xor	ax,ax
	mov	es,ax
	assume	es:intseg
        cli
	mov	ax,psav_off		; restore vector
	mov	prnoff,ax
	mov	ax,psav_seg
	mov	prnseg,ax
        sti
	assume	es:nothing
;
	IFDEF	LOAD_DS
	pop	ds
	ENDIF
;
        ret
;
tsk_remove_int17	endp
;
;----------------------------------------------------------------------
;
;	void near tsk_install_int17 (void)
;
;	This routine installs the int handler.
;
Localfunc tsk_install_int17
;
	IFDEF	LOAD_DS
	push	ds
	mov	ax,@CTASK_DATA
	mov	ds,ax
	ENDIF
;
	xor	ax,ax
	mov	es,ax			; establish addressing for intseg
	assume	es:intseg
;
	mov	ax,prnoff		; save old int addr
	mov	psav_off,ax
	mov	ax,prnseg
	mov	psav_seg,ax
	cli
	mov	prnoff,offset @prn_int 	; set new int addr
	mov	prnseg,cs
	sti
	assume	es:nothing
	callp	create_ticker,<<ds,#ticker>,0,0>
	IFDEF	LOAD_DS
	pop	ds
	ENDIF
	ret
;
tsk_install_int17	endp
;
;----------------------------------------------------------------------
;
	assume	ss:nothing
;
;	Interrupt handler
;
@prn_int	proc	far
;
	sti
	push	ds
	push	es
	push	si
	push	dx
	push	cx
	mov	cx,SEG biosdata
	mov	es,cx
	mov	si,@CTASK_DATA
	mov	ds,si
;
	assume	es:biosdata
	mov	si,dx			; printer number
	shl	si,1
	mov	dx,pr_base[si]		; load port
	or	dx,dx
	jz	prn_ret			; return if no printer
;
	or	ah,ah			; fn 0 = output
	jz	prn_output
	dec	ah			; fn 1 = init
	jz	prn_init
	dec	ah			; fn 2 = status
	jz	prn_status
;
prn_ret:
	pop	cx
	pop	dx
	pop	si
	pop	es
	pop	ds
	iret
;
;	AH = 2: Return printer Status
;
prn_status:
	inc	dx			; status port
prn_st2:
	mov	cl,al			; save al
	in	al,dx
	mov	ah,al
	and	ah,0f8h
	xor	ah,048h
	mov	al,cl			; restore al
	jmp	prn_ret
;
;	AH = 1: Initialize printer
;
prn_init:
	push	ax
	inc	dx
	inc	dx
	mov	al,8
	out	dx,al
	mov	ax,tsk_glob_rec.ticks_per_sec
	mov	cl,4
	shr	ax,cl
	inc	ax
	mov	ticker.ticklo,ax	; 1/16 second
	push	bx
;
;	Wait for 1/16 sec. Busy waiting loop with scheduling here to
;	avoid compatibility problems and stack overruns.
;
prini_loop:
	call	yield
	cmp	ticker.ticklo,0
	jnz	prini_loop
;
	pop	bx
	mov	al,0ch
	out	dx,al
	dec	dx
	pop	ax
	jmp	prn_st2
;
;	AH = 0: Output character in AL
;
prn_output:
	push	ax
	out	dx,al
	inc	dx
	shr	si,1
	push	dx
	mov	ax,tsk_glob_rec.ticks_per_sec
	mov	dl,pr_timeout[si]
	xor	dh,dh
	mul	dx
	cli
	mov	ticker.ticklo,ax
	mov	ticker.tickhi,dx
	sti
	pop	dx
;
prout_wait:
	mov	cx,10
prout_1:
	in	al,dx
	test	al,80h
	jnz	prout_2
	loop	prout_1
	cmp	ticker.tickhi,0
	jnz	prout_1a
	cmp	ticker.ticklo,0
	jz	prout_timeout
;
;	printer is busy, yield and try again later.
;
prout_1a:
	push	bx
	call	yield
	pop	bx
	jmp	prout_wait
;
prout_2:
	inc	dx
	mov	al,0dh
	out	dx,al
	mov	al,0ch
	jmp	short $+2
	jmp	short $+2
	out	dx,al
	dec	dx
	pop	ax
	jmp	prn_st2
;
prout_timeout:
	mov	cl,al
	pop	ax
	mov	ah,cl
	and	ah,0f9h
	or	ah,1
	xor	ah,048h
	jmp	prn_ret
;
@prn_int	endp
;
	.tsk_ecode
	end

