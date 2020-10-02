/* trace.c - 8080 instruction tracer					*/

#include <stdio.h>
#include "compile.h"
#include "config.h"
#include "defs.h"

#ifndef isdigit
#define isdigit(c)	( (c) >= '0' && (c) <= '9' )
#endif

#define X		RegX(*rp)
#define A		RegA(*rp)
#define C		RegC(*rp)
#define BC		RegBC(*rp)
#define DE		RegDE(*rp)
#define HL		RegHL(*rp)

#define Addr(x)		( ( WORD )( (x) - Mem ) )

BOOL tflag = 0;		/* trace modes: 1 for mnemonic, 2 for registers,  */
			/*		4 for statistics		  */

WORD trlo = 0;		/* lowest  address of trace range */
WORD trhi = MEMSIZE;	/* highest address of trace range */

#if TRACE

long Freq [ 256 ];	/* frequency counter */

/* mnemonic table for dis-assembling */

struct { short args; char * mnem; } OpCode [ 256 ] = {
    /* 0x00 */ { 0, "NOP"      },
    /* 0x01 */ { 2, "LXI  B,"  },
    /* 0x02 */ { 0, "STAX B"   },
    /* 0x03 */ { 0, "INX  B"   },
    /* 0x04 */ { 0, "INR  B"   },
    /* 0x05 */ { 0, "DER  B"   },
    /* 0x06 */ { 1, "MVI  B,"  },
    /* 0x07 */ { 0, "RLC"      },
    /* 0x08 */ { 0, "???"      },
    /* 0x09 */ { 0, "DAD  B"   },
    /* 0x0A */ { 0, "LDAX B"   },
    /* 0x0B */ { 0, "DEX  B"   },
    /* 0x0C */ { 0, "INR  C"   },
    /* 0x0D */ { 0, "DER  C"   },
    /* 0x0E */ { 1, "MVI  C,"  },
    /* 0x0F */ { 0, "RRC"      },
    /* 0x10 */ { 0, "???"      },
    /* 0x11 */ { 2, "LXI  D,"  },
    /* 0x12 */ { 0, "STAX D"   },
    /* 0x13 */ { 0, "INX  D"   },
    /* 0x14 */ { 0, "INR  D"   },
    /* 0x15 */ { 0, "DCR  D"   },
    /* 0x16 */ { 1, "MVI  D,"  },
    /* 0x17 */ { 0, "RAL"      },
    /* 0x18 */ { 0, "???"      },
    /* 0x19 */ { 0, "DAD  D"   },
    /* 0x1A */ { 0, "LDAX D"   },
    /* 0x1B */ { 0, "DEX  D"   },
    /* 0x1C */ { 0, "INR  E"   },
    /* 0x1D */ { 0, "DER  E"   },
    /* 0x1E */ { 1, "MVI  E,"  },
    /* 0x1F */ { 0, "RRA"      },
    /* 0x20 */ { 0, "???"      },
    /* 0x21 */ { 2, "LXI  H,"  },
    /* 0x22 */ { 2, "SHLD "    },
    /* 0x23 */ { 0, "INX  H"   },
    /* 0x24 */ { 0, "INR  H"   },
    /* 0x25 */ { 0, "DER  H"   },
    /* 0x26 */ { 1, "MVI  H,"  },
    /* 0x27 */ { 0, "DAA"      },
    /* 0x28 */ { 0, "???"      },
    /* 0x29 */ { 0, "DAD  H"   },
    /* 0x2A */ { 2, "LHLD "    },
    /* 0x2B */ { 0, "DEX  H"   },
    /* 0x2C */ { 0, "INR  L"   },
    /* 0x2D */ { 0, "DER  L"   },
    /* 0x2E */ { 1, "MVI  L,"  },
    /* 0x2F */ { 0, "CMA"      },
    /* 0x30 */ { 0, "???"      },
    /* 0x31 */ { 2, "LXI  SP," },
    /* 0x32 */ { 2, "STA  "    },
    /* 0x33 */ { 0, "INX  SP"  },
    /* 0x34 */ { 0, "INR  M"   },
    /* 0x35 */ { 0, "DER  M"   },
    /* 0x36 */ { 1, "MVI  M,"  },
    /* 0x37 */ { 0, "STC"      },
    /* 0x38 */ { 0, "???"      },
    /* 0x39 */ { 0, "DAD  SP"  },
    /* 0x3A */ { 2, "LDA  "    },
    /* 0x3B */ { 0, "DEX  SP"  },
    /* 0x3C */ { 0, "INR  A"   },
    /* 0x3D */ { 0, "DER  A"   },
    /* 0x3E */ { 1, "MVI  A,"  },
    /* 0x3F */ { 0, "CMC"      },
    /* 0x40 */ { 0, "MOV  B,B" },
    /* 0x41 */ { 0, "MOV  B,C" },
    /* 0x42 */ { 0, "MOV  B,D" },
    /* 0x43 */ { 0, "MOV  B,E" },
    /* 0x44 */ { 0, "MOV  B,H" },
    /* 0x45 */ { 0, "MOV  B,L" },
    /* 0x46 */ { 0, "MOV  B,M" },
    /* 0x47 */ { 0, "MOV  B,A" },
    /* 0x48 */ { 0, "MOV  C,B" },
    /* 0x49 */ { 0, "MOV  C,C" },
    /* 0x4A */ { 0, "MOV  C,D" },
    /* 0x4B */ { 0, "MOV  C,E" },
    /* 0x4C */ { 0, "MOV  C,H" },
    /* 0x4D */ { 0, "MOV  C,L" },
    /* 0x4E */ { 0, "MOV  C,M" },
    /* 0x4F */ { 0, "MOV  C,A" },
    /* 0x50 */ { 0, "MOV  D,B" },
    /* 0x51 */ { 0, "MOV  D,C" },
    /* 0x52 */ { 0, "MOV  D,D" },
    /* 0x53 */ { 0, "MOV  D,E" },
    /* 0x54 */ { 0, "MOV  D,H" },
    /* 0x55 */ { 0, "MOV  D,L" },
    /* 0x56 */ { 0, "MOV  D,M" },
    /* 0x57 */ { 0, "MOV  D,A" },
    /* 0x58 */ { 0, "MOV  E,B" },
    /* 0x59 */ { 0, "MOV  E,C" },
    /* 0x5A */ { 0, "MOV  E,D" },
    /* 0x5B */ { 0, "MOV  E,E" },
    /* 0x5C */ { 0, "MOV  E,H" },
    /* 0x5D */ { 0, "MOV  E,L" },
    /* 0x5E */ { 0, "MOV  E,M" },
    /* 0x5F */ { 0, "MOV  E,A" },
    /* 0x60 */ { 0, "MOV  H,B" },
    /* 0x61 */ { 0, "MOV  H,C" },
    /* 0x62 */ { 0, "MOV  H,D" },
    /* 0x63 */ { 0, "MOV  H,E" },
    /* 0x64 */ { 0, "MOV  H,H" },
    /* 0x65 */ { 0, "MOV  H,L" },
    /* 0x66 */ { 0, "MOV  H,M" },
    /* 0x67 */ { 0, "MOV  H,A" },
    /* 0x68 */ { 0, "MOV  L,B" },
    /* 0x69 */ { 0, "MOV  L,C" },
    /* 0x6A */ { 0, "MOV  L,D" },
    /* 0x6B */ { 0, "MOV  L,E" },
    /* 0x6C */ { 0, "MOV  L,H" },
    /* 0x6D */ { 0, "MOV  L,L" },
    /* 0x6E */ { 0, "MOV  L,M" },
    /* 0x6F */ { 0, "MOV  L,A" },
    /* 0x70 */ { 0, "MOV  M,B" },
    /* 0x71 */ { 0, "MOV  M,C" },
    /* 0x72 */ { 0, "MOV  M,D" },
    /* 0x73 */ { 0, "MOV  M,E" },
    /* 0x74 */ { 0, "MOV  M,H" },
    /* 0x75 */ { 0, "MOV  M,L" },
    /* 0x76 */ { 0, "HLT"      },
    /* 0x77 */ { 0, "MOV  M,A" },
    /* 0x78 */ { 0, "MOV  A,B" },
    /* 0x79 */ { 0, "MOV  A,C" },
    /* 0x7A */ { 0, "MOV  A,D" },
    /* 0x7B */ { 0, "MOV  A,E" },
    /* 0x7C */ { 0, "MOV  A,H" },
    /* 0x7D */ { 0, "MOV  A,L" },
    /* 0x7E */ { 0, "MOV  A,M" },
    /* 0x7F */ { 0, "MOV  A,A" },
    /* 0x80 */ { 0, "ADD  B"   },
    /* 0x81 */ { 0, "ADD  C"   },
    /* 0x82 */ { 0, "ADD  D"   },
    /* 0x83 */ { 0, "ADD  E"   },
    /* 0x84 */ { 0, "ADD  H"   },
    /* 0x85 */ { 0, "ADD  L"   },
    /* 0x86 */ { 0, "ADD  M"   },
    /* 0x87 */ { 0, "ADD  A"   },
    /* 0x88 */ { 0, "ADC  B"   },
    /* 0x89 */ { 0, "ADC  C"   },
    /* 0x8A */ { 0, "ADC  D"   },
    /* 0x8B */ { 0, "ADC  E"   },
    /* 0x8C */ { 0, "ADC  H"   },
    /* 0x8D */ { 0, "ADC  L"   },
    /* 0x8E */ { 0, "ADC  M"   },
    /* 0x8F */ { 0, "ADC  A"   },
    /* 0x90 */ { 0, "SUB  B"   },
    /* 0x91 */ { 0, "SUB  C"   },
    /* 0x92 */ { 0, "SUB  D"   },
    /* 0x93 */ { 0, "SUB  E"   },
    /* 0x94 */ { 0, "SUB  H"   },
    /* 0x95 */ { 0, "SUB  L"   },
    /* 0x96 */ { 0, "SUB  M"   },
    /* 0x97 */ { 0, "SUB  A"   },
    /* 0x98 */ { 0, "SBB  B"   },
    /* 0x99 */ { 0, "SBB  C"   },
    /* 0x9A */ { 0, "SBB  D"   },
    /* 0x9B */ { 0, "SBB  E"   },
    /* 0x9C */ { 0, "SBB  H"   },
    /* 0x9D */ { 0, "SBB  L"   },
    /* 0x9E */ { 0, "SBB  M"   },
    /* 0x9F */ { 0, "SBB  A"   },
    /* 0xA0 */ { 0, "ANA  B"   },
    /* 0xA1 */ { 0, "ANA  C"   },
    /* 0xA2 */ { 0, "ANA  D"   },
    /* 0xA3 */ { 0, "ANA  E"   },
    /* 0xA4 */ { 0, "ANA  H"   },
    /* 0xA5 */ { 0, "ANA  L"   },
    /* 0xA6 */ { 0, "ANA  M"   },
    /* 0xA7 */ { 0, "ANA  A"   },
    /* 0xA8 */ { 0, "XRA  B"   },
    /* 0xA9 */ { 0, "XRA  C"   },
    /* 0xAA */ { 0, "XRA  D"   },
    /* 0xAB */ { 0, "XRA  E"   },
    /* 0xAC */ { 0, "XRA  H"   },
    /* 0xAD */ { 0, "XRA  L"   },
    /* 0xAE */ { 0, "XRA  M"   },
    /* 0xAF */ { 0, "XRA  A"   },
    /* 0xB0 */ { 0, "ORA  B"   },
    /* 0xB1 */ { 0, "ORA  C"   },
    /* 0xB2 */ { 0, "ORA  D"   },
    /* 0xB3 */ { 0, "ORA  E"   },
    /* 0xB4 */ { 0, "ORA  H"   },
    /* 0xB5 */ { 0, "ORA  L"   },
    /* 0xB6 */ { 0, "ORA  M"   },
    /* 0xB7 */ { 0, "ORA  A"   },
    /* 0xB8 */ { 0, "CMP  B"   },
    /* 0xB9 */ { 0, "CMP  C"   },
    /* 0xBA */ { 0, "CMP  D"   },
    /* 0xBB */ { 0, "CMP  E"   },
    /* 0xBC */ { 0, "CMP  H"   },
    /* 0xBD */ { 0, "CMP  L"   },
    /* 0xBE */ { 0, "CMP  M"   },
    /* 0xBF */ { 0, "CMP  A"   },
    /* 0xC0 */ { 0, "RNZ"      },
    /* 0xC1 */ { 0, "POP  B"   },
    /* 0xC2 */ { 2, "JNZ  "    },
    /* 0xC3 */ { 2, "JMP  "    },
    /* 0xC4 */ { 2, "CNZ  "    },
    /* 0xC5 */ { 0, "PUSH B"   },
    /* 0xC6 */ { 1, "ADI  "    },
    /* 0xC7 */ { 0, "RST  0"   },
    /* 0xC8 */ { 0, "RZ"       },
    /* 0xC9 */ { 0, "RET"      },
    /* 0xCA */ { 2, "JZ	  "    },
    /* 0xCB */ { 0, "???"      },
    /* 0xCC */ { 2, "CZ	  "    },
    /* 0xCD */ { 2, "CALL "    },
    /* 0xCE */ { 1, "ACI  "    },
    /* 0xCF */ { 0, "RST  1"   },
    /* 0xD0 */ { 0, "RNC"      },
    /* 0xD1 */ { 0, "POP  D"   },
    /* 0xD2 */ { 2, "JNC  "    },
    /* 0xD3 */ { 1, "OUT  "    },
    /* 0xD4 */ { 2, "CNC  "    },
    /* 0xD5 */ { 0, "PUSH D"   },
    /* 0xD6 */ { 1, "SUI  "    },
    /* 0xD7 */ { 0, "RST  2"   },
    /* 0xD8 */ { 0, "RC"       },
    /* 0xD9 */ { 0, "???"      },
    /* 0xDA */ { 2, "JC	  "    },
    /* 0xDB */ { 1, "IN	  "    },
    /* 0xDC */ { 2, "CC	  "    },
    /* 0xDD */ { 0, "???"      },
    /* 0xDE */ { 1, "SBI  "    },
    /* 0xDF */ { 0, "RST  3"   },
    /* 0xE0 */ { 0, "RPO"      },
    /* 0xE1 */ { 0, "POP  H"   },
    /* 0xE2 */ { 2, "JPO  "    },
    /* 0xE3 */ { 0, "XTHL"     },
    /* 0xE4 */ { 2, "CPO  "    },
    /* 0xE5 */ { 0, "PUSH H"   },
    /* 0xE6 */ { 1, "ANI  "    },
    /* 0xE7 */ { 0, "RST  4"   },
    /* 0xE8 */ { 0, "RPE"      },
    /* 0xE9 */ { 0, "PCHL"     },
    /* 0xEA */ { 2, "JPE  "    },
    /* 0xEB */ { 0, "XCHG"     },
    /* 0xEC */ { 2, "CPE  "    },
    /* 0xED */ { 2, "ESC  "    },
    /* 0xEE */ { 1, "XRI  "    },
    /* 0xEF */ { 0, "RST  5"   },
    /* 0xF0 */ { 0, "RP"       },
    /* 0xF1 */ { 0, "POP  PSW" },
    /* 0xF2 */ { 2, "JP	  "    },
    /* 0xF3 */ { 0, "DI"       },
    /* 0xF4 */ { 1, "CP	  "    },
    /* 0xF5 */ { 0, "PUSH PSW" },
    /* 0xF6 */ { 1, "ORI  "    },
    /* 0xF7 */ { 0, "RST  6"   },
    /* 0xF8 */ { 0, "RM"       },
    /* 0xF9 */ { 0, "SPHL"     },
    /* 0xFA */ { 2, "JM	  "    },
    /* 0xFB */ { 0, "EI"       },
    /* 0xFC */ { 2, "CM	  "    },
    /* 0xFD */ { 0, "???"      },
    /* 0xFE */ { 1, "CPI  "    },
    /* 0xFF */ { 0, "RST  7"   }
};

