; 	SEARCH.ASM
;
;	Version 1.2	30-June-80
;
;  	this program searches RAM or a file for the specified
;	data and reports the addresses of any occurences. in the
;	case of a file, the entire file is read into RAM in order
;	to search as quickly as possible (a 16K file is searched
;	in less than 10 seconds) - the size of the file that may be
;	searched is limited therefore by memory size. an attempt
;	to read a file that is too large will result in an error
;	message - the file however will be compared as far as
;	possible.
;	
;	protocol: A>SEARCH RA STRING                  (RAM  ASCII data)
;		    SEARCH RH XX XX XX                (RAM  HEX   data)
;		    SEARCH FA STRING [FILENAME.EXT]   (file ASCII data)
;		    SEARCH FH XX XX XX [FILENAME.EXT] (file HEX   data)
;
;	note:	  since the CP/M command line is converted
;		  to UPPER CASE, the HEX data format must be
;		  used to search for lower case characters.
;
;		  addresses are given in HEX and offset octal format for
;		  RAM searches, and in sector-byte format for file
;		  searches.
;
BASE	EQU	4200H		; HEATH base
BDOS	EQU	BASE+5		; entry point to DOS
MEMSIZ	EQU	BASE+2		; last page of system memory
TBUFF	EQU	BASE+80H	; I/O buffer
TPA	EQU	BASE+100H	; Transient Program Area
WRCON	EQU	2		; write console character
PRINTF	EQU	9		; print routine
OPENF	EQU	15		; open file
READF	EQU	20		; read next record
SETDMA	EQU	26		; set DMA
BELL	EQU	7		; terminal bell
LF	EQU	10		; line feed
CR	EQU	13		; carriage return
ESC	EQU	27		; escape
SECTOR	EQU	128		; sector size
BDOSERR	EQU	255		; BDOS error code
SPACE	EQU	' '		; space
EOL	EQU	'$'		; string terminator
CHL	EQU	1894H		; ROM complement HL routine
COMP	EQU	1830H		; ROM compare routine
DU66	EQU	1846H		; ROM divide routine
CDEHL	EQU	188EH		; ROM compare DEHL routine
MOVE	EQU	18AAH		; ROM move routine
MU66	EQU	18DFH		; ROM multiply routine
RSTALL	EQU	1927H		; ROM restore all routine
SAVALL	EQU	192CH		; ROM save all routine
UDD	EQU	196FH		; ROM unpack routine
;
	ORG	TPA
START:	
	LDA	MEMSIZ		; get high memory page
	STA	LAST+1		; save
	LXI	H,TBUFF+1       ; point to first space
	MOV	A,M		; get char
	CPI	SPACE		; space?
	JNZ	SYNERR		; no: error
	LXI	H,TBUFF+4	; point to second space
	MOV	A,M		; get char
	CPI	SPACE		; space?
	JNZ	SYNERR		; no: error
	DCX	H		; point to mode byte 1
	DCX	H		;
	MOV	A,M		; get mode
	CPI	'F'		; file?
	JZ	GOTFILE		; yes: bypass
	CPI	'R'		; RAM?
	JNZ	MODERR		; error: process
STARTF:
	INX	H       	; point to mode byte 2
	MOV	A,M		; get mode
	CPI	'A'		; ASCII?
	JZ	GOTASCII	; yes: bypass
	CPI	'H'		; HEX?
	JZ	GOTHEX		; yes: bypass
MODERR:
	LXI	D,MODMSG	; print error message
	JMP	PRTERR		; bypass
SYNERR:
	LXI	D,SYNMSG	; msg pntr
PRTERR:
	CALL	PERR		; print it
	RET			; to CP/M
;
GOTHEX:
	LDA	TBUFF		; get command line count
	SUI	3		; adjust
	MVI	B,0		; divide by 3
	MOV	C,A		;
	LXI	D,3		;
	CALL	DU66		;
	MOV	A,L		; get result
	STA	TBUFF		; save as HEX data count
	MOV	C,A		; convert ASCII data
	CALL	CONVERT		;   to binary data
	LDA	TBUFF		; restore count again
	JMP	HEXSTART	; bypass
GOTASCII:
	LXI	H,TBUFF		; get command length
	MOV	A,M		;
	SUI	4		; adjust
HEXSTART:
	MOV	C,A		; set C to data count
	LXI	D,TBUFF+5	; set DE to data	
	LHLD	FIRST		; set HL to start of memory
