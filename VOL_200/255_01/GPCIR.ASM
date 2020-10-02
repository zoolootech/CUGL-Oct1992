          page   80,132
          page
;
;       EGA Graphic Primitive for Turbo Pascal 3.01A, Version 01FEB86.
;       (C) 1986 by Kent Cedola, 2015 Meadow Lake Ct., Norfolk, VA, 23518
;
;       The algorithm for drawing a circle (below) was from an article in
;       Dr. Dobb's Journal, December 1983, pp. 19 by Michael T. Enright.
;
;       I converting the BASIC sample to Turbo Pascal and then to assembler
;       for top speed.  This routine will plot a solid color or pattern and
;       automatically clip for the current viewport.
;
;       procedure GPCIR(Radius: Integer);
;

radius    equ    4

xcoor1    equ    -2                    ; Location of X graphic bit in AL:SI
xcoor2    equ    -6                    ;   ... format (AH not used).
ycoor1    equ    -10                   ; Location of Y graphic row, contains
ycoor2    equ    -12                   ;   ... row byte offset.

xd        equ    -14                   ; Long integer (32 bits)
yd        equ    -18                   ; Long integer (32 bits)
zx        equ    -22                   ; Long integer (32 bits)
zy        equ    -26                   ; Long integer (32 bits)
tx        equ    -30                   ; Long integer (32 bits)
ty        equ    -34                   ; Long integer (32 bits)
tb        equ    -38                   ; Long integer (32 bits)
er        equ    -42                   ; Long integer (32 bits)
atx       equ    -46                   ; Long integer (32 bits)
aty       equ    -50                   ; Long integer (32 bits)
atb       equ    -54                   ; Long integer (32 bits)

xf1       equ    -58
xf2       equ    -60
yf1       equ    -62
yf2       equ    -64

needed    equ    66                    ; Amount of temporary space needed

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
          extrn  _gdc_flg:byte,_gds_flg:byte,_gdw_flg:byte

_data     ends

_text     segment byte public 'code'

          assume cs:_text,ds:dgroup
          public _gpcir
_gpcir    proc   near

          push   bp
          mov    bp,sp
          sub    sp,needed             ; Reserve space for temp. variables

          push   si
          push   di

          mov    _GDC_FLG,2            ; Set clipping flag

;    Compute AE = R (save in BX for later)

          mov    ax,[bp+radius]        ; Load user specified radius
          or     ax,ax                 ; If radius is zero, make it 1
          jnz    notzero
          inc    ax
notzero:
          mov    bx,ax                 ; Copy to BX (AX used later)
          mov    ax,[_GDCUR_X]
          sub    ax,bx
          cmp    ax,_gdvw_x2
          jbe    next1
          jmp    theend
next1:
          add    ax,bx
          add    ax,bx
          cmp    ax,_gdvw_x1
          jae    next2
          jmp    theend
next2:

;    Compute BE = (R * _GDASPC1 / _GDASPC2) (save in DX for later)

          mov    ax,bx                 ; Start with R (AE above)
          mul    _GDASPC1              ; Compute the real Y radius from the
          div    _GDASPC2              ;   the defined screen aspects
          mov    dx,ax                 ; Copy to DX (temp save area)

;    Compute the Y coordinate graphic row offset.

          mov    cx,_GDCUR_Y           ; Compute the upper and lower row for
          sub    cx,ax                 ;   line clipping logic
          cmp    cx,_gdvw_y2           ;
          jbe    next3
          jmp    theend
next3:
          mov    [bp+yf1],cx           ;   ...
          add    cx,ax                 ;   ...
          add    cx,ax                 ;   ...
          cmp    cx,_gdvw_y1
          jae    next4
          jmp    theend
next4:
          mov    [bp+yf2],cx           ;   ...

          mov    _GDC_FLG,0            ; Clear clipping flag

          shl    ax,1                  ; Compute the graphic segment offset
          shl    ax,1                  ;   for the Y radius length
          add    ax,dx                 ;   ...
          mov    si,ax                 ; Copy to SI (temp save area)

          mov    ax,_GDCUR_Y           ; Compute the graphic segment offset
          shl    ax,1                  ;   for the current row
          shl    ax,1                  ;   ... = A000 + (80 * Y) / 16;
          add    ax,_GDCUR_Y           ;   ...
          add    ax,0A000h             ;   ...
          sub    ax,si                 ; Init the top and bottow y coordinates
          mov    [bp+ycoor1],ax        ;   ...
          add    ax,si                 ;   ...
          add    ax,si                 ;   ...
          mov    [bp+ycoor2],ax        ;   ...

