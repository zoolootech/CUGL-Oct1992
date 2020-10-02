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
_data     ends

_text     segment byte public 'code'

          assume cs:_text,ds:dgroup
          public _gpslope
_gpslope  proc   near

          push   bp
          mov    bp,sp
          push   si
          push   di

          push   ds                    ; Set ES equal to DS so we can use the
          pop    es                    ;   ... STOSB instruction

          mov    di,[bp+4]             ; Clear the output slope array
          mov    cx,[bp+6]             ;   Load number of bytes in array
          xor    al,al                 ;   Fill with zeros (AL)
          rep stosb                    ;   Fill the slope array with zeros

          mov    di,[bp+4]             ; Load address of the slope array
          mov    bx,[bp+6]             ; Load Delta X (for slope calculations)
          mov    dx,[bp+8]             ; Load Delta Y (for slope calculations)

          cmp    dx,bx
          ja     octant2
octant1:
          mov    cx,bx
          dec    cx
          mov    si,bx                 ; Error Register = -DX/2
          shr    si,1                  ;   ...
          neg    si                    ;   ...
octant1L:
          add    si,dx
          js     octant1E
          sub    si,bx
          inc    al
octant1E:
          stosb
          xor    al,al
          loop   octant1L
          jmp    short return

octant2:
          mov    cx,dx
          dec    cx
          mov    si,dx                 ; Error Register = -DY/2
;         shr    si,1                  ;   ...
          neg    si                    ;   ...
octant2L:
          inc    byte ptr [di]
          add    si,bx
          js     octant2E
          sub    si,dx
          inc    di
octant2E:
          loop   octant2L
          inc    byte ptr [di]
return:
          pop    di
          pop    si
          pop    bp
          ret

_gpslope  endp

_text     ends
          end