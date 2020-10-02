;
;	--- Version 2.2 90-10-12 10:46 ---
;
;	CTask - Support modules for DOS handler, plus keyboard routines.
;
;	Public Domain Software written by
;		Thomas Wagner
;		Ferrari electronic Gmbh
;		Beusselstrasse 27
;		D-1000 Berlin 21
;		Germany
;
;	This file is new with Version 2.1. Those routines which are
;	needed in both primary and secondary kernels were moved here
;	from tskdos.asm and tskkbd.asm.
;
;	This avoids linking unneeded modules for secondary kernels.
;
;---------------------------------------------------------------------------
;
	name	tsksec
;
	include	tsk.mac
;
	.tsk_model
;
	Pubfunc	tsk_free_mem
	Pubfunc	tsk_getpsp
;
	Pubfunc	t_read_key
	Pubfunc	t_wait_key
	Pubfunc	t_keyhit
;
;
; Structure of a DOS MCB (Memory Control Block)
;
mcbstruc	struc
;
mcb_id		db	?
mcb_owner	dw	?
mcb_len		dw	?
;
mcbstruc	ends
;
	.tsk_code
;
Localfunc tsk_free_mem,<owner: word>
;
	mov	ah,51h
	int	21h
	push	bx			; save current PSP
	mov	bx,owner
	mov	ah,50h
	int	21h			; set new PSP
;
free_beg:
	mov	ah,52h
	int	21h			; get DOS invars
	mov	dx,es:[bx-2]		; start of MCB chain
	mov	bx,owner
;
free_loop:
	mov	es,dx
	cmp	es:[mcb_owner],bx
	jne	free_next
;
;	If we have released a block, we restart at the beginning of the
;	MCB chain, since DOS might merge blocks, thereby invalidating
;	our pointers. In the current versions of DOS, this seems to be
;	an unneccessary precaution, but you never know...
;
	inc	dx
	mov	es,dx
	mov	ah,49h			; free memory block
	int	21h
	jmp	free_beg
;
free_next:
	cmp	es:[mcb_id],'Z'		; end of chain ?
	je	free_end		; then exit
	add	dx,es:[mcb_len]
	inc	dx			; point to next
	jmp	free_loop
;
free_end:
	pop	bx
	mov	ah,50h
	int	21h			; restore PSP
	ret
;
tsk_free_mem	endp
;
;---------------------------------------------------------------------------
;---------------------------------------------------------------------------
;
;	int t_read_key (void)
;
;	Waits for key from keyboard. Returns char in lower byte,
;	Scan-Code in upper byte.
;
Globalfunc t_read_key
;
	mov	ax,4112h
	int	16h
	ret
;	
t_read_key	endp
;
;
;	int t_wait_key (dword timeout)
;
;	Waits for key from keyboard. Returns char in lower byte,
;	Scan-Code in upper byte.
;
Globalfunc t_wait_key,<tout: dword>
;
	mov	cx,word ptr (tout+2)
	mov	dx,word ptr (tout)
	mov	ax,4012h
	int	16h
	ret
;	
t_wait_key	endp
;
;
;	int t_keyhit (void)
;
;	Checks if char is available. Returns -1 if not, else the
;	character value. The character remains in the buffer.
;
Globalfunc t_keyhit
;
	mov	ax,4212h
	int	16h
	jnz	t_kh_ok
	mov	ax,-1
t_kh_ok:
	ret
;
t_keyhit	endp
;
;-------------------------------------------------------------------------
;
Localfunc tsk_getpsp
;
	mov	ah,51h
	int	21h
	mov	ax,bx
	ret
;
tsk_getpsp	endp
;
	.tsk_ecode
        end

