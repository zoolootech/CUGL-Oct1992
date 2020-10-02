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
          public _gdcolor,_gdgseg

_gdcolor  db     0
_gdgseg   dw     0A000h

_data     ends

_text     segment byte public 'code'

          assume cs:_text,ds:dgroup
          public _egacolor,_egaline,_egaplot,_egafillrect

_egacolor proc   near
          push   bp
          mov    bp,sp

          mov    al,[bp+4]
          mov    _gdcolor,al

          pop    bp
          ret
_egacolor endp

_egaline  proc   near

          push   bp
          mov    bp,sp
          push   si
          push   di

          call   ega_init

          mov    cx,[bp+4]             ; Load starting X coordinate (X1)
          mov    bx,[bp+6]             ; Load starting Y coordinate (Y1)

          cmp    cx,[bp+8]             ; Is X1 <= X2?
          jbe    noxchg                ;   Yes, continue
          xchg   [bp+8],cx             ;   No, then switch (X1,Y1) with (X2,Y2)
          xchg   [bp+10],bx            ;     ...
          mov    [bp+4],cx
          mov    [bp+6],bx
noxchg:
          call   bit_addr

          mov    ah,_gdcolor

          mov    bx,[bp+8]             ; Load ending X coordinate (X2)
          mov    di,[bp+10]            ; Load ending Y coordinate (Y2)

          sub    bx,[bp+4]             ; Compute Delta X := X2 - X1;
          sub    di,[bp+6]             ; Compute Delta Y := Y2 - Y1;

          push   bp
          or     di,di
          jns    line01
          neg    di
          cmp    di,bx
          ja     lineoct7
          jmp    short lineoct8
line01:
          cmp    di,bx
          ja     lineoct2

lineoct1:
          mov    cx,bx                 ; Count Down = DX
          mov    bp,bx                 ; Error Register = -DX/2
          shr    bp,1                  ;   ...
          neg    bp                    ;   ...
lineoct1L:
          out    dx,al
          cmp    byte ptr es:[si],0
          mov    es:[si],ah
          dec    cx
          js     linedone
          ror    al,1
          adc    si,0
          add    bp,di
          js     lineoct1L
          add    si,80
          sub    bp,bx
          jmp    short lineoct1L

lineoct2:
          mov    cx,di                 ; Count Down = DY
          mov    bp,di                 ; Error Register = -DY/2
          shr    bp,1                  ;   ...
          neg    bp                    ;   ...
lineoct2L:
          out    dx,al
          cmp    byte ptr es:[si],0
          mov    es:[si],ah
          dec    cx
          js     linedone
          add    si,80
          add    bp,bx
          js     lineoct2L
          ror    al,1
          adc    si,0
          sub    bp,di
          jmp    short lineoct2L

lineoct7:
          mov    cx,di                 ; Count Down = -DY
          mov    bp,di                 ; Error Register = DY/2
          shr    bp,1                  ;   ...
          neg    bp                    ;
lineoct7L:
          out    dx,al
          cmp    byte ptr es:[si],0
          mov    es:[si],ah
          dec    cx
          js     linedone
          sub    si,80
          add    bp,bx
          js     lineoct7L
          ror    al,1
          adc    si,0
          sub    bp,di
          jmp    short lineoct7L

lineoct8:
          mov    cx,bx                 ; Count Down = DX
          mov    bp,bx                 ; Error Register = -DX/2
          shr    bp,1                  ;   ...
          neg    bp                    ;   ...
lineoct8L:
          out    dx,al
          cmp    byte ptr es:[si],0
          mov    es:[si],ah
          dec    cx
          js     linedone
          ror    al,1
          adc    si,0
          add    bp,di
          js     lineoct8L
          sub    si,80
          sub    bp,bx
          jmp    short lineoct8L

linedone:
          pop    bp

          call   ega_term

          pop    di
          pop    si
          pop    bp
          ret

_egaline  endp

_egaplot  proc   near

          push   bp
          mov    bp,sp
          push   si
          push   di

          call   ega_init

          mov    cx,[bp+4]
          mov    bx,[bp+6]

          call   bit_addr

          out    dx,al                 ; Set mask byte to change only our bit
          mov    al,es:[si]
          mov    al,_gdcolor
          mov    es:[si],al            ; Apply merge function to bit and store

          call   ega_term

          pop    di
          pop    si
          pop    bp
          ret
_egaplot  endp

_egafillrect proc near

          push   bp
          mov    bp,sp
          push   si
          push   di

          call   ega_init

          mov    ax,[bp+4]             ; Load X1 coordinate of current frame
          mov    bx,[bp+6]             ; Load Y1 coordinate of current frame
          mov    di,[bp+8]             ; Load X2 coordinate of current frame
          mov    si,[bp+10]            ; Load Y2 coordinate

          cmp    ax,di
          jbe    fill01
          xchg   ax,di
