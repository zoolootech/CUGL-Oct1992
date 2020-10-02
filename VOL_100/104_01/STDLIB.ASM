	title	small c i/o package
	extrn	bios
bdos	equ	5		;address on entry address for bdos
cpmcmd  equ	80h		;address of cpm command line
true	equ	1		;value for true
false	equ	0		;value for false		
;
;	toupper
;
;	function: to shift lower case character to upper
;
;	calling format from "c"
;	toupper(char);
;
toupper:csect
	mov	a,l		;get character to test
	cpi	61h		;check to see if less then 'a'
	rc			;yes return to caller
	cpi	7bh		;check to see if greater then 'z'
	rnc			;yes..return to caller
	ani	5fh		;mask off lowwer case bit
	mov	l,a		;put it in reg for return
	ret			;return to caller
;
;	tolowwer
;
;	function: to convert character to lowwer case
;
;	calling format from "c"
;	tolowwer(char)
;
tolowwer:csect
	mov	a,l		;get character to shift to lowwer case
	cpi	'A'		;see if less then 'A'
	rc			;yes...return to caller
	cpi	'Z'+1		;see if greater then 'Z'
	rnc			;yes...return to caller
	ani	0bfh		;remove upper case bit
	mov	l,a		;put chracter back in place
	ret
;
;	isalpha
;
;	function check to see if alpha character a-z or A-Z only
;
;	calling format from "c"
;	isalpha(char)
;
isalpha:csect
	push	h		;save calling arg for later
	call	isupper		;check to see if upper case
	xchg			;put answer in de
	pop	h		;get character to test back
	call	islowwer	;check to see if lowwer case
	dad	d		;add isupper to is lowwer answer
	ret

;
;	isupper
;
;	function: to check to see if character is upper case
;
;	calling format in "c"
;	isupper(char) 
;
isupper:csect
	mov	a,l		;get character to check
	cpi	'A'		;see if less then upper case a
	jc	isupper1	;not upper case
	cpi	'Z'+1		;check to see if less then 
	jnc	isupper1	;no upper case
	lxi	h,true		;yes it is upper case
	ret
isupper1:
	lxi	h,false		;not upper case
	ret
;
;	islowwer
;
;	function: to check to see if character is lowwer case
;
;	calling format from "c"
;	islowwer(char)
;
islowwer:csect
	mov	a,l		;get character to check
	cpi	'a'		;see if less then 'a'
	jc	islowwer1	;yes...not upper case
	cpi	'z'+1		;is it greater then 'z'
	jnc	islowwer1	;yes...not lower case
	lxi	h,true		;return true for lowwer case
	ret
islowwer1:
	lxi	h,false		;return false for anything but lowwer case
	ret
;
;	isdigit
;
;	function: to check to see if character is digit 0-9 only
;
;	calling format from "c"
;	isdigit(char)
;
isdigit:csect
	mov	a,l		;get character to test
	cpi	'0'		;see if less then a zero
	jc	isdigit1	;yes...not a vaild digit
	cpi	'9'+1		;is it greater than nine
	jnc	isdigit1	;yes...not a vaild digit
	lxi	h,true		;is a vaild digit
	ret
isdigit1:
	lxi	h,false		;not a vaild digit
	ret
;
;	isspace
;
;	function: to check to see if white space tab or blank
;
;	calling format from "c"
;	isspace(char)
;
isspace:csect
	mov	a,l		;get character to check
	lxi	h,true		;assume true
	cpi	' '		;check for a space
	rz
	cpi	9		;check for a tab
	rz
	lxi	h,false
	ret
;
;	strlen
;
;	function: to get the lenght of a string
;
;	calling format in "c"
;	len=strlen(string);
;
strlen:	csect
	xchg			;put address of string in de
	lxi	h,0		;make string of zero lenght
strlen1:
	ldax	d		;get character from string
	ora	a		;check to see if end of string
	rz			;yes return to caller
	inx	h		;add 1 to string lenght
	inx	d		;move to next character
	jmp	strlen1		;loop till end of string found
