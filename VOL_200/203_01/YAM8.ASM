TITLE YAM8: CRC SUBS
PAGE 50,132
;DATE (24 jun 85)
;
;CRCK is a program to compute 
;a CYCLIC-REDUNDANCY-CHECK number based on the
;CCITT standard polynominal:
;   X^16 + X^15 + X^13 + X^7 + X^4 + X^2 + X + 1
;
;Useful for checking accuracy of file transfers.
;More accurate than a simple checksum.
;
;**************************************************
;
;	       unsigned crck(buffer, bufsize, oldcrc)
;
;	       At start of packet, oldcrc is set to 0
;
;	       crc is accumulated by:
;	       	        oldcrc=crck(buffer, bufsize, oldcrc);
;
;	       crck for file is final value of oldcrc
;
;	       A Short Hostory of this function and crckfile() in yam7.c"
;
;	       1.  First version used getc and called crck once per char.
;	       this took 39.2 seconds to crck all the yam C files (12357)
;
;	       2.  Then crckfile was recoded to use read() instead of getc.
;	       Time: 19.1 seconds
;
;	       3.  Several small changes in crckfile were unsuccessful in
;	       reducing this time.
;
;	       4.  crck and crckfile recoded to call crck once per sector.
;	       This reduced time to 11.7 seconds, same as crck itself.
;	       That is the current version.  Note that the CRC polynomial used
;	       here is somewhat unusual; the only thing I know sure is that
;	       the answers agree with those given by the CRCK program -hence the
;	       function name.
;

; SMALL MODEL
INCLUDE \LC\S\DOS.MAC

	pseg
public crck
crck:
	push	bp
	mov	bp,sp
	mov	bx,[bp+4]	; get buffer pointer
	mov	cx,[bp+6]	; get buffer size
	mov	dx,[bp+8]	; get oldcrc
;
;---------------------------------------------
;Bsed on an 8080 routine for generating a
;CYCLIC-REDUNDANCY-CHECK by Fred Gutman.
;From 'EDN' magazine, June 5, 1979 issue, page 84.
;
bytlop:
	test	dh,80h		; Q-bit mask
	pushf	 	      	; save status
	add	dx,dx		; shl oldcrc
	add	dl,[bx]		; add byte from buffer
	inc	bx		; increment pointer to next byte
	popf			; retreive Q-bit status
	jz	qb2		; if Q-bit is zero
	xor	dx,0A097h	; gen. poly
qb2:
	loop	bytlop		; do till buffer m-t
	mov	ax,dx		; answer in accumulator for return
	pop	bp
	ret

	endps
        end
