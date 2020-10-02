/*
   MISC.H  - misc.c prototypes for flex project. 1/89 DrH
*/

void action_out( void ) ;
char *allocate_array( int, int) ;
void bubble( int [], int ) ;
char clower( char ) ;
char *copy_string( char *str ) ;
void cshell( char [], int) ;
void dataend( void ) ;
void dataflush( void) ;
char *gettimestr( void) ;
void lerrif( char *, int) ;
void lerrsf( char *, char *) ;
void flexerror( char * ) ;
void flexfatal( char * ) ;
void line_directive_out( FILE *) ;
void mk2data( int) ;
void mkdata( int ) ;
int myctoi( char * ) ;
char myesc( char []) ;
int otoi( char * ) ;
char *reallocate_array( char *, int, int) ;
void skelout( void) ;
void transition_struct_out( int, int ) ;