;
;	strcpy
;
;	function: to copy first second string to first string
;
;	calling format from "c"
;	strcpy(object,source);
;
strcpy:	csect
	pop	b		;get return address from stack
	pop	d		;get source address
	pop	h		;get object address
	push	h		;restore machine stack
	push	d
	push	b
	push	h
strcpy1:
	ldax	d		;get address of source  character
	mov	m,a		;store character in object string
	ora	a		;check to see if end of string
	jz	strcpy2		;end of string return to caller
	inx	h
	inx	d		;move pointer to next byte
	jmp	strcpy1		;loop till done with copy
strcpy2:
	pop	h		;return address of object string
	ret
;
;	strcat
;
;	function: to put string2 at then end of string1 and return address of 
;	      	  string 1
;
;	calling format in "c"
;	strcat(string1,string2);
;
strcat:	csect
	pop	b		;get return address
	pop	d		;get address of string 2
	pop	h		;get address of string 1
	push	h		;retore machine stack
	push	d
	push	b
	push	h		;save address of source string
strcat1:
	mov	a,m		;get character from source buffer
	ora	a		;check to see if zero
	jz	strcat2		;yes end of string found
	inx	h		;move pointer to next byte
	jmp	strcat1		;loop till end of string found
strcat2:
	ldax	d		;get character from string 2
	mov	m,a		;save character in string1
	ora	a		;check to see if end of buffer
	jz	strcat3		;yes return to caller
	inx	h		;move object pointer up 1
	inx	d		;move source pointer up 1
	jmp	strcat2
strcat3:
	pop	h		;get address of string1
	ret
;	strpos
;
;	function: to look for string 2 in string 1
;
;	calling format in "c"
;	strpos(string1,string2);
;
strpos:	csect
	call	argload		;get args load into read be=1 de=2 hl=3
	push	b
	pop	h
	lxi	b,1		;de=string2, hl=string1 bc=0
	xchg
strpos1:
	ldax	d		;get character to check
	ora	a		;check to see if end of string
	jz	strpos5		;end of string1 string 2 not found in string1
	cmp	m		;is it equal to string2 character 1
	jz	strpos2		;yes...
	inx	d		;no add 1 to string 1 pointer
	inx	b		;add 1 to offset pointer
	jmp	strpos1		;loop till end of string1 or string2 found
strpos2:
	push	h		;save strating address of string 21
	push	d		;save current address of string 1
strpos3:
	inx	d
	inx	h
	mov	a,m		;get character from string 2
	ora	a		;set machine flags
	jz	strpos4		;end of string2 found must be a mathc
	ldax	d		;get arg1
	cmp	m		;is this character a match
	jz	strpos3 
	pop	d		;restore pointer for string1 and string 2
	pop	h
	inx	d		;add 1 to string pointer
	inx	b		;add 1 to offset in string1
	jmp	strpos1		;loop till end of string 1
strpos4:
	mov	l,c		;string found return offset for caller
	mov	h,b
	pop	b
	pop	b
	ret
strpos5:
	lxi	h,0		;string not found
	ret
;
;	setmem
;
;	function: to fill a block of memory with a given constant
;
;	calling format in "c"
;	setmem(address,count,constant);
;
setmem:	csect
	pop	b		;get return address
	pop	h		;get constant
	mov	a,l		;put constant in a
	pop	b		;get count
	pop	d		;get address
	lxi	h,8		;offset in to put stack back
	dad	sp
	sphl			;restore stack pointer
	mov	l,a		;place to save constant
setmem1:
	mov	a,b		;check to see if count is zero
	ora	c
	rz			;all done with move
	mov	a,l		;get constant to store
	stax	d		;set memory loaction
	inx	d		;move pointer to next pointer
	dcx	b		;subtract 1 from count
	jmp	setmem1		;loop till all bytes set
;
;	movmem
;
;	function: to move source address to object address for count bytes
;
;	calling format from  "C"
;	movmem(source,object,count);
;
movmem:	csect
	call	argload		;get args bc=1, de=2, hl=3
	push	b		;switch args so that 
	push	d		;bc=count
	push	h		;de=object
	pop	b		;hl=source
	pop	h
	pop	d
	mov	a,b		;check to see if count = 0
	ora	c
	rz			;zero return to caller
	call	movmemht	;check to see if source<dest
	jc	movmemt		;move tial first
	mvi	a,2		;check to see if z80
	inr	a
	jpe	movmemf8	;8080 do a byte by byte move
	dw	0b0edh
	ret
