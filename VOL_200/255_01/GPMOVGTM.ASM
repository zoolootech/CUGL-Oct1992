          page   80,132
          page
;
;         Kent Cedola
;         2015 Meadow Lake Ct.
;         Norfolk, Virginia  23518
;
;         gpmovgtm(array,w,h,l);
;

dgroup    group  _data

_data     segment word public 'data'
          assume ds:dgroup

          extrn  _gdcur_x:word,_gdcur_y:word

_data     ends

_TEXT     SEGMENT BYTE PUBLIC 'code'
          assume cs:_text,ds:dgroup

          public _gpmovgtm

_gpmovgtm proc   near

          push   bp
          mov    bp,sp

          push   di
          push   si

          mov    ax,_gdcur_y
          shl    ax,1
          shl    ax,1
          add    ax,_gdcur_y
          add    ax,0A000h
          mov    es,ax

          mov    cx,_gdcur_x           ; Copy X coordinate to a work register
          mov    SI,CX                 ; Copy X corrdinate
          SHR    SI,1                  ; Divide X corrdinate by three and
          SHR    SI,1                  ;   ...
          SHR    SI,1                  ;   ...
          AND    CL,7                  ; Determine the starting bit to plot

          mov    di,[bp+4]

          mov    dx,03CEh
          mov    al,4
          out    dx,al
          inc    dx

; four plane code
;         SI    = address of first byte
;         DX    = address of memory read port (all set up)
;         DI    = address of output array
;         CL    = bit rotate
;         ES    = graphic segment

nextcol4:
          push   si
          push   cx
          push   di
          mov    ax,[bp+6]
          mov    [bp+4],ax
nextrow4:
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

          dec    word ptr [bp+4]
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
          pop    cx
          pop    si
          add    di,[bp+10]
          add    si,80

          dec    word ptr [bp+8]
          jz     colend4
          jmp    nextcol4

colend4:
          mov    al,0
          out    dx,al

          pop    si
          pop    di

          pop    bp
          ret                          ; Return to caller

_gpmovgtm endp

_text     ends
          END
