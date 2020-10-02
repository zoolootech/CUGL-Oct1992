;
;	--- Version 2.2 90-10-12 15:33 ---
;
;	CTask - Local Stack Switch handler module.
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
;	In this version, all Hard- and Software INT handlers switch
;	to a local stack (INT 16 for AH=0 only). Stack overruns have
;	been observed, especially in conjunction with VDISK and the
;	Microsoft Editor. Some Microsoft products (and some other TSR's)
;	link into interrupts without switching to local stacks. This
;	can lead to situations where just a few words are available
;	for use by the CTask handlers. In the abovementioned combination,
;	a keyboard interrupt occurring while VDISK is active leaves
;	four (!) words of stack space for INT 16 execution.
;	Since most of the interrupts should be reentrant, a number
;	of stacks are maintained (constant NUM_STACKS), and the call 
;	is passed without a stack switch if all local stacks are in use. 
;
;	To avoid duplication of the stack switch and register save code 
;	in all handlers, a global stack switcher is implemented here. 
;	It pushes the address of a back-switch routine on the new stack, 
;	so the calling routine doesn't have to care about explicitly
;	calling the restore routine.
;
;	No registers may be pushed prior to the call to switch_stack if
;	the auto-switchback is to be used, since switch_stack assumes
;	the old stack to contain the callers flags at offset 4.
;
;	When old_stack is called to explicitly switch back, stack 
;	contents prior to the call to switch_stack are insignificant.
;
;	Switch_stack will set up registers as follows:
;		BP = base of register save area on stack
;		DS,ES = DGROUP
;		Direction flag clear
;		Interrupts enabled
;	All other registers are unchanged.
;
;	To allow preservation of the flag state on entry to switch_stack,
;	the flags are pushed on the save area, but not automatically
;	restored. Copy the entry_flags into caller_flags to restore
;	the entry flags on return.
;
;	old_stack will not restore flags from the stack, it will preserve
;	the flag state.
;
;	Version 2.1 slightly changes the handling of flag restoration
;	on auto-backswitch. The old algorithm restored the caller flags
;	on entry to the backswitch routine, which caused problems with
;	debuggers if the interrupted code was being single-stepped.
;	Now the original flags always will be restored on exit of the
;	back-switch, regardless of the return used (IRET or RET 2).
;	To modify the flags returned, you have to explicitly modify
;	the caller_flags field in the saved register area.
;
	name	tskstck
;
	include	tsk.mac
	include	tskdeb.h
;
	.tsk_model
;
	Pubfunc	tsk_switch_stack
	Pubfunc	tsk_old_stack
;
	IF	DEBUG AND DEB_FLASHERS
	Locext	tsk_inccdis
	extrn	tsk_debflash: word
	extrn	tsk_dtposn: dword
	ENDIF
;
;
STACKSIZE	=	256	; local stack size (bytes)
NUM_STACKS	=	8	; Number of local stacks
;
	IF	FAR_STACKS
ctask_stacks	segment word public 'CTASK_STACKS'
	ELSE
	.tsk_data
	ENDIF
;
lstacks	label	word
	db	NUM_STACKS * STACKSIZE dup (?)
;
	IF	FAR_STACKS
ctask_stacks	ends
	.tsk_data
	ENDIF
;
	IF	NOT ROM_CODE
;
	.tsk_edata
	.tsk_code
;
sseg	equ	<cs>
	ENDIF
;
stack_full	dw	0	; count 'all stacks in use' events
	IF	DEBUG AND DEB_FLASHERS
usecnt	db	0
	ENDIF
	EVEN
;
;
	IF	ROM_CODE
r_ds		dw	?
	ENDIF
r_ss		dw	?
r_sp		dw	?
r_bx		dw	?
r_cx		dw	?
r_bp		dw	?
call_ip		dw	?
call_cs		dw	?
retaoff		dw	?
	IF	LOCALS_FAR
retaseg		dw	?
	ENDIF
sflags		dw	?
;
;	Caution: Since a zero slot offset is used to detect
;	that no stack was allocated, the "stacks" array may not
;	be located at offset 0 in the segment.
;
stacks	label	word
xst	=	offset lstacks + STACKSIZE
	rept	NUM_STACKS
	dw	xst
xst	=	xst + STACKSIZE
	endm
;
	IF	ROM_CODE
;
	.tsk_edata
	.tsk_code
;
sseg	equ	<ds>
	ENDIF
;
tsk_dgroup	dw	@CTASK_DATA
;
	IF	FAR_STACKS
stackseg	dw	SEG ctask_stacks
	ELSE
stackseg	equ	<tsk_dgroup>
	ENDIF
;
;
Localfunc tsk_switch_stack
;
	IF	ROM_CODE
	push	ds
	mov	ds,cs:tsk_dgroup
	pop	r_ds
	ENDIF
	pushf
	cli
	pop	sseg:sflags
	pop	sseg:retaoff		; return address
	IF	LOCALS_FAR
	pop	sseg:retaseg
	ENDIF
	mov	sseg:r_bx,bx		; save bx & cx in CS
	mov	sseg:r_cx,cx
	mov	sseg:r_bp,bp		; save BP
	mov	bp,sp
	mov	bx,[bp]			; get caller's IP
	mov	sseg:call_ip,bx
	mov	bx,2[bp]		; get caller's CS
	mov	sseg:call_cs,bx
	mov	bp,4[bp]		; get caller's flags into BP
;
	mov	cx,NUM_STACKS		; total number of stacks
	mov	bx,offset stacks	; stack table
stlp:
	cmp	word ptr sseg:[bx],0	; in use ?
	jne	st_found		; jump if free
	inc	bx
	inc	bx
	loop	stlp
;
;	No unused stack, continue on caller's stack
;
	IF	DEBUG AND DEB_FLASHERS
	push	ds
	mov	ds,cs:tsk_dgroup
	cmp	tsk_debflash,0
	je	debdd0
	push	ax
	mov	ax,DEBP_CNTSTOFL
	call	tsk_inccdis
	pop	ax
debdd0:
	pop	ds
	ENDIF
	inc	sseg:stack_full
	xor	bx,bx
	jmp	short st_old
;
;	Stack found, perform switch
;
st_found:
	mov	sseg:r_ss,ss		; save SS/SP
	mov	sseg:r_sp,sp
	mov	ss,cs:stackseg		; load new SS/SP
	mov	sp,sseg:[bx]
	mov	cx,sp			; save new SP value
	mov	word ptr sseg:[bx],0	; mark stack in use
	push	sseg:r_ss			; push old SS/SP
	push	sseg:r_sp
st_old:
	push	sseg:r_bx
	push	sseg:r_cx
	push	bx			; push stack slot index
	push	cx			; push new SP
;
	push	ax			; push remaining regs
	push	dx
	push	si
	push	di
	push	sseg:r_bp
	IF	ROM_CODE
	push	r_ds
	ELSE
	push	ds
	ENDIF
	push	es
;
	push	bp			; push caller's flags
	push	sseg:call_cs		; push caller's CS
	push	sseg:call_ip		; push caller's IP
;
	push	sseg:sflags		; push entry flags
	push	cs			; and special return
	mov	cx,offset @stsw_retn
	push	cx
	mov	bx,sseg:r_bx		; restore regs bx,cx
	mov	cx,sseg:r_cx
	mov	bp,sp			; set BP to stack bottom
	IF	LOCALS_FAR
	push	sseg:retaseg
	ENDIF
	push	sseg:retaoff		; push retaddr on new stack
	cld
	sti
	mov	ds,cs:tsk_dgroup
	mov	es,cs:tsk_dgroup
	IF	DEBUG AND DEB_FLASHERS
	cmp	slot_idx[bp],0
	je	debdd1
	cmp	tsk_debflash,0
	je	debdd1
	inc	sseg:usecnt
	push	ax
	push	ds
	push	si
	mov	al,sseg:usecnt
	add	al,'0'
	lds	si,tsk_dtposn
	mov	DEBP_STACKNUM[si],al
	pop	si
	pop	ds
	pop	ax
debdd1:
	ENDIF
	ret				; restore flags & return
;
tsk_switch_stack	endp
;
;
Localfunc tsk_old_stack
;
	pushf				; save flags
	cli
	IF	ROM_CODE
	mov	ds,cs:tsk_dgroup
	ENDIF
	pop	sseg:sflags
	pop	sseg:retaoff		; return address
	IF	LOCALS_FAR
	pop	sseg:retaseg
	ENDIF
	add	sp,12			; discard dummy return & flags
;
	IF	DEBUG AND DEB_FLASHERS
	mov	ds,cs:tsk_dgroup
	cmp	slot_idx[bp],0
	je	debdd2
	cmp	tsk_debflash,0
	je	debdd2
	dec	sseg:usecnt
	mov	al,sseg:usecnt
	add	al,'0'
	lds	si,tsk_dtposn
	mov	DEBP_STACKNUM[si],al
	IF	ROM_CODE
	mov	ds,cs:tsk_dgroup
	ENDIF
debdd2:
	ENDIF
;
	pop	es			; restore regs
	IF	ROM_CODE
	pop	r_ds
	ELSE
	pop	ds
	ENDIF
	pop	bp
	pop	di
	pop	si
	pop	dx
	pop	ax
;
	pop	cx			; new SP
	pop	bx			; stack slot index
	or	bx,bx			; no new stack?
	jnz	old_sw
	pop	cx
	pop	bx
	jmp	short old_ret
old_sw:
	mov	sseg:[bx],cx		; reset stack slot
	pop	cx
	pop	bx
	pop	sseg:r_sp
	pop	ss   			; old SS
	mov	sp,sseg:r_sp
old_ret:
	IF	LOCALS_FAR
	push	sseg:retaseg
	ENDIF
	push	sseg:retaoff		; push retaddr on old stack
	push	sseg:sflags
	IF	ROM_CODE
	mov	ds,r_ds
	ENDIF
	popf				; restore flags
	ret				; and return
;
tsk_old_stack	endp
;
; stsw_retn is where the call returns to when there is no explicit
; call to old_stack.
;
@stsw_retn	proc	far
;
	cli
	add	sp,4			; discard caller CS/IP
	IF	ROM_CODE
	mov	ds,cs:tsk_dgroup
	ENDIF
	pop	sseg:sflags		; store caller's flags in CS
;
	IF	DEBUG AND DEB_FLASHERS
	mov	ds,cs:tsk_dgroup
	cmp	slot_idx[bp],0
	je	debdd3
	cmp	tsk_debflash,0
	je	debdd3
	dec	sseg:usecnt
	mov	al,sseg:usecnt
	add	al,'0'
	lds	si,tsk_dtposn
	mov	DEBP_STACKNUM[si],al
	IF	ROM_CODE
	mov	ds,cs:tsk_dgroup
	ENDIF
debdd3:
	ENDIF
;
	pop	es			; restore regs
	IF	ROM_CODE
	pop	r_ds
	ELSE
	pop	ds
	ENDIF
	pop	bp
	pop	di
	pop	si
	pop	dx
	pop	ax
;
	pop	cx			; new SP
	pop	bx			; stack slot index
	or	bx,bx			; no new stack?
	jnz	retn_sw
	pop	cx
	pop	bx
	jmp	short retn_ret
retn_sw:
	mov	sseg:[bx],cx		; reset stack slot
	pop	cx
	pop	bx
	pop	sseg:r_sp
	pop	ss   			; old SS
	mov	sp,sseg:r_sp
retn_ret:
	push	bp
	mov	bp,sp
	push	ax
	mov	ax,sseg:sflags		; push flags on old stack
	mov	6[bp],ax
	pop	ax
	pop	bp
	IF	ROM_CODE
	mov	ds,r_ds
	ENDIF
	iret
;
@stsw_retn	endp
;
	.tsk_ecode
	end