movmemf8:
	mov	a,m		;get source byte
	stax	d		;save byte in object buffer
	inx	h		;move object pointer up 1
	inx	d		;move source pointer up 1 byte
	dcx	b		;subtract 1 from count
	mov	a,b		;see if done
	ora	c
	jnz	movmemf8	;no keep on looping
	ret
movmemt:
	dcx	b		;tail first. compute new source
	dad	b		;and destination address
	xchg
	dad	b
	xchg
	mvi	a,2		;check to see if z80
	inr	a
	jpe	movmemt8	;8080 do a byte by byte move
	dw	0b8edh
	ret
movmemt8:
	mov	a,m		;get source byte
	stax	d		;save object byte
	dcx	h		;move to next byte of source buffer
	dcx	d		;move to next byte of object buffer
	dcx	b		;subtract 1 from count
	mov	a,b		;cehck to see if all done
	ora	c
	jnz	movmemt8	;no keep on looping
	ret
movmemht:
	mov	a,h
	cmp	d
	rnz
	mov	a,l
	cmp	e
	ret
;
;	outp
;
;	function: to output 1 byte to a data port
;
;	calling format from "C"
;	outp(port,data)
;
outp:	csect
	pop	b		;get return address
	pop	h		;get data for port
	pop	d		;get port number
	push	d		;restore machine stack
	push	h
	push	b
	mov	a,e		;get port number
	sta	outp1+1		;save port number
	mov	a,l		;get data to send
outp1:
	out	0		;send data to port
	ret
;
;	inp
;
;	function: to input 1 byte from data port and return to caller
;
;	calling format in "c"
;	data=inp(port);
;
inp:	csect
	mov	a,l		;get port number
	sta	inp1+1		;save port number for input
inp1:
	in	0		;input data from port
	mvi	h,0		;zero high byte of number
	mov	l,a		;put data in reg for "c"
	ret
;	
;	puts
;
;	function: to print a string to console ending with a null
;
;	call format from "C"
;
;	puts(string);
;
puts:	csect
	push	h		;save calling address in stack
puts1:  mov	a,m		;get character to print
	ora	a		;check for zero
	jz	puts2		;if done return
	push	h		;save pointer for later user
	mov	l,a		;put character in reg for push
	call	putch		;print character on console
	pop	h		;get address of character back
	inx	h		;move to next character
	jmp	puts1		;loop till done with string
puts2:
	pop	h		;get address of start of string
	ret			;return to caller
;
;	console i/o functions
;
;	putch
;
;	output 1 character to console and do not check 
;	to see if character is ready at console input
;
;	calling format from "C"
;	putch(c)
;
putch:	csect
	mov	a,l		;get character to print
	cpi	0ah		;check for line feed
	jnz	putch1		;no.....
	mvi	e,0dh		;print return 
	mvi	c,2		;cpm print number function
	call	bdos		;print chararcer to console
	mvi	l,0ah		;print line feed
putch1:
	mov	e,l		;put character in correct reg for call
	mvi	c,2		;cpm output function
	call	bdos		;print character to console
	ret
;
;	function: to get 1 character from cpm console and return
;		  character to caller
;
;	calling format in "C"
;	ch=getch();
;
getch:  csect
	mvi	c,1			;cpm function number for getch
	call	bdos			;get character from console
	cpi	0dh			;check to see if a return
	jnz	getch1  		;no check for a ^z end of file
	mvi	d,0ah			;send a lf to console
	push	c			;put it stack for putch
	call	putch			;print character on console
	lxi	h,10			;and return lf (new line)
	ret
getch1:
	cpi	26			;check to see if end of file
	jnz	getch2  		;not a end of file return char.
	lxi	h,-1			;return end of file maker
	ret				;all done return to caller
getch2:
	mov	l,a			;move character to return reg.
	mvi	h,0			;zero high byte of reutrn
	ret
