/*	file fmdx.c
	"float multiply/divide by X"
	implemented here with X = 10	*/
#asm
	NAME	('FMDX')
	INCLUDE	DEQ.MAC
#endasm

float	fdten(fexp)	/* float divide by ten */
float	fexp;
{
#asm
	.Z80
	LDR.FV	FE10
	EQUJPS	.FDIV,ST.MDX
	.8080
#endasm
}

float	fmten(fexp)	/* float multiply by ten */
float	fexp;
{
#asm
	.Z80
	LDR.FV	FE10
	EQUJPS	.FMUL,ST.MDX
	.8080
#endasm
}

#asm
	EXTRN	CMDBYT
	.Z80
ST.MDX:	EQUJPS	,FLSD.U##,QUIK.1##
	.8080
#endasm
