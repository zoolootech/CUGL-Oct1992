        PAGE   60,132
        TITLE  Routines allocate/free FAR memory

;  001  13-Dec-86  farmem.asm

;       Copyright (c) 1986 by Blue Sky Software.  All rights reserved.


_TEXT   SEGMENT  BYTE PUBLIC 'CODE'
_TEXT   ENDS
_DATA   SEGMENT  WORD PUBLIC 'DATA'
_DATA   ENDS
CONST   SEGMENT  WORD PUBLIC 'CONST'
CONST   ENDS
_BSS    SEGMENT  WORD PUBLIC 'BSS'
_BSS    ENDS
DGROUP  GROUP   CONST,  _BSS,   _DATA
        ASSUME  CS: _TEXT, DS: DGROUP, SS: DGROUP, ES: DGROUP

_TEXT   SEGMENT

;****************************************************************************
;
;  char far *
;  malloc_f(size)              /* allocate far memory blk of size bytes */
;  unsigned int size;          /* not a long */
;
;****************************************************************************

        PUBLIC  _malloc_f

_malloc_f PROC NEAR
        push    bp
        mov     bp,sp

        mov     ah,48h                 ;DOS int 21h - 48h is alloc memory

        mov     bx,[bp+4]              ;round size up to # paragraphs
        add     bx,15                  ; (size + 15) >> 4
        mov     cl,4
        shr     bx,cl

        int     21h                    ;try for it

        jc      nomem                  ;carry set if insufficient memory

        mov     dx,ax                  ;go it - dx is return segment
        jmp     SHORT mexit

nomem:  xor     dx,dx                  ;no mem - return NULL (segment)

mexit:  xor     ax,ax                  ;always zero offset

        mov     sp,bp
        pop     bp
        ret

_malloc_f       ENDP


;*****************************************************************************
;
;  char far *
;  largest_f(max,sizep)   /* allocate largest possible block up to max */
;  unsigned int max, *sizep;
;
;*****************************************************************************

        PUBLIC  _largest_f

_largest_f PROC NEAR
        push    bp
        mov     bp,sp

tryall: mov     ah,48h                 ;DOS int 21h - 48h is alloc memory

        mov     bx,[bp+4]              ;round size up to # paragraphs
        add     bx,15                  ; (size + 15) >> 4
        mov     cl,4
        shr     bx,cl

        int     21h                    ;try for it

        jc      nomeml                 ;carry set if insufficient memory

        mov     dx,ax                  ;go it - dx is return segment
        mov     bx,[bp+6]              ;return size of block
        mov     ax,[bp+4]
        mov     [bx],ax

        jmp     SHORT lexit

nomeml: mov     ax,bx
        mov     cl,4                   ;couldn't get it, try what dos said
        shl     ax,cl                  ; was the max (bx)
        mov     [bp+4],ax
        or      ax,ax                  ;shouldn't happen, but...
        jne     tryall

        xor     dx,dx                  ;no mem at all! - return NULL (segment)

lexit:  xor     ax,ax                  ;always zero offset

        mov     sp,bp
        pop     bp
        ret

_largest_f      ENDP


;*****************************************************************************
;
;  free_f(memloc)              /* release far memory */
;  char far *memloc;
;
;*****************************************************************************

        PUBLIC _free_f

_free_f PROC NEAR
        push    bp
        mov     bp,sp

        mov     ax,[bp+6]              ;segmemt of mem to free - offset
        mov     es,ax                  ;  better be 0!

        mov     ah,49h                 ;DOS int 21h - 49h releases mem
        int     21h

        mov     sp,bp
        pop     bp
        ret

_free_f ENDP

_TEXT   ENDS
END
