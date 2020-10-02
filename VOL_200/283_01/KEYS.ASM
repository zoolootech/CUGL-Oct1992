;
;  ROM-BIOS keyboard services for small-model C programs
;
;  C functions provided:
;      extern unsigned read_keyboard( void (*wait_fn)( void ) );
;
;  4/21/88, d.c.oshel
;
	TITLE   keys.asm
	NAME    keys

	.8087
_TEXT	SEGMENT  WORD PUBLIC 'CODE'
_TEXT	ENDS
_DATA	SEGMENT  WORD PUBLIC 'DATA'
_DATA	ENDS
CONST	SEGMENT  WORD PUBLIC 'CONST'
CONST	ENDS
_BSS	SEGMENT  WORD PUBLIC 'BSS'
_BSS	ENDS
DGROUP	GROUP	CONST, _BSS, _DATA
	ASSUME  CS: _TEXT, DS: DGROUP, SS: DGROUP

_TEXT      SEGMENT
	ASSUME	CS: _TEXT
;
;=========================================================
; unsigned near read_keyboard( void (* near fn)( void ) )
;=========================================================
;
	PUBLIC	_read_keyboard

_read_keyboard	PROC NEAR
	push	bp			;old value, wotever it is
	mov	bp,sp			;get current stack frame
	jmp	short testit
waitfn:
	call	WORD PTR [bp+4]		;near function address on stack
testit:
	mov 	ah,1			;key ready?
	int	22     			;ROM-BIOS kbhit service
	jnz	waitfn			;no, wait for it
;
	mov	ah,0			;yes, get it
	int	22			;ROM-BIOS rdkey service
	cmp	al,0			;aux byte present in ax?
	jz	done			;yes, extended ascii, ax is fine
	sub	ah,ah			;normal, don't cbw, sign-extends ax
done:
	pop	bp			;restore old value
	ret	

_read_keyboard	ENDP


_TEXT	ENDS
END
