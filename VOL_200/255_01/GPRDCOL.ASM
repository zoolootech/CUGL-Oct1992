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

          extrn  _gdmerge:byte
          extrn  _gdcur_x:word,_gdcur_y:word
          extrn  _gdvw_x1:word,_gdvw_x2:word,_gdvw_x3:word
          extrn  _gdvw_y1:word,_gdvw_y2:word,_gdvw_y3:word
          extrn  _gdc_flg:byte

_data     ends

_text     segment byte public 'code'

          assume cs:_text,ds:dgroup
          public _gprdcol
_gprdcol  proc   near

          push   bp
          mov    bp,sp
          push   si
          push   di

          mov    ax,_GDCUR_Y           ; Compute the segment of the graphic
          shl    ax,1                  ;   byte that is to be changed
          shl    ax,1                  ;   ES = A000 + (80 * Y) / 16;
          add    ax,_GDCUR_Y           ;   ...
          add    ax,0A000h             ;   ...
          mov    es,ax                 ;   ...
          mov    si,_GDCUR_X           ; Compute the column byte offset
          mov    cx,si                 ;   ... (Save for later)
          shr    si,1                  ;   SI = X / 8;
          shr    si,1                  ;   ...
          shr    si,1                  ;   ...
          and    cl,7                  ;   ...  (It has to be done this way to
          mov    ah,080h
          ror    ah,cl

          mov    cx,[bp+6]
          mov    di,[bp+4]

          mov    dx,03CEh
          mov    al,4
          out    dx,al
          inc    dx


; four plane code
;         SI    = address of first byte
;         DX    = address of memory read port (all set up)
;         DI    = address of output array
;         CX    = count
;         ES    = graphic segment

colnext:
          xor    bl,bl

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
          mov    [di],bl
          inc    di
          add    si,80
          loop   colnext

colend4:
          pop    di
          pop    si

          pop    bp
          ret
_gprdcol  endp

_text     ends
          END
