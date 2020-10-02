          page   80,132
          page
;
;       EGA Graphic Primitive for Microsoft C 3.0, Version 01APR86.
;       (C) 1986 by Kent Cedola, 2015 Meadow Lake Ct., Norfolk, VA, 23518
;

dpl       struc
flags     db     ?
device    db     ?
width     dw     ?
height    dw     ?
w_inch    dw     ?
h_inch    dw     ?
colors    dw     ?
palettes  dw     ?
initrd    dw     ?
readrw    dw     ?
termrd    dw     ?
initwt    dw     ?
writrw    dw     ?
termwt    dw     ?
link      dw     ?
segment   dw     ?
pages     dw     ?
dpl       ends

dgroup    group  _data

_data     segment word public 'data'
          assume ds:dgroup

_vdwritbp dw     0
_vdwritb1 db     0
_vdwritb2 db     0
_vdwritw1 dw     0
_vdwritw2 dw     0
_vdwritw3 dw     0

vdpl      db     01h                   ; Flags
          db     'V'                   ; Device
          dw     ?                     ; Width
          dw     ?                     ; Height
          dw     1                     ; W_inch
          dw     1                     ; H_inch
          dw     ?                     ; Colors
          dw     ?                     ; Palettes
          dw     offset _text:_vpinitrd  ; Initrd
          dw     offset _text:_vpreadrw  ; Readrw
          dw     offset _text:_vptermrd  ; Termrd
          dw     offset _text:_vpinitwt  ; Initwt
          dw     offset _text:_vpwritrw  ; Writrw
          dw     offset _text:_vptermwt  ; Termwt
          dw     0                     ; Link
          dw     ?                     ; Segment
          dw     ?                     ; Pages



_data     ends

_text     segment byte public 'code'

          assume cs:_text,ds:dgroup
          public _vpinit,_vpterm
          public _vpinitrd,_vpreadrw,_vptermrd
          public _vpinitwt,_vpwritrw,_vptermwt

          page

    dpl = vpinit(x,y,bits);

_vpinit   proc   near

          push   bp
          mov    bp,sp
          push   si

          mov    ax,32                 ; Allocate 32 bytes of memory for DPL
          push   ax                    ;   Store number of bytes (32) on stack
          call   _malloc               ;   Call C's memory allocation function
          add    sp,2                  ;   Restore Stack
          or     ax,ax                 ;   Check for "Out of Memory" condition
          jz     initerr               ;     Jump on error (Out of Memory)

          mov    di,ax                 ; Initialize new DPL (pointer in SI)
          push   di                    ;   Save pointer of DPL for later
          lea    si,vdpl               ;   Copy standard pattern to new DPL
          mov    cx,16                 ;     Set counter for DPL size in words
          push   ds                    ;     Copy DS to ES
          pop    es                    ;       ...
          rep movsw                    ;     Perform the copy operation

          pop    si                    ; Restore DPL into SI

          mov    ax,[bp+4]             ; Load width and copy to DPL
          mov    [si].width,ax         ;   ...
          mov    ax,[bp+6]             ; Load height and copy to DPL
          mov    [si].height,ax        ;   ...
          mov    ax,[bp+8]             ; Load bits and copy to DPL
          mov    [si].colors,ax        ;   ...
          mov    [si].palettes,ax      ;   ...

          mul    [si].width            ; Determine width in bytes
          add    ax,[si].palettes      ;   ... = (width * bits + bits - 1) / 8
          dec    ax                    ;   ...
          shr    ax,1                  ;   ...
          shr    ax,1                  ;   ...
          shr    ax,1                  ;   ...

          mul    [si].height           ; Determine number of bytes needed

          div    sixteen               ; Determine number of paragraphs needed
          mov    [si].pages,ax         ;   Copy to DPL




;****
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
