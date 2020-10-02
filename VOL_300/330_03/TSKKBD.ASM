;
;	--- Version 2.2 90-10-12 14:51 ---
;
;	CTask - Keyboard handler module.
;
;	Public Domain Software written by
;		Thomas Wagner
;		Ferrari electronic Gmbh
;		Beusselstrasse 27
;		D-1000 Berlin 21
;		Germany
;
;	This module traps the keyboard interrupts to allow task switching
;	on waiting for a character.
;	To avoid problems with programs that access the keyboard buffer
;	directly instead of going through INT 16, the logic has been changed
;	in version 1.2. The keyboard characters are no longer placed into
;	a pipe, instead the keyboard hardware interrupt just sets a flag
;	to signal that there might be something in the buffer. The keyboard
;	read routines wait on this flag if the original INT 16 status call
;	indicates that no key is available.
;
;	Note that there is a slight chance of this logic leading to busy
;	waiting in the original INT 16. This could happen if the process
;	is interrupted between the status check and the actual keyboard
;	fetch, and the interrupting routine snatches away the keystroke.
;	Since this is not very likely to occur, and would not be fatal
;	anyway, it would be overkill to try to avoid this.
;
;	If INT 16 is entered via CTask's t_read_key and t_wait_key,
;	the stack is not switched.
;
;	In Version 2.1, the keyboard access routines (t_read_key etc.) 
;	were moved to file 'tsksec.asm' to save code space in secondary
;	kernels. Also, the bug that the stack was always switched, contrary
;	to what the above paragraph said, was fixed.
;	The t_xxx routines now use the extended keyboard functions
;	if available. Function 5 (stuff keyboard buffer) is intercepted
;	to set the tsk_key_avail flag.
;
;	Version 2.1 adds hotkey processing to the INT 9 frontend.
;	It would be "nicer" to process hotkeys in the INT 15 scancode
;	intercept BIOS hook, but this hook is not present in older
;	XT and AT BIOSes. Version 2.2 will use the INT 15 entry if the
;	BIOS indicates that it does support it.
;
;	CAUTION: This module can only be installed in the primary kernel.
;		 It is not ROMable.
;
	name	tskkbd
;
	include	tsk.mac
	include	tskdeb.h
;
	.tsk_model
;
	Pubfunc	tsk_install_kbd
	Pubfunc	tsk_remove_kbd
;
	public	tsk_key_avail
;
	Globext	create_flag
	Globext	delete_flag
	Globext	set_flag
	Globext	clear_flag
	Globext	wait_flag_set
	extrn	sched_int: far
	Locext	tsk_switch_stack
	Locext	tsk_old_stack
	Locext	tsk_timer_action
	Locext	tsk_dequeue
	Locext	tsk_putqueue
;
	IF	DEBUG AND DEB_FLASHERS
	Locext	tsk_inccdis
	extrn	tsk_debflash: word
	ENDIF
;
	extrn	tsk_glob_rec: byte
;
inta00		=	20h	; 8259 int controller base
eoi		=	20h	; unspecific EOI
;
keyb_data	=	60h	; keyboard data port
keyb_ctl	=	61h	; keyboard control (PC/XT only)
;
intseg	segment at 0
		org	09h*4
hwdoff		dw	?	; keyboard hardware interrupt
hwdseg		dw	?
		org	15h*4
int15off 	dw	?	; system services interrupt
int15seg 	dw	?
		org	16h*4
kbdoff		dw	?	; keyboard I/O interrupt
kbdseg		dw	?
;
intseg	ends
;
biosdata	segment at 40h
		org	17h
keyb_flags_1	db	?
keyb_flags_2	db	?
		org	96h
keyb_flags_3	db	?
;
biosdata	ends
;
;----------------------------------------------------------------------------
;
;	Variables
;
	.tsk_data
;
	IF	TSK_NAMEPAR
kbd_name	db	"KEYAVAIL",0
	ENDIF
