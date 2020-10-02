        PAGE   60,132
        TITLE  Grab Control-Break, Control-C interrupts

;  001  12-Oct-86  grabbrk.asm

;       Copyright (c) 1986 by Blue Sky Software.  All rights reserved.


_TEXT   SEGMENT  BYTE PUBLIC 'CODE'
_TEXT   ENDS
CONST   SEGMENT  WORD PUBLIC 'CONST'
CONST   ENDS
_BSS    SEGMENT  WORD PUBLIC 'BSS'
_BSS    ENDS
_DATA   SEGMENT  WORD PUBLIC 'DATA'
_DATA   ENDS

DGROUP  GROUP   CONST,  _BSS,   _DATA
        ASSUME  CS: _TEXT, DS: DGROUP, SS: DGROUP, ES: DGROUP

_DATA   SEGMENT
        brkflgp dw    0           ; address of int set NZ when ^Break/^C
_DATA   ENDS

_TEXT   SEGMENT

;
;  grabbrk(brkflgp)
;
;     grabbrk gets called once.  It takes over the DOS Control-Break,
;     Control-C interrupt vector.  Whenever a ^Break/^C interrupt is
;     signaled, grabbrk sets the int pointed to by brkflagp to a NZ value.

        PUBLIC _grabbrk

_grabbrk PROC   NEAR
        push    bp
        mov     bp,sp
        push    ds

        mov     ax,[bp+4]              ; save address of brkflag
        mov     brkflgp,ax

        mov     ax,2523h               ; set interrupt vector 23 to our routine
        push    cs
        pop     ds
        mov     dx,offset intrtn
        int     21h

        pop     ds
        mov     sp,bp                  ; back to C pgm
        pop     bp
        ret

_grabbrk ENDP


;  intrtn  - interrupt routine which gets control by ^Break/^C

intrtn  PROC    FAR

        push    bx                     ; must save all changed regs

        mov     bx,brkflgp             ; get address of brkflag
        mov     word ptr [bx],1        ; set brkflag to NZ

        pop     bx                     ; back to DOS (?)
        iret

intrtn  ENDP

_TEXT   ENDS
        END
