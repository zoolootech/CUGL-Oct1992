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
          public _xdwritbp
          public _xdwritw1,_xdwritw2,_xdwritw3
          public _xdwritb1

          extrn  _GDGSEG:word,_GDMERGE:byte

_xdwritbp dw     0
_xdwritb1 db     0
_xdwritb2 db     0
_xdwritw1 dw     0
_xdwritw2 dw     0
_xdwritw3 dw     0
_data     ends

_text     segment byte public 'code'

          assume cs:_text,ds:dgroup
          public _gpinitwt

_gpinitwt proc   near

          push   bp
          mov    bp,sp

          mov    ax,[bp].buffer
          mov    _xdwritbp,ax
          cmp    [bp].dir,0
          jnz    bottotop
toptobot:
          mov    ax,5
          mov    _xdwritw2,ax
          mov    ax,[bp].y_coor
          jmp    short cont
bottotop:
          mov    ax,-5
          mov    _xdwritw2,ax
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
          mov    _xdwritw1,ax

          mov    al,80h                ; Compute mask byte to first bit
          and    cl,7                  ;   ...  (It has to be done this way to
          ror    al,cl                 ;   ...   use merge value in write)
          mov    _xdwritb1,al          ;   ...

          mov    ax,[bp].x_len
          mov    _xdwritw3,ax

          mov    dx,03CEh              ; Load graphic controller's address port
          mov    ah,_GDMERGE
          mov    al,3
          out    dx,ax
          mov    ax,0205h
          out    dx,ax

          pop    bp
          ret

_gpinitwt endp

_text     ends
          end
