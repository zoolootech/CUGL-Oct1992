/*! wdos.c	run SYSTEM COMMAND
 *
 */
#include "wsys.h"

#ifndef __TURBOC__
	#include <direct.h>
#else 
	#include <dir.h>
#endif



/*! wdos
 *	DOS interface
 */
void wdos(void)
	{
	char 	command[60];
	char	curr_path[81];
	char    message[81];



	int     more =1;

	#ifndef TEXTONLY
	if (wmode == 'G' )
		{
		wpromptc (NULL,  "DOS not available in graphics mode", NULL);
		return;
		}
	#endif


	command[0]= '0';


	while (more != ESCAPE)
		{
		/* get a command */
		getcwd(curr_path, 80);
		strcpy ( message, "enter DOS COMMAND at the prompt:\n\n\r");
		strcat ( message, curr_path );
		strcat ( message, ">" );

		more = wprompts ("DOS", message, command,sizeof(command) );



		/* several alternate ways to signal quit
		 */
		if ( 	   stricmp(command, "exit") ==0
			|| stricmp(command, "quit") ==0
			|| more == ESCAPE   )
			{
			break;   /*terminate  while() loop*/
			}

		/* do the command */
		wopen(0,0, wxabsmax, wyabsmax, GREEN, NO_BORDER, 0,
			WSAVE2RAM);
		wprintf("%s> %s\n",curr_path, command);
		system(command);
		wgoto ( 0,wyabsmax );
		wscroll ();
		wprintf("Press any key to continue...");
		wgetc();
		wclose();

		} /*end while*/

	return;
	} /* end of wdos */

