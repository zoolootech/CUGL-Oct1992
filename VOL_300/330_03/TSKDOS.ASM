;
;	--- Version 2.2 90-10-12 10:37 ---
;
;	CTask - DOS access module.
;
;	Public Domain Software written by
;		Thomas Wagner
;		Ferrari electronic Gmbh
;		Beusselstrasse 27
;		D-1000 Berlin 21
;		Germany
;
; NOTE: Some routines have been moved to "tsksec.asm" in Version 2.1
;	to support a minimal secondary kernel.
;
;	The support for the critical section interrupt was dropped in
;	version 2.1. Swapping the DOS variable area eliminates
;	the need for maintaining critical sections across calls.
;
; The DOS interrupt (and the related direct disk I/O interrupts) are
; not reentrant. Access to DOS thus has to be channelled such that no
; two tasks use DOS services simultaneously. However, there is one
; exception to this rule. Whenever DOS is waiting for the keyboard, it
; issues a special interrupt, INT 28h, to signal that background
; processing for functions > 0Ch may be performed. This is used in the
; DOS interface for CTask in the following manner:
;
;   A task issuing a DOS interrupt will request one of two resources:
;   "lower_dos" for functions <= 0C, "upper_dos" for functions > 0C.
;   If a task gets access to "lower_dos", it will also request the
;   "upper_dos" resource to lock out other tasks from interrupting.
;   This "upper_dos" resource is shortly released on each INT 28, and
;   then immediately reclaimed, with task priority temporarily raised
;   to the maximum value. The first task waiting to execute a 
;   function > 0C will thus be scheduled to execute the request, but
;   the resource will be reassigned to the INT 28 handler as soon as
;   this request terminates, so the waiting task will not be delayed too
;   long.
;
; There are two additional safety measures which have to be taken to
; avoid getting into conflicts with other resident background programs,
; especially the DOS PRINT background spooler:
;
;   Before actually executing the request, the status of the DOS in-use
;   flag is checked. If this flag is set, the task enters a busy waiting
;   loop, calling the scheduler so that the processor is not tied up.
;
; NOTE: The method for checking the status of DOS is described in-depth
;	in the book "The MS-DOS Encyclopedia" from Microsoft Press, in
;	the chapter on TSR programming. The logic in this module was
;	developed without the help of this book, so if you compare
;	the code here with the routines in the Encyclopedia, you may
;	notice that not all re-entry conditions are checked here.
;	According to my experience with debugging TSR's and CTask, the
;	logic should be sufficient for all but the most obscure TSR's.
;	If you want to be completely on the safe side, you might consider
;	adding the more thorough checks listed in the Encyclopedia.
;
; CAUTION:	This module is not ROMable, and it can only be
;		installed in the primary kernel.
;
	name	tskdos
;
	include	tsk.mac
;
	IF	DOS
;
	.tsk_model
;
	include	tskdeb.h
;
	Pubfunc	tsk_install_dos
	Pubfunc	tsk_remove_dos
	Pubfunc	tsk_fatal
	IF	CHECKING
	Pubfunc	tsk_fatal_pmd
	Globext	tsk_set_currdis
	Globext	tsk_set_dualdis
	Globext	tsk_rputc
	Globext	tsk_rputs
	Globext	tsk_vrprintf
	CGlbext	tsk_rprintf
	ENDIF
;
	public	tsk_emergency_exit
;
	Globext	create_resource
	Globext	delete_resource
	Globext	request_resource
	Globext	request_cresource
	Globext	release_resource
	Globext	create_flag
	Globext	delete_flag
	Globext	set_flag
	Globext	clear_flag
	Globext	wait_flag_clear
	Globext	yield
	Globext	preempt_off
;
	Locext	tsk_switch_stack
	Locext	tsk_old_stack
	Locext	tsk_remove_group
	Locext	tsk_kill_group
	Locext	tsk_remove_tasker
;
	extrn	tsk_instflags: word
;
	extrn	tsk_glob_rec: byte
	IF	NOT SINGLE_DATA
	extrn	tsk_global: dword
	ENDIF
;
get_in_use_flag	=	34h	; DOS-function to get in_use_flag address
get_invars	=	5d06h	; DOS-function to get DOS-variables area
;
psp_offset	=	10h-2	; Offset of current PSP in DOS save area
;				; (-2 because we add 2 to the start)
;
;	Structure of the start of a PSP, including undocumented fields
;
psp_record	struc
;
		dw	?	; INT 20
		dw	?	; alloc block end
		db	?	; reserved
		db	5 dup(?) ; system call
psp_exit_addr	dd	?	; exit routine address
psp_ctlc_addr	dd	?	; control C routine address
psp_cerr_addr	dd	?	; critical error routine address
parent_psp	dw	?	; PSP of parent process
		db	20 dup(?) ; files table
psp_envseg	dw	?	; environment segment
psp_ustack	dd	?	; ss/sp of caller
		dw	?	; file table length
		dd	?	; file table pointer
		dd	?	; pointer to nested PSP (?)
;
psp_record	ends
;
;
intseg	segment at 0
		org	10h*4
vidoff		dw	?	; video interrupt
vidseg		dw	?
		org	13h*4
diskoff		dw	?	; disk i/o interrupt
diskseg		dw	?
		org	20h*4
termoff		dw	?	; program terminate vector
termseg		dw	?
		org	21h*4
idosoff		dw	?	; dos interrupt
idosseg		dw	?
		org	25h*4
absreadoff	dw	?	; absolute disk read
absreadseg	dw	?
		org	26h*4
abswriteoff	dw	?	; absolute disk write
abswriteseg	dw	?
		org	27h*4
keepoff		dw	?	; Terminate but stay resident
keepseg		dw	?
		org	28h*4
