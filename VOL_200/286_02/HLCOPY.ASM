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

        extrn   wrtvec:word

_DATA   ends

_TEXT   segment byte public 'CODE'
        assume  cs:_TEXT,ds:DGROUP

        public _hlcopy
        extrn  $calc:near


; A C callable routine
;       hlcopy(sptr,dptr,startx,length);
;       char far *sptr, far *dptr;
;       int     startx,length;
;
_hlcopy proc    near    ;public routine
        push    bp
        mov     bp,sp
        push    si
        push    di
        mov     di,[bp+smo+8]   ; get starting x coordinate
        and     di,0fh          ; get dot position within word
        mov     si,di
        add     di,[bp+smo+10]
        shl     si,1
        mov     ax,_LEFTWORD[si]
        les     bx,dword ptr [bp+smo]     ; load sptr into es:bx
        cmp     di,16           ; di > 16 if the line is not within the
        ja      chl1            ; same word
        shl     di,1            ; othwise the line is in the same word
        xor     ax,_LEFTWORD[di]
        and     ax,es:[bx]
        les     bx,dword ptr [bp+smo+4]   ; load dptr into es:bx
        pop     di
        pop     si
        pop     bp
        jmp     wrtvec
; line span across more than 1 word
chl1:   and     ax,es:[bx]
        les     bx,dword ptr [bp+smo+4]   ; load dptr into es:bx
        call    wrtvec
        mov     cl,4            ; get the number of full words
        mov     si,di           ; between first and last word
        shr     si,cl
        dec     si
        jz      short chl2
        mov     cx,si
chl3:   les     bx,dword ptr [bp+smo]
        inc     bx
        inc     bx
        mov     [bp+smo],bx
        mov     ax,es:[bx]
        les     bx,dword ptr [bp+smo+4]
        inc     bx              ; repeat copying
        inc     bx
        mov     [bp+smo+4],bx
        call    wrtvec
        loop    short chl3
chl2:   les     bx,dword ptr [bp+smo]     ; load sptr into es:bx
        inc     bx              ; point to next word
        inc     bx
        and     di,0fh
        shl     di,1
        mov     ax,_RIGHTWORD[di]       ; set up last word
        and     ax,es:[bx]
        les     bx,dword ptr [bp+smo+4]
        inc     bx
        inc     bx
        pop     di
        pop     si
        pop     bp
        jmp     word ptr wrtvec
_hlcopy endp

_TEXT   ends
        end

