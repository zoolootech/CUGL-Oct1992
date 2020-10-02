
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

          extrn  _gdcolor:byte,_gdmerge:byte
          extrn  _gdcur_x:word,_gdcur_y:word
          extrn  _gdvw_x1:word,_gdvw_x2:word,_gdvw_x3:word
          extrn  _gdvw_y1:word,_gdvw_y2:word,_gdvw_y3:word
          extrn  _gdc_flg:byte

_data     ends

_text     segment byte public 'code'

          assume cs:_text,ds:dgroup
          public _gpbox
_gpbox    proc   near

          push   bp
          mov    bp,sp
          push   si
          push   di

          mov    dx,03CEh
          mov    ah,_gdmerge
          mov    al,03h
          out    dx,ax
          mov    ax,00205h
          out    dx,ax

          mov    ax,_gdcur_x           ; Load X1 coordinate of current frame
          mov    bx,_gdcur_y           ; Load Y1 coordinate of current frame
          mov    di,[bp+4]             ; Load X2 coordinate of current frame
          mov    si,[bp+6]             ; Load Y2 coordinate

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
          mov    al,0ffh
          out    dx,al
          dec    dx
          mov    ax,0003
          out    dx,ax
          mov    ax,0005
          out    dx,ax

          pop    di
          pop    si
          pop    bp
          ret

_gpbox    endp
_text     ends
          end
