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

          extrn  _gdtype:byte
          extrn  _gdmaxcol:word,_gdmaxrow:word
          extrn  _gdcolor:byte,_gdmerge:byte,_gdaspc1:word,_gdaspc2:word
          extrn  _gdcur_x:word,_gdcur_y:word
          extrn  _gdwd_x1:word,_gdwd_x2:word,_gdwd_x3:word
          extrn  _gdwd_y1:word,_gdwd_y2:word,_gdwd_y3:word
          extrn  _gdvw_x1:word,_gdvw_x2:word,_gdvw_x3:word
          extrn  _gdvw_y1:word,_gdvw_y2:word,_gdvw_y3:word
          extrn  _gdc_flg:byte,_gds_flg:byte,_gdw_flg:byte

_data     ends

_text     segment byte public 'code'

          assume cs:_text,ds:dgroup
          public _gpinit
_gpinit   proc   near

          push   bp
          mov    bp,sp

          mov    al,_gdtype
          cmp    al,3
          je     cg
          cmp    al,4
          je     mono
          cmp    al,5
          je     ecd
          jmp    short done
cg:
          mov    ax,3
          mov    _gdaspc1,ax
          dec    ax
          mov    _gdaspc2,ax
          mov    al,0Eh
          jmp    short set
mono:
          mov    al,0Fh
          jmp    short set
ecd:
          mov    ax,3
          mov    _gdaspc1,ax
          inc    ax
          mov    _gdaspc2,ax
          mov    al,10h
set:
          xor    ah,ah
          int    10h
done:
          xor    ax,ax
          mov    _gdcolor,al
          mov    _gdmerge,al
          mov    _gdcur_x,ax
          mov    _gdcur_y,ax
          mov    _gdwd_x1,ax
          mov    _gdwd_y1,ax
          mov    _gdvw_x1,ax
          mov    _gdvw_y1,ax
          mov    _gdc_flg,al
          mov    _gds_flg,al
          mov    _gdw_flg,al

          mov    ax,_gdmaxcol
          mov    _gdwd_x2,ax
          mov    _gdvw_x2,ax
          inc    ax
          mov    _gdwd_x3,ax
          mov    _gdvw_x3,ax
          mov    ax,_gdmaxrow
          mov    _gdwd_y2,ax
          mov    _gdvw_y2,ax
          inc    ax
          mov    _gdwd_y3,ax
          mov    _gdvw_y3,ax

          cmp    _gdtype,3

          pop    bp
          ret

_gpinit   endp

_text     ends
          end
