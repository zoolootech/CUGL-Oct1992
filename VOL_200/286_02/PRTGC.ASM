        TITLE   PRTG   of GDS
        page    60,132
        .SFCOND
;
; *=============================================================*
; *                                                             *
; *     prtc    append a character to the buffer                *
; *                                                             *
; *=============================================================*

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
        extrn   _NULLPRT:word
        extrn   _PBPTR:word, _PBCOUNT:word

tlen    dw      ?
_DATA   ends

_TEXT   segment byte public 'CODE'
        assume  cs:_TEXT,ds:DGROUP
        public  _prt
;
;  prt(intarray);
;  int intarray[];
;
_prt    proc    near    ;public to c
        push    bp
        mov     bp,sp
        mov     ax,16   ; each integer is 16 bit
        mov     tlen,ax
pt2:    mov     bx,[bp+smo]
        mov     cx,8    ; get one bit in each integer and convert to a byte
pt1:    shl     word ptr ss:[bx],1
IFDEF EPSON
        rcl     al,1
ELSE
  IFDEF OKI
        rcr     al,1
  ELSE
    .err NO PRINTER TYPE DEFINED
  ENDIF
ENDIF
        add     bx,2
        loop    pt1
        xor     ah,ah
        push    ax
        call    _prtgc  ; output one byte
        mov     sp,bp
        dec     word ptr [tlen] ; until all 16 are converted
        jnz     pt2
        pop     bp
        ret
_prt    endp

;
;  routine for appending 1 column of graphics data to the buffer
;  also insert NULLPRT of nulls to the buffer.
;  That is the difference between prtgc and prtc
_prtgc  proc    near  
        push    bp
        mov     bp,sp
        mov     bx,_PBPTR       ; load current pointer
        mov     dx,[bp+smo]
        mov     [bx],dl
        inc     bx              ; update length count
        inc     _PBCOUNT
IFDEF OKI
        cmp     dl,03h          ; if data = 03h (graphics command code)
        jnz     pgc3
        mov     [bx],dl         ; send it twice
        inc     bx
        inc     _PBCOUNT
pgc3:
ENDIF
        mov     cx,_NULLPRT
        test    cx,cx
        jz      pgc1
        mov     al,0            ; output null if NULLPRT is non-zero
pgc2:   mov     [bx],al
        inc     bx
        inc     _PBCOUNT
        loop    pgc2            ; loop NULLPRT times
pgc1:   mov     word ptr _PBPTR,bx      ; save new pointer
        pop     bp
        ret
_prtgc  endp

_TEXT   ends
        end

