;
;	--- Version 2.2 90-10-12 10:46 ---
;
;	CTask - Code sharing function stubs
;
;	Public Domain Software written by
;		Thomas Wagner
;		Ferrari electronic Gmbh
;		Beusselstrasse 27
;		D-1000 Berlin 21
;		Germany
;
;	This file is new with version 2.1.
;	It is used to generate the function entry stubs to allow
;	applications to share the code of the first, resident,
;	CTask invocation.
;
;	Two object files are generated from this source:
;
;	If GEN_JTAB is defined on assembly, an entry address table is 
;	generated that is to be linked with the primary invocation.
;
;	If no Define is given on assembly, the function stubs for the 
;	application are generated.
;
;	There are two macros, 'stub' and 'dstub'. The 'stub' macro
;	creates an actual entry/external, whereas the 'dstub' macro
;	only creates a dummy table entry, and no public label.
;
;	You should define all entries you are using anyway in your
;	primary invocation with 'stub'.
;	Functions that are not used in either primary or secondary
;	may be defined with 'dstub'.
;	YOU SHOULD NOT delete entries or change the order of entries.
;
;	It is possible to have a mix of code sharing and locally loaded
;	routines in an application. The routines in tskmain.c and tsksub.c
;	should, however, always be enabled with 'stub', since they are always
;	included in the primary, and could cause problems when directly
;	linked.
;
;	CAUTION: The application may use small model, i.e. near entries
;		 into the stub table. The NEAR_CODE define may be
;		 set to 1 when compiling this module for the application.
;
;		 The primary kernel MUST use far routines for code
;		 sharing to work, and the entry points must load DS on
;		 entry.
;
	include	tsk.mac
;
	.tsk_model
;
;
	IFDEF	GEN_JTAB
	public	tsk_jmptab
	ENDIF
;
nstubs	=	0
;
stub	macro	extn,ccc
	IFNB	<ccc>
	IFDEF	GEN_JTAB
	CGlbext	extn
	ELSE
	CPubfnc	extn
	ENDIF
	ELSE
	IFDEF	GEN_JTAB
	Globext	extn
	ELSE
	Pubfunc	extn
	ENDIF
	ENDIF
	IF1
estr	catstr	<stb>,%nstubs
% estr	equ	<extn>
	ENDIF
nstubs	=	nstubs+1
	endm
;
dstub	macro	extn
nstubs	=	nstubs+1
	endm
;
;------------------------------------------------------------------
;
;	The routines from the module
;		tskres
;	must be linked directly, since they establish the
;	stub linkage.
;
;	The routines from the modules
;		tskmain
;		tskgrp
;	must not be defined as function stubs. Those routines
;	need local routine/variable info, and thus must be linked
;	with the secondary invocation. For secondary invocations
;	not creating groups, the routines defined there are not needed
;	anyway.
;
;	The local routines defined in
;		tsksub
;		tskque
;	are normally defined as 'near'. Including those routines
;	into the stub list would require them to be defined as
;	far _loadds (Huge for TC). This would slow down the most basic
;	operations, and is not recommended.
;
;------------------------------------------------------------------
;
;	The following entries should always be 'stub', not 'dstub',
;	since they are in the primary anyway.
;
;	module tskinst
;
	stub	tsk_install_main
	stub	tsk_remove_tasker
;
;	module tskutil
;
	stub	preempt_on
	stub	preempt_off
	stub	tsk_ena_preempt
	stub	tsk_dis_preempt
	stub	t_delay
;
;	module tsktask
;
	stub	create_task
	stub	kill_task
	stub	start_task
	stub	wake_task
;
;	module tskasm
;
	stub	schedule
	stub	yield
	stub	tsk_dis_int
	stub	tsk_ena_int
	stub	tsk_nop
	stub	tsk_inpw
	stub	tsk_flags
	stub	tsk_dseg
;
;-------------------------------------------------------------------
;
;	The following functions may be either 'stub' or 'dstub',
;	but are normally used by kernel routines anyway, so you can
;	keep them as 'stub'.
;
;	module tskrsc
;	Resources are used in the DOS module
;
	stub	create_resource
	stub	delete_resource
	stub	release_resource
	stub	request_resource
	stub	request_cresource
	stub	c_request_resource
	stub	c_request_cresource
	stub	check_resource
