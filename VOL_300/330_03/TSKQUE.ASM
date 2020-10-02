;
;	--- Version 2.2 90-10-12 10:46 ---
;
;	CTask - Queue management
;
;	Public Domain Software written by
;		Thomas Wagner
;		Ferrari electronic Gmbh
;		Beusselstrasse 27
;		D-1000 Berlin 21
;		Germany
;
;	Since managing the queues is one of the most timing critical 
;	parts of CTask, the main routines have been coded in Assembler.
;	The corresponding C code is included as a reference.
;
;	This file is new with 2.0.
;
	name	tskque
;
	include	tsk.mac
	include	tskdeb.h
;
	.tsk_model
;
	Pubfunc	tsk_enqueue
	Pubfunc	tsk_dequeue
	Pubfunc	tsk_putqueue
	Pubfunc	tsk_enqtimer
	Pubfunc	tsk_deqtimer
;
	extrn	tsk_scheduler: far
;
	IF	CHECKING
	CGlbext	tsk_fatal_pmd
	ENDIF
;
	global_ext
;
	.tsk_data
	.tsk_edata
	.tsk_code
;
	IF	CHECKING
tsk_dgroup	dw	@CTASK_DATA
	ENDIF
;
;
; tsk_enqueue
;
;	This routine adds a task (or any other queue element) to a queue
;	in priority order. The search starts at the queue's tail end so
;	as to better support the 'yield' operation (which disturbs the
;	priority order in the queue).
;
;void near tsk_enqueue (queheadptr q, queptr elem)
;{
;   queptr curr;
;
;   while (1)
;      {
;      curr = q->prev;
;      while (!(curr->kind & Q_HEAD) &&
;             curr->el.pri.prior < elem->el.pri.prior)
;         curr = curr->prev;
;      }
;   elem->prev = curr;
;   elem->next = curr->next;
;   curr->next = elem->next->prev = elem;
;}
;
Localfunc tsk_enqueue,<uses ds di, que: far ptr, elem: far ptr>
;
	CHECK_TCBPTR	elem,"tsk_enqueue: elem"
	CHECK_QHEAD	que,"tsk_enqueue: queue"
	les	di,elem
	lds	bx,que
	mov	cx,es:q_el.q_prior[di]	; load priority into BX
	lds	bx,q_prev[bx]		; last queue element
;
enq_loop:
	test	q_kind[bx],Q_HEAD	; at head?
	jnz	enq_found		; then insert
	cmp	q_el.q_prior[bx],cx	; else check priority
	jae	enq_found		; if above or equal, insert
;
	lds	bx,q_prev[bx]		; backup one element
	jmp	enq_loop		; and try again
;
enq_found:
	mov	word ptr es:q_prev[di],bx	; elem->prev = curr
	mov	word ptr es:q_prev+2[di],ds
	mov	ax,word ptr q_next[bx]		; elem->next = curr->next;
	mov	word ptr es:q_next[di],ax
	mov	dx,word ptr q_next+2[bx]
	mov	word ptr es:q_next+2[di],dx
	mov	word ptr q_next[bx],di		; curr->next = elem;
	mov	word ptr q_next+2[bx],es
	mov	bx,ax
	mov	ds,dx
	mov	word ptr q_prev[bx],di		; elem->next->prev = elem
	mov	word ptr q_prev+2[bx],es
	ret
