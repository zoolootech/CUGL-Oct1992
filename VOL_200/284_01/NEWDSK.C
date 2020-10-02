#include <stdio.h>
#include <fcntl.h>

main ( argc, argv )
    int argc;
    char * argv [];
{
    register int i;
    int fd;
    int rc;
    static char buf [ 2048 ];

    if ( argc < 2 ) {
	fprintf( stderr, "usage: newdsk file ...\n" );
	return( 2 );
    }

    for ( i = 0; i < 2048; i++ ) buf[ i ] = 0xE5;

    rc = 0;
    for ( i = 1; i < argc; i++ ) {
	fd = open( argv[ i ], O_WRONLY | O_TRUNC | O_CREAT, 0666 );
	if ( fd < 0 ) {
	    fprintf( stderr, "cannot open %s\n", argv[ i ] );
	    rc = 1;
	    continue;
	}
	if ( write( fd, buf, 2048 ) < 2048 ) {
	    fprintf( stderr, "cannot write to %s\n", argv[ i ] );
	    close( fd );
	    rc = 1;
	    continue;
	}
	if ( close( fd ) < 0 ) {
	    fprintf( stderr, "cannot close %s\n", argv[ i ] );
	    rc = 1;
	    continue;
	}
    }
    return( rc );
}
