          page   80,132
          page
;
;         Kent Cedola
;         2015 Meadow Lake Court
;         Norfolk, Virginia  23518
;

dgroup    group  _data

_data     segment word public 'data'
          assume ds:dgroup

          extrn  _gdwd_x1:word,_gdwd_x2:word,_gdwd_x3:word
          extrn  _gdwd_y1:word,_gdwd_y2:word,_gdwd_y3:word
          extrn  _gdvw_x1:word,_gdvw_x2:word,_gdvw_x3:word
          extrn  _gdvw_y1:word,_gdvw_y2:word,_gdvw_y3:word
          extrn  _gdc_flg:byte

_data     ends

_text     segment byte public 'code'

          assume cs:_text,ds:dgroup
          public _gpclip2
_gpclip2  proc   near

          push   bp
          mov    bp,sp

          push   si
          push   di

          push   bp

          mov    di,[bp+4]
          mov    cx,[di]               ; Load 'from' x coordinate of draw
          mov    di,[bp+6]
          mov    bx,[di]               ; Load 'from' y coordinate of draw
          mov    di,[bp+8]
          mov    si,[di]               ; Load 'to' x coordinate of draw
          mov    di,[bp+10]
          mov    di,[di]               ; Load 'to' y coordinate of draw

          mov    _gdc_flg,0

;      Perform the Cohen-Sutherland two-step clipping algorithm, in which a
;  four-element code indicates the position of a point.  From pages 36-37 of
;  the book "Microcomputer Displays, Graphics, and Animation" by Bruce Atwick.

dline01:
          XOR    AX,AX

;      Generate 4-element start and end point codes

          CMP    BX,_gdwd_y1
          JGE    $+5
          OR     AH,8
          CMP    BX,_gdwd_y2
          JLE    $+5
          OR     AH,4
          CMP    CX,_gdwd_x1
          JGE    $+5
          OR     AH,2
          CMP    CX,_gdwd_x2
          JLE    $+5
          OR     AH,1

          CMP    DI,_gdwd_y1
          JGE    $+4
          OR     AL,8
          CMP    DI,_gdwd_y2
          JLE    $+4
          OR     AL,4
          CMP    SI,_gdwd_x1
          JGE    $+4
          OR     AL,2
          CMP    SI,_gdwd_x2
          JLE    $+4
          OR     AL,1

          OR     AX,AX
          JNZ    dline15
          JMP    dline09
dline15:
          mov    _gdc_flg,1
          TEST   AH,AL
          JZ     dline02
          mov    _gdc_flg,2
          pop    bp
          jmp    done
dline02:
          OR     AL,AL
          JNZ    dline03

;      Swap start and end points

          XCHG   CX,SI                 ;
          XCHG   BX,DI                 ;
          XCHG   AH,AL

dline03:

          TEST   AL,2
          JNZ    dline06
          TEST   AL,4
          JNZ    dline07
          TEST   AL,8
          JNZ    dline08
dline05:
          MOV    BP,DI
          SUB    BP,BX
          MOV    AX,_gdwd_x2
          SUB    AX,CX
          IMUL   BP
          MOV    BP,SI
          SUB    BP,CX
          IDIV   BP
          ADD    AX,BX
          MOV    SI,_gdwd_x2
          MOV    DI,AX
          JMP    dline01
dline06:
          MOV    BP,DI
          SUB    BP,BX
          MOV    AX,_gdwd_x1
          SUB    AX,CX
          IMUL   BP
          MOV    BP,SI
          SUB    BP,CX
          IDIV   BP
          ADD    AX,BX
          MOV    SI,_gdwd_x1
          MOV    DI,AX
          JMP    dline01
dline07:
          MOV    BP,SI
          SUB    BP,CX
          MOV    AX,_gdwd_y2
          SUB    AX,BX
          IMUL   BP
          MOV    BP,DI
          SUB    BP,BX
          IDIV   BP
          ADD    AX,CX
          MOV    SI,AX
          MOV    DI,_gdwd_y2
          JMP    dline01
dline08:
          MOV    BP,SI
          SUB    BP,CX
          MOV    AX,_gdwd_y1
          SUB    AX,BX
          IMUL   BP
          MOV    BP,DI
          SUB    BP,BX
          IDIV   BP
          ADD    AX,CX
          MOV    SI,AX
          MOV    DI,_gdwd_y1
          JMP    dline01
dline09:
          pop    bp

          mov    ax,di
          mov    di,[bp+4]
          mov    [di],cx               ; Store 'from' x coordinate of draw
          mov    di,[bp+6]
          mov    [di],bx               ; Store 'from' y coordinate of draw
          mov    di,[bp+8]
          mov    [di],si               ; Store 'to' x coordinate of draw
          mov    di,[bp+10]
          mov    [di],ax               ; Store 'to' y coordinate of draw
done:

          mov    al,_gdc_flg
          xor    ah,ah

          pop    di
          pop    si

          pop    bp
          ret

_gpclip2  endp
_text     ends
          end
