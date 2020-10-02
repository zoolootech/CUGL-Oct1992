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
          public _gpwtcol
_gpwtcol  proc   near

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
          mov    di,_GDCUR_X           ; Compute the column byte offset
          mov    cx,di                 ;   ... (Save for later)
          shr    di,1                  ;   DI = X / 8;
          shr    di,1                  ;   ...
          shr    di,1                  ;   ...
          mov    dx,03CEh              ; Load graphic controller's address port
          mov    ah,_GDMERGE
          mov    al,3
          out    dx,ax
          mov    ax,0205h              ; Mask value is in address three (3)
          out    dx,ax                 ; Point to the mask register
          mov    al,8                  ; Mask value is in address eight (8)
          out    dx,al                 ; Point to the mask register
          inc    dx                    ; Bump port addr to the mask register
          mov    al,80h                ; Compute mask byte to change one bit
          and    cl,7                  ;   ...  (It has to be done this way to
          ror    al,cl                 ;   ...   use merge value in write)
          out    dx,al

          mov    cx,[bp+6]
          mov    si,[bp+4]
nextbit:
          mov    ah,[si]
          inc    si
          mov    bl,es:[di]
          mov    es:[di],ah            ; Apply merge function to bit and store
          add    di,80
          loop   nextbit

          mov    al,0FFh
          out    dx,al                 ; Restore the mask byte (for text)
          dec    dx
          mov    ax,0005h
          out    dx,ax
          mov    ax,0003h
          out    dx,ax

          pop    di
          pop    si
          pop    bp
          ret

_gpwtcol  endp

_text     ends
          end
