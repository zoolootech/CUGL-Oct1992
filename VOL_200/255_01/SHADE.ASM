	  PAGE	 80,132
	  PAGE

DGROUP	  GROUP   GDATA

GDATA     SEGMENT PARA PUBLIC 'DATA'
	  Extrn   gd_sadr:word,gd_shad:word,gd_gseg:word
	  Extrn   gd_flag1:word
GDATA	  ENDS

_TEXT     SEGMENT BYTE PUBLIC 'CODE'
	  Public  _shade
	  Extrn   setgc:near
	  ASSUME  CS: _TEXT, DS: DGROUP, SS: DGROUP, ES: DGROUP

_shade	  Proc	 Near
	  PUSH	 BP
	  MOV	 BP,SP
	  PUSH	 DI
	  PUSH	 SI

	  OR	 gd_flag1,008h	       ; Turn shading mode active
	  MOV	 SI,[BP+4]
	  OR	 SI,SI
	  JNS	 cont
	  XOR	 AX,AX

	  POP	 SI
	  POP	 DI
	  MOV	 SP,BP
	  POP	 BP
	  RET
cont:
	  LEA	 DI,gd_shad
	  MOV	 CX,8
shade01:
	  PUSH	 CX
	  MOV	 CX,4
shade02:
	  LODSB 		       ; Load pixel color pair
	  MOV	 BL,AL
	  SHR	 BL,1
	  SHR	 BL,1
	  SHR	 BL,1
	  SHR	 BL,1
	  MOV	 BYTE PTR [DI],BL
	  ADD	 DI,8
	  AND	 AL,0Fh
	  MOV	 BYTE PTR [DI],AL
	  ADD	 DI,8
	  LOOP	 shade02

	  POP	 CX
	  SUB	 DI,63
	  LOOP	 shade01

	  MOV	 AX,0502h
	  CALL	 setgc
	  MOV	 AX,08FFh
	  CALL	 setgc

	  MOV	 ES,gd_gseg
	  MOV	 SI,[BP+4]	       ; Address of the new shade matrix
	  MOV	 DI,gd_sadr
	  MOV	 CX,32
	  MOV	 AL,080h
fbyte01:
	  OUT	 DX,AL
	  MOV	 AH,ES:[DI]
	  MOV	 AH,[SI]
	  SHR	 AH,1
	  SHR	 AH,1
	  SHR	 AH,1
	  SHR	 AH,1
	  MOV	 ES:[DI],AH
	  ROR	 AL,1
	  OUT	 DX,AL
	  MOV	 AH,ES:[DI]
	  MOV	 AH,[SI]
	  AND	 AH,0Fh
	  MOV	 ES:[DI],AH
	  ROR	 AL,1
	  ADC	 DI,0
	  INC	 SI
	  LOOP	 fbyte01

	  XOR	 AX,AX

	  POP	 SI
	  POP	 DI
	  MOV	 SP,BP
	  POP	 BP
	  RET
_shade	  Endp

_TEXT	  Ends
	  End
