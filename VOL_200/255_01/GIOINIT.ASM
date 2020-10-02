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

;------ VEGA HiRes (640x480) Data:

Vega_Seg	segment at 0C000h	;VEGA ROM Segment
		org	028h
mul8_addr	label	word		;Address of emulation routine
		org	02Ah
version 	label	byte		;Version string
		org	03FFEh
feature 	label	byte		;Feature flags
Vega_Seg	ends


VerChk  db      'VEGA BIOS Code, ',0    ;VEGA BIOS Version string subset

;
;	Alt. Mode 10: 640 x 480 16-color graphics (>64k ram):
;
HiRes_Params	label	byte
	db	80, 36, 14
	dw	0B400h
	db	001h, 00Fh, 000h, 006h
	db	0ABh
	db	064h, 04Fh, 053h, 021h, 053h, 000h
	db	0F0h, 01Fh, 000h, 000h, 000h, 000h
	db	000h, 000h, 000h, 000h, 0E0h, 02Ch
	db	0DFh, 028h, 00Fh, 0E1h, 00Ch, 0E3h
	db	0FFh
	db	000h, 001h, 002h, 003h, 004h, 005h
	db	014h, 007h, 038h, 039h, 03Ah, 03Bh
	db	03Ch, 03Dh, 03Eh, 03Fh, 001h, 000h
	db	00Fh, 000h
	db	000h, 000h, 000h, 000h, 000h, 000h
	db	005h, 00Fh, 0FFh


Emulate label	dword			;Far pointer to Emulate routine
EmulOff dw	0
EmulSeg dw	0

;------ End of VEGA Data...

_data	  ends

_text     segment byte public 'code'

	  assume cs:_text,ds:dgroup
	  public _Check_Vega, _Set_Hires

;
;	Check EGA ROM for VEGA BIOS version string, and 24 Mhz clock feature
;
;	Exit:	Carry set:	VEGA Deluxe found
;		Carry clear:	VEGA Deluxe not found
;
_Check_Vega	proc	near

	  push	 bp
	  mov	 bp,sp
	  push	 si
	  push	 di
	  push	 es
	  cld				;Strings go forward

	mov	ah,12H			;Bios alternate select
	mov	bl,10H			;Return EGA information
	int	10H
	or	bh,bh			;check for color monitor
	jnz	Chk_Fail		;if bh=1, mono monitor

	mov	ax,Vega_Seg		;Get VEGA ROM seg
	mov	es,ax			;copy into ES
	assume	es:Vega_Seg

	lea	di,version		;Point at ROM version string
	lea	si,VerChk		;Point at version string subset
Chk_Loop:
	lodsb				;Get source byte
	or	al,al			;End of string?
	jz	Chk_Equal		;If at end, set carry (Equal)
	cmp	al,es:[di]		;Does this byte match?
	jne	Chk_Fail		;No, exit (Not Equal)
	inc	di			;Yes, advance dest pointer
	jmp	Chk_Loop		;Loop over source string
Chk_Fail:
	xor	ax,ax			;else clear carry
	jmp	short Chk_Exit		;and exit
Chk_Equal:
        test    es:feature,1            ;Is this a "Deluxe" VEGA?
        jz      Chk_Fail                ;No, can't do sexy hires modes
	mov	 ax,1			;Yes, Set carry flag
Chk_Exit:

	pop	es			;Restore regs
	pop	di
	pop	si
	pop	bp
	ret

_Check_Vega	endp

;
;	Set VEGA HiRes (640x480) mode
;
_Set_Hires	proc	near

	  push	 bp
	  mov	 bp,sp
	  push	 si
	  push	 di
	  push	 es

	mov	bx,Vega_Seg		;Setup to get Vega Segment
	mov	es,bx
	assume	es:Vega_Seg

	mov	bx,es:Mul8_addr 	;Get offset of emulation routine
	mov	EmulOff,bx		;Save offset
	mov	EmulSeg,es		;and segment

	push	ds			;Copy code segment
	pop	es			;into ES
	lea	di,HiRes_Params 	;Get address of HiRes parameters
	mov	dx,3D4h 		;Get CRT port address
	mov	ax,9			;Get special mode
	call	Emulate 		;Set EGA registers from param table

	mov	ax,040H 		;Get the bios data segment
	mov	es,ax
	mov	word ptr es:[4AH],80	;Set the number of columns
	mov	byte ptr es:[84H],33	;Set the number of rows

	call	Pal_On			;Turn the palette on

	 pop	 es			;Restore registers
	 pop	 di
	 pop	 si
	 pop	 bp
	 ret				;Return

_Set_Hires	endp

;
;	Turn Palette On
;
Pal_On	proc	near

	mov	dx,3DAh 		;Get status port address (Color)...
	in	al,dx			;Set attribute flip-flop
	mov	dx,3C0h
	mov	al,20h
	out	dx,al			;Enable palette
	ret

Pal_On	endp

_text	  ends
	  end