;    Compute the X coordinate graphic bit offset.

          mov    si,_GDCUR_X           ; Compute the column byte offset
          mov    [bp+xf1],si           ; Save the left and right column numbers
          mov    [bp+xf2],si           ;   for clipping logic
          mov    cx,si                 ;   ... (Save for later)
          shr    si,1                  ;   DI = X / 8;
          shr    si,1                  ;   ...
          shr    si,1                  ;   ...
          mov    al,80h                ; Compute mask byte to change one bit
          and    cl,7                  ;   ...  (It has to be done this way to
          ror    al,cl                 ;   ...   use merge value in write)
          mov    [bp+xcoor1],al        ; Init the left and right x coordinates
          mov    [bp+xcoor1-2],si      ;   ...  byte 0   = bit mask value
          mov    [bp+xcoor2],al        ;   ...  byte 1   = not used
          mov    [bp+xcoor2-2],si      ;   ...  byte 2-3 = byte column offset

;    Compute XD = BE * BE

          mov    cx,dx                 ; Save BE for later (in CX)
          mov    ax,dx                 ; Must use AX for multiply
          mul    dx                    ; BE * BE (AX * DX)
          mov    [bp+xd],ax            ; Save XD
          mov    [bp+xd-2],dx          ;   ...

;    Compute DX = 2 * BE * BE (or XD * 2)

          shl    ax,1                  ; Long shift left for multiply
          rcl    dx,1                  ;   ...
          mov    [bp+zx],ax            ; Save DX
          mov    [bp+zx-2],dx          ;   ...

;    Compute DY = 2 * AE * AE

          mov    ax,bx                 ; Move current AE to AX
          mul    bx                    ;   ... (AE * AE)
          push   dx                    ; Save the value (AE * AE) for later
          push   ax                    ;   ...
          shl    ax,1                  ; Compute AE * AE * 2 (shift left)
          rcl    dx,1                  ;   ...
          mov    [bp+zy],ax            ; Save DY
          mov    [bp+zy-2],dx          ;   ...

;    Compute YD = (2 * BE - 1) * AE * AE

          pop    ax
          shl    cx,1                  ; Compute BE * 2
          dec    cx                    ; Compute BE * 2 - 1
          mul    cx                    ; Compute (BE * 2 - 1) * AE
          mov    [bp+yd],ax            ; Save what we have so far
          mov    [bp+yd-2],dx          ;   ...
          pop    ax                    ; Compute (BE * 2 - 1) * AE * AE
          mul    cx                    ;   ...
          add    [bp+yd-2],ax          ;   ...

;    Compute ER = 0

          xor    ax,ax                 ; Clear AX to clear ER
          mov    [bp+er],ax            ;   ...
          mov    [bp+er-2],ax          ;   ...

;    Setup the EGA to perform graphics

          mov    dx,03CEh              ; Load EGA graphic controller port addr.
          mov    ah,_GDMERGE           ; Load current merge setting
          mov    al,3                  ; Merge register is number three
          out    dx,ax                 ; Set the EGA's merge register
          mov    ax,0205h              ; Load write mode number two
          out    dx,ax                 ; Set the EGA's mode register
          mov    al,8                  ; Load the mask register number
          out    dx,al                 ; Just postion EGA's controller to it

          call   plotpnts              ; Plot the first set of points

next_x:
          mov    cx,-1

          mov    ax,[bp+er]            ; Compute TX = ER + XD
          mov    dx,[bp+er-2]          ;   ...
          add    ax,[bp+xd]            ;   ...
          adc    dx,[bp+xd-2]          ;   ...
          mov    [bp+tx],ax            ;   ...
          mov    [bp+tx-2],dx          ;   ...
          jns    abs_tx                ; Compute ATX = abs(TX), if needed
          xor    ax,cx
          xor    dx,cx
          inc    ax
          jnc    abs_tx
          inc    dx
