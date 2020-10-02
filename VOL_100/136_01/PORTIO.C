/*	HEADER:  CUG136.04;
	TITLE:		PORTIO;
	DATE:		1/27/1984;
	DESCRIPTION:	"I/O Port routines for C/80";
	KEYWORDS:	I/O subroutines, Port I/O;
	SYSTEM:		CP/M;
	FILENAME:	PORTIO.C;
	AUTHORS:	R. Rodman;
	COMPILERS:	C/80;
*/

/* port i/o routines for c/80 v3.0
	840127 rr orig file */

#ifneed portout,PORTOUT

/* portout - send value to io port */

portout( p, v ) {
#asm
	LXI H,4
	DAD SP
	MOV A,M		;point to first argument
	STA OPORT
	DCX H
	DCX H		;get second argument
	MOV A,M
	OUT 0
OPORT	EQU $-1
#endasm
}
#endif

#ifneed portin,PORTIN

/* portin - read input port */

portin( p ) {
#asm
	LXI H,2
	DAD SP
	MOV A,M		;get port number
	STA IPORT
	IN 0
IPORT	EQU $-1
	LXI H,0
	MOV L,A		;return value
#endasm
}
#endif


