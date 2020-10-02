          page   80,132
          page
;
;         Kent Cedola
;         2015 Meadow Lake Court
;         Norfolk, Virginia  23518
;

color     equ    4

tmpw1     equ    -2
tmpw2     equ    -4
tmpw3     equ    -6
tmpw4     equ    -8
tmpw5     equ    -10
tmpw6     equ    -12
tmpw7     equ    -14
tmpw8     equ    -16
tmpw9     equ    -18
tmpwA     equ    -20

needed    equ    22

dgroup    group  _data

_data     segment word public 'data'
          assume ds:dgroup

          extrn  _gdtype:byte
          extrn  _gdmaxcol:word,_gdmaxrow:word
          extrn  _gdcolor:byte,_gdmerge:byte,_gdaspc1:word,_gdaspc2:word
          extrn  _gdcur_x:word,_gdcur_y:word
          extrn  _gdwd_x1:word,_gdwd_x2:word,_gdwd_x3:word
          extrn  _gdwd_y1:word,_gdwd_y2:word,_gdwd_y3:word
          extrn  _gdvw_x1:word,_gdvw_x2:word,_gdvw_x3:word
          extrn  _gdvw_y1:word,_gdvw_y2:word,_gdvw_y3:word
          extrn  _gdgsadr:word,_gdshad1:byte
          extrn  _gdc_flg:byte,_gds_flg:byte,_gdw_flg:byte

_data     ends

_text     segment byte public 'code'

          assume cs:_text,ds:dgroup
          public _gpflood
_gpflood  proc   near

          push   bp                    ; Save caller's base frame pointer
          mov    bp,sp                 ; Setup of new frame with parameters
          sub    sp,needed             ; Get some room for our variables

          push   si
          push   di

          mov    cx,_GDCUR_X           ; Load X corrdinate
          mov    bx,_GDCUR_Y           ; Load Y corrdinate

;      If the starting coordinate is not in the current viewport then return
;  boundry error (GDC_FLG = 2).

          CMP    CX,_GDVW_X1           ; Is the pixel below our minimum?
          JL     nopoint               ;   Yes, return with a error
          CMP    CX,_GDVW_X2           ; Is the pixel above our maximum?
          JG     nopoint               ;   Yes, return with a error
          CMP    BX,_GDVW_Y1           ; Is the pixel below our minimum?
          JL     nopoint               ;   Yes, return with a error
          CMP    BX,_GDVW_Y2           ; Is the pixel above our maximum?
          JLE    egainit               ;   Yes, return with a error
nopoint:
          mov    _GDC_FLG,2            ;
          jmp    done

egainit:
          mov    ax,0A000h
          mov    es,ax

          mov    dx,03CEh
          mov    al,02
          mov    ah,[BP+color]         ; Color
          out    dx,ax
          mov    al,3
          mov    ah,_GDMERGE
          out    dx,ax
          mov    ax,0A05h
          out    dx,ax
          mov    al,08
          out    dx,al
          inc    dx

          XOR    AX,AX
          PUSH   AX
          PUSH   CX
          PUSH   CX
          PUSH   BX
          MOV    AH,1
          PUSH   AX

