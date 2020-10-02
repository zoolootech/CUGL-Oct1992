          page   80,132
          page
;
;       EGA Graphic Primitive for Microsoft C 3.0, Version 01APR86.
;       (C) 1986 by Kent Cedola, 2015 Meadow Lake Ct., Norfolk, VA, 23518
;

stackval  struc
          dw     ?
          dw     ?
x_coor    dw     ?
y_coor    dw     ?
x_len     dw     ?
y_len     dw     ?
page      dw     ?
buffer    dw     ?
dir       dw     ?
stackval  ends

workarea  struc
base      db     ?
device    db     ?
xorigin   dw     ?
yorigin   dw     ?
width     dw     ?
height    dw     ?
x         dw     ?
y         dw     ?
w         dw     ?
h         dw     ?
aspect1   dw     ?
aspect2   dw     ?
workarea  ends

dgroup    group  _data

_data     segment word public 'data'
          assume ds:dgroup
          extrn  _GDGSEG:word,_GDMERGE:byte
          extrn  _xdreadbp:word
          extrn  _xdreadw1:word,_xdreadw2:word,_xdreadw3:word
          extrn  _xdreadb1:byte
_data     ends

_text     segment byte public 'code'

          assume cs:_text,ds:dgroup
          public _gpinitrd

_gpinitrd proc   near

          push   bp
          mov    bp,sp

          mov    ax,[bp].buffer
          mov    _xdreadbp,ax
          cmp    [bp].dir,0
          jnz    bottotop
toptobot:
          mov    ax,5
          mov    _xdreadw2,ax
          mov    ax,[bp].y_coor
          jmp    short cont
bottotop:
          mov    ax,-5
          mov    _xdreadw2,ax
          mov    ax,[bp].y_coor
          add    ax,[bp].y_len
          dec    ax
cont:
          mov    cx,ax
          shl    ax,1
          shl    ax,1
          add    ax,cx
          add    ax,_gdgseg
          mov    dx,[bp].x_coor
          mov    cx,dx
          shr    dx,1                  ;   X / 8;
          shr    dx,1                  ;   ...
          shr    dx,1                  ;   ...
          add    ax,dx
          mov    _xdreadw1,ax

          and    cl,7
          mov    _xdreadb1,cl

          mov    ax,[bp].x_len
          mov    _xdreadw3,ax

          pop    bp
          ret

_gpinitrd endp

_text     ends
          end
