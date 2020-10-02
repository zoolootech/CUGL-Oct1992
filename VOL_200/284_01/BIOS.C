/* bios.c - CP/M BIOS simulator						*/
/* This file depends on system configuration:				*/
/* The version is for Turbo C on MS-DOS.  Phisical disk support routine */
/* calls NEC disk BIOS and cannot run on other hardwares.		*/

#include <stdio.h>
#include <fcntl.h>
#include <dos.h>
#include <conio.h>
#include <io.h>
#include "config.h"
#include "bios.h"

/* This module logically consists of 5 parts: phisical disk I/O, direct */
/* I/O port simulation, normal (simulated) disk I/O, character I/O, and */
/* common initialize routines.	The first 2 parts are optional, and you */
/* can eliminate the codes when they are not necessary.			*/

/* ==================================================================== */
/* PART 1: PHISICAL DISK I/O						*/
/* ==================================================================== */

#if PHISDISK

/* phisical disk I/O functions */

/* disk skew table - not included in BIOS, used on phisical disk only	*/
char skew [ 26 ] = {
     1,	 7, 13, 19,
    25,	 5, 11, 17,
    23,	 3,  9, 15,
    21,	 2,  8, 14,
    20, 26,  6, 12,
    18, 24,  4, 10,
    16, 22
};

char   phisflag;		/* use phisical disk I/O */

#define PDA	0x92		/* phisical device address: #3 of 8" drives */

char phbuffer [ 256 ];		/* phisical I/O buffer */
char * phbuf;			/* top of phisical I/O buffer */

x_phinit ()
{
    struct REGPACK regs;
    unsigned address;

    phbuf = phbuffer;
    address = ( FP_SEG( phbuf ) << 4 ) + FP_OFF( phbuf );
    if ( address > 0xFF80 ) phbuf += 128;

    regs.r_ax = 0x0700 | PDA;
    intr( 0x1B, &regs );
    if ( regs.r_flags & 1 ) return( 1 );

    return( 0 );
}

x_phrd ( ptr, trk, sec )
    char * ptr;
    int trk, sec;
{
    struct REGPACK regs;

    regs.r_ax = 0x1600 | PDA;
    regs.r_bx = 128;
    regs.r_cx = 0x0000 | trk;
    regs.r_dx = 0x0000 | skew[ sec ];
    regs.r_bp = FP_OFF( phbuf );
    regs.r_es = FP_SEG( phbuf );
    intr( 0x1B, &regs );
    memcpy( ptr, phbuf, 128 );
    return( regs.r_flags & 1 );
}

x_phwr ( ptr, trk, sec )
    char * ptr;
    int trk, sec;
{
    struct REGPACK regs;

    memcpy( phbuf, ptr, 128 );
    regs.r_ax = 0x1500 | PDA;
    regs.r_bx = 128;
    regs.r_cx = 0x0000 | trk;
    regs.r_dx = 0x0000 | skew[ sec ];
    regs.r_bp = FP_OFF( phbuf );
    regs.r_es = FP_SEG( phbuf );
    intr( 0x1B, &regs );
    return( regs.r_flags & 1 );
}

#else

x_phinit () { return 1; }
/*ARGSUSED*/ x_phrd ( ptr, trk, sec ) char * ptr; int trk, sec; { return 1; }
/*ARGSUSED*/ x_phwr ( ptr, trk, sec ) char * ptr; int trk, sec; { return 1; }

#endif /* PHISDISK */

/* ==================================================================== */
/* PART 2: DIRECT I/O SIMULATION					*/
/* ==================================================================== */
/* The following lines are used to simulate IN and OUT instruction.	*/
/* You can eliminate the codes by undefining INOUT, or can add them to	*/
/* run programs which access phisical I/O ports, simulating actions of	*/
/* hardware.  If you do, you must supply actual programs modifing func- */
/* tions x_in and x_out also, which just act as your I/O hardware.	*/

