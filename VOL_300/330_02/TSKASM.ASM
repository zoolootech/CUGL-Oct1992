;
;	--- Version 2.2 90-10-12 10:37 ---
;
;	CTask - Scheduler and miscellaneous utilities
;
;	Public Domain Software written by
;		Thomas Wagner
;		Ferrari electronic Gmbh
;		Beusselstrasse 27
;		D-1000 Berlin 21
;		Germany
;
;
; NOTE: Version 2.1 enables interrupts shortly after setting the in_sched
;	flag. This greatly reduces interrupt latency, but it has its
;	pitfalls. The main culprit is modification of the current TCB
;	while the scheduler is busy processing it. However, the situations
;	where a "foreign" TCB is modified are rare, so it is possible to
;	handle those cases in the appropriate places. Note that it was
;	never a good idea to make a task waiting or, worse yet, kill the
;	current task, while the scheduler was active. This was not detected
;	in previous releases, but would still cause crashes, even though
;	the interrupt was enabled only in the eligible waiting loop.
;
;	The only legitimate action is to make a task eligible while it
;	is being scheduled out. Since this only modifies the queue head
;	and task status, no adverse effects are possible. But making a task
;	waiting that is already scheduled out (as it was possible in
;	all versions up to 2.1) simply won't work. The wait action requires
;	that the scheduler switch the context away from the running task,
;	which it can't do when it's already active and there is no real
;	"running task". So the scheduler would immediately return, and 
;	the wait would never occur, most likely causing some very 
;	strange effects. Version 2.1 now explicitly checks for this error,
;	and will terminate the tasker if this should ever happen.
;
;	Other safeguards include not calling the INT 8 timer interrupt 
;	chain for early INT8 processing while the scheduler is active.
;	If INT8_EARLY is used, the tick chain is called from within
;	the interrupt handler. Since the interrupt could have hit 
;	while the scheduler was active, and the original INT 8
;	may have TSRs attached that call DOS, dangerous conflicts are
;	possible. For the same reason, the scheduler will set the In-DOS
;	flag while it is active, to keep TSR's activated by other
;	interrupts (esp. the keyboard) from calling DOS.
;	Using the same delayed processing as is used in INT 8 in 
;	the INT 9 handler was tried, but led to lost keystrokes depending
;	on system load.
;
;
;	Interrupts also are enabled in most other places in the scheduler,
;	except when traversing the queues.
;
;	Having interrupts enabled during the save/restore code also
;	eliminates a deadlock problem with floating point exceptions
;	on the 8087, which simplifies saving and restoring the '87 registers.
;
;
;	Version 2.2 eliminates a race condition problem introduced by
;	the above changes. If a task was made waiting, only the queue head
;	reflected this. Only after the scheduler had entered the task into
;	the appropriate queue would it have been safe to enable interrupts,
;	since an interrupt handler calling (indirectly) tsk_runable would
;	never see the task as waiting if it hit before this time.
;	To keep interrupts enabled, the logic was changed in that the
;	wait action now inserts the task into the appropriate queue
;	directly, without waiting for the scheduler to do it. Naturally,
;	this means a change here so that the scheduler doesn't touch
;	the queue if the task is already enqueued. Only if the task is not
;	in any queue is the queue head used to determine the place to
;	enqueue, which now can occur only when making a running task
;	eligible.
;
;**************************  CAUTION:  ***********************************
;
;	If the DOS flag is set, the scheduler uses an undocumented feature
;	of DOS versions >= 3.10 to save and restore certain variables
;	local to the DOS kernel. This allows for fast, secure task switching
;	with tasks owning different PSP's. To save and restore the complete
;	state of DOS on every task switch would not be feasible with any
;	other documented method.
;
;	NOTE that this method relies on certain inner workings of DOS
;	that may not be compatible with future releases or "compatible"
;	operating systems. It has been tested with all DOS versions
;	from 3.10 through 3.20, 3.30, up to PC/MS-DOS 4.01. It
;	also has been reported to work with DR-DOS 3.41.
;
;*************************************************************************
;
	name	tskasm
;
	include	tsk.mac
	include	tskdeb.h
;
	.tsk_model
;
	public	tsk_scheduler
	public	sched_int
;
	Pubfunc	schedule
	Pubfunc	yield
	Pubfunc	c_schedule
