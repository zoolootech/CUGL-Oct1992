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
        extrn   _STYLE:word
        extrn   _DOTVALUE:word,_LEFTWORD:word,_RIGHTWORD:word
        extrn   _LASTX:word, _LASTY:word

        extrn   wrtvec:word

tlen    dw      ?       ; temporary length
vlpw    dw      ?       ; vertical line plot word
vlsty   dw      ?       ; vertical line style
wid     dw      ?       ; width
voa     dw      ?       ; vertical line offset address
vsa     dw      ?       ; vertical line segment address
_DATA   ends

_TEXT   segment byte public 'CODE'
        assume  cs:_TEXT,ds:DGROUP
        public  _plot, _horzl,_SetStyle,_vertl
        public  $dot,_vert1l

        extrn  $calc:near, downln:near

; routine to plot a single dot
; input : x,y-coordinates in ax and bx
$dot    proc    near    ; public routine
        push    di
        mov     di,ax
        and     di,0fh
        call    $calc           ; calculate the word address
        mov     es,ax
        shl     di,1
        mov     ax,_DOTVALUE[di] ; get dot position
        pop     di
        jmp     wrtvec          ; go and write it
$dot    endp

; A C callable routine
;       ret=plot(x,y);
;       int ret,x,y;
;
;       x and y are the x and y coordintaes
;       return value is the plot word, usually not useful
_plot   proc    near    ;public routine
        push    bp
        mov     bp,sp
        mov     ax,[bp+smo]     ; get x coordinate
        mov     bx,[bp+smo+2]   ; get y coordinate
        pop     bp
        jmp    short $dot
_plot   endp

; A C callable routine
;       horzl(x,y,length);
;       int     x,y,length
;
;       no checking for correct x,y coordinate and length
;               and no clipping too.
_horzl  proc    near    ;public routine
        push    bp
        mov     bp,sp
        push    si
        push    di
        mov     ax,[bp+smo]     ; get x coordinate
        mov     bx,[bp+smo+2]   ; get y coordinate
        mov     di,ax
        call    $calc           ; calculate word address
        mov     es,ax           ; segment number in es
        and     di,0fh          ; get dot position within word
        mov     si,di
        add     di,[bp+smo+4]
        shl     si,1
        mov     ax,_LEFTWORD[si]
        cmp     di,16           ; di > 16 if the line is not within the
        ja      hl1             ; same word
        shl     di,1            ; othwise the line is in the same word
        xor     ax,_LEFTWORD[di]
        pop     di
        pop     si
        pop     bp
        and     ax,_STYLE       ; write with the setting in _STYLE
        jmp     wrtvec          ; write the line
; line span across more than 1 word
hl1:    and     ax,_STYLE       ; write the first portion
        call    wrtvec
        mov     cl,4            ; get the number of full words
        mov     si,di           ; between first and last word
        shr     si,cl
        dec     si
        jz      short hl2
        mov     cx,si
        mov     ax,_STYLE
hl3:    inc     bx              ; repeat writing with the setting in _STYLE
        inc     bx
        call    wrtvec
        loop    short hl3
hl2:    inc     bx              ; point to next word
        inc     bx
        and     di,0fh
        shl     di,1
        mov     ax,_RIGHTWORD[di]       ; set up last word
        pop     di
        pop     si
        pop     bp
        and     ax,_STYLE       ; use STYLE
        jmp     word ptr wrtvec
_horzl  endp

; *-------------------------------------------------------------*
; * This routine is quite long and complicated because it trys  *
; * to optimize the speed in drawing a wide vertical line.      *
; * The program is not difficult to understand if you know      *
; * what it is trying to do. Understand horzl above is helpful  *
; *-------------------------------------------------------------*
; A C callable routine
;       vertl(x,y,length,width);
;       int     x,y,length
;
;       no checking for correct x,y coordinate and length
;               and no clipping too.

_vertl  proc    near    ;public routine
        push    bp
        mov     bp,sp
        push    si
        push    di
        mov     ax,[bp+smo]     ; get x coordinate
        mov     bx,[bp+smo+2]   ; get y coordinate
        call    $calc           ; calculate starting address
        mov     voa,bx          ; save it in vsa and voa
        mov     vsa,ax
        mov     es,ax
        mov     cx,[bp+smo+2]   ; get y coordinate
        mov     dx,_STYLE
        xchg    dh,dl
        and     cl,0fh          ; adjust _STYLE for different y coordinate
        mov     ch,cl
        jz      short vl2
        rol     dx,cl
