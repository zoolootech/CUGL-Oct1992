;
;	This file references most CTask kernel files.
;	It is used to check if any routines from the standard
;	run-time library are referenced.
;	The kernel is only model-independent if no externals are
;	unsatisfied when this file is linked with no default libraries.
;
	.model	large,c
	public	_acrtused
	public	free
	public	malloc
	public	calloc
	public	realloc
	public	_psp
;
_acrtused	=	9876h
;
	include	tsk.mac
;
	Globext	install_tasker	; tskmain.c (references tsktimer.c)
	Globext	create_pipe	; tskpip.c
	Globext	create_wpipe	; tskwpip.c
	Globext	create_counter	; tskcnt.c
	Globext	create_resource	; tskrsc.c
	Globext	create_flag	; tskflg.c
	Globext	create_buffer	; tskbuf.c
	Globext	create_mailbox	; tskmsg.c
	Globext	v24_install	; tsksio.c
	Globext	prt_install	; tskprt.c
;
	.stack	128
;
	.data?
_psp	dw	?
;
	.code
;
begin:
	mov	ax,4c01h
	int	21h			; in case it's accidentally called
;
free:
malloc:
calloc:
realloc:
	call	install_tasker
	call	create_pipe
	call	create_wpipe
	call	create_counter
	call	create_resource
	call	create_flag
	call	create_buffer
	call	create_mailbox
	call	v24_install
	call	prt_install
;
	end	begin

