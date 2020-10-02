        PAGE    60,132
        TITLE   Routines to work with DOS dates

; 001   13-Dec-86 date.asm

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

_BSS    SEGMENT
fmtbuf  DB 09H DUP (?)
        EVEN
_BSS    ENDS

_TEXT   SEGMENT

;****************************************************************************
;
;  unsigned int
;  getdate()
;
;  Get current date in integer format.
;
;****************************************************************************

        PUBLIC  _getdate

_getdate PROC NEAR
        push    bp
        mov     bp,sp

        mov     ax,2a00h               ;get date function
        int     21h

        mov     ax,cx                  ;return ( (cx - 1980) << 9) +
        sub     ax,1980                ;         (dh << 5) + dl )
        mov     cl,9
        shl     ax,cl
        mov     bx,ax

        mov     al,dh
        sub     ah,ah
        mov     cl,5
        shl     ax,cl

        sub     dh,dh

        or      ax,bx
        or      ax,dx

        mov     sp,bp
        pop     bp
        ret

_getdate        ENDP


;*****************************************************************************
;
;   char *
;   mmddyy(date)
;   unsigned int date;
;
;   Convert an integer date to MM/DD/YY format.
;
;*****************************************************************************

        PUBLIC  _mmddyy

_mmddyy PROC NEAR
        push    bp
        mov     bp,sp

        mov     bx,OFFSET DGROUP:fmtbuf

        mov     ax,[bp+4]              ;isolate month
        and     ax,01e0h
        mov     cl,5
        shr     ax,cl

        mov     ah,' '                 ;cvt to ascii with blank fill
        call    al2asc

        mov     BYTE PTR [bx],'/'      ;separator
        inc     bx

        mov     ax,[bp+4]              ;isolate day
        and     ax,001fh

        mov     ah,'0'                 ;cvt to ascii with 0 fill
        call    al2asc

        mov     BYTE PTR [bx],'/'      ;separator
        inc     bx

        mov     ax,[bp+4]              ;isolate year
        and     ax,0fe00h
        mov     cl,9
        shr     ax,cl
        add     ax,80                  ;offset from year 1980

        mov     ah,'0'                 ;cvt to ascii with 0 fill
        call    al2asc

        mov     BYTE PTR [bx],0        ;terminate string

        mov     ax,OFFSET DGROUP:fmtbuf  ;tell caller where it is

        mov     sp,bp
        pop     bp
        ret

_mmddyy ENDP


al2asc  PROC    NEAR                   ;cvt al to 2 ascii digits

        cmp     al,10                  ;is there a tens digit to cvt?
        jae     dotens                 ;yes, go do it
        mov     [bx],ah                ;no, store fill char
        jmp     SHORT dounits

dotens: sub     cl,cl                  ;count the # tens
cntens: inc     cl
        sub     al,10
        cmp     al,10
        jae     cntens

        add     cl,'0'                 ;convert count to ascii
        mov     [bx],cl                ;  and store

dounits: inc    bx                     ;move to the units digit
        add     al,'0'                 ;convert units to ascii
        mov     [bx],al                ;  and store

        inc     bx                     ;update for caller
        ret
al2asc  ENDP

_TEXT   ENDS
END
