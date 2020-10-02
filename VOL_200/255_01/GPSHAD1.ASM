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

          extrn  _gdgseg:word,_gdgsadr:word,_gdshad1:byte
          extrn  _gdc_flg:byte,_gds_flg:byte,_gdw_flg:byte

_data     ends

_text     segment byte public 'code'

          assume cs:_text,ds:dgroup
          public _gpshad1
_gpshad1  proc   near

          push   bp
          mov    bp,sp
          push   si
          push   di

          mov    _GDS_FLG,-1           ; Turn line style & shading on

          mov    dx,03CEh
          mov    ax,0205h
          out    dx,ax
          mov    al,08h
          out    dx,al
          inc    dx

          mov    es,_GDGSEG
          lea    si,_GDSHAD1           ; Address of the new shade matrix
          inc    si
          inc    si
          mov    di,_GDGSADR
          MOV    CX,64
          MOV    AL,080h
fbyte01:
          OUT    DX,AL
          MOV    AH,ES:[DI]
          MOV    AH,[SI]
          MOV    ES:[DI],AH
          ROR    AL,1
          ADC    DI,0
          INC    SI
          LOOP   fbyte01

          mov    al,0FFh
          out    dx,al
          dec    dx
          mov    ax,0005h
          out    dx,ax

          pop    di
          pop    si
          pop    bp
          ret
_gpshad1  endp

_text     ends
          end
