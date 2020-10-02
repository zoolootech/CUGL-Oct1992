        TITLE   MISCELLANEOUS of GDS
        page    60,132
        .SFCOND
;
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
_DATA   ends

_TEXT   segment byte public 'CODE'
        assume  cs:_TEXT,ds:DGROUP
        public  _nor_ptr, _exchange

;       ret=nor_ptr(ptr);
;       int huge *ret,*ptr
;
;       return pointer with offset < 10h
_nor_ptr proc    near    ;public to c
        push    bp
        mov     bp,sp
        mov     ax,[bp+smo]
        mov     dx,ax
        mov     cl,4
        shr     dx,cl
        add     dx,[bp+smo+2]
        and     ax,0fh
        pop     bp
        ret
_nor_ptr endp

_exchange proc   near    ;public to c
        mov     bx,sp
        mov     ax,ss:[bx+smo-2]
        xchg    ah,al
        ret
_exchange endp

_TEXT   ends
        end

