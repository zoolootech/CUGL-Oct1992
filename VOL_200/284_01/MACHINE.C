/* machine.c - an 8080 machine instruction interpreter			*/

#include <setjmp.h>
#include "compile.h"
#include "config.h"
#include "defs.h"

BYTE Mem [ MEMSIZE ];			/* main memory for our 8080 CPU */

/* macros for 8080 registers						*/

#define B		RegB(regs)
#define C		RegC(regs)
#define D		RegD(regs)
#define E		RegE(regs)
#define H		RegH(regs)
#define L		RegL(regs)
#define A		RegA(regs)
#define M		( Mem[HL] )

#define BC		RegBC(regs)
#define DE		RegDE(regs)
#define HL		RegHL(regs)

#define X		RegX(regs)	/* X acts as a carry flag	*/
#define XA		RegXA(regs)

/* macros for flags							*/

#define CF		( X & 1 )
#define ZF		( res == 0 )
#define SF		( res & 0x80 )

#if PFLAG
#define PF		( parity[res] )
#else
#define PF		XXXXXX()
#endif

#if HFLAG
#define HF		( hf & 0x10 )
#else
#define HF		XXXXXX()
#endif

#define CFF		CF
#define ZFF		( ZF << 6 )
#define SFF		SF

#if PFLAG
#define PFF		PF
#else
#define PFF		(0)
#endif

#if HFLAG
#define HFF		HF
#else
#define HFF		(0)
#endif

/* macros to write operations of each instruction			*/

#define GetB()		( *ppc++ )
#define GetW()		( Lo(tp) = GetB(), Hi(tp) = GetB(), tp )
#define IgnB()		( ppc++ )
#define IgnW()		( ppc+=2 )
#define NxB		( *ppc )
#define NxW		( *ppc | ( *( ppc + 1 ) << 8 ) )
#define Ma(a)		( Mem+(a) )
#define W(x)		( (x)-Mem )

#define MkFF(r)		( res = (r) )
#define SetPSW(psw)	( A=Hi(psw), res=setff( X=Hff(Lo(psw)) ) )
#define ResPSW(psw)	( Hi(psw)=A, Lo(psw)= SFF|ZFF|HFF|PFF|CFF )
#define N(r)		( (r)&0x0F )

/* macros related with an H flag					*/

#if HFLAG
#define HfA(x,y,c)	( hf=N(x)+N(y)+(c) )
#define HfS(x,y,b)	( hf=N(x)-N(y)-(b) )
#define Hf0()		( hf=0 )
#define Hf1()		( hf=0x40 )
#define Hff(x)		( hf=(x) )
#define DAA()		( XA = daa( XA, hf ) )
#else
#define HfA(x,y,c)
#define HfS(x,y,b)
#define Hf0()
#define Hf1()
#define Hff(x)		(x)
#define DAA()		XXXXXX()
#endif

/* macros for some specific instructions				*/

#define DAD(x)		( HL=ex=(EXTRA)HL+(x), X=ex>>16 )

#define Push(x)		( *--psp=Hi(x), *--psp=Lo(x) )
#define Pull(x)		( Lo(x)=(*psp++), Hi(x)=(*psp++) )
#define Jump(a)		( ppc=Ma(a) )
#define Call(a)		( pp=Ma(a), tp=W(ppc), Push(tp), ppc=pp )
#define Return()	( Pull(tp), ppc=Ma(tp) )

#define BrCC(f,op)	if (f) op( GetW() ); else IgnW()
#define RtCC(f)		if (f) Return()

#define nop()					/* nothing to do	*/
#define halt()		longjmp( retpos, 1 )	/* stop executing	*/
#define XXXXXX()	( XXX(--ppc), halt() )	/* illegal OP code	*/

#if ! INOUT
#define i_in(x,y)	XXXXXX()
#define i_out(x,y)	XXXXXX()
#endif

/* macro to execute external procedure					*/
#define SUBMIT(func)	if (func) halt()

/* a table to caliculate parity flag					*/

