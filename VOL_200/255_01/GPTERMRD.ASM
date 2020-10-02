          page   80,132
          page
;
;       EGA Graphic Primitive for Microsoft C 3.0, Version 01APR86.
;       (C) 1986 by Kent Cedola, 2015 Meadow Lake Ct., Norfolk, VA, 23518
;

dgroup    group  _data

_data     segment word public 'data'
          assume ds:dgroup

_data     ends

_text     segment byte public 'code'

          assume cs:_text,ds:dgroup
          public _gptermrd

_gptermrd proc   near

          ret

_gptermrd endp

_text     ends
          end