char Carry[ 2 ][ 4 ] = { "NC", "Cy" };
char Sp [ 14 + 1 ] = "		    ";
short OpLen [ 256 ];

trace ( p, q, rp )
    register BYTE * p;
    BYTE * q;
    REGS * rp;
{
    register FILE * fp = stdout;

    if ( Addr(p) < trlo || Addr(p) >= trhi ) return;

    if ( tflag & 0x01 ) {
	fputxx( Addr(p), fp ); putc( ':', fp ); putc( ' ', fp );
	fputs( OpCode[ *p ].mnem, fp );
	switch ( OpCode[ *p ].args ) {
	    case 2: fputx( *( p + 2 ), fp );
	    case 1: fputx( *( p + 1 ), fp );
	    case 0: break;
	}
    }

    if ( ( tflag & 0x03 ) == 3 ) fputs( Sp + OpLen[ *p ], fp );

    if ( tflag & 0x02 ) {
	fputs( "[ A=", fp ); fputx( A, fp );
	fputs( " BC=", fp ); fputxx( BC, fp );
	fputs( " DE=", fp ); fputxx( DE, fp );
	fputs( " HL=", fp ); fputxx( HL, fp );
	fputs( " SP=", fp ); fputxx( Addr(q), fp );
	fputs( " M=", fp );
	if ( HL >= 0 && HL < MEMSIZE ) {
	    fputx( Mem[ HL ], fp );
	} else {
	    fputs( "??", fp );
	}
	putc( ' ', fp );
	fputs( Carry[ X & 1 ], fp );
	fputs( " ]", fp );
    }

    if ( tflag & 0x03 ) {
	putc( '\n', fp );
	fflush( fp );
    }

    if ( tflag & 0x04 ) {
	Freq[ *p ]++;
    }
}

