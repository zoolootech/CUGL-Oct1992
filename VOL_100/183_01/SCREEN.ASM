Comment  @

        This assembler routine is used with the Microsoft C 3.0 compiler
      to do video calls.



        Name  cls -- clear screen

        Synopsis  cls();

        Descrpt   clear the screen from 0,0 to 79, 24



        Name      cpos -- positions the cursor

        Synopsis  cpos(row, col);
                  int row;
                  int col;

        Descrpt   positions the cursor at row, col.
                  must be between 1,1 and 80, 25

@


        assume  cs:_text
_text   segment public byte 'code'
        public  _cls, _cpos

_cls    proc    near
        push    bp
        mov     bp,sp

        mov     ax,0600h
        mov     cx,0000
        mov     dx,184fh
        mov     bh,07
        int     10h

        mov     ah,02
        mov     dx,0000
        mov     bh,00
        int     10h

        pop     bp
        ret
_cls    endp

_cpos   proc    near
        push    bp
        mov     bp,sp

        mov     dx,[bp + 4]
        mov     ax,[bp + 6]
        pop     bp

        mov     dh,dl
        mov     dl,al
        sub     dh,01
        sub     dl,01
        mov     ah,02
        mov     bh,00
        int     10h
        ret
_cpos   endp

_text   ends
        end
