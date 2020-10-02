	  page	 80,132
	  page
;
;	  Kent Cedola
;	  2015 Meadow Lake Court
;	  Norfolk, Virginia  23518
;

dgroup	  group  _data
_data     segment word public 'data'

	  assume ds:dgroup

	  public _gdtype,_gdother,_gdmemory
	  public _gdmaxcol,_gdmaxrow,_gdmaxpal,_gdmaxclr
	  public _gdcolor,_gdmerge,_gdstyle,_gdaspc1,_gdaspc2
	  public _gdcur_x,_gdcur_y,_gdgseg,_gdgsadr
	  public _gdwd_x1,_gdwd_x2,_gdwd_x3,_gdwd_y1,_gdwd_y2,_gdwd_y3
	  public _gdvw_x1,_gdvw_x2,_gdvw_x3,_gdvw_y1,_gdvw_y2,_gdvw_y3
	  public _gdgsadr,_gdshade,_gdshad1
	  public _gdc_flg,_gds_flg,_gdw_flg
	  public _gdcurx1,_gdcury1

	  public _gdwkmws,_gdwkpws,_gdwkmwa,_gdwkpwa
	  public _gdwkmwv,_gdwkpwv,_ddwkmwa,_ddwkpwa
	  public _gdwkmax,_gdwkptx,_gdwkmay,_gdwkpty
	  public _gdwkx1,_gdwky1,_gdwkw1,_gdwkh1
	  public _gdwkx2,_gdwky2,_gdwkw2,_gdwkh2

_gdtype   db	 5
	  db	 0
_gdother  dw	 0
_gdmemory dw	 64
_gdmaxcol dw	 639
_gdmaxrow dw	 349
_gdmaxpal dw	 16
_gdmaxclr dw	 64

_gdcolor  db	 0
_gdmerge  db	 0

_gdstyle  dd	 0

_gdaspc1  dw	 0
_gdaspc2  dw	 0
_gdcur_x  dw	 0
_gdcur_y  dw	 0

_gdgseg   dw	 0A000h
_gdgsadr  dw	 07FF8h

_gdshade  dw	 0
_gdshad1  db	 64 dup(0)

_gdwd_x1  dw	 0
_gdwd_x2  dw	 0
_gdwd_x3  dw	 0
_gdwd_y1  dw	 0
_gdwd_y2  dw	 0
_gdwd_y3  dw	 0
_gdvw_x1  dw	 0
_gdvw_x2  dw	 0
_gdvw_x3  dw	 0
_gdvw_y1  dw	 0
_gdvw_y2  dw	 0
_gdvw_y3  dw	 0

_gdc_flg  db	 0
_gds_flg  db	 0
_gdw_flg  db	 0

_gdcurx1  dw	 0
_gdcury1  dw	 0

_gdwkmws  dw	 0		       ; Maximum Number of Workspaces
_gdwkpws  dw	 0		       ; Pointer to Workspace array
_gdwkmwa  dw	 0		       ; Maximum Number of Workareas
_gdwkpwa  dw	 0		       ; Pointer to Workarea array
_gdwkmwv  dw	 0		       ; Maximum Number of Workviews
_gdwkpwv  dw	 0		       ; Pointer to Workview array
_ddwkmwa  dw	 0		       ; Maximum Number of Workareas, Image
_ddwkpwa  dw	 0		       ; Pointer to Workarea array, Image

_gdwkmax  dw	 0		       ; Maximum Output Column Length (X)
_gdwkptx  dw	 0		       ; Pointer to byte array used in zooming
_gdwkmay  dw	 0		       ; Maximum Output Row Length (Y)
_gdwkpty  dw	 0		       ; Pointer to byte array used in zooming

_gdwkx1   dw	 0
_gdwky1   dw	 0
_gdwkw1   dw	 0
_gdwkh1   dw	 0

_gdwkx2   dw	 0
_gdwky2   dw	 0
_gdwkw2   dw	 0
_gdwkh2   dw	 0

_data	  ends

_text     segment byte public 'code'

	  assume cs:_text,ds:dgroup
	  public _gpparms
_gpparms  proc	 near

	  push	 bp
	  mov	 bp,sp

	  mov	 ah,12h
	  mov	 bl,10h
	  mov	 cx,-1
	  int	 10h
	  cmp	 cx,-1
	  je	 not_ega
	  mov	 _gdother,cx
	  mov	 ch,cl
	  mov	 al,bl
	  and	 ax,03h
	  inc	 ax
	  mov	 cl,6
	  shl	 ax,cl
	  mov	 _gdmemory,ax
	  or	 bl,bl
	  jnz	 more_64k
	  mov	 ax,4
	  mov	 _gdmaxclr,ax
more_64k:
	  cmp	 ch,09h 	    ; Match switch setting on EGA board
	  je	 ecd
	  cmp	 ch,03h 	    ; Monochrome as Primary, EGA secondary
	  je	 ecd
	  cmp	 ch,06h
	  je	 cg
	  cmp	 ch,0Eh
	  je	 cg
	  cmp	 ch,05h
	  je	 mono
	  cmp	 ch,0Dh
	  je	 mono
not_ega:
	  xor	 al,al
	  jmp	 short done
ecd:
	  mov	 al,5
	  jmp	 short done
cg:
	  mov	 ax,199
	  mov	 _gdmaxrow,ax
	  mov	 al,3
	  jmp	 short done
mono:
	  mov	 ax,4
	  mov	 _gdmaxpal,ax
done:
	  xor	 ah,ah
	  mov	 _gdtype,al

	  pop	 bp
	  ret

_gpparms  endp

_text	  ends
	  end
