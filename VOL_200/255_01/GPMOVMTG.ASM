          page   80,132
          page

          title  'Enhance Graphic Adapter Function: WTPIXELS()'

;   Initialize graphic software for 640x350 color/mono graphics.
;
;         Kent Cedola
;         2015 Meadow Lake Ct.
;         Norfolk, Virginia  23518
;

dgroup    group  _data

_data     segment word public 'data'
          assume ds:dgroup

          extrn  _gdcur_x:word,_gdcur_y:word

_data     ends

_text     SEGMENT BYTE PUBLIC 'CODE'
          assume cs:_text,ds:dgroup

          public _gpmovmtg

_gpmovmtg proc   near

          push   bp
          mov    bp,sp
          push   si
          push   di

          mov    dx,03CEh
          mov    ax,0205h
          out    dx,ax
          mov    al,8
          out    dx,al
          inc    dx

          mov    ax,_gdcur_y
          shl    ax,1
          shl    ax,1
          add    ax,_gdcur_y
          add    ax,0A000h
          mov    es,ax

          mov    cx,_gdcur_x           ; Copy X coordinate to a work register
          MOV    di,cx                 ; Copy X corrdinate
          SHR    DI,1                  ; Divide X corrdinate by three and
          SHR    DI,1                  ;   ...
          SHR    DI,1                  ;   ...

          MOV    AL,80h                ; Determine the starting bit to plot
          AND    CL,7                  ;   ...
          ROR    AL,CL                 ;   ...

          mov    si,[bp+4]
          mov    cx,[bp+6]
          mov    bx,[bp+8]
nextrow:
          push   di
          push   si
          push   cx
          push   ax
nextbit:
          OUT    DX,AL                 ; Set mask bit
          MOV    AH,ES:[DI]            ; Read graphic byte into latches
          mov    ah,[si]
          inc    si
          MOV    ES:[DI],AH            ; Apply merge function to bit and store
          ror    al,1
          adc    di,0
          loop   nextbit

          pop    ax
          pop    cx
          pop    si
          pop    di

          add    si,[bp+10]
          add    di,80
          dec    bx
          jnz    nextrow

          mov    al,0FFh
          out    dx,al
          dec    dx
          mov    ax,5
          out    dx,ax
          mov    al,3
          out    dx,ax

          pop    di
          pop    si
          pop    bp
          ret                          ; Return to caller

_gpmovmtg endp

_text     ends
          end
