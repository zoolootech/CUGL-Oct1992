/* main.c - main program for virtual CP/M simulator */

main ( argc, argv )
    int argc;
    char * argv [];
{
    if ( !trace_init( argc, argv ) ) {
	if ( !x_init( argc, argv ) ) {
	    if ( !i_init( argc, argv ) ) {
		machine();
	    }
	    i_exit();
	}
	x_exit();
    }
    trace_exit();
    exit( 0 );
}