;      Fill in each specified horizontal line using the current shading and
;  color values.  Then scan the rows above and below the row just filled in.
;
;         SP +0  option bits
;         SP +2  vertical bit corrdinate (Y)
;         SP +4  horizontal bit corrdinate (X)
;         SP +6  horizontal bit corrdinate (X')

loop:
          mov    si,bp                 ; Save BP in holding register
scan:

;         MOV    AH,8
;         INT    21h
;         CMP    AL,'*'
;         JNE    bug1
;         INC    color
;ug1:
;         CMP    AL,'$'
;         JNE    debug
;         MOV    AX,3
;         INT    10h
;         NOP
;ebug:
          MOV    BX,SP
          CMP    Word Ptr ss:[BX+0],0
          JNE    scan01
          POP    AX
          mov    bp,si
          jmp    done
scan01:
          MOV    BP,BX
scan02:
          ADD    BP,8
          MOV    AX,[BP+0]
          OR     AX,AX
          JNZ    scan02A
          JMP    scan09
scan02A:
          MOV    AX,[BP+2]
          CMP    AX,ss:[BX+2]
          JNE    scan02
          MOV    AX,[BP+4]
          CMP    AX,ss:[BX+6]
          JG     scan02
          MOV    AX,[BP+6]
          CMP    AX,ss:[BX+4]
          JL     scan02

          XOR    CL,CL
          MOV    AX,ss:[BX+4]
          CMP    AX,[BP+4]
          JL     scan03
          OR     CL,008h
          CMP    AX,[BP+4]
          JNE    scan03
          XOR    CL,00Ch
scan03:
          MOV    AX,ss:[BX+6]
          CMP    AX,[BP+6]
          JL     scan04
          OR     CL,002h
          CMP    AX,[BP+6]
          JNE    scan04
          XOR    CL,003h
scan04:
          CMP    CL,000h
          JNE    cond01
          PUSH   ss:[BX+6]
          PUSH   [BP+4]
          PUSH   ss:[BX+2]
          MOV    AH,02Ch
          PUSH   AX
          MOV    AX,ss:[BX+6]
          INC    AX
          XCHG   AX,[BP+4]
          DEC    AX
          MOV    ss:[BX+6],AX
          OR     Byte Ptr ss:[BX+1],004h
          OR     Byte Ptr [BP+1],008h
          JMP    scan
cond01:
          CMP    CL,001h
          JNE    cond02
          MOV    AX,[BP+4]
          DEC    AX
          MOV    ss:[BX+6],AX
          MOV    AH,028h
          MOV    [BP+0],AX
          OR     Byte Ptr ss:[BX+1],004h
          JMP    scan
cond02:
          CMP    CL,002h
          JNE    cond04
          MOV    AX,[BP+4]
          DEC    AX
          XCHG   AX,ss:[BX+6]
          PUSH   AX
          MOV    AX,[BP+6]
          INC    AX
          PUSH   AX
          PUSH   ss:[BX+2]
          MOV    AH,028h
          PUSH   AX
          OR     Byte Ptr ss:[BX+1],004h
          OR     Byte Ptr [BP+1],028h
          JMP    scan
cond04:
          CMP    CL,004h
          JNE    cond05
          MOV    AX,ss:[BX+6]
          INC    AX
          MOV    [BP+4],AX
          OR     Byte Ptr ss:[BX+1],024h
          OR     Byte Ptr [BP+1],008h
          JMP    scan
cond05:
          CMP    CL,005h
          JNE    cond06
          MOV    AH,020h
          MOV    [BP+0],AX
          ADD    SP,8
          JMP    scan
cond06:
          CMP    CL,006h
          JNE    cond08
          MOV    AX,[BP+6]
          INC    AX
          MOV    ss:[BX+4],AX
          OR     Byte Ptr ss:[BX+1],008h
          OR     Byte Ptr [BP+1],024h
          JMP    scan
cond08:
          CMP    CL,008h
          JNE    cond09
          MOV    AX,ss:[BX+4]
          DEC    AX
          XCHG   AX,[BP+6]
          PUSH   AX
          MOV    AX,ss:[BX+6]
          INC    AX
          PUSH   AX
          PUSH   [BP+2]
          MOV    AX,[BP+0]
          OR     AH,08h
          PUSH   AX
          OR     Byte Ptr ss:[BX+1],02Ch
          OR     Byte Ptr [BP+1],004h
          JMP    scan
cond09:
          CMP    CL,009h
          JNE    cond10
          MOV    AX,ss:[BX+4]
          DEC    AX
          MOV    [BP+6],AX
          OR     Byte Ptr ss:[BX+1],028h
          OR     Byte Ptr [BP+1],004h
          JMP    scan
cond10:
          CMP    CL,00Ah
          JNE    conderr
          PUSH   [BP+6]
          PUSH   ss:[BX+4]
          PUSH   [BP+2]
          MOV    AH,02Ch
          PUSH   AX
          MOV    AX,[BP+6]
          INC    AX
          XCHG   AX,ss:[BX+4]
          DEC    AX
          MOV    [BP+6],AX
          OR     Byte Ptr ss:[BX+1],008h
          OR     Byte Ptr [BP+1],004h
          JMP    scan
conderr:
          MOV    AX,3
          INT    10h
          mov    bp,si
          jmp    done
scan09:
          mov    bp,si                 ; Restore BP pointer from SI save area

          POP    AX                    ; Load options (AH) & bit mask (AL)
          POP    DI                    ; Load vertical Y corrdinate
          POP    CX                    ; Load horizontal X corrdinate

          MOV    [bp+tmpw1],CX         ; Save horizontal X corrdinate
          MOV    [bp+tmpw7],CX         ;   ...
          MOV    [bp+tmpw2],DI         ; Save vertical Y corrdinate

          MOV    BX,CX                 ; Copy X corrdinate
          MOV    SI,CX                 ; Copy X corrdinate
          MOV    AL,80h                ; Determine the starting bit to plot
          AND    CL,7                  ;   ...
          ROR    AL,CL                 ;   ...

          SHR    SI,1                  ; Divide X corrdinate by three and
          SHR    SI,1                  ;   ...
          SHR    SI,1                  ;   ...
          shl    di,1
          shl    di,1
          add    di,[bp+tmpw2]
          shl    di,1
          shl    di,1
          shl    di,1
          shl    di,1
          ADD    SI,di                 ; Load Y (row) offset in graphic memory

          MOV    [bp+tmpw3],AX         ; Save horizontal X in (AL/SI) form
          MOV    [bp+tmpw4],SI         ;   ...

;      Determine the left most bit of the current horizontal line (AL/SI).  BX
;  is the current X corrdinate.

          TEST   [bp+tmpw3],00800h     ;
          JNZ    left03                ;

          TEST   AL,ES:[SI]            ; Is the current point a border?
          JNZ    left03                ;   Yes, don't need to scan left

          MOV    CX,[bp+tmpw1]         ;
          SUB    CX,_GDVW_X1           ; Compute Delta of X and X(left limit)
          JZ     left03                ;   Zero delta, don't need to scan left
left01:
          ROL    AL,1                  ; Position to the next bit on the left
          SBB    SI,0                  ; Automatic SI decrement on carry
          TEST   AL,ES:[SI]            ; Are we now at the paint border?
          LOOPZ  left01                ; No, continue scanning to the left
          JZ     left02

          ROR    AL,1
          ADC    SI,0
          INC    CX
left02:
          ADD    CX,_GDVW_X1           ;
          MOV    [bp+tmpw1],CX         ; Save horizontal X corrdinate
          XCHG   AX,[bp+tmpw3]         ; Exchange horizontal X in (AL/SI) form
          XCHG   SI,[bp+tmpw4]         ;   ...
left03:

;      Force scan to the right define by lenght value, during the force fill
;  skip over border areas

          POP    CX                    ; Load end of force area
          MOV    [bp+tmpw8],CX
          SUB    CX,BX                 ; Determine delta of the start and stop
          INC    CX                    ;   X corrdinates
force01:
          TEST   AL,ES:[SI]            ; Are we at the paint border?
          JNZ    force02               ;   Yes, enter force scan mode
          ROR    AL,1                  ; Next bit to the right
          JC     force1B
force1A:
          INC    BX                    ; Increment working X corrdinate
          LOOP   force01               ; Continue filling to the right
          JMP    short right01         ;
force1B:
          INC    SI                    ; Automatic SI increment on carry
          CMP    CX,9
          JLE    force1A
          CMP    BYTE PTR ES:[SI],0
          JNE    force1A
          SUB    CX,8
          ADD    BX,8
          JMP    short force1B
force02:
          CALL   entry
          DEC    CX
          JZ     force04
force03:
          ROR    AL,1                  ; Next bit to the right
          ADC    SI,0                  ;   Jump on overflow
          INC    BX                    ;
          TEST   AL,ES:[SI]            ; Are we at a non paint border?
          LOOPNZ force03               ;   Yes, enter fill mode
          OR     CX,CX                 ;
          JNZ    force05               ;
force04:
          JMP    loop
force05:
          MOV    [bp+tmpw1],BX
          MOV    [bp+tmpw3],AX
          MOV    [bp+tmpw4],SI
          JMP    short force01

right01:
          TEST   [bp+tmpw3],00400h     ;
          JNZ    right03               ;

          MOV    CX,_GDVW_X2
          SUB    CX,BX                 ; Compute Delta of X and X(left limit)
          JS     right03
          INC    CX
right02:
          TEST   AL,ES:[SI]            ; Are we at the paint border?
          JNZ    right03               ;   Yes, continue
          ROR    AL,1                  ; Position to the next bit on the left
          ADC    SI,0                  ;
          INC    BX                    ; Add one to current X corrdinate
          LOOP   right02               ; Continue filling to the left
right03:
          CALL   entry
          JMP    loop

entry:
          CMP    BX,[bp+tmpw1]
          JNZ    entry01
          RET
entry01:
          PUSH   BX
          PUSH   AX
          PUSH   SI
          PUSH   CX

          MOV    CX,BX
          SUB    CX,[bp+tmpw1]
;         DEC    CX

          MOV    BX,[bp+tmpw1]
          AND    BX,7
          SHL    BX,1
          SHL    BX,1
          SHL    BX,1
          MOV    DI,[bp+tmpw2]
          AND    DI,7
          OR     BX,DI
          ADD    DI,_GDGSADR
          MOV    AX,[bp+tmpw3]
          MOV    SI,[bp+tmpw4]

          CMP    _GDS_FLG,0
          JNE    fill01

fill001:
          out    DX,AL
          mov    ah,es:[si]
          mov    ah,_GDCOLOR
          mov    es:[si],ah
          ror    al,1
          jnc    fill003
fill002:
          INC    SI
          CMP    CX,8
          JLE    fill003
          MOV    AL,0FFh
          OUT    DX,AL
          MOV    ES:[SI],AH
          SUB    CX,8
          MOV    AL,080h
          JMP    short fill002
fill003:
          LOOP   fill001
          jmp    short fill04

;******

fill01:
          OUT    DX,AL                 ; Set graphic bit for current byte
          MOV    AH,ES:[SI]            ;
          push   bx
          mov    bh,bl
          shr    bl,1
          shr    bl,1
          shr    bl,1
          and    bl,7
          shl    bh,1
          shl    bh,1
          shl    bh,1
          and    bh,038h
          or     bl,bh
          xor    bh,bh
          add    bx,offset DGROUP:_GDSHAD1
          inc    bx
          inc    bx
          MOV    AH,ds:[BX]            ;
          pop    bx
          MOV    ES:[SI],AH            ;
          ADD    BX,8                  ;
          AND    BX,63                 ;
          ROR    AL,1                  ; Next bit to the right
          JNC    fill03
fill02:
          INC    SI
          CMP    CX,8
          JLE    fill03
          TEST   _GDMERGE,0FFh         ; Using merge mode other than zero?
          JNZ    fill03                ;** Yes, skip fast paint fill logic
          PUSH   AX
          MOV    AL,0FFh
          OUT    DX,AL
          dec    dx
          MOV    AX,0105h
          out    dx,ax
          inc    dx
          MOV    AL,ES:[DI]
fill02A:
          CMP    CX,8
          JLE    fill02B
          MOV    ES:[SI],AL
          INC    SI
          SUB    CX,8
          JMP    short fill02A
fill02B:
          dec    dx
          MOV    AX,0A05h
          out    dx,ax
          MOV    AX,0FF08h
          out    dx,ax
          inc    dx
          POP    AX
fill03:
          LOOP   fill01

fill04:
          POP    CX
          POP    SI
          POP    AX
          POP    BX

          TEST   [bp+tmpw3],02000h
          JZ     skip01
          RET
skip01:
          MOV    [bp+tmpw5],AX         ; Save AX in non-stack area
          POP    [bp+tmpw6]
top:
          MOV    AX,[bp+tmpw2]
          CMP    AX,_GDVW_Y1           ; Are we already at the top?
          JE     bot                   ;   Yes, stack the bottow
          TEST   [bp+tmpw3],08000h     ; Do we perform a force scan check?
          JNZ    top01                 ;   Yes, continue

          MOV    AX,BX
          DEC    AX
          PUSH   AX
          PUSH   [bp+tmpw1]
          MOV    AX,[bp+tmpw2]
          DEC    AX
          PUSH   AX
          MOV    AH,040h
          PUSH   AX
          JMP    short bot
top01:

          MOV    AX,[bp+tmpw1]
          CMP    AX,[bp+tmpw7]
          JGE    top02

;      store the front half

          MOV    AX,[bp+tmpw7]
          DEC    AX
          PUSH   AX
          PUSH   [bp+tmpw1]
          MOV    AX,[bp+tmpw2]
          DEC    AX
          PUSH   AX
          MOV    AH,040h
          PUSH   AX
top02:
          MOV    AX,BX
          DEC    AX
          CMP    AX,[bp+tmpw8]
          JLE    bot

;      store the back half

          PUSH   AX

          MOV    AX,[bp+tmpw8]
          INC    AX
          PUSH   AX
          MOV    AX,[bp+tmpw2]
          DEC    AX
          PUSH   AX
          MOV    AH,040h
          PUSH   AX
bot:
          MOV    AX,[bp+tmpw2]
          CMP    AX,_GDVW_Y2           ; Are we already at the bottow?
          JNE    bot0                  ;   Yes, return         w
          PUSH   [bp+tmpw6]
          MOV    AX,[bp+tmpw5]         ; Restore AX from non-stack area
          RET
bot0:
          TEST   [bp+tmpw3],04000h     ; Do we perform a force scan check?
          JNZ    bot01                 ;   Yes, continue

          MOV    AX,BX
          DEC    AX
          PUSH   AX
          PUSH   [bp+tmpw1]
          MOV    AX,[bp+tmpw2]
          INC    AX
          PUSH   AX
          MOV    AH,080h
          PUSH   AX
          JMP    short bot03
bot01:
          MOV    AX,[bp+tmpw1]
          CMP    AX,[bp+tmpw7]
          JGE    bot02

;      store the front half

          MOV    AX,[bp+tmpw7]
          DEC    AX
          PUSH   AX
          PUSH   [bp+tmpw1]
          MOV    AX,[bp+tmpw2]
          INC    AX
          PUSH   AX
          MOV    AH,080h
          PUSH   AX
bot02:
          MOV    AX,BX
          DEC    AX
          CMP    AX,[bp+tmpw8]
          JLE    bot03

;      store the back half

          PUSH   AX
          MOV    AX,[bp+tmpw8]
          INC    AX
          PUSH   AX
          MOV    AX,[bp+tmpw2]
          INC    AX
          PUSH   AX
          MOV    AH,080h
          PUSH   AX
bot03:
          PUSH   [bp+tmpw6]
          MOV    AX,[bp+tmpw5]         ; Restore AX from non-stack area
          RET

done:
          mov    dx,03CEh
          mov    ax,3
          out    dx,ax
          mov    ax,5
          out    dx,ax
          mov    ax,0FF08h
          out    dx,ax

          pop    di
          pop    si
          mov    sp,bp
          pop    bp
          ret

_gpflood  endp
_text     ends
          end