LOOP:	PUSH	B		; save
	PUSH	D		;
	PUSH	H		;
	CALL	COMP		; compare data with memory
	POP	H		; restore
	POP	D		;
	POP	B		;
	JZ	FOUNDIT		; found?
	INX	H		; no: bump memory pntr
	CALL	CHECKEND	; finished?
	JNZ	LOOP		; no: continue
	LDA	FLAG		; was anything found?
	CPI	0		; 
	RNZ			; yes: return to CP/M
	LXI	D,NFMSG		; no: print 'not found'
	CALL	PSTR		;
	RET			; to CP/M
;
GOTFILE:
	LXI	H,TYPE
	INR	M	
	LXI	H,TBUFF+5	; pntr to first byte of cmd
	MVI	C,2		; reset cntr
CNTCHAR:
	INR	C		; cntr + 1
	MOV	A,M		; get char
	INX	H		; bump pntr
	CPI	'['		
	INX	D		;
	DCR	C		; done?
	JNZ	GETEXT1		; no: continue
DONE		; get cntr
	STA	TBUFF		; save cntr
	CALL	SAVTBUFF	; save tbuff
	SHLD	FNPNTR		; save filename pntr
;
	LXI	D,FILEFCB	; FCB pntr
	INX	H		; bump pntr to possible ':'
	MOV	A,M		; get byte
	CPI	':'		; drive specified?
	JNZ	NODRV		; no: bypass
	DCX	H		; get drive
	MOV	A,M
	SUI	'A'-1		; subtract offset
	STA	FILEFCB		; save
	INX	H		; pntr to first letter
	INX	H		;
	SHLD	FNPNTR		; save
	JMP	GET1		; bypass
NODRV:	
	DCX	H		; pntr to first letter
GET1:	
	LXI	D,FILEFCB+1     ; pntr to FILEFCB+1
	MVI	C,8		; cntr
GETBYT:
	MOV	A,M		; get byte
	CPI	'.'		; beginning of .EXT?
	JNZ	GET2		; no: bypass
	INX	H		; bump pntr
	JMP	GETEXT		; bypass
GET2:
	STAX	D		; save	
	INX	H		; bump pntrs
	INX	D		;
	DCR	C		; done?
	JNZ	GETBYT		; no: continue
	MOV	A,M		; get byte
	CPI	'.'		; extent specified?
	JNZ	DONEGET		; no: bypass
GETEXT:
	MVI	C,3		; move ext
	LXI	D,FILEFCB+9	; set pntr
GETEXT1:
	MOV	A,M		;
	STAX	D		;
	INX	H		; bump pntrs
	INX	D		;
	DCR	C		; done?
	JNZ	GETEXT1		; no: continue
DONEGET:
	CALL	READFILE	; get file
	CALL	RSTTBUFF	;
	LXI	H,TBUFF+2	; setup
	JMP	STARTF		;
SAVTBUFF:
	CALL	SAVALL
	LXI	B,50		;
	LXI	D,TBUFF
	LXI	H,TEMPBUF
	CALL	MOVE
	JMP	RSTALL
RSTTBUFF:
	CALL	SAVALL
	LXI	B,50
	LXI	D,TEMPBUF
	LXI	H,TBUFF
	CALL	MOVE
	JMP	RSTALL
;
FOUNDIT:
	MVI	A,1		; set flag
	STA	FLAG		;
	LDA	TYPE
	ORA	A
	JZ	FOUND1
;
	PUSH	B
	PUSH	D
	PUSH	H
	LHLD	FIRST
	CALL	CHL
	MOV	D,H
	MOV	E,L
	POP	H
	PUSH	H
	DAD	D
	MOV	B,H	
	MOV	C,L
	LXI	D,SECTOR
	CALL	DU66
	PUSH	D
	INX	H
	MOV	B,H
	MOV	C,L
	MVI	A,3
	LXI	H,FFMSG2
	CALL	UDD
	POP	D
	INX	D
	MOV	B,D
	MOV	C,E
	MVI	A,3
	LXI	H,FFMSG3
	CALL	UDD
	LXI	D,FFMSG1
	CALL	PSTR
	POP	H
	POP	D
	POP	B
   	JMP	FOUND2
FOUND1:
	PUSH	D		; save
	LXI	D,FMSG		; print 'found'
	CALL	PSTR		;
	POP	D		; restore
	CALL	PADDR		; print addr (hex and offset octal)