;
	Pubfunc	tsk_callfunc
	Pubfunc	tsk_dis_int
	Pubfunc	tsk_ena_int
	Pubfunc	tsk_cli
	Pubfunc	tsk_sti
	Pubfunc	tsk_dseg
	Pubfunc	tsk_flags
	Pubfunc	tsk_outp
	Pubfunc	tsk_inp
	Pubfunc	tsk_inpw
	Pubfunc	tsk_nop
	Pubfunc	tsk_memcpy
;
	IF	CLOCK_MSEC
	Pubfunc	tsk_timeout
	ENDIF
;
	IF	CHECKING
	CGlbext	tsk_fatal_pmd
	ENDIF
;
	IF	DEBUG AND DEB_TSKDIS
	extrn	tsk_debtask: word
	ENDIF
	IF	DEBUG AND DEB_FLASHERS
	Pubfunc	tsk_inccdis
	extrn	tsk_debflash: word
	ENDIF
	IF	DEBUG
	extrn	tsk_dtposn: dword
	ENDIF
;
	global_ext
;
;
LSTACK_SIZE	=	512	; local stack size in words
;
INT_FLAG	=	2h	; Int enable flag in upper byte of flag reg
;
sr_flags	=	8	; Offset of flag register on task stack
sr_ds		=	2	; Offset of DS on task stack
;
psp_savoff	=	2eh	; Offset of PSP stack pointer save dword
;
	IF	FAR_STACKS
ctask_stacks	segment word public 'CTASK_STACKS'
	ELSE
	.tsk_data
	ENDIF
;
	dw	LSTACK_SIZE dup(?)
slocal_stack	label	word
;
	IF	FAR_STACKS
ctask_stacks	ends
	.tsk_data
	ENDIF
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
;------------------------------------------------------------------------
;
;	enq	Enqueue tcb in queue. For local use only.
;		entry:	es:di = tcb to enqueue
;		exit:	-
;		uses:	ax, cx, si
;	NOTE:	Interrupts must be disabled on entry.
;
enq	macro
;
	push	ds
	lds	cx,es:cqueue.q_next[di]	; this is NULL if not in queue
	mov	ax,ds
	or	ax,cx
	jnz	enq_end			; don't touch if enqueued
	lds	si,es:qhead[di]		; queue head pointer
	mov	ax,ds
	or	ax,si	
	jz	enq_end			; nothing left to do if queue null
;
	mov	cx,es:cqueue.q_el.q_prior[di]
	mov	ax,es:cqueue.q_el.q_ini_prior[di]
	mov	es:cqueue.q_el.q_prior[di],ax
	lds	si,q_last[si]		; last queue element
;
enq_loop:
	test	q_kind[si],Q_HEAD	; at head?
	jnz	enq_found		; then insert
	cmp	q_el.q_prior[si],cx	; else check priority
	jae	enq_found		; if above or equal, insert
;
	lds	si,q_prev[si]		; backup one element
	jmp	enq_loop		; and try again
;
enq_found:
	mov	word ptr es:q_prev[di],si	; elem->prev = curr
	mov	word ptr es:q_prev+2[di],ds
	mov	ax,word ptr q_next[si]		; elem->next = curr->next;
	mov	word ptr es:q_next[di],ax
	mov	cx,word ptr q_next+2[si]
	mov	word ptr es:q_next+2[di],cx
	mov	word ptr q_next[si],di		; curr->next = elem;
	mov	word ptr q_next+2[si],es
	mov	si,ax
	mov	ds,cx
	mov	word ptr q_prev[si],di		; elem->next->prev = elem
	mov	word ptr q_prev+2[si],es
;
enq_end:
	pop	ds
;
	endm
;
;	upd_prior: Update priority of tasks in eligible queue.
;	           Only activated if tsk_var_prior is nonzero.
;
;	entry:	ds:bx = global variable block
;	exit:	-
;	uses:	ax,di,es
;
;	NOTE:	Contrary to what earlier versions said, this loop
;		must be protected by interrupt disable.
;		Since it steps through a pointer chain, and this
;		pointer chain might be modified by external events
;		(a task becoming eligible to run), there is indeed
;		a danger of race conditions.
;
upd_prior	macro
;
	les	di,eligible_queue.q_first[bx]
;
pinc_loop:
	test	es:q_kind[di],Q_HEAD	; queue head?
	jnz	updp_end
	inc	es:q_el.q_prior[di]
	jnz	pinc_nxt
	dec	es:q_el.q_prior[di]
pinc_nxt:
	les	di,es:q_next[di]
	jmp	pinc_loop
;
updp_end:
;
	endm
;
;-------------------------------------------------------------------------
;
	IF	DEBUG AND DEB_TSKDIS
;
fill8	macro
	mov	cx,10
	mov	ax,0720h
	rep stosw
	endm
