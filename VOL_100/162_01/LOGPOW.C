/*	file logpow.c */
/*	release 1.2, Feburary 1986	*/
#asm
	NAME	('LOGPOW')
	INCLUDE DEQ.MAC
	EXTRN	CMDIO,STATUS,CMDBYT,ST.FLU,FLRD,FLDR,QUIK.2,FLSD.U
	EXTRN	ST.FL2,FLSD.B
#endasm


float	log(fexp)	/* return common log */
float	fexp;
{
#asm
	.Z80
	EQUJPS	.LOG,ST.FLU
	.8080
#endasm
}

float	ln(fexp)	/* return natural log */
float	fexp;
{
#asm
	.Z80
	EQUJPS	.LN,ST.FLU
	.8080
#endasm
}

float	exp(fexp)	/* return e ^ fexp  */
float	fexp;
{
#asm
	.Z80
	EQUJPS	.EXP,ST.FLU
	.8080
#endasm
}

float	pow(f1,f2)	/* f1 ^ f2 */
float f1,f2;
{
#asm
	.Z80
	EQUJPS	.PWR,ST.FL2
	.8080
#endasm
}

float	pow10(fexp)	/* returns ten ^ fexp  */
float	fexp;
{
#asm
	.Z80
	LDR.FV	FE10
	EQUJPS	.PWR,FLRD,ST.FLU	; 2nd fix, Feb 86
	.8080
#endasm
}

float	invrt(fexp)	/* returns 1/fexp */
float	fexp;
{
#asm
	.Z80
	LDR.FV	FE1
	EQUJPS	.FDIV,FLRD,ST.FLU
	.8080
#endasm
}

float	fabs(fexp)	/* returns absolute value of fexp */
float	fexp;
{
#asm
	.Z80
	EQUJPS	PXF,FLSD.U,CMDIO,YFABS
YFABS:	LD	A,(STATUS)
	BIT	6,A
	JR	NZ,NFABS
	JP	FLDR

NFABS:	LD	A,NXF
	LD	(CMDBYT),A
	JP	QUIK.2

	.8080
#endasm
}

float	sqrt(fexp)	/* returns square root of fexp */
float	fexp;
{
#asm
	.Z80
	EQUJPS	.SQRT,ST.FLU
	.8080
#endasm
}
�!T"�͉0�*�  ́���!�!��&>��h��L���!  �d͟>�32���L�����d  !  "�!�"��&>�|���/A"�!�"�͉0�*|��5*�|��5!�!�ͨ<��