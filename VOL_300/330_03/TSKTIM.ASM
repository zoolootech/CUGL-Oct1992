;
;	--- Version 2.2 90-10-12 10:46 ---
;
;	CTask - Timer interrupt handler (IBM specific)
;
;	Public Domain Software written by
;		Thomas Wagner
;		Ferrari electronic Gmbh
;		Beusselstrasse 27
;		D-1000 Berlin 21
;		Germany
;
;	NOTE: Logic in this module has been changed to accommodate two
;	      different ways to handle chaining to the original INT 8
;	      entry in version 1.2.
;
;	CAUTION: This module can only be installed in the primary kernel.
;		 It is not ROMable.
;
;	For non-IBM applications, you have to completely replace
;	this module and write your own.
;	You should be able to use this module at least as a model
;	by throwing out about 80%, mainly the initialization 
;	and interrupt chaining stuff. All you really need is the
;	tsk_timer_counter increment, and the ticker processing,
;	which can be left unchanged. The section enclosed by
;	IF INT8_EARLY most likely is the one you should use as 
;	reference, since you will usuallly not have other routines 
;	chained to the timer tick in an embedded system.
;
	name	tsktim
;
	include	tsk.mac
	include	tskdeb.h
;
	.tsk_model
;
	Pubfunc	tsk_install_timer
	Pubfunc	tsk_remove_timer
	IF	NOT INT8_EARLY
	Pubfunc	tsk_chain_timer
	ENDIF
;
	extrn	sched_int: far
	Globext	inc_counter
	Globext	set_flag
;
	extrn	tsk_key_avail: byte
	extrn	tsk_glob_rec: byte
	extrn	tsk_timer_counter:word
	extrn	tsk_instflags: word
	IF	NOT INT8_EARLY
	extrn	tsk_int8_counter:word
	ENDIF
;
	IF	DEBUG AND DEB_FLASHERS
	Locext	tsk_inccdis
	extrn	tsk_debflash: word
	extrn	tsk_dtposn: dword
	ENDIF
;
;
STACKSIZE	=	128	; local stack size (words)
;
timer	equ	40h			; 8253 timer base I/O address
inta00	equ	20h			; 8259 int controller base
eoi	equ	20h			; unspecific EOI
;
intseg	segment at 0
;
		org	8*4
tintofs		dw	?		; timer interrupt entry
tintseg		dw	?

intseg	ends
;
biosdataseg	segment at 40h
	org	1ah
kbdptr1	dw	?
kbdptr2	dw	?
biosdataseg	ends
;
	IF	FAR_STACKS
ctask_stacks	segment word public 'CTASK_STACKS'
	ELSE
	.tsk_data
	ENDIF
;
	dw	STACKSIZE dup(?)
local_stack	label	word
;
	IF	FAR_STACKS
ctask_stacks	ends
	.tsk_data
	ENDIF
;
divisor		dw	?
timflag		dw	?
timcnt		dw	?
sys_ticks	dw	?
;
	.tsk_edata
	.tsk_code
;
tsk_dgroup	dw	@CTASK_DATA
;
	IF	FAR_STACKS
stackseg	dw	SEG ctask_stacks
	ELSE
stackseg	equ	<tsk_dgroup>
	ENDIF
;
timer_save	label	dword		; in CSEG to allow addressing
tsofs		dw	?
tsseg		dw	?
;
r_ss		dw	?
r_sp		dw	?
in_timer	db	0
;
;----------------------------------------------------------------------
;
 	IF	NOT INT8_EARLY
;
;	Timer interrupt handler, late INT 8 processing
;
;	Normal timer tick. The tick counter is incremented, and
;	the interrupt controller is checked for other pending interrupts.
;	If the timer tick occurred during processing of another interrupt,
;	we may not call the scheduler, since this would delay the
;	interrupt handler.
;
;	Note that an EOI is issued here to allow interrupts to occur
;	during further processing. The original INT 8 handler will be
;	chained to from a special task. The reason behind this is that
;	some TSR's link into the timer interrupt and may do some lengthy
;	processing there. To allow the TSR to be preempted, we must use
;	a task for the INT 8 processing.
;
@timer_int_late	proc	far
;
;	first, check for overrun. If we're already processing a tick,
;	we simply ignore this one. That's not that great a way to
;	react, but anything else would easily mess things up, and we're
;	already in trouble anyway if this should happen.
;
	cmp	cs:in_timer,0
	je	timer_ok
	push	ax
	mov	al,eoi			; issue EOI
	out	inta00,al
	pop	ax
	iret
