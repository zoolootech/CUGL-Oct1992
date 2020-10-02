/*! wexit.c
 *
 *	verify that user wants to quit,
 *	if yes, exit to DOS.
 */
#include "wsys.h"


void wexit (void)
	{
	unsigned char saveattr;
	int  key;




	saveattr = wgetattr();

	wsetattr( ( RED<<4) + WHITE ) ;

	key =wpromptc (NULL, "Verify: Q = quit program, ESCAPE = continue",
			"Quit",  NULL);


	if ( key == 'Q' )
		exit(00);


	wsetattr (saveattr);
	return;
	}