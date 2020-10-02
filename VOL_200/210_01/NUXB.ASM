	TITLE 'Loadable bios module for CP/M v.2.2'
	PAGE 62

;9/20/86--N.T.Carnevale
;After C.Sondgeroth's XBIOS--see Micro/Systems Journal vol.1 #2 p.66 et seq.
;See also my article in the July/August 1986 issue of M/S J pp.72-85.
;
;7/12/85 Modified to allow use of a general xbios loader,
;and to enable use of a special utility to remove xbios's--NTC
;Compile with RMAC, then link like so:
;
;    link nuxb[os]
;
;to produce an .SPR file for use by the bios extension loader LXB


CR	EQU	0DH
LF	EQU	0AH
CDISK	EQU	4	;addr holds currently logged drive
PRINTF	EQU	9	;bdos "print string" function


;These macros code for strings to identify the bios extension.
;To allow easy manipulation by C programs, each macro should 
;end with a null.
;NOTE:	the total length of the data produced by these two macros 
;must be 225 bytes or less.

VERSION MACRO
	;; revision number & date
	;; terminate this macro with a null
	DB 'v.1.1 7/12/85'
	DB 0
	ENDM

FUNCT	MACRO
	;; tell what it's for
	;; put in as many lines as desired, with cr lf as needed
	;; restrictions:  
	;; --terminate this macro with a null
	;; --total length of the data produced by the version and funct
	;;   macros must be 255 bytes or less  
	DB 'Tests new version'
	DB 0
	ENDM


;**********************
;This header identifies the xbios module and provides vital information
;used by the loader and other utilities

	;"continuation jump" to bdos--patched to correct location
	;by xbios loader.  If this is the only xbios module, this 
	;jmp goes directly to the original bdos.  Otherwise, it 
	;goes to the xbios module that was previously loaded.
BDOS:	JMP $-$

	;distance from end of this header to the local copy 
	;of the original bios's entry table, i.e. total length 
	;of the version and function strings.
	DB OLDTBL-HDREND

	;how many jump instructions copied from the bios's table
	DB (TBLEND - OLDTBL)/3

	;Next 11 bytes reserved for future use
RESERV: DB 0
	DW 0,0,0,0,0

	;The next string, which is exactly 16 bytes long and starts 
	;exactly 16 bytes from the first location in the xbios,
	;is checked by utilities that manipulate resident xbios modules.
LABEL:	DB 'xbios installed$'

HDREND	EQU $

;end of fixed-length header
;**********************


	;strings that tell what it is & what it does
	VERSION
	FUNCT


;When the bios extension is first installed, the contents of the
;original bios jump table are copied into the following table.
;These jumps allow calls to any bios routines that are not 
;supposed to be intercepted. 
;If your bios has additional jumps that you might want to intercept,
;add more entries to this table

OLDTBL	EQU $

WBOOT	JMP $-$
CONST	JMP $-$
CONIN	JMP $-$
CONOUT	JMP $-$
LIST	JMP $-$
PUNCH	JMP $-$
READER	JMP $-$
HOME	JMP $-$
SELDSK	JMP $-$
SETTRK	JMP $-$
SETSEC	JMP $-$
SETDMA	JMP $-$
READ	JMP $-$
WRITE	JMP $-$
LISTS	JMP $-$
SECTRAN JMP $-$

TBLEND	EQU $


;This table should be the same length as the OLDTBL.
;It defines new entry points for the bios routines that
;are to be modified.  Any routines that are not to be intercepted
;should jump to the corresponding entries in the previous table.
;Eventually they will reach the original bios entry points.

NEWTBL	EQU $

	JMP XWBOOT	;to special bios extension "warm boot" routine. 
			;This is the only jump that cannot go to the 
			;corresponding entry in the previous table.
	;the rest of these may be modified to intercept the bios call
	JMP CONST
	JMP CONIN
	JMP CONOUT
	JMP LIST
	JMP PUNCH
	JMP READER
	JMP HOME
	JMP SELDSK
	JMP SETTRK
	JMP SETSEC
	JMP SETDMA
	JMP READ
	JMP WRITE
	JMP LISTS
	JMP SECTRAN


;This is the special warm boot routine that intercepts CP/M's attempt
;to overwrite the call to the bios extension's own jump table.	It
;bypasses the reload of the CCP and reinitialization of the low memory
;jump vectors that are part of a normal CP/M 2.2 warm boot.
XWBOOT:	LXI SP,80H	;reset stack pointer
	LXI B,80H	;set default dma address
	CALL SETDMA
	LXI H,BDOS
	SHLD 6		;fix "jmp bdos" at 0005 to point to this module
	LXI D,CRLF
	MVI C,PRINTF
	CALL BDOS	;send cr lf to console
	LXI D,LABEL
	MVI C,PRINTF
	CALL BDOS	;say xbios is running
	LHLD 1		;get bios+3 address
	LXI D,-1600H	;calculate entry point of CCP
	DAD D
	LDA CDISK
	MOV C,A
	PCHL		;jump into CCP

CRLF:	DB CR,LF,'$'
	END