;
;	Not a second interrupt, continue processing. First, set the
;	in_timer marker, and switch to our local stack.
;
timer_ok:
	inc	cs:in_timer
	mov	cs:r_ss,ss
	mov	cs:r_sp,sp
	mov	ss,cs:stackseg
	mov	sp,offset local_stack
	sti
	cld
	push	ax
	push	ds
	mov	ds,cs:tsk_dgroup
;
	IF	DEBUG AND DEB_FLASHERS
	cmp	tsk_debflash,0
	je	debdd0
	mov	ax,DEBP_CNTTICK
	call	tsk_inccdis
debdd0:
	ENDIF
;
	push	es			; save other regs
	push	bx
	push	cx
	push	dx
	push	si
	push	di
	push	bp
	callp	inc_counter,<<ds,#tsk_timer_counter>> ; increase timer tick counter
;
;	Now the timer counter is decremented. If it is zero,
;	we must chain to the original INT 8, so the counter for
;	the chain task is incremented.
;
	dec	timcnt			; decrement tick count
	jnz	no_pass			; pass on this int if zero
;
	mov	ax,sys_ticks
	mov	timcnt,ax		; re-init tick counter
;
	callp	inc_counter,<<ds,#tsk_int8_counter>>
;
;	Now we decrement all installed tick counters 
;
no_pass:
	les	bx,tsk_glob_rec.ticker_chain
;
count_dec:
	mov	ax,es
	or	ax,bx
	jz	tick_ready
	mov	ax,es:ticklo[bx]
	or	ax,es:tickhi[bx]
	jz	count_next
	sub	es:ticklo[bx],1
	sbb	es:tickhi[bx],0
count_next:
	les	bx,es:ticknext[bx]
	jmp	count_dec
;
;	Now check the BIOS keyboard buffer.
;	If there are chars in the buffer, set the tsk_key_avail flag
;	to wake up tasks waiting for the keyboard. This check is required
;	to find out about keyboard stuffers placing characters directly
;	into the keyboard buffer.
;
tick_ready:
	mov	ax,SEG biosdataseg
	mov	es,ax
	assume	es:biosdataseg
	mov	ax,es:kbdptr1
	cmp	ax,es:kbdptr2
	assume	es:@CTASK_DATA
	je	tick_exit
	callp	set_flag,<<ds,#tsk_key_avail>>
;
tick_exit:
	pop	bp
	pop	di
	pop	si
	pop	dx
	pop	cx
	pop	bx
	pop	es
;
	cli
	mov	al,eoi			; issue EOI
	out	inta00,al
	mov	al,0bh			; access int control reg
	out	inta00,al
	in	al,inta00		; ints pending?
	or	al,al
	jnz	tim_retn		; don't schedule if other ints active
	IF	DEBUG AND DEB_FLASHERS
	cmp	tsk_debflash,0
	je	debdda1
	push	bx
	lds	bx,tsk_dtposn
	mov	byte ptr [bx+DEBP_CNTTICK+DEBFLASH_NDIGS*2],'+'
	pop	bx
debdda1:
	ENDIF
	pop	ds
	pop	ax
	mov	ss,cs:r_ss
	mov	sp,cs:r_sp
	dec	cs:in_timer
	jmp	sched_int		; else schedule
;
tim_retn:
	IF	DEBUG AND DEB_FLASHERS
	cmp	tsk_debflash,0
	je	debdda2
	push	bx
	lds	bx,tsk_dtposn
	mov	byte ptr [bx+DEBP_CNTTICK+DEBFLASH_NDIGS*2],'-'
	pop	bx
debdda2:
	ENDIF
	pop	ds
	pop	ax
	mov	ss,cs:r_ss
	mov	sp,cs:r_sp
	dec	cs:in_timer
	iret
;
@timer_int_late	endp
;
	ENDIF
;
;----------------------------------------------------------------------
;
 	IF	NOT INT8_LATE
;
;	Timer interrupt handler, early INT 8 processing
;
;	With this interrupt handler, the original INT 8 is chained to
;	directly, before calling the scheduler.
;
;	This avoids compatibility problems with TSR's that do strange
;	things in the timer tick, but it may lead to problems within
;	CTask, should the strange TSR decide to loop in the INT 8 handler,
;	or even to never return. Oh well, you can't please all TSR's all
;	of the time...
;
@timer_int_early	proc	far
;
	cld
	push	ax
	push	ds
	mov	ds,cs:tsk_dgroup
;
	IF	DEBUG AND DEB_FLASHERS
	cmp	tsk_debflash,0
	je	debdd1
	mov	ax,DEBP_CNTTICK
	call	tsk_inccdis
debdd1:
	ENDIF
