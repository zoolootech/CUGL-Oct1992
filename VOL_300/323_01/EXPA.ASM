;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; expa.asm - support routines for EXPLOD.C.  Compile with "masm /mx expa ;".
;
; NOTE:
;   Make sure one of DLC or TURBOC is defined below.  C compilers other 
;   than Datalight C and Turbo C will probably require changes to the 
;   segment naming.  This file has been tested with the Arrowsoft Assembler 
;   1.00d and MASM 5.0.
;
; (C) 1989 Dennis Lo
; You are free to use and distribute this source code, provided that the
; authors' names remain in the code, and that modified versions are clearly 
; distinguished from the original.
;
; 89/06/24 Dennis Lo		(V1.0) Initial release. 
; 89/07/03 Dennis Lo		(V1.1) Added ifdefs for linking with Turbo C.
; 89/07/26 Erik Liljencrantz	Added EGA support with autodetection
;				Restores previous CRT textmode on exit
; 89/08/22 Dennis Lo		(V1.2) Allow frame buffer to be in any segment
;				Optimized HGC/CGA frame draw loop
;				Restricted EGA colours to only the bright ones
;				 to get rid of the flickering effect.
; 89/09/03 Erik Liljencrantz	Added VGA support with autodetection
; 89/09/23 Dennis Lo		Added DOS memory allocation routines
;				Cosmetic changes.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;------------------------------------------------------------------
;        *** IMPORTANT: Uncomment one of the following ***
;
; uncomment this for Datalight C *******
;DLC	 equ	 1
;
; uncomment this for Turbo C ********
TURBOC	equ	1
;
;------------------------------------------------------------------


;;;;;;;;;;;;;;;;;;;;;;;;; SEGMENT NAMES ;;;;;;;;;;;;;;;;;;;;;;;;;
;segment names for Datalight C
ifdef DLC
pgroup		group	prog
prog		segment byte public 'prog'
		assume	cs:pgroup
prog		ends
dgroup		group	data
data		segment word public 'data'
		assume ds:dgroup
data		ends
endif

;segment names for Turbo C
ifdef TURBOC
		name	t
_TEXT		segment byte public 'CODE'
DGROUP		group	_DATA,_BSS
		assume	cs:_TEXT,ds:DGROUP,ss:DGROUP
_TEXT		ends
_DATA		segment word public 'DATA'
_d@		label	byte
_DATA		ends
_BSS		segment word public 'BSS'
_b@		label	byte
_BSS		ends
endif


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; CONST ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	HGC port addresses
config		equ	03bfh
index		equ	03b4h
cntrl		equ	03b8h
;	HGC control codes
scrn_on		equ	8
grph		equ	2
text		equ	20h

par		equ	4		;stack offset of 1st C call parameter
DEADPOINT	equ	32767		;flag for dead point in frame table


;;;;;;;;;;;;;;;;;;;;;;;;;;;;; DATA ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ifdef DLC
data		segment word public 'data'
endif
ifdef TURBOC
_DATA		segment word public 'DATA'
endif

Vidtype 	db	?		;video card type
Vidseg		dw	?		;video mem segment
Vidsize 	dw	?		;video mem size
SaveMode	DB	?		;Save video text mode
Color		db	?		;Current frame colour (for EGA)

;	Table of bit values for plotting points
;masktable	db	128,64,32,16,8,4,2,1	;1-pixel wide points
masktable	db	192,96,48,24,12,6,3,1	;2-pixel wide points

;	HGC 6845 config tables 
hgc_gtable	db	35h,2dh,2eh,07h
		db	5bh,02h,57h,57h
		db	02h,03h,00h,00h
hgc_ttable	db	61h,50h,52h,0fh
		db	19h,06h,19h,19h
		db	02h,0dh,0bh,0ch

;	Palette register table for EGA
;	Consists of colour values for registers 0-15, plus border colour
palette		db	00h,07h,3ah,3bh,3ch,3eh,3fh,07h
		db	3ah,3bh,3ch,3eh,07h,3ah,3bh,3ch,00h

ifdef DLC
data		ends
endif
ifdef TURBOC
_DATA		ends
endif


;;;;;;;;;;;;;;;;;;;;;;;;;;;;; CODE ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ifdef DLC
prog		segment byte public 'prog'
endif
ifdef TURBOC
_TEXT		segment byte public 'CODE'
endif


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Return the number of paragraphs of memory available.
; int num_para = DosMemLeft ();
;
		public	_DosMemLeft
