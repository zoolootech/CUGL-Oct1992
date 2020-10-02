FD_SIZE equ     20
oa      equ     4

DGROUP  group   _DATA
_DATA   segment word public 'DATA'
        assume  ds:DGROUP

        extrn   _DOTVALUE:word,_LEFTWORD:word,_RIGHTWORD:word

        extrn   wrtvec:word
        extrn   _WINX1:word,_WINX2:word,_WINY1:word,_WINY2:word

vsa     dw      ?
voa     dw      ?
voa2    dw      ?
fflag   dw      ?
_DATA   ends

_TEXT   segment byte public 'CODE'
        assume  cs:_TEXT,ds:DGROUP

        extrn   $calc:near, $read:near, $aread:near, $write:near

        public  _hlnlmt, _skipright
;
;  hlnlmt(x,y,boundary,fill_pattern,&leftx, &rightx);
;
_hlnlmt proc    near
        push    bp
        mov     bp,sp
        push    di
        mov     fflag,0
        mov     ax,[bp+oa]
        mov     bx,[bp+oa+2]
        mov     di,ax
        and     di,0fff0h
        call    $calc
        mov     es,ax
        mov     vsa,ax
        mov     voa,bx
        mov     voa2,bx
IFDEF JLASER
        call    $read
ELSE
        mov     ax,es:[bx]
ENDIF
        xor     ax,[bp+oa+4]
        mov     cx,di          ; code for handling window
        cmp     cx,_WINX1
        jae     hlt510
        mov     bx,_WINX1
        and     bx,0fh
        shl     bx,1
        or      ax,_DOTVALUE[bx-2]
hlt510: add     cx,15
        cmp     cx,_WINX2
        jbe     hlt520
        mov     bx,_WINX2
        and     bx,0fh
        shl     bx,1
        or      ax,_DOTVALUE[bx+2]
hlt520: push    ax
        mov     bx,[bp+oa]
        and     bx,0fh
        shl     bx,1
        test    ax,_DOTVALUE[bx]
        jz      hlt005
        pop     ax
        mov     ax,0ffffh
        jmp     hltret          ; return
hlt005: test    ax,ax
        jnz     short hlt010
        pop     ax
        mov     ax,[bp+oa+6]
        mov     bx,voa
        jmp     near ptr hlt200
hlt010: and     ax,_RIGHTWORD[bx+2]
        jnz     short hlt020
        mov     dx,_RIGHTWORD[bx+2]
        jmp     short hlt100
hlt020: mov     dx,_DOTVALUE[bx]
        xchg    dl,dh
        xchg    ah,al
        mov     cx,bx
        shr     cx,1
        shr     ax,1
hlt030: test    ax,dx
        jnz     hlt040
        shr     ax,1
        dec     cx
        jmp     hlt030
hlt040: mov     dx,_RIGHTWORD[bx+2]
        mov     bx,cx
        shl     bx,1
        and     dx,_LEFTWORD[bx]
        mov     bx,[bp+oa+8]
        add     cx,di
        mov     [bx],cx
hlt100: pop     ax
        push    dx
        mov     bx,[bp+oa]
        and     bx,0fh
        shl     bx,1
        and     ax,_LEFTWORD[bx+2]
        jnz     short hlt105
        mov     dx,_LEFTWORD[bx+2]
        jmp     hlt130
hlt105: mov     dx,_DOTVALUE[bx]
        xchg    ah,al
        xchg    dh,dl
        mov     cx,bx
        shr     cx,1
        shl     ax,1
hlt110: test    ax,dx
        jnz     hlt120
        shl     ax,1
        inc     cx
        jmp     hlt110
hlt120: mov     dx,_LEFTWORD[bx+2]
        mov     bx,cx
        shl     bx,1
        and     dx,_RIGHTWORD[bx+2]
        mov     bx,[bp+oa+10]
        add     cx,di
        mov     [bx],cx
hlt130: pop     ax
        or      dx,ax
        mov     ax,[bp+oa+6]
        and     ax,dx
        mov     bx,voa
        not     dx
        mov     fflag,dx
        xchg    ax,dx
IFDEF JLASER
        call    $aread
ELSE
        and     ax,es:[bx]
ENDIF
        or      ax,dx
hlt200: 
IFDEF JLASER
        call    $write
ELSE
        mov     es:[bx],ax
ENDIF
        test    fflag,0080h
        jz      hlt220
        jmp     hltrgt2
hlt220: mov     bx,voa2
hlt308: sub     bx,2
        mov     voa2,bx
IFDEF JLASER
        call    $read
ELSE
        mov     ax,es:[bx]
ENDIF
        mov     cx,di
        sub     cx,16
        js      hltrgt
        mov     di,cx
        cmp     cx,_WINX1
        jb      hlt310
        xor     ax,[bp+oa+4]
        jnz     hlt320
        mov     ax,[bp+oa+6]
IFDEF JLASER
        call    $write
ELSE
        mov     es:[bx],ax
