          page   80,132
          page
;
;         Kent Cedola
;         2015 Meadow Lake Court
;         Norfolk, Virginia  23518
;

_text     segment byte public 'code'

          assume cs:_text
          public _gpldpal
_gpldpal  proc   near

          push   bp
          mov    bp,sp

          push   ds
          pop    es

          mov    ax,1002h
          mov    dx,[bp+4]
          INT    10h

          pop    bp
          ret
_gpldpal  endp

_text     ends
          end
