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

	  extrn  _gdcolor:byte,_gdmerge:byte
	  extrn  _gdcur_x:word,_gdcur_y:word
	  extrn  _gdvw_x1:word,_gdvw_x2:word,_gdvw_x3:word
	  extrn  _gdvw_y1:word,_gdvw_y2:word,_gdvw_y3:word
	  extrn  _gdc_flg:byte,_gds_flg:byte
	  extrn  _gdstyle:dword,_gdgseg:word

_data	  ends

_text     segment byte public 'code'

	  assume cs:_text,ds:dgroup
	  public _gpline
_gpline   proc	 near

	  push	 bp
	  mov	 bp,sp
	  push	 si
	  push	 di

	  mov	 cx,_gdcur_x	       ; Load starting X coordinate (X1)
	  mov	 bx,_gdcur_y	       ; Load starting Y coordinate (Y1)
	  mov	 si,[bp+4]	       ; Load ending X coordinate (X2)
	  mov	 di,[bp+6]	       ; Load ending Y coordinate (Y2)
	  mov	 _gdcur_x,si	       ; Store new current (X,Y) coordinates
	  mov	 _gdcur_y,di	       ;   ...

	  mov	 _gdc_flg,0	       ; Set the clipping flag to zero

;      Perform the Cohen-Sutherland two-step clipping algorithm, in which a
;  four-element code indicates the position of a point.  From pages 36-37 of
;  the book "Microcomputer Displays, Graphics, and Animation" by Bruce Atwick.

dline01:
	  XOR	 AX,AX

;      Generate 4-element start and end point codes

	  CMP	 BX,_gdvw_y1
	  JGE	 $+5
	  OR	 AH,8
	  CMP	 BX,_gdvw_y2
	  JLE	 $+5
	  OR	 AH,4
	  CMP	 CX,_gdvw_x1
	  JGE	 $+5
	  OR	 AH,2
	  CMP	 CX,_gdvw_x2
	  JLE	 $+5
	  OR	 AH,1

	  CMP	 DI,_gdvw_y1
	  JGE	 $+4
	  OR	 AL,8
	  CMP	 DI,_gdvw_y2
	  JLE	 $+4
	  OR	 AL,4
	  CMP	 SI,_gdvw_x1
	  JGE	 $+4
	  OR	 AL,2
	  CMP	 SI,_gdvw_x2
	  JLE	 $+4
	  OR	 AL,1

	  OR	 AX,AX
	  JNZ	 dline15
	  JMP	 dline09
dline15:
	  mov	 _gdc_flg,1
	  TEST	 AH,AL
	  JZ	 dline02
	  mov	 _gdc_flg,2
	  jmp	 theend
dline02:
	  OR	 AL,AL
	  JNZ	 dline03

;      Swap start and end points

	  XCHG	 CX,SI		       ;
	  XCHG	 BX,DI		       ;
	  XCHG	 AH,AL

dline03:

	  TEST	 AL,2
	  JNZ	 dline06
	  TEST	 AL,4
	  JNZ	 dline07
	  TEST	 AL,8
	  JNZ	 dline08
dline05:
	  MOV	 BP,DI
	  SUB	 BP,BX
	  MOV	 AX,_gdvw_x2
	  SUB	 AX,CX
	  IMUL	 BP
	  MOV	 BP,SI
	  SUB	 BP,CX
	  IDIV	 BP
	  ADD	 AX,BX
	  MOV	 SI,_gdvw_x2
	  MOV	 DI,AX
	  JMP	 dline01
dline06:
	  MOV	 BP,DI
	  SUB	 BP,BX
	  MOV	 AX,_gdvw_x1
	  SUB	 AX,CX
	  IMUL	 BP
	  MOV	 BP,SI
	  SUB	 BP,CX
	  IDIV	 BP
	  ADD	 AX,BX
	  MOV	 SI,_gdvw_x1
	  MOV	 DI,AX
	  JMP	 dline01
dline07:
	  MOV	 BP,SI
	  SUB	 BP,CX
	  MOV	 AX,_gdvw_y2
	  SUB	 AX,BX
	  IMUL	 BP
	  MOV	 BP,DI
	  SUB	 BP,BX
	  IDIV	 BP
	  ADD	 AX,CX
	  MOV	 SI,AX
	  MOV	 DI,_gdvw_y2
	  JMP	 dline01
dline08:
	  MOV	 BP,SI
	  SUB	 BP,CX
	  MOV	 AX,_gdvw_y1
	  SUB	 AX,BX
	  IMUL	 BP
	  MOV	 BP,DI
	  SUB	 BP,BX
	  IDIV	 BP
	  ADD	 AX,CX
	  MOV	 SI,AX
	  MOV	 DI,_gdvw_y1
	  JMP	 dline01
