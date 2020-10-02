;
;	SOUNDINT.ASM            (Lattice version)               06 Feb 86
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
;	Define IBM PC ports used to control speakers
;
KBCTL  	EQU 	61H		;Keyboard control port (controls speaker too)
SPKTMR 	EQU 	42H		;8253 Timer address
SPKCTL 	EQU 	43H		;8253 Timer address
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
        IF      LPROG
	EXTRN	sound_out:FAR          
	EXTRN	sound_done:FAR
        ENDIF
;
	PSEG                            ;Beginning of code segment
;
        IF      (LPROG EQ 0)
	EXTRN	sound_out:NEAR          
	EXTRN	sound_done:NEAR
        ENDIF
;
	PUBLIC	snd_irh
	PUBLIC	snd_giv
	PUBLIC	snd_siv
	PUBLIC	snd_brk
;
;
;
snd_irh	PROC    FAR
	STI                             ; Enable interrupts
	PUSH	DS                      ;\
	PUSH	ES                      ; \
	PUSH	SI                      ;  \
	PUSH	DI                      ;   \
	PUSH	BP                      ; Save all registers
	PUSH	AX                      ;   /
	PUSH	BX                      ;  /
	PUSH	CX                      ; /
	PUSH	DX                      ;/
	MOV	AX,SEG DGROUP           ;\
	MOV	ES,AX                   ; Load expected segment values
	MOV	DS,AX                   ;/
	CALL	sound_out               ; Call the real code
	POP	DX                      ;\
	POP	CX                      ; \
	POP	BX                      ;  \
	POP	AX                      ;   \
	POP	BP                      ; Restore all registers
	POP	DI                      ;   /
	POP	SI                      ;  /
	POP	ES                      ; /
	POP	DS                      ;/
        INT     61H                     ; Call previous INT vector
	IRET                            ; Return
snd_irh ENDP
;
;
	IF	LPROG
snd_giv	PROC    FAR
	ELSE
snd_giv	PROC    NEAR
	ENDIF
	PUSH	BP                      ;Save frame pointer
	MOV	BP,SP                   ;Make new frame pointer
	PUSH	ES                      ;Preserve ES, just in case
	XOR	AX,AX                   ;Zero ES register so it will
	MOV	ES,AX                   ;   point to low memory
	MOV	BX,[BP+ARGS]            ;Make BX into proper offset,
	SHL	BX,1                    ;   which is the vector number
	SHL	BX,1                    ;   times 4 (2 shifts)
	PUSHF                           ;Save INT flag
	CLI                             ;Disable interrupts
	MOV	AX,ES:[BX+2]            ;Get the high ...
        MOV     BX,ES:[BX]              ; ... and low words of vector
	POPF                            ;Restore INT flag
	POP	ES                      ;Preserve ES, just in case
	POP	BP                      ;Restore caller's frame
	RET
snd_giv ENDP
;
;
	IF	LPROG
snd_siv	PROC    FAR
	ELSE
snd_siv	PROC    NEAR
	ENDIF
	PUSH	BP                      ;Save frame pointer
	MOV	BP,SP                   ;Make new frame pointer
	PUSH	ES                      ;Preserve ES, just in case
	XOR	AX,AX                   ;Zero ES register so it will
	MOV	ES,AX                   ;   point to low memory
	MOV	BX,[BP+ARGS]            ;Make BX into proper offset,
	SHL	BX,1                    ;   which is the vector number
	SHL	BX,1                    ;   times 4 (2 shifts)
	PUSHF                           ;Save INT flag
	CLI                             ;Disable interrupts
	MOV	AX,[BP+ARGS+2]          ;Get low word of vector 
	MOV	ES:[BX],AX              ;   and install it.
	MOV	AX,[BP+ARGS+4]          ;Get high word of vector
	MOV	ES:[BX+2],AX            ;   and install it.
	POPF                            ;Restore INT flag
	POP	ES                      ;Preserve ES, just in case
	POP	BP                      ;Restore caller's frame
	RET
snd_siv ENDP
;
;
snd_brk PROC    FAR
	STI
	PUSH	DS                      ;\
	PUSH	ES                      ; \
	PUSH	SI                      ;  \
	PUSH	DI                      ;   \
	PUSH	BP                      ; Save all registers
	PUSH	AX                      ;   /
	PUSH	BX                      ;  /
	PUSH	CX                      ; /
	PUSH	DX                      ;/
	MOV	AX,SEG DGROUP           ;\
	MOV	ES,AX                   ; Load expected segment values
	MOV	DS,AX                   ;/
	CALL	sound_done              ; Tidy up loose ends
        CLC                             ; Clear Carry (see above desc.)
	INT     23H                     ; Call the real handler
	POP	DX                      ;\
	POP	CX                      ; \
	POP	BX                      ;  \
	POP	AX                      ;   \
	POP	BP                      ; Restore all registers
	POP	DI                      ;   /
	POP	SI                      ;  /
	POP	ES                      ; /
	POP	DS                      ;/
        JC      abort                   ; Abort if handler set carry flag
;
cont:   IRET                            ; Continue, use IRET
;
abort:  POPF                            ; Get flags off stack
        STC                             ; Pass "abort" message back to DOS
        RET                             ; Long return to DOS
snd_brk ENDP
;
;
	ENDPS	
	END