_DosMemLeft	proc	near
		push	bp
		mov	bp,sp
		push	es

		mov	bx,0f000h	;Call DOS malloc with a request for
		mov	ah,48h		; 960K to make sure it fails
		int	21h
		jc	memleft_cont

		xor	ax,ax		;Error: DOS malloc() succeeded!
		jmp	memleft_end	;Return 0

memleft_cont:
		mov	ax,bx		;Return mem size remaining

memleft_end:
		pop	es
		pop	bp
		ret
_DosMemLeft	endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Allocate a block of memory 
; int segment_addr = DosAlloc (size_in_paragraphs);
; Returns 0 if unsucessful.
;
		public	_DosAlloc
_DosAlloc	proc	near
		push	bp
		mov	bp,sp
		push	es

		mov	bx,par[bp]	;get # paragraphs requested
;		add	bx,15		;# paragraphs = (# bytes + 15) / 16
;		shr	bx,1
;		shr	bx,1
;		shr	bx,1
;		shr	bx,1
		mov	ah,48h		;call DOS malloc
		int	21h
		jnc	alloc_end
		xor	ax,ax		;if failure then return 0
alloc_end:
		pop	es
		pop	bp
		ret
_DosAlloc	endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Free a block of memory allocated by DosAlloc
; DosFree (int segment_addr);
; Returns 0 if successful, 1 if not.
;
		public	_DosFree
_DosFree	proc	near
		push	bp
		mov	bp,sp
		push	es

		mov	ax,par[bp]	;get segment address to free at
		mov	es,ax
		mov	ah,49h		;call DOS free
		int	21h

		jc	free_end	;if error then return DOS err code
		xor	ax,ax		;else return 0
free_end:
		pop	es
		pop	bp
		ret
_DosFree	endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Nonblocking keyboard read - returns ASCII char
; int key = ChkKey();
;
		public	_ChkKey
_ChkKey		proc	near
		push	si
		push	di

		mov	ah,1
		int	16h
		jz	nokey		;if no key then return
		mov	ax,0		;else get the key out of the buffer
		int	16h
		mov	ah,0
		jmp	chkret
nokey:
		mov	ax,0
chkret:
		pop	di
		pop	si
		ret
_ChkKey		endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Guess video card type.  Returns 'h', 'c', 'e', or 'v'.
; int card = gr_card();
;
		public	_gr_card
_gr_card	proc	 near
		push	bp
		push	si
		push	di
	
		mov	ah,0Fh
		int	10h
		MOV	SaveMode,AL	;Save textmode to restore later...
		MOV	BL,AL
		mov	ax,'h'		; Mono, assume HGC
		cmp	bl,07h		;
		je	card_done	;
		MOV	AX,1200h
		MOV	BX,0FF10h
		MOV	CX,00FFh
		INT	10h
		CMP	CL,12	   ;CL<12
		JAE	Card_CGA
		CMP	BH,1	   ;BH<=1
		JA	Card_CGA
		CMP	BL,3	   ;BL<=3
		JA	Card_CGA
;OK, either EGA or VGA
;Use INT 10/AH=1A00 to determine which one...
		MOV	AX,1A00h
		INT	10h
		CMP	AL,1Ah		;Returns AL=1Ah if function supported
		JNE	Card_EGA
		CMP	BL,08h		;VGA with analog color display
		JNE	Card_EGA
		MOV	AX,'v'		;Assume VGA
		JMP SHORT Card_Done
Card_EGA:
		MOV	AX,'e'		;Assume EGA
		JMP SHORT Card_Done
Card_CGA:
		MOV	AX,'c'		;Assume CGA
Card_Done:
		pop	di
		pop	si
		pop	bp
		ret
_gr_card endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Set video card type
; gr_setcard (char video_card);
;
		public	_gr_setcard
_gr_setcard	proc	near
		push	bp
		mov	bp,sp
	
		mov	ax,par[bp]	;get video_card parameter
		mov	Vidtype,al

	;Set video mem segment
		cmp	al,'h'		
		jne	not_herc
		mov	ax,0b000h	;Hercules
		mov	cx,8000h	; 32K bytes
		jmp	s_end
not_herc:
		cmp	al,'e'		
		jne	not_ega
		mov	ax,0A000h	;EGA
		mov	cx,6D60h	; 28000 bytes (but in four planes)
		jmp	s_end