;
@strcpy8	proc	near
	mov	cx,8
	mov	ah,7
strcpyl:
	lodsb
	or	al,al
	jz	strcpyfill
	stosw
	loop	strcpyl
;
strcpyfill:
	add	cx,2
	mov	al,' '
	rep stosw
	ret
;
@strcpy8	endp
;
	ENDIF
;
	IF	DEBUG AND DEB_FLASHERS
;
;	tsk_inccdis - increment counter on display
;
;	Entry:	AX is counter first digit offset
;		DS is CTask dgroup
;
;	Uses:	All registers preserved
;
Localfunc	tsk_inccdis
;
	push	ds
	push	si
	push	cx
	lds	si,tsk_dtposn
	add	si,ax
	mov	cx,DEBFLASH_NDIGS
	add	si,(DEBFLASH_NDIGS - 1) * 2
incdis_loop:
	inc	byte ptr [si]
	cmp	byte ptr [si],'9'
	jbe	incdis_end
	mov	byte ptr [si],'0'
	sub	si,2
	loop	incdis_loop
incdis_end:
	pop	cx
	pop	si
	pop	ds
	ret
;
tsk_inccdis	endp
;
	ENDIF
;
;-------------------------------------------------------------------------
;
;	The scheduler. Note that this routine is entered with the stack
;	set up as for an interrupt handler.
;
tsk_scheduler	proc	far
;
	cli			; better safe than sorry
;
;	First, check if we're already in the scheduler. This might
;	happen if an interrupt handler schedules, and it would have
;	catastrophic results if the scheduler would be re-entered.
;	Previous versions used a code-segment based variable to store
;	this flag. Starting at version 2.0, the flag is located in the 
;	global variable block, to support ROM-based implementations, 
;	and to avoid multiple entries into the scheduler when code
;	sharing is not used in secondary invocations.
;
;	Version 2.1 sets the 'pretick' flag when the scheduler is busy.
;	This allows an immediate re-schedule if the schedule request
;	hit after loading the new task.
;
	push	ds
	push	bx
	mov	ds,cs:tsk_dgroup
	IF	SINGLE_DATA
	cmp	tsk_glob_rec.in_sched,0	; already in the scheduler?
	je	sched_ok	; continue if not
	mov	tsk_glob_rec.pretick,1	; mark schedule pending
	ELSE
	lds	bx,tsk_global
	cmp	in_sched[bx],0	; already in the scheduler?
	je	sched_ok	; continue if not
	mov	pretick[bx],1	; mark schedule pending
	ENDIF
	pop	bx
	pop	ds		; else return immediately
	iret
;
;	Not the second time into the scheduler, set the in_sched flag,
;	and load the current task's TCB.
;	If we're running under DOS, additionally set the In-DOS flag
;	for the reasons outlined above.
;
sched_ok:
	IF	SINGLE_DATA
	inc	tsk_glob_rec.in_sched
	IF	DOS
	push	ds
	lds	bx,tsk_glob_rec.dos_in_use
	inc	byte ptr ds:[bx]
	pop	ds
	ENDIF
	lds	bx,tsk_glob_rec.current_task
	ELSE
	inc	in_sched[bx]
	IF	DOS
	push	ds
	push	bx
	lds	bx,dos_in_use[bx]
	inc	byte ptr [bx]
	pop	bx
	pop	ds
	ENDIF
	lds	bx,current_task[bx]
	ENDIF
	push	ax
	mov	ax,ds
	or	ax,bx
	pop	ax
;
;	Registers are stored in the TCB to keep stack usage minimal.
;	If there is no current task (i.e. it has been killed),
;	we can't store the registers.
;
	IF	CHECKING
	jnz	chk_rsave
	jmp	no_rsave
chk_rsave:
	CHECK_TCBPTR_R	ds,bx,"scheduler: current task"
	ELSE
	jz	no_rsave
	ENDIF
	cmp	state[bx],ST_RUNNING
	jne	store_regs
	mov	state[bx],ST_ELIGIBLE
;
store_regs:
	sti
	mov	t_sp[bx],sp
	mov	t_ss[bx],ss
	mov	t_ax[bx],ax
	mov	t_cx[bx],cx
	mov	t_dx[bx],dx
	mov	t_bp[bx],bp
	mov	t_si[bx],si
	mov	t_di[bx],di
	mov	t_es[bx],es
	mov	bp,sp
	or	byte ptr sr_flags+1[bp],INT_FLAG ; enable interrupts on return
	mov	ax,sr_ds[bp]
	mov	t_ds[bx],ax
