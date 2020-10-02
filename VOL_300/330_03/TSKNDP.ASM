;
;	--- Version 2.2 90-10-12 10:38 ---
;
;	CTask - 80x87 Numeric Data Processor support
;
;	Public Domain Software written by
;		Thomas Wagner
;		Ferrari electronic Gmbh
;		Beusselstrasse 27
;		D-1000 Berlin 21
;		Germany
;
;	This file is new with version 2.1.
;
;	It contains support routines for the 80x87 NDP.
;	It will assemble to nothing if the NDP configuration flag
;	is FALSE.
;
	name	tskndp
;
	include	tsk.mac
;
	IF	NDP
	.tsk_model
;
	Pubfunc	tsk_check_ndp
;
	.tsk_code
;
;	tsk_check_ndp	Check for presence of NDP.
;
;	Returns 1 if NDP is present, 0 if not.
;
;	NOTE: Only the Equipment byte returned by INT 11 is checked. 
;	No real check for the presence of an 80x87 is done.
;	An actual check would be safer, but there seem to be a number
;	of machines where executing NDP-instructions without an NDP
;	installed will hang the machine. To avoid this, we rely on
;	the equip byte actually reflecting the true configuration.
;	Since most other programs using the NDP will do the same
;	(including Turbo C, and possibly MS C, too), there seems to
;	be not much gain in more thorough checks. If you disagree,
;	and can supply "safe" checks for an NDP, please send me
;	your modifications.
;
Localfunc tsk_check_ndp
;
	int	11h			; equipment
	and	ax,2
	jz	no_ndp
	mov	ax,1
	ret
;
no_ndp:
	xor	ax,ax
	ret
;
tsk_check_ndp	endp
;
	.tsk_ecode
	ENDIF
	end

