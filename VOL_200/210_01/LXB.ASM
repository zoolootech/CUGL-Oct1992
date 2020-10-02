	TITLE 'LXB--BIOS extension loader'
	PAGE 62

;9/20/86--N.T.Carnevale
;After C.Sondgeroth's XLOAD--see Micro/Systems Journal vol.1 #2 p.66 et seq.
;See also my article in the July/August 1986 issue of M/S J pp.72-85.
;Modified 7/12/85 by NTC for new xbios called nuxb


TRUE	EQU	0FFH
FALSE	EQU	NOT TRUE

INTRUP	EQU	FALSE	;TRUE if your hardware uses interrupts routinely


HDRLNTH EQU	20H	;# of bytes in the xbios's constant-length header


BOOT	EQU	0	;warm boot entry
BDOS	EQU	5
FCB	EQU	5CH	;default fcb

CONOUT	EQU	2	;console output func
OPEN	EQU	15	;bdos open file
READ	EQU	20	;     read sequential
SETDMA	EQU	26	;     set dma addr

CR	EQU	0DH
LF	EQU	0AH


	ORG 100H

XLOAD	EQU $
	LXI H,0
	DAD SP
	SHLD STACK	;saves stack pointer
	LXI SP,STACK	;& creates local stack
	LDA FCB+1
	CPI ' ' 	;check for name of bios extension (xbios) file
	JNZ XL20
	CALL MSG
	DB 'USAGE:  lxb filnam',CR,LF
	DB '  where filnam is a bios extension file',CR,LF
	DB '  (.SPR extension assumed)'
	DB 0
CPM$EXIT:
	LHLD STACK
	SPHL		;restore stack pointer
	RET		;& bail out


XL20:	LXI H,FCB+9	;here starts file type
	MVI M,'S'	;force .SPR extension
	INX H
	MVI M,'P'
	INX H
	MVI M,'R'
	INX H
	MVI C,24	;# of bytes remaining in fcb
SLCLR:	MVI M,0 	;clear the rest of the fcb to nulls
	INX H
	DCR C
	JNZ SLCLR
	LXI D,FCB
	MVI C,OPEN
	CALL BDOS	;open the xbios file
	ANA A
	JP SL50 	;if successful
	CALL MSG	;else say failed
	DB 'unable to find the bios extension file'
	DB 0
	JMP CPM$EXIT


;Read the bios extension file into low memory
SL50:	LXI H,BUF	;hl = dma addr
SL60:	PUSH H
	XCHG		;de = dma addr
	MVI C,SETDMA
	CALL BDOS	;tell cpm where to put next sector
	LXI D,FCB
	MVI C,READ	;read next sector
	CALL BDOS
	POP H
	LXI D,128
	DAD D		;hl = next dma addr
	ANA A
	JZ SL60 	;not finished


;Relocate the .SPR file's contents to high memory (just below
;ccp, or below the last xbios module)
IF	INTRUP
	DI
ENDIF
	LDA BOOT+2	;calc page of ccp
	SUI 16H
	MOV H,A 	;hl = ccp base page addr
	LDA BDOS+2	;current bdos page addr
	CMP H
	JNC REL1	;if no prior xbios
	LHLD BDOS+1	;else get current top of tpa
REL1:	MVI L,0 	;start on a page boundary
	XCHG		;de = 1+ top of tpa
	LXI H,BUF+1	;hl points to length of spr code
	MOV C,M
	INX H
	MOV B,M 	;bc = length of spr code
	PUSH B
	INR C		;round bc up to an integral # of pages
	DCR C		;if c = 0, code ends on a page boundary
	JZ REL2
	INR B
REL2:	MOV A,D 	;calc where to move the code, put result in de
	SUB B		;a = bios page
	STA XBPAGE	;for future use
	MOV D,A
	MVI E,0 	;de = 1 + top of current tpa - code length
	LXI H,BUF+256	;start of relocatable code
	POP B
	PUSH B		;bc = actual length of spr code
	PUSH D		;de = where to move it to (start of xbios)
	CALL MOVEM	;first move it up without relocation

	POP D		;de = start of xbios
	LXI H,BUF+256	;hl = start of relocatable code
	POP B		;bc = length "
	DAD B		;hl = start of bit map

	PUSH D		;save page offset
	XCHG		;hl = addr of code, de = addr of bit map
	PUSH B		;save byte count


