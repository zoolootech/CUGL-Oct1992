        TITLE   PRT    of GDS
        page    60,132
        .SFCOND
;
; *=============================================================*
; *                                                             *
; *    This file contain low level functions for printing       *
; *    functions include:                                       *
; *     prt     convert a 8 integer array for printing          *
; *     pbflush flush every thing in buffer to printer          *
; *     rmv0    remove trailing zeroes                          *
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
        extrn   _PBPTR:word, _PBCOUNT:word, _CFBUF:byte

_DATA   ends

_TEXT   segment byte public 'CODE'
        assume  cs:_TEXT,ds:DGROUP
        public  _prtc, _pbflush
        public  _rmv0

        extrn   $calc:near

;
;  prtc(ch);
;  char ch;
;
;  append the character ch to the buffer
;
_prtc   proc    near    ;public to c
        push    bp
        mov     bp,sp
        mov     bx,_PBPTR       ; load pointer
        mov     dx,[bp+smo]     ; load data
        mov     [bx],dl         ; append to the buffer
        inc     word ptr _PBPTR ; update pointer
        inc     _PBCOUNT        ; update length count
        pop     bp
        ret
_prtc  endp

;
;  pbflush();
;
;  flush buffer to the printer
_pbflush proc    near    ;public to c
        push    bp
        push    ds      ; avoid ds being affected
        mov     cx,_PBCOUNT
        mov     bx,offset DGROUP:_CFBUF
        mov     word ptr _PBPTR,bx
        mov     _PBCOUNT,0
        test    cx,cx
        jz      pbf2
        mov     dx,0
pbf1:   mov     al,[bx]
        xor     ah,ah
        int     17h     ; use BIOS call
        and     ah,29h
        jnz     pbf2
        inc     bx
        loop    pbf1    ; loop until all printed
pbf2:   pop     ds
        pop     bp
        ret
_pbflush endp

; rmv0(header_length);
; int header_length;
;
; remove trailing zero and return new length
; if new length is less than or equal to header length, header will be removed
_rmv0   proc    near    ;public to c
        push    bp
        mov     bp,sp
        mov     cx,_PBCOUNT
        mov     dx,[bp+smo]     ; load header length
        mov     bx,word ptr _PBPTR      ; load current pointer
op1:    dec     bx              ; looks back
        mov     al,[bx]         ; 
        test    al,al           ; test for zero
        jnz     short op2       ; jump if not zero
        dec     cx              ; else continue
        cmp     cx,dx           ; end of search if current length < header len
        ja      op1
        mov     dx,offset DGROUP:_CFBUF
        mov     word ptr _PBPTR,dx      ; reset pointer
        mov     ax,0            ; clear count
        mov     _PBCOUNT,ax
        pop     bp
        ret                     ; return 0
op2:    inc     bx              ; 
        mov     word ptr _PBPTR,bx      ; update pointer and count
        mov     _PBCOUNT,cx
        sub     cx,dx
        mov     ax,cx
        pop     bp
        ret                     ; return new length
_rmv0   endp

_TEXT   ends
        end