idleoff		dw	?	; dos idle interrupt
idleseg		dw	?
		org	2ah*4
csectoff	dw	?	; dos critical section
csectseg	dw	?
		org	40h*4
fdiskoff	dw	?	; redirected floppy disk I/O interrupt
fdiskseg 	dw	?
;
intseg	ends
;
;----------------------------------------------------------------------------
;
;	Variables
;
	.tsk_data
;
idle_active	db	0    		; Idle-Interrupt active
dos310		db	0		; DOS version >= 3.10
;
	IF	TSK_NAMEPAR
udos_name	db	"DOSUPPER",0
ldos_name	db	"DOSLOWER",0
hdio_name	db	"HARDDISK",0
fdio_name	db	"FLEXDISK",0
vid_name	db	"VIDEO",0
	ENDIF
;
;
in_error	dd	?		; Adress of DOS error-mode-flag
;
lower_dos	resource <>
upper_dos	resource <>
hdisk_io	resource <>
fdisk_io	resource <>
video		resource <>
;
vid_flags	dw	?
i13h_flags	dw	?
i13f_flags	dw	?
;
	.tsk_edata
	.tsk_code
;
;---------------------------------------------------------------------------
;
;	Original Interrupt-Entries
;
savvid		label	dword		; original Video entry
savvidoff	dw	?
savvidseg	dw	?
;
savdisk		label	dword		; original Disk I/O entry
savdiskoff	dw	?
savdiskseg	dw	?
;
savfdisk 	label	dword		; original Floppy Disk I/O entry
savfdiskoff	dw	?
savfdiskseg	dw	?
;
savtermoff	dw	?		; Terminate vector save
savtermseg	dw	?
;
savdos		label	dword		; original DOS-Entry
savdosoff	dw	?
savdosseg	dw	?
;
savidle		label	dword		; original IDLE-Entry
savidleoff	dw	?
savidleseg	dw	?
;
savcsect	label	dword		; Critical Section save
savcsectoff	dw	?
savcsectseg	dw	?
;
savabsread	label	dword		; Absolute Disk Read save
savabsreadoff	dw	?
savabsreadseg	dw	?
;
savabswrite	label	dword		; Absolute Disk Write save
savabswriteoff	dw	?
savabswriteseg	dw	?
;
savkeepoff	dw	?		; Terminate resident vector save
savkeepseg	dw	?
;
;
dos_version	dw	?		; DOS version
;
temp_1		dw	?
;
tsk_dgroup	dw	@CTASK_DATA
;
zero		db	0
;
calldos	macro
	pushf
	cli
	call	cs:savdos
	endm
;
;---------------------------------------------------------------------------
;
;	void tsk_install_dos (void)
;
;		Install DOS handler
;
Localfunc tsk_install_dos
;
;	create needed resources & flags
;
	IFDEF	LOAD_DS
	push	ds
	mov	ax,@CTASK_DATA
	mov	ds,ax
	ENDIF
;
	IF	TSK_NAMEPAR
	callp	create_resource,<<ds,#upper_dos>,<ds,#udos_name>>
	ELSE
	callp	create_resource,<<ds,#upper_dos>>
	ENDIF
;
	IF	TSK_NAMEPAR
	callp	create_resource,<<ds,#lower_dos>,<ds,#ldos_name>>
	ELSE
	callp	create_resource,<<ds,#lower_dos>>
	ENDIF
;
	test	tsk_instflags,IFL_DISK
	jz	inst_nodsk1
	IF	TSK_NAMEPAR
	callp	create_resource,<<ds,#hdisk_io>,<ds,#hdio_name>>
	ELSE
	callp	create_resource,<<ds,#hdisk_io>>
	ENDIF
;
	IF	TSK_NAMEPAR
	callp	create_resource,<<ds,#fdisk_io>,<ds,#fdio_name>>
	ELSE
	callp	create_resource,<<ds,#fdisk_io>>
	ENDIF
;
inst_nodsk1:
	test	tsk_instflags,IFL_VIDEO
	jz	inst_novid1
	IF	TSK_NAMEPAR
	callp	create_resource,<<ds,#video>,<ds,#vid_name>>
	ELSE
	callp	create_resource,<<ds,#video>>
	ENDIF
;
inst_novid1:
;
	mov	tsk_glob_rec.l_swap,0	; init swap area length to 0
;
;	Get the DOS version. Only versions 3.1 and up support the
;	DOS variable swap, so disable swapping if it's an earlier version,
;	or if it's the OS/2 box.
;
	mov	ah,30h
	int	21h
	mov	cs:dos_version,ax
	cmp	al,3
	jb	not_dos3
	cmp	al,0ah
	je	not_dos3	; OS/2 compatibility box
        cmp     al,3
        jne     is_dos3         ; DOS 4.x 
	cmp	ah,10
	jb	not_dos3
is_dos3:
	inc	dos310
;
;	Get address and length of the DOS internal variable area.
;	This call is not documented, and not available in versions
;	prior to 3.1. It returns the address of the area in DS:SI,
;	and the length of the area that must be swapped between calls
;	from different groups in DX.
;	Additionally, the total length of the area, including
;	the DOS-stacks, minus the length in DX, is returned in CX.
;
;	The DOS in_error flag is at the start of this area, the
;	in_use flag is the second byte. Both bytes are excluded 
;	from the swap.
;
	test	tsk_instflags,IFL_NODOSVARS
	jnz	not_dos3
;
	push	ds
	mov	ax,get_invars
	int	21h
	mov	ax,ds
	pop	ds
	mov	word ptr in_error,si
	mov	word ptr in_error+2,ax
	inc	si
	mov	word ptr tsk_glob_rec.dos_in_use,si
	mov	word ptr tsk_glob_rec.dos_in_use+2,ax
	inc	si
	mov	word ptr tsk_glob_rec.dos_vars,si
	mov	word ptr tsk_glob_rec.dos_vars+2,ax
	sub	dx,2
	mov	tsk_glob_rec.l_swap,dx
	jmp	short save_ints