not_ega:
		cmp	al,'v'
		jne	not_vga
		mov	ax,0A000h	;VGA
		mov	cx,9600h	; 38400 bytes (but in four planes)
		jmp	s_end
not_vga:
		mov	ax,0b800h	;CGA
		mov	cx,4000h	; 16K bytes
s_end:
		mov	Vidseg,ax
		mov	Vidsize,cx

		pop	bp
		ret
_gr_setcard endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Set video card for graphics mode
; gr_gmode();
;
		public	_gr_gmode
_gr_gmode	proc	near
		push	bp
		push	es
		push	ds
		push	si

		mov	al,Vidtype
		cmp	al,'h'
		jne	NotHerc
		call	hgc_gmode      ;HGC
		jmp	g_end
NotHerc:
		cmp	al,'e'
		jne	NotEga
		call	ega_gmode      ;EGA
		jmp	g_end
NotEga:
		cmp	al,'v'
		jne	NotVga
		call	vga_gmode      ;VGA
		jmp	g_end
NotVga:
		call	cga_gmode      ;CGA
g_end:
		pop	si
		pop	ds
		pop	es
		pop	bp
		ret
_gr_gmode	endp


;Set HGC graphics mode, page 0
hgc_gmode:
		mov	dx,config
		mov	al,3
		out	dx,al

		mov	al,grph
		lea	si,hgc_gtable
		mov	bx,0
		mov	cx,4000h
		call	hgc_setmd
		ret


;Set EGA graphics mode
ega_gmode:
		mov	ah,00h		;Set 640x350 16-colour mode
		mov	al,10h		;Probably one of the few changes for VGA
		int	10h		;640x480 resolution

		mov	ax,ds		;set colour palette
		mov	es,ax
		lea	dx,palette
		mov	ax,1002h
		int	10h
		ret

;Set VGA graphics mode
vga_gmode:
		mov	ah,00h		;Set 640x350 16-colour mode
		mov	al,12h		;Mode 12h: 640x480 in 16 colours
		int	10h		

		mov	ax,ds		;set colour palette
		mov	es,ax
		lea	dx,palette
		mov	ax,1002h
		int	10h
		ret

;Set CGA graphics mode
cga_gmode:
		mov	ah,00h		;set 320x200 4-colour mode
		mov	al,04h
		int	10h

		mov	ah,0bh		;set green-red-brown colour palette
		mov	bx,0100h
		int	10h
		ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Set text mode.
; gr_tmode();
		public	_gr_tmode
_gr_tmode	proc  near
		push	bp
		push	es
		push	ds
		push	si

		mov	al,Vidtype
		cmp	al,'h'
		jne	t_cga
t_hgc:		call	hgc_tmode
		jmp	t_end
t_cga:		call	cga_tmode	  ;for CGA, EGA, & VGA
t_end:
		pop	si
		pop	ds
		pop	es
		pop	bp
		ret
_gr_tmode	endp


;Set HGC text mode
hgc_tmode:
		mov	dx,config
		mov	al,0
		out	dx,al

		mov	al,text
		lea	si,hgc_ttable
		mov	bx,720h
		mov	cx,2000h
		call	hgc_setmd
		ret


;Set CGA & EGA & VGA text mode
cga_tmode:
		mov	ah,00h
		mov	al,SaveMode   ;Restore previous textmode
		int	10h
		ret


;;;;;;;;;;;;;;;;;;;;;;;;; hgc_setmd (Hercules support)
; Set display mode to graphics or text.  Local.
; params:
;	al = value to be output to 6845 control port
;	si = 6845 param table
;	cx = # of words to be cleared
;	bx = blank value
hgc_setmd	proc	near
		push	di
		push	ds
		push	es
		push	ax
		push	bx
		push	cx

;	change mode but without scrn_on
		mov	dx,cntrl
		out	dx,al

;	initialize the 6845
		mov	ax,ds
		mov	es,ax		;also point es:si to param table
	
		mov	dx,index
		mov	cx,12		;12 params to be output
		xor	ah,ah		;starting from reg. 0
parms:
		mov	al,ah
		out	dx,al		;output 6845 reg. index
	
		inc	dx
		lodsb
		out	dx,al		;output 6845 reg. data
	
		inc	ah
		dec	dx
		loop	parms

