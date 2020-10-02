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

          extrn  _gdvw_x1:word,_gdvw_x2:word,_gdvw_x3:word
          extrn  _gdvw_y1:word,_gdvw_y2:word,_gdvw_y3:word

_data     ends

_text     segment byte public 'code'

          assume cs:_text,ds:dgroup
          public _gpviewport
_gpviewport proc near

          push   bp
          mov    bp,sp

          mov    ax,[bp+4]
          mov    bx,[bp+8]
          cmp    ax,bx
          jle    view01
          xchg   ax,bx
view01:
          mov    _gdvw_x1,ax
          mov    _gdvw_x2,bx
          sub    bx,ax
          inc    bx
          mov    _gdvw_x3,bx

          mov    ax,[bp+6]
          mov    bx,[bp+10]
          cmp    ax,bx
          jle    view02
          xchg   ax,bx
view02:
          mov    _gdvw_y1,ax
          mov    _gdvw_y2,bx
          sub    bx,ax
          inc    bx
          mov    _gdvw_y3,bx

          pop    bp
          ret

_gpviewport endp

_text     ends
          end