;
;	For versions < 3.10, and if the IFL_NODOSVARS flag is set,
;	get the address of DOS's in_use-flag. This flag indicates that
;	DOS is already active. This might happen if there are other
;	background tasks, like popups or print spoolers, active in
;	parallel to CTask.
;	This is also the address of the critical error flag in DOS. Beginning 
;	with DOS 3.10 the flag is located one byte before the in_use_flag.
;	With older DOS versions, we would have to search through DOS for the 
;	address. This is omitted here, but you could include the code
;	for pre 3.1 versions from pages 378-379 of the MS-DOS Encyclopedia.
;
not_dos3:
	mov	ah,get_in_use_flag
	int	21h
	mov	word ptr tsk_glob_rec.dos_in_use,bx
	mov	word ptr tsk_glob_rec.dos_in_use+2,es
	mov	word ptr in_error+2,es
	dec	bx
	mov	word ptr in_error,bx
	cmp	dos310,0
	jne	save_ints		; OK for DOS >= 3.10
;
;	For DOS < 3.1, we simply ignore the error flag.
;	CTask's DOS handler will not work that great anyway with older
;	versions.
;
	mov	word ptr in_error,offset zero
	mov	word ptr in_error+2,cs
;
;	Save old interrupt vectors
;
save_ints:
        push    es
	xor	ax,ax
	mov	es,ax
;
        assume  es:intseg
;
	test	tsk_instflags,IFL_VIDEO
	jz	inst_novid2
	mov	ax,vidoff		; video
	mov	savvidoff,ax
	mov	ax,vidseg
	mov	savvidseg,ax
;
inst_novid2:
	test	tsk_instflags,IFL_DISK
	jz	inst_nodsk2
	mov	ax,diskoff		; Disk I/O
	mov	savdiskoff,ax
	mov	ax,diskseg
	mov	savdiskseg,ax
	mov	ax,fdiskoff		; Floppy Disk I/O
	mov	savfdiskoff,ax
	mov	ax,fdiskseg
	mov	savfdiskseg,ax
;
inst_nodsk2:
	mov	ax,termoff		; DOS
	mov	savtermoff,ax
	mov	ax,termseg
	mov	savtermseg,ax
;
	mov	ax,idosoff		; DOS
	mov	savdosoff,ax
	mov	ax,idosseg
	mov	savdosseg,ax
;
	mov	ax,idleoff		; IDLE
	mov	savidleoff,ax
	mov	ax,idleseg
	mov	savidleseg,ax
;
	mov	ax,csectoff		; Critical Section
	mov	savcsectoff,ax
	mov	ax,csectseg
	mov	savcsectseg,ax
;
	mov	ax,absreadoff		; Absolute Disk read
	mov	savabsreadoff,ax
	mov	ax,absreadseg
	mov	savabsreadseg,ax
;
	mov	ax,abswriteoff		; Absolute Disk write
	mov	savabswriteoff,ax
	mov	ax,abswriteseg
	mov	savabswriteseg,ax
;
	mov	ax,keepoff		; Terminate Resident
	mov	savkeepoff,ax
	mov	ax,keepseg
	mov	savkeepseg,ax
;
;	Enter new Interrupt-Entries
;
	cli
	test	tsk_instflags,IFL_VIDEO
	jz	inst_novid3
	mov	vidoff,offset @videntry		; Video Entry
	mov	vidseg,cs
;
inst_novid3:
	test	tsk_instflags,IFL_DISK
	jz	inst_nodsk3
	mov	diskoff,offset @diskentry	; Disk I/O Entry
	mov	diskseg,cs
	mov	fdiskoff,offset @fdiskentry	; Floppy Disk I/O Entry
	mov	fdiskseg,cs
;
inst_nodsk3:
	mov	idosoff,offset @dosentry	; DOS-Entry
	mov	idosseg,cs
	mov	idleoff,offset @idleentry	; Idle-Entry
	mov	idleseg,cs
	mov	termoff,offset terminate_int	; Terminate Process Entry
	mov	termseg,cs
	mov	csectoff,offset @critsectint	; Critical Section Entry
	mov	csectseg,cs
	mov	keepoff,offset keep_int		; Keep Process Entry
	mov	keepseg,cs
	mov	absreadoff,offset absread_int	; Absolute Disk Read Entry
	mov	absreadseg,cs
	mov	abswriteoff,offset abswrite_int	; Absolute Disk Write Entry
	mov	abswriteseg,cs
	sti
;
	assume	es:nothing
;
        pop     es
	IFDEF	LOAD_DS
	pop	ds
	ENDIF
	ret
;
;
tsk_install_dos	endp
;
;---------------------------------------------------------------------------
;
;	void tsk_remove_dos (void)
;
;		Un-install DOS handler
;
Localfunc tsk_remove_dos
;
	IFDEF	LOAD_DS
	push	ds
	mov	ax,@CTASK_DATA
	mov	ds,ax
	ENDIF
;
;	Delete resources & flags
;
	callp	delete_resource,<<ds,#upper_dos>>
	callp	delete_resource,<<ds,#lower_dos>>
;
	test	tsk_instflags,IFL_DISK
	jz	rem_nodsk1
	callp	delete_resource,<<ds,#hdisk_io>>
	callp	delete_resource,<<ds,#fdisk_io>>
;
rem_nodsk1:
	test	tsk_instflags,IFL_VIDEO
	jz	rem_novid1
	callp	delete_resource,<<ds,#video>>
;
rem_novid1:
        push    es
	xor	ax,ax
	mov	es,ax
;
        assume  es:intseg
