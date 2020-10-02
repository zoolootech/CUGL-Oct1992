	TITLE	'Extended Sorted Directory'
  	;++ ****************************************
	;
	;$$ XSDIR		written by:	S.J.Singer
	;			modified by:	H.R.Moran Jr.
	;
	; Original file was XDIR by S.J.Singer CPMUG vol 24
	;
	; XSDMAC.LIB is the same as MACRO.LIB (  S.J.Singer CPMUG vol 24)
	; with two exeptions. The removal of a conditional assembly
	; in the MOVE macro (which did not work properly) and
	; the addition of macro AMATCH (ambiguous match) which was but
	; is no longer used by this program.
	;
	; Modifications:
	;
	;	1) Allow specification of ambiguous file names.
	;	2) Count remaining space correctly even if not
	;	   all files are displayable.
	;	3) Made useable on double density or non-standard diskettes.
	;	4) Four column format instead of three.
	;	5) Show indication of the number of directory entries used.
	;	6) Comment more thoroughly to ease further modification.
	;
	;
	; XSDIR is a CPM utility that displays
	; a disk directory in a more readable form
	; than DIR. The directory is read from the
	; specified drive, sorted then displayed in a
	; 4 column format. Both the file names and
	; file sizes in 1K groups are displayed.
	; XSDIR is written in the Digital Research
	; Macrossembler and uses a large number of Macros.
	; these are contained in a library called
	; MACRO.LIB (written by S.J.Singer) which
	; is required if the program is to be re-assembled.
	;
	;
	;	COMMAND FORMAT
	;
	; XSDIR		Display Directory of Logged disk
	; XSDIR {<drive>:}{<ambiguous file name}
	;
	; Where items enclosed in braces are optional and
	; items enclosed in <> are classes.
	;
	;
	; examples:
	;	XSDIR ?ABC*.COM
	;
	; Include all files whose type is .COM and
	; whose second,third and fourth characters
	; of their names are A,B,C respectively.
	;
	;	XSDIR B:
	;
	; Include all files on drive B.
	;
	;	XSDIR B:*.ASM
	;
	; Include all files on drive B whose type
	; is .ASM.
	;
	;-- ****************************************
	;
	MACLIB	XSDMAC		;INCLUDE MACRO LIBRARY
	PAGE
	;
	;
	; Parameter Definitions
	;
	;----- HARDWARE SPECIFIC PARAMETERS -----
	;
SMALL	EQU	0		;64 wide CRT is false (1 => true)
CRTLEN	EQU	20		;Number of physical lines on console device
DSKSIZ	EQU	241		;Number of kbytes on a disk
	;
	;----- END HARDWARE SPECIFIC  PARAMETERS -----
	;
LINES	EQU	CRTLEN-4	;Lines per page on display
ENTRIES	EQU	128		;Max number of directory entries
	;
TBUFF	EQU	80H		;Transient buffer
TFCB	EQU	5CH		;Transient FCB
BDOS	EQU	5		;Entry to Basic Disk Operating system
BOOT	EQU	0		;Entry to warmboot in BIOS
	;
	; Make sure we can't point past the directory pointer table
	;
DIRLEN	EQU	2*(ENTRIES+1)		;Pointer table length
	;
	;
	PAGE
	;
	ORG	100H		;CP/M compatible origin
	;
XSDIR:
BEGIN:	LXI	H,0
	DAD	SP		;Save CCP Stack-pointer
	SHLD	CCPSTK
	;
	LXI	SP,LCLSTK	;Init Local Stack-pointer
	;
DIR:	LDA	TFCB+1		;1'st char of ambiguous file name
	STA	ALLF		;Save as a flag for the
				;bytecount print format routine
	CPI	' '
	JNZ	ADDED		;Was a name match specified ?
				;No, make a match anything FCB.
	FILL	TFCB+1,TFCB+12,'?'
	FILL	TFCB+13,TFCB+32
ADDED:	MVI	A,'?'
	STA	TFCB+12		;Force match of all extents
	LDA	TFCB		;A=drive specifier
	ORA	A
	JNZ	SPECDRIV	;Was drive explicitly specified ?
	DISKIO	?DRIVE		;No,use the default drive
	INR	A