;
tsk_key_avail	flag <>
;
	.tsk_edata
	.tsk_code
;
;
;	Original Interrupt-Entries
;
savhwd		label	dword		; original hardware int entry
savhwdoff	dw	?
savhwdseg	dw	?
;
savkbd		label	dword		; original keyboard I/O entry
savkbdoff	dw	?
savkbdseg	dw	?
;
savint15	label	dword		; original system services entry
savint15off	dw	?
savint15seg	dw	?
;
ext_keyboard	db	0		; extended keyboard BIOS present
kb_intercept	db	0		; keyboard intercept is called
sched_pending	db	0		; scheduler call is pending
;
;---------------------------------------------------------------------------
;
;	check_hotkey checks the hotkey queue passed in ES:BX for
;	a match. If there is a match, the associated action is
;	performed (via tsk_timer_action, since hotkey elements are
;	essentially timer elements).
;
;	Returns Carry set if no match, Carry clear on match.
;
	IF	HOTKEYS
;
@check_hotkey	proc	near
;
	push	ds
;
	mov	cx,SEG biosdata
	mov	ds,cx
	assume	ds:biosdata		; for checking the flags
;
	cmp	es:telem.scancode[bx],0
	je	no_scancomp
;
checkhotloop:
	cmp	es:telem.scancode[bx],al
	jne	hot_next			; no more checks on mismatch
;
no_scancomp:
	cmp	es:telem.kbflags1.hf_mask[bx],0		; check flag 1?
	je	no_flcomp1				; jump if not
;
	mov	ah,keyb_flags_1
	and	ah,es:telem.kbflags1.hf_mask[bx]
	cmp	ah,es:telem.kbflags1.hf_value[bx]
	jne	hot_next			; no more checks on mismatch
;
no_flcomp1:
	cmp	es:telem.kbflags2.hf_mask[bx],0		; check flag 2?
	je	no_flcomp2		       		; jump if not
;
	mov	ah,keyb_flags_2
	and	ah,es:telem.kbflags2.hf_mask[bx]
	cmp	ah,es:telem.kbflags2.hf_value[bx]
	jne	hot_next			; no more checks on mismatch
;
no_flcomp2:
	cmp	es:telem.kbflags3.hf_mask[bx],0		; check flag 3?
	je	do_hotkey		       		; match if not
;
	mov	ah,keyb_flags_3
	and	ah,es:telem.kbflags3.hf_mask[bx]
	cmp	ah,es:telem.kbflags3.hf_value[bx]
	je	do_hotkey			; match
;
hot_next:
	les	bx,es:tlink.q_next[bx]		; next in queue
	test	es:q_kind[bx],Q_HEAD		; queue end?
	jnz	no_hotkey
	cmp	es:telem.scancode[bx],0
	je	no_scancomp
	jmp	checkhotloop
;
no_hotkey:
	sti
	pop	ds
	assume	ds:@CTASK_DATA
	stc
	ret
;
;	Execute hotkey action. First, remove from queue to allow
;	enable/disable/delete calls in the hotkey action.
;
do_hotkey:
	pop	ds
	cli
	or	es:tflags[bx],TFLAG_BUSY	; mark busy
	push	si
	push	es
	push	bx
	callp	tsk_dequeue,<<es,bx>>		; remove
	sti
	pop	bx
	pop	es
;
	push	es
	push	bx
	callp	tsk_timer_action,<<es,bx>>
	pop	bx
	pop	es
	pop	si
;
;	If continued hotkey, re-enqueue.
;
	cli
	test	es:tflags[bx],TFLAG_UNQUEUE OR TFLAG_REMOVE
	jnz	no_enque
	test	es:tflags[bx],TFLAG_ENQUEUE OR TFLAG_REPEAT
	jz	no_enque
	push	es
	push	bx
	callp	tsk_putqueue,<<ds,si>,<es,bx>>
	pop	bx
	pop	es
;
no_enque:
	and	es:tflags[bx],NOT (TFLAG_BUSY OR TFLAG_UNQUEUE OR TFLAG_ENQUEUE)
	sti