;
;	Restore interrupt entries
;
	cli
	test	tsk_instflags,IFL_VIDEO
	jz	rem_novid2
	mov	ax,savvidoff
	mov	vidoff,ax
	mov	ax,savvidseg
	mov	vidseg,ax
;
rem_novid2:
	test	tsk_instflags,IFL_DISK
	jz	rem_nodsk2
	mov	ax,savdiskoff
	mov	diskoff,ax
	mov	ax,savdiskseg
	mov	diskseg,ax
	mov	ax,savfdiskoff
	mov	fdiskoff,ax
	mov	ax,savfdiskseg
	mov	fdiskseg,ax
;
rem_nodsk2:
	mov	ax,savtermoff
	mov	termoff,ax
	mov	ax,savtermseg
	mov	termseg,ax
;
	mov	ax,savdosoff
	mov	idosoff,ax
	mov	ax,savdosseg
	mov	idosseg,ax
;
	mov	ax,savidleoff
	mov	idleoff,ax
	mov	ax,savidleseg
	mov	idleseg,ax
;
	mov	ax,savcsectoff
	mov	csectoff,ax
	mov	ax,savcsectseg
	mov	csectseg,ax
;
	mov	ax,savabsreadoff
	mov	absreadoff,ax
	mov	ax,savabsreadseg
	mov	absreadseg,ax
;
	mov	ax,savabswriteoff
	mov	abswriteoff,ax
	mov	ax,savabswriteseg
	mov	abswriteseg,ax
;
	mov	ax,savkeepoff
	mov	keepoff,ax
	mov	ax,savkeepseg
	mov	keepseg,ax
;
	sti
;
        pop     es
	IFDEF	LOAD_DS
	pop	ds
	ENDIF
	ret
;
	assume	es:nothing
;
tsk_remove_dos	endp
;
;
;---------------------------------------------------------------------------
;---------------------------------------------------------------------------
;
;
;	INT 10: Video BIOS interrupt
;
@videntry	proc	far
;
	call	tsk_switch_stack
	callp	request_cresource,<<ds,#video>,0,0>
	mov	ax,caller_flags[bp]
	mov	vid_flags,ax
	call	tsk_old_stack
	pushf
	cli
	call	savvid
	call	tsk_switch_stack
	mov	ax,vid_flags
	mov	caller_flags[bp],ax
	callp	release_resource,<<ds,#video>>
	iret
;
@videntry	endp
;
;---------------------------------------------------------------------------
;
;
;	INT 13 and INT 40: Disk I/O BIOS interrupt
;
@diskentry	proc	far
;
	call	tsk_switch_stack
	test	dl,80h			; fixed disk ?
	jz	disk_floppy		; jump if not
	callp	request_cresource,<<ds,#hdisk_io>,0,0>
	call	tsk_old_stack
	pushf
	cli
	call	savdisk
	call	tsk_switch_stack
	callp	release_resource,<<ds,#hdisk_io>>
	mov	ax,entry_flags[bp]
	mov	caller_flags[bp],ax
	iret
;
disk_floppy:
	callp	request_cresource,<<ds,#fdisk_io>,0,0>
	call	tsk_old_stack
	pushf
	cli
	call	savdisk
	call	tsk_switch_stack
	callp	release_resource,<<ds,#fdisk_io>>
	mov	ax,entry_flags[bp]
	mov	caller_flags[bp],ax
	iret
;
@diskentry	endp
;
@fdiskentry	proc	far
;
	call	tsk_switch_stack
	callp	request_cresource,<<ds,#fdisk_io>,0,0>
	call	tsk_old_stack
	pushf
	cli
	call	savfdisk
	call	tsk_switch_stack
	callp	release_resource,<<ds,#fdisk_io>>
	mov	ax,entry_flags[bp]
	mov	caller_flags[bp],ax
	iret
;
@fdiskentry	endp
;
;---------------------------------------------------------------------------
;
;	Stack-Offsets relative to BP
;
d_func	=	-2
;
;---------------------------------------------------------------------------
;
;	INT 25: Absolute Disk Read 
;
;	This interrupt is channeled through the normal DOS-function
;	processing, with function = 0x25 and special flag set.
;	It is re-translated later after the necessary resources
;	have been requested. INT 25 is handled like a function > 0C.
;	Interrupts 25 und 26 leave the flag-word on the stack.
;	Since flags are removed in normal processing, the flag-word
;	has to be duplicated on the stack here.
;
absread_int:
	sti
	push	bp			; reserve space
	push	bp			; save BP
	mov	bp,sp
	push	ax
	mov	ax,4[bp]		; Move return offset, segment down
	mov	2[bp],ax
	mov	ax,6[bp]
	mov	4[bp],ax
	mov	ax,8[bp]		; duplicate flags
	mov	6[bp],ax
	pop	ax
	pop	bp
	call	tsk_switch_stack
	mov	ax,2501h
	mov	cx,ax
	jmp	short dosentry_2
;
;
;---------------------------------------------------------------------------
;
;	INT 26: Absolute Disk Write
;
;	This interrupt is channeled through the normal DOS-function
;	processing, with function = 0x26 and special flag set.
;	It is re-translated later after the necessary resources
;	have been requested. INT 26 is handled like a function > 0C.
;	Interrupts 25 und 26 leave the flag-word on the stack.
;	Since flags are removed in normal processing, the flag-word
;	has to be duplicated on the stack here.
;
abswrite_int:
	sti
	push	bp			; reserve space
	push	bp			; save BP
	mov	bp,sp
	push	ax
	mov	ax,4[bp]		; Move return offset, segment down
	mov	2[bp],ax
	mov	ax,6[bp]
	mov	4[bp],ax
	mov	ax,8[bp]		; duplicate flags
	mov	6[bp],ax
	pop	ax
	pop	bp
	call	tsk_switch_stack
	mov	ax,2601h
	mov	cx,ax
	jmp	short dosentry_2
