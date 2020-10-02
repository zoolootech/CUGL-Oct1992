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

          extrn  _xdwritbp:word
          extrn  _xdwritw1:word,_xdwritw2:word,_xdwritw3:word
          extrn  _xdwritb1:byte

_data     ends

_text     segment byte public 'code'

          assume cs:_text,ds:dgroup
          public _gpwritrw

_gpwritrw proc   near

          push   si
          push   di

          mov    si,_xdwritbp
          mov    di,0
          mov    es,_xdwritw1
          mov    cx,_xdwritw3

          mov    dx,03CEh              ; Load graphic controller's address port
          mov    al,8                  ; Mask value is in address eight (8)
          out    dx,al                 ; Point to the mask register
          inc    dx                    ; Bump port addr to the mask register

          mov    ah,_xdwritb1

          mov    al,es:[di]
nextbit:
          mov    al,ah
          out    dx,al
          lodsb
          xchg   es:[di],al
          ror    ah,1
          adc    di,0
          loop   nextbit

          mov    ax,_xdwritw2
          add    _xdwritw1,ax

          pop    di
          pop    si
          ret

_gpwritrw endp

_text     ends
          END