abs_tx:
          mov    [bp+atx],ax           ;   ...
          mov    [bp+atx-2],dx         ;   ...

          mov    ax,[bp+er]            ; Compute TY = ER - YD
          mov    dx,[bp+er-2]          ;   ...
          sub    ax,[bp+yd]            ;   ...
          sbb    dx,[bp+yd-2]          ;   ...
          mov    [bp+ty],ax            ;   ...
          mov    [bp+ty-2],dx          ;   ...
          jns    abs_ty                ; Compute ATY = abs(TY), if needed
          xor    ax,cx                 ;   ...
          xor    dx,cx                 ;   ...
          inc    ax                    ;   ...
          jnc    abs_ty                ;   ...
          inc    dx                    ;   ...
abs_ty:
          mov    [bp+aty],ax           ;   ...
          mov    [bp+aty-2],dx         ;   ...

          mov    ax,[bp+tx]            ; Compute TB = ER + XD - YD
          mov    dx,[bp+tx-2]          ;   ... (or TB = TX - YD)
          sub    ax,[bp+yd]            ;   ...
          sbb    dx,[bp+yd-2]          ;   ...
          mov    [bp+tb],ax            ;   ...
          mov    [bp+tb-2],dx          ;   ...
          jns    abs_tb                ; Compute ATB = abs(TB), if needed
          xor    ax,cx                 ;   ...
          xor    dx,cx                 ;   ...
          inc    ax                    ;   ...
          jnc    abs_tb                ;   ...
          inc    dx                    ;   ...
abs_tb:
          mov    [bp+atb],ax           ;   ...
          mov    [bp+atb-2],dx         ;   ...