;
tsk_enqueue	endp
;
;
; tsk_putqueue
;
;	This routine adds a queue element to the end of a queue.
;
;void near tsk_putqueue (queheadptr q, queptr elem)
;{
;   elem->next = q;
;   elem->prev = q->prev;
;   q->prev = elem->prev->next = elem;
;}
;
Localfunc tsk_putqueue,<uses ds di, que: far ptr, elem: far ptr>
;
	CHECK_PTR	elem,"tsk_putqueue: elem"
	CHECK_QHEAD	que,"tsk_putqueue: queue"
	les	di,elem
	lds	bx,que
	mov	word ptr es:q_next[di],bx	; elem->next = que
	mov	word ptr es:q_next+2[di],ds
	mov	ax,word ptr q_prev[bx]		; elem->prev = que->prev;
	mov	word ptr es:q_prev[di],ax
	mov	dx,word ptr q_prev+2[bx]
	mov	word ptr es:q_prev+2[di],dx
	mov	word ptr q_prev[bx],di		; que->prev = elem;
	mov	word ptr q_prev+2[bx],es
	mov	bx,ax
	mov	ds,dx
	mov	word ptr q_next[bx],di		; elem->prev->next = elem
	mov	word ptr q_next+2[bx],es
	ret
;
tsk_putqueue	endp
;
;
;
; tsk_enqtimer
;
;	This routine adds a task (or a timer element) to
;	the timeout queue.
;
;	This is slightly different from the normal enqueue in that
;	queue elements are not inserted based on priority, but rather
;	based on the tick count. Each element's tick counter stores
;	the difference in ticks to the previous element. This speeds
;	up the timeout loop, since only the first element has to be
;	counted down, but it makes insertion a tad more complicated.
;
;void near tsk_enqtimer (queptr elem, dword ticks)
;{
;   queptr curr, q;
;
;   if (!ticks)
;      return;
;   q = &GLOBDATA timer_queue;
;
;   curr = q->next;
;   while (!(curr->kind & Q_HEAD) &&
;          curr->el.ticks <= ticks)
;      {
;      ticks -= curr->el.ticks;
;      curr = curr->next;
;      if (!ticks)
;         break;
;      }
;   if (curr->kind & Q_HEAD)
;      curr->el.ticks -= ticks;
;   elem->next = curr;
;   elem->prev = curr->prev;
;   curr->prev = elem->prev->next = elem;
;   elem->el.ticks = ticks;
;}
;
Localfunc tsk_enqtimer,<uses ds di, elem: far ptr, ticks: dword>
;
	IFDEF	LOAD_DS
	mov	ax,@CTASK_DATA
	mov	ds,ax
	ENDIF
;
	CHECK_PTR	elem,"tsk_enqtimer: elem"
	les	di,elem
	IF	SINGLE_DATA
	lea	bx,tsk_glob_rec.timer_queue
	ELSE
	lds	bx,tsk_global
	add	bx,timer_queue
	ENDIF
	CHECK_QHEAD_R	ds,bx,"tsk_enqtimer: timer queue"
;
	mov	ax,word ptr (ticks)	; load tick count into DX:AX
	mov	dx,word ptr (ticks+2)
	mov	cx,ax
	or	cx,dx
	jz	enqt_ret
	lds	bx,q_first[bx]		; first queue element
;
et_loop:
	test	q_kind[bx],Q_HEAD	; at head?
	jnz	et_found1		; then insert
	sub	ax,word ptr q_el.q_ticks[bx]	; else check ticks
	sbb	dx,word ptr q_el.q_ticks+2[bx]
	jc	et_found		; insert on overflow
;
	lds	bx,q_next[bx]		; next element
	jmp	et_loop			; and try again
;
et_found:
	add	ax,word ptr q_el.q_ticks[bx]	; restore ticks
	adc	dx,word ptr q_el.q_ticks+2[bx]
;
et_found1:
	mov	word ptr es:q_el.q_ticks[di],ax	; elem->el.ticks = ticks
	mov	word ptr es:q_el.q_ticks+2[di],dx
;
	test	q_kind[bx],Q_HEAD	; at head?
	jnz	et_notick		; no tick mod if yes
;
	sub	word ptr q_el.q_ticks[bx],ax	; else curr->el.ticks -= ticks
	sbb	word ptr q_el.q_ticks+2[bx],dx
