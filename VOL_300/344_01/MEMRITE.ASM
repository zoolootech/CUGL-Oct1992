
		TITLE   MEMRITE
		PAGE    60,132

		;MEMRITE.ASM

    COMMENT     @

	This sub-routine writes directly to video ram on the specified
	video page. It first checks for mono text mode, then whether
	a VGA or EGA is currently active. If neither is the case
	it defaults to CGA.

 extern void far pascal memrite(
     int atr,int row,int col,int page,int dir,char far *str
     );

	Can easily be modified for Turbo Pascal 4.0+ by accomodating
	Turbo Pascal string format.  In fact I debugged this in TP 4.0
	and changed the string handling code for C, the " pascal "
	function modifier saves me from moving stuff around on the stack
	and messing up working code.

	Will link with any memory model except tiny

	@       Modified    **  10/29/89  **     Michael Kelly


	MAXSTR          EQU     8192

	VMODE           EQU     BYTE PTR ES:[0049h]
	VCOLS           EQU     WORD PTR ES:[004Ah]
	VPAGE_SIZE      EQU     WORD PTR ES:[004Ch]
	VROWS           EQU     BYTE PTR ES:[0084h]
	INFO_BYTE       EQU     BYTE PTR ES:[0087h]

	BYTES_PER_ROW   EQU     WORD PTR [BP-2]     ;local variables
	IS_CGA_ADAPT    EQU     WORD PTR [BP-4]

	ATR_PARAM       EQU     [BP+18]    ;proc arguments
	ROW_PARAM       EQU     [BP+16]
	COL_PARAM       EQU     [BP+14]
	PAGE_PARAM      EQU     [BP+12]
	DIR_PARAM       EQU     [BP+10]
	STR_PARAM       EQU     [BP+6]

	CSEG    SEGMENT BYTE    PUBLIC  'CODE'
		ASSUME  CS:CSEG
		PUBLIC  MEMRITE
    MEMRITE     PROC    FAR
	START:  PUSH    BP
		MOV     BP,SP
		SUB     SP,4
		PUSH    DS
		PUSH    SI
		PUSH    ES
		PUSH    DI
		PUSHF
		CLD
		XOR     CX,CX
		MOV     IS_CGA_ADAPT,0        ;check the video hardware
		MOV     DX,0B000h
		XOR     DI,DI                 ;assume mono video segment
		MOV     AX,40h                ;and memory ofset of 0 for now.
		MOV     ES,AX
		MOV     AL,VMODE
		CMP     AL,7                  ;check video mode
		JE      MRITE_02
		MOV     DX,0B800h
		CMP     AL,3
		JBE     MRITE_01
		JMP     EXIT

    MRITE_01:
		MOV     AX,1A00h
		INT     10h
		CMP     AL,1Ah
		JNE     MRITE_011
		CMP     BL,2
		JNE     MRITE_02
		MOV     IS_CGA_ADAPT,1
		JMP     SHORT MRITE_02
    MRITE_011:
		MOV     AH,12h
		MOV     BL,10h
		INT     10h
		CMP     BL,10h
		JNE     MRITE_012
		MOV     IS_CGA_ADAPT,1
		JMP     SHORT MRITE_02
    MRITE_012:
		MOV     AX,40h
		MOV     ES,AX
		TEST    INFO_BYTE,8
		JZ      MRITE_02
		MOV     IS_CGA_ADAPT,1
    MRITE_02:   MOV     AX,40h
		MOV     ES,AX
		MOV     AX,VCOLS
		SHL     AX,1
		MOV     BYTES_PER_ROW,AX

		MOV     BX,PAGE_PARAM       ;Calc offset of video page
		CMP     BX,0                ;using shifts and adds.
		JE      MRITE_03
		MOV     AX,VPAGE_SIZE
		OR      BX,BX
		JZ      MRITE_03
		CMP     BX,7
		JBE     MRITE_021
		JMP     EXIT                ;Assume Max page # is 7.
    MRITE_021:  CMP     BX,1
		JA      MRITE_022
		MOV     DI,AX
		JMP     SHORT MRITE_03
    MRITE_022:  CMP     BX,2
		JA      MRITE_023
		SHL     AX,1
		MOV     DI,AX
		JMP     SHORT MRITE_03
    MRITE_023:  CMP     BX,3
		JA      MRITE_024
		MOV     DI,AX
		SHL     AX,1
		ADD     DI,AX
		JMP     SHORT MRITE_03
    MRITE_024:  CMP     BX,4
		JA      MRITE_025
		SHL     AX,1
		SHL     AX,1
		MOV     DI,AX
		JMP     SHORT MRITE_03
    MRITE_025:  CMP     BX,5
		JA      MRITE_026
		MOV     DI,AX
		SHL     AX,1
		SHL     AX,1
		ADD     DI,AX
		JMP     SHORT MRITE_03
    MRITE_026:  CMP     BX,6
		JA      MRITE_027
		SHL     AX,1
		MOV     DI,AX
		SHL     AX,1
		ADD     DI,AX
		JMP     SHORT MRITE_03
    MRITE_027:  PUSH    AX
		SHL     AX,1
		MOV     DI,AX
		SHL     AX,1
		ADD     DI,AX
		POP     AX
		ADD     DI,AX

    MRITE_03:
		MOV     AX,ROW_PARAM    ;get screen row and sub 1
		DEC     AX
		JZ      MRITE_04
		MOV     BX,BYTES_PER_ROW
		MUL     BL                  ;then mul to get memory offset.

    MRITE_04:
		MOV     BX,COL_PARAM        ;throw in video column calc.
		XOR     BH,BH
		DEC     BX
		SHL     BX,1


		ADD     AX,BX
		ADD     DI,AX           ;sum page row and col to get
		MOV     ES,DX           ;right place in screen memory.
		MOV     AX,ATR_PARAM
		MOV     AH,AL
		LDS     SI,STR_PARAM
		MOV     CX,MAXSTR

    MRITE_05:
		CMP     IS_CGA_ADAPT,0  ;now we jump to routines depending
		JNE     MRITE_09        ;on screen writing direction
					;and wether we must hassle with a
					;Cga video adapter to avoid the
					;abominable snow man.

		OR      BYTE PTR DIR_PARAM,0
		JNZ     MRITE_07

    MRITE_06:   LODSB               ;horizontal write until
		OR      AL,AL       ;'\0' char found.
		JZ      EXIT
		STOSW
		LOOP    MRITE_06
		JMP     EXIT


    MRITE_07:   LODSB               ;vertical write until
		OR      AL,AL       ;'\0' char found.
		JZ      EXIT
		MOV     WORD PTR ES:[DI],AX
		ADD     DI,BYTES_PER_ROW
		LOOP    MRITE_07
		JMP     EXIT

						;same thing but with
						;Cga sync hassle.

    MRITE_09:   MOV     DX,03DAh                ;Crt status reg
		OR      BYTE PTR DIR_PARAM,0    ;to detect retrace sync.
		JNZ     MRITE_13

    MRITE_10:   LODSB
		OR      AL,AL
		JZ      EXIT        ;done if we've found '\0' char.
		MOV     BX,AX
		CLI                 ;turn off interrupts and use two
    MRITE_11:   IN      AL,DX       ;loop to snag start of horiz video sync.
		AND     AL,1
		JNZ     MRITE_11

    MRITE_12:   IN      AL,DX
		AND     AL,1        ;if bit 0 = 1, we're in horiz sync.
		JZ      MRITE_12

		XCHG    AX,BX       ;get char/attr pair and stuff in memory
		STOSW               ;during horiz sync.
		STI
		LOOP    MRITE_10    ;repeat until '\0' char found.
		JMP     EXIT

    MRITE_13:   ADD     DI,2        ;same as above but writes vertcally
    MRITE_14:   DEC     DI          ;down the screen.
		DEC     DI
		LODSB
		OR      AL,AL
		JZ      EXIT
    MRITE_15:   MOV     BX,AX
		CLI
    MRITE_16:   IN      AL,DX
		AND     AL,1
		JNZ     MRITE_16

    MRITE_17:   IN      AL,DX
		AND     AL,1
		JZ      MRITE_17
		XCHG    AX,BX
		STOSW
		STI
		ADD     DI,BYTES_PER_ROW
		LOOP    MRITE_14


	EXIT:
		POPF              ;restore flags & regs
		POP     DI
		POP     ES
		POP     SI
		POP     DS
		MOV     SP,BP
		POP     BP
		RET     14              ;discard params & return
    MEMRITE     ENDP

	CSEG    ENDS

		END     START




