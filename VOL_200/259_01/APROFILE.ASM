		 page	 ,132
		 name	 APROFILE

;   APROFILE.ASM - Low level application profile generator.
;
;      V01.00  010788  Bob Withers   Program originally complete
;

MSDOS		 equ	 21h
TIMER_SET_CNT	 equ	 36h

TIMER_INT_CNT	 equ	 32
TIMER_CNT_WORD	 equ	 0800h
;			 TIMER_INT_CNT	 TIMER_CNT_WORD   Int per Sec
;			       1	     0000h	      18.2
;			       2	     8000h	      36.4
;			       4	     4000h	      72.8
;			       8	     2000h	     145.6
;			      16	     1000h	     291.2
;			      32	     0800h	     582.4
;			      64	     0400h	    1164.8

TIMER_CTL_PORT	 equ	 43h
TIMER_DATA_PORT  equ	 40h
I8259_EOI_PORT	 equ	 20h
EOI_CODE	 equ	 20h

		 if1
		 ifdef	 LMODEL
		 %out	 Assembling for LARGE model
		 else
		 ifdef	 MMODEL
		 %out	 Assembling for MEDIUM model
		 else
		 ifdef	 CMODEL
		 %out	 Assembling for COMPACT model
		 else
		 %out	 Assembling for SMALL model
		 endif
		 endif
		 endif
		 endif

BegCodeSeg	 macro	 name
		 ifdef	 LMODEL
name&_TEXT	 segment byte public 'CODE'
		 assume  cs:&name&_TEXT
		 else
		 ifdef	 MMODEL
name&_TEXT	  segment byte public 'CODE'
		 assume  cs:&name&_TEXT
		 else
		 ifdef	 CMODEL
_TEXT		 segment byte public 'CODE'
		 assume  cs:_TEXT
		 else
_TEXT		 segment byte public 'CODE'
		 assume  cs:_TEXT
		 endif
		 endif
		 endif
		 endm

EndCodeSeg	 macro	 name
		 ifdef	 LMODEL
name&_TEXT	 ends
		 else
		 ifdef	 MMODEL
name&_TEXT	 ends
		 else
		 ifdef	 CMODEL
_TEXT		 ends
		 else
_TEXT		 ends
		 endif
		 endif
		 endif
		 endm

Entry		 macro	 name
		 public  _&name
		 ifdef	 LMODEL
_&name		 proc	 far
		 else
		 ifdef	 MMODEL
_&name		 proc	 far
		 else
		 ifdef	 CMODEL
_&name		 proc	 near
		 else
_&name		 proc	 near
		 endif
		 endif
		 endif
		 push	 bp
		 mov	 bp,sp
		 push	 si
		 push	 di
		 push	 es
		 endm

Exit		 macro	 name
		 pop	 es
		 pop	 di
		 pop	 si
		 pop	 bp
		 ret
_&name		 endp
		 endm

		 ifdef	 LMODEL
Parm1		 struc
Save_BP1	 dw	 ?
Ret_Addr1	 dd	 ?
P_TotSegs	 dw	 ?
P_SegTab	 dd	 ?
Parm1		 ends
		 else
		 ifdef	 MMODEL
Parm1		 struc
Save_BP1	 dw	 ?
Ret_Addr1	 dd	 ?
P_TotSegs	 dw	 ?
P_SegTab	 dd	 ?
Parm1		 ends
		 else
		 ifdef	 CMODEL
Parm1		 struc
Save_BP1	 dw	 ?
Ret_Addr1	 dw	 ?
P_TotSegs	 dw	 ?
P_SegTab	 dd	 ?
Parm1		 ends
		 else
Parm1		 struc
Save_BP1	 dw	 ?
Ret_Addr1	 dw	 ?
P_TotSegs	 dw	 ?
P_SegTab	 dd	 ?
Parm1		 ends
		 endif
		 endif
		 endif

Parm2		 struc
Save_BP2	 dw	 ?
Int_Addr	 dd	 ?
Flags		 dw	 ?
Parm2		 ends

Seg1		 struc
SegAddr 	 dw	 ?
LinCnt		 dw	 ?
PubCnt		 dw	 ?
LinPtr		 dd	 ?
PubPtr		 dd	 ?
Seg1		 ends

Ofs1		 struc
OfsAddr 	 dw	 ?
OfsLoCnt	 dw	 ?
OfsHiCnt	 dw	 ?
Ofs1		 ends


		 BegCodeSeg PRFA