;
;	This is the entry when restarting the scheduler.
;	Note that registers don't have to be saved again.
;
sched_restart:
no_rsave:
	cli
	mov	ss,cs:stackseg		; switch to local stack
	mov	sp,offset slocal_stack
	sti
	cld
	mov	ds,cs:tsk_dgroup	; establish addressing of our vars
	IF	DEBUG AND DEB_FLASHERS
	cmp	tsk_debflash,0
	je	debdd0
	mov	ax,DEBP_CNTSCHED
	call	tsk_inccdis
debdd0:
	ENDIF
	IF	DEBUG AND DEB_TSKDIS
	cmp	tsk_debtask,0
	je	debdd1
	les	di,tsk_dtposn
	mov	byte ptr es:[di],'*'
debdd1:
	ENDIF
	IF	SINGLE_DATA
	mov	bx,offset tsk_glob_rec
	ELSE
	lds	bx,tsk_global
	ENDIF
;
	cmp	var_prior[bx],0
	je	no_var_pri
;
	cli
	upd_prior			; update priorities in eligible queue
	sti
;
no_var_pri:
;
	and	preempt[bx],1		; Turn off temp preempt flag
;
	les	di,current_task[bx]	; get current tcb again
	mov	ax,es			; check if NULL (current task killed)
	or	ax,di
	IF	NOT CHECKING
	jz	no_current
	ELSE
	jnz	do_chkstk
	jmp	no_current
;
do_chkstk:
	push	ds
	lds	si,es:stkbot[di]
	mov	cx,8
	xor	ah,ah
chkstk:
	lodsb
	cmp	ah,al
	jne	stkoflo
	inc	ah
	loop	chkstk
	jmp	short chkstend
;
stkmsg	db	"Stack Overflow",0
;
stkoflo:
	callp	tsk_fatal_pmd,<<cs,#stkmsg>>
;
chkstend:
	pop	ds
	ENDIF
;
;	If there is a scheduler-entry routine in the current task's
;	TCB, call it. (This is used by the DOS handler module)
;	NOTE: Interrupts are still enabled.
;
	IF	DOS
	mov	ax,word ptr es:sched_ent_func[di]
	or	ax,word ptr es:sched_ent_func[di]+2
	jz	no_schentcall
;
	push	es
	push	di
	push	ds
	push	bx
	call	es:sched_ent_func[di]
	pop	bx
	pop	ds
	pop	di
	pop	es
;
no_schentcall:
	ENDIF
;
	cli
	enq				; Enqueue current task
	sti
;
no_current:
	lea	si,eligible_queue[bx]
;
;	Now we enter an enabled loop if there is no task in the
;	eligible queue.
;
wait_elig:
	IF	DEBUG AND DEB_TSKDIS
	push	ds
	mov	ds,cs:tsk_dgroup
	cmp	tsk_debtask,0
	je	debdd2
	les	di,tsk_dtposn
	add	di,DEBP_ELIGTSK
	pop	ds
	push	ds
	push	si
	lds	si,q_first[si]
	test	q_kind[si],Q_HEAD
	jnz	deb101
	push	si
	lea	si,tname.nname[si]
	call	@strcpy8
	pop	si
	lds	si,q_next[si]
	test	q_kind[si],Q_HEAD
	jnz	deb102
	lea	si,tname.nname[si]
	call	@strcpy8
	jmp	short deb105
;
deb101:
	fill8
deb102:
	fill8
deb105:
	pop	si
debdd2:
	pop	ds
	ENDIF
	jmp	$+2			; allow ints
	cli
	les	di,q_first[si]
	test	es:q_kind[di],Q_HEAD
	jz	not_empty		; jump if not
;
	sti			       	; enable interrupts
	IF	DEBUG AND DEB_FLASHERS
	cmp	tsk_debflash,0
	je	debdidle
	mov	ax,DEBP_CNTIDLE
	call	tsk_inccdis
debdidle:
	ENDIF
	jmp	$+2
	jmp	wait_elig
;
;	Eligible queue not empty, activate first eligible task.
;	First, take it off the queue.
;	The preemption tick flag is cleared here. If it should be
;	set by an interrupt handler trying to schedule after this point,
;	we immediately re-schedule when hitting the end.
;
not_empty:
	CHECK_TCBPTR_R	es,di,"scheduler: eligible task"
	mov	pretick[bx],0		; No preemption tick
	push	di
	push	es
	mov	bp,sp			; address new pointer thru BP
