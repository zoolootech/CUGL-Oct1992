        TITLE   CHARDISP of GDS
        page    60,132
        .SFCOND
;
; *==========================================================*
; *                                                          *
; *   This file contains character display related functions *
; *     _wrtc -- move the pixel pattern from buffer to       *
; *              the current frame.                          *
; *     _copyfont -- copy the pixel data to buffer for       *
; *              manipulation.                               *
; *     _ctextL -- clip the left side of the pixel data      *
; *     _ctextR -- clip the right side of the pixel data     *
; *                                                          *
; *   See also the comment in writec function                *
; *                                                          *
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
        extrn   _LEFTWORD:word,_RIGHTWORD:word

        extrn   wrtvec:word

_DATA   ends

_TEXT   segment byte public 'CODE'
        assume  cs:_TEXT,ds:DGROUP
        extrn   $calc:near

        public  _wrtc,_copyfont,_ctextL,_ctextR
;
;  wrtc(x,y,shift,write_width,height,store_width,buffer,offset_flag);
;  x,y are the top-left-hand corner of the character box
;  shift -- the number of bits to shift right before writing
;  write_width -- the number of words per row in writing
;  store_width -- the number of words per row occupied in buffer
;  buffer -- address of the pixel data buffer
;  offset_flag -- 0 means normal, non-0 means skip first pixel word per row
;                 in the buffer. (see the comment below.
_wrtc   proc    near    ; public to c
        push    bp
        mov     bp,sp
        push    si
        push    di
        shl     word ptr [bp+smo+10],1  ; convert store_width to byte
        mov     ax,[bp+smo+6]   ; load write_width into ax
        shl     ax,1            ; convert to byte
        sub     [bp+smo+10],ax  ; store_width now contains the difference
                                ; of store_width and write_width in byte
        test    word ptr [bp+smo+4],08h ; shift >= 8 ?
        jz      wc10                    ; no, jump
        sub     word ptr [bp+smo+4],8   ; sub. shift by 8
        mov     bx,[bp+smo+12]  ; then shift the pixel data by 1 byte
        mov     di,[bp+smo+6]   ; write_width
        mov     si,[bp+smo+8]   ; height
wc11:   mov     cx,di
        mov     al,0
wc12:   mov     dx,[bx]
        xchg    dh,dl
        xchg    dh,al
        mov     [bx],dx
        inc     bx
        inc     bx
        loop    wc12            ; until the whole row is shifted
        add     bx,[bp+smo+10]  ; move to begining of next row
        dec     si              ; until every row is shifted
        jnz     wc11
wc10:   mov     ax,[bp+smo+4]
        test    ax,ax
        jz      short wc4
        mov     si,[bp+smo+8]   ; height
        mov     dx,[bp+smo+12]  ; buffer address
        mov     di,ax
wc1:    mov     bx,dx
wc2:    mov     cx,[bp+smo+6]
        clc
wc3:    rcr     word ptr [bx],1
        inc     bx
        inc     bx
        loop    wc3             ; until the whole row is shifted 1 bit
        dec     di
        jnz     short wc1       ; until the row is shifted n times
        add     bx,[bp+smo+10]
        mov     dx,bx
        mov     di,ax
        dec     si
        jnz     short wc2       ; until every row is shifted
wc4:    mov     si,[bp+smo+12]  ; pixel buffer address
wc5:    mov     ax,[bp+smo]     ; x coordinate
        mov     bx,[bp+smo+2]   ; y coordinate
        call    $calc           ; calculate destination addr.
        mov     es,ax
        mov     cx,[bp+smo+6]   ; load write_width (in words)
        test    [bp+smo+14],0ffffh ; flag set?
        jz      wc6             ; no, skip to wc6
        inc     si              ; otherwise skip first pixel word
        inc     si
        dec     cx              ; decrement write_width
                                ; in this case, write_width is at least 2
wc6:    mov     ax,[si]
        xchg    ah,al           ; xchg before writing
        call    wrtvec
        inc     si              ; move to next word
        inc     si
        inc     bx
        inc     bx
        loop    wc6             ; loop write_width times
        add     si,[bp+smo+10]  ; move to begining of next row
        inc     word ptr [bp+smo+2] ; move down one row (increment y)
        dec     word ptr [bp+smo+8] ; decrement height
        jnz     short wc5           ; if non-zero, loop
        pop     di              ; else done
        pop     si
        pop     bp
        ret
_wrtc   endp

;
;  copyfont(raw_data_width,height,store_width,raw_data_ptr,buffer);
;  raw_data_width -- number of byte per row
;  height -- in row
;  store_width -- number of words per row when stored in buffer
;  raw_data_ptr (huge) -- address of raw pixel data
;  buffer -- buffer address
;
_copyfont proc   near    ; public to c
        push    bp
        mov     bp,sp
        push    si
        push    di
        mov     ax,ds
        mov     es,ax
        mov     al,[bp+smo+2]   ; height
        mov     bl,[bp+smo+4]   ; store_width
        mul     bl              ; multiple to get memory required
        mov     cx,ax           ; move result to cx
        mov     di,[bp+smo+10]  ; buffer
        cld
        xor     ax,ax           ; clear buffer first
        rep stosw
        mov     si,[bp+smo]
        mov     dx,[bp+smo+4]
        shl     dx,1            ; convert to byte
        sub     dx,si           ; difference of store_width and raw_data_width
        mov     di,[bp+smo+10]
        les     bx,[bp+smo+6]   ; get raw data pointer
cf1:    mov     cx,si
cf2:    mov     ax,es:[bx]
        xchg    ah,al
        dec     cx
        jz      cf3             ; means raw_data_width is an odd number
        mov     [di],ax
        inc     bx
        inc     di
        inc     bx
        inc     di
        loop    short cf2
cf4:    add     di,dx
        dec     word ptr [bp+smo+2]
        jnz     short cf1       ; move row by row
        pop     di
        pop     si
        pop     bp
        ret
cf3:    xor     al,al           ; special handling of last byte if
        mov     [di],ax         ; raw_data_width is odd
        inc     bx
        inc     di
        jmp     short cf4
_copyfont endp
;
;  ctextR(clip_start,store_width,height,buffer);
;  clip_start -- It is in number of bits from the left side of a row.
;                ctextR discards the bits after clip_start.
;  store_width -- width in word of each row
;  height -- 
;  buffer -- address of the buffer
;
_ctextR proc    near    ; public to c
        push    bp
        mov     bp,sp
        push    si
        push    di
        mov     bx,[bp+smo]     ; get clip_start
        mov     si,bx
        and     bx,0fh          ; clip_width mod 16
        shl     bx,1
        mov     ax,_RIGHTWORD[bx]       ; get mask to clear the right part
        xchg    ah,al
        mov     bx,[bp+smo+4]
        mov     cl,4
        shr     si,cl           ; last word need
        mov     di,[bp+smo+6]   ; get buffer address
cr1:    add     di,si
        add     di,si           ; increase 2*si byte (si words)
        and     [di],ax         ; clear bits after clip_start
        inc     di              ; move to next word
        inc     di
        mov     cx,[bp+smo+2]
        sub     cx,si
        dec     cx              ; words left on the right of clip_start
        jz      cr2             ; jump if none
        mov     word ptr [di],0 ; else clear next word
        shl     cx,1
        add     di,cx
cr2:    dec     bx
        jnz     cr1             ; loop until every row is processed
        pop     di
        pop     si
        pop     bp
        ret
_ctextR endp
;
;  ctextL(clip_width,store_width,height,buf_siz,buffer);
;  clip_width -- bits to be discarded on the left
;  store_width -- in terms of words
;  height --
;  buf_size = height * store_width
;  buffer -- buffer address
_ctextL proc    near    ; public to c
        push    bp
        mov     bp,sp
        push    si
        push    di
        mov     di,[bp+smo+8]
        mov     bx,[bp+smo]
        mov     si,bx
        and     bx,0fh
        shl     bx,1
        mov     ax,_LEFTWORD[bx]        ; get mask to clear left part
        xchg    ah,al
        mov     cl,4
        shr     si,cl
        test    si,si           ; more than or equal to 16 bits
        jz      cl2
        mov     cx,[bp+smo+6]   ; requires block move
        sub     cx,si           ; set up loop count
        shl     si,1            ; bytes to move
        add     si,di
        mov     bx,ds
        mov     es,bx
        cld
        rep movsw               ; move all data left
cl2:    mov     di,[bp+smo+8]   ; 
        mov     cx,[bp+smo+4]
        mov     bx,[bp+smo+2]
        shl     bx,1
cl1:    and     [di],ax         ; clear the left part
        add     di,bx
        loop    cl1             ; loop until all row is finished
        pop     di
        pop     si
        pop     bp
        ret
_ctextL endp

_TEXT   ends
        end