vl2:    mov     vlsty,dx
        mov     ax,[bp+smo]
        and     ax,0fh
        mov     di,ax
        shl     di,1
        mov     dx,_LEFTWORD[di]
        add     ax,[bp+smo+6]   ; get width
        cmp     ax,16           ; <=16 if the width of the line is in 1 word
        ja      short vl1
        mov     di,ax           ; within 1 word
        shl     di,1
        xor     dx,_LEFTWORD[di]        ; set up the word that should
        mov     vlpw,dx                 ; be in drawing
        mov     cx,[bp+smo+4]   ; get length
        mov     tlen,cx
        mov     cx,[bp+smo+2]
        mov     dx,vlsty
        call    $vertl          ; draw the line
        pop     di
        pop     si
        pop     bp
        ret
;
; not within 1 word
vl1:    mov     di,ax
        and     di,0fh
        shl     di,1
        mov     cx,_RIGHTWORD[di]
        push    cx
        mov     cl,4
        shr     ax,cl
        mov     wid,ax
        mov     vlpw,dx         ; set up the plot word
        mov     cx,[bp+smo+4]
        mov     tlen,cx
        mov     cx,[bp+smo+2]
        mov     dx,vlsty
        call    $vertl          ; draw first portion
        mov     vlpw,0ffffh     ; repeat drawing with whole word
        dec     wid             ; (no style for width)
        jz      short vl3
vl4:    add     voa,2           ; move to next horizontal word
        mov     bx,voa          ; restore starting address
        mov     es,vsa
        mov     cx,[bp+smo+4]   ; get length
        mov     tlen,cx
        mov     cx,[bp+smo+2]   ; get y coordinate
        mov     dx,vlsty        ; set style
        call    $vertl          ; draw it
        dec     wid
        jnz     short vl4
vl3:    add     voa,2           ; move to last word
        mov     bx,voa
        mov     es,vsa
        pop     cx
        test    cx,cx
        jz      vl5
        mov     vlpw,cx
        mov     cx,[bp+smo+4]
        mov     tlen,cx
        mov     cx,[bp+smo+2]
        mov     dx,vlsty
        call    $vertl    ; draw last word
vl5:    pop     di
        pop     si
        pop     bp
        ret
_vertl  endp

; A C callable routine but it is not public now
;       vert1l(x,y,length);
;       int x,y,length;
; 
; the width is always 1
_vert1l proc    near
        push    bp
        mov     bp,sp
        push    si
        push    di
        mov     ax,[bp+smo]     ; get x coordinate
        mov     bx,[bp+smo+2]   ; get y coordinate
        call    $calc
        mov     es,ax
        push    bx
        mov     bx,[bp+smo]
        and     bx,0fh
        shl     bx,1
        mov     cx,_DOTVALUE[bx] ; get the plot word
        mov     vlpw,cx
        pop     bx
        mov     cx,[bp+smo+4]   ; get length
        mov     tlen,cx
        mov     cx,[bp+smo+2]
        mov     dx,_STYLE
        xchg    dh,dl           ; adjust style for different starting y
        and     cl,0fh
        jz      short v1l1
        rol     dx,cl
v1l1:   mov     cx,[bp+smo+2]
        call    $vertl    ; draw 1 line
        pop     di
        pop     si
        pop     bp
        ret
_vert1l endp

; input : es:bx starting address
;         dx    style
;         vlpw  plot word (word to be stored)
;         tlen  length of the line
; this routine affects si
$vertl  proc    near
        mov     _LASTY,cx
        mov     ax,vlpw
        mov     cx,tlen
v$l1:   rol     dx,1
        jnc     short v$l2
        call    wrtvec
v$l2:   call    downln
        loop    short v$l1
        ret
$vertl  endp

; A C callable routine
;       SetStyle(style);
;
_SetStyle proc   near    ;public routine
        mov     bx,sp
        mov     ax,ss:[bx+smo-2]
        test    ax,ax
        jnz     short   ss1     ; if style is 0
        mov     ax,_STYLE
        xchg    ah,al
        not     word ptr _STYLE ; reverse the old style
        ret
ss1:    xchg    ah,al
        xchg    _STYLE,ax
        xchg    ah,al
        ret
_SetStyle endp

_TEXT   ends
        end