;
	mov	ax,word ptr es:cqueue.q_next[di] ; next in eligible queue
	mov	cx,word ptr es:cqueue.q_next+2[di]
	mov	word ptr es:cqueue.q_next[di],0	; mark not in queue
	mov	word ptr es:cqueue.q_next+2[di],0
	mov	di,ax
	mov	es,cx
	mov	word ptr q_first[si],di
	mov	word ptr q_first+2[si],es	; eligible_queue.first = next
	mov	word ptr es:q_prev[di],si
	mov	word ptr es:q_prev+2[di],ds	; next->prev = eligible_queue
;
;	Again, interrupts can be enabled here.
;
	sti
;
;	Now check if the new task is the same as the old one.
;	If that's the case, we skip the save/restore function calls,
;	and the DOS variable copy.
;
	les	di,current_task[bx]	; the previously running task
	mov	ax,es
	cmp	ax,[bp]			; same as the new one ?
	jne	chk_sav			; jump if not same segment
	cmp	di,2[bp]
	jne	chk_sav			; jump if not same offset
	jmp	sched_complete		; don't save/restore if same task
;
chk_sav:
	or	ax,di
	jz	set_new_task		; don't save if no previous
;
;	First, call the save function if one is defined in the old TCB.
;
	mov	ax,word ptr es:save_func[di]
	or	ax,word ptr es:save_func+2[di]
	jz	no_savfcall		; jump if no save function
;
	push	ds			; save our registers
	push	bx
	push	es
	push	di
;
	push	es			; push TCB address
	push	es			; push segment again
	pop	ds			; and put into DS
	push	di
;	
	call	es:save_func[di]	; call function
        add     sp,4
;
        pop     di			; restore registers
        pop     es
        pop     bx
        pop     ds
;
;	Save the DOS variables, and the DOS-related interrupt vectors
;	in the old TCB.
;
no_savfcall:
	IF	DOS
	mov	es:t_new[di],0		; task is no longer new