; 	carry is now clear
	ret
;
@check_hotkey	endp
;
	ENDIF
;
;---------------------------------------------------------------------------
;
;	void tsk_install_kbd (void)
;
;		Install keyboard handler
;
Localfunc tsk_install_kbd
;
	IFDEF	LOAD_DS
	push	ds
	mov	ax,@CTASK_DATA
	mov	ds,ax
	ENDIF
;
;	Check for extended keyboard BIOS functions. Since there is no
;	error return when executing the functions in non-extended BIOS
;	versions, we have to do a little guesswork.
;
	mov	cs:ext_keyboard,0
;
	push	ds
	mov	ax,40h
	mov	ds,ax
	assume	ds:biosdata
;
	mov	ax,11ffh	; this certainly is no valid key
	int	16h		; get extended status
	cmp	ax,11ffh	; has the value changed?
	je	not_extended	; if not, it's surely not extended.
	cli
	mov	bl,keyb_flags_1
	mov	ah,12h
	int	16h
	cmp	bl,al
	jne	not_extended
	not	al
	mov	keyb_flags_1,al
	mov	ah,12h
	int	16h
	mov	keyb_flags_1,bl
	sti
	not	al
	cmp	bl,al
	jne	not_extended
;
;	An extended keyboard BIOS is present. This BIOS likely also supports
;	the INT 15 intercept hook, so check for it with the "return config
;	parameters" call.
;
	inc	cs:ext_keyboard
	IF	HOTKEYS
	mov	ah,0c0h
	int	15h
	jc	not_extended
	or	ah,ah
	jnz	not_extended
	cmp	word ptr es:[bx],8
	jb	not_extended
	test	byte ptr es:5[bx],10h	; keyboard intercept present
	jz	not_extended
	inc	cs:kb_intercept
	ENDIF
;
not_extended:
	sti
	pop	ds
;
	assume	ds:@CTASK_DATA
;
	IF	TSK_NAMEPAR
	callp	create_flag,<<ds,#tsk_key_avail>,<ds,#kbd_name>>
	ELSE
	callp	create_flag,<<ds,#tsk_key_avail>>
	ENDIF
;
;	Save old interrupt vectors
;
        push    es
	xor	ax,ax
	mov	es,ax
;
        assume  es:intseg
;
	mov	ax,kbdoff
	mov	savkbdoff,ax
	mov	ax,kbdseg
	mov	savkbdseg,ax
;
	mov	ax,hwdoff
	mov	savhwdoff,ax
	mov	ax,hwdseg
	mov	savhwdseg,ax
;
	IF	HOTKEYS
	cmp	cs:kb_intercept,0
	je	enter_new
	mov	ax,int15off
	mov	savint15off,ax
	mov	ax,int15seg
	mov	savint15seg,ax
	cli
	mov	int15off,offset @intercept
	mov	int15seg,cs
	ENDIF
;
;	Enter new Interrupt-Entries
;
enter_new:
	cli
	mov	kbdoff,offset @kbdentry
	mov	kbdseg,cs
	mov	hwdoff,offset @hwdentry
	mov	hwdseg,cs
	sti
        pop     es
;
	IFDEF	LOAD_DS
	pop	ds
	ENDIF
	ret
;
	assume	es:nothing
;
tsk_install_kbd	endp
;
;
;	void tsk_remove_kbd (void)
;
;		Un-install keyboard handler
;
Localfunc tsk_remove_kbd
;
	IFDEF	LOAD_DS
	push	ds
	mov	ax,@CTASK_DATA
	mov	ds,ax
	ENDIF
;
        push    es
	xor	ax,ax
	mov	es,ax
;
        assume  es:intseg
;
;	Restore interrupt entries
;
	cli
;
	mov	ax,savkbdoff
	mov	kbdoff,ax
	mov	ax,savkbdseg
	mov	kbdseg,ax
