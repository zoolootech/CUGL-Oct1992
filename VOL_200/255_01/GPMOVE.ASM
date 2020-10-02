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

          extrn  _gdcur_x:word,_gdcur_y:word

_data     ends

_text     segment byte public 'code'

          assume cs:_text,ds:dgroup
          public _gpmove
_gpmove   proc   near

          push   bp
          mov    bp,sp

          mov    ax,[bp+4]
          mov    _gdcur_x,ax
          mov    ax,[bp+6]
          mov    _gdcur_y,ax

          pop    bp
          ret

_gpmove   endp

_text     ends
          end
