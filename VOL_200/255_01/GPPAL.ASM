          page   80,132
          page
;
;         Kent Cedola
;         2015 Meadow Lake Court
;         Norfolk, Virginia  23518
;

_text     segment byte public 'code'

          public _gppal

          ASSUME CS:_TEXT

_gppal    proc   near

          push   bp
          mov    bp,sp

          mov    Bl,[bp+4]
          mov    Bh,[bp+6]
          mov    AX,1000h
          or     BL,BL
          jns    L1
          mov    AL,01
L1:
          INT    10h

          pop    bp
          ret

_gppal    endp

_TEXT     ends
          END