;
	mov	ax,savhwdoff
	mov	hwdoff,ax
	mov	ax,savhwdseg
	mov	hwdseg,ax
;
	IF	HOTKEYS
	cmp	cs:kb_intercept,0
	je	rest_ready
	mov	ax,savint15off
	mov	int15off,ax
	mov	ax,savint15seg
	mov	int15seg,ax
	ENDIF
;
rest_ready:
	sti
;
        pop     es
;
;	Delete the keyboard available flag
;
	callp	delete_flag,<<ds,#tsk_key_avail>>
;
	IFDEF	LOAD_DS
	pop	ds
	ENDIF
	ret
;
	assume	es:nothing
;
tsk_remove_kbd	endp
;
;
;
;---------------------------------------------------------------------------
;---------------------------------------------------------------------------
;
;	INT 9 - Keyboard hardware interrupt
;
;	Version 2.1 adds hotkey processing.
;
@hwdentry	proc	far
;
        call    tsk_switch_stack
	IF	DEBUG AND DEB_FLASHERS
	cmp	tsk_debflash,0
	je	debdd0
	mov	ax,DEBP_CNTKEYBD
	call	tsk_inccdis
debdd0:
	ENDIF
;
;	Check the scancode hotkey queue.
;	Two queues are maintained for hotkeys, one for hotkey elements
;	with nonzero scancode, and one for zero scancode elements.
;	If there is no scancode, the hotkey is a shift-key combination,
;	which can only be checked *after* chaining to the old INT 9.
;	Hotkeys with scancode have to be checked *before* chaining,
;	so the scancode can be removed on a match.
;	If the BIOS supports keyboard intercept, this part is skipped.
;
	IF	HOTKEYS
	cmp	cs:kb_intercept,0
	jne	no_firstcheck
	lea	si,tsk_glob_rec.hotkey_scan.q_first
	les	bx,dword ptr [si]
	test	es:q_kind[bx],Q_HEAD	; queue empty?
	jnz	no_firstcheck		; then don't read key
	in	al,keyb_data
	call	@check_hotkey
	jnc	hotkey_found		; remove scancode on match
;
no_firstcheck:
	ENDIF
;
	pushf
        cli
	call	cs:savhwd		; let original handler process key
;
	callp	set_flag,<<ds,#tsk_key_avail>>
;
;	check no-scancode hotkeys
;
	IF	HOTKEYS
	lea	si,tsk_glob_rec.hotkey_noscan.q_first
	les	bx,dword ptr [si]
	test	es:q_kind[bx],Q_HEAD	; queue empty?
	jnz	no_nshot		; don't check if yes
	call	@check_hotkey
	jnc	immed_sched		; schedule on hotkey match
;
no_nshot:
	cmp	cs:sched_pending,0
	jne	immed_sched
	ENDIF
;
	iret
;
	IF	HOTKEYS
hotkey_found:
;
;	Acknowledge keyboard, so hotkey disappears.
;
	cli
	in	al,keyb_ctl
	mov	ah,al
	or	al,80h
	out	keyb_ctl,al
	xchg	ah,al
	out	keyb_ctl,al
;
	mov	al,eoi
	out	inta00,al
	sti
;
;	on a hotkey match, we schedule immediately.
;
immed_sched:
	cli
	mov	cs:sched_pending,0
	mov	al,0bh			; access int control reg
	out	inta00,al
	in	al,inta00		; ints pending?
	or	al,al
	jnz	no_immed		; don't schedule if other ints active
	sti
	call	tsk_old_stack
	jmp	sched_int
;
no_immed:
	iret
	ENDIF
;
@hwdentry	endp
;
;
	IF	HOTKEYS
;
@intercept	proc	far
;
	cmp	ah,4fh
	je	do_intercept
	jmp	cs:savint15
;
do_intercept:
	pushf
	sti
	cld
	push	ds
	push	es
	push	si
	push	di
	push	bx
	push	ax
	mov	si,@CTASK_DATA
	mov	ds,si
