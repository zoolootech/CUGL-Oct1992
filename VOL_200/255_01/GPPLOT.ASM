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

          extrn  _gdtype:byte
          extrn  _gdmaxcol:word,_gdmaxrow:word
          extrn  _gdcolor:byte,_gdmerge:byte,_gdaspc1:word,_gdaspc2:word
          extrn  _gdcur_x:word,_gdcur_y:word
          extrn  _gdvw_x1:word,_gdvw_x2:word,_gdvw_x3:word
          extrn  _gdvw_y1:word,_gdvw_y2:word,_gdvw_y3:word
          extrn  _gdc_flg:byte,_gds_flg:byte,_gdw_flg:byte

_data     ends

_text     segment byte public 'code'

          assume cs:_text,ds:dgroup
          public _gpplot
_gpplot   proc   near

          push   bp
          mov    bp,sp
          push   si
          push   di

          mov    _GDC_FLG,0

          mov    ax,[bp+6]
          cmp    ax,_GDVW_Y1
          jl     error
          cmp    ax,_GDVW_Y2
          jg     error
          mov    di,[bp+4]
          cmp    di,_GDVW_X1
          jl     error
          cmp    di,_GDVW_X2
          jle    goodpt
error:
          mov    _GDC_FLG,2
          jmp    short theend
goodpt:
          mov    _GDCUR_Y,ax           ;
          shl    ax,1                  ;   byte that is to be changed
          shl    ax,1                  ;   ES = A000 + (80 * Y) / 16;
          add    ax,[bp+6]             ;   ...
          add    ax,0A000h             ;   ...
          mov    es,ax                 ;   ...
          MOV    _GDCUR_X,di           ;
          mov    cx,di                 ;   ... (Save for later)
          shr    di,1                  ;   DI = X / 8;
          shr    di,1                  ;   ...
          shr    di,1                  ;   ...
          mov    dx,03CEh              ; Load graphic controller's address port
          mov    ah,_GDMERGE
          mov    al,3
          out    dx,ax
          mov    ax,0205h
          out    dx,ax
          mov    al,8                  ; Mask value is in address three (3)
          out    dx,al                 ; Point to the mask register
          inc    dx                    ; Bump port addr to the mask register
          mov    al,80h                ; Compute mask byte to change one bit
          and    cl,7                  ;   ...  (It has to be done this way to
          ror    al,cl                 ;   ...   use merge value in write)
          out    dx,al                 ; Set mask byte to change only our bit
          mov    al,es:[di]
          mov    al,_GDCOLOR
          mov    es:[di],al            ; Apply merge function to bit and store

          mov    al,0FFh
          out    dx,al                 ; Restore the mask byte (for text)
          dec    dx
          mov    ax,5
          out    dx,ax
          mov    al,3
          out    dx,ax
theend:
          pop    di
          pop    si
          pop    bp
          ret
_gpplot   endp

_text     ends
          end