fill01:
          cmp    bx,si
          jbe    fill02
          xchg   bx,si
fill02:

          MOV    cx,ax                 ; Determine the first mask byte
          AND    cl,7                  ;   ...
          mov    dl,0FFh
          shr    dl,cl
          mov    [bp+4],dl

          MOV    cx,DI                 ; Determine the second (last) mask byte
          AND    cl,7                  ;   ...
          mov    dl,080h
          sar    dl,cl
          mov    [bp+6],dl

          mov    cx,si                 ; Load Y2 corrdinate of current frame

          MOV    DX,AX                 ; Determine width in bytes
          SHR    DX,1                  ; Convert X1 to a byte count
          SHR    DX,1                  ;   ...
          SHR    DX,1                  ;   ...
          SHR    DI,1                  ; Convert X2 to a byte count
          SHR    DI,1                  ;   ...
          SHR    DI,1                  ;   ...
          SUB    DI,DX                 ; Width + 1
          JNZ    clear01
          PUSH   AX
          MOV    AL,[bp+6]
          AND    AL,[bp+4]
          MOV    [bp+8],AL
          POP    AX
clear01:
          DEC    DI                    ; Width
          SUB    CX,BX                 ; Determine the Height of the frame
          INC    CX                    ;   ...

          MOV    SI,AX                 ;
          SHR    SI,1                  ; Divide X corrdinate by eight and
          SHR    SI,1                  ;   ...
          SHR    SI,1                  ;   ...
          mov    ax,bx
          shl    ax,1
          shl    ax,1
          add    ax,bx
          add    ax,0A000h
          mov    es,ax

;         CX     Height
;         SI     Byte Offset
;         DI     Width
;         ES     'A000'

          mov    dx,03CEh
          mov    al,8
          out    dx,al
          inc    dx

          MOV    AH,_gdcolor

          MOV    AL,[bp+4]
          OUT    DX,AL
          CALL   col_fill
          OR     DI,DI
          JS     L3
          JZ     L2
          MOV    AL,0FFh
          OUT    DX,AL
L6:
          CALL   col_fill
          DEC    DI
          JNZ    L6
L2:
          MOV    AL,[bp+6]
          OUT    DX,AL
          CALL   col_fill
L3:
          jmp    short done

;         AH     Color
;         CX     Height
;         DX     '03CF'
;         SI     Byte Offset
;         ES     'A000'

col_fill:
          PUSH   CX
          PUSH   SI
col_f01:
          MOV    AL,ES:[SI]
          MOV    ES:[SI],AH
          ADD    SI,80
          LOOP   col_f01

          POP    SI
          POP    CX
          INC    SI
          RET

done:
          call   ega_term

          pop    di
          pop    si
          pop    bp
          ret

_egafillrect endp

ega_init  proc   near

          mov    dx,03CEh              ; Load address of EGA graphic controller
          mov    al,_gdcolor           ; Load current color setting
          or     al,al                 ; Is the merge (xor) bit set?
          jns    nomerge               ;   No, jump over merge code
          mov    ax,01803h             ;   Yes, load xor merge setting
          out    dx,ax                 ;     Set the EGA hardware register
nomerge:
          mov    ax,00205h             ; Load parameters for write mode #2
          out    dx,ax                 ; Set the graphic controller write mode

          mov    al,08h                ;
          out    dx,al                 ;
          inc    dx                    ;

          mov    es,_gdgseg

          ret
ega_init  endp

ega_term  proc   near

          mov    dx,03CEh
          mov    ax,00003h
          out    dx,ax
          mov    al,005h
          out    dx,ax
          mov    ax,0FF08h
          out    dx,ax

          ret

ega_term  endp


;         CX,BX  (X,Y)

bit_addr  proc   near

          mov    si,bx                 ; Compute SI = Y * 80 for row offset
          shl    si,1                  ;   ... (SI = Y * 2)
          shl    si,1                  ;   ... (SI = Y * 4)
          add    si,bx                 ;   ... (SI = Y * 5)
          shl    si,1                  ;   ... (SI = Y * 10)
          shl    si,1                  ;   ... (SI = Y * 20)
          shl    si,1                  ;   ... (SI = Y * 40)
          shl    si,1                  ;   ... (SI = Y * 80)
          mov    bx,cx                 ; Compute BX = X / 8 for column offset
          shr    bx,1                  ;   ... (BX = X / 2)
          shr    bx,1                  ;   ... (BX = X / 4)
          shr    bx,1                  ;   ... (BX = X / 8)
          add    si,bx                 ; Compute BX = BX + SI, first byte

          and    cl,7                  ; Compute AL = starting bit mask
          mov    al,080h               ;   ... load default of bit 0
          ror    al,cl                 ;   ... rotate to the starting bit mask

          ret
bit_addr  endp

_text     ends
          end