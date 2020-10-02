
/* demo macro
 *
 *	shows how keyboard macro facility is installed and used.
 */
#include <stdlib.h>
#include "wtwg.h"

main ()
	{
	int n;

	char 		msg[80];
	WMACRO_NAMES   	*mn;
	int            	key;

	winit ('T');


	/* Install keyboard macro routines.
	 * Pressing ALT= activates the amcro recorder,
	 * which asks for a key to assign the new macro to (ALT_1 thru ALT_0)
	 * then asks for a name for the file macname.mcr to store macro in.
	 * then begins recording all key and mouse use until next ALT=
	 */
	wmacro_install ();

	do	{

		msg[0] =0;

		wprompts ( "TYPE HERE", "enter word or two\n"
			" use macros by pressing ALT=\n\n  ",
			msg, 80 );

		key = wpromptc ( "ECHO", msg, NULL );

		}
	while ( key != ESCAPE );


	/* get ptr to table of names of defined macros.
	 */
	mn = wmacro_names();

	wclear();
	for ( n=0; n<10; ++n)
		{
		if ( *mn[n] )
			{
			wprintf ("MACRO #%i name=%s\n",n,(*mn)[n] );
			}
		else
			{
			wprintf ("MACRO #%i is undefined\n", n);
			}
		}

	wputs ("\nPress any key ");

	/* wait */
	wgetc();

	return (0);
	}
