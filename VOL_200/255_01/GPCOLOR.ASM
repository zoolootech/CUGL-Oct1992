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

          extrn  _gdcolor:byte,_gds_flg:byte

_data     ends

_text     segment byte public 'code'

          assume cs:_text,ds:dgroup
          public _gpcolor
_gpcolor  proc   near

          push   bp
          mov    bp,sp

          mov    al,[bp+4]
          mov    _gdcolor,al
          mov    _gds_flg,0

          pop    bp
          ret

_gpcolor  endp

_text     ends
          end