dline09:

	  mov	 dx,03CEh	       ; Load address of EGA graphic controller
	  mov	 ah,_gdmerge	       ; Load current merge setting
	  mov	 al,03h 	       ; Load address of merge register
	  out	 dx,ax		       ; Set the graphic controller function
	  mov	 ax,00205h	       ; Load parameters for write mode #2
	  out	 dx,ax		       ; Set the graphic controller write mode

	  mov	 dx,si

	  cmp	 dx,cx		       ; Is X2 >= X1?
	  jae	 noxchg 	       ;   Yes, continue
	  xchg	 cx,dx		       ;   No, then switch (X1,Y1) with (X2,Y2)
	  xchg	 bx,di		       ;     ...
noxchg:
	  sub	 dx,cx		       ; Compute Delta X := X2 - X1;
	  sub	 di,bx		       ; Compute Delta Y := Y2 - Y1;

	  mov	 si,bx		       ; Compute SI = Y * 80 for row offset
	  shl	 si,1		       ;   ... (SI = Y * 2)
	  shl	 si,1		       ;   ... (SI = Y * 4)
	  add	 si,bx		       ;   ... (SI = Y * 5)
	  shl	 si,1		       ;   ... (SI = Y * 10)
	  shl	 si,1		       ;   ... (SI = Y * 20)
	  shl	 si,1		       ;   ... (SI = Y * 40)
	  shl	 si,1		       ;   ... (SI = Y * 80)
	  mov	 bx,cx		       ; Compute BX = X / 8 for column offset
	  shr	 bx,1		       ;   ... (BX = X / 2)
	  shr	 bx,1		       ;   ... (BX = X / 4)
	  shr	 bx,1		       ;   ... (BX = X / 8)
	  add	 bx,si		       ; Compute BX = BX + SI, first byte

	  mov	 si,dx		       ; SI now contains delta X

	  mov	 dx,03CEh	       ;
	  mov	 al,8		       ;
	  out	 dx,al		       ;
	  inc	 dx		       ;

	  and	 cl,7		       ; Compute AL = starting bit mask
	  mov	 al,080h	       ;   ... load default of bit 0
	  ror	 al,cl		       ;   ... rotate to the starting bit mask

	  cmp	 _gds_flg,0	       ; Do we use the current line style?
	  jne	 style
	  jmp	 line
style:
	  push	 si
	  les	 si,_gdstyle
	  cmp	 byte ptr es:[si],1
	  jne	 style00
	  mov	 ah,es:[si+1]
	  pop	 si
	  jmp	 line
style00:
	  pop	 si
	  mov	 [bp+4],si
	  mov	 [bp+6],di
	  or	 di,di
	  jns	 style02
	  neg	 di
	  cmp	 di,si
	  ja	 style01
	  jmp	 octant8
style01:
	  jmp	 octant7
style02:
	  cmp	 di,si
	  ja	 octant2
octant1:
	  mov	 cx,si		       ; Count Down = DX
	  mov	 di,si		       ; Error Register = -DX/2
	  shr	 di,1		       ;   ...
	  neg	 di		       ;   ...
	  les	 si,_gdstyle
	  mov	 ah,es:[si]
	  inc	 si
octant1L:
	  call	 putdot

	  ror	 al,1		       ; Compute X := X + 1;
	  adc	 bx,0		       ;   ...
	  add	 di,[bp+6]	       ;
	  jle	 octant1L	       ;
	  add	 bx,80		       ; Compute Y := Y + 1;
	  sub	 di,[bp+4]	       ;
	  jmp	 short octant1L        ;

octant2:
	  mov	 cx,di		       ; Count Down = DY
	  shr	 di,1		       ; Error Register = -DX/2
	  neg	 di		       ;   ...
	  les	 si,_gdstyle
	  mov	 ah,es:[si]
	  inc	 si
octant2L:
	  call	 putdot

	  add	 bx,80		       ; Compute Y := Y + 1;
	  add	 di,[bp+4]	       ;
	  jle	 octant2L	       ;
	  ror	 al,1		       ; Compute X := X + 1;
	  adc	 bx,0		       ;   ...
	  sub	 di,[bp+6]	       ;
	  jmp	 short octant2L        ;

octant7:
	  mov	 cx,di		       ; Count Down = DY
	  shr	 di,1		       ; Error Register = -DY/2
	  neg	 di		       ;   ...
	  les	 si,_gdstyle
	  mov	 ah,es:[si]
	  inc	 si
octant7L:
	  call	 putdot

	  sub	 bx,80		       ; Compute Y := Y + 1;
	  add	 di,[bp+4]	       ;
	  jle	 octant7L	       ;
	  ror	 al,1		       ; Compute X := X + 1;
	  adc	 bx,0		       ;   ...
	  add	 di,[bp+6]	       ;
	  jmp	 short octant7L        ;

