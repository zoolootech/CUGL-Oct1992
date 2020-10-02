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
          public _gprdrow
_gprdrow  proc   near

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

          mov    bx,[bp+6]
          mov    di,[bp+4]
          mov    bp,bx

          mov    dx,03CEh
          mov    al,4
          out    dx,al
          inc    dx

; four plane code
;         SI    = address of first byte
;         DX    = address of memory read port (all set up)
;         DI    = address of output array
;         BP    = width
;         CL    = bit rotate
;         ES    = graphic segment

          mov    al,0
          out    dx,al
          mov    ah,es:[si]
          rol    ah,cl
          mov    al,1
          out    dx,al
          mov    bh,es:[si]
          rol    bh,cl
          mov    al,2
          out    dx,al
          mov    bl,es:[si]
          rol    bl,cl
          mov    al,3
          out    dx,al
          mov    ch,es:[si]
          rol    ch,cl

          sub    cl,8
          neg    cl

nextbit4:
          xor    al,al

          rol    ch,1
          adc    al,0
          rol    al,1
          rol    bl,1
          adc    al,0
          rol    al,1
          rol    bh,1
          adc    al,0
          rol    al,1
          rol    ah,1
          adc    al,0

          mov    [di],al
          inc    di

          dec    bp
          jz     rowend4

          dec    cl
          jnz    nextbit4
          inc    si
          mov    al,0
          out    dx,al
          mov    ah,es:[si]
          mov    al,1
          out    dx,al
          mov    bh,es:[si]
          mov    al,2
          out    dx,al
          mov    bl,es:[si]
          mov    al,3
          out    dx,al
          mov    ch,es:[si]

          mov    cl,8

          jmp    short nextbit4

rowend4:
          pop    di
          pop    si
          pop    bp
          ret                          ; Return to caller

_gprdrow  endp

_text     ends
          end
