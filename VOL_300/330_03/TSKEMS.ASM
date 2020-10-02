;
;	--- Version 2.2 90-10-12 10:38 ---
;
;	CTask - EMS support
;
;	Public Domain Software written by
;		Thomas Wagner
;		Ferrari electronic Gmbh
;		Beusselstrasse 27
;		D-1000 Berlin 21
;		Germany
;
;	This module contains the EMS interface. CTask version 2.1
;	will save and restore the EMS page map (the LIM 3.2 compatible
;	64k standard frame only) on a task switch if EMS support is 
;	installed.
;
;	If the EMS driver supports it, the LIM 4.0 save/restore
;	partial page map function is used. If this function is not
;	available, the LIM 3.2 full page map save/restore call is used,
;	provided that the space in the TCB (EMS_SAVE_SIZE) can hold
;	the page map information.
;
	name	tskems
;
	include	tsk.mac
;
	IF	DOS AND EMS
;
	.tsk_model
;
	Pubfunc	tsk_install_ems
;
	extrn	tsk_glob_rec: byte
;
;
	.tsk_data
;
emm_name	db	"EMMXXXX0"
;
ems_map		dw	4
		dw	4 dup(?)
;
getfunc		dw	?
setfunc		dw	?
;
	.tsk_edata
	.tsk_code
;
;	tsk_install_ems	installs EMS support if an EMS driver is
;	present, and saving the page map is possible.
;
;	Returns 1 is EMS installed, 0 if no driver, -1 if saving
;	is impossible.
;
Localfunc tsk_install_ems,<uses si di>
;
	IFDEF	LOAD_DS
	push	ds
	mov	ax,@CTASK_DATA
	mov	ds,ax
	ENDIF
	xor	ax,ax
	mov	es,ax
	mov	es,word ptr es:[67h*4+2]
	mov	di,0ah
	mov	si,offset emm_name
	mov	cx,8
	repe cmpsb
	je	ems_there
	jmp	no_ems
;
;	EMS is installed, but the partial map functions failed.
;	Try the full page map save/restore, which is present in
;	LIM 3.2.
;
try_32:
	mov	ax,4e03h	; get size of full page map save array
	int	67h
	or	ah,ah
	jnz	bad_ems		; we can't save if this fails
	cmp	al,EMS_SAVE_SIZE
	ja	bad_ems		; we can't save if array too large
	mov	getfunc,4e00h	; get full map
	mov	setfunc,4e01h	; set full map
	jmp	enter_ok
;
bad_ems:
	mov	ax,-1
	jmp	inst_ems_end
;
;	EMS is installed, check if we can save a partial page
;	map in the reserved space, and if the save partial page map
;	call is implemented at all.
;
ems_there:
	mov	ax,4f02h	; get size of partial page map save array
	mov	bx,4		; for the four standard pages
	int	67h
	or	ah,ah
	jnz	try_32		; if this call fails, we might have LIM 3.2
	cmp	al,EMS_SAVE_SIZE
	ja	bad_ems		; we can't save if array too large
;
;	The partial page map call is available, and the space allocated
;	in the TCB is sufficient. Now we have to build the page table
;	for the get partial map function.
;
;	Get physical pages
;
	mov	ax,5801h	; get number of entries
	int	67h
	or	ah,ah
	jnz	try_32		; if this call fails, we might have LIM 3.2
	mov	al,4
	mul	cl		; space required
	sub	sp,ax		; make room on stack
	mov	di,sp
	push	ax		; save size
	mov	ax,ss
	mov	es,ax
	mov	ax,5800h	; get physical page array
	int	67h
	or	ah,ah
	jz	phys_ok		; if this call fails, we might have LIM 3.2
	pop	ax
	add	sp,ax
	jmp	try_32
;
;	Find the physical page address for the first four pages
;	(the LIM 3.2 64k frame)
;
phys_ok:
	mov	ax,-1
;
first_loop:
	cmp	word ptr es:[di+2],ax	; compare logical page number
	ja	first_next		; skip if above what we already found
	mov	ax,word ptr es:[di+2]	; the new minimum page
	mov	bx,di			; save the index
	mov	dx,cx			; and the remaining entries
	or	ax,ax
	jz	first_found
;
first_next:
	add	di,4
	loop	first_loop
;
;	We found the first logical page. Now copy the physical
;	page numbers for the first four pages to the array.
;
first_found:
	mov	di,offset ems_map
	mov	word ptr [di],4		; four entries
	add	di,2
	mov	cx,4
;
enter_map:
	mov	ax,es:[bx]		; physical page
	mov	[di],ax			; store in map array
	add	bx,4
	add	di,2
	dec	cx
	jz	enter_rdy
	dec	dx
	jnz	enter_map
	mov	bx,sp			; recycle to start of list
	add	bx,2			; SP+2 since we pushed AX
	jmp	enter_map
;
enter_rdy:
	pop	ax
	add	sp,ax
	mov	getfunc,4f00h		; get partial map
	mov	setfunc,4f01h		; set partial map
;
;	Enter the functions into the global variable block
;
enter_ok:
	mov	word ptr tsk_glob_rec.ems_save,offset @ems_savefn
	mov	word ptr tsk_glob_rec.ems_save+2,cs
	mov	word ptr tsk_glob_rec.ems_rest,offset @ems_restfn
	mov	word ptr tsk_glob_rec.ems_rest+2,cs
	mov	word ptr tsk_glob_rec.ems_savetsk,offset @save_ems_tsk
	mov	word ptr tsk_glob_rec.ems_savetsk+2,cs
;
	mov	ax,1
	jmp	short inst_ems_end
;
no_ems:
	xor	ax,ax
;
inst_ems_end:
	IFDEF	LOAD_DS
	pop	ds
	ENDIF
	ret
;
tsk_install_ems	endp
;
;
;	The save and restore routines are called by the scheduler,
;	and are entered with
;		DS    = CTask data segment
;		ES:DI = TCB
;
;	ems_savefn - Save EMS context function
;
@ems_savefn	proc	far
;
	push	si
	push	di
	lea	di,t_ems_map[di]	; destination
	mov	si,offset ems_map	; not used for full map
	mov	ax,getfunc		; get (partial) page map
	int	67h
	pop	di
	pop	si
	ret
;
@ems_savefn	endp
;
;	ems_restfn - Restore EMS context function
;
@ems_restfn	proc	far
;
	mov	ax,setfunc		; set (partial) page map
	push	ds
	push	si
	mov	si,es
	mov	ds,si
	lea	si,t_ems_map[di]	; source
	int	67h
	pop	si
	pop	ds
	ret
;
@ems_restfn	endp
;
;	save_ems_tsk	Save current EMS context in the specified task's TCB
;
@save_ems_tsk	proc	far uses di, task: far ptr
;
	IFDEF	LOAD_DS
	push	ds
	mov	ax,@CTASK_DATA
	mov	ds,ax
	ENDIF
	les	di,task
	call	@ems_savefn
	IFDEF	LOAD_DS
	pop	ds
	ENDIF
	ret
;
@save_ems_tsk	endp
;
	.tsk_ecode
;
	ENDIF
;
	end

