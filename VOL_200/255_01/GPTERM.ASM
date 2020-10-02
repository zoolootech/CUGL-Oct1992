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

_data     ends

_text     segment byte public 'code'

          assume cs:_text,ds:dgroup
          public _gpterm
_gpterm   proc   near

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
ecd:
          mov    al,3
          jmp    short set
mono:
          mov    al,7
set:
          xor    ah,ah
          int    10h
done:
          pop    bp
          ret

_gpterm   endp

_text     ends
          end