;
	lea	si,tsk_glob_rec.hotkey_scan.q_first
	les	bx,dword ptr [si]
	test	es:q_kind[bx],Q_HEAD	; queue empty?
	jz	inter_check
	stc
	jmp	short inter_ret
;
inter_check:
	push	cx
	push	dx
	call	@check_hotkey
	pop	dx
	pop	cx
;
inter_ret:
	pop	ax
	pop	bx
	pop	di
	pop	si
	pop	es
	pop	ds
	jc	inter_chain
	popf
	mov	cs:sched_pending,1
	clc
	ret	2
;
inter_chain:
	popf
	jmp	cs:savint15
;
@intercept	endp
;
	ENDIF
;
;---------------------------------------------------------------------------
;---------------------------------------------------------------------------
;
;	INT 16 - Keyboard I/O
;
@kbdentry	proc	far
;
        pushf
	sti
	or	ah,ah
	jz	kbd_read
	cmp	ah,10h
	jz	kbd_read_ext	; extended read
	cmp	ah,05h
	jz	kbd_stuff	; stuff char in key buffer
	cmp	ax,4012h
	jz	kbd_readns
	cmp	ax,4112h
	jz	kbd_readns
	cmp	ax,4212h
	jz	kbd_keyhit
;
kbd_pass:
        popf
	jmp	cs:savkbd	; pass on functions != 0
;
;	The "4212" code is used by t_keyhit. It will execute function
;	1 or 11h depending on ext_keyboard.
;
kbd_keyhit:
	mov	ah,1
	cmp	cs:ext_keyboard,0
	je	kbd_pass
	mov	ah,11h
	jmp	kbd_pass
;
;	The "4012" and "4112" codes are used by t_wait_key and t_read_key.
;	The t_wait_key code 4012 uses the timeout supplied in CX:DX.
;	The t_read_key code 4112 uses an endless timeout (0L).
;
kbd_readns:
	popf			;2.1a
	sti			;2.1a
	push	ds		;2.1a
	mov	bx,@CTASK_DATA	;2.1a
	mov	ds,bx		;2.1a
	mov	bx,8001h
	cmp	cs:ext_keyboard,0
	je	readns1
	mov	bx,9011h
readns1:
;2.1a	popf
	cmp	ah,40h
	jne	kbd_read2
	jmp	short kbr_loop
;
kbd_stuff:
        popf
	call	cs:savkbd
	call	tsk_switch_stack
	mov	ax,entry_flags[bp]
	mov	caller_flags[bp],ax
	callp	set_flag,<<ds,#tsk_key_avail>>
	iret
;
kbd_read_ext:
	cmp	cs:ext_keyboard,0
	je	kbd_pass		; pass on if no extended kbd
	mov	al,11h
	jmp	short kbd_read1
;
kbd_read:
	mov	al,1
kbd_read1:
        popf
	call	tsk_switch_stack
	mov	bx,ax
kbd_read2:
	xor	cx,cx
	mov	dx,cx
;
kbr_loop:
	push	bx
	push	cx
	push	dx
	callp	clear_flag,<<ds,#tsk_key_avail>>
	pop	dx
	pop	cx
	pop	bx
	mov	ah,bl
	pushf
	cli
	call	cs:savkbd
	jnz	kbr_get_key
	push	bx
	push	cx
	push	dx
	callp	wait_flag_set,<<ds,#tsk_key_avail>,<cx,dx>>
	pop	dx
	pop	cx
	pop	bx
	or	ax,ax
	jz	kbr_loop
	mov	ax,-1
	jmp	short kbr_retn
;
kbr_get_key:
	mov	ah,bh
	and	ah,7fh
	pushf
	cli
	call	cs:savkbd
kbr_retn:
	test	bh,80h
	jnz	kbr_retns
	mov	save_ax[bp],ax
	iret			;2.1a
kbr_retns:
	pop	ds		;2.1a
	iret
;
@kbdentry	endp
;
	.tsk_ecode
	end

