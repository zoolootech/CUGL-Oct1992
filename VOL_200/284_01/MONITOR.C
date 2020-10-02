/* monitor.c - virtual environment manager */

#include <stdio.h>
#include "compile.h"
#include "config.h"
#include "defs.h"
#include "mon.h"

#define X		RegX(*rp)
#define A		RegA(*rp)
#define C		RegC(*rp)
#define BC		RegBC(*rp)
#define DE		RegDE(*rp)
#define HL		RegHL(*rp)

#define Ag(n)		( Mem[ DE + (n) ] )
#define Addr(x)		( ( WORD )( (x) - Mem ) )

#define WRALL		(0)
#define WRDIR		(1)
#define WRUAL		(2)
#define WREAD		(3)

BYTE * dmaptr;

BOOL mflag = 0;		/* memory dump on abort: 0 for no, 1 for yes */
BOOL fflag = 0;		/* alart on 'status flag conflict' */

char * bootfile;	/* boot image file for 8080 */

/* unknown code */
XXX ( p )
    BYTE * p;
{
    fputs( "Unknown opcode: ", stderr );
    fputxx( Addr( p ), stderr );
    fputs( ": ", stderr );
    fputx( *p, stderr );
    fputs( "\n", stderr );
    if ( mflag ) coredump( CORE_SYS );
}

#if INOUT

/* I/O simulation */
i_in ( p, rp )
    BYTE * p;
    REGS * rp;
{
    if ( IoMap[ *p ] & CANINPUT ) {
	A = x_in( *p );
	return( 0 );
    } else {
	fputs( "Unknown IN operation: ", stderr );
	fputxx( Addr(p) - 1, stderr );
	fputs( ": DB ", stderr );
	fputx( *p, stderr );
	fputs( "\n", stderr );
	if ( mflag ) coredump( CORE_SYS );
	return( 1 );
    }
}

i_out ( p, rp )
    BYTE *p;
    REGS * rp;
{
    if ( IoMap[ *p ] & CANOUTPUT ) {
	x_out( *p, A );
    } else {
	fputs( "Unknown OUT operation: ", stderr );
	fputxx( Addr(p) - 1, stderr );
	fputs( ": DB ", stderr );
	fputx( *p, stderr );
	fputs( "\n", stderr );
	if ( mflag ) coredump( CORE_SYS );
    }
}

#endif

/* special escape for virtual BIOS */
esc ( x, rp )
    BYTE * x;
    register REGS * rp;
{
    switch ( *x ) {
    case 0x00: A = 0x00;					break;
    case 0x01: A = i_boot() ? 0xFF : 0x00;			break;
    case 0x02: A = x_const() ? 0xFF : 0x00;			break;
    case 0x03: A = x_conin() & 0x7F;				break;
    case 0x04:	   x_conot( C & 0x7F );				break;
    case 0x05:	   x_lstot( C & 0x7F );				break;
    case 0x06:	   x_auxot( C );				break;
    case 0x07: A = x_auxin();					break;
    case 0x0C:	   dmaptr = &Mem[ BC ];				break;
    case 0x0D: A = x_rd( dmaptr, Ag(0), Ag(1), Ag(2), WREAD );	break;
    case 0x0E: A = x_wr( dmaptr, Ag(0), Ag(1), Ag(2), C );	break;
    case 0x0F: A = x_lstst() ? 0xFF : 0x00;			break;
    default:
	fputs( "Unknown ESC operation: ", stderr );
	fputxx( Addr(x) - 2, stderr );
	fputs( ": ED xx ", stderr );
	fputx( *x, stderr );
	fputs( "\n", stderr );
	if ( mflag ) coredump( CORE_SYS );
	return( 1 );
    }
    return( 0 );
}

/* Boot strap procedure */

i_boot ()
{
    if ( loader( bootfile ) ) {
	fputs( "Cannot load system image: ", stderr );
	fputs( bootfile, stderr );
	fputs( "\n", stderr );
	return( 1 );
    } else {
	return( 0 );
    }
}

/* status flag conflict */
i_flag ()
{
    if ( fflag ) {
	fputs( "Alart: status flag conflict\n", stderr );
    }
}

/* default file name interface */
char * i_sysdsk ()
{
    return( DISK_SYS );
}

/* initialization ( includes virtual IPL procedure ) */
i_init ( argc, argv )
    int argc;
    char * argv [];
{
    int n;

    bootfile = BOOT_SYS;

    for ( n = 1; n < argc; n++ ) {
	if ( argv[ n ][ 0 ] == '-' ) {
	    switch ( argv[ n ][ 1 ] ) {
	    case 'm': mflag = 1; break;
	    case 'f': fflag = 1; break;
	    case 'b': bootfile = argv[ n + 1 ]; break;
	    }
	}
    }

    if ( i_boot() ) return( 1 );

    return( 0 );
}

i_exit ()
{
}
