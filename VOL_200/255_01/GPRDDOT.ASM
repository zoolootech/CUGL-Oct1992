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
          public _gprddot
_gprddot  proc   near

          push   bp
          mov    bp,sp
          push   si

          mov    ax,[bp+6]             ; Compute the segment of the graphic
          mov    _GDCUR_Y,ax           ;
          shl    ax,1                  ;   byte that is to be changed
          shl    ax,1                  ;   ES = A000 + (80 * Y) / 16;
          add    ax,_GDCUR_Y           ;   ...
          add    ax,0A000h             ;   ...
          mov    es,ax                 ;   ...
          mov    si,[bp+4]             ; Compute the column byte offset
          mov    _GDCUR_X,si
          mov    cx,si                 ;   ... (Save for later)
          shr    si,1                  ;   SI = X / 8;
          shr    si,1                  ;   ...
          shr    si,1                  ;   ...
          and    cl,7                  ;   ...  (It has to be done this way to
          mov    ah,80h
          ror    ah,cl

          mov    dx,03CEh
          mov    al,4
          out    dx,al
          inc    dx

          xor    bx,bx

          mov    al,0
          out    dx,al
          test   es:[si],ah
          jz     bit1
          or     bl,1
bit1:
          mov    al,1
          out    dx,al
          test   es:[si],ah
          jz     bit2
          or     bl,2
bit2:
          mov    al,2
          out    dx,al
          test   es:[si],ah
          jz     bit3
          or     bl,4
bit3:
          mov    al,3
          out    dx,al
          test   es:[si],ah
          jz     bit4
          or     bl,8
bit4:
          mov    ax,bx

          pop    si
          pop    bp
          ret

_gprddot  endp

_text     ends
          end