ENDIF
        jmp     hlt308
hlt310: mov     bx,_WINX1
        and     bx,0fh
        shl     bx,1
        or      ax,_DOTVALUE[bx-2]
hlt320: xchg    ah,al
        mov     cx,16
hlt330: test    ax,1
        jnz     hlt340
        shr     ax,1
        dec     cx
        jmp     hlt330
hlt340: mov     bx,cx
        shl     bx,1
        mov     dx,_LEFTWORD[bx]
        mov     bx,[bp+oa+8]
        add     cx,di
        mov     [bx],cx
        mov     ax,[bp+oa+6]
        and     ax,dx
        mov     bx,voa2
        not     dx
        xchg    ax,dx
IFDEF JLASER
        call    $aread
ELSE
        and     ax,es:[bx]
ENDIF
        or      ax,dx
IFDEF JLASER
        call    $write
ELSE
        mov     es:[bx],ax
ENDIF
        jmp     short hltrgt3
hltrgt: mov     bx,[bp+oa+8]
        mov     word ptr [bx],0
hltrgt3: mov     ax,voa
        mov     voa2,ax
        mov     di,[bp+oa]
hltrgt2: test   fflag,0100h
        jnz     hltret2
        or      di,0fh
        mov     bx,voa2
hlt408: add     bx,2
        mov     voa2,bx
IFDEF JLASER
        call    $read
ELSE
        mov     ax,es:[bx]
ENDIF
        xor     ax,[bp+oa+4]
        add     di,16
        cmp     di,_WINX2
        ja      hlt410
        test    ax,ax
        jnz     hlt420
        mov     ax,[bp+oa+6]
IFDEF JLASER
        call    $write
ELSE
        mov     es:[bx],ax
ENDIF
        jmp     hlt408
hlt410: mov     bx,_WINX2
        and     bx,0fh
        shl     bx,1
        or      ax,_DOTVALUE[bx+2]
hlt420: xchg    ah,al
        xor     cx,cx
hlt430: test    ax,08000h
        jnz     hlt440
        shl     ax,1
        inc     cx
        jmp     hlt430
hlt440: mov     bx,cx
        shl     bx,1
        mov     dx,_RIGHTWORD[bx]
        mov     bx,[bp+oa+10]
        add     cx,di
        sub     cx,16
        mov     [bx],cx
        mov     ax,[bp+oa+6]
        and     ax,dx
        mov     bx,voa2
        not     dx
        xchg    ax,dx
IFDEF JLASER
        call    $aread
ELSE
        and     ax,es:[bx]
ENDIF
        or      ax,dx
IFDEF JLASER
        call    $write
ELSE
        mov     es:[bx],ax
ENDIF
hltret2: mov    ax,0
hltret: pop     di
        pop     bp
        ret
_hlnlmt endp

_skipright proc near
        push    bp
        mov     bp,sp
        push    si
        push    di
        mov     ax,[bp+oa]
        mov     bx,[bp+oa+2]
        mov     di,ax
        or      di,0fh
        call    $calc
        mov     es,ax
        mov     si,bx
IFDEF JLASER
        call    $read
ELSE
        mov     ax,es:[bx]
ENDIF
        xor     ax,[bp+oa+4]
        mov     bx,[bp+oa]
        and     bx,0fh
        shl     bx,1
        and     ax,_LEFTWORD[bx]
        cmp     di,[bp+oa+6]
        jbe     skr020
        mov     cx,bx
        mov     bx,[bp+oa+6]
        and     bx,0fh
        shl     bx,1
        or      ax,_DOTVALUE[bx+2]
        mov     bx,cx
skr020: test    ax,_DOTVALUE[bx]
        jz      short skr105
        mov     ax,[bp+oa]
        jmp     skrret
skr105: test    ax,ax
        jz      short skr200
        mov     dx,_DOTVALUE[bx]
        xchg    ah,al
        xchg    dh,dl
        mov     cx,bx
        shr     cx,1
        inc     cx
        shl     ax,1
skr110: test    ax,dx
        jnz     skrret2
        shl     ax,1
        inc     cx
        jmp     skr110
;
skr200: add     si,2
        mov     bx,si
IFDEF JLASER
        call    $read
ELSE
        mov     ax,es:[bx]
ENDIF
        xor     ax,[bp+oa+4]
        add     di,16
        cmp     di,[bp+oa+6]
        ja      skr410
        test    ax,ax
        jz      skr200
        jmp     skr420
skr410: mov     bx,[bp+oa+6]
        and     bx,0fh
        shl     bx,1
        or      ax,_DOTVALUE[bx+2]
skr420: xchg    ah,al
        xor     cx,cx
skr430: test    ax,08000h
        jnz     skr440
        shl     ax,1
        inc     cx
        jmp     skr430
skrret2:
skr440: mov     ax,cx
        add     ax,di
        sub     ax,16-1
skrret: pop     di
        pop     si
        pop     bp
        ret
_skipright endp

_TEXT   ends
        end

