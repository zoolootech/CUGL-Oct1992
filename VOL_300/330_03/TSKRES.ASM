;
;	--- Version 2.2 90-10-12 10:46 ---
;
;	CTask - Residency check.
;
;	Public Domain Software written by
;		Thomas Wagner
;		Ferrari electronic Gmbh
;		Beusselstrasse 27
;		D-1000 Berlin 21
;		Germany
;
;	This file is new with Version 2.1. The routine to check
;	whether a primary kernel is resident was moved here
;	from tskdos.asm, and the link_ctask routine was added.
;
;	This avoids linking unneeded modules for secondary kernels,
;	and allows code sharing.
;
;---------------------------------------------------------------------------
;
	name	tskres
;
	include	tsk.mac
;
	.tsk_model
;
	IF	DOS
	Pubfunc	ctask_resident
	Pubfunc	link_ctask
	ENDIF
;
	IF	NOT SINGLE_DATA
	public	tsk_global
	ENDIF
	public	tsk_version
;
	.tsk_data
;
	IF	NOT SINGLE_DATA
tsk_global	dd		0
	ENDIF
;
tsk_version	db		"CTask21"
;
	.tsk_edata
	IF	DOS
	.tsk_code
;
Globalfunc ctask_resident,<uses si di>
;
	IFDEF	LOAD_DS
	push	ds
	mov	ax,@CTASK_DATA
	mov	ds,ax
	ENDIF
;
	mov	dx,offset tsk_version
	mov	bx,1234h
	mov	ax,3000h
	int	21h
	xor	ax,ax			; init return code to false
	cmp	bx,0fff0h		; make sure we don't get segment wrap
	jae	not_resident		; can't be right if offset too high
	cmp	cx,0fff0h		; can't be up there, either
	jae	not_resident
	or	cx,cx
	jz	not_resident		; or down there
	mov	di,bx
	mov	es,cx
	mov	si,offset tsk_version
	mov	cx,8
	repe cmpsb
	jnz	not_resident		; Not resident if no match
;
;	CTask is resident, set global variable pointer
;
	mov	word ptr tsk_global,bx
	mov	word ptr tsk_global+2,es
	mov	ax,1
;
not_resident:
	IFDEF	LOAD_DS
	pop	ds
	ENDIF
	ret
;
ctask_resident	endp
;
;
Globalfunc link_ctask
;
	IFDEF	LOAD_DS
	push	ds
	mov	ax,@CTASK_DATA
	mov	ds,ax
	ENDIF
;
	call	ctask_resident
	or	ax,ax
	jz	link_fail
	les	bx,tsk_global
	les	ax,es:stub_table[bx]
	mov	bx,es
	or	ax,bx
	jz	link_fail
	mov	ax,1
;
link_fail:
	IFDEF	LOAD_DS
	pop	ds
	ENDIF
	ret
;
link_ctask	endp
;
	.tsk_ecode
	ENDIF
        end