;Get the next bit map byte and use it to direct relocation of 8 bytes
;Stop when end of relocatable code is found
RELNMAP:
	MVI C,8
	LDAX D
	MOV B,A 	;b = map byte
	INX D		;point to next map byte
;Shift & test relocation bits.
;If bit is 0, don't alter code, else add offset for new base addr
RELNBYT:
	DCR C		;count bits in the byte
	JM RELNMAP	;get next byte
	MOV A,B
	RAL		;get next bit
	MOV B,A
	JNC NOREL
	LDA XBPAGE	;offset
	ADD M		;add offset to code
	MOV M,A
NOREL:	INX H		;next location to consider
	XTHL		;hl = byte count
	DCX H
	MOV A,H
	ORA L		;done?
	XTHL		;hl = code pointer again
	JNZ RELNBYT	;no, keep working


;After done with relocation:
;1.  fetch the original bios jump table and save a copy of it 
;	in the new xbios module
;2.  modify the original bios jump table so it points to the 
;	corresponding locations in the xbios's jump table
	POP B		;remove counter from the stack
	POP H		;hl = start of new bios module
	PUSH H
;Next section is for new xbios--NTC
;Copy original bios jmp table into xbios at OLDTBL
	INX H
	INX H
	INX H	
	;m = length of strings between end of header and start of jmp table
	PUSH H		;save place in the xbios header
	MOV A,M
	ADI HDRLNTH	;a = distance from start of xbios to start of jmp table
	MOV L,A 	;hl points to start of strings
	XCHG		;de is addr of xbios's copy of orig jmp table (dest)
	POP H		;return to xbios preface for more
	INX H
	MOV A,M 	;# of jmp instr in the table (omits jmp cold)
	STA JMPNUM	;for future use
	ADD M
	ADD M		;a = length of jmp table in bytes (omitting jmp cold)
	MOV C,A
	MVI B,0 	;bc = # of bytes to copy
	LHLD BOOT+1	;hl = addr of 1st jmp to copy from bios table
	CALL MOVEM	;copy orig bios jmp table into oldtbl in xbios
	LHLD BOOT+1	;hl = addr of jmp wboot in orig bios table
	LDA JMPNUM
	MOV C,A 	;# of jmp instr to patch
;Now ready to patch the original jmp table to point to xbios's own table
;
NEWJMP: INX H		;patch old jmp table to point to 
	MOV M,E 	;corresponding entries in xbios
	INX H
	MOV M,D
	INX H
	INX D
	INX D
	INX D		;point to next entry addr in xbios jmp table
	DCR C
	JNZ NEWJMP	;to finish fixing the old jmp table
	POP D
	PUSH D		;de = addr of xbios
	LHLD BDOS+1	;hl = destination of old bdos jmp (bdos entry)
	PUSH H		;saved on stack
	LXI H,BDOS+1	;hl = where to patch the jmp to the xbios
	MOV M,E
	INX H
	MOV M,D 	;now location 5 contains "jmp xbios"
	POP B		;bc = old bdos entry
	POP H		;hl = start of xbios
	INX H
	MOV M,C
	INX H
	MOV M,B 	;now first jmp in xbios is "jmp bdos"
IF	INTRUP
	EI
ENDIF
	RST 0		;cp/m warm boot


;Send inline message, terminated by null, to console
;hl must point to the message
MSG	EQU $
	POP H
	MOV A,M 	;a = next char
	INX H
	PUSH H
	ANA A
	RZ
	MOV E,A
	MVI C,CONOUT
	CALL BDOS
	JMP MSG


MOVEM:	MOV A,M 	;move bc bytes from hl to de
	STAX D
	INX H
	INX D
	DCX B
	MOV A,B
	ORA C
	JNZ MOVEM
	RET


JMPNUM: DS 1		;will hold # of jmps in the bios jmp table
XBPAGE: DS 1		;will hold page addr of xbios

	DS 64
STACK:	DS 2

BUF	EQU $		;spr file goes here first

	END
