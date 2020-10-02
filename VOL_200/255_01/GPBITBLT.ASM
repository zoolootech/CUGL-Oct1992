          page   80,132
          page
;
;       EGA Graphic Primitive for Microsoft C 3.0, Version 01APR86.
;       (C) 1986 by Kent Cedola, 2015 Meadow Lake Ct., Norfolk, VA, 23518
;

towork    equ    4
fmwork    equ    6

towk      equ    -20
tox       equ    -18
toy       equ    -16
tow       equ    -14
toh       equ    -12
fmwk      equ    -10
fmx       equ    -8
fmy       equ    -6
fmw       equ    -4
fmh       equ    -2

needed    equ    20                    ; Amount of temporary space needed

xorigin   equ    2
yorigin   equ    4
width     equ    6
height    equ    8
x         equ    10
y         equ    12
w         equ    14
h         equ    16
aspect1   equ    18
aspect2   equ    20

dgroup    group  _data

_data     segment word public 'data'
          assume ds:dgroup

          public _gdxmax,_gdxscale,_gdymax,_gdyscale

_gdxmax   dw     1024
_gdxscale db     1024 dup(0)
_gdymax   dw     1024
_gdyscale db     1024 dup(0)

_data     ends

_text     segment byte public 'code'

          assume cs:_text,ds:dgroup
          public _gpbitblt

          extrn  _xpcpywtg:near
;         extrn  _xpcpywtw:near,_xpcpywtg:near,_xpcpywtd:near,_xpcpywtp:near
;         extrn  _xpcpygtw:near,_xpcpygtg:near,_xpcpygtd:near,_xpcpygtp:near
;         extrn  _xpcpydtw:near,_xpcpydtg:near,_xpcpydtd:near,_xpcpydtp:near

_gpbitblt proc   near

          push   bp
          mov    bp,sp
          sub    sp,needed             ; Reserve space for temp. variables

fmparm:
          mov    bx,[bp+fmwork]        ; Load pointer to 'from' work parameters
          cmp    byte ptr[bx+1],0      ; Check for 'from' workspace
          jne    fmarea                ; No, jump to 'from' workarea code
fmspace:
          mov    ax,[bx+x]             ; Compute 'from' true X coordinate
          cmp    ax,[bx+width]         ;   ...
          jae    wkerror               ;   ...
          mov    [bp+fmx],ax           ;   ...

          mov    ax,[bx+y]             ; Compute 'from' true Y coordinate
          cmp    ax,[bx+height]        ;   ...
          jae    wkerror               ;   ...
          mov    [bp+fmy],ax           ;   ...
          jmp    short fmcont
fmarea:
          mov    ax,[bx+xorigin]       ; Compute 'from' true X coordinate
          add    ax,[bx+x]             ;   ...
          cmp    ax,[bx+width]         ;   ...
          jae    wkerror               ;   ...
          mov    [bp+fmx],ax           ;   ...

          mov    ax,[bx+yorigin]       ; Compute 'from' true Y coordinate
          add    ax,[bx+y]             ;   ...
          cmp    ax,[bx+height]        ;   ...
          jae    wkerror               ;   ...
          mov    [bp+fmy],ax           ;   ...
fmcont:
          mov    ax,[bx+width]         ; Compute 'from' true width
          sub    ax,[bp+fmx]           ;   ...
          cmp    ax,[bx+w]             ;   ...
          jbe    fmar01                ;   ...
          mov    ax,[bx+w]             ;   ...
fmar01:                                ;   ...
          mov    [bp+fmw],ax           ;   ...

          mov    ax,[bx+height]        ; Compute 'from' true height
          sub    ax,[bp+fmy]           ;   ...
          cmp    ax,[bx+h]             ;   ...
          jbe    fmar02                ;   ...
          mov    ax,[bx+h]             ;   ...
fmar02:                                ;   ...
          mov    [bp+fmh],ax           ;   ...
          jmp    short toparm
wkerror:
          jmp    fullclip
toparm:
          mov    bx,[bp+towork]        ; Load pointer to 'to' work parameters
          cmp    byte ptr [bx+1],0     ; Check for 'to' workspace
          jne    toarea                ; No, jump to 'to' workarea code
tospace:
          mov    ax,[bx+x]             ; Compute 'to' true X coordinate
          cmp    ax,[bx+width]         ;   ...
          jae    wkerror               ;   ...
          mov    [bp+tox],ax           ;   ...

          mov    ax,[bx+y]             ; Compute 'to' true Y coordinate
          cmp    ax,[bx+height]        ;   ...
          jae    wkerror               ;   ...
          mov    [bp+toy],ax           ;   ...
          jmp    short tocont