IntCnt		 dw	 TIMER_INT_CNT
TotSegs 	 dw	 0
SegTab		 dd	 0
OldInt08	 dd	 0
OtherCntLo	 dw	 0
OtherCntHi	 dw	 0
BiosCntLo	 dw	 0
BiosCntHi	 dw	 0
DosCntLo	 dw	 0
DosCntHi	 dw	 0
HitCntLo	 dw	 0
HitCntHi	 dw	 0

;
;     void cdecl PrfInit(TotSegs, SegTab)
;     unsigned	  TotSegs;
;     char	 *SegTab;
;
		 Entry	 PrfInit
		 mov	 ax,[bp].P_TotSegs	 ;Get my parameters
		 mov	 TotSegs,ax		 ;	    .
		 les	 ax,[bp].P_SegTab	 ;	    .
		 mov	 word ptr SegTab,ax	 ;	    .
		 mov	 word ptr SegTab+2,es	 ;	    .
		 mov	 ax,3508h		 ;Get current timer vector
		 int	 MSDOS			 ;	    .
		 mov	 word ptr OldInt08,bx	 ;	    .
		 mov	 word ptr OldInt08+2,es  ;	    .
		 mov	 ax,2508h		 ;Setup my timer vector addr
		 push	 ds			 ;	    .
		 push	 cs			 ;	    .
		 pop	 ds			 ;	    .
		 mov	 dx,offset Int08	 ;	    .
		 int	 MSDOS			 ;	    .
		 pop	 ds			 ;	    .
		 mov	 al,TIMER_SET_CNT	 ;Set timer 0 interrupt rate
		 out	 TIMER_CTL_PORT,al	 ;    to higher speed
		 mov	 ax,TIMER_CNT_WORD	 ;	    .
		 out	 TIMER_DATA_PORT,al	 ;	    .
		 mov	 al,ah			 ;	    .
		 out	 TIMER_DATA_PORT,al	 ;	    .
		 Exit	 PrfInit


;
;     unsigned long cdecl PrfTerm()
;
		 Entry	 PrfTerm
		 mov	 al,36h 		 ;Reset timer 0 interrupt
		 out	 TIMER_CTL_PORT,al	 ;    rate to normal
		 xor	 al,al			 ;	    .
		 out	 TIMER_DATA_PORT,al	 ;	    .
		 nop				 ;	    .
		 out	 TIMER_DATA_PORT,al	 ;	    .
		 mov	 ax,2508h		 ;Restore old timer vector
		 push	 ds			 ;	    .
		 lds	 dx,OldInt08		 ;	    .
		 int	 MSDOS			 ;	    .
		 pop	 ds			 ;	    .
		 mov	 ax,OtherCntLo		 ;Return 'other' hit count
		 mov	 dx,OtherCntHi		 ;	    .
		 Exit	 PrfTerm


;
;     unsigned long cdecl PrfDosCnt()
;
		 Entry	 PrfDosCnt
		 mov	 ax,DosCntLo		 ;Return 'DOS' hit count
		 mov	 dx,DosCntHi		 ;	    .
		 Exit	 PrfDosCnt


;
;     unsigned long cdecl PrfBiosCnt()
;
		 Entry	 PrfBiosCnt
		 mov	 ax,BiosCntLo		 ;Return 'BIOS' hit count
		 mov	 dx,BiosCntHi		 ;	    .
		 Exit	 PrfBiosCnt


;
;     unsigned long cdecl PrfHitCnt()
;
		 Entry	 PrfHitCnt
		 mov	 ax,HitCntLo		 ;Return 'appl' hit count
		 mov	 dx,HitCntHi		 ;	    .
		 Exit	 PrfHitCnt


Int08		 proc	 far
		 push	 bp			 ;Set up stack frame
		 mov	 bp,sp			 ;	    .
		 push	 ax			 ;Save all registers
		 push	 bx			 ;	    .
		 push	 cx			 ;	    .
		 push	 dx			 ;	    .
		 push	 si			 ;	    .
		 push	 di			 ;	    .
		 push	 es			 ;	    .
		 push	 ds			 ;	    .
		 lds	 si,[bp].Int_Addr	 ;Addr of interrupt
		 les	 di,SegTab		 ;Addr of segment table
		 mov	 cx,TotSegs		 ;Total # segments
		 mov	 ax,ds			 ;Seg Addr of interrupt in AX
		 cmp	 ax,1500h		 ;Is exection in DOS
		 ja	 Int08_1		 ;  No	: continue
		 inc	 DosCntLo		 ;Bump the DOS hit count
		 jnz	 Int08_0		 ;	    .
		 inc	 DosCntHi		 ;	    .