;
;---------------------------------------------------------------------------
;
;	INT 27: Terminate But Stay Resident Interrupt
;
;	This interrupt is translated to INT 21, function 31.
;
keep_int:
	call	tsk_switch_stack
;
	add	dx,0fh			; last addr + 0f to round
	sub	dx,caller_cs[bp]	; minus CS (= PSP)
	mov	cl,4
	shr	dx,cl			; div 16 = paragraphs

	mov	ax,3100h		; Keep process
	mov	save_ax[bp],ax
	mov	cx,ax
	jmp	short dosentry_2
;
;---------------------------------------------------------------------------
;
;	INT 20: Terminate Program interrupt
;
;	This interrupt is translated to INT 21, function 4c.
;
terminate_int:
	call	tsk_switch_stack
	mov	ax,4c00h
	mov	save_ax[bp],ax
	mov	cx,ax
	jmp	short dosentry_2
;
;---------------------------------------------------------------------------
;
;	INT 21: DOS-Interrupt
;
@dosentry        proc    far
;
	call	tsk_switch_stack
	xor	cl,cl
	mov	ch,ah
;
dosentry_2:
	push	cx		; BP-2: func in CH, special flag in CL
	or	cl,cl
	jnz	dosent_x	; no function check if special
;
;	Check if this is a special 'get dos version' call to determine
;	if CTask is resident.
;
;	A special call has
;
;		AX = 3000h
;		BX = 1234h
;		DS:DX = pointer to version string
;
	cmp	ax,3000h
	jne	no_spdos
	cmp	bx,1234h
	jne	no_spdos
	cmp	dx,0fff0h
	jae	no_spdos
	push	ds
	mov	si,dx
	mov	ds,save_ds[bp]
	mov	di,offset tsk_glob_rec
	mov	cx,8
	repe cmpsb
	pop	ds
	jz	is_spdos
	jmp	short no_spdos
;
;	Special version call returns global variable block address
;	in BX (offset) and CX (segment).
;
is_spdos:
	add	sp,2
	call	tsk_old_stack
	mov	ax,cs:dos_version
	mov	bx,offset tsk_glob_rec
	mov	cx,@CTASK_DATA
	iret
;
;---------------------------------------------------------------------------
;
;
no_spdos:
	mov	ax,d_func[bp]
        or	ah,ah			; terminate?
        jne     dosent_x
        mov     save_ax[bp],4c00h	; translate to fn 4c, retcode 0
        mov     d_func[bp],4c00h
;
;	Now the real fun begins.
;
dosent_x:
	sti				; Interrupts allowed now
;
;	DL is used as an "emergency" marker. If nonzero, no resources
;	are to be requested, and the in-dos flag is not to be checked.
;	There is an "emergency" if the task is marked as owning
;	DOS resources, but re-enters INT 21.
;
	xor	dl,dl
	les	bx,tsk_glob_rec.current_task
	test	es:t_indos[bx],OWN_UPPER OR OWN_LOWER
	jz	no_reenter		; no trouble if task doesn't own DOS
;
	call	@dosbusy		; this sets DL nonzero if busy
	jnz	no_reenter		; Don't release resources if busy
;
;	If the task owns DOS resources, but DOS is not busy, something
;	strange is going on (this case should have been handled by the
;	'schedent' routine). Anyway, we assume that DOS is right about
;	not being busy, and release the resources. Although the resources
;	will be requested again a few instructions later, this is sensible
;	to give other tasks that might be waiting on the resource a chance
;	to execute.
;
	call	@relres
	xor	dl,dl
;
;	The preliminaries are done with, we now have to distinguish
;	between functions 00-0C and 0D-FF.
;
no_reenter:
	mov	ax,d_func[bp]
	cmp	ah,0ch
	jbe	lower_funcs
	jmp	upper_funcs
;
;
;	Functions 00-0C
;
lower_funcs:
;
;	first, request the "lower_dos" resource unless this is an emergency.
;
	or	dl,dl
	jnz	lower_emergency
	callp	request_resource,<<ds,#lower_dos>,0,0>
;
;	we have it, now let's get the upper_dos resource, too
;
	callp	request_resource,<<ds,#upper_dos>,0,0>
;
	les	bx,tsk_glob_rec.current_task
	mov	es:t_indos[bx],OWN_UPPER or OWN_LOWER
;
;	both resources gained, now we may execute the function if dos is free
;
	call	@wait_dos_free
;
;	Set the scheduler entry function
;
	cli
	mov	word ptr es:sched_ent_func[bx],offset @schedent
	mov	word ptr es:sched_ent_func+2[bx],cs
	sti
;
lower_emergency:
	add	sp,2
	push	caller_flags[bp]
	popf
	call	tsk_old_stack
;
	calldos				; execute function
;
;	Now we have to release the resources, unless DOS still indicates
;	that it is busy. This would indicate that a critical error has
;	occurred, and DOS processing is not really complete.
;
	call	tsk_switch_stack
;
	call	@dosbusy
	jnz	no_relc			; return if DOS still busy
;
;	Clear the flags in the TCB, and the scheduler entry function.
;
	les	bx,tsk_glob_rec.current_task
	cli
	mov	es:t_indos[bx],0
	mov	word ptr es:sched_ent_func[bx],0
	mov	word ptr es:sched_ent_func+2[bx],0
	sti
;
	callp	release_resource,<<ds,#upper_dos>>
	callp	release_resource,<<ds,#lower_dos>>
;
;	All done, restore registers and return.
;
no_relc:
	cli
	mov	ax,entry_flags[bp]
	mov	caller_flags[bp],ax
	iret
