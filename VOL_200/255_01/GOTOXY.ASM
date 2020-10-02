          page   80,132
          page

;  Author
;
;    Kent Cedola
;    2015 Meadow Lake Court
;    Norfolk, Virginia 23518
;

_TEXT     SEGMENT BYTE PUBLIC 'CODE'

          public _gotoxy

          ASSUME CS:_TEXT

_gotoxy   proc   near

          push   bp                    ; Save C frame pointer
          mov    bp,sp                 ; Create new frame containing parameters

          mov    ah,0Fh
          int    10h

          mov    dl,[bp+4]             ; Load specified attribute
          mov    dh,[bp+6]             ; Load specified attribute

          mov    ah,2
          int    10h                   ; <<< BIOS >>> - Video

          pop    bp                    ; Restore C frame pointer
          ret                          ; Return to caller

_gotoxy   endp

_TEXT     ends
          end
