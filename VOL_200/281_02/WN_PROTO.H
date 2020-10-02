/*  wn_proto.h
 *
 *  This header contains the function prototypes for the Unicorn Software
 *  window system Version 2.0.
 *
 */



void	    fld_add( WINDOWPTR, FIELDPTR );
void	    fld_all( WINDOWPTR );
void	    fld_clear( FIELDPTR );
void	    fld_dsply( WINDOWPTR, char *, char * );
int	    fld_err( char * );
int	    fld_hlp( char * );
void	    fld_init( WINDOWPTR );
FIELDPTR    fld_make( WINDOWPTR, int, int, char *, int, char *, char *, int );
int	    fld_message( char * );
int	    fld_read( WINDOWPTR, FIELDPTR );
void	    fld_rjsp( char * );
void	    fld_rjz( char * );
int	    fld_terminate( int );
void	    fld_value( WINDOWPTR, FIELDPTR );
void	    fm_clear( WINDOWPTR );
int	    fm_proc( WINDOWPTR );
int	    sf_active( FIELDPTR, int );
int	    sf_begfunc( FIELDPTR, PFI );
int	    sf_endfunc( FIELDPTR, PFI );
int	    sf_hlptag( FIELDPTR, char * );
int	    sf_inactive( FIELDPTR, int );
int	    sf_message( FIELDPTR, char * );
int	    sf_on_key( FIELDPTR, PFI );
int	    sf_patt( FIELDPTR, int );
int	    sf_prompt( FIELDPTR, char * );
int	    sf_prot( FIELDPTR, char * );
int	    sf_usr_val( FIELDPTR, PFI );
int	    sf_val( FIELDPTR, PFI );
int	    val_mon( int );
int	    val_date( char * );
int	    val_state( char * );
void	    wn_addbg( WINDOWPTR );
void	    wn_addend( WINDOWPTR );
int	    *wn_addr( WINDOWPTR, int, int );
void	    wn_back( WINDOWPTR );
void	    wn_bclr( WINDOWPTR, int );
void	    wn_bdr( WINDOWPTR );
void	    wn_char( WINDOWPTR, int, int, int, int );
int	    wn_check( WINDOW ** );
void	    wn_clear( WINDOWPTR );
void	    wn_clrerr( void );
void	    wn_csr( WINDOWPTR, int, int );
void	    wn_delete( WINDOWPTR );
void	    wn_dsply( WINDOWPTR );
void	    wn_err( char * );
void	    wn_front( WINDOWPTR );
int	    wn_getch( WINDOWPTR, int, int );
void	    wn_hide( WINDOWPTR );
void	    wn_insert( WINDOWPTR, WINDOWPTR );
WINDOWPTR   wn_make( int, int, int, int );
void	    wn_margin( WINDOWPTR, int );
void	    wn_mkbdr( WINDOWPTR, BORDERPTR );
void	    wn_move( WINDOWPTR, int, int, int );
void	    wn_pchar( WINDOWPTR, int );
void	    wn_position( WINDOWPTR, int, int );
void	    wn_printf( WINDOWPTR, char *, ... );
void	    wn_rposition( WINDOWPTR, int, int );
void	    wn_scroll( WINDOWPTR, int );
void	    wn_swap( WINDOWPTR );
void	    wn_unlist( WINDOWPTR );
void	    wn_wclr( WINDOWPTR, int );
void	    wn_wnclr( WINDOWPTR, int );