;
;	The timer counter is decremented. If it is zero,
;	we chain directly to the original INT 8.
;	Since the local stack is not yet in use, 
;	multiple entries into this part are possible.
;
	xor	ah,ah			; no delayed tick
	dec	timcnt			; decrement tick count
	jnz	eno_pass
;
	mov	ax,sys_ticks
	mov	timcnt,ax		; re-init tick counter
;
;	Version 2.1 adds a check for scheduler active here.
;	We may not call the original INT 8 if the scheduler is active,
;	since this would crash the system when the old INT 8 causes
;	a task_wait. See tskasm.asm for a more detailed explanation.
;
	cmp	tsk_glob_rec.in_sched,0
	je	call_oldint
;
	mov	ah,1			; mark delayed tick
	jmp	short eno_pass
;
call_oldint:
	sti
	pop	ds
	pop	ax
	pushf
	call	cs:timer_save		; call original INT 8
	push	ax
	push	ds
	mov	ds,cs:tsk_dgroup
	xor	ah,ah			; no delayed tick
	jmp	short e_pass
;
;	On return from INT 8 processing, or when INT 8 was not chained,
;	processing is pretty much the same as in timer_int_late.
;
eno_pass:
	mov	al,eoi			; issue EOI when not chained
	out	inta00,al
;
e_pass:
	cli
	cmp	cs:in_timer,0
	je	etimer_ok
	pop	ds
	pop	ax
	iret
;
etimer_ok:
	inc	cs:in_timer
	mov	cs:r_ss,ss
	mov	cs:r_sp,sp
	mov	ss,cs:stackseg
	mov	sp,offset local_stack
	sti
	cld
;
	push	es			; save other regs
	push	bx
	push	cx
	push	dx
	push	si
	push	di
	push	bp
;
;	If the tick was delayed, we have to notify the tsk_int8 to
;	process an additional tick when there's time.
;
	IF	NOT INT8_EARLY
	or	ah,ah			; check delay flag
	jz	no_delay
	callp	inc_counter,<<ds,#tsk_int8_counter>>
	ENDIF
;
no_delay:
	callp	inc_counter,<<ds,#tsk_timer_counter>> ; increase timer tick counter
;
;	Now we decrement all installed tick counters 
;
	les	bx,tsk_glob_rec.ticker_chain
;
ecount_dec:
	mov	ax,es
	or	ax,bx
	jz	etick_ready
	mov	ax,es:ticklo[bx]
	or	ax,es:tickhi[bx]
	jz	ecount_next
	sub	es:ticklo[bx],1
	sbb	es:tickhi[bx],0
ecount_next:
	les	bx,es:ticknext[bx]
	jmp	ecount_dec
;
;	Now check the BIOS keyboard buffer.
;	If there are chars in the buffer, set the tsk_key_avail flag
;	to wake up tasks waiting for the keyboard. This check is required
;	to find out about keyboard stuffers placing characters directly
;	into the keyboard buffer.
;
etick_ready:
	mov	ax,SEG biosdataseg
	mov	es,ax
	assume	es:biosdataseg
	mov	ax,es:kbdptr1
	cmp	ax,es:kbdptr2
	assume	es:@CTASK_DATA
	je	etick_exit
	callp	set_flag,<<ds,#tsk_key_avail>>
;
etick_exit:
	pop	bp
	pop	di
	pop	si
	pop	dx
	pop	cx
	pop	bx
	pop	es
;
	cli
	mov	al,0bh			; access int control reg
	out	inta00,al
	in	al,inta00		; ints pending?
	or	al,al
	jnz	etim_retn		; don't schedule if other ints active
	mov	ss,cs:r_ss
	mov	sp,cs:r_sp
	pop	ds
	pop	ax
	dec	cs:in_timer
	jmp	sched_int		; else schedule
;
etim_retn:
	mov	tsk_glob_rec.pretick,1	; mark that we missed a schedule
	mov	ss,cs:r_ss
	mov	sp,cs:r_sp
	pop	ds
	pop	ax
	dec	cs:in_timer
	iret
;
@timer_int_early	endp
;
	ENDIF
;
;---------------------------------------------------------------------
;
;	Install timer.
;	The scheduler is not called on this first tick.
;
@tim_install	proc	far
;
	push	ax
	push	ds
	push	es
;
	mov	ds,cs:tsk_dgroup
;
	mov	timflag,0		; signal init ready
	mov	ax,sys_ticks
	mov	timcnt,ax		; init tick counter
	mov	al,36h
	out	timer+3,al		; setup to load divisor
	mov	al,byte ptr divisor
	out	timer,al		; lsb
	mov	al,byte ptr divisor+1
	out	timer,al		; msb
