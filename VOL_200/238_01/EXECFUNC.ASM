oa      equ     4

DGROUP  GROUP   _DATA
_DATA   segment word public 'DATA'
        assume  ds:DGROUP
funaddr dw      _TEXT:_Arc1
        dw      _TEXT:_Arc2
        dw      _TEXT:_ArcPoint
        dw      _TEXT:_BlockCopy
        dw      _TEXT:_BlockLoad
        dw      _TEXT:_BlockSave
        dw      _TEXT:_Box
        dw      _TEXT:_Circle
        dw      _TEXT:_CreateFrame
        dw      _TEXT:_Dot
        dw      _TEXT:_Draw
        dw      _TEXT:_Earc1
        dw      _TEXT:_Earc2
        dw      _TEXT:_Ellipse
        dw      _TEXT:_EnvRsto
        dw      _TEXT:_EnvSave
        dw      _TEXT:_FillCircle
        dw      _TEXT:_FillEllipse
        dw      _TEXT:_HorzLine
        dw      _TEXT:_Line
        dw      _TEXT:_LoadFont
        dw      _TEXT:_NextXY
        dw      _TEXT:_PatternFill
        dw      _TEXT:_PlotType
        dw      _TEXT:_PrintFrame
        dw      _TEXT:_PrintPage
        dw      _TEXT:_ReadStr
        dw      _TEXT:_Rectangle
        dw      _TEXT:_RelOrg
        dw      _TEXT:_RemvFont
        dw      _TEXT:_RemvFrame
        dw      _TEXT:_ResetWin
        dw      _TEXT:_SelectFont
        dw      _TEXT:_SelectFrame
        dw      _TEXT:_SetOrg
        dw      _TEXT:_SetStyle
        dw      _TEXT:_SetWin
        dw      _TEXT:_SolidFill
        dw      _TEXT:_VertLine
        dw      _TEXT:_WriteStr
        dw      _TEXT:_XHLine
_DATA   ends

_TEXT   segment byte public 'CODE'
        assume  cs:_TEXT,ds:DGROUP

        public  _execfunc

        extrn   _Arc1:near, _Box:near, _Circle:near,
        extrn   _CreateFrame:near, _HorzLine:near, _PrintFrame:near
        extrn   _Dot:near, _Earc1:near, _Ellipse:near
        extrn   _Line:near, _LoadFont:near, _PlotType:near
        extrn   _Rectangle:near, _RelOrg:near, _RemvFont:near
        extrn   _RemvFrame:near, _SetStyle:near, _SelectFont:near
        extrn   _ResetWin:near, _SelectFrame:near, _SetOrg:near
        extrn   _SetWin:near, _VertLine:near, _WriteStr:near
        extrn   _Draw:near, _FillCircle:near, _FillEllipse:near
        extrn   _Arc2:near, _Earc2:near, _PrintPage:near
        extrn   _EnvSave:near, _EnvRsto:near, _BlockCopy:near
        extrn   _BlockLoad:near, _BlockSave:near, _ArcPoint:near
        extrn   _SolidFill:near, _PatternFill:near, _ReadStr:near
        extrn   _XHLine:near, _NextXY:near

_execfunc proc   near
        push    bp
        mov     bp,sp
        mov     bx,[bp+oa+2]
        test    bx,bx
        jz      short ef4
        mov     cx,bx
        dec     bx
        shl     bx,1 
        shl     bx,1
        add     bx,[bp+oa+4]
ef1:    mov     ax,[bx]
        cmp     ax,1    ; INTG
        jnz     short ef010
        mov     ax,[bx+2]
        push    ax
        jmp     short ef900
ef010:  cmp     ax,2    ; STRG
        jnz     short ef020
        mov     ax,[bx+2]
        push    ax
        jmp     ef900
ef020:  sub     ax,ax
        push    ax
ef900:  sub     bx,4
        loop    ef1
ef4:    mov     bx,[bp+oa]
        shl     bx,1
        call    funaddr[bx]
        mov     sp,bp
        mov     cx,[bp+oa+6]
        mov     bx,[bp+oa+8]
        mov     [bx],cx
        cmp     cx,1
        jnz     ef110
        mov     [bx+2],ax
        jmp     ef190
ef110:  cmp     cx,2
        jnz     ef120
        mov     [bx+2],ax
        jmp     ef190
ef120:  mov     [bx],word ptr 0
ef190:  pop     bp
        ret
_execfunc endp

_TEXT   ends
        end

