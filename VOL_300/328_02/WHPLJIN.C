/* WHPLJIN.C
 *		keytrap routine to call laserjet printer dump
 */
#include "wsys.h"

static int version =0;
static void handler (void);

void whplj_install ( int key )
	{
	whotkey_install ( key,handler );
	}
	
static void handler (void)
	{
	FILE *f;
	int key;
	char fname [8+1+3+1], buffer[80];
	
	key = wpromptc ( NULL, "Print Screen to...", "LaserJet", "File", NULL );
		
	switch ( key )
		{
		case ('L'):
			whplj_dump ( stdprn );
			wpromptc ( NULL, "PRINTING COMPLETED", NULL );
			break;
		case ('F'):
			sprintf ( fname, "HPLJ%4.4i.PCL", version++);
			f = fopen ( fname, "wb" );
			if ( f != NULL )
				{
				whplj_dump ( f );
				fclose (f);
				sprintf ( buffer, "Screen file created: %s", fname );
				wpromptc ( NULL, buffer, NULL );
				}
			break;
		}
	
	return;		/* handler */
	}	
/*----------------- end of WHPLJIN.C ----------------------*/