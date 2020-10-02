;
;	DELAY.ASM               (Lattice version)               20 Feb 86
;
;   /* ------------------------------------------------------------ */
;   /*      This is a portion of the SOUND EFFECTS LIBRARY.         */
;   /*                                                              */
;   /*      Copyright (C) 1986 by Paul Canniff.                     */
;   /*      All rights reserved.                                    */
;   /*                                                              */
;   /*      This library has been placed into the public domain     */
;   /*      by the author.  Use is granted for non-commercial       */
;   /*      pusposes, or as an IMBEDDED PORTION of a commercial     */
;   /*      product.                                                */
;   /*                                                              */
;   /*      Paul Canniff                                            */
;   /*      PO Box 1056                                             */
;   /*      Marlton, NJ 08053                                       */
;   /*                                                              */
;   /*      CompuServe ID: 73047,3715                               */
;   /*                                                              */
;   /* ------------------------------------------------------------ */
;
; -------------------------------------------------------------------------
;	Set up macros for segment values, etc.
;
	INCLUDE DOS.MAC			;Lattice gets from file.
;
	DSEG                            ;Dummy DATA Segment
	ENDDS
;
	IF	LPROG
ARGS	EQU	6                       ;Bytes on stack before args 
	ELSE
ARGS	EQU	4                       ;Bytes on stack before args
	ENDIF
;
; -------------------------------------------------------------------------
;
	PSEG                            ;Beginning of code segment
;
	PUBLIC	delay
;
        IF      LPROG
delay	PROC    FAR
        ELSE
delay   PROC    NEAR
        ENDIF
        PUSH    BP
        MOV     BP,SP
        MOV     CX,[BP+ARGS]
OUTLP:  PUSH    CX
        MOV     CX,279
INLP:   LOOP    INLP
        POP     CX
        LOOP    OUTLP
        POP     BP
	RET                       
delay   ENDP
;
;
	ENDPS	
	END






