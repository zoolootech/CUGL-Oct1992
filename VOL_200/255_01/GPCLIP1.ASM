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
          public _gpclip1
_gpclip1  proc   near

          push   bp
          mov    bp,sp

          push   si
          push   di

          mov    si,[bp+4]
          mov    di,[bp+6]

          mov    cx,[si]
          mov    bx,[di]

          cmp    cx,_gdwd_x1           ; If the scale (real) X,Y coordinates
          jb     outside               ;   ... are not in the current viewport
          cmp    cx,_gdwd_x2           ;   ... and the clipping mode is on
          ja     outside               ;   ... ignore this plot
          cmp    bx,_gdwd_y1           ;   ...
          jb     outside               ;   ...
          cmp    bx,_gdwd_y2           ;   ...
          ja     outside               ;   ...

          MOV    AX,CX                 ; Copy X coordinate to a work register
          SUB    AX,_gdwd_x1           ; Substract the Window's X offset
          MUL    _gdvw_x3              ; Multiply by the Viewport's X delta
          DIV    _gdwd_x3              ; Divide by the Window's X offset
          ADD    AX,_gdvw_x1           ; Add the Viewport's X offset
          MOV    [si],AX               ; Copy the converted X back to CX

          MOV    AX,BX                 ; Copy Y coordinate to a work register
          SUB    AX,_gdwd_y1           ; Substract the Window's Y offset
          MUL    _gdvw_y3              ; Multiply by the Viewport's Y delta
          DIV    _gdwd_y3              ; Divide by the Window's Y offset
          ADD    AX,_gdvw_y1           ; Add the Viewport's Y offset
          MOV    [di],AX               ; Copy the converted Y back to CX
          xor    al,al
          jmp    short done            ;
outside:
          mov    al,-1
done:
          mov    _gdc_flg,al
          xor    ah,ah

          pop    di
          pop    si

          pop    bp
          ret
_gpclip1  endp

_text     ends
          end