SPECDRIV:
	ADI	'A'-1		;Make it ASCII.
	STA	DRIVEID		;Save the ASCII drive id.
	PRINT	<CR,LF>
	XRA	A
	STA	COUNT		;Count of Directory entries=0
	FILL	PDIR,PDIR+DIRLEN;Zero the pointer table
	LXI	H,DIRBUF 	;HL=.(Directory buffer)
	SHLD	OUTBPTR		;OUTBPTR=.(Directory buffer)
	LXI	H,PDIR		;HL=.(Pointer table)
	SHLD	IPOINT		;IPOINT=.(Pointer table)
	;
	; Read the specified portion of the
	; directory into a buffer. If the Extent
	; is non-zero then replace the current
	; entry in the buffer thereby adding to its
	; allocation count.
	;
	DISKIO	SEARCH,TFCB	;Search for 1'st match in directory
	CPI	255
	JZ	ENDFIL		;Was a match found ?
	ANI	3		;Yes,continue
	JMP	DIR6
	;
DIR4:	DISKIO	SERNXT,TFCB	;Search for next match in directory
	CPI	255
	JZ	SORT		;Was another matching entry found ?
	ANI	3		;Yes
DIR6:	LHLD	OUTBPTR
	XCHG			;DE=destination pointer
	LXI	H,TBUFF		;Calculate pointer to entry in TBUFF
	ADD	A
	ADD	A
	ADD	A
	ADD	A
	ADD	A		;Multiply byte pointer by entry size (32)
	ADD	L		;Construct pointer to entry
	MOV	L,A
	JNC	DIR8
	INR	H
DIR8:	SHLD	INBPTR	;Save pointer to entry
	INX	H	;bypass 1'st byte (0 or 0E5H)
	SAVE	H,D
	LXI	D,11
	DAD	D	;HL=.(extent byte)
	MOV	A,M	;A=extent byte
	ORA	A
	JZ	DIR10	;Is this the zeroth extent ?
	LXI	H,0	;No, search for same name and 'switch' entries
	SHLD	J	;J=0 (Initialize index)
DIR9:	DLOAD	PDIR,J	;HL=PDIR(J)
	DJZ	DIR10	;Is the table empty ?
	XCHG		;No,search for a match with current entry
	LHLD	INBPTR	;HL=.(current entry)
	SAVE	D,H
	INX	H	;bypass drive id
	MATCH	,,11	;Compare name and type (11 chars)
	RESTORE	H,D
	JZ	SWITCH	;Do they match ?
			;No, compare with next saved entry
	INDEX	J,2	;J=J+2 (bytes i.e. 1 entry)
	JMP	DIR9
	;
SWITCH:	INX	H	;bypass drive id
	MOVE	,,15	;Overwrite the old saved entry
	RESTORE	H
	JMP	DIR4	;Read another entry from Disk Directory
	;
DIR10:	RESTORE	D,H
	MOVE	,,15	;Save the current entry
	LDA	COUNT
	INR	A
	STA	COUNT	;COUNT=COUNT+1
	LHLD	OUTBPTR
	DSTORE	0,IPOINT;Update pointer table to reflect new entry
	INDEX	OUTBPTR,16	;OUTBPTR=OUTBPTR+16 (bytes  i.e. 1 entry)
	INDEX	IPOINT,2;IPOINT=IPOINT+2 (bytes i.e. 1 entry)
      	JMP	DIR4	;Read another entry from Disk Directory
	;
	; This routine prints the directory
	; in 4 columns. The number of lines
	; Printed is controlled by the variable LINES.
	; All matched directory names are present
	; in the table but only a maximum of
	; 4 times LINES will be printed.
	;
DIR14:	LXI	H,0
	SHLD	W		;Initialize the count of kbytes on disk.
	SHLD	I		;Initialize the pointer index