;
et_notick:
	mov	word ptr es:q_next[di],bx	; elem->next = curr
	mov	word ptr es:q_next+2[di],ds
	mov	ax,word ptr q_prev[bx]		; elem->prev = curr->prev;
	mov	word ptr es:q_prev[di],ax
	mov	dx,word ptr q_prev+2[bx]
	mov	word ptr es:q_prev+2[di],dx
	mov	word ptr q_prev[bx],di		; curr->prev = elem;
	mov	word ptr q_prev+2[bx],es
	mov	bx,ax
	mov	ds,dx
	mov	word ptr q_next[bx],di		; elem->prev->next = elem
	mov	word ptr q_next+2[bx],es
;
enqt_ret:
	ret
;
tsk_enqtimer	endp
;
;
;  tsk_dequeue
;
;	This routine removes an element from a queue.
;
;void near tsk_dequeue (queptr elem)
;{
;   if (elem->next == NULL)
;      return;
;   elem->next->prev = elem->prev;
;   elem->prev->next = elem->next;
;   elem->next = NULL;
;}
;
Localfunc tsk_dequeue,<uses ds di, elem: far ptr>
;
	CHECK_PTR	elem,"tsk_dequeue: elem"
	lds	bx,elem
	les	di,q_next[bx]		; remove from queue
	mov	ax,es			; check if enqueued
	or	ax,di
	jz	deq_ret			; nothing to do if not in queue
	xor	ax,ax			; clear next pointer
	mov	word ptr q_next[bx],ax
	mov	word ptr q_next+2[bx],ax
	lds	bx,q_prev[bx]
	mov	word ptr es:q_prev[di],bx
	mov	word ptr es:q_prev+2[di],ds
	mov	word ptr q_next[bx],di
	mov	word ptr q_next+2[bx],es
;
deq_ret:
	ret
;
tsk_dequeue	endp
;
;
;  tsk_deqtimer
;
;	This routine removes an element from the timer queue.
;	It is different from the normal dequeue in that the tick
;	difference must be updated for the next in line.
;
;	Since this routine is also called for the timer entry in
;	task blocks when the task is made runable, we have to check
;	that the timer entry actually is enqueued in the timer queue.
;	A task could also be enqueued in the watch or hotkey queue,
;	and timer tick updating must be skipped in that case. This
;	check has been added in 2.2.
;
;void near tsk_deqtimer (queptr elem)
;{
;   if (elem->next == NULL)
;      return;
;   if (elem->kind == TYP_TIMER)
;      if (!(elem->next->kind & Q_HEAD))
;         elem->next->el.ticks += elem->el.ticks;
;   elem->next->prev = elem->prev;
;   elem->prev->next = elem->next;
;   elem->next = NULL;
;}
;
Localfunc tsk_deqtimer,<uses ds di, elem: far ptr>
;
	CHECK_PTR	elem,"tsk_deqtimer: elem"
	lds	bx,elem
	les	di,q_next[bx]
	mov	ax,es
	or	ax,di
	jz	deqtim_ret		; nothing to do if not in queue
	cmp	q_kind[bx],TYP_TIMER	; is it a timer element?
	jne	dqt_notick		; don't update ticks if watch/hotkey
	test	es:q_kind[di],Q_HEAD
	jnz	dqt_notick
	mov	ax,word ptr q_el.q_ticks[bx]	; first update next tick count
	mov	dx,word ptr q_el.q_ticks+2[bx]
	add	word ptr es:q_el.q_ticks[di],ax
	adc	word ptr es:q_el.q_ticks+2[di],dx
;
dqt_notick:
	xor	ax,ax
	mov	word ptr q_next[bx],ax
	mov	word ptr q_next+2[bx],ax
	lds	bx,q_prev[bx]
	mov	word ptr es:q_prev[di],bx
	mov	word ptr es:q_prev+2[di],ds
	mov	word ptr q_next[bx],di
	mov	word ptr q_next+2[bx],es
;
deqtim_ret:
	ret
;
tsk_deqtimer	endp
;
	.tsk_ecode
	end