toarea:
          mov    ax,[bx+xorigin]       ; Compute 'to' true X coordinate
          add    ax,[bx+x]             ;   ...
          cmp    ax,[bx+width]         ;   ...
          jae    wkerror               ;   ...
          mov    [bp+tox],ax           ;   ...

          mov    ax,[bx+yorigin]       ; Compute 'to' true Y coordinate
          add    ax,[bx+y]             ;   ...
          cmp    ax,[bx+height]        ;   ...
          jae    wkerror               ;   ...
          mov    [bp+toy],ax           ;   ...
tocont:
          mov    ax,[bx+w]             ; If both width and height are zero then
          or     ax,[bx+h]             ;   use 'from' width and height values
          jne    toar01                ; No, jump over zero w,h code
          mov    ax,[bp+fmw]           ; Copy 'from' width and height to 'to'
          mov    [bp+tow],ax           ;   widht and height
          mov    ax,[bp+fmh]           ;   ...
          mov    [bp+toh],ax           ;   ...
          jmp    short toar04          ;   ...
toar01:
          cmp    ax,0                  ; If the width is zero then compute it
          jnz    toar02                ;   from the 'from' / 'to' height ratio
          mov    ax,[bx+h]             ;   ...
          mul    word ptr [bp+fmw]     ;   ...
          div    word ptr [bp+fmh]     ;   ...
          mul    word ptr [bx+aspect1] ;   ...
          div    word ptr [bx+aspect2] ;   ...
toar02:                                ;   ...
          mov    [bp+tow],ax           ;   ...

          mov    ax,[bx+h]             ; If the height is zero then compute it
          cmp    ax,0                  ;   from the 'from' / 'to' width ratio
          jnz    toar03                ;   ...
          mov    ax,[bx+w]             ;   ...
          mul    word ptr [bp+fmh]     ;   ...
          div    word ptr [bp+fmw]     ;   ...
          mul    word ptr [bx+aspect2] ;   ...
          div    word ptr [bx+aspect1] ;   ...
toar03:                                ;   ...
          mov    [bp+toh],ax           ;   ...
toar04:
          mov    ax,[bx+width]         ; Compute 'to' true width
          sub    ax,[bp+tox]           ;   ...
          cmp    ax,[bp+tow]           ;   ...
          jge    toar05                ;   ...
          mov    [bp+tow],ax           ;   ...
toar05:
          mov    ax,[bx+height]        ; Compute 'to' true height
          sub    ax,[bp+toy]           ;   ...
          cmp    ax,[bp+toh]           ;   ...
          jge    toar06                ;   ...
          mov    [bp+toh],ax           ;   ...
toar06:

;   Build slope array

          push   ds
          pop    es

          pop    si
          pop    di

          lea    di,dgroup:_gdxscale
          mov    bx,[bp+fmw]
          mov    dx,[bp+tow]
          call   _gpslope

          lea    di,dgroup:_gdyscale
          mov    bx,[bp+fmh]
          mov    dx,[bp+toh]
          call   _gpslope

          pop    di
          pop    si

;   Determine which routines to call for pixel copy.

          mov    bx,[bp+towork]
          mov    [bp+towk],bx
          mov    al,[bx]
          shl    al,1
          shl    al,1
          mov    bx,[bp+fmwork]
          mov    [bp+fmwk],bx
          or     al,[bx]
          xor    bh,bh
          mov    bl,al
          shl    bx,1
          add    bx,offset _text:vectable
          call   cs:[bx]
          jmp    short theend
fullclip:
          mov    ax,-2
          jmp    short done
theend:
          xor    ax,ax
done:
          mov    sp,bp                 ; Restore the stack
          pop    bp
          ret

vectable  dw     offset _text:_xpcpywtw
          dw     offset _text:_xpcpywtg
          dw     offset _text:_xpcpywtd
          dw     offset _text:_xpcpywtp
          dw     offset _text:_xpcpygtw
          dw     offset _text:_xpcpygtg
          dw     offset _text:_xpcpygtd
          dw     offset _text:_xpcpygtp
          dw     offset _text:_xpcpydtw
          dw     offset _text:_xpcpydtg
          dw     offset _text:_xpcpydtd
          dw     offset _text:_xpcpydtp

_xpcpywtw:
_xpcpywtd:
_xpcpywtp:
_xpcpygtw:
_xpcpygtg:
_xpcpygtd:
_xpcpygtp:
_xpcpydtw:
_xpcpydtg:
_xpcpydtd:
_xpcpydtp:
          ret

_gpbitblt endp

_gpslope  proc   near

;         DI = address of slope
;         BX = X1
;         DX = X2
;         ES = DS

          cmp    dx,bx
          je     line
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
          ret

octant2:
          mov    byte ptr [di],0
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
          mov    byte ptr [di],0
octant2E:
          loop   octant2L
          inc    byte ptr [di]
          ret

line:
          mov    al,1
          mov    cx,bx
          rep stosb

          ret

_gpslope  endp

_text     ends
          end
