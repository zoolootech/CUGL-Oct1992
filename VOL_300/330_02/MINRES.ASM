	.model	large,c
	dosseg
;
	include	tsk.mac
;
;	This is a sample for a minimal program that makes the
;	CTask kernel resident.
;
	public	_acrtused
	public	_psp
	public	alloc_resource
;
	Pubfunc	tsk_alloc
	Pubfunc	tsk_free
;
	Globext	install_tasker
	Globext	preempt_on
	Globext	set_priority
	Globext	schedule
;
_acrtused	equ	9876h
;
NULL	segment para public 'BEGDATA'
NULL	ends
;
stacklen	=	256
;
	.stack	stacklen
;
	.data?
;
alloc_resource	resource <>
;
_psp	dw	?
;
	.data
;
main_name	db	"CTRES",0
;
	.code
;
;	Dummy allocation routines. The memory allocation is never called
;	by CTask if you don't explicitly request it.
;
tsk_free:
tsk_alloc:
	xor	ax,ax
	mov	dx,ax
	ret
;
;	The main program. 
;	NOTE: This sample program does *not* clear uninitialized memory
;	      to zero. CTask has explicit initialization code for all
;	      of it's variables.
;
main:
	mov	ax,@data
	mov	ds,ax
	mov	_psp,es
	cld
	mov	ax,es:[2ch]	; Environment pointer in PSP
	mov	es,ax
	mov	ah,49h		; Release environment
	int	21h
;
;	install_tasker (0, 0, IFL_DISK | IFL_PRINTER | IFL_INT15);
;
ifls	=	IFL_DISK OR IFL_PRINTER OR IFL_INT15
	callp	install_tasker,<0,0,ifls,<ds,#main_name>>
;
;	set_priority (NULL, PRI_STD);
;
	callp	set_priority,<<0,0>,PRI_STD>
;
	call	preempt_on
	call	schedule
;
	mov	dx,offset STACK		; stack end
	add	dx,15			; round up
	mov	cl,4
	shr	dx,cl			; convert to paragraphs
	add	dx,@data		; add data start segment
	sub	dx,_psp			; minus program segment
	mov	ax,3100h		; TSR
	int	21h
;
	end	main

