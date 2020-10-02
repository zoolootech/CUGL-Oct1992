
		TITLE   LITEBAR
		PAGE    60,132

		;LITEBAR.ASM

    COMMENT     @

	This sub-routine inserts the attribute bytes in video ram that
	control colors in text modes. It first checks for Mono text mode,
	then whether a VGA or EGA is currently active. If neither is the
	case it defaults to CGA.  Works only in Text Modes.



	;extern void far pascal LITEBAR(
	    int atr, int row, int col, int page, int len, int dir
	    );

	@       Modified    **  10/29/89  **     Michael Kelly


	;Since we don't mess with a Data Segment this module can
	;be linked to any memory model except tiny.
	;I debugged this in Turbo Pascal 4.0, can be used there too.

	VMODE           EQU     BYTE PTR ES:[0049h]
	VCOLS           EQU     WORD PTR ES:[004Ah]
	VPAGE_SIZE      EQU     WORD PTR ES:[004Ch]
	VROWS           EQU     BYTE PTR ES:[0084h]
	INFO_BYTE       EQU     BYTE PTR ES:[0087h]

	BYTES_PER_ROW   EQU     WORD PTR [BP-2]     ;local variables
	IS_CGA_ADAPT    EQU     WORD PTR [BP-4]

	ATR_PARAM       EQU     [BP+16]    ;proc arguments
	ROW_PARAM       EQU     [BP+14]
	COL_PARAM       EQU     [BP+12]
	PAGE_PARAM      EQU     [BP+10]
	LEN_PARAM       EQU     [BP+8]
	DIR_PARAM       EQU     [BP+6]

	CSEG    SEGMENT BYTE    PUBLIC  'CODE'
		ASSUME  CS:CSEG
		PUBLIC  LITEBAR
