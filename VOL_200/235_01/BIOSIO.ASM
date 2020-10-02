        PAGE   60,132
        TITLE  Routines to do bios screen I/O

;  002  30-Jan-87  biosio.asm

;       Copyright (c) 1986 by Blue Sky Software.  All rights reserved.


_TEXT   SEGMENT  BYTE PUBLIC 'CODE'
_TEXT   ENDS
CONST   SEGMENT  WORD PUBLIC 'CONST'
CONST   ENDS
_BSS    SEGMENT  WORD PUBLIC 'BSS'
_BSS    ENDS
_DATA   SEGMENT  WORD PUBLIC 'DATA'
_DATA   ENDS

DGROUP  GROUP   CONST,  _BSS,   _DATA
        ASSUME  CS: _TEXT, DS: DGROUP, SS: DGROUP, ES: DGROUP

_TEXT      SEGMENT

;******************************************************************************
;
;  getvideomode(width,page)       get the current video mode/width/page
;  int *width, *page;
;
;******************************************************************************

        PUBLIC _getvideomode

_getvideomode PROC NEAR
        push   bp
        mov    bp,sp

        mov    ah,0fh                  ; just use bios int 10h - 15
        int    10h

        mov    cl,bh                   ; save page # (bh)
        xor    ch,ch
        mov    bx,[bp+6]               ; page pointer
        mov    [bx],cx

        mov    cl,ah                   ; current width
        mov    bx,[bp+4]               ; width pointer
        mov    [bx],cx

        xor    ah,ah                   ; al already has the mode

        mov    sp,bp
        pop    bp
        ret

_getvideomode ENDP


;******************************************************************************
;
;  setvideomode(mode)             set the current video mode
;  int mode;
;
;******************************************************************************

        PUBLIC _setvideomode

_setvideomode PROC NEAR
        push   bp
        mov    bp,sp

        mov    ah,0                    ; just use bios int 10h - 0
        mov    al,[bp+4]               ; low byte of mode
        int    10h

        mov    sp,bp
        pop    bp
        ret

_setvideomode ENDP


;******************************************************************************
;
;  readcursor(page,row,col,stscan,endscan)  /* read cursor info */
;  int page;
;  int *row, *col, *stscan, *endscan;
;
;******************************************************************************

        PUBLIC _readcursor

_readcursor  PROC NEAR
        push   bp
        mov    bp,sp

        mov    ah,3                    ; just use bios int 10h - 3
        mov    bh,[bp+4]               ; low byte of page
        int    10h

        xor    ah,ah
        mov    al,dh                   ; cursor row
        mov    bx,[bp+6]
        mov    [bx],ax

        mov    al,dl                   ; cursor column
        mov    bx,[bp+8]
        mov    [bx],ax

        mov    al,ch                   ; cursor start scan line
        mov    bx,[bp+10]
        mov    [bx],ax

        mov    al,cl                   ; cursor end scan line
        mov    bx,[bp+12]
        mov    [bx],ax

        mov    sp,bp
        pop    bp
        ret

_readcursor ENDP


;******************************************************************************
;
;  setcursorsize(startscan,endscan)   /* set cursor size */
;  int startscan, endscan;
;
;******************************************************************************

        PUBLIC _setcursorsize

_setcursorsize PROC   NEAR
        push   bp
        mov    bp,sp

        mov    ah,1                    ; just use DOS int 10h - 1
        mov    ch,[bp+4]               ; starting scan line
        mov    cl,[bp+6]               ; ending scan line
        int    10h

        mov    sp,bp
        pop    bp
        ret
_setcursorsize ENDP


;******************************************************************************
;
;  setdisplaypage(page)                /* set current display page # */
;  int page;
;
;******************************************************************************

        PUBLIC _setdisplaypage

_setdisplaypage PROC   NEAR
        push   bp
        mov    bp,sp

        mov    ah,5                    ; just use DOS int 10h - 5
        mov    al,[bp+4]               ; wanted display page #
        int    10h

        mov    sp,bp
        pop    bp
        ret
_setdisplaypage ENDP


;******************************************************************************
;
;  getraw()       get a char from console without ^C checking
;
;******************************************************************************

        PUBLIC _getraw

_getraw PROC   NEAR
        push   bp
        mov    bp,sp

        mov    ah,7                    ; just use DOS int 21h - 7
        int    21h
        xor    ah,ah

        mov    sp,bp
        pop    bp
        ret
_getraw ENDP


;******************************************************************************
;
;  peekchr()      peek a char from console without removing from buffer
;
;******************************************************************************

        PUBLIC _peekchr

_peekchr PROC  NEAR
        push   bp
        mov    bp,sp

        mov    ah,1                    ; just use BIOS int 16h - 1
        int    16h
        jz     nochr                   ; Z means no char waiting

        xor    ah,ah                   ; got a char in al, clr ah
        jmp    SHORT peekret

nochr:  xor    ax,ax                   ; tell caller no char

peekret: mov   sp,bp
        pop    bp
        ret
_peekchr ENDP


_TEXT   ENDS
        END
