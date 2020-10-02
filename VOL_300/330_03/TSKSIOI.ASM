;
;	--- Version 2.2 90-10-12 10:46 ---
;
;	CTask - SIO interrupt handler (IBM specific)
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
;	To avoid stack overflows while processing serial interrupts in
;	the tsksio module, all SIO interrupts are routed through the
;	"envelope" routines defined here.
;
	name	tsksioi
;
	include	tsk.mac
;
	.tsk_model
;
	public	tsksio_int2
	public	tsksio_int3
	public	tsksio_int4
	public	tsksio_int5
	public	tsksio_int7
	public	tsksio_int10
	public	tsksio_int11
	public	tsksio_int12
	public	tsksio_int15
;
        Locext	tsk_switch_stack
	Locext	tsk_sio_int
;
sioint	macro	irq
;
tsksio_int&irq	proc	far
        call    tsk_switch_stack
	callp	tsk_sio_int,<irq>
        cli
	mov	al,20h
	IF	irq GE 8
	out	0a0h,al
	ENDIF
	out	20h,al
	iret
;
tsksio_int&irq	endp
;
	endm
;
;
	.tsk_code
;
	sioint	2
	sioint	3
	sioint	4
	sioint	5
	sioint	7
	sioint	10
	sioint	11
	sioint	12
	sioint	15
;
	.tsk_ecode
	end

