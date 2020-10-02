        TITLE   CALCADDR of GDS
        page    60,132
        .SFCOND
;
; *==========================================================*
; *   This module contains most of the hardware dependent    *
; *   definition here. Currently only IBM color graphics     *
; *   card (640X400 mode) and Hercules graphics (720X348)    *
; *   are supported. Read MAKE.DOC for instruction on how    *
; *   to compile the program for one of them.                *
; *                                                          *
; *   If you want to use other kind of display card. You     *
; *   have to modify this file your self. More information   *
; *   on this can be found in CUSTOMIZ.DOC                   *
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

IFDEF   HERC
  IFDEF HERCHALF
absscrn segment at 0b000h ; for Hercules page 0 of 0..1
  ELSE
absscrn segment at 0b800h ; for Hercules page 1 of 0..1
  ENDIF
; commented out last line and uncommented next line to use page 0
; absscrn segment at 0b000h ; for Hercules page 0 of 0..1
ENDIF

IFDEF   COLOR
absscrn segment at 0b800h ; for Color Graphics Card
ENDIF
        public  _SCREEN
        org     0
_SCREEN label   word            ; define the address of video memory
absscrn ends

; *----------------------------------------------------------*
; * Address of 8x8 character font in IBM BIOS. This is also  *
; * true for most IBM compatibles.                           *
; *                                                          *
; * This is the font used for font number 0 in text display. *
; * if your computer don't have the font in the BIOS or      *
; * your character font is not 8X8, read CUSTOMIZ.DOC        *
; *----------------------------------------------------------*
absfont segment at 0ffa6h 
        public  _ROMFONT
        org     0eh
_ROMFONT label   byte   ; its address is F000:FA6E or FFA6:000E
absfont ends

DGROUP  group   _DATA
_DATA   segment word public 'DATA'
        assume  ds:DGROUP
        extrn   _CUR_FRAME:word
        extrn   _FTABLE:word,_FRM_ST:word
        extrn   _LNBYTE:word, _LADDRO:word, _LADDRS:word
        extrn   _LASTX:word, _LASTY:word

IFDEF JLASER
        extrn   jlaserseg:word
ENDIF

IFDEF HERC
gtable  db      35h,2dh,2eh,07h         ; 6845 parameter for
        db      5bh,02h,57h,57h         ; Hercules graphics mode
        db      02h,03h,00h,00h
ttable  db      61h,50h,52h,0fh         ; 6845 parameter for
        db      19h,06h,19h,19h         ; Hercules text mode
        db      02h,0dh,0bh,0ch
ENDIF
_DATA   ends

_TEXT   segment byte public 'CODE'
        assume  cs:_TEXT,ds:DGROUP
        public  _calcaddr, $calc, _settext, _setgraph, downln, _phyaddr

IFDEF HERC
;
; port address
; read Hercules user manual for more information
index   equ     03b4h
cntrl   equ     03b8h
config  equ     03bfh
; control codes
scrn_on equ     8
grph    equ     2
text    equ     20h

IFDEF HERCHALF
pagenu  equ     00h
ELSE
pagenu  equ     80h
ENDIF

_settext proc    near    ;public to c
        push    es
        push    si
        push    di
        mov     al,0
        mov     dx,config
        out     dx,al
        mov     al,text
        lea     si,ttable
        mov     bx,720h
        mov     cx,2000
        call    setmd
        pop     di
        pop     si
        pop     es
        ret
_settext endp
;
_setgraph proc   near    ;public to c
        push    es
        push    si
        push    di
        mov     al,03h
        mov     dx,config
        out     dx,al
        mov     al,grph+pagenu
        lea     si,gtable
        mov     bx,0
        mov     cx,4000h
        call    setmd
        pop     di
        pop     si
        pop     es
        ret
_setgraph endp
;
setmd   proc    near
        push    ds
        push    es
        push    ax
        push    bx
        push    cx
        mov     dx,cntrl
        out     dx,al
        mov     ax,ds
        mov     es,ax
        mov     dx,index
        mov     cx,12
        xor     ah,ah
parms:  mov     al,ah
        out     dx,al
        inc     dx
        lodsb
        out     dx,al
        inc     ah
        dec     dx
        loop    parms
        pop     cx
        mov     ax,absscrn
        cld
        mov     es,ax
        xor     di,di
        pop     ax
        rep     stosw
        mov     dx,cntrl
        pop     ax
        add     al,scrn_on
        out     dx,al
        pop     es
        pop     ds
        ret
setmd   endp
ENDIF

IFDEF COLOR
_settext proc   near    ; public to c
        push    si
        push    di
        mov     ax,02
        int     010h
        pop     di
        pop     si
        ret
_settext endp

_setgraph proc  near    ; public to c
        push    si
        push    di
        mov     ax,06
        int     010h
        pop     di
        pop     si
        ret
_setgraph endp
ENDIF

; *----------------------------------------------------------*
; * routine to calculate the word address of any dot in      *
; * any frame. However, it is optimize for the predefined    *
; * screen.                                                  *
; *----------------------------------------------------------*
;       x coordinate in AX
;       y coordinate in BX
;       return value in AX:BX
;       CL and DX are also affected.
$calc   proc    near
        push    ax
        mov     ax,_CUR_FRAME   ; get current frame number
        test    ax,ax           ; frame 0
        jz      cal1            ; use the optimized routine
