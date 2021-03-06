;
;5/15/80
;
;patch courtesy of Andy Johnson-Laird
;
;******************************CAUTION******************************
;
;This patch is for grown-ups who know what they are doing.
;Note that if files of the same name are present on both USER 0 and
;the logged-in USER number, the OPEN, SEARCH for FIRST, SEARCH for NEXT
;and READ SEQUENTIAL functions will match directory entries on a first-come
;first-served basis.  This will cause exciting results if multiple extent
;files of the same name are present on both USER 0 and logged-in USER.
;
;Recommended use:
;
;	1) Keep on USER 0 only commonly used programs (Files.COM)
;	2) Set these programs to $SYS to reduce directory clutter.
;	3) Set these programs to $R/O to avoid accidental erasure of
;	   invisible programs.
;	4) NEVER place on any other USER number programs of the same name.
;	5) Have turn-key applications cold-boot directly into the assigned
;	   USER number.  The USER number is the high nibble of address 4.
;
;
msize	equ	20		;set your memory size here
;
bdos	equ	(msize-20)*1024+3C00h
;
patch$area	equ	??????	;this will depend on YOUR system, and
				;should probably be in your BIOS area
;
	org	bdos+075eh
	jmp	patch		;overlays:	mov	a,b
;						cpi	0dh
;
;
	org	patch$area
patch:	mov	a,b
	ora	a
	jnz	patch2
	ldax	d
	cpi	0e5h
	jz	patch2
	mov	a,m
	ora	a
	jz	bdos+077ch
patch2:	mov	a,b
	cpi	0dh
	jmp	bdos+0761h
;
	end
erved basis.  This will cause exciting results if multiple extent
;files of the same name are