LITEBAR         PROC    FAR
	START:  PUSH    BP
		MOV     BP,SP
		SUB     SP,4
		PUSH    DS
		PUSH    SI
		PUSH    ES
		PUSH    DI
		PUSHF
		CLD
		XOR     CX,CX               ;check the video hardware
		MOV     IS_CGA_ADAPT,0
		MOV     DX,0B000h           ;assume mono video segment
		XOR     DI,DI               ;and memory ofset of 0 for now.
		MOV     AX,40h
		MOV     ES,AX
		MOV     AL,VMODE            ;check video mode
		CMP     AL,7
		JE      LITE_02
		MOV     DX,0B800h
		CMP     AL,3
		JBE     LITE_01
		JMP     EXIT                    ;exit if not in text mode.

    LITE_01:
		MOV     AX,1A00h                ;Test for Vga Bios support.
		INT     10h
		CMP     AL,1Ah
		JNE     LITE_011
		CMP     BL,2
		JNE     LITE_02
		MOV     IS_CGA_ADAPT,1
		JMP     SHORT LITE_02
    LITE_011:
		MOV     AH,12h                      ;Ega Bios support?
		MOV     BL,10h
		INT     10h
		CMP     BL,10h
		JNE     LITE_012
		MOV     IS_CGA_ADAPT,1              ;Default to Cga.
		JMP     SHORT LITE_02
    LITE_012:
		MOV     AX,40h
		MOV     ES,AX
		TEST    INFO_BYTE,8
		JZ      LITE_02                 ;If bit 3 = 0, Ega is active
		MOV     IS_CGA_ADAPT,1          ;else using Cga.


    LITE_02:    MOV     AX,40h
		MOV     ES,AX
		MOV     AX,VCOLS
		SHL     AX,1
		MOV     BYTES_PER_ROW,AX

		MOV     BX,PAGE_PARAM       ;Calc offset of video page
		CMP     BX,0                ;using shifts and adds.
		JE      LITE_03
		MOV     AX,VPAGE_SIZE
		OR      BX,BX
		JZ      LITE_03
		CMP     BX,7
		JBE     LITE_021
		JMP     EXIT                ;Assume Max page # is 7
    LITE_021:   CMP     BX,1
		JA      LITE_022
		MOV     DI,AX
		JMP     SHORT LITE_03
    LITE_022:   CMP     BX,2
		JA      LITE_023
		SHL     AX,1
		MOV     DI,AX
		JMP     SHORT LITE_03
    LITE_023:   CMP     BX,3
		JA      LITE_024
		MOV     DI,AX
		SHL     AX,1
		ADD     DI,AX
		JMP     SHORT LITE_03
    LITE_024:   CMP     BX,4
		JA      LITE_025
		SHL     AX,1
		SHL     AX,1
		MOV     DI,AX
		JMP     SHORT LITE_03
    LITE_025:   CMP     BX,5
		JA      LITE_026
		MOV     DI,AX
		SHL     AX,1
		SHL     AX,1
		ADD     DI,AX
		JMP     SHORT LITE_03
    LITE_026:   CMP     BX,6
		JA      LITE_027
		SHL     AX,1
		MOV     DI,AX
		SHL     AX,1
		ADD     DI,AX
		JMP     SHORT LITE_03
    LITE_027:   PUSH    AX
		SHL     AX,1
		MOV     DI,AX
		SHL     AX,1
		ADD     DI,AX
		POP     AX
		ADD     DI,AX

    LITE_03:
		MOV     AX,ROW_PARAM        ;get video row param
		DEC     AX                  ;rows numberred from 1 so
		JZ      LITE_04             ;subtract one for mem offset.
		MOV     BX,BYTES_PER_ROW
		MUL     BL

    LITE_04:
		MOV     BX,COL_PARAM        ;do same with video column.
		XOR     BH,BH
		DEC     BX
		SHL     BX,1                ;mult by 2 for char/attr pair.


		ADD     AX,BX
		ADD     DI,AX
		MOV     ES,DX
		MOV     AX,ATR_PARAM        ;get color or attr.
		MOV     CX,LEN_PARAM        ;num of chars to color.
		OR      CX,CX
		JNZ     LITE_05
		JMP     EXIT

    LITE_05:
		CMP     IS_CGA_ADAPT,0  ;jump to hardware hassle if Cga
		JNE     LITE_09         ;otherwise just stuff video memory.

		OR      BYTE PTR DIR_PARAM,0
		JNZ     LITE_07

    LITE_06:    INC     DI                  ;stuff memory horizontally
		STOSB
		LOOP    LITE_06
		JMP     EXIT

				    ;or vertically depending on dir param.
    LITE_07:    INC     DI
    LITE_08:    MOV     BYTE PTR ES:[DI],AL
		ADD     DI,BYTES_PER_ROW
		LOOP    LITE_08
		JMP     EXIT

    LITE_09:    MOV     DX,03DAh             ;Crt status reg for sync timing
		OR      BYTE PTR DIR_PARAM,0    ;to avoid "snow" on screen.
		JNZ     LITE_13

    LITE_10:    INC     DI
		MOV     BX,AX
		CLI                 ;turn off interrupts and wait
    LITE_11:    IN      AL,DX       ;for horiz sync start to avoid snow.
		AND     AL,1
		JNZ     LITE_11

    LITE_12:    IN      AL,DX
		AND     AL,1        ;if bit 0 = 1 sync has begun
		JZ      LITE_12

		XCHG    AX,BX       ;so stuff one word in memory.
		STOSB
		STI
		LOOP    LITE_10     ;repeat until len param satisfied.
		JMP     EXIT

    LITE_13:    ADD     DI,2        ;same as above but in vertacal dir
    LITE_14:    DEC     DI
		DEC     DI
    LITE_15:    MOV     BX,AX
		CLI
    LITE_16:    IN      AL,DX
		AND     AL,1
		JNZ     LITE_16

    LITE_17:    IN      AL,DX
		AND     AL,1
		JZ      LITE_17
		XCHG    AX,BX
		STOSB
		STI
		ADD     DI,BYTES_PER_ROW
		LOOP    LITE_14


	EXIT:
		POPF              ;restore flags & regs
		POP     DI
		POP     ES
		POP     SI
		POP     DS
		MOV     SP,BP
		POP     BP
		RET     12              ;discard params & return
LITEBAR         ENDP

	CSEG    ENDS

		END     START




