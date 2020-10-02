        TITLE   DOSREAD  of GDS
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
tmp     dw      ?
        dw      ?
_DATA   ends

_TEXT   segment byte public 'CODE'
        assume  cs:_TEXT,ds:DGROUP
        public  _dosread,_dosopen,_dosseek,_dosclose

_dosopen proc   near    ; public to c
        push    bp
        mov     bp,sp
        mov     dx,[bp+smo]
        mov     ax,3d00h
        int     21h
        jnc     dso010
        mov     ax,0ffffh
dso010: pop     bp
        ret
_dosopen endp

_dosseek proc near      ; public to c
        push    bp
        mov     bp,sp
        mov     bx,[bp+smo]
        mov     ax,4200h
        mov     dx,[bp+smo+2]
        xor     cx,cx
        int     21h
        jnc     dss010
        mov     bx,0ffffh
dss010: mov     ax,bx
        pop     bp
        ret
_dosseek endp

_dosread proc near      ; public to c
        assume  es:DGROUP
        push    bp
        mov     bp,sp
        push    si
        push    di
        push    ds
        push    ds
        pop     es
        mov     es:tmp,0
        mov     es:tmp+2,0
        mov     bx,[bp+smo]
dsr005: mov     ax,3f00h
        lds     dx,[bp+smo+2]
        mov     cx,[bp+smo+8]
        test    cx,cx
        jnz     dsr010
        mov     cx,[bp+smo+6]
        cmp     cx,8000h
        jb      dsr020
dsr010: sub     word ptr [bp+smo+6],08000h
        sbb     word ptr [bp+smo+8],0
        mov     cx,8000h
        add     [bp+smo+4],0800h
dsr020: int     21h
        jc      dsrerr
        add     es:tmp,ax
        adc     es:tmp+2,0
        cmp     ax,08000h
        je      dsr005
        mov     ax,es:tmp
        mov     dx,es:tmp+2
        jmp     dsr030
dsrerr: mov     ax,0ffffh
        mov     dx,ax
dsr030: pop     ds
        pop     di
        pop     si
        pop     bp
        ret
        assume  es:nothing
_dosread endp

_dosclose proc near
        push    bp
        mov     bp,sp
        mov     bx,[bp+smo]
        mov     ax,3e00h
        int     21h
        jnc     dsc010
        mov     bx,0ffffh
dsc010: mov     ax,bx
        pop     bp
        ret
_dosclose endp       

_TEXT   ends
        end

