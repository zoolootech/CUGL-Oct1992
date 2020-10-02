        TITLE   LINE of GDS
        page    60,132
        .SFCOND
;
; *==========================================================*
; * This file contain level 1 line drawing                   *
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
        extrn   _STYLE:word
_DATA   ends

_TEXT   segment byte public 'CODE'
        assume  cs:_TEXT,ds:DGROUP
        public  _lline1,_lline3,_lline2,_lline4

        extrn  $calc:near, downln:near, $dot:near
        extrn  _horzl:near, _vert1l:near

_lline1 proc    near    ;public to c
        push    bp
        mov     bp,sp
        push    si
        push    di
        mov     si,[bp+smo+6]
        mov     dx,_STYLE
        xchg    dh,dl
        mov     cx,[bp+smo]
        and     cl,0fh
        jz      short   ll1d
        rol     dx,cl
        jmp     short ll1d
ll1a:   inc     word ptr [bp+smo]
        mov     ax,[bp+smo+8]
        test    ax,ax
        jns     short ll1b
        add     ax,[bp+smo+10]
        jmp     short ll1c
ll1b:   add     ax,[bp+smo+12]
        mov     bx,[bp+smo+4]
        add     [bp+smo+2],bx
ll1c:   mov     [bp+smo+8],ax
ll1d:   rol     dx,1
        jnc     ll1e
        mov     ax,[bp+smo]
        mov     bx,[bp+smo+2]
        push    dx
        call    $dot
        pop     dx
ll1e:   dec     si
        jns     ll1a
        pop     di
        pop     si
        pop     bp
        ret
_lline1 endp

_lline2 proc    near    ;public to c
        push    bp
        mov     bp,sp
ll2a:   mov     bx,1
        mov     ax,[bp+smo+8]
ll2b:   test    ax,ax
        jns     short ll2c
        add     ax,[bp+smo+10]
        inc     bx
        dec     word ptr [bp+smo+6]
        jnz     short ll2b
        jmp     short ll2d
ll2c:   add     ax,[bp+smo+12]
        mov     [bp+smo+8],ax
        push    bx
        mov     ax,[bp+smo+2]
        push    ax
        add     ax,[bp+smo+4]
        mov     [bp+smo+2],ax
        push    word ptr [bp+smo]
        add     [bp+smo],bx
        call    _horzl
        mov     sp,bp
        dec     word ptr [bp+smo+6]
        jnz     short ll2a
        mov     bx,1
ll2d:   push    bx
        push    word ptr [bp+smo+2]
        push    word ptr [bp+smo]
        call    _horzl
        mov     sp,bp
        pop     bp
        ret
_lline2 endp

_lline3 proc    near    ;public to c
        push    bp
        mov     bp,sp
        push    si
        push    di
        mov     si,[bp+smo+6]
        mov     dx,_STYLE
        xchg    dh,dl
        mov     cx,[bp+smo+2]
        and     cl,0fh
        jz      ll3d
        rol     dx,cl
        jmp     short ll3d
ll3a:   inc     word ptr [bp+smo+2]
        mov     ax,[bp+smo+8]
        test    ax,ax
        jns     short ll3b
        add     ax,[bp+smo+10]
        jmp     short ll3c
ll3b:   add     ax,[bp+smo+12]
        mov     bx,[bp+smo+4]
        add     [bp+smo],bx
ll3c:   mov     [bp+smo+8],ax
ll3d:   rol     dx,1
        jnc     ll3e
        mov     ax,[bp+smo]
        mov     bx,[bp+smo+2]
        push    dx
        call    $dot
        pop     dx
ll3e:   dec     si
        jns     ll3a
        pop     di
        pop     si
        pop     bp
        ret
_lline3 endp

_lline4 proc    near    ;public to c
        push    bp
        mov     bp,sp
ll4a:   mov     bx,1
        mov     ax,[bp+smo+8]
ll4b:   test    ax,ax
        jns     short ll4c
        add     ax,[bp+smo+10]
        inc     bx
        dec     word ptr [bp+smo+6]
        jnz     short ll4b
        jmp     short ll4d
ll4c:   add     ax,[bp+smo+12]
        mov     [bp+smo+8],ax
        push    bx
        push    word ptr [bp+smo+2]
        add     [bp+smo+2],bx
        mov     ax,[bp+smo]
        push    ax
        add     ax,[bp+smo+4]
        mov     [bp+smo],ax
        call    _vert1l
        mov     sp,bp
        dec     word ptr [bp+smo+6]
        jnz     short ll4a
        mov     bx,1
ll4d:   push    bx
        push    word ptr [bp+smo+2]
        push    word ptr [bp+smo]
        call    _vert1l
        mov     sp,bp
        pop     bp
        ret
_lline4 endp

_TEXT   ends
        end
