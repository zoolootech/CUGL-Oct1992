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
          public _gpscale
_gpscale  proc   near

          push   bp
          mov    bp,sp

          push   si

          mov    si,[bp+4]
          MOV    AX,[si]               ; Copy X coordinate to a work register
          SUB    AX,_gdwd_x1           ; Substract the Window's X offset
          IMUL   _gdvw_x3              ; Multiply by the Viewport's X delta
          IDIV   _gdwd_x3              ; Divide by the Window's X offset
          ADD    AX,_gdvw_x1           ; Add the Viewport's X offset
          MOV    [si],ax               ; Copy the converted X back

          mov    si,[bp+6]
          MOV    AX,[si]               ; Copy Y coordinate to a work register
          SUB    AX,_gdwd_y1           ; Substract the Window's Y offset
          IMUL   _gdvw_y3              ; Multiply by the Viewport's Y delta
          IDIV   _gdwd_y3              ; Divide by the Window's Y offset
          ADD    AX,_gdvw_y1           ; Add the Viewport's Y offset
          MOV    [si],ax               ; Copy the converted Y back

          pop    si
          pop    bp
          ret

_gpscale  endp
_text     ends
          end