trace_init ( argc, argv )
    int argc;
    char * argv [];
{
    register int i;

    for ( i = 0; i < 256; i++ ) {
	OpLen[ i ] = OpCode[ i ].args * 2 + strlen( OpCode[ i ].mnem );
    }

    for ( i = 1; i < argc; i++ ) {
	if ( strcmp( argv[ i ], "-t" ) == 0 ) {
	    tflag = atoi( argv[ i + 1 ] );
	}
    }

    return( 0 );
}

fputl ( x, fp )
    register long x;
    FILE * fp;
{
    char line [ 16 ];
    register char * u;

    for ( u = line; u < line + 16 - 1; *u++ = ' ' ) ;
    *u = '\0';
    u = line + 13;
    while ( x > 0 ) {
	*u-- = x % 10 + '0';
	x /= 10;
    }
    fputs( line, fp );
}

trace_exit ()
{
    register long * p, * q;
    long total;

    if ( tflag & 0x04 ) {
	total = 0L;
	for ( p = Freq; p < Freq + 256; p++ ) total += *p;
	fputl( total, stdout );
	fputs( "total\n", stdout );
	for (;;) {
	    q = Freq;
	    for ( p = Freq; p < Freq + 256; p++ ) {
		if ( *p > *q ) q = p;
	    }
	    if ( *q <= 0 ) break;
	    fputl( *q, stdout );
	    fputs( OpCode[ q - Freq ].mnem, stdout );
	    switch ( OpCode[ q - Freq ].args ) {
	    case 2: fputs( "nn", stdout );
	    case 1: fputs( "nn", stdout );
	    case 0: fputs( "\n", stdout );
	    }
	    *q = 0L;
	}
    }
}

#else /* TRACE */

/*ARGSUSED*/
trace_init ( argc, argv ) int argc; char * argv []; { return( 0 ); }
trace_exit () {}

#endif /* TRACE */
