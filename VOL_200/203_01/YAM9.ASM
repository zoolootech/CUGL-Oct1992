TITLE YAM9: CRC SUBS
PAGE 50,132
;************************************************************************
;* CRCSUBS (Cyclic Redundancy Code Subroutines) version 1.20
;* 8086 Mnemonics
;*
;*     	 This subroutine will compute and check a true 16-bit
;*	      Cyclic Redundancy Code for a message of arbitrary length.
;*
;*	      The  use  of this scheme will guarantee detection of all
;*	      single and double bit errors, all  errors  with  an  odd
;*	      number  of  error bits, all burst errors of length 16 or
;*	      less, 99.9969% of all 17-bit error bursts, and  99.9984%
;*	      of  all  possible  longer  error bursts.  (Ref: Computer
;*	      Networks, Andrew S.  Tanenbaum, Prentiss-Hall, 1981)
;*
;*
;************************************************************************
;*
;*	      From: CRCSUB12.ASM
;*	      Designed & coded by Paul Hansknecht, June 13, 1981
;*
;*	      Copyright (c) 1981, Carpenter Associates
;*	      	        	            Box 451
;*	      	        	            Bloomfield Hills, MI 48013
;*	      	        	            313/855-3074
;*
;*	      This program may be freely reproduced for non-profit use.
;*
;************************************************************************
;
;	       unsigned updcrc(char, oldcrc)
;
;	       At start of packet, oldcrc is set to initial value
;	       	        oldcrc=0;
;
;	       crc is accumulated by:
;	       	        oldcrc=updcrc(char, oldcrc);
;
;	       at end of packet,
;	       	        oldcrc=updcrc(0,updcrc(0,oldcrc));
;	       	        send(oldcrc>>8); send(oldcrc);
;
;	       on receive, the return value of updcrc is checked after the
;	       last call (with the second CRC byte); 0 indicates no error detected
;

; SMALL MODEL
INCLUDE \LC\S\DOS.MAC
		pseg
public updcrc

updcrc:
	push	bp		; save frame pointer
	mov	bp,sp		; stack pointer to frame pointer
	mov	ax,[bp+4]	; get char
	mov	bx,[bp+6]	; get oldcrc
	mov	cx,8		; The generator is X^16 + X^12 + X^5 + 1       
updloop:			; as recommended by CCITT.		     
	rol	al,1		; An alternate generator which is often	
	rcl	bx,1		; used in synchronous transmission protocols   
	jnb	skipit		; is X^16 + X^15 + X^2 + 1. This may be	
								; used by substituting XOR 8005H for
	xor	bx,1021H	; XOR 1021H in the adjacent code.
skipit:
	loop	updloop	 		
	mov	ax,bx		; mov to accumulator for return		
	pop	bp		
	ret			; return with latest crc in ax

	endps
	end