cond1:
          mov    ax,[bp+atx]           ; Load absolute value of TX
          mov    dx,[bp+atx-2]         ;   ...
          cmp    dx,[bp+aty-2]         ; if (abs(TX) < abs(TY)
          ja     cond2                 ;   ...
          jb     cond1a                ;   ...
          cmp    ax,[bp+aty]           ;   ...
          jae    cond2                 ;   ...
cond1a:
          cmp    dx,[bp+atb-2]         ; and (abs(TX) < abs(TB))
          ja     cond2                 ;   ...
          jb     cond1b                ;   ...
          cmp    ax,[bp+atb]           ;   ...
          jae    cond2                 ;   ...
cond1b:

;    Compute XF1 = XF1 - 1

          rol    byte ptr [bp+xcoor1],1
          sbb    word ptr [bp+xcoor1-2],0
          dec    word ptr [bp+xf1]

;    Compute XF2 = XF2 + 1

          ror    byte ptr [bp+xcoor2],1
          adc    word ptr [bp+xcoor2-2],0
          inc    word ptr [bp+xf2]

          mov    ax,[bp+tx]            ; ER = TX
          mov    dx,[bp+tx-2]          ;   ...
          mov    [bp+er],ax            ;   ...
          mov    [bp+er-2],dx          ;   ...
          mov    ax,[bp+zx]            ; XD = XD + DX
          mov    dx,[bp+zx-2]          ;   ...
          add    [bp+xd],ax            ;   ...
          adc    [bp+xd-2],dx          ;   ...

          jmp    until

cond2:
          mov    ax,[bp+aty]           ; Load absolute value of TY
          mov    dx,[bp+aty-2]         ;   ...
          cmp    dx,[bp+atx-2]         ; if (abs(TY) < abs(TX)
          ja     cond3                 ;   ...
          jb     cond2a                ;   ...
          cmp    ax,[bp+atx]           ;   ...
          jae    cond3                 ;   ...
cond2a:
          cmp    dx,[bp+atb-2]         ; and (abs(TY) < abs(TB))
          ja     cond3                 ;   ...
          jb     cond2b                ;   ...
          cmp    ax,[bp+atb]           ;   ...
          jae    cond3                 ;   ...
cond2b:

;    Compute YF1 = YF1 - 1

          add    word ptr [bp+ycoor1],5
          inc    word ptr [bp+yf1]

;    Compute YF2 = YF2 + 1

          sub    word ptr [bp+ycoor2],5
          dec    word ptr [bp+yf2]

          mov    ax,[bp+ty]            ; ER = TY
          mov    dx,[bp+ty-2]          ;   ...
          mov    [bp+er],ax            ;   ...
          mov    [bp+er-2],dx          ;   ...

          mov    ax,[bp+zy]            ; YD = YD - DY
          mov    dx,[bp+zy-2]          ;   ...
          sub    [bp+yd],ax            ;   ...
          sbb    [bp+yd-2],dx          ;   ...

          jmp    until

cond3:

;    Compute XF1 = XF1 - 1

          rol    byte ptr [bp+xcoor1],1
          sbb    word ptr [bp+xcoor1-2],0
          dec    word ptr [bp+xf1]

;    Compute XF2 = XF2 + 1

          ror    byte ptr [bp+xcoor2],1
          adc    word ptr [bp+xcoor2-2],0
          inc    word ptr [bp+xf2]

;    Compute YF1 = YF1 + 1

          add    word ptr [bp+ycoor1],5
          inc    word ptr [bp+yf1]

;    Compute YF2 = YF2 - 1

          sub    word ptr [bp+ycoor2],5
          dec    word ptr [bp+yf2]

          mov    ax,[bp+tb]            ; ER = TB
          mov    dx,[bp+tb-2]          ;   ...
          mov    [bp+er],ax            ;   ...
          mov    [bp+er-2],dx          ;   ...

          mov    ax,[bp+zx]            ; XD = XD + DX
          mov    dx,[bp+zx-2]          ;   ...
          add    [bp+xd],ax            ;   ...
          adc    [bp+xd-2],dx          ;   ...

          mov    ax,[bp+zy]            ; YD = YD - DY
          mov    dx,[bp+zy-2]          ;   ...
          sub    [bp+yd],ax            ;   ...
          sbb    [bp+yd-2],dx          ;   ...

until:
          call   plotpnts              ; Plot the next set of points

          mov    ax,[bp+ycoor1]        ; When the two y coordinates are equal
          cmp    ax,[bp+ycoor2]        ;   ... we are done.
          je     done                  ;   ... (all done condition)
          jmp    next_x                ;   ... (do the next row condition)
done:
          mov    al,0FFh               ; Reset EGA back to standard mode
          out    dx,al                 ;   ...
          dec    dx                    ;   ...
          mov    ax,00005h             ;   ...
          out    dx,ax                 ;   ...
          mov    ax,00003h             ;   ...
          out    dx,ax                 ;   ...

          jmp    theend                ; Restore the callers parameter frame

plotpnts:
          mov    ah,_GDCOLOR
          mov    dx,03CFh

          mov    bx,[bp+xf1]
          cmp    bx,_GDVW_X1           ; X1
          jae    next5
          mov    _GDC_FLG,1
          jmp    short plot2
next5:
          mov    al,[bp+xcoor1]
          out    dx,al
          mov    si,[bp+xcoor1-2]
          mov    bx,[bp+yf1]
          cmp    bx,_GDVW_Y1           ; Y1
          jae    next6
          mov    _GDC_FLG,1
          jmp    short plot1
next6:
          mov    es,[bp+ycoor1]
          mov    al,es:[si]
          mov    es:[si],ah
plot1:
          mov    bx,[bp+yf2]
          cmp    bx,_GDVW_Y2           ; Y2
          jbe    next7
          mov    _GDC_FLG,1
          jmp    short plot2
next7:
          mov    es,[bp+ycoor2]
          mov    al,es:[si]
          mov    es:[si],ah
plot2:
          mov    bx,[bp+xf2]
          cmp    bx,_GDVW_X2           ; X2
          jbe    next8
          mov    _GDC_FLG,1
          jmp    short plot4
next8:
          mov    al,[bp+xcoor2]
          out    dx,al
          mov    si,[bp+xcoor2-2]
          mov    bx,[bp+yf1]
          cmp    bx,_GDVW_Y1           ; Y1
          jae    next9
          mov    _GDC_FLG,1
          jmp    short plot3
next9:
          mov    es,[bp+ycoor1]
          mov    al,es:[si]
          mov    es:[si],ah
plot3:
          mov    bx,[bp+yf2]
          cmp    bx,_GDVW_Y2           ; Y2
          jbe    next0
          mov    _GDC_FLG,1
          jmp    short plot4
next0:
          mov    es,[bp+ycoor2]
          mov    al,es:[si]
          mov    es:[si],ah
plot4:
          ret

theend:
          pop    di
          pop    si

          mov    sp,bp                 ; Restore the stack
          pop    bp
          ret

_gpcir    endp

_text     ends
          end