;		clear the display buffer
		pop	cx
		mov	ax,0b000h
		cld

		mov	es,ax
		xor	di,di
		pop	ax
		rep	stosw

;		scrn_on, page 0
		mov	dx,cntrl
		pop	ax
		add	al,scrn_on
		out	dx,al

		pop	es
		pop	ds
		pop	di
		ret
hgc_setmd	endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Returns video mem addr of a (x,y) point
; int video_addr = gr_addr (int x, int y);
;
par_x		equ	par	;in:  x
par_y		equ	2+par	;in:  y

		public	_gr_addr
_gr_addr	proc   near
		push	bp
		mov	bp,sp
		push	si
		push	di

		mov	bx,par_x[bp]
		mov	cx,par_y[bp]
		call	graddr

		pop	di
		pop	si
		pop	bp
		ret
_gr_addr	endp


;
;Local routine to calculate video address.
;On entry, BX=x, CX=y.	Returns addr in AX.
;
graddr		proc	near
		mov	al,Vidtype
		cmp	al,'h'
		je	hgc_addr
		cmp	al,'e'
		je	ega_addr
		cmp	al,'v'
		je	vga_addr
		jmp	cga_addr
graddr		endp

;Calc HGC addr = 90 * (y / 4)  +  2000h * (y & 3)  +  x / 8
hgc_addr:
		;loc = (y / 4) * 90
		mov	ax,cx
		shr	ax,1
		shr	ax,1

		;ax * 90 = ax*2 + ax*8 + ax*16 + ax*64
		shl	ax,1			;*2
		mov	dx,ax
		shl	ax,1			;*4
		shl	ax,1			;*8
		add	dx,ax
		shl	ax,1			;*16
		add	dx,ax
		shl	ax,1			;*32
		shl	ax,1			;*64
		add	ax,dx
		MOV	DI,AX			;store sum in DI

		;loc += (y & 3) * 2000H
		AND	CX,3
		MOV	AX,2000H
		MUL	CX			;(msb ignored)
		ADD	DI,AX

		;loc += (x / 8)
		mov	ax,bx
		shr	ax,1
		shr	ax,1
		shr	ax,1
		add	ax,di
		ret


;Calc EGA addr = 80 * y + x / 8
ega_addr:
vga_addr:
		MOV	AX,cx
		SHL	AX,1
		SHL	AX,1
		SHL	AX,1
		SHL	AX,1
		MOV	DX,AX
		SHL	AX,1
		SHL	AX,1
		ADD	DX,AX
		MOV	AX,bx
		SHR	AX,1
		SHR	AX,1
		SHR	AX,1
		ADD	AX,DX
		RET


;Calc CGA addr = 80 * (y / 4)  +  2000h * (y & 3)  +  x / 8
cga_addr:
		;loc = (y / 2) * 80
		MOV	AX,cx
		SHR	AX,1
	
		MOV	DL,80
		MUL	DL
		MOV	DI,AX			;store sum in DI

		;loc += (y & 1) * 2000H
		MOV	AX,2000H
		AND	CX,1
		jz	ca_noadd
		ADD	DI,AX
ca_noadd:
		;loc += (x / 8)
		MOV	AX,bx
		shr	ax,1
		shr	ax,1
		shr	ax,1
		ADD	ax,di
		ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Returns video mem addr of a (x,y) point
; int video_addr = gr_addr (int x, int y);
;
		public	_gr_value
_gr_value proc	near
		push	bp
		mov	bp,sp
		mov	bx,par_x[bp]
		call	grvalue
		pop	bp
		ret
_gr_value endp

;
; Local: returns the byte value of a (x,y) point in AL
; Input: BX=x
; Output: AL = 2 << [7 - (x & 7)]
;
grvalue		proc   near
		and	bx,7
		mov	al,masktable[bx]
		ret
grvalue endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; gr_store_pt (x, y, dest_seg, dest_ofs, centre_addr)
;
dest_seg	equ	par+4
dest_ofs	equ	par+6
cent_addr	equ	par+8 

		public	_gr_store_pt
_gr_store_pt	proc	near
		push	bp
		mov	bp,sp
		push	es
		push	si
		push	di
	
		mov	bx,par_x[bp]		;get addr
		cmp	bx,DEADPOINT
		je	store_dead
		mov	cx,par_y[bp]
		call	graddr
		sub	ax,cent_addr[bp]
		mov	dx,ax
	
		mov	bx,par_x[bp]		;get value
		call	grvalue
		mov	cl,al
