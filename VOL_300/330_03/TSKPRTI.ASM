;
;	--- Version 2.2 90-10-12 10:45 ---
;
;	CTask - Printer interrupt handler (IBM specific)
;
;	Public Domain Software written by
;		Thomas Wagner
;		Ferrari electronic Gmbh
;		Beusselstrasse 27
;		D-1000 Berlin 21
;		Germany
;
;	This file is new with version 1.2.
;
;	To avoid stack overflows while processing printer interrupts in
;	the tskprt module, all printer interrupts are routed through the
;	"envelope" routines defined here.
;
	name	tskprti
;
	include	tsk.mac
;
	.tsk_model
;
	public	tskprt_int0
	public	tskprt_int1
	public	tskprt_int2
;
        Locext	tsk_switch_stack
	Locext	tsk_prt_int
;
prtint	macro	num
;
tskprt_int&num	proc	far
        call    tsk_switch_stack
	callp	tsk_prt_int,<num>
        cli
	mov	al,20h
	out	20h,al
	iret
;
tskprt_int&num	endp
;
	endm
;
;
	.tsk_code
;
	prtint	0
	prtint	1
	prtint	2
;
	.tsk_ecode
	end


