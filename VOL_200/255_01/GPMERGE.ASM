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

          extrn  _gdmerge:byte

_data     ends

_text     segment byte public 'code'

          assume cs:_text,ds:dgroup
          public _gpmerge
_gpmerge  proc   near

          push   bp
          mov    bp,sp

          mov    al,[bp+4]
          and    al,3
          shl    al,1
          shl    al,1
          shl    al,1
          mov    _gdmerge,al

          pop    bp
          ret

_gpmerge  endp

_text     ends
          end