DIR16:	DLOAD	PDIR+LINES*0,I	;pointer to column 1
	DJZ	ENDFIL		;Exit if pointer is zero (i.e. empty)
	CALL	DIR20		;Print column 1 entry
	;
	IF SMALL = 1		;64 wide CRT ?
	PRINT	<'|'>		;Yes, print short spacer
	ELSE			;otherwise No
	PRINT	<' | '>		;print spacer
	ENDIF
	;
	DLOAD	PDIR+LINES*2,I	;pointer to column 2
	DJZ	DIR18		;bypass if pointer is zero (i.e. empty)
	CALL	DIR20		;Print column 2 entry
	;
	IF SMALL = 1		;64 wide CRT ?
	PRINT	<'|'>		;Yes, print short spacer
	ELSE			;otherwise No
	PRINT	<' | '>		;print spacer
	ENDIF
	;
	DLOAD	PDIR+LINES*4,I	;pointer to column 3
	DJZ	DIR18		;bypass if pointer is zero (i.e. empty)
	CALL	DIR20		;Print column 3 entry
	;
	IF SMALL = 1		;64 wide CRT ?
	PRINT	<'|'>		;Yes, print short spacer
	ELSE			;otherwise No
	PRINT	<' | '>		;print spacer
	ENDIF
	;
	DLOAD	PDIR+LINES*6,I	;pointer to column 4
	DJZ	DIR18		;bypass if pointer is zero (i.e. empty)
	CALL	DIR20		;Print column 4 entry
DIR18:	PRINT	CRLF,$
	INDEX	I,2		;I=I+2 (bytes i.e. 1 entry)
	LXI	D,LINES*2	;Check index limit
	CPHL
	JNZ	DIR16		;Is console screen full ?
	MVI	A,1		;Yes, set Screen Full Flag
	STA	SFF		;SFF=1
	;
DIR16A:	DLOAD	PDIR+LINES*6,I	;No, Add in sizes of remaining files
	DJZ	ENDFIL
	CALL	DIR20A
	INDEX	I,2
	LXI	D,2*ENTRIES
	CPHL
	JNZ	DIR16A		;Max entries exceeded ?
	JMP	ENDFIL		;Yes, finish up and exit
	;
	;
	; Subroutine to print a single
	;	Directory entry
	;
DIR20:	MVI	C,8		;C=name length
DIR22:	SAVE	B,H		;Save registers from BDOS
	MVI	C,2		;C=print console function
	MOV	E,M		;E=char to be printed
	CALL	BDOS
	RESTORE	H,B		;Restore the registers
	INX	H		;Increment the char pointer
	DCR	C		;Decrement the character count
	JNZ	DIR22		;Has the name been fully printed ?
	;			;Yes
	IF SMALL = 0
	SAVE
	PRINT	<' '>		;Print spacer
	RESTORE
	ENDIF
	;
	MVI	C,3		;Print file type
DIR22B:	SAVE	B,H
	MVI	C,2
	MOV	E,M
	CALL	BDOS
	RESTORE	H,B
	INX	H
	DCR	C
	JNZ	DIR22B		;Has type been fully printed ?
	;
	JMP	DIR24		;Yes,print file size
	;
	; Add in sizes of files which won't fit on screen
	;
DIR20A:	LXI	B,11
	DAD	B		;HL=.(extent byte)
	;
  	; Accumulate the file size in kbytes
	;
DIR24:	MOV	A,M		;A=extent byte
	XCHG			;DE=.(entry)
	MVI	H,0
	MOV	L,A		;HL=extent byte
	LDA	FCBCNT
	ADD	L
	INR	A
	STA	FCBCNT
	DAD	H
	DAD	H
	DAD	H
	DAD	H		;HL=HL*16 kbytes per extent
	INX	D
	INX	D
  	INX	D		;DE=.(number of records byte)
	LDAX	D		;A=number of records in final extent
	RRC
	RRC
	RRC			;A=A/8 records per kbyte
	PUSH	PSW
	ANI	1FH		;Round up if there is a fractional kbyte
	ADD	L		;(note: no carrys are possible)
	MOV	L,A		;HL=# of full kbytes in file
	POP	PSW
	ANI	0E0H		;A=number of records mod 8
	JZ	DIR26		;Does final extent contain a partial kbyte ?
	INX	H		;Yes, add one kbyte for the partial one