;
;	module tskflg
;	Flags are in the DOS module, too
;
	stub	create_flag
	stub	delete_flag
	stub	set_flag
	stub	clear_flag
	stub	clear_flag_wait_set
	stub	wait_flag_set
	stub	wait_flag_clear
	stub	check_flag
;
;	module tskcnt
;	Counters are used for the timer tasks
;
	stub	create_counter
	stub	delete_counter
	stub	clear_counter
	stub	wait_counter_set
	stub	wait_counter_clear
	stub	inc_counter
	stub	set_counter
	stub	check_counter
;
;-------------------------------------------------------------------
;
;	The following functions may be either 'stub' or 'dstub'.
;	They are not referenced by kernel routines, but if you have
;	them in the primary, you can keep them as 'stub'.
;
;	module tskname
;
	dstub	find_group_name
	dstub	find_name
;
;	module tsksec
;
	dstub	t_read_key
	dstub	t_wait_key
	dstub	t_keyhit
;
;	module tsktutl
;
	dstub	get_priority
	dstub	set_priority
	dstub	set_funcs
	dstub	set_user_ptr
	dstub	get_user_ptr
	dstub	curr_task
	dstub	stop_task
;
;	module tsktops
;
	dstub	create_timer,c
	dstub	delete_timer
	dstub	change_timer,c
;
;	module tsktsub
;
	dstub	delete_watch
;
;	module tskmemw
;
	dstub	create_memory_watch,c
	dstub	wait_memory
;
;	module tskporw
;
	dstub	create_port_watch,c
	dstub	wait_port
;
;	module tskhot
;
	dstub	create_hotkey_entry,c
	dstub	wait_hotkey
;
;	module	tskmail
;
	dstub	create_mailbox
	dstub	delete_mailbox
	dstub	send_mail
	dstub	wait_mail
	dstub	c_wait_mail
	dstub	check_mailbox
;
;	module	tskpip
;
	dstub	create_pipe
	dstub	delete_pipe
	dstub	read_pipe
	dstub	c_read_pipe
	dstub	write_pipe
	dstub	c_write_pipe
	dstub	wait_pipe_empty
	dstub	check_pipe
	dstub	pipe_free
	dstub	flush_pipe
;
;	module	tskwpip
;
	dstub	create_wpipe
	dstub	delete_wpipe
	dstub	read_wpipe
	dstub	c_read_wpipe
	dstub	write_wpipe
	dstub	c_write_wpipe
	dstub	wait_wpipe_empty
	dstub	check_wpipe
	dstub	wpipe_free
	dstub	flush_wpipe
;
;	module	tskbuf
;
	dstub	create_buffer
	dstub	delete_buffer
	dstub	read_buffer
	dstub	c_read_buffer
	dstub	write_buffer
	dstub	c_write_buffer
	dstub	check_buffer
;
;	module	tsktick
;
	dstub	create_ticker
	dstub	delete_ticker
	dstub	set_ticker
	dstub	get_ticker
;
;-------------------------------------------------------------------
;
	IFNDEF	GEN_JTAB
	.tsk_data
	extrn	tsk_global: dword
;
	.tsk_edata
	ENDIF
	.tsk_code
;
nstb	=	0
;
	IFDEF	GEN_JTAB
;
;	The 'fault' entry just does an INT 3, which breaks to a debugger
;	if one is installed. If no debugger is running, you're out of luck.
;	But... you should never get here unless you have different 
;	stub/dstub combinations in primary and secondary.
;
@fault	proc	far
	int	3
	ret
@fault	endp
;
;
tsk_jmptab	label	dword
	ENDIF
;
	rept	nstubs
estr	catstr	<stb>,%nstb
	IFDEF	GEN_JTAB
%	IFDEF	estr
	dd	estr
	ELSE
	dd	@fault
	ENDIF
	ELSE
	IFDEF	estr
% estr:
	IF	NEAR_CODE
	pop	bx
	push	cs
	push	bx
	ENDIF
	IFDEF	LOAD_DS
	mov	bx,@CTASK_DATA
	mov	es,bx
	les	bx,es:tsk_global
	ELSE
	les	bx,tsk_global
	ENDIF
	les	bx,es:stub_table[bx]
	jmp	dword ptr es:[bx+(nstb*4)]
	ENDIF
	ENDIF
nstb	=	nstb+1
	endm
;
	.tsk_ecode
	end