;
	xor	ax,ax
	mov	es,ax
	assume	es:intseg
;
	IF	INT8_EARLY
	mov	ax,offset @timer_int_early
	ELSE
	mov	ax,offset @timer_int_late
	IF	NOT INT8_LATE
	test	tsk_instflags,IFL_INT8_DIR
	jz	tim_inst_1
	mov	ax,offset @timer_int_early
	ENDIF
	ENDIF
;
tim_inst_1:
	mov	es:tintofs,ax
	mov	es:tintseg,cs
;
	pop	es
	pop	ds
	pop	ax
	jmp	cs:timer_save
;
@tim_install	endp
;
;------------------------------------------------------------------------
;
;       Un-Install timer (wait until system tick count reached).
;	The scheduler is not called while waiting for the tick count.
;
@tim_uninstall	proc	far
;
	push	ax
	push	ds
;
	mov	ds,cs:tsk_dgroup
;
	cli
	dec	timcnt			; decrement tick count
	jz	uninit			; go un-install if zero
	mov	al,eoi			; else just issue EOI
	out	inta00,al
	pop	ds
	pop	ax
	iret
;
;	Uninstall timer int handler
;
uninit:
	mov	timflag,0		; mark un-install complete
	mov	al,36h			; setup to load divisor
	out	timer+3,al
	mov	al,0			; divisor 0 means 65536
	out	timer,al		; lsb
	out	timer,al		; msb
;
	push	es
	xor	ax,ax
	mov	es,ax
	assume	es:intseg
	mov	ax,cs:tsofs		; restore vector
	mov	tintofs,ax
	mov	ax,cs:tsseg
	mov	tintseg,ax
	pop	es
	pop	ds
	pop	ax
	jmp	cs:timer_save		; pass on interrupt
;
@tim_uninstall	endp
;
;----------------------------------------------------------------------
;
	IF	NOT INT8_EARLY
;
;	void far tsk_chain_timer (void)
;
;       Pass timer tick on to interrupt 8 chain.
;
Localfunc tsk_chain_timer
;
	IF	DEBUG AND DEB_FLASHERS
	cmp	tsk_debflash,0
	je	debdd2
	mov	ax,DEBP_CNTTCHAIN
	call	tsk_inccdis
debdd2:
	ENDIF
	pushf
	cli
	call	cs:timer_save
	ret
;
tsk_chain_timer	endp
;
	ENDIF
;
;
;	void near tsk_install_timer (word divisor, word sys_ticks)
;
;	This routine installs the timer tick int handler.
;	The timer chip is reprogrammed on the next tick.
;
Localfunc tsk_install_timer,<pdivisor: word, psysticks: word>
;
	IFDEF	LOAD_DS
	push	ds
	mov	ds,cs:tsk_dgroup
	ENDIF
;
	mov	ax,pdivisor
	mov	divisor,ax
	mov	ax,psysticks
	mov	sys_ticks,ax
	mov	timflag,1		; set init-flag
	xor	ax,ax
	mov	es,ax			; establish addressing for intseg
	assume	es:intseg
;
	mov	ax,tintofs		; save old timer int addr
	mov	tsofs,ax
	mov	ax,tintseg
	mov	tsseg,ax
	cli
	mov	tintofs,offset @tim_install ; set new timer int addr
	mov	tintseg,cs
	sti
	assume	es:nothing
wait_set:
	cmp	timflag,0		; wait until timer started
	jne	wait_set
	IFDEF	LOAD_DS
	pop	ds
	ENDIF
	ret
;
tsk_install_timer	endp
;
;
;	void far tsk_remove_timer (void)
;
;	This routine un-installs the timer tick int handler.
;	The timer chip is reprogrammed & the interrupt vector
;	restored when the system tick count reaches zero.
;
Localfunc tsk_remove_timer
;
	IFDEF	LOAD_DS
	push	ds
	mov	ds,cs:tsk_dgroup
	ENDIF
;
	mov	timflag,2		; set un-init flag for timer

	xor	ax,ax
	mov	es,ax			; establish addressing for intseg
	assume	es:intseg
;
	cli
	mov	tintofs,offset @tim_uninstall ; set new timer int addr
	mov	tintseg,cs
	sti
	assume	es:nothing
wait_tim:
        sti                             ; just to be safe
	cmp	timflag,0		; wait until int un-installed
	jne	wait_tim
	IFDEF	LOAD_DS
	pop	ds
	ENDIF
	ret
;
tsk_remove_timer	endp
;
	.tsk_ecode
	end