DIR26:	LDA	SFF		;A=Screen Full Flag
	ORA	A	;
	JNZ	DIR30A		;Is the console screen full ?
				;No, continue displaying
	;Print spacing based on how many digits in the kbyte count
	;
	SAVE	H
	LXI	D,100
	CPHL
	JM	DIR26A
	PRINT	' '		; 3 digits => 1 space
	JMP	DIR30
	;
DIR26A:	RESTORE	H
	SAVE	H
	LXI	D,10
	CPHL
	JM	DIR28
	PRINT	'  '		; 2 digits => 2 spaces
	JMP	DIR30
	;
DIR28:	PRINT	'   '		; 1 digit  => 3 spaces
	;
	; Add kbytes in this file to total kbytes counted
	;
	;
DIR30:	POP	H
	PUSH	H
	XCHG
	LHLD	W
	DAD	D
	SHLD	W		;W=W + kbytes in this file
	POP	H
	DECOUT	;Print the number of kbytes in this file as decimal
	RET
	;
DIR30A:	PUSH	H
	XCHG
	LHLD	W
	DAD	D
	SHLD	W		;W=W + kbytes in this file
	POP	H
	RET
	;
	;This is the exit point from the program.
	;Print the number of files and the space remaining.
	;Restore the CCP stack pointer and return to CCP.
	;
ENDFIL:	PRINT	<CR,LF>
	PRINT	<'Drive - '>
	CHAROUT	DRIVEID		;Print ASCII of specified or default disk
	PRINT	<' '>
	DECOUT	COUNT
	PRINT	' Files ('
	DECOUT	FCBCNT
	PRINT	' entries) '
	LHLD	W
	LDA	ALLF
	CPI	' '
	JNZ	SOME		;Were all files printed?
	LXI	B,DSKSIZ	;Yes, calculate remaining space
	MOV	A,C
	SUB	L
	MOV	L,A
	MOV	A,B
	SBB	H
	MOV	H,A
SOME:	DECOUT		;Print either sum of files matched or remaining space
	LDA	ALLF
	CPI	' '
	JNZ	SOME2		;Were all files printed?
				;Yes
				;Show the space remaining on disk.
	IF SMALL = 1
	PRINT	<'K bytes remaining',CR,LF>
	ELSE
	PRINT	<'K bytes remaining on disk',CR,LF>
	ENDIF
	;
	JMP	EF1
				;show sum of files printed
				;if not all files were printed
SOME2:
	IF SMALL = 1
	PRINT	<'K bytes matched',CR,LF>
	ELSE
	PRINT	<'K bytes in the matched files',CR,LF>
	ENDIF
	;
EF1:	LHLD	CCPSTK
	SPHL			;Restore the CCP stack-pointer
	RET			;Return to CCP without reboot.
	;
	;This section does the actual sorting
	;of the directory. During the
	;input of the directory names,
	;a table of address pointers PDIR
	;was constructed. The SORT routine
	;sorts the address pointers
	;rather than the actual directory. 
	;this is an implementation of
	;C. A. R. Hoare's QUICKSORT algorithm.
	;The algorithm is very fast and generally
	;useful, however caution
	;should be used with large files.
	;The algorithm is recursive and
	;the stack space required is proportional
	;to the number of items to be sorted.
	;
SORT:	LDA	COUNT		;A=number of entries found
	ORA	A
	JZ	ENDFIL		;Were any entries found ?
	DCR	A		;Yes, A=number of entries-1
	LXI	H,0
	MOV	L,A
	DAD	H		;HL=2*(number of entries-1)
	SHLD	LAST		;End of Array is HL
	LXI	H,0
	SHLD	FIRST		;Start of Array is zero
	LXI	H,-1
	PUSH	H		;Flag stack as being empty
	LHLD	FIRST
	PUSH	H
	LHLD	LAST
	PUSH	H		;Stack contains first and last indices
	;
	;Pop stack and recursively call SPLIT until stack is empty
	;