;
;	gets
;
;	function: to get a string from console and return address
;	          of input string
;
;	calling format from "C"
;	gets(string);
;
gets:	csect
	push	h		;save address of string
	push	h
	mvi	m,250		;save for bdos call
	mvi	c,10		;cpm read string function
	xchg			;put address of string in de for cpm
	call	bdos		;call cpm to read string in
	pop	d		;get address of input string back
	mov	h,d		;put address in hl
	mov	l,e
	inx	d		;move to lenght count
	mov	b,m		;get number of character in string
	inx	d
gets1:
	ldax	d		;get character from input buffer
	mov	m,a		;save character in buffer
	inx	h		;move pointer to next byte
	inx	d	
	dcr	b		;see if all done with move
	jnz	gets1     	;no keep on looping
gets2:
	mvi	m,0		;put ending zero in stringo
	mvi	c,2		;cpm output character function
	mvi	e,0ah		;character to output
	call	bdos		;output a linefeed
	pop	h		;get address of iput string
	ret			;all done return to calleru
;
;	kbstat
;
;	funtion: to get status of cpm console returns
;	true or 1 if console character is ready or
;	0 if character is not ready
;
;	call format for c
;	kbstat()
;
kbstat: csect
	mvi	c,2		;cpm function number for status check
	push	b		;send jump number out
	push	b		;arg
	call	bios		;call bios cons-stat
	pop	b		;remove args from stack
	pop	b
	mov	a,l		;get return status from bios
	ora	a		;set machine status reg.
	lxi	h,0		;make it false (keyboard not ready)
	rz			;return to caller keyboard not ready
	lxi	h,1		;keyboard ready
	ret			;all done return to caller
;
;
;	open
;
;	function to build fcb and try to open file
;	address of fcb in de while name in hl. if file
;	is open ok then 0 is return else -1 is return
;
;	calling format from "C"
;	open(fcb,filename)
;
open:	csect
	pop	b		;get return addess
	pop	h		;get address of file name
	pop	d		;address of fcb
	push	d		;put data back on stack
	push	h
	push	b
	push	d		;put address of fcb in stack
	push	h		;put address of file name in stack
	call	setfcb		;try to scan for a fcb
	pop	d		;remove address of file name from stack
	pop	d		;remove address of fcb from stack
	inr	l		;check to see if vaild return code
	jz	open10		;error invaild file name
	mvi	c,15		;cpm function to open a file
	call	bdos		;execute cpm open function
	inr	a		;see if error opening file
	jz	open10		;error file not found
	lxi	h,0		;set return code to zero
	ret			;return to caller
open10:
	lxi	h,-1		;error return code
	ret			;return to caller
;
;	create
;
;	function to build fcb and delete file then
;	try to create file address of fcb in de 
;	while name in hl. if file is created ok
;	0 is return else -1 is return
;
;	calling format from "C"
;	create(fcb,filename)
;
create:	csect
	pop	b		;get return address
	pop	h		;get address of file name
	pop	d		;get address of fcb
 	push	d		;put data back on stack
	push	h
	push	b
	push	d		;set up arg list for setfcb
	push	h
	call	setfcb		;try toscan and build fcb
	pop	d		;remove arg for setfcb from stack
	pop	d
	inr	l		;see if error in file name
	jz	create10	;error in file name
	push	d		;save address of fcb
	mvi	c,19		;cpm function to delete a file
	call	bdos		;call bdos to do needed function
	pop	d		;restore address of fcb to create
	mvi	c,22		;cpm function to make file
	call	bdos		;call bods to do needed function
	inr	a		;check to see if error
	jz	create10	;error unable to creat file
	lxi	h,0		;every thing ok set return code to zero
	ret			;return to caller
create10:
	lxi	h,-1		;error set return code to -1
	ret
;
;	close
;
;	function to close a file that is current open.
;	address of fcb in de. if able to close file o is
;	is return or -1 if unable to close
;
;	calling format from "C"
;	close(fcb)
;
close:	csect
	pop	b		;return address
	pop	d		;address of fcb
	push	d		;put data back in stack
	push	b
	mvi	c,16		;cpm function to close a file
	call	bdos		;call cpm to do needed function
	inr	a		;check for errors
	jz	close10		;error unable to close file
	lxi	h,0		;every thing is ok set return code to zero
	ret			;return to caller
close10:
	lxi	h,-1		;error set return code to error
	ret
;
;	unlink
;
;	function to build fcb and try to delete
;	delete file. if ok return 0 else -1
;
;	calling format from "C"
;	unlink(filename)
;
unlink:	csect
	pop	b		;get return address
	pop	d		;get address of file name
	push	d		;put data back in stack
	push	b
	lxi	h,-36		;put stack pointer in hl
	dad	sp		;address of stack in hl
	sphl			;set new address of stack pointer
	lxi	b,36		;get address of fcb for delete
	dad	b		;add to get base for fcb
	push	h		;set up arg for setfcb
	push	d
	call	setfcb		;try to build fcb
	pop	d		;remove arg for setfcb
	pop	d
	inr	l		;see if error
	jz	unlink10	;error not a vaild file name
	mvi	c,19		;function number to delete a file
	call	bdos		;call cpm to delete file from system
	inr	a		;check to see if in error
	jz	unlink10	;error unable to delete file
	lxi	d,0		;return code
	jmp	unlink11	;reset stack pointer
unlink10:
	lxi	d,-1		;set error code 
unlink11:
	lxi	h,36		;number of byte to move back
	dad	sp		;add to current stack pointer
	sphl			;put new value in stack pointer
	xchg			;put return code in hl
	ret			;all done return to caller
;
;	read
;
;	function to read x record from disk and return
;	number of record read or -1
;
;	calling format from "C"
;	read(fcb,buffer,count);
;
read:	csect
	lxi	h,2		;offset in stack till count
	dad	sp		;get address of number of record to read
	mov	a,m		;get low byte of count
	mov	e,m		;save for later use
	inx	h		;move to high byte of count
	ora	m		;see if 16 bit count is zero
	mov	d,m		;get high byte of count 
	jz	read1		;yes do not read any more records
	dcx	d		;subtract 1 from count
	mov	m,d		;put high byte back
	dcx	h		;point to high byte
	mov	m,e		;save low byte
	inx	h		;move to buffer address
	inx	h
	mov	e,m		;get address of buffer
	inx	h		;move to high byte of address
	push	h		;save current pointer on stack
	mov	d,m		;get high byte of address
	mvi	c,26		;set dma address
	call	bdos		;call cpm to set buffer address
	pop	h		;remove current pointer from stack
	inx	h		;move to address of fcb
	push	h		;put current pointer back on stack
	mov	e,m		;get low byte of fcb address
	inx	h		;move to high byte of address
	mov	d,m		;get high address of fcb
	mvi	c,20		;read record from dsik
	call	bdos		;call cpm to do it
	pop	h		;get current pointer back
	ora	a		;check for errors
	jnz	read2		;error reading file from disk
	dcx	h		;move to address of buffer
	mov	d,m		;get high byte of address
	dcx	h		;move to low  byte of address
	mov	e,m		;get low byte of address
	xchg			;put address in hl and varble address in hl
	lxi	b,80h		;lenght of 1 record on disk
	dad	b		;add sector lenght to buffer address
	xchg			;put buffer address back in de
	mov	m,e		;save new low byte of address
	inx	h		;move to high byte of address
	mov	m,d		;save high byte of address
	jmp	read		;see it time to read next record
read1:
	lxi	h,0		;every thing is ok return to caller
	jmp	read3		;set dma address to tbuf and return
read2:
	lxi	h,-1		;error reading file
read3:
	push	h		;save return code
	lxi	d,cpmcmd	;address of temp buff
	mvi	c,26		;set dma address function
	call	bdos		;call cpm to do needed funtion
	pop	h		;get return code from stack
	ret			;all done return to caller
;
;	function to write x record from	disk and return
;	false if no errors or true if error
;
;	calling format from "C"
;	write(fcb,buffer,count);
;
write:	csect
	lxi	h,2		;offset in stack till count
	dad	sp		;get address of number of record to read
	mov	a,m		;get low byte of count
	mov	e,m		;save for later use
	inx	h		;move to high byte of count
	ora	m		;see if 16 bit count is zero
	mov	d,m		;get high byte of count 
	jz	write1		;yes do	not write any more records
	dcx	d		;subtract 1 from count
	mov	m,d		;put high byte back
	dcx	h		;point to high byte
	mov	m,e		;save low byte
	inx	h		;move to buffer address
	inx	h
	mov	e,m		;get address of buffer
	inx	h		;move to high byte of address
	push	h		;save current pointer on stack
	mov	d,m		;get high byte of address
	mvi	c,26		;set dma address
	call	bdos		;call cpm to set buffer address
	pop	h		;remove current pointer from stack
	inx	h		;move to address of fcb
	push	h		;put current pointer back on stack
	mov	e,m		;get low byte of fcb address
	inx	h		;move to high byte of address
	mov	d,m		;get high address of fcb
	mvi	c,21		;write record to disk
	call	bdos		;call cpm to do it
	pop	h		;get current pointer back
	ora	a		;check for errors
	jnz	write2		;error writing file from disk
	dcx	h		;move to address of buffer
	mov	d,m		;get high byte of address
	dcx	h		;move to low  byte of address
	mov	e,m		;get low byte of address
	xchg			;put address in hl and varble address in hl
	lxi	b,80h		;lenght of 1 record on disk
	dad	b		;add sector lenght to buffer address
	xchg			;put buffer address back in de
	mov	m,e		;save new low byte of address
	inx	h		;move to high byte of address
	mov	m,d		;save high byte of address
	jmp	write		;see it	time to	write next record
write1:
	lxi	h,0		;every thing is ok return to caller
	jmp	write3		;set dma address to tbuf and return
write2:
	lxi	h,-1		;error writing file
write3:
	push	h		;save return code
	lxi	d,cpmcmd	;address of temp buff
	mvi	c,26		;set dma address function
	call	bdos		;call cpm to do needed funtion
	pop	h		;get return code from stack
	ret			;all done return to caller
;
;	setfcb
;
;	function to build fcb for file name pointed to in hl and
;	address of fcb in de 
;
;	calling format from "C"
;	setfcb(fcb,filename);
;
setfcb:	csect
	pop	b		;get return address
	pop	h		;address of file name
	pop	d		;address of fcb
	push	d		;put data back on stack
	push	h
	push	b
	dcx	h		;set pointer back 1 before file name
setfcb1:
	inx	h		;move pointer to next chracter
	mov	a,m		;get character from buffer
	ora	a		;check for end of buffer
	jz	setfcb6		;error null file name
	cpi	' '		;see if leading space 
	jz	setfcb1		;yes move to next character
	push	h		;save starting address of file name
setfcb1a:
	mov	a,m		;get character from file name
	ora	a		;see if end of string
	jz	setfcb1b	;yes... loop back till not space
	inx	h		;move to next byte
	jmp	setfcb1a
setfcb1b:
	dcx	h		;move back 1 character
	mov	a,m		;get this chracter from buffer
	cpi	' '		;is this character a space
	jz	setfcb1b	;loop till none space character is found
	inx	h		;move to end of string
	mvi	m,0		;move end of string marker
	pop	h		;get starting address back
	inx	h		;move to ":" if disk spec
	mov	a,m		;get this character form file name
	dcx	h		;put pointer back
	cpi	':'		;see if disk spec in file name 
	jnz	setfcb2		;no gt ready for file name
	mov	a,m		;get disk name
	call	setfcb10		;make if upper case
	sui	'@'		;make it 01-16 for disk a-p
	inx	h		;move to start of file name
	inx	h
	jmp	setfcb3		;move disk value into fcb
setfcb2:
	xra	a		;make defualt disk
setfcb3:
	stax	d		;save value for disk drive	
	inx	d		;move fcb to file name
	mvi	b,8		;number of byte in file name
	call	setfcb11	;move file name into place
	jnz	setfcb6		;see if bad character in file name
	cpi	'.'		;chek to see if file type
	jnz	setfcb4		;no do not move pointer past '.'
	inx	h		;move to file type byte
setfcb4:
	mvi	b,3		;number of bytes in file type
	call	setfcb11	;move file type into fcb
	jnz	setfcb6		;see if bad character in file type
	mvi	b,24		;number of bytes left in fcb
	mvi	a,0		;value to file fcb with
setfcb5:
	stax	d		;put 0 in fcb ex-r2
	inx	d		;move to next byte
	dcr	b		;see if done with move
	jnz	setfcb5		;no keep on moveing
	lxi	h,0		;return false if no errors
	ret
setfcb6:
	lxi	h,-1		;return -1 if bad character in file name
	ret
;
;	shift character to upper case
;
setfcb10:
	cpi	061h		;is it less then lowwer case a
	rc			;yes...
	cpi	07ah		;is it greater then lowwer case z
	rnc			;yes
	ani	05fh		;make it loower case
	ret
;
;	scan b characters and move into fcb checking for "?" and
;	bad character in file name
;
setfcb11:
	mov	a,m		;get character form file name
	cpi	'*'		;is it a wild card
	mvi	a,'?'		;yes make it all ???
	jz	setfcb114	;make all ???
setfcb112:
	mov	a,m		;get character to check
	call	setfcb12	;get and check to see if vaild		
	jc	setfcb113	;no....
	stax	d		;save character in fcb
	inx	h		;move both pointers up by 1 bye
	inx	d
	dcr	b		;see if  done with file name
	jnz	setfcb11	;no keep on moveing
	jmp	setfcb115	;done with file look for ending char
setfcb113:
	ora	a		;check to see if end of file name
	jz	setfcb113a	;yes...
	cpi	'.'		;check for delimter
	rnz			;no return and show error
setfcb113a:
	mvi	a,' '		;done with field  pad with blanks
setfcb114:
	stax	d		;fill fcb with value
	inx	d		;move to next byte of fcb
	dcr	b		;see if done with move
	jnz	setfcb114	;no keep on filling
setfcb115:
	mov	a,m		;get character from file name
	ora	a
	rz
	cpi	'.'		;check for '.'
	rz
	inx	h
	jmp	setfcb115	;not ending character keep on looking
;
;	check to see if vaild character for fcb a-z and 0-9 only
;	set carry if not a vaild character otherwise reset carry
;
setfcb12:
	call	setfcb10	;upper case character
	cpi	'?'		;wild card character
	jz	setfcb121	;yes vaild character
	cpi	'$'		;needed for submit files
	jz	setfcb121	;yes valid character
	cpi	'0'		;is it less then '0'
	rc			;yes error invaild character
	cpi	'9'+1		;is it greater then 0 and less then eq 9
	jc	setfcb121	;yes vaild character
	cpi	'A'		;is it less then 'A'
	rc			;yes error between '9' and 'A'
	cpi	'Z'+1		;is it between 'A' and 'Z' 
	cmc			;flop carry bit
	ret
setfcb121:
	stc			;set carry flag
	cmc			;set carry off
	ret
;
;	rsvstk
;
;	function: to set how close to go to machine stack pointer
;
;	calling format in "C"
;	rsvstk(amount);
;
rsvstk:	csect
	shld	@alocmx		;save number of bytes not to used
	ret
;
;	storinit
;
;	function: to init stroage allocation system
;
;	calling format from "c"
;	storinit();
;
storinit:csect
	lxi	h,@base_free	;get address of start of free list
	shld	@base_free	;save as first element
	shld	@allocp		;save as pointer to last block
	lxi	h,0		;get size of free list area
	sta	@base_free+2	;save as current size of free area
	lxi	h,3000		;size of memory not to use
	shld	@alocmx		;save it for later use
	ret
;
;	data area used for free storage mangment
;
@free_list:dsect
@Base_free:	ds	4
@allocp:	ds	2
@alocmx		ds	2
;
;	argload
;
;	function to load bc with arg back 3 de with arg back 2 hl with
;		 1 back
argload:csect
	lxi	h,8		;offset in stack for arg for bc
	dad	sp		;add to stack pointer
	mov	c,m
	inx	h
	mov	b,m
	inx	h
	mov	e,m
	inx	h
	mov	d,m
	inx	h
	mov	a,m
	inx	h
	mov	h,m
	mov	l,a
	ret


inx	h
	mov	h,m
	mov	l,a
	ret


	st