/*	file ftrig.c	*/
#asm
	NAME	('FTRIG')
	INCLUDE DEQ.MAC
	EXTRN	ST.FLU,CMDBYT,PORTD,CMDIO,QUIK.2
#endasm

float	sin(fexp)	/* return sine of fexp */
float	fexp;
{
#asm
	.Z80
	EQUJPS	.SIN,ST.FLU
	.8080
#endasm
}

float	cos(fexp)	/* return cosine of fexp */
float	fexp;
{
#asm
	.Z80
	EQUJPS	.COS,ST.FLU
	.8080
#endasm
}

float	tan(fexp)	/* return tangent of fexp */
float	fexp;
{
#asm
	.Z80
	EQUJPS	.TAN,ST.FLU
	.8080
#endasm
}

float	asin(fexp)	/* return arc sine of fexp */
float	fexp;
{
#asm
	.Z80
	EQUJPS	.ASIN,ST.FLU
	.8080
#endasm
}

float	acos(fexp)	/* return arc cosine of fexp */
float	fexp;
{
#asm
	.Z80
	EQUJPS	.ACOS,ST.FLU
	.8080
#endasm
}

float	atan(fexp)	/* return arc tangent of fexp */
float	fexp;
{
#asm
	.Z80
	EQUJPS	.ATAN,ST.FLU
	.8080
#endasm
}

float	fdpi(fexp)	/* divide float expression by pi constant */
float	fexp;
{
#asm
	.Z80
	LD	IX,FDIV.X
FMD.PI: LD	HL,2
	ADD	HL,SP
	LD	BC,(PORTD)
	LD	B,4
	OTIR
	LD	A,PXF.PI
	LD	(CMDBYT),A
	JP	CMDIO

FDIV.X: LD	A,.FDIV 	; float divide, then exit
	LD	(CMDBYT),A
	JP	QUIK.2
	.8080
#endasm
}

float	fmpi(fexp)	/* multiply float expression by pi constant */
float	fexp;
{
#asm
	.Z80
	LD	IX,FMUL.X
	JP	FMD.PI

FMUL.X: EQUJPS	.FMUL,QUIK.2	; float multiply, then exit
	.8080
#endasm
}



















