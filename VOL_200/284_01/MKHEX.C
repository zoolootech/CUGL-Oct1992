/* mkhex.c - make CPMnn.HEX file */

#include <stdio.h>

#define COPYRIGHT	"Copyright (C) 1979 Digital Research"

#define LENGTH		0x1600
#define RECORD		32

unsigned char sum;

puthex ( b, fp )
	unsigned char b;
	FILE * fp;
{
	fprintf( fp, "%02X", b );
	sum += b;
}

main ( argc, argv )
	int argc;
	char * argv [];
{
	FILE * ifp, * ofp;
	char ifid [ 14 ], ofid [ 14 ], bfid [ 14 ];
	unsigned base, size;
	int n, i, c;

	/* check command syntax */
	if ( argc != 2 ) {
		fprintf( stderr, "usage: mkhex size\n" );
		return( 2 );
	}

	/* get CP/M size in KB */
	size = atoi( argv[ 1 ] );
	if ( size < 20 ) {
		fprintf( stderr, "%dKB is too small; min. 20KB\n", size );
		return( 2 );
	}
	if ( size > 64 ) {
		fprintf( stderr, "%dKB is too large; max. 64KB\n", size );
		return( 2 );
	}

	/* find file names */
	sprintf( ifid, "CPM%2d.COM",  size );
	sprintf( ofid, "CPM%2d.HEX",  size );
	sprintf( bfid, "BIOS%2d.HEX", size );

	/* find base address of CP/M system */
	base = 0x3400 + ( size - 20 ) * 1024;

	/* open output file */
	ofp = fopen( ofid, "w" );
	if ( ofp == NULL ) {
		fprintf( stderr, "cannot open %s for output\n", ofid );
		return( 1 );
	}

	/* open CP/M image file */
	ifp = fopen( ifid, "rb" );
	if ( ifp == NULL ) {
		fprintf( stderr, "cannot open %s for input\n", ifid );
		return( 1 );
	}

	/* skip garbage */
	if ( fseek( ifp, 0x0880L, 0 ) ) {
		fprintf( stderr, "cannot seek to 0980H of %s\n", ifid );
		return( 1 );
	}

	/* output title */
	fprintf( ofp, "# %dKB CP/M-80 ver. 2.2, %s\n", size, COPYRIGHT );

	/* generate Intel HEX format */
	for ( n = 0; n < LENGTH; n += RECORD ) {

		/* clear check sum */
		sum = 0;

		/* output a record header */
		putc( ':', ofp );
		puthex( RECORD, ofp );
		puthex( ( n + base ) >> 8,   ofp );
		puthex( ( n + base ) & 0xFF, ofp );
		puthex( 0, ofp );

		/* output data */
		for ( i = 0; i < RECORD; i++ ) {
			puthex( getc( ifp ), ofp );
		}

		/* output check sum and trailer */
		puthex( ( -sum ) & 0xFF, ofp );
		putc( '\n', ofp );
	}

	/* close input file */
	if ( feof( ifp ) ) {
		fprintf( stderr, "file %s is too short\n", ifid );
		return( 1 );
	}
	if ( ferror( ifp ) ) {
		fprintf( stderr, "file read error on %s\n", ifid );
		return( 1 );
	}
	if ( fclose( ifp ) ) {
		fprintf( stderr, "cannot close file %s\n", ifid );
		return( 1 );
	}

	/* open BIOS file */
	ifp = fopen( bfid, "r" );
	if ( ifp == NULL ) {
		fprintf( stderr, "cannot open %s for input\n", bfid );
		return( 1 );
	}

	/* copy BIOS file */
	fprintf( ofp, "# %dKB BIOS image\n", size );
	while ( ( c = getc( ifp ) ) != EOF ) putc( c, ofp );

	/* close BIOS file */
	if ( ferror( ifp ) ) {
		fprintf( stderr, "file read error on %s\n", bfid );
		return( 1 );
	}
	if ( fclose( ifp ) ) {
		fprintf( stderr, "cannot close file %s\n", bfid );
		return( 1 );
	}

	/* close CP/M image file */
	if ( ferror( ofp ) ) {
		fprintf( stderr, "file write error on %s\n", ofid );
		return( 1 );
	}
	if ( fclose( ofp ) ) {
		fprintf( stderr, "cannot close file %s\n", ofid );
		return( 1 );
	}

	return( 0 );
}