store_pt:	
		mov	bx,dest_ofs[bp] 	 ;get dest addr
		mov	ax,dest_seg[bp]
		mov	es,ax
		mov	es:[bx],dx		;store addr in dest
		mov	es:[bx+2],cl		;store value in dest
	
		pop	di
		pop	si
		pop	es
		pop	bp
		ret
_gr_store_pt	endp

store_dead:	mov	dx,bx
		jmp	store_pt

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Plot one frame of 3-byte (offset, value) point records.
; gr_frplot (num_points, frame_seg, frame_offset, centre_addr, draw_or_erase)
;
fpar_nump	equ	par		;No. of points per frame
fpar_frm_seg	equ	par+2		;frame segment
fpar_frm_ofs	equ	par+4		;frame offset
fpar_centre	equ	par+6		;video addr of explosion's centre
fpar_mode	equ	par+8		;for EGA: 0 means erase, 1 means draw

		public	_gr_frplot
_gr_frplot proc near
		push	bp
		mov	bp,sp
		push	ds
		push	es
		push	si
		push	di

		mov	cx,fpar_nump[bp]
		mov	si,fpar_frm_ofs[bp]
		mov	dx,fpar_centre[bp]
		mov	di,3
		cmp	VidType,'e'
		je	fr_ega
		cmp	VidType,'v'
		je	fr_vga

;Plot frame on HGC or CGA
fr_cga_hgc:
		mov	ax,Vidseg		;get video segment 
		mov	es,ax
		mov	ax,fpar_frm_seg[bp]	;get frame table segment
		mov	ds,ax
frloop:
		mov	bx,[si] 		;get point offset
		add	bx,dx			;add centre to offset
		mov	al,[si+2]		;get point's byte value
		xor	es:[bx],al
frnext:
		add	si,di
		loop	frloop
fr_end:
		pop	di
		pop	si
		pop	es
		pop	ds
		pop	bp
		ret

;Plot frame on EGA
fr_vga:
fr_ega:
		MOV	BL,fpar_mode[BP]
		MOV	AX,00F02H
		push	dx
		MOV	DX,03C4H      ;3C4
		OUT	DX,AX	      ;Map Mask = enable write in all planes
		MOV	DX,03CEH
		MOV	AX,0FF01H
		OUT	DX,AX
		MOV	AX,00005H
		OUT	DX,AX	      ;Mode register
		CMP	BL,0
		JE	fr_Clear
		MOV	AX,01803H
		OUT	DX,AX		    ;Select XOR of data
	
		INC	Color		    ;Increment colorcounter
		MOV	AH,Color
	
;Alternative coloring of the explosion...
;		MOV	  AX,40h	      ;Get color from timer...!
;		MOV	  ES,AX 	      ;
;		MOV	  AH,ES:[06Ch]	      ;

;Alternative explosion colouring using "random" numbers from ROM
;		inc	Colour
;		mov	ax,0f000h
;		mov	es,ax
;		mov	bl,Color
;		xor	bh,bh
;		mov	ah,es:[bx]
;		or	ah,ah
;		and	ah,15
;		jne	notblack
;		inc	ah
;notblack:	
					;here ah should contain colour byte
		JMP SHORT fr_cont
fr_Clear:
		MOV	AX,00003H
		OUT	DX,AX		    ;Select unmodified data
		XOR	AH,AH
fr_cont:
		XOR	AL,AL
		OUT	DX,AX
		MOV	AL,08
		OUT	DX,AL			; Select register 8 again
		INC	DX

		mov	ax,Vidseg		;get video segment
		mov	es,ax
		mov	ax,fpar_frm_seg[bp]	;get frame table segment
		mov	ds,ax
		pop	bp			;get centre addr pushed earlier
egaloop:
		mov	bx,[si] 		;get point offset
		cmp	bx,DEADPOINT		;check if point is dead
		je	eganext
		add	bx,BP			;add centre to offset 
		mov	al,[si+2]		;get point's byte value
		OUT	DX,AL
		XCHG	AL,ES:[BX]
eganext:
		add	si,di
		loop	egaloop
		JMP	fr_end

_gr_frplot	endp


ifdef DLC
prog		ends
endif
ifdef TURBOC
_TEXT		ends
endif
	end
