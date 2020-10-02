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

          extrn  _gdcolor:byte

_data     ends

_text     segment byte public 'code'

          assume cs:_text,ds:dgroup
          public _gprintf
_gprintf  proc   near

          push   bp
          mov    bp,sp
          push   si
          push   di

          mov    si,[bp+4]
          lea    bx,[bp+6]

          mov    ah,0Fh
          int    10h
          mov    ah,03h
          int    10h

          mov    ah,0Eh
          mov    bl,_gdcolor

nxtchr:
          lodsb
          or     al,al
          jz     done
          int    10h
          jmp    short nxtchr
done:
          pop    di
          pop    si
          pop    bp
          ret

_gprintf  endp

_text     ends
          end
