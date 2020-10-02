          page   80,132
          page
;
;         Kent Cedola
;         2015 Meadow Lake Court
;         Norfolk, Virginia  23518
;

_text     segment byte public 'code'

          assume cs:_text
          public _gprdpal
_gprdpal  proc   near

          push   bp
          mov    bp,sp

          xor    ax,ax
          mov    es,ax

          les    bx,dword ptr es:04A8h
          add    bx,4
          les    bx,dword ptr es:[bx]
          mov    ax,es
          or     ax,bx
          jz     error

          mov    AL,[bp+4]
          cmp    al,-1
          jne    inq01
          mov    al,17
inq01:
          xor    ah,ah
          add    bx,ax

          mov    al,es:[bx]
          xor    ah,ah
          jmp    short done
error:
          mov    ax,-1
done:
          pop    bp
          ret

_gprdpal  endp

_text     ends
          end
