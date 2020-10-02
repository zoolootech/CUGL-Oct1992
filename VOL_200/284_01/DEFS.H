/* defs.h - common definitions						*/

#if LITTLE_INDIAN

typedef union {
    struct { WORD bc,  de,  hl,	 xa;  } p;
    struct { BYTE c,b, e,d, l,h, a,x; } q;
} REGS;

#define Hi(x)		( *(( BYTE * )&(x)+1) )
#define Lo(x)		( *(( BYTE * )&(x)+0) )

#endif

#if BIG_INDIAN

typedef union {
    struct { WORD bc,  de,  hl,	 xa; } p;
    struct { BYTE b,c, d,e, h,l, x,a; } q;
} REGS;

#define Hi(x)		( *(( BYTE * )&(x)+0) )
#define Lo(x)		( *(( BYTE * )&(x)+1) )

#endif

#define RegBC(s)	( (s).p.bc )
#define RegDE(s)	( (s).p.de )
#define RegHL(s)	( (s).p.hl )
#define RegXA(s)	( (s).p.xa )
#define RegB(s)		( (s).q.b )
#define RegC(s)		( (s).q.c )
#define RegD(s)		( (s).q.d )
#define RegE(s)		( (s).q.e )
#define RegH(s)		( (s).q.h )
#define RegL(s)		( (s).q.l )
#define RegX(s)		( (s).q.x )
#define RegA(s)		( *( BYTE * )&(s).q.a )
	/* above strange cast in RegA is for a BUG of MS-C ver 3.00.0	*/

#define HiR(r)		( (r)>>8 )
#define LoR(r)		( (r)&0x0FF )

/* Main memory of 8080							*/
extern BYTE Mem [ MEMSIZE ];

/* Trace flag; refered outside trace.c for speed			*/
extern BOOL tflag;

/* virtual I/O port mapping						*/
#if INOUT
extern char IoMap [ 256 ];
#define CANINPUT	1
#define CANOUTPUT	2
#endif