;
;--------------------------------------------------------------------------
;
;	Functions 0D and above
;
upper_funcs:
;
;	first, request the "upper_dos" resource unless this is an emergency.
;
	or	dl,dl
	jnz	upper_emergency
;
	callp	request_resource,<<ds,#upper_dos>,0,0>
;
	les	bx,tsk_glob_rec.current_task
	mov	es:t_indos[bx],OWN_UPPER
;
;	resource gained, now we may execute the function if dos is free
;
	call	@wait_dos_free
;
;	Set the scheduler entry function
;
	cli
	mov	word ptr es:sched_ent_func[bx],offset @schedent
	mov	word ptr es:sched_ent_func+2[bx],cs
	sti
;
upper_emergency:
	cmp	byte ptr d_func[bp],0
	jne	no_term
	mov	ax,save_ax[bp]
	cmp	ah,31h		; terminate resident?
	jne	ckfunc1
	jmp	term_resident
ckfunc1:
	cmp	ax,4b00h	; spawn new process?
	jne	ckfunc2
	jmp	terminate
ckfunc2:
	cmp	ah,4ch		; terminate program?
	jne	no_term
	jmp	terminate
;
no_term:
;
;	Filter special-functions 25/26 (Absolute Read/Write)
;
	pop	ax
	cmp	ax,2501h
	jne	uf_exec1
	push	caller_flags[bp]
	popf
	call	tsk_old_stack
	pushf
	cli
	call	cs:savabsread
	pop	cs:temp_1		; remove flags
	jmp	short uf_complete
;
uf_exec1:
	cmp	ax,2601h
	jne	uf_exec2
	push	caller_flags[bp]
	popf
	call	tsk_old_stack
	pushf
	cli
	call	cs:savabswrite
	pop	cs:temp_1		; remove flags
	jmp	short uf_complete
;
uf_exec2:
	push	caller_flags[bp]
	popf
	call	tsk_old_stack
;
	calldos				; execute function
;
;	Now we have to release the resources, unless DOS still indicates
;	that it is busy. This would indicate that a critical error has
;	occurred, and DOS processing is not really complete.
;	However, if we were called from INT 28, DOS must still be busy,
;	so we only check the in_error flag.
;
uf_complete:
	call	tsk_switch_stack
;
	cmp	idle_active,0
	jnz	uf_nobsychk		; don't check busy if INT 28 call
	les	bx,tsk_glob_rec.dos_in_use
	cmp	es:byte ptr [bx],0
	jne	no_relc_uf		; don't release if busy
;
uf_nobsychk:
	les	bx,in_error
	cmp	es:byte ptr [bx],0
	jnz	no_relc_uf		; don't release if in error
;
	les	bx,tsk_glob_rec.current_task
	cli
	mov	es:t_indos[bx],0
	mov	word ptr es:sched_ent_func[bx],0
	mov	word ptr es:sched_ent_func+2[bx],0
	sti
;
	callp	release_resource,<<ds,#upper_dos>>
;
;	All done, restore registers and return.
;
no_relc_uf:
	cli
	mov	ax,entry_flags[bp]
	mov	caller_flags[bp],ax
	iret
;
;--------------------------------------------------------------------------
;
;	Terminate, TSR and Spawn calls go directly to DOS.
;	TSR has to reset the exit address if it's the PSP of the 
;	current group, so the group is not deleted.
;
term_resident:
	les	bx,tsk_glob_rec.dos_vars
	mov	ax,es:[bx+psp_offset]		; current PSP
	les	bx,tsk_glob_rec.current_task
	les	bx,es:tgroup[bx]
	cmp	es:gcreate_psp[bx],ax
	jne	terminate
	push	ds
	mov	ds,ax
	mov	ax,word ptr es:grp_exit_addr[bx]
	mov	word ptr ds:psp_exit_addr,ax
	mov	ax,word ptr es:grp_exit_addr+2[bx]
	mov	word ptr ds:psp_exit_addr+2,ax
	pop	ds
;
terminate:
	add	sp,2
	push	caller_flags[bp]
	popf
	call	tsk_old_stack
	cli
	jmp	cs:savdos
;
@dosentry	endp
;
;--------------------------------------------------------------------------
;
;	'dosbusy' checks the DOS busy flags.
;
;	Entry:	-
;	Exit:	DL is nonzero if DOS is busy.
;
@dosbusy	proc	near
;
	push	bx
	push	es
	les	bx,in_error
	mov	dl,es:byte ptr [bx]
	les	bx,tsk_glob_rec.dos_in_use
	or	dl,es:byte ptr [bx]
	pop	es
	pop	bx
	ret
;
@dosbusy	endp
;
;--------------------------------------------------------------------------
;
;	Wait_dos_free uses busy waiting (calling _yield) in case
;	some other background program has entered DOS without going
;	through our INT 21 interface.
;
@wait_dos_free	proc	near
	push	es
	push	bx
;
in_use_loop:
	les	bx,in_error
	cmp	byte ptr es:[bx],0
	jne	is_in_use		; wait if in_error set
;
	cmp	idle_active,0		; idle interrupt active?
	je	ck_inuse		; check for flag if no
	pop	bx
	pop	es
	ret				; else return immediately
;
ck_inuse:
	les	bx,tsk_glob_rec.dos_in_use
	cmp	byte ptr es:[bx],0
	jne	is_in_use
	pop	bx
	pop	es
	ret
;
is_in_use:
	call	yield
	jmp	in_use_loop
;
@wait_dos_free	endp
;
;----------------------------------------------------------------------------
;
;	'relres' releases the DOS-resources of a task.
;
;	Entry:
;		ES:BX = TCB pointer
;
@relres	proc	near
;
	test	es:t_indos[bx],OWN_UPPER
	jz	rel_lower
	xor	es:t_indos[bx],OWN_UPPER
	push	bx
	push	es
	callp	release_resource,<<ds,#upper_dos>>
	pop	es
	pop	bx