#if INOUT
/* table to define effective I/O ports					*/
#define IN		CANINPUT
#define OT		CANOUTPUT
#define IO		(CANINPUT|CANOUTPUT)
char IoMap [ 256 ] = {
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* virtual I/O */

x_in ( a )
    int a;
{
    /* YOU CAN FILL THE CODE HERE */
    return( 0 );
}

/*ARGSUSED*/
x_out ( a, n )
    int a, n;
{
    /* YOU CAN FILL THE CODE HERE */
}

#endif /* INOUT */

/* ==================================================================== */
/* PART 3: NORMAL DISK							*/
/* ==================================================================== */

/* open mode to binary read/write					*/
#define OMODES		( O_RDWR | O_BINARY )

/* get (continuous) secter number					*/
#define SECNUM(t,s)	( ( t - DPB_OFF ) * DPB_SPT + s )

int    disk [ NDISKS ];
char * name [ NDISKS ];

x_rd ( ptr, drv, trk, sec )
    char * ptr;
    int drv, trk, sec;
{
    switch ( disk[ drv ] ) {
    case EMPTY:
	return( 1 );
    case PHIS:
	return( x_phrd( ptr, trk, sec ) );
    default:
	if ( trk < DPB_OFF ) return( 1 );
	lseek( disk[ drv ], ( long )SECNUM( trk, sec ) * SECLEN, 0 );
	return( read( disk[ drv ], ptr, SECLEN ) < 0 );
    }
}

x_wr ( ptr, drv, trk, sec )
    char * ptr;
    int drv, trk, sec;
{
    switch ( disk[ drv ] ) {
    case EMPTY:
	return( 1 );
    case PHIS:
	return( x_phwr( ptr, trk, sec ) );
    default:
	if ( trk < DPB_OFF ) return( 1 );
	lseek( disk[ drv ], ( long )SECNUM( trk, sec ) * SECLEN, 0 );
	return( write( disk[ drv ], ptr, SECLEN ) < SECLEN );
    }
}

/* ==================================================================== */
/* PART 4: CHARACTER I/O						*/
/* ==================================================================== */

/* pseudo BIOS functions */

x_const ()
{
    return( ioctl( 0, 6 ) );
}

x_conin ()
{
    return( getch() );
}

x_conot ( c )
    char c;
{
    putch( c );
}

x_lstot ( c )
    char c;
{
    write( 4, &c, 1 );
}

x_lstst ()
{
    return( ioctl( 4, 7 ) );
}

x_auxin ()
{
    char c;

    if ( read( 3, &c, 1 ) < 1 ) {
	return( 0x1A );
    } else {
	return( c & 0xFF );
    }
}

x_auxot ( c )
    char c;
{
    write( 3, &c, 1 );
}

/* ==================================================================== */
/* PART 5: INITIALIZE ROUTINES						*/
/* ==================================================================== */

int cbrk;			/* old check-break-key mode */
int cimode, comode;		/* old console modes */

/* initialize */
x_init ( argc, argv )
    int argc;
    char * argv [];
{
    int i, d;
    char * p;
    extern char * i_sysdsk();

    name[ 0 ] = i_sysdsk();
    for ( i = 1; i < NDISKS; i++ ) {
	name[ i ] = NULL;
    }

    for ( i = 0; i < argc; i++ ) {
	p = argv[ i ];
	if ( p[ 0 ] == '-' && p[ 1 ] >= 'A' && p[ 1 ] < 'A' + NDISKS ) {
	    name[ p[ 1 ] - 'A' ] = argv[ i + 1 ];
	}
    }

#if PHISDISK
    phisflag = 0;
#endif

    for ( i = 0; i < NDISKS; i++ ) {
	if ( name[ i ] == NULL ) {
	    d = EMPTY;
	} else if ( strcmp( name[ i ], "." ) == 0 ) {
#if PHISDISK
	    if ( phisflag ) {
		fputs( "Only one phisical drive is allowed\n", stderr );
		return( 1 );
	    } else {
		d = PHIS;
		phisflag = 1;
	    }
#else
	    fputs( "phisical drive is not supported\n", stderr );
	    return( 1 );
#endif
	} else {
	    d = open( name[ i ], OMODES );
	    if ( d < 0 ) {
		fputs( "Cannot open ", stderr );
		fputs( name[ i ], stderr );
		fputs( "\n", stderr );
		return( 1 );
	    }
	}
	disk[ i ] = d;
    }

#if PHISDISK
    if ( phisflag ) {
	if ( x_phinit() ) {
	    fputs( "Cannot use phisical drive\n", stderr );
	    return( 1 );
	}
    }
#endif

    setmode( 0, O_BINARY );
    setmode( 1, O_BINARY );
    cimode = ioctl( 0, 0 );
    if ( cimode >= 0 && ( cimode & 0x0080 ) ) {
	ioctl( 0, 1, cimode & 0x00FF | 0x0060 );
    }
    comode = ioctl( 1, 0 );
    if ( comode >= 0 && ( comode & 0x0080 ) ) {
	ioctl( 1, 1, comode & 0x00FF | 0x0020 );
    }
    cbrk = getcbrk();
    setcbrk( 0 );

    return( 0 );
}

/* exit fixup */
x_exit()
{
    int i;

    setcbrk( cbrk );
    if ( comode >= 0 && ( cimode & 0x0080 ) ) ioctl( 1, 1, comode & 0xFF );
    if ( cimode >= 0 && ( comode & 0x0080 ) ) ioctl( 0, 1, cimode & 0xFF );

    for ( i = 0; i < NDISKS; i++ ) {
	if ( disk[ i ] >= 0 ) close( disk[ i ] );
    }
}
