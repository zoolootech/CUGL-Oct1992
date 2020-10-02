/* util.c - general utility routines for monitor */

#include <stdio.h>
#include "compile.h"
#include "config.h"
#include "defs.h"

#ifndef isdigit
#define isdigit(c)	( (c) >= '0' && (c) <= '9' )
#endif

BYTE sum;

static char hexch [] = {
    '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'
};

/* general utilities */

fputx ( b, fp )
    BYTE b;
    register FILE * fp;
{
    putc( hexch[ b >> 4	  ], fp );
    putc( hexch[ b & 0x0F ], fp );
}

fputxx ( w, fp )
    WORD w;
    register FILE * fp;
{
    putc( hexch[ Hi( w ) >> 4	], fp );
    putc( hexch[ Hi( w ) & 0x0F ], fp );
    putc( hexch[ Lo( w ) >> 4	], fp );
    putc( hexch[ Lo( w ) & 0x0F ], fp );
}

/* Intel HEX format loader ( for IPL and warm boot ) */

static BYTE hex [ 257 ] = {
    0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0,0,0,0,0,0,
    0, 10, 11, 12, 13, 14, 15
};

gethex ( fp )
    register FILE * fp;
{
    BYTE x;

    x =	 hex[ getc( fp ) + 1 ] << 4;
    x |= hex[ getc( fp ) + 1 ];
    sum += x;
    return( x );
}

loader ( fid )
    char * fid;
{
    register BYTE * p;
    register FILE * fp;
    register BYTE x;
    int c, n;
    WORD ad;

    fp = fopen( fid, "r" );
    if ( fp == ( FILE * )NULL ) return( 1 );

    for (;;) {
	while ( ( c = getc( fp ) ) != EOF && c != ':' ) ;
	if ( c == EOF ) break;
	sum = 0;
	if ( ( n = gethex( fp ) ) == 0 ) break;
	ad = gethex( fp ) << 8,
	ad |= gethex( fp );
	if ( gethex( fp ) != 0 ) {
	    fclose( fp );
	    return( 2 );
	}
	p = &Mem[ ad ];
	while ( --n >= 0 ) {
	    *p++ = gethex( fp );
	}
	gethex( fp );
	if ( sum ) {
	    fclose( fp );
	    return( 2 );
	}
    }

    if ( feof( fp ) ) {
	fclose( fp );
	return( 2 );
    } else {
	for ( n = 0; n < 8; n++ ) {
	    if ( getc( fp ) != '0' ) {
		fclose( fp );
		return( 2 );
	    }
	}
	fclose( fp );
	return( 0 );
    }
}

/* machine aborted - exit with/without core dump */

puthex ( b, fp )
    int b;
    FILE * fp;
{
    fputx( b, fp );
    sum += b;
}

coredump ( core )
    char * core;
{
    register BYTE * p;
    register FILE * fp;
    register WORD i;

    fp = fopen( core, "w" );
    if ( fp == ( FILE * )NULL ) {
	fputs( "Cannot write to: ", stderr );
	fputs( core, stderr );
	fputs( "\n", stderr );
	return( 1 );
    }

    for ( i = 0; i < MEMSIZE ; i += 32 ) {
	sum = 0;
	putc( ':', fp );
	puthex( 0x20, fp );
	puthex( i >> 8,	  fp );
	puthex( i & 0xFF, fp );
	puthex( 0x00, fp );
	for ( p = &Mem[ i ]; p < &Mem[ i + 32 ]; p++ ) {
	    puthex( *p, fp );
	}
	puthex( -sum & 0xFF, fp );
	putc( '\n', fp );
    }

    fputs( ":0000000000\n", fp );
    fclose( fp );

    fputs( core, stderr );
    fputs( " dumped\n", stderr );

    return( 0 );
}