FOUND2:
	CALL	CHECKEND	; finished?
	RZ			; yes: return to CP/M
	INX	H		; no: bump memory pointer
	JMP	LOOP		; continue
;
;	compare present address with high memory limit
;
CHECKEND:
	PUSH	D		; save
	PUSH	H		;
	XCHG			; DE = present addr
	LHLD	LAST		; HL = high memory limit
	CALL	CDEHL		; done?
	POP	H		; restore
	POP	D		;
	RET			; return
;
;	print address in hex and split octal
;
PADDR:
	PUSH	B		; save
	PUSH	D		;
	PUSH	H		;
	MOV	A,H		; print high HEX byte
	CALL	PHEX		;
	MOV	A,L		; print low HEX byte
	CALL	PHEX		;
	MVI	A,' '		; print space
	CALL	PCHAR		;
	MVI	A,'('		; print '('
	CALL	PCHAR		;
	POP	H		; restore address
	PUSH	H		; save again
	MOV	A,H		; print high OCTAL byte
	CALL	POCT		;
	MVI	A,'.'		; print '.'
	CALL	PCHAR		;
	POP	H		; restore address
	PUSH	H		; save again
	MOV	A,L		; print low OCTAL byte
	CALL	POCT		;
	MVI	A,')'		; print ')'
	CALL	PCHAR		;
	MVI	A,CR		; print CRLF
	CALL	PCHAR		;
	MVI	A,LF		;
	CALL	PCHAR		;
	POP	H		; restore
	POP	D		;
	POP	B		;	
	RET			; return
;
;	print hex byte in register A
;
PHEX:
	PUSH	PSW		; save
	RRC			; shift high 4 bits
	RRC			;
	RRC			;
	RRC			;
	CALL	PNIB		; print it
	POP	PSW		; restore
	CALL	PNIB		; print it
	RET			; return
;
PNIB:	
	ANI	0FH		; mask out high 4 bits
	CPI	10		; alphabetic?
	JNC	P10		; yes: bypass
	ADI	'0'		; add ASCII offset
	JMP	PRN		; bypass
P10	ADI	'A'-10		; add ASCII & letter offset	
PRN	CALL	PCHAR		; print it
	RET			; return
;
;	print character in register A
;
PCHAR:
	PUSH	B		; save
	PUSH	D		;
	PUSH	H		;
	MVI	C,WRCON		; print CON: char function
	MOV	E,A		; set up -> E
	CALL	BDOS		; do it
	POP	H		; restore
	POP	D		;
	POP	B		;
	RET			; return
;
; 	print EOL terminated string pointed to by DE
;
PSTR:	
	PUSH	B		; save	
	PUSH	D		;
	PUSH	H		;
	MVI	C,PRINTF	; print CON: string function
	CALL	BDOS		; do it
	POP	H		; save
	POP	D		;
	POP	B		;
	RET			; return
;
;	print octal byte in register A
;
POCT	PUSH	PSW		; save
	RRC			; shift left 2 bits
	RRC			;
	RRC			;
	RRC			;
	RRC			;
	RRC			;
	ANI	03H		; mask out high 6 bits
	ADI	'0'		; add ASCII offset
	CALL	PCHAR		; print it
	POP	PSW		; restore
	PUSH	PSW		; save again
	RRC			; shift middle 3 bits
	RRC			;
	RRC			;
 	ANI	07H		; mask out high 5 bits
	ADI	'0'		; add ASCII offset
	CALL	PCHAR		; print it
	POP	PSW		; restore
	ANI	07H		; mask out high 5 bits
	ADI	'0'		; add ASCII offset
	CALL	PCHAR		; print it
	RET			; return
;
; 	convert HEX ASCII data to binary and save at TBUFF+4
;
CONVERT:
	LHLD	APNTR		; get ASCII pntr
	MOV	A,M		; get ASCII char
	CPI	'A'  		; hex letter?
	JC 	NOTLET1		; no:bypass
	SUI	'A'-10		; yes: subtract ASCII & letter offset
	JMP	SHIFT1		; bypass
NOTLET1:
	SUI	'0'		; subtract ASCII offset
SHIFT1:
	RLC			; shift to high bit positions
	RLC			;
	RLC			;
	RLC			;
	MOV	B,A		; save
	INX	H		; bump ASCII pntr
	MOV	A,M		; get ASCII char
	CPI	'A'  		; hex letter?
	JC 	NOTLET2		; no: bypass
	SUI	'A'-10		; yes: subtract ASCII & letter offset
	JMP	MERGE 		; bypass