;
rel_lower:
	test	es:t_indos[bx],OWN_LOWER
	jz	rel_exit
	xor	es:t_indos[bx],OWN_LOWER
	push	bx
	push	es
	callp	release_resource,<<ds,#lower_dos>>
	pop	es
	pop	bx
;
rel_exit:
	ret
;
@relres	endp
;
;----------------------------------------------------------------------------
;
;	The 'schedent' function is called by the scheduler if the
;	scheduler is entered and the current task owns one of the
;	DOS resources.
;	This routine checks whether the DOS busy flags are clear now,
;	which would mean that the task left DOS without passing through
;	the return address. If that's the case, we release the resources.
;
;	The only catch is that there is a race condition if we just check
;	the busy flags. There is a time slot where the function is set in
;	the TCB, but the busy flag is clear because DOS has not yet been
;	entered. Since we have no control over what will happen, and how 
;	long it will take to reach the point where the in_use flag is 
;	incremented, we can't simply disable preemption. So what we have
;	to do is to wait for some other indication that DOS has begun
;	executing the request, and mark this in the TCB. In all versions
;	starting at 3.1, DOS will call the 'critical section' INT 2A
;	with AX=8200 on functions > 0C after incrementing the in_use flag.
;	We also have some indication that DOS is active if the 'idle'
;	INT 28 is called, or if the in_use flag is set when we get here.
;
;	Entry:
;		DS	CTask data segment
;		BX	Global data block offset
;		ES:DI	Task control block
;
@schedent	proc	far
;
	call	@dosbusy		; DOS still busy ?
	jz	schedent_relres		; release resources if not busy
	or	es:t_indos[di],DOS_ENTERED
	ret
;
schedent_relres:	
	test	es:t_indos[di],DOS_ENTERED
	jz	schedent_ret		; don't release if DOS not yet entered
;
	mov	bx,di
	call	@relres
;
schedent_ret:
	ret
;
@schedent	endp
;
;----------------------------------------------------------------------------
;
;	INT 28: DOS Idle Interrupt
;
@idleentry	proc	far
;
	call	tsk_switch_stack
;
;	check the in_use flag. If it's set, set the DOS_ENTERED flag
;	in the current TCB.
;
	les	bx,tsk_glob_rec.dos_in_use
	cmp	byte ptr es:[bx],0
	je	no_entermark
;
	les	bx,tsk_glob_rec.current_task
	or	es:t_indos[bx],DOS_ENTERED
;
;	Check if someone is waiting for upper_dos. If not, we can return
;	immediately.
;
no_entermark:
	les	bx,upper_dos.rwaiting.q_first
	test	es:q_kind[bx],Q_HEAD
	jnz	idle_exit
;
;	Also make sure this is not a second invocation of INT 28, and
;	that the in_error flag is not set.
;	Normally, this should never happen, but better safe than sorry.
;
	cmp	idle_active,0
	jne	idle_exit
	les	bx,in_error
	cmp	byte ptr es:[bx],0
	jne	idle_exit
;
	inc	idle_active
;
;	someone is waiting, let's please him by releasing the resource.
;	temporarily increase priority
;
	les	bx,tsk_glob_rec.current_task
	push	es:cqueue.q_el.q_prior[bx]
	push	es:cqueue.q_el.q_ini_prior[bx]
	mov	es:cqueue.q_el.q_prior[bx],0ffffh
	mov	es:cqueue.q_el.q_ini_prior[bx],0ffffh
	push	bx
	push	es
;
;	release resource & request it again
;
	callp	release_resource,<<ds,#upper_dos>>
	callp	request_resource,<<ds,#upper_dos>,0,0>
;
;	ready, restore priority
;
	cli
	pop	es
	pop	bx
	pop	es:cqueue.q_el.q_ini_prior[bx]
	pop	es:cqueue.q_el.q_prior[bx]
;
	mov	idle_active,0
;
idle_exit:
	push	caller_flags[bp]
	popf
	call	tsk_old_stack
	cli
;
	jmp	cs:savidle		; chain to original interrupt
;
@idleentry	endp
;
;----------------------------------------------------------------------------
;
;	INT 2A: DOS Critical Section Interrupt
;
;	Not documented.
;	Is used by DOS PRINT to mark Critical Regions.
;	Usage by PRINT:
;		AX = 8700 - Begin Critical Region
;				Returns:
;				Carry set if already active.
;		AX = 8701 - End Critical Region
;
;	Other usage in DOS, function unknown:
;		AH = 82 (AL undefined)
;			seems to be called on DOS-Functions > 0C
;		AH = 84 (AL undefined)
;			seems to be called when DOS is idle
;
;	We only handle function number 82 in this version,
;	it is used to mark that DOS has been entered. All functions
;	are passed on to the next in chain.
;
@critsectint	proc	far
;
	cmp	ah,82h
	jne	cs_exit
	push	ds
	push	es
	push	bx
	mov	ds,cs:tsk_dgroup
;
;	check the in_use flag. If it's set, set the DOS_ENTERED flag
;	in the current TCB.
;
	les	bx,tsk_glob_rec.dos_in_use
	cmp	byte ptr es:[bx],0
	je	cs_no_enter
;
	les	bx,tsk_glob_rec.current_task
	or	es:t_indos[bx],DOS_ENTERED
;
cs_no_enter:
	pop	bx
	pop	es
	pop	ds
;
cs_exit:
	jmp	cs:savcsect
;
@critsectint	endp
;
;---------------------------------------------------------------------------
;---------------------------------------------------------------------------
;
term_err_msg	db	0dh,0ah,"Program terminated - CTask uninstalled"
		db	0dh,0ah,'$'