IFDEF JLASER
        cmp     ax,01h
        jne     notjlsr
        push    ds
        mov     ax,jlaserseg
        mov     ds,ax
        shl     bx,1
        mov     ax,[bx+06000h]
        pop     ds
        sub     bx,bx
        xchg    bh,ah
        shr     bx,1
        shr     bx,1
        pop     dx
        mov     cl,4
        shr     dx,cl
        shl     dx,1
        add     bx,dx
        ret
notjlsr:
ENDIF
        mov     ax,bx           ; otherwise use parameter in table
        mul     _LNBYTE         ; line offset in dx ax
        pop     bx              ; get x coordinate in bx
        mov     cl,4
        shr     bx,cl           
        shl     bx,1            ; get horizontal offset in byte
        add     bx,ax           ; add with line offset to get result
        adc     dx,0      
        add     bx,_FRM_ST      ; add with the frame starting address
        adc     dx,0
        mov     ax,bx          
        mov     cx,4
cal4:   shr     dx,1            ; shift right 4 to get para. number
        rcr     ax,1
        loop    cal4
        add     ax,_FRM_ST+2    ; segment number in ax, add with start addr
        sub     ax,64           ; 
        and     bx,0fh          ; use last digit only
        add     bx,1024         ; offset is at least 1024
        ret

IFDEF COLOR
; The code below if for Color Graphics Card only
cal1:   mov     ax,0
        shr     bx,1
        jnc     short cal3
        mov     ax,0200h        ; odd lines start at ba00h segment
cal3:   add     ax,absscrn      ; even lines start b800h segment
        pop     dx              ; get x coordinate in dx
        mov     cl,4
        shr     dx,cl
        shl     dx,1            ; get horizontal byte offset
        mov     cl,4            ; 
        shl     bx,cl           ; bx <<= 4 <==> 16*bx
        add     dx,bx
        shl     bx,1            ; 
        shl     bx,1            ; bx <<=2  <==> 4*bx <==> 64*y
                                ; 16+64 = 80 (80 byte per line)
        add     bx,dx           ; move offset in bx
        ret
ENDIF

IFDEF HERC
;
; The code below is for Hercules Card
cal1:   mov     ax,bx           ; move y coordinate to ax
        shr     ax,1            ; 
        shr     ax,1            ; y coordinate / 4
        mul     _LNBYTE         ; get line offset from begining of region
        xchg    ax,bx           ; move y coordinate to ax again
        and     ax,03h          ; get region number 0..3
        mov     cl,9
        shl     ax,cl           ; region number * 512 
        add     ax,absscrn      ; add with starting segment number
        pop     dx              ; get x coordinate in dx
        mov     cl,4
        shr     dx,cl
        shl     dx,1            ; get horizontal offset number
        add     bx,dx           ; offset in bx
        ret
ENDIF

$calc   endp

; A C callable routine
;       ret=calcaddr(x,y);
;       int huge ret;
;       int x,y;
;
;       return value is a 4-byte pointer
_calcaddr proc   near    ;public to c
        push    bp
        mov     bp,sp
        mov     ax,[bp+smo]     ; get x coordinate
        mov     bx,[bp+smo+2]   ; get y coordinate
        call    $calc
        mov     dx,ax           ; result return in DX:AX
        mov     ax,bx
        pop     bp
        ret
_calcaddr endp

_phyaddr proc   near
        push    bp
        mov     bp,sp
        mov     ax,[bp+smo]
        mov     bx,[bp+smo+2]
        call    $calc
IFDEF JLASER
        cmp     _CUR_FRAME,01h
        jne     isphy
        mov     dx,0220h
        out     dx,al
        mov     ax,jlaserseg
isphy:
ENDIF
        mov     dx,ax
        mov     ax,bx
        pop     bp
        ret
_phyaddr endp

; *----------------------------------------------------------*
; * This routine is used to calculate the address of dot     *
; * below current dot. Address in given in es:bx             *
; *----------------------------------------------------------*
; input es:bx
; output es:bx
; also affect si and use the global variable _LASTY
downln  proc    near
        mov     si,_CUR_FRAME
        test    si,si
        jz      dl01            ; not frame 0
IFDEF JLASER
        cmp     si,01h
        jne     dlother
        push    ax
        mov     si,_LASTY
        shl     si,1
        mov     ax,jlaserseg
        mov     es,ax
        mov     ax,es:[si+06000h]
        sub     al,al
        shr     ax,1
        shr     ax,1
        sub     bx,ax
        inc     _LASTY
        add     si,2
        mov     ax,es:[si+06000h]
        mov     si,ax
        sub     ah,ah
        mov     es,ax
        mov     ax,si
        sub     al,al
        shr     ax,1
        shr     ax,1
        add     bx,ax
        pop     ax
        ret
dlother:
ENDIF
        mov     si,es           ; for frame created by GDS
        add     si,_LADDRS      ; next line is always _LADDRS:_LADDRO
        mov     es,si           ; from current line
        add     bx,_LADDRO
        ret

IFDEF HERC
dl01:   mov     si,es
        inc     word ptr _LASTY
        test    _LASTY,03h      ; get current region number
        jnz     dl02            ; then update address appropiately
        sub     si,0800h
        add     bx,90
dl02:   add     si,0200h
        mov     es,si
        ret
ENDIF

IFDEF COLOR
dl01:   mov     si,es
        inc     word ptr _LASTY
        test    _LASTY,01h
        jnz     dl02
        sub     si,0400h
        add     bx,80
dl02:   add     si,0200h
        mov     es,si
        ret
ENDIF
downln  endp

_TEXT   ends
        end

