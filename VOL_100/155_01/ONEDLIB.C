/* HEADER: xxx.yy;
   TITLE: ONED;
   DATE: 8/18/83;
   DESCRIPTION: "UNIX like library functions called by oned
      fgets -- Read one line.
      setjmp -- Save registers for later restoration by longjmp.
      longjmp -- Transfer execution to previously called setjmp.
      Coded in 8080 assembly language for use with C/80 compiler.";
   KEYWORDS: UNIX library;
   FILENAME: ONEDLIB.C;
   CRC: xxxx;
   SYSTEM: CP/M;
   COMPILERS: C/80;
   AUTHORS: David L. Fox;
   REFERENCES: AUTHORS: Edward K. Ream;
         TITLE: "RED--A Better C Screen Editor, Part II";
         CITATION: "Doctor Dobb's Journal 82, 89(August 1983).";
      ENDREF
*/
/* Library functions needed to simulate a UNIX like environment. */

/* fgets(s, n, fdesc) reads n-1 characters, or up to a new-line
   (which is retained), whichever comes first, from the file
   specified by fdesc into s.  s is null terminated.  Fgets returns
   its first argument.  Returns NULL on EOF. */

char *
fgets(s, n, fdesc)
char *s;
int n, fdesc;
{
/*	char *t;
/*	int i, c;
/*	
/*	t=s;
/*	for (i = 0; (c = getc(fdesc)) != '\n'; ++i, ++s)
/*	{	if (c == EOF) return(NULL);
/*		*s = c;
/*		if (i == n-1)
/*		{
/*			*s = '\0';	/* need to ungetc(c); here */
/*			return(t);
/*		}
/*	}
/*	*s = c;
/*	*++s = '\0';
/*	return(t);
*/
#asm
; Assembly language version 18 Aug 83
	LXI	H,2
	DAD	SP
	MOV	C,M
	INX	H
	MOV	B,M	; fdesc into b,c
	INX	H
	MOV	E,M
	INX	H
	MOV	D,M	; n into d,e
	DCX	D
	INX	H
	MOV	A,M
	INX	H
	MOV	H,M
	MOV	L,A	; s into h,l
	PUSH	H
	PUSH	H
fg.lp:	MOV	A,E
	ORA	D
	JZ	fg.d1	; return if n == 0
	PUSH	D
	PUSH	B
	CALL	getc
	POP	B
	MOV	A,H
	ANA	L
	CMA
	ORA	A
	JZ	fg.eof	; return on EOF
	MOV	A,L	; char into a
	POP	D
	POP	H
	MOV	M,A
	INX	H
	PUSH	H	; save new s
	CPI	10
	JZ	fg.d1	; check for \n
	DCX	D	; decrement n
	JMP	fg.lp
fg.eof:	POP	B
	POP	H
	XRA	A
	MOV	M,A
	POP	B
	MOV	H,A
	MOV	L,A	; return 0
	RET
fg.d1:	POP	H
	XRA	A
	MOV	M,A	; terminate string
	POP	H
			; compiler adds return
#endasm
}

setjmp(env)
char *env;
{
#asm
; Version: 7 Aug 83
;
; Adapted from deffa.csm  BDS C version 1.46, 3/22/82
; Original by Leor Zolman
; Modified by E. K. Ream
; Published Aug 83 DDJ
; Modified by DLF for C/80
;
; setjmp(env)
; env is 6 byte environment buffer
; returns 0 when called, non-zero for return from longjmp
;
	LXI	H,2		; get arg into h,l
	DAD	SP
	CALL	h.
	MOV	M,E		; save d,e
	INX	H
	MOV	M,D
	INX	H
	XCHG		; save sp
	LXI	H,0
	DAD	SP
	XCHG
	MOV	M,E
	INX	H
	MOV	M,D
	INX	H
	POP	D		; save return address
	PUSH	D
	MOV	M,E
	INX	H
	MOV	M,D
	LXI	H,0		; return 0
#endasm
}

longjmp(buffer, return_value)
char *buffer;
int return_value;
{
#asm
;
;	unlike the BDS C version of this routine,
;	the return_value is REQUIRED
;
	LXI	H,4		; get first arg
	DAD	SP
	CALL	h.
	MOV	C,M		; restore d,e, put it in b,c
	INX	H
	MOV	B,M
	INX	H
	MOV	E,M		; restore sp, put it in de
	INX	H
	MOV	D,M
	INX	H
	SHLD	temp_lj	; save pointer to return address
	LXI	H,2		; get second arg
	DAD	SP
	CALL	h.
	XCHG		; old sp into h,l second arg in d,e
	SPHL		; restore old sp
	POP	H		; pop return addr off stack
	LHLD	temp_lj	; get ptr to ra
	CALL	h.		; new ret. addr. into h,l
	XCHG		; ra in d,e ret val in h,l
	PUSH	D
	MOV	D,B	; copy bc to de
	MOV	E,C
	RET
temp_lj: DS	2
#endasm
}