Int08_0:	 jmp	 Int08_H		 ;	    .
Int08_1:
		 cmp	 ax,0f000h		 ;Is execution in ROM BIOS?
		 jb	 Int08_A		 ;  No	: continue
		 inc	 BiosCntLo		 ;Bump the BIOS hit count
		 jnz	 Int08_2		 ;	    .
		 inc	 BiosCntHi		 ;	    .
Int08_2:	 jmp	 Int08_H		 ;	    .
Int08_A:
		 cmp	 ax,es:[di].SegAddr	 ;Look for seg in the seg tab
		 je	 Int08_B		 ;Jump if found
		 add	 di,type Seg1		 ;Increment to next seg entry
		 loop	 Int08_A		 ;Loop thru entire table
		 inc	 OtherCntLo		 ;Seg not found, inc other cnt
		 jnz	 Int08_H		 ;	    .
		 inc	 OtherCntHi		 ;	    .
		 jmp	 short Int08_H		 ;	    .
Int08_B:
		 inc	 HitCntLo		 ;Seg found in table, hit cnt
		 jnz	 Int08_B1		 ;	    .
		 inc	 HitCntHi		 ;	    .
Int08_B1:
		 mov	 cx,es:[di].LinCnt	 ;Cnt of Line # entries in seg
		 or	 cx,cx			 ;Is it zero?
		 jz	 Int08_E		 ;  Yes : skip line # check
		 lds	 bx,es:[di].LinPtr	 ;Get pointer to line # tab
		 cmp	 si,[bx].OfsAddr	 ;Int ofs > highest line #
		 ja	 Int08_E		 ;  Yes : skip line # check
Int08_C:
		 cmp	 si,[bx].OfsAddr	 ;Locate line # entry in table
		 jae	 Int08_D		 ;Jump when found
		 add	 bx,type Ofs1		 ;Increment to next line entry
		 loop	 Int08_C		 ;Loop thru entire table
		 jmp	 short Int08_E		 ;Skip line # entry at end
Int08_D:
		 inc	 [bx].OfsLoCnt		 ;Inc cnt for line # entry
		 jnz	 Int08_E		 ;	    .
		 inc	 [bx].OfsHiCnt		 ;	    .
Int08_E:
		 mov	 cx,es:[di].PubCnt	 ;Cnt of Public entries in seg
		 or	 cx,cx			 ;Is it zero?
		 jz	 Int08_H		 ;  Yes : skip public check
		 lds	 bx,es:[di].PubPtr	 ;Get pointer to public table
Int08_F:
		 cmp	 si,[bx].OfsAddr	 ;Locate public entry in table
		 jae	 Int08_G		 ;Jump when found
		 add	 bx,type Ofs1		 ;Inc to next public entry
		 loop	 Int08_F		 ;Loop thru entire table
		 jmp	 short Int08_H		 ;Skip public entry at end
Int08_G:
		 inc	 [bx].OfsLoCnt		 ;Inc cnt for public entry
		 jnz	 Int08_H		 ;	    .
		 inc	 [bx].OfsHiCnt		 ;	    .
Int08_H:
		 pop	 ds			 ;Restore all registers
		 pop	 es			 ;	    .
		 pop	 di			 ;	    .
		 pop	 si			 ;	    .
		 pop	 dx			 ;	    .
		 pop	 cx			 ;	    .
		 pop	 bx			 ;	    .
		 dec	 IntCnt 		 ;Decrement interrupt count
		 jnz	 Int08_I		 ;Not time to call ROM timer
		 mov	 IntCnt,TIMER_INT_CNT	 ;Reset interrupt counter
		 pop	 ax			 ;Restore final registers
		 pop	 bp			 ;	    .
		 jmp	 OldInt08		 ;Let old vector EOI & IRET
Int08_I:
		 mov	 al,EOI_CODE		 ;Send the EOI to the 8259
		 out	 I8259_EOI_PORT,al	 ;	    .
		 pop	 ax			 ;Restore final regiaters
		 pop	 bp			 ;	    .
		 iret				 ;Return from interrupt
Int08		 endp

		 EndCodeSeg PRFA
		 end