#if PFLAG
static BYTE parity [ 256 ] = {
    4,0,0,4,0,4,4,0,0,4,4,0,4,0,0,4,
    0,4,4,0,4,0,0,4,4,0,0,4,0,4,4,0,
    0,4,4,0,4,0,0,4,4,0,0,4,0,4,4,0,
    4,0,0,4,0,4,4,0,0,4,4,0,4,0,0,4,
    0,4,4,0,4,0,0,4,4,0,0,4,0,4,4,0,
    4,0,0,4,0,4,4,0,0,4,4,0,4,0,0,4,
    4,0,0,4,0,4,4,0,0,4,4,0,4,0,0,4,
    0,4,4,0,4,0,0,4,4,0,0,4,0,4,4,0,
    0,4,4,0,4,0,0,4,4,0,0,4,0,4,4,0,
    4,0,0,4,0,4,4,0,0,4,4,0,4,0,0,4,
    4,0,0,4,0,4,4,0,0,4,4,0,4,0,0,4,
    0,4,4,0,4,0,0,4,4,0,0,4,0,4,4,0,
    4,0,0,4,0,4,4,0,0,4,4,0,4,0,0,4,
    0,4,4,0,4,0,0,4,4,0,0,4,0,4,4,0,
    0,4,4,0,4,0,0,4,4,0,0,4,0,4,4,0,
    4,0,0,4,0,4,4,0,0,4,4,0,4,0,0,4
};
#endif

/* a special function/macro for POP PSW instruction			*/

#if CKPSW

setff ( ff )
    register int ff;
{
    if ( ff & 0x40 ) {
	/* when Zero flag is on, Sign should be off, and Parity on */
#if PFLAG
	if ( ( ff & 0x84 ) != 0x04 ) {
	    i_flag();		/* alart it and ignore the case */
	}
#else
	if ( ( ff & 0x80 ) != 0x00 ) {
	    i_flag();		/* alart it and ignore the case */
	}
#endif
	return( 0x00 );
    } else {
	/* when Zero flag is off, other two can take any value */
	if ( ff & 0x80 ) {
	    return( ( ff & 0x04 ) | 0x80 );
	} else {
	    return( ( ff & 0x04 ) | 0x20 );
	}
    }
}

#else /* CKPSW */

#define setff(x)	( fftab[x] )

#define szp		0x04
#define szP		0x24
#define sZp		0x00	/* inconsistent */
#define sZP		0x00
#define Szp		0x80
#define SzP		0x84
#define SZp		0x00	/* inconsistent */
#define SZP		0x00	/* inconsistent */

static BYTE fftab [ 256 ] = {

    szp,szp,szp,szp, szP,szP,szP,szP, szp,szp,szp,szp, szP,szP,szP,szP,
    szp,szp,szp,szp, szP,szP,szP,szP, szp,szp,szp,szp, szP,szP,szP,szP,
    szp,szp,szp,szp, szP,szP,szP,szP, szp,szp,szp,szp, szP,szP,szP,szP,
    szp,szp,szp,szp, szP,szP,szP,szP, szp,szp,szp,szp, szP,szP,szP,szP,

    sZp,sZp,sZp,sZp, sZP,sZP,sZP,sZP, sZp,sZp,sZp,sZp, sZP,sZP,sZP,sZP,
    sZp,sZp,sZp,sZp, sZP,sZP,sZP,sZP, sZp,sZp,sZp,sZp, sZP,sZP,sZP,sZP,
    sZp,sZp,sZp,sZp, sZP,sZP,sZP,sZP, sZp,sZp,sZp,sZp, sZP,sZP,sZP,sZP,
    sZp,sZp,sZp,sZp, sZP,sZP,sZP,sZP, sZp,sZp,sZp,sZp, sZP,sZP,sZP,sZP,

    Szp,Szp,Szp,Szp, SzP,SzP,SzP,SzP, Szp,Szp,Szp,Szp, SzP,SzP,SzP,SzP,
    Szp,Szp,Szp,Szp, SzP,SzP,SzP,SzP, Szp,Szp,Szp,Szp, SzP,SzP,SzP,SzP,
    Szp,Szp,Szp,Szp, SzP,SzP,SzP,SzP, Szp,Szp,Szp,Szp, SzP,SzP,SzP,SzP,
    Szp,Szp,Szp,Szp, SzP,SzP,SzP,SzP, Szp,Szp,Szp,Szp, SzP,SzP,SzP,SzP,

    SZp,SZp,SZp,SZp, SZP,SZP,SZP,SZP, SZp,SZp,SZp,SZp, SZP,SZP,SZP,SZP,
    SZp,SZp,SZp,SZp, SZP,SZP,SZP,SZP, SZp,SZp,SZp,SZp, SZP,SZP,SZP,SZP,
    SZp,SZp,SZp,SZp, SZP,SZP,SZP,SZP, SZp,SZp,SZp,SZp, SZP,SZP,SZP,SZP,
    SZp,SZp,SZp,SZp, SZP,SZP,SZP,SZP, SZp,SZp,SZp,SZp, SZP,SZP,SZP,SZP,

};