;
group_term_msg	db	0dh,0ah,"Program terminated - Task Group removed"
		db	0dh,0ah,'$'
;
gcb_mixup_err	db	0dh,0ah,"Group chain damaged - System halted"
		db	07h,'$'
;
;---------------------------------------------------------------------------
;
;	tsk_emergency_exit is entered by DOS when a task group exits
;	without releasing the current group.
;	Registers are set up, remove_group is called, and the program
;	is terminated by jumping to the terminate_address.
;
tsk_emergency_exit	proc	far
;
	pushf
	sub	sp,4		; make room for return addr
	push	bp
	mov	bp,sp
	push	ax
	push	bx
	push	cx
	push	dx
	push	si
	push	di
	push	ds
	push	es
	mov	ax,@CTASK_DATA
	mov	ds,ax
;
	les	bx,tsk_glob_rec.current_task[bx]
	les	bx,es:tgroup[bx]
	mov	ax,word ptr es:grp_exit_addr[bx]
	mov	2[bp],ax
	mov	ax,word ptr es:grp_exit_addr+2[bx]
	mov	4[bp],ax
;
	callp	tsk_remove_group,<<es,bx>,0>
;
	mov	dx,offset group_term_msg
        cmp	ax,0
	je	emergency_end
	jb	pg_fatal
;
	les	bx,tsk_glob_rec.current_task[bx]
	les	bx,es:tgroup[bx]
	callp	tsk_kill_group,<<es,bx>>
	call	tsk_remove_tasker
;
	mov	dx,offset term_err_msg
;
emergency_end:
;
	mov	ax,cs
	mov	ds,ax
	mov	ah,9
	int	21h
;
	pop	es
	pop	ds
	pop	di
	pop	si
	pop	dx
	pop	cx
	pop	bx
	pop	ax
	pop	bp
	iret
;
pg_fatal:
	mov	si,offset gcb_mixup_err
	jmp	short fatal_error
;
tsk_emergency_exit	endp
;
;--------------------------------------------------------------------------
;
;	fatal_error can be called if the system can't continue for
;	some reason. It issues an error message and halts the system.
;	SI must point to the '$'-terminated error message.
;
;	tsk_fatal does the same, but assumes C calling sequence and
;	a zero-terminated string.
;
Globalfunc tsk_fatal,<strp: far ptr>
;
	cld
	lds	si,strp
	jmp	short dis_err
;
tsk_fatal	endp
;
fatal_error:
	mov	ax,cs
	mov	ds,ax
;
dis_err:
	IF	IBM
	xor	ax,ax
	int	10h		; re-init display
fatal_loop:
	lodsb
	cmp	al,'$'
	je	fatal_end
	or	al,al
	jz	fatal_end
        mov     bx,7
        mov     ah,14
	int	10h
	jmp	fatal_loop
	ELSE
	mov	dx,si
	mov	ah,9
	int	21h
	ENDIF
fatal_end:
	sti
	jmp	fatal_end
;
;--------------------------------------------------------------------------
;
;	void CGlobalfunc tsk_fatal_pmd (byteptr string, ...)
;
;	tsk_fatal_pmd is only present in "checking" mode.
;	It does a minimal "post mortem dump", and displays the
;	error message passed as parameter.
;	The messages are output to both the primary and secondary 
;	monitor via the "regen" printf routines in tskprf.
;	The system is then halted, this routine never returns.
;
	IF	CHECKING
;
;
CGlobalfunc	tsk_fatal_pmd,<txt: far ptr,argp:far ptr>
;
;	int	3	; uncomment to break to debugger
	push	sp
	push	ss
	push	es
	push	ds
	push	di
	push	si
	push	dx
	push	cx
	push	bx
	push	ax
;
	mov	ds,tsk_dgroup
	callp	preempt_off
	callp	tsk_set_currdis
;
	callp	tsk_rputc,<0ch>
	les	di,txt
	lea	bx,argp
	call	@fatal_dump
;
	callp	tsk_set_dualdis
	or	ax,ax
	jz	no_second
;
	callp	tsk_rputc,<0ch>
	les	di,txt
	lea	bx,argp
	call	@fatal_dump
;
no_second:
	jmp	fatal_end
;
tsk_fatal_pmd	endp
;
;
pmd1	db	0ah
	db	'AX = %04X  BX = %04X  CX = %04X  DX = %04X',0ah
	db	'SI = %04X  DI = %04X  DS = %04X  ES = %04X',0ah
	db	'SS = %04X  SP = %04X  BP = %04X',0ah
	db	'IP = %04X  CS = %04X',0ah,0
pmd2	db	'Current Task = %FP'
	IF	TSK_NAMED
	db	' (%s)'
	ENDIF
	db	0ah,0
pmde	db	'System Halted',0
;
;
@fatal_dump	proc	near
;
	callp	tsk_vrprintf,<<es,di>,<ss,bx>>
	mov	bx,sp
	add	bx,2
	callp	tsk_vrprintf,<<cs,#pmd1>,<ss,bx>>
;
	IF	SINGLE_DATA
	mov	bx,offset tsk_glob_rec
	push	ds
	pop	es
	ELSE
	les	bx,tsk_global
	ENDIF
	les	bx,es:current_task[bx]
	IF	TSK_NAMED
	lea	dx,tname.nname[bx]
	callp	tsk_rprintf,<<cs,#pmd2>,<es,bx>,<es,dx>>
	ELSE
	callp	tsk_rprintf,<<cs,#pmd2>,<es,bx>>
	ENDIF
	callp	tsk_rprintf,<<cs,#pmde>>
	ret
;
@fatal_dump	endp
;
	ENDIF
;
;---------------------------------------------------------------
;
	.tsk_ecode
;
	ENDIF
;
        end