NOTLET2:
	SUI	'0'		; subtract ASCII offset
MERGE:
	ORA	B		; combine high and low order bits
	LHLD	BPNTR		; get binary pntr
 	MOV	M,A		; save binary data
	INX	H		; bump binary pntr
	SHLD	BPNTR		; save
	LHLD	APNTR		; restore ASCII pntr
	INX	H		; bump it 3 times (past space)
	INX	H		;
	INX	H		;
	SHLD	APNTR		; save
	DCR	C		; finished?
	JNZ	CONVERT		; no: continue
	RET			; return
;
;	open file at FILEFCB
;
OPENFILE:
	MVI	C,OPENF		; open file
	LXI	D,FILEFCB	;
	CALL	BDOS		;
	CPI	BDOSERR		; error?
	RET
;
;	read file into RAM beginning at BUFFER
;
READFILE:	
	CALL	OPENFILE	; open: error?
	JNZ	READ1		; no: bypass
	POP	PSW		; discard RET addr
	LXI	D,OPENERR	; print error
	JMP	PRTERR
READ1:
	LXI	D,BUFFER	; set DMA addr to BUFFER
	CALL	SETADDR		;
	MVI	C,READF		; read file until EOF
     	LXI	D,FILEFCB	;
RLOOP:
	PUSH	B		; save
	PUSH	D		;
	PUSH	H		;
	CALL	BDOS		;
	LXI	H,TOTAL		; increment total read
	INR	M		;
	STA	TEMP
	POP	H		; restore
	POP	D		;
	POP	B		;
	CALL	INRADDR		; 
	JC 	TOOBIG
	LDA	TEMP
	ORA	A
	JZ 	RLOOP		; no: continue
;
TOOBIG:
	LXI	H,TOTAL		; read past end: adjust
	DCR	M
	LXI	D,TBUFF		; reset DMA
	CALL	SETADDR		;
	LXI	H,BUFFER	; set pntr to first addr
	SHLD	FIRST		;
	PUSH	H
	LXI     B,SECTOR	; multiply total * 128
	LHLD	TOTAL		;
	XCHG			;
	CALL	MU66		;
	XCHG
	POP	H
	DAD	D
	SHLD	LAST		; save
 	RET			;
INRADDR:
	PUSH	B
	PUSH	D
	PUSH	H
	LHLD	DMAADDR		; add 128 to DMA addr
	LXI	D,128
	DAD	D
	SHLD	DMAADDR
	XCHG
	CALL	SETADDR
;
	LHLD	DMAADDR
	LDA	MEMSIZ
	DCR	A
	SUB	H
	JNC	SIZOK
	LXI	D,SIZMSG
	CALL	PERR 
	STC
SIZOK:
	POP	H
	POP	D
	POP	B
	RET
SETADDR:
	CALL	SAVALL
	MVI	C,SETDMA	; set DMA addr to DE
	CALL	BDOS		;
	JMP	RSTALL
PERR:
	PUSH	D
	LXI	D,ERRMSG
	CALL	PSTR
	POP	D
	CALL	PSTR
	RET
;
;	data storage
;
TEMPBUF	DS	50
TEMP	DB	0
FLAG	DB	0
TYPE	DB	0
TOTAL	DB	0
FIRST	DW	0
LAST	DB	0FFH,0
DMAADDR	DW	BUFFER
MODMSG	DB	'illegal mode',CR,LF,EOL	
SYNMSG	DB	'illegal syntax'
	DB	CR,LF,EOL
OPENERR DB	'file cannot be found',CR,LF,EOL
FMSG	DB	'  data found at address: ',EOL
FFMSG1	DB	'  data found in sector: '
FFMSG2	DS	3
      	DB	' byte: '
FFMSG3	DS	3
	DB	CR,LF,EOL
NFMSG	DB	'  data cannot be found',CR,LF,EOL
SIZMSG	DB	'file too large',CR,LF,CR,LF,EOL
ERRMSG	DB	BELL,'  error: ',EOL
APNTR	DW	TBUFF+5		; ASCII pntr
BPNTR	DW	TBUFF+5		; binary pntr
FNPNTR	DW	0		; filename pntr
FILEFCB	DB	0,'           ',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
BUFFER	EQU	$
;
	END	START
