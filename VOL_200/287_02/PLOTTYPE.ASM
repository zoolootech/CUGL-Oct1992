        TITLE   PLOTTYPE of GDS
        page    60,132
        .SFCOND
;
; *==========================================================*
; * This file contains the lowest level routine in writing   *
; * and accessing the frame memory. And some other related   *
; * routines. Don't write or read the frame memory whenever  *
; * possible, use these routines.                            *
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

        extrn   _CUR_PLOT:word
IFDEF JLASER
        extrn   _CUR_FRAME:word
        public  jlaserseg
ENDIF

        public  wrtvec  ; write vector

IFDEF JLASER
jpgname db      'EMMXXXX0',0
procid  dw      0
jlaserseg dw    0
dmaln   dw      0
ENDIF

owrtvec dw      $write
pltvec  dw      $or
        dw      $and
        dw      $xor
wrtvec  dw      $or     ; default is $or
_DATA   ends

_TEXT   segment byte public 'CODE'
        assume  cs:_TEXT,ds:DGROUP
        public  _fr_write,_PlotType,_fr_read
        public  $aread, $read, _writetype, $write

IFDEF JLASER
        public  _initjlsr, _cleanjlsr, _dumpjlsr
ENDIF

$aread  proc    near
IFDEF JLASER
        cmp     _CUR_FRAME,01h
        jne     ardother
        push    es
        push    dx
        push    ax
        mov     dx,0220h
        mov     ax,es
        out     dx,al
        mov     ax,jlaserseg
        mov     es,ax
        pop     ax
        and     ax,es:[bx]
        pop     dx
        pop     es
        ret
ardother:
ENDIF
        and     ax,es:[bx]
        ret
$aread  endp

$read   proc    near
IFDEF JLASER
        cmp     _CUR_FRAME,01h
        jne     rdother
        push    es
        push    dx
        push    ax
        mov     dx,0220h
        mov     ax,es
        out     dx,al
        mov     ax,jlaserseg
        mov     es,ax
        pop     ax
        mov     ax,es:[bx]
        pop     dx
        pop     es
        ret
rdother:
ENDIF
        mov     ax,es:[bx]
        ret
$read   endp

; A C callable routine
;       ret=fr_read(location);
;       int ret;
;       int far *location;
;
;       return value is the word at location
_fr_read proc   near     ;public routine
        push    bp
        mov     bp,sp
        les     bx,dword ptr [bp+smo]   ; get address into es:bx
        mov     ax,es:[bx]
        pop     bp
        ret
_fr_read endp

; A C callable routine
;       ret=fr_write(location, word);
;       int ret, word;
;       int far *location;
;
;       return value is same as word
_fr_write proc   near    ;public routine
        push bp
        mov     bp,sp
        mov     ax,[bp+smo+4]
        les     bx,dword ptr [bp+smo]   ; get address into es:bx
        pop     bp
        jmp     word ptr wrtvec
_fr_write endp
;
; all external accessing of $or, $and or $xor must go
; through the vector -- wrtvec
$write  proc    near
IFDEF JLASER
        cmp     _CUR_FRAME,01h
        jne     wrtother
        push    es
        push    dx
        push    ax
        mov     dx,0220h
        mov     ax,es
        out     dx,al
        mov     ax,jlaserseg
        mov     es,ax
        pop     ax
        mov     es:[bx],ax
        pop     dx
        pop     es
        ret
wrtother:
ENDIF
        mov     es:[bx],ax
        ret
$write  endp

$or     proc    near
IFDEF JLASER
        cmp     _CUR_FRAME,01h
        jne     orother
        push    es
        push    dx
        push    ax
        mov     dx,0220h
        mov     ax,es
        out     dx,al
        mov     ax,jlaserseg
        mov     es,ax
        pop     ax
        or      es:[bx],ax
        pop     dx
        pop     es
        ret
orother:
ENDIF
        or      es:[bx],ax
        ret
$or     endp

$and    proc    near
IFDEF JLASER
        cmp     _CUR_FRAME,01h
        jne     andother
        push    es
        push    dx
        push    ax
        mov     dx,0220h
        mov     ax,es
        out     dx,al
        mov     ax,jlaserseg
        mov     es,ax
        pop     ax
        or      es:[bx],ax
        xor     es:[bx],ax
        pop     dx
        pop     es
        ret
andother:
ENDIF
        or      es:[bx],ax      ; reset all corresponding
        xor     es:[bx],ax      ; bit which are 1 in ax to 0
        ret                     ; in the word pointed by es:bx
$and    endp

$xor    proc    near
IFDEF JLASER
        cmp     _CUR_FRAME,01h
        jne     xorother
        push    es
        push    dx
        push    ax
        mov     dx,0220h
        mov     ax,es
        out     dx,al
        mov     ax,jlaserseg
        mov     es,ax
        pop     ax
        xor     es:[bx],ax
        pop     dx
        pop     es
        ret
xorother:
ENDIF
        xor     es:[bx],ax
        ret
$xor    endp

; A C callable routine
;       PlotType(type);
;       int type;      /* use only the least significant 2 bits */
_PlotType proc   near    ;public routine
        push    bp
        mov     bp,sp
        mov     bx,[bp+smo]     ; get the type
        and     bx,03h
        mov     cx,_CUR_PLOT
        mov     _CUR_PLOT,bx
        shl     bx,1
        mov     ax,pltvec[bx]   ; set up the vector
        mov     wrtvec,ax
        mov     ax,cx
        pop     bp
        ret
_PlotType endp

_writetype proc near
        mov     _CUR_PLOT,04h
        mov     ax,owrtvec
        mov     wrtvec,ax
        ret
_writetype endp

IFDEF   JLASER
_initjlsr proc  near
        mov     dx,0220h
        mov     al,0f2h
        out     dx,al
        push    ds
        mov     ax,0d000h
        mov     ds,ax
        mov     ax,1
        mov     ds:033a3h,al
        mov     ax,3
        mov     ds:033a3h,al
        pop     ds
        mov     ax,3d00h
        lea     dx,jpgname
        int     21h
        jc      jlsr_err
        xchg    ax,bx
        mov     ax,4407h
        xor     cx,cx
        int     21h
        pushf
        push    ax
        mov     ah,3eh
        int     21h
        pop     ax
        popf
        jc      jlsr_err
        cmp     al,0ffh
        jne     jlsr_err
        mov     ah,40h
        int     67h
        or      ah,ah
        jnz     jlsr_err
        mov     ah,42h
        int     67h
        or      ah,ah
        jnz     jlsr_err
        xchg    ax,bx
        mov     bx,63
        cmp     bx,ax
        ja      jlsr_err
        mov     ah,43h
        int     67h
        or      ah,ah
        jnz     jlsr_err
        mov     procid,dx
        mov     ah,41h
        int     67h
        mov     jlaserseg,bx
        mov     ax,4401h
        xor     bx,bx
        int     67h
        mov     ah,3dh
        mov     dx,procid
        mov     bx,62
        int     67h
        or      ah,ah
        jnz     jlsr_err
        mov     dmaln,bx
        mov     ah,3eh
        int     67h
        or      ah,ah
        jnz     jlsr_err
        ret
jlsr_err:
        mov     ax,0ffffh
        ret
_initjlsr endp

_cleanjlsr proc near
        mov     ah,45h
        mov     dx,[procid]
        int     67h
        ret
_cleanjlsr endp

_dumpjlsr proc  near
        mov     ah,3fh
        int     67h
        mov     al,ah
        sub     ah,ah
        ret
_dumpjlsr endp

ENDIF

_TEXT   ends
        end

