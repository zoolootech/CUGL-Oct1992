/* WPOPFKEY.C hotkey routine to pop up an FKEY menu 
 * 		once the mouse center button is pressed.
 */
#include "wsys.h" 

static char **save_labels = NULL;

static int fkeylen[] = 
	{3,3,3, 3,3,3, 3,3,3, 4,4,4};
static int fkeyval[] =
	{FKEY(1),FKEY(2),FKEY(3), FKEY(4),FKEY(5),FKEY(6), 
	 FKEY(7),FKEY(8),FKEY(9), FKEY(10),FKEY_11,FKEY_12 };
static char *fkeyname[] =
	{
	"F1","F2","F3", "F4","F5","F6",  	
	"F7","F8","F9", "F10","F11","F12"
	};	 
static int mouse_fkeys (void);
	

void wpopfkey ( char **keylabels )
	{
	save_labels = keylabels;
	wmspopup ( mouse_fkeys );
	return;		/* wpopfkey () */
	}
	
static int  mouse_fkeys (void)
	{
	int more_fkeys =1;
	int n, row, col, key;
	char *label;
	
	wopen ( 15, 5, 55, 14, LIGHTGRAY, SINGLE_BORDER, LIGHTGRAY, WSAVE2RAM );
	wtitle ( " Function Keys " );
	col =5; 
	row =1;
	for ( n=0; n<12; ++n )
		{
		/* note sequence quarantee operator to assign FKEY labels 
		 * until all are done
		 */
		if ( ( more_fkeys != 0 ) && (NULL == (label=save_labels[n]) ) )
			{
			more_fkeys = 0;
			}
		if ( n==6 )
			{
			col = 30;
			row = 1;
			}	
		wbutton_add (fkeyname[n], col, row, fkeylen[n], fkeyval[n], 0 );
		wgoto ( col + 5, row );
		wputfl ( label, 15 );
		row +=2;
		}
	wbutton_add ( "ESCAPE", 22, 13, 7, ESCAPE, 0 );
		
	do { key = wgetc (); }	
	while (  ! wbutton_test(key) );
			
	wclose ();	
	if ( key == ESCAPE ) key =0;
	return key;		/* mouse_fkeys() */
	} 	
	
	
/*--------------------- end of WPOPFKEY.C ----------------------*/