SORT2:	POP	H
	MOV	A,H
	CPI	0FFH
	JZ	DIR14		;Is stack empty ?
	SHLD	J		;No, continue sorting
	SHLD	LAST		;J=LAST=HL
	POP	H		;HL=stack
	SHLD	I
	SHLD	FIRST		;I=FIRST=HL
	CALL	SPLIT
	LHLD	I
	XCHG			;DE=I
	LHLD	FIRST		;HL=FIRST
	CPHL
	JZ	SORT4		;Is I=FIRST ?
	PUSH	H		;No, stack new I
	DCX	D
	DCX	D
	PUSH	D		;stack new J
SORT4:	LHLD	J
	XCHG			;DE=J
	LHLD	LAST		;HL=LAST
	CPHL
	JZ	SORT8		;Is J=LAST ?
	INX	D		;No
	INX	D
	PUSH	D		;stack new I
	PUSH	H		;stack new J
SORT8:	JMP	SORT2
	;
	;SPLIT subroutine does a single
	;partition on an array of pointers
	;
SPLIT:	HALF	I
	XCHG			;DE=I/2
	HALF	J		;HL=J/2
	DAD	D		;HL=(I+J)/2
	MOV	A,L
	ANI	0FEH
	MOV	L,A
	SHLD	K		;K=(I+J)/2 forced to even number
	DLOAD	PDIR,K
	SHLD	W		;W=PDIR(K)
SPLIT2:	DLOAD	PDIR,I
	XCHG			;DE=PDIR(I)
	LHLD	W		;HL=.(partition element) i.e. PDIR(K)
	MATCH	,,11		;Compare keys
	JP	SPLIT4		;Is DIRBUF(PDIR(I)) < DIRBUF(PDIR(K)) ?
				;Yes (or is it No ?)
	INDEX	I,2		;I=I+2 (bytes i.e. 1 element)
	JMP	SPLIT2
	;
SPLIT4:	DLOAD	PDIR,J
	XCHG			;DE=PDIR(J)
	LHLD	W		;HL=.(partition element) i.e. PDIR(K)
	XCHG			;swap DE with HL
	MATCH	,,11		;compare keys
	JP	SPLIT6		;Is DIRBUF(PDIR(K) < DIRBUF(PDIR(K)) ?
				;Yes (or is it No ?)
	INDEX	J,-2		;J=J-2 (bytes i.e. 1 element)
	JMP	SPLIT4
	;
SPLIT6:	LHLD	I
	XCHG			;DE=I
	LHLD	J		;HL=J
	CPHL
	RZ			;Is I=J ?
	DLOAD	PDIR,I		;No, switch pointers
	SAVE	H
	DLOAD	PDIR,J
	DSTORE	PDIR,I		;PDIR(J)=PDIR(J)
	RESTORE	H
	DSTORE	PDIR,J		;and vice versa
	JMP	SPLIT2
	;
	; Data Area
	;
DRIVEID:	DS	1		;ASCII drive id
ALLF:	DS	1		;ALL Files printed Flag
SFF:	DB	0		;Screen full flag (init to 0 i.e. false)
SPACE:	DB	' $'		;ASCII Space
CRLF:	DB	0DH,0AH,24H	;ASCII CR LF
I:	DW	0		;pseudo index register
J:	DW	0		;pseudo index register
K:	DW	0		;pseudo index register
FIRST:	DW	0		;start of Array
LAST:	DW	0		;end of Array
W:	DW	0		;storage for partition index
LINE:	DW	0		;line number for listing
IPOINT:	DW	0		;variable buffer pointer
COUNT:	DW	0		;count of directory entries
FCBCNT:	DW	0		;count of directory entries used
INBPTR:	DW	0		;pointer to input buffer
OUTBPTR:
	DW	0		;pointer to directory buffer
CCPSTK:	DW	0		;saved CCP stack pointer
	;
ENDSTK:	DS	ENTRIES*20+10	;local stack area
LCLSTK:				;local top of stack (initial)
	;
PDIR	DS	DIRLEN		;pointer table to directory buffer
	;
DIRBUF:	DS	ENTRIES*16	;directory buffer
	;
	END	BEGIN
                                                                                                       