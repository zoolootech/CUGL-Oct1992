/* Screen editor:  operating system module
 *
 * Source:  ed8.c
 * Version: June 19, 1981.
 */

/* all calls to the operating system are made here.
 * only this module and the assembler libraries will have to be
 * rewritten for a new operating system.
 */

/* the routines syscstat(), syscin() and syscout() come in
 * two flavors:  CP/M version 2.2 and CP/M version 1.4.
 * Comment out which ever you don't use.
 */

/* CP/M 2.2 versions of syscstat(), syscin(), syscout() */

/* return -1 if no character is ready from the console. 
 * otherwise, return the character.
 */

syscstat()
{
	return(cpm(6,-1));
}

/* wait for next character from the console.
 * do not echo it.
 */

syscin()
{
int c;
	while ((c=cpm(6,-1))==0) {
		;
	}
	return(c);
}

/* print character on the console */

syscout(c) char c;
{
	cpm(6,c);
	return(c);
}

/* CP/M 1.4 versions of syscstat(), syscin(), syscout() */

/* start comment out ----------
#asm
;
;	bios(n,bc)
;
;	call the n'th bios routine.
;	bc is put into the bc-register as a parameter.
;	returns whatever bios puts into the a-reg.
;
QZBIOS:
	POP	H	;get return address
	POP	B	;    bc
	POP	D	;    n
	PUSH	D	;restore stack the way it was
	PUSH	B
	PUSH	H
	LHLD	1	;point hl at start of BIOS jump table
	DCX	H
	DCX	H
	DCX	H
	MOV	A,E	;put index into jump table into de
	ADD	A
	ADD	E
	MVI	D,0
	MOV	E,A
	DAD	D	;point hl at proper entry in jump table
	PUSH	H	;push call address
	LXI	H,QZBIOS1 ;point hl at return address
	XTHL		;push return address, get call address
	PCHL		;call proper BIOS routine
QZBIOS1:
	MOV	L,A 	;put return code from BIOS into hl
	MVI	H,0
	RET		;go back to small-c
#endasm

syscstat()
{
	if (bios(2,0)==255) {
		return(-1);
	}
	else {
		return(0);
	}
}

syscin()
{
	return(bios(3,0));
}

syscout(c) char c;
{
	bios(4,c);
	return(c);
}
---------- end comment out */


/* print character on the printer */

syslout(c) char c;
{
	cpm(5,c);
	return(c);
}

#asm
;
; sysend()
;
; return address of last usable memory location.
;
QZSYSEND:
	LHLD	6	;get address of BDOS-1
	DCX	H	;
	RET		;return
#endasm

/* open a file */

sysopen(name,mode) char *name, *mode;
{
int file;
	if ((file=fopen(name,mode))==0) {
		return(ERR);
	}
	else {
		return(file);
	}
}

/* close a file */

sysclose(file) int file;
{
	/* fclose doesn't reliably return OK */
	fclose(file);
	return(OK);
}

/* read next char from file */

sysrdch(file) int file;
{
int c;
	if ((c=getc(file))==-1) {
		return(EOF);
	}
	else {
		return(c);
	}
}

/* write next char to file */

syspshch(c,file) char c; int file;
{
	if (putc(c,file)==-1) {
		error("disk write failed");
		return(ERR);
	}
	else {
		return(c);
	}
}

/* read one char from END of file */

syspopch(file) int file;
{
	error("syspopch() not implemented");
	return(ERR);
}

/* check file name for syntax */

syschkfn(args) char *args;
{
	return(OK);
}

/* copy file name from args to buffer */

syscopfn(args,buffer) char *args, *buffer;
{
int n;
	n=0;
	while (n< SYSFNMAX-1) {
		if (args[n]==EOS) {
			break;
		}
		else {
			buffer[n]=args[n];
			n++;
		}
	}
	buffer[n]=EOS;
}

/* move a block of n bytes down (towards HIGH addresses).
 * block starts a source and the first byte goes to dest.
 * this routine is only called from bufmovdn() as follows
 *	sysmovdn( n=to-from+1, dest=to+length, source=to);
 */

#asm
QZSYSMOVDN:
	POP	H	;get return address
	POP	B	;BC = source
	POP	D	;DE = dest
	XTHL		;HL = n, restore return address
	JMP	SYSMOVDN2	;go enter loop
;
; this code is 15 times faster than the c-code it replaces.
SYSMOVDN1:
	LDAX	B	;*dest-- = *source--
	STAX	D
	DCX	B
	DCX	D
	DCX	H	;while ((n--)>0)
SYSMOVDN2:
	MOV	A,L
	ORA	H
	JNZ	SYSMOVDN1
;
	XTHL		;HL = return address
	PUSH	H	;restore stack
	PUSH	H
	PUSH	H
	RET		;return
#endasm

/* move a block of n bytes up (towards LOW addresses).
 * the block starts at source and the first byte goes to dest.
 * this routine is called only from bufmovup() as follows:
 * 	sysmovup( n=to-from+1, dest=from-length, source=from);
 */

#asm
QZSYSMOVUP:
	POP	H	;get return address
	POP	B	;BC = source
	POP	D	;DE = dest
	XTHL		;HL = n, save return address
	JMP	SYSMOVUP2	;go enter loop
;
; this code is 15 times faster than the c-code it replaces.
SYSMOVUP1:
	LDAX	B	;*dest++ = *source++
	STAX	D
	INX	B
	INX	D
	DCX	H	;while ((n--)>0)
SYSMOVUP2:
	MOV	A,L
	ORA	H
	JNZ	SYSMOVUP1
;
	XTHL		;HL = return address
	PUSH	H	;restore stack
	PUSH	H
	PUSH	H
	RET		;return
#endasm

	;go enter loop
;
; this code is 15 times faster than the c-code it                                                                                                                                