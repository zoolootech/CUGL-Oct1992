          page   80,132
          page
;
;       EGA Graphic Primitive for Turbo Pascal 3.01A, Version 01MAR86.
;       (C) 1986 by Kent Cedola, 2015 Meadow Lake Ct., Norfolk, VA, 23518
;
;       The algorithm for drawing a circle (below) was from an article in
;       Dr. Dobb's Journal, December 1983, pp. 19 by Michael T. Enright.
;
;       I converted it from BASIC source (from above) to assembler for high
;       speed plotting and added color/shading fill and clipping code.
;

radius    equ    4

xcoor1    equ    -2                    ; X coordinate (left half)
xcoor2    equ    -4                    ; X coordinate (right half)
ycoor1    equ    -6                    ; Location of Y graphic row, contains
ycoor2    equ    -8                    ;   ... row byte offset.

xc        equ    -10
yc        equ    -12

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

needed    equ    68                    ; Amount of temporary space needed

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
          extrn  _gdshade:word
          extrn  _gdc_flg:byte,_gds_flg:byte,_gdw_flg:byte

_data     ends

_text     segment byte public 'code'

          assume cs:_text,ds:dgroup
          public _gpball
_gpball   proc   near

          push   bp
          mov    bp,sp
          sub    sp,needed             ; Reserve space for temp. variables

          push   si
          push   di

          mov    _GDC_FLG,2            ; Set clipping flag

;    Compute AE = R (save in BX for later)

          mov    ax,[bp+radius]        ; Load user specified radius
          or     ax,ax                 ; If radius is zero, make it 1
          jnz    notzero               ;
          inc    ax                    ;
notzero:
          mov    bx,ax                 ; Copy to BX (AX used later)
          mov    ax,_GDCUR_X
          sub    ax,bx
          cmp    ax,_GDVW_X2
          jbe    next1
          jmp    theend
next1:
          mov    [bp+xc],ax
          add    ax,bx
          add    ax,bx
          cmp    ax,_GDVW_X1
          jae    next2
          jmp    theend
next2:

;    Compute BE = (R * _GDASPC1 / _GDASPC2) (save in DX for later)

          mov    ax,bx                 ; Start with R (AE above)
          mul    _GDASPC1           ; Compute the real Y radius from the
          div    _GDASPC2           ;      the defined screen aspects
          mov    dx,ax                 ; Copy to DX (temp save area)

;    Compute the Y coordinate graphic row offset.

          mov    cx,_GDCUR_Y           ; Compute the upper and lower row for
          sub    cx,ax                 ;   line clipping logic
          cmp    cx,_GDVW_Y2           ;
          jbe    next3
          jmp    theend
next3:
          mov    [bp+yc],cx
          mov    [bp+yf1],cx           ;   ...
          add    cx,ax                 ;   ...
          add    cx,ax                 ;   ...
          cmp    cx,_GDVW_Y1
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

          mov    ax,_GDCUR_X           ; Compute the column byte offset
          mov    [bp+xf1],ax           ; Save the left and right column numbers
          mov    [bp+xf2],ax           ;   for clipping logic

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

          dec    word ptr [bp+xf1]

;    Compute XF2 = XF2 + 1

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

          dec    word ptr [bp+xf1]

;    Compute XF2 = XF2 + 1

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
          mov    dx,03CFh
          mov    al,0FFh               ; Reset EGA back to standard mode
          out    dx,al                 ;   ...
          dec    dx                    ;   ...
          mov    ax,0005h              ;   ...
          out    dx,ax                 ;   ...
          mov    al,03h                ;   ...
          out    dx,ax                 ;   ...

          jmp    theend                ; Restore the callers parameter frame

plotpnts:
          mov    ax,[bp+xf1]           ; Load left x coordinate
          mov    di,_GDVW_X2           ; Load X2 coordinate of viewport (speed)
          cmp    ax,di                 ; Are we out of the viewport?
          ja     plotret               ;   Yes, skip this plot
          mov    cx,[bp+xf2]           ; Load right x coordinate
          mov    si,_GDVW_X1           ; Load X1 coordinate of viewport (speed)
          cmp    cx,si                 ; Are we out of the viewport?
          jb     plotret               ;   Yes, skip this plot
          cmp    ax,si                 ; Clip x1 to stay in viewport
          jae    plot01                ;   Already in the viewport, continue
          mov    ax,si                 ;   Lets clip it
plot01:
          cmp    cx,di                 ; Clip x2 to stay in viewport
          jbe    plot02                ;   Already in the viewport, continue
          mov    cx,di                 ;   Lets clip it
plot02:
          sub    cx,ax                 ; Compute delta X + 1
          inc    cx                    ;   ...
          push   ax                    ; Save starting x coordinate for later
          push   cx                    ; Save counter for later

          mov    cx,ax                 ; Compute starting bit
          shr    ax,1                  ;
          shr    ax,1                  ;
          shr    ax,1                  ;
          mov    di,ax                 ;
          and    cl,7                  ;
          mov    al,080h               ;
          ror    al,cl                 ;
          mov    dx,03CFh              ;

          pop    cx                    ; Restore counter (width of line)
          pop    bx

          mov    si,[bp+yf1]
          cmp    si,_GDVW_Y1
          jb     plot03
          cmp    si,_GDVW_Y2
          ja     plot03
          mov    es,[bp+ycoor1]
          push   ax
          push   bx
          push   cx
          push   di
          call   plotline
          pop    di
          pop    cx
          pop    bx
          pop    ax
plot03:
          mov    si,[bp+yf2]
          cmp    si,_GDVW_Y1
          jb     plotret
          cmp    si,_GDVW_Y2
          ja     plotret
          mov    es,[bp+ycoor2]
          call   plotline

plotret:
          ret

plotline:
          cmp    _GDS_FLG,0
          jne    shading

color:
          mov    ah,_GDCOLOR
color01:
          out    dx,al
          mov    bh,es:[di]
          mov    es:[di],ah
          ror    al,1
          jnc    color04
color02:
          inc    di
          cmp    cx,8
          jb     color04
          mov    al,0FFh
          out    dx,al
color03:
          mov    es:[di],ah
          inc    di
          sub    cx,8
          cmp    cx,8
          jae    color03
          mov    al,080h
          inc    cx
color04:
          loop   color01
          ret

;         al = bit
;         bx = starting offset
;         cx = width
;         dx = port
;         di = byte
;         es = segment

shading:
          mov    si,_GDSHADE
          mov    bx,[si]
          inc    si
          inc    si
          push   cx
          xor    bh,bh
          mov    cx,bx
          jmp    short shade02
shade01:
          push   cx
          mov    cx,bx
          sub    si,bx
shade02:
          out    dx,al
          mov    ah,es:[di]
          mov    ah,[si]
          mov    es:[di],ah
          ror    al,1
          adc    di,0
          inc    si
          loop   shade02
          pop    cx
          loop   shade01
          ret

theend:
          pop    di
          pop    si

          mov    sp,bp                 ; Restore the stack
          pop    bp
          ret
_gpball   endp

_text     ends
          end
