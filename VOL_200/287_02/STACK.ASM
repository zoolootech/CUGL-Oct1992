        TITLE   VHLN of GDS
        page    60,132
        .SFCOND
;
; *==========================================================*
; * This file contain level 1 horizontal line, vertical line *
; * drawing and dot plotting routines                        *
; *==========================================================*

IFDEF   COLOR
  IFDEF HERC
   .err both display type defined
  ENDIF
else
  IFNDEF HERC
    HERC equ 0
  ENDIF
ENDIF

smo     equ     4       ; small model offset value

DGROUP  group   _DATA
_DATA   segment word public 'DATA'
        assume  ds:DGROUP
        extrn   ___brklvl:word
_DATA   ends

_TEXT   segment byte public 'CODE'
        assume  cs:_TEXT,ds:DGROUP

        public  _stackavail,_alloca

_stackavail proc near
        pop     cx
        cmp     ___brklvl,sp
        jb      stka10
        xor     ax,ax
        jmp     cx
stka10: mov     ax,sp
        sub     ax,___brklvl
        jmp     cx
_stackavail endp

_alloca proc    near
        pop     cx
        pop     dx
        inc     dx
        and     dl,0feh
        mov     bx,sp
        sub     sp,dx
        mov     ax,sp
        push    [bx+2]
        push    [bx]
        push    dx
        jmp     cx
_alloca endp

_TEXT   ends
        end