octant8:
	  mov	 cx,si		       ; Count Down = DX
	  mov	 di,si		       ; Error Register = -DX/2
	  shr	 di,1		       ;   ...
	  neg	 di		       ;   ...
	  les	 si,_gdstyle
	  mov	 ah,es:[si]
	  inc	 si
octant8L:
	  call	 putdot

	  ror	 al,1		       ; Compute X := X + 1;
	  adc	 bx,0		       ;   ...
	  sub	 di,[bp+6]	       ;
	  jle	 octant8L	       ;
	  sub	 bx,80		       ; Compute Y := Y + 1;
	  sub	 di,[bp+4]	       ;
	  jmp	 short octant8L        ;
line:
	  mov	 ah,_gdcolor	       ; Load current color setting
	  push	 bp
	  mov	 es,_gdgseg
	  or	 di,di
	  jns	 line01
	  neg	 di
	  cmp	 di,si
	  ja	 lineoct7
	  jmp	 short lineoct8
line01:
	  cmp	 di,si
	  ja	 lineoct2

lineoct1:
	  mov	 cx,si		       ; Count Down = DX
	  mov	 bp,si		       ; Error Register = -DX/2
	  shr	 bp,1		       ;   ...
	  neg	 bp		       ;   ...
lineoct1L:
	  out	 dx,al
	  cmp	 byte ptr es:[bx],0
	  mov	 es:[bx],ah
	  dec	 cx
	  js	 linedone
	  ror	 al,1
	  adc	 bx,0
	  add	 bp,di
	  jle	 lineoct1L
	  add	 bx,80
	  sub	 bp,si
	  jmp	 short lineoct1L

lineoct2:
	  mov	 cx,di		       ; Count Down = DY
	  mov	 bp,di		       ; Error Register = -DY/2
	  shr	 bp,1		       ;   ...
	  neg	 bp		       ;   ...
lineoct2L:
	  out	 dx,al
	  cmp	 byte ptr es:[bx],0
	  mov	 es:[bx],ah
	  dec	 cx
	  js	 linedone
	  add	 bx,80
	  add	 bp,si
	  jle	 lineoct2L
	  ror	 al,1
	  adc	 bx,0
	  sub	 bp,di
	  jmp	 short lineoct2L

lineoct7:
	  mov	 cx,di		       ; Count Down = -DY
	  mov	 bp,di		       ; Error Register = DY/2
	  shr	 bp,1		       ;   ...
	  neg	 bp		       ;
lineoct7L:
	  out	 dx,al
	  cmp	 byte ptr es:[bx],0
	  mov	 es:[bx],ah
	  dec	 cx
	  js	 linedone
	  sub	 bx,80
	  add	 bp,si
	  jle	 lineoct7L
	  ror	 al,1
	  adc	 bx,0
	  sub	 bp,di
	  jmp	 short lineoct7L

lineoct8:
	  mov	 cx,si		       ; Count Down = DX
	  mov	 bp,si		       ; Error Register = -DX/2
	  shr	 bp,1		       ;   ...
	  neg	 bp		       ;   ...
lineoct8L:
	  out	 dx,al
	  cmp	 byte ptr es:[bx],0
	  mov	 es:[bx],ah
	  dec	 cx
	  js	 linedone
	  ror	 al,1
	  adc	 bx,0
	  add	 bp,di
	  jle	 lineoct8L
	  sub	 bx,80
	  sub	 bp,si
	  jmp	 short lineoct8L

linedone:
	  pop	 bp
	  jmp	 short alldone

putdot:
	  out	 dx,al
	  push	 ax		       ; Save AX
	  mov	 al,es:[si]	       ; Load color for next pixels in segment
	  mov	 es,_gdgseg	       ; Load graphic segment register (ES)
	  mov	 ah,es:[bx]	       ; Load current byte in to EGA latches
	  mov	 es:[bx],al	       ; Set the current bit to graphic memory
	  mov	 es,word ptr _gdstyle+2 ; Load line style segment register (ES)
	  pop	 ax		       ; Restore AX
	  dec	 ah		       ; Decrement line style color counter
	  jnz	 putdot1	       ; If non-zero, jump over reset code
	  mov	 si,word ptr _gdstyle  ; End of line style array, reset to the
	  mov	 ah,es:[si]	       ;   ... beginning of the array
putdot1:
	  inc	 si		       ; Increment to next color in line style
	  dec	 cx		       ; Decrement the length of the line
	  js	 done		       ; If negative, then all done
	  ret
done:
	  pop	 ax
alldone:
	  mov	 dx,03CEh
	  mov	 ax,00003h
	  out	 dx,ax
	  mov	 al,005h
	  out	 dx,ax
	  mov	 ax,0FF08h
	  out	 dx,ax
theend:
	  xor	 ah,ah
	  mov	 al,_gdc_flg

	  pop	 di
	  pop	 si
	  pop	 bp
	  ret

_gpline   endp

_text	  ends
	  end