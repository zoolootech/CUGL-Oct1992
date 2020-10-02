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

          mov    si,0
          mov    di,_xdwritbp
          mov    es,_xdwritw1
          mov    bp,_xdwritw3
          mov    cl,_xdwritb1

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

          mov    ax,_xdwritw2
          add    _xdwritw1,ax

          pop    di
          pop    si
          ret

_gpwritrw endp

_text     ends
          END