;
	push	ds
	push	di
	mov	ax,ds
	mov	ds,es:base_psp[di]	; get base PSP address
	mov	si,psp_savoff		; offset to save (caller's SS:SP)
	add	di,psp_sssp		; destination
	movsw				; save two words
	movsw
	mov	ds,ax
	mov	cx,l_swap[bx]		; swap area addr & size
	jcxz	no_swap_sav
	lds	si,dos_vars[bx]
	rep movsb
;
no_swap_sav:
	xor	cx,cx			; copy int21-24 interrupt vectors
	mov	ds,cx
	mov	si,21h*4
	mov	cx,8
	rep movsw
;
	pop	di
	pop	ds
	ENDIF
;
;	if EMS support is installed, call the page map save function.
;
	IF	EMS
	mov	ax,word ptr ems_save[bx]
	or	ax,word ptr ems_save+2[bx]
	jz	no_emssav
	call	ems_save[bx]
;
no_emssav:
	ENDIF
;
;	If the NDP is active, save the NDP context.
;
	IF	NDP
	test	es:flags[di],F_USES_NDP
	jz	no_ndpsave
	wait
	fsave	es:ndpsave[di]
;
no_ndpsave:
	ENDIF
;
;	Save complete. The new TCB becomes the current task.
;
set_new_task:
	IF	DEBUG AND DEB_TSKDIS
	push	ds
	mov	ds,cs:tsk_dgroup
	cmp	tsk_debtask,0
	je	deb205
	les	di,tsk_dtposn
	pop	ax			; saved DS
	pop	ds			; the new task's TCB
	pop	si
	push	si
	push	ds
	push	ax			; saved DS
	mov	byte ptr es:[di],'+'
	add	di,DEBP_CURRTSK
	lea	si,tname.nname[si]
	call	@strcpy8
	pop	ds
	push	ds
	lds	si,current_task[bx]	; the previous task
	lea	si,tname.nname[si]
	call	@strcpy8
	pop	ds
	push	ds
	lds	si,eligible_queue.q_first[bx]
	test	q_kind[si],Q_HEAD
	jnz	deb201
	push	si
	lea	si,tname.nname[si]
	call	@strcpy8
	pop	si
	lds	si,q_next[si]
	test	q_kind[si],Q_HEAD
	jnz	deb202
	lea	si,tname.nname[si]
	call	@strcpy8
	jmp	short deb205
;
deb201:
	fill8
deb202:
	fill8
deb205:
	pop	ds
	ENDIF
	pop	es
	pop	di
	cli
	mov	word ptr current_task[bx],di 	; set tcb into current
	mov	word ptr current_task+2[bx],es
	sti
;
;	if EMS support is installed, call the page map restore function.
;
	IF	EMS
	mov	ax,word ptr ems_rest[bx]
	or	ax,word ptr ems_rest+2[bx]
	jz	no_emsrest
	call	ems_rest[bx]
no_emsrest:
	ENDIF
;
;	Now check for a restore function in the new TCB.
;
	mov	ax,word ptr es:rest_func[di]
	or	ax,word ptr es:rest_func+2[di]
	jz	no_resfcall			; jump if no restore function
;
	push	bx				; save our regs
	push	ds
	push	di
	push	es
;
	push	es				; push TCB addr
	push	es				; push segment again
	pop	ds				; and put into DS
	push	di
;
	call	es:rest_func[di]	; call restore function
        add     sp,4
;
        pop     es
        pop     di
        pop     ds
        pop     bx
;
no_resfcall:
;
;	If NDP is enabled, restore 80x87 context.
;
	IF	NDP
	test	es:flags[di],F_USES_NDP
	jz	no_ndprest
	cmp	es:t_new[di],0		; is this TCB a fresh one?
	jne	no_ndprest		; then NDP state isn't valid
;
	wait
	frstor	es:ndpsave[di]
;
no_ndprest:
	ENDIF
;
;	Restore DOS variables and int vectors from new TCB.
;
	IF	DOS
	cmp	es:t_new[di],0		; is this TCB a fresh one?
	jne	sched_complete		; then the DOS info isn't valid.
;
	push	di
	push	es
	push	ds
	mov	dx,ds
	mov	ax,es
	mov	ds,ax
	mov	si,di
	add	si,base_psp
	lodsw
	mov	es,ax
	mov	di,psp_savoff		; offset to restore (caller's SS:SP)
	cli
	movsw				; restore two words
	movsw
	sti
;
	mov	ax,ds
	mov	ds,dx
	mov	cx,l_swap[bx]
	jcxz	no_swap_rest
	les	di,dos_vars[bx]
	mov	ds,ax
	rep movsb
;
no_swap_rest:
	mov	ds,ax
	mov	di,21h*4		; restore int21-24
	xor	cx,cx
	mov	es,cx
	mov	cx,8
	cli
	rep movsw
	sti
;
	pop	ds
	pop	es
	pop	di
	ENDIF
;
;	And that's it. Wrap it up by resetting the priority, 
;	restoring the registers, resetting the in_sched flag, 
;	and returning to the task.
;	Note: to allow keeping interrupts enabled as long as possible,
;	the in_sched flag is cleared after reloading the registers.
;
sched_complete:
	mov	ax,es:q_el.q_ini_prior[di]	; reset current tasks priority
	mov	es:q_el.q_prior[di],ax
	mov	es:state[di],ST_RUNNING		; set task state
;
	push	es
	push	di
	IF	DEBUG AND DEB_TSKDIS
	mov	ds,cs:tsk_dgroup
	cmp	tsk_debtask,0
	je	debddx
	les	di,tsk_dtposn
	mov	byte ptr es:[di],' '
debddx:
	ENDIF
	pop	bx
	pop	ds
	mov	es,t_es[bx]			; restore all registers
	mov	di,t_di[bx]
	mov	si,t_si[bx]
	mov	bp,t_bp[bx]
	mov	dx,t_dx[bx]
	mov	cx,t_cx[bx]
	mov	ax,t_ax[bx]
	mov	ss,t_ss[bx]
	mov	sp,t_sp[bx]
;
;	All done except for resetting the in_sched flag.
;	Now we check the preemption tick flag, and restart the schedule
;	if it is set. To avoid needless reschedules, the priority of the
;	current task is compared to the priority of the first task in
;	the eligible queue. If there is no task eligible, or if its
;	priority is less or equal to the current task, the current task
;	is allowed to continue.
;
	mov	ds,cs:tsk_dgroup
	cli
	IF	SINGLE_DATA
	cmp	tsk_glob_rec.pretick,0
	je	sched_end
	mov	bx,offset tsk_glob_rec
	ELSE
	lds	bx,tsk_global
	cmp	pretick[bx],0
	je	sched_end
	ENDIF
;
	push	es
	push	di
	les	di,current_task[bx]	; get current tcb again
	lds	bx,eligible_queue.q_first[bx]
	test	q_kind[bx],Q_HEAD
	jnz	no_restart
	mov	bx,cqueue.q_el.q_prior[bx]	; prior of eligible task
	cmp	bx,es:cqueue.q_el.q_prior[di]	; prior of current task
	jbe	no_restart
;
;	Note: registers don't have to be popped, since the stack
;	is reset anyway at the sched_restart entry.
;
do_rest:
	jmp	sched_restart
;
;
no_restart:
	pop	di
	pop	es
	mov	ds,cs:tsk_dgroup
	IF	NOT SINGLE_DATA
	lds	bx,tsk_global
	ENDIF
;
sched_end:
	IF	SINGLE_DATA
	mov	tsk_glob_rec.in_sched,0
	IF	DOS
	lds	bx,tsk_glob_rec.dos_in_use
	dec	byte ptr [bx]
	ENDIF
	ELSE
	mov	in_sched[bx],0
	IF	DOS
	lds	bx,dos_in_use[bx]
	dec	byte ptr [bx]
	ENDIF
	ENDIF
;
	pop	bx
	pop	ds
	iret
;
tsk_scheduler	endp
;
;
;--------------------------------------------------------------------------
;
;
;	sched_int  
;
;	Is the scheduler entry for interrupt handlers.
;	It checks if preemption is allowed, returning if not.
;	The stack is assumed to be set up as on interrupt entry.
;	
sched_int	proc	far
;
	push	ds
	push	bx
	mov	ds,cs:tsk_dgroup
	IF	SINGLE_DATA
	mov	bx,offset tsk_glob_rec
	ELSE
	lds	bx,tsk_global
	ENDIF
	cmp	preempt[bx],0		; preempt flags 0?
	jne	no_sched1		; no scheduling if set
	lds	bx,current_task[bx]	; current running task
	test	flags[bx],F_CRIT	; preemption allowed for this task?
	jnz	no_sched		; no scheduling if flag set
	pop	bx			; else go schedule
	pop	ds
	jmp	tsk_scheduler
;
no_sched:
	mov	ds,cs:tsk_dgroup
	IF	SINGLE_DATA
	mov	bx,offset tsk_glob_rec
	ELSE
	lds	bx,tsk_global
	ENDIF
no_sched1:
	mov	pretick[bx],1		; Mark preemption pending
	pop	bx
	pop	ds
	iret
;
sched_int	endp
;
;
;	void far schedule (void)
;
;	Entry for calling the scheduler. Rearranges the stack to
;	contain flags.
;	NOTE: Uses ax,bx.
;
Globalfunc	schedule
;
	IF	NEAR_CODE
	pop	ax
	pushf
	push	cs
	push	ax
	ELSE
	pop	ax
	pop	bx
	pushf
	push	bx
	push	ax
	ENDIF
	cli
	jmp	tsk_scheduler
;
schedule	endp
;
;
;	void far yield (void)
;
;	Entry for calling the scheduler with priority temporarily
;       set to zero. Rearranges the stack to contain flags.
;	NOTE: Uses ax,bx, es.
;
Globalfunc	yield
;
	IF	NEAR_CODE
	pop	ax
	pushf
	push	cs
	push	ax
	ELSE
	pop	ax
	pop	bx
	pushf
	push	bx
	push	ax
	ENDIF
;
	IF	SINGLE_DATA
	IFDEF	LOAD_DS
	mov	es,cs:tsk_dgroup
	les	bx,es:tsk_glob_rec.current_task
	ELSE
	les	bx,tsk_glob_rec.current_task
	ENDIF
	ELSE
	IFDEF	LOAD_DS
	mov	es,cs:tsk_dgroup
	les	bx,es:tsk_global
	ELSE
	les	bx,tsk_global
	ENDIF
        les     bx,es:current_task[bx]
	ENDIF
        cli
        mov     es:q_el.q_prior[bx],0
	jmp	tsk_scheduler
;
yield	endp
;
;
;	void far c_schedule (void)
;
;	Entry for conditionally calling the scheduler. Rearranges 
;	the stack to contain flags, then jumps to _sched_int.
;	NOTE: Uses ax,bx.
;
Globalfunc	c_schedule
;
	IF	NEAR_CODE
	pop	ax
	pushf
	push	cs
	push	ax
	ELSE
	pop	ax
	pop	bx
	pushf
	push	bx
	push	ax
	ENDIF
	cli
	jmp	sched_int
;
c_schedule	endp
;
;--------------------------------------------------------------------------
;
;	void tsk_callfunc (farptr funcad, farptr param)
;
;	Calls the given function, placing the segment address of
;	the parameter into the DS register.
;
Localfunc	tsk_callfunc,<uses ds, funcad: far ptr, param: far ptr>
;
	lds	ax,param
	push	ds
	push	ax
	call	funcad
	add	sp,4
	ret
tsk_callfunc	endp
;
;
;	word tsk_dseg (void)
;
;	Returns current contents of DS register.
;
Globalfunc	tsk_dseg
	mov	ax,ds
	ret
tsk_dseg	endp
;
;
;	word tsk_flags (void)
;
;	Returns current contents of Flag register.
;
Globalfunc	tsk_flags
	pushf
	pop	ax
	ret
tsk_flags	endp
;
;
;	int tsk_dis_int (void)
;
;	Returns current state of the interrupt flag (1 if ints were 
;	enabled), then disables interrupts.
;
Globalfunc	tsk_dis_int
;
	pushf
	pop	ax
	mov	al,ah
	shr	al,1
	and	ax,1
	cli
	ret
;
tsk_dis_int	endp
;
;
;	void far tsk_ena_int (int state)
;
;	Enables interrupts if 'state' is nonzero.
;
Globalfunc	tsk_ena_int,<istate: word>
;
	cmp	istate,0
	je	teiend
	sti
teiend:
	ret
;
tsk_ena_int	endp
;
;
;	tsk_cli/tsk_sti: disable/enable int
;	NOTE: These routines are normally replaced by intrinsics.
;
Globalfunc	tsk_cli
	cli
	ret
tsk_cli	endp
;
;
Globalfunc tsk_sti
	sti
	ret
tsk_sti	endp
;
;
;	tsk_inp/tsk_outp: input/output from/to port
;	NOTE: These routines are normally replaced by intrinsics,
;	      except for Turbo C tsk_inpw.
;
Globalfunc tsk_inp,<port: word>
;
	mov	dx,port
	in	al,dx
	xor	ah,ah
	ret
;
tsk_inp	endp
;
;
Globalfunc tsk_inpw,<port: word>
;
	mov	dx,port
	in	ax,dx
	ret
;
tsk_inpw	endp
;
;
Globalfunc tsk_outp,<port: word, val: word>
;
	mov	dx,port
	mov	al,byte ptr(val)
	out	dx,al
	ret
;
tsk_outp	endp
;
;
;	void tsk_nop (void)
;
;	Do nothing. Used for very short delays.
;
Globalfunc	tsk_nop
;
	jmp	short tnop1
tnop1:
	jmp	short tnop2
tnop2:
	ret
;
tsk_nop	endp
;
;
;	void tsk_memcpy (farptr dest, farptr src, word nbytes)
;
Globalfunc	tsk_memcpy,<uses ds si di, dest: far ptr, src: far ptr, len: word>
;
	mov	cx,len
	lds	si,src
	les	di,dest
	xor	bx,bx
	shr	cx,1
	rcl	bx,1
	jcxz	cpy_byte
	rep movsw
cpy_byte:
	or	bx,bx
	jz	cpy_end
	movsb
cpy_end:
	ret
;
tsk_memcpy	endp
;
;
	IF	CLOCK_MSEC
;
;	dword Localfunc tsk_timeout (dword tout)
;
;	Translates milliseconds to clock ticks using integer arithmetic.
;	Routine provided by Chris Blum.
;
Localfunc	tsk_timeout,<tout:dword>
;
	mov	dh,byte ptr tout
	mov	dl,byte ptr tout+3
	mov	ax,word ptr tout+1
	or	dx,dx
	jnz	tsk_timmax
	or	ax,ax
	jz	tsk_timok
tsk_timmax:
	push	ds
	mov	ds,cs:tsk_dgroup
	IF	SINGLE_DATA
	mov	bx,offset tsk_glob_rec
	ELSE
	lds	bx,tsk_global
	ENDIF
	mov	cx,tick_factor[bx]
	pop	ds
	xor	dh,dh
	cmp	cx,dx
	jbe	tsk_timhi
	div	cx
	mov	bx,ax
	xor	al,al
	mov	ah,byte ptr tout
	div	cx
	inc	cx
	shr	cx,1
	cmp	dx,cx
	mov	dx,bx
	jb	tsk_timmin
	add	ax,1
	adc	dx,0
	jnc	tsk_timok
tsk_timhi:
	mov	ax,0ffffh
	mov	dx,ax
	jmp short tsk_timok
tsk_timmin:
	or	ax,ax
	jnz	tsk_timok
	or	dx,dx
	jnz	tsk_timok
	inc	ax
tsk_timok:
	ret
;
tsk_timeout	endp
;
	ENDIF
;
	.tsk_ecode
	end