#endif /* CKPSW */

/* a special function for DAA instruction				*/

#if HFLAG

daa( acc, hf )
    WORD acc;
    BYTE hf;
{
    if ( hf & 0x10 ) {
	if ( ( Hi( acc ) & 1 ) || ( Lo( acc ) & 0xF0 ) > 0x90 ) {
	    return( acc + 0x66 );
	} else {
	    return( acc + 0x06 );
	}
    } else if ( ( Lo( acc ) & 0x0F ) > 0x09 ) {
	if ( ( Hi( acc ) & 1 ) || ( Lo( acc ) & 0xF0 ) > 0x80 ) {
	    return( acc + 0x66 );
	} else {
	    return( acc + 0x06 );
	}
    } else {
	if ( ( Hi( acc ) & 1 ) || ( Lo( acc ) & 0xF0 ) > 0x90 ) {
	    return( acc + 0x60 );
	} else {
	    return( acc );
	}
    }
}

#endif /* HFLAG */

/* main body of the machine - an 8080 machine instruction interpreter	*/

void machine ()
{
    register BYTE * ppc;		/* used like program counter	*/
    register BYTE * psp;		/* used like stack pointer	*/
    REGS regs;				/* 8080 registers		*/
    BYTE res;				/* result of arith./logical op. */
#if HFLAG
    BYTE hf;				/* Half carry Flag		*/
#endif

    WORD tp;
    WORD rp;
    BYTE tb;
    BYTE * pp;
    EXTRA ex;
    jmp_buf retpos;

    if ( setjmp( retpos ) ) return;

    ppc = Ma( 0 );			/* 8080 starts on address 0	*/
    psp = Ma( 0 );

    for (;;) {
#if TRACE
	if ( tflag ) trace( ppc, psp, &regs );
#endif
	switch ( GetB() ) {
/*									*/
/*	OP code	      Mnemonic	   Operation				*/
/* -------------------------------------------------------------------- */
/*									*/
	default:   /* ???	*/ XXXXXX();				break;

	case 0x00: /* NOP	*/ nop();				break;
	case 0x01: /* LXI  B,nn */ C=GetB(), B=GetB();			break;
	case 0x02: /* STAX B	*/ Mem[BC]=A;				break;
	case 0x03: /* INX  B	*/ BC++;				break;
	case 0x04: /* INR  B	*/ HfA(B,1,0); MkFF(++B);		break;
	case 0x05: /* DCR  B	*/ HfS(B,1,0); MkFF(--B);		break;
	case 0x06: /* MVI  B,n	*/ B=GetB();				break;
	case 0x07: /* RLC	*/ X=A, XA<<=1, A=X; Hf0();		break;
/*	case 0x08:		*/
	case 0x09: /* DAD  B	*/ DAD(BC);				break;
	case 0x0A: /* LDAX B	*/ A=Mem[BC];				break;
	case 0x0B: /* DEX  B	*/ BC--;				break;
	case 0x0C: /* INR  C	*/ HfA(C,1,0); MkFF(++C);		break;
	case 0x0D: /* DER  C	*/ HfS(C,1,0); MkFF(--C);		break;
	case 0x0E: /* MVI  C,n	*/ C=GetB();				break;
	case 0x0F: /* RRC	*/ X=A, A=XA>>1; Hf0();			break;

/*	case 0x10:		*/
	case 0x11: /* LXI  D,nn */ E=GetB(), D=GetB();			break;
	case 0x12: /* STAX D,nn */ Mem[DE]=A;				break;
	case 0x13: /* INX  D	*/ DE++;				break;
	case 0x14: /* INR  D	*/ HfA(D,1,0); MkFF(++D);		break;
	case 0x15: /* DCR  D	*/ HfS(D,1,0); MkFF(--D);		break;
	case 0x16: /* MVI  D,n	*/ D=GetB();				break;
	case 0x17: /* RAL	*/ tb=CF, XA<<=1, A|=tb; Hf0();		break;
/*	case 0x18:		*/
	case 0x19: /* DAD  D	*/ DAD(DE);				break;
	case 0x1A: /* LDAX D	*/ A=Mem[DE];				break;
	case 0x1B: /* DEX  D	*/ DE--;				break;
	case 0x1C: /* INR  E	*/ HfA(E,1,0); MkFF(++E);		break;
	case 0x1D: /* DER  E	*/ HfS(E,1,0); MkFF(--E);		break;
	case 0x1E: /* MVI  E,n	*/ E=GetB();				break;
	case 0x1F: /* RRA	*/ tb=A, A=XA>>1, X=tb; Hf0();		break;

/*	case 0x20:		*/
	case 0x21: /* LXI  H,nn */ L=GetB(), H=GetB();			break;
	case 0x22: /* SHLD nn	*/ pp=Ma(GetW()), *pp=L, *(pp+1)=H;	break;
	case 0x23: /* INX  HL	*/ HL++;				break;
	case 0x24: /* INR  H	*/ HfA(H,1,0); MkFF(++H);		break;
	case 0x25: /* DER  H	*/ HfS(H,1,0); MkFF(--H);		break;
	case 0x26: /* MVI  H,n	*/ H=GetB();				break;
	case 0x27: /* DAA	*/ DAA();				break;
/*	case 0x28:		*/
	case 0x29: /* DAD  H	*/ X=H>>7, HL<<=1;			break;
	case 0x2A: /* LHLD nn	*/ pp=Ma(GetW()), L=(*pp), H=(*(pp+1)); break;
	case 0x2B: /* DEX  H	*/ HL--;				break;
	case 0x2C: /* INR  L	*/ HfA(L,1,0); MkFF(++L);		break;
	case 0x2D: /* DER  L	*/ HfS(L,1,0); MkFF(--L);		break;
	case 0x2E: /* MVI  L,n	*/ L=GetB();				break;
	case 0x2F: /* CMA	*/ A^=0xFF; Hf1();			break;

/*	case 0x30:		*/
	case 0x31: /* LXI  SP,nn*/ psp=Ma(GetW());			break;
	case 0x32: /* STA  nn	*/ Mem[GetW()]=A;			break;
	case 0x33: /* INX  SP	*/ psp++;				break;
	case 0x34: /* INR  M	*/ HfA(M,1,0); MkFF(++M);		break;
	case 0x35: /* DER  M	*/ HfS(M,1,0); MkFF(--M);		break;
	case 0x36: /* MVI  M,n	*/ M=GetB();				break;
	case 0x37: /* STC	*/ X=1; Hf0();				break;
/*	case 0x38:		*/
	case 0x39: /* DAD  SP	*/ DAD(psp-Mem);			break;
	case 0x3A: /* LDA  nn	*/ A=Mem[GetW()];			break;
	case 0x3B: /* DEX  SP	*/ psp--;				break;
	case 0x3C: /* INR  A	*/ HfA(A,1,0); MkFF(++A);		break;
	case 0x3D: /* DER  A	*/ HfS(A,1,0); MkFF(--A);		break;
	case 0x3E: /* MVI  A,n	*/ A=GetB();				break;
	case 0x3F: /* CMC	*/ X^=1;				break;

	case 0x40: /* MOV  B,B	*/ nop();				break;
	case 0x41: /* MOV  B,C	*/ B=C;					break;
	case 0x42: /* MOV  B,D	*/ B=D;					break;
	case 0x43: /* MOV  B,E	*/ B=E;					break;
	case 0x44: /* MOV  B,H	*/ B=H;					break;
	case 0x45: /* MOV  B,L	*/ B=L;					break;
	case 0x46: /* MOV  B,M	*/ B=M;					break;
	case 0x47: /* MOV  B,A	*/ B=A;					break;
	case 0x48: /* MOV  C,B	*/ C=B;					break;
	case 0x49: /* MOV  C,C	*/ nop();				break;
	case 0x4A: /* MOV  C,D	*/ C=D;					break;
	case 0x4B: /* MOV  C,E	*/ C=E;					break;
	case 0x4C: /* MOV  C,H	*/ C=H;					break;
	case 0x4D: /* MOV  C,L	*/ C=L;					break;
	case 0x4E: /* MOV  C,M	*/ C=M;					break;
	case 0x4F: /* MOV  C,A	*/ C=A;					break;

	case 0x50: /* MOV  D,B	*/ D=B;					break;
	case 0x51: /* MOV  D,C	*/ D=C;					break;
	case 0x52: /* MOV  D,D	*/ nop();				break;
	case 0x53: /* MOV  D,E	*/ D=E;					break;
	case 0x54: /* MOV  D,H	*/ D=H;					break;
	case 0x55: /* MOV  D,L	*/ D=L;					break;
	case 0x56: /* MOV  D,M	*/ D=M;					break;
	case 0x57: /* MOV  D,A	*/ D=A;					break;
	case 0x58: /* MOV  E,B	*/ E=B;					break;
	case 0x59: /* MOV  E,C	*/ E=C;					break;
	case 0x5A: /* MOV  E,D	*/ E=D;					break;
	case 0x5B: /* MOV  E,E	*/ nop();				break;
	case 0x5C: /* MOV  E,H	*/ E=H;					break;
	case 0x5D: /* MOV  E,L	*/ E=L;					break;
	case 0x5E: /* MOV  E,M	*/ E=M;					break;
	case 0x5F: /* MOV  E,A	*/ E=A;					break;

	case 0x60: /* MOV  H,B	*/ H=B;					break;
	case 0x61: /* MOV  H,C	*/ H=C;					break;
	case 0x62: /* MOV  H,D	*/ H=D;					break;
	case 0x63: /* MOV  H,E	*/ H=E;					break;
	case 0x64: /* MOV  H,H	*/ nop();				break;
	case 0x65: /* MOV  H,L	*/ H=L;					break;
	case 0x66: /* MOV  H,M	*/ H=M;					break;
	case 0x67: /* MOV  H,A	*/ H=A;					break;
	case 0x68: /* MOV  L,B	*/ L=B;					break;
	case 0x69: /* MOV  L,C	*/ L=C;					break;
	case 0x6A: /* MOV  L,D	*/ L=D;					break;
	case 0x6B: /* MOV  L,E	*/ L=E;					break;
	case 0x6C: /* MOV  L,H	*/ L=H;					break;
	case 0x6D: /* MOV  L,L	*/ nop();				break;
	case 0x6E: /* MOV  L,M	*/ L=M;					break;
	case 0x6F: /* MOV  L,A	*/ L=A;					break;

	case 0x70: /* MOV  M,B	*/ M=B;					break;
	case 0x71: /* MOV  M,C	*/ M=C;					break;
	case 0x72: /* MOV  M,D	*/ M=D;					break;
	case 0x73: /* MOV  M,E	*/ M=E;					break;
	case 0x74: /* MOV  M,H	*/ M=H;					break;
	case 0x75: /* MOV  M,L	*/ M=L;					break;
	case 0x76: /* HLT	*/ halt();				break;
	case 0x77: /* MOV  M,A	*/ M=A;					break;
	case 0x78: /* MOV  A,B	*/ A=B;					break;
	case 0x79: /* MOV  A,C	*/ A=C;					break;
	case 0x7A: /* MOV  A,D	*/ A=D;					break;
	case 0x7B: /* MOV  A,E	*/ A=E;					break;
	case 0x7C: /* MOV  A,H	*/ A=H;					break;
	case 0x7D: /* MOV  A,L	*/ A=L;					break;
	case 0x7E: /* MOV  A,M	*/ A=M;					break;
	case 0x7F: /* MOV  A,A	*/ nop();				break;

	case 0x80: /* ADD  B	*/ HfA(A,B,0);	MkFF(XA=A+B);		break;
	case 0x81: /* ADD  C	*/ HfA(A,C,0);	MkFF(XA=A+C);		break;
	case 0x82: /* ADD  D	*/ HfA(A,D,0);	MkFF(XA=A+D);		break;
	case 0x83: /* ADD  E	*/ HfA(A,E,0);	MkFF(XA=A+E);		break;
	case 0x84: /* ADD  H	*/ HfA(A,H,0);	MkFF(XA=A+H);		break;
	case 0x85: /* ADD  L	*/ HfA(A,L,0);	MkFF(XA=A+L);		break;
	case 0x86: /* ADD  M	*/ HfA(A,M,0);	MkFF(XA=A+M);		break;
	case 0x87: /* ADD  A	*/ MkFF(Hff(XA<<=1));			break;
	case 0x88: /* ADC  B	*/ HfA(A,B,CF); MkFF(XA=A+B+CF);	break;
	case 0x89: /* ADC  C	*/ HfA(A,C,CF); MkFF(XA=A+C+CF);	break;
	case 0x8A: /* ADC  D	*/ HfA(A,D,CF); MkFF(XA=A+D+CF);	break;
	case 0x8B: /* ADC  E	*/ HfA(A,E,CF); MkFF(XA=A+E+CF);	break;
	case 0x8C: /* ADC  H	*/ HfA(A,H,CF); MkFF(XA=A+H+CF);	break;
	case 0x8D: /* ADC  L	*/ HfA(A,L,CF); MkFF(XA=A+L+CF);	break;
	case 0x8E: /* ADC  M	*/ HfA(A,M,CF); MkFF(XA=A+M+CF);	break;
	case 0x8F: /* ADC  A	*/ MkFF(Hff(XA=(A<<1)|CF));		break;

	case 0x90: /* SUB  B	*/ HfS(A,B,0);	MkFF(XA=A-B);		break;
	case 0x91: /* SUB  C	*/ HfS(A,C,0);	MkFF(XA=A-C);		break;
	case 0x92: /* SUB  D	*/ HfS(A,D,0);	MkFF(XA=A-D);		break;
	case 0x93: /* SUB  E	*/ HfS(A,E,0);	MkFF(XA=A-E);		break;
	case 0x94: /* SUB  H	*/ HfS(A,H,0);	MkFF(XA=A-H);		break;
	case 0x95: /* SUB  L	*/ HfS(A,L,0);	MkFF(XA=A-L);		break;
	case 0x96: /* SUB  M	*/ HfS(A,M,0);	MkFF(XA=A-M);		break;
	case 0x97: /* SUB  A	*/ MkFF(Hff(XA=0));			break;
	case 0x98: /* SBB  B	*/ HfS(A,B,CF); MkFF(XA=A-B-CF);	break;
	case 0x99: /* SBB  C	*/ HfS(A,C,CF); MkFF(XA=A-C-CF);	break;
	case 0x9A: /* SBB  D	*/ HfS(A,D,CF); MkFF(XA=A-D-CF);	break;
	case 0x9B: /* SBB  E	*/ HfS(A,E,CF); MkFF(XA=A-E-CF);	break;
	case 0x9C: /* SBB  H	*/ HfS(A,H,CF); MkFF(XA=A-H-CF);	break;
	case 0x9D: /* SBB  L	*/ HfS(A,L,CF); MkFF(XA=A-L-CF);	break;
	case 0x9E: /* SBB  M	*/ HfS(A,M,CF); MkFF(XA=A-M-CF);	break;
	case 0x9F: /* SBB  A	*/ MkFF(Hff(XA=(-CF)));			break;

	case 0xA0: /* ANA  B	*/ MkFF(A&=B), X=0; Hf1();		break;
	case 0xA1: /* ANA  C	*/ MkFF(A&=C), X=0; Hf1();		break;
	case 0xA2: /* ANA  D	*/ MkFF(A&=D), X=0; Hf1();		break;
	case 0xA3: /* ANA  E	*/ MkFF(A&=E), X=0; Hf1();		break;
	case 0xA4: /* ANA  H	*/ MkFF(A&=H), X=0; Hf1();		break;
	case 0xA5: /* ANA  L	*/ MkFF(A&=L), X=0; Hf1();		break;
	case 0xA6: /* ANA  M	*/ MkFF(A&=M), X=0; Hf1();		break;
	case 0xA7: /* ANA  A	*/ MkFF(A   ), X=0; Hf1();		break;
	case 0xA8: /* XRA  B	*/ MkFF(A^=B), X=0; Hf0();		break;
	case 0xA9: /* XRA  C	*/ MkFF(A^=C), X=0; Hf0();		break;
	case 0xAA: /* XRA  D	*/ MkFF(A^=D), X=0; Hf0();		break;
	case 0xAB: /* XRA  E	*/ MkFF(A^=E), X=0; Hf0();		break;
	case 0xAC: /* XRA  H	*/ MkFF(A^=H), X=0; Hf0();		break;
	case 0xAD: /* XRA  L	*/ MkFF(A^=L), X=0; Hf0();		break;
	case 0xAE: /* XRA  M	*/ MkFF(A^=M), X=0; Hf0();		break;
	case 0xAF: /* XRA  A	*/ MkFF(A=0 ), X=0; Hf0();		break;

	case 0xB0: /* ORA  B	*/ MkFF(A|=B), X=0; Hf0();		break;
	case 0xB1: /* ORA  C	*/ MkFF(A|=C), X=0; Hf0();		break;
	case 0xB2: /* ORA  D	*/ MkFF(A|=D), X=0; Hf0();		break;
	case 0xB3: /* ORA  E	*/ MkFF(A|=E), X=0; Hf0();		break;
	case 0xB4: /* ORA  H	*/ MkFF(A|=H), X=0; Hf0();		break;
	case 0xB5: /* ORA  L	*/ MkFF(A|=L), X=0; Hf0();		break;
	case 0xB6: /* ORA  M	*/ MkFF(A|=M), X=0; Hf0();		break;
	case 0xB7: /* ORA  A	*/ MkFF(A   ), X=0; Hf0();		break;
	case 0xB8: /* CMP  B	*/ HfS(A,B,0); MkFF(tp=A-B); X=Hi(tp);	break;
	case 0xB9: /* CMP  C	*/ HfS(A,C,0); MkFF(tp=A-C); X=Hi(tp);	break;
	case 0xBA: /* CMP  D	*/ HfS(A,D,0); MkFF(tp=A-D); X=Hi(tp);	break;
	case 0xBB: /* CMP  E	*/ HfS(A,E,0); MkFF(tp=A-E); X=Hi(tp);	break;
	case 0xBC: /* CMP  H	*/ HfS(A,H,0); MkFF(tp=A-H); X=Hi(tp);	break;
	case 0xBD: /* CMP  L	*/ HfS(A,L,0); MkFF(tp=A-L); X=Hi(tp);	break;
	case 0xBE: /* CMP  M	*/ HfS(A,M,0); MkFF(tp=A-M); X=Hi(tp);	break;
	case 0xBF: /* CMP  A	*/ MkFF(Hff(X=0));			break;

	case 0xC0: /* RNZ	*/ RtCC(!ZF);				break;
	case 0xC1: /* POP  B	*/ Pull(BC);				break;
	case 0xC2: /* JNZ  nn	*/ BrCC(!ZF,Jump);			break;
	case 0xC3: /* JMP  nn	*/ Jump(GetW());			break;
	case 0xC4: /* CNZ  nn	*/ BrCC(!ZF,Call);			break;
	case 0xC5: /* PUSH B	*/ Push(BC);				break;
	case 0xC6: /* ADI  n	*/ HfA(A,NxB,0);  MkFF(XA=A+GetB());	break;
	case 0xC7: /* RST  0	*/ Call(0x00);				break;
	case 0xC8: /* RZ	*/ RtCC(ZF);				break;
	case 0xC9: /* RET	*/ Return();				break;
	case 0xCA: /* JZ   nn	*/ BrCC(ZF,Jump);			break;
/*	case 0xCB:		*/
	case 0xCC: /* CZ   nn	*/ BrCC(ZF,Call);			break;
	case 0xCD: /* CALL nn	*/ Call(GetW());			break;
	case 0xCE: /* ACI  n	*/ HfA(A,NxB,CF); MkFF(XA=A+GetB()+CF); break;
	case 0xCF: /* RST  1	*/ Call(0x08);				break;

	case 0xD0: /* RNC	*/ RtCC(!CF);				break;
	case 0xD1: /* POP  D	*/ Pull(DE);				break;
	case 0xD2: /* JNC  nn	*/ BrCC(!CF,Jump);			break;
	case 0xD3: /* OUT  n	*/ SUBMIT(i_out(ppc++,&regs));		break;
	case 0xD4: /* CNC  nn	*/ BrCC(!CF,Call);			break;
	case 0xD5: /* PUSH D	*/ Push(DE);				break;
	case 0xD6: /* SUI  n	*/ HfS(A,NxB,0);  MkFF(XA=A-GetB());	break;
	case 0xD7: /* RST  2	*/ Call(0x10);				break;
	case 0xD8: /* RC	*/ RtCC(CF);				break;
/*	case 0xD9:		*/
	case 0xDA: /* JC   nn	*/ BrCC(CF,Jump);			break;
	case 0xDB: /* IN   n	*/ SUBMIT(i_in(ppc++,&regs));		break;
	case 0xDC: /* CC   nn	*/ BrCC(CF,Call);			break;
/*	case 0xDD:		*/
	case 0xDE: /* SBI  n	*/ HfS(A,NxB,CF); MkFF(XA=A-GetB()-CF); break;
	case 0xDF: /* RST  3	*/ Call(0x18);				break;

	case 0xE0: /* RPO	*/ RtCC(!PF);				break;
	case 0xE1: /* POP  H	*/ Pull(HL);				break;
	case 0xE2: /* JPO  nn	*/ BrCC(!PF,Jump);			break;
	case 0xE3: /* XTHL	*/ tp=HL, Pull(HL), Push(tp);		break;
	case 0xE4: /* CPO  nn	*/ BrCC(!PF,Call);			break;
	case 0xE5: /* PUSH H	*/ Push(HL);				break;
	case 0xE6: /* ANI  n	*/ MkFF(A&=GetB()), X=0; Hf1();		break;
	case 0xE7: /* RST  4	*/ Call(0x20);				break;
	case 0xE8: /* RPE	*/ RtCC(PF);				break;
	case 0xE9: /* PCHL	*/ Jump(HL);				break;
	case 0xEA: /* JPE  nn	*/ BrCC(PF,Jump);			break;
	case 0xEB: /* XCHG	*/ rp=HL, HL=DE, DE=rp;			break;
	case 0xEC: /* CPE  nn	*/ BrCC(PF,Call);			break;
	case 0xED: /* ESC  ?, n */ IgnB(); SUBMIT(esc(ppc++,&regs));	break;
	case 0xEE: /* XRI  n	*/ MkFF(A^=GetB()), X=0; Hf0();		break;
	case 0xEF: /* RST  5	*/ Call(0x28);				break;

	case 0xF0: /* RP	*/ RtCC(!SF);				break;
	case 0xF1: /* POP  PSW	*/ Pull(tp), SetPSW(tp);		break;
	case 0xF2: /* JP   nn	*/ BrCC(!SF,Jump);			break;
	case 0xF3: /* DI	*/ nop();				break;
	case 0xF4: /* CP   nn	*/ BrCC(!SF,Call);			break;
	case 0xF5: /* PUSH PSW	*/ ResPSW(tp), Push(tp);		break;
	case 0xF6: /* ORI  n	*/ MkFF(A|=GetB()), X=0; Hf0();		break;
	case 0xF7: /* RST  6	*/ Call(0x30);				break;
	case 0xF8: /* RM	*/ RtCC(SF);				break;
	case 0xF9: /* SPHL	*/ psp=Ma(HL);				break;
	case 0xFA: /* JM   nn	*/ BrCC(SF,Jump);			break;
	case 0xFB: /* EI	*/ nop();				break;
	case 0xFC: /* CM   nn	*/ BrCC(SF,Call);			break;
/*	case 0xFD:		*/
	case 0xFE: /* CPI  n	*/ HfS(A,NxB,0); MkFF(tp=A-GetB()), X=Hi(tp); break;
	case 0xFF: /* RST  7	*/ Call(0x38);				break;
	}
    }
}
