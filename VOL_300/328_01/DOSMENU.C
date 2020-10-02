static char helptext[] =

" DOSMENU - a menu program for batch files.\n"
"    PARAMETERS:\n"
"         a list of menu choices, separated by spaces.\n"
"         to place a space inside a line, use double quotes\n"
"         optionally, the first parameter may start with a '!'\n"
"         ...in which case it is the menu title.\n"
"    RETURN:\n"
"         DOSMENU sets the errorlevel to the number of the choice, 1,2,3....\n"
"                 or to 0 if user pressed ESCAPE\n"
"    NOTES:\n"
"         remember that test for ERRORLEVEL is EQUAL OR GREATER than value\n"
"    EXAMPLE (in a .BAT file):\n"
"      DOSMENU !select  \"word perfect\"  dbase  lotus\n"
"      if ERRORLEVEL 3 goto :lotus\n"
"      if ERRORLEVEL 2 goto :dbase           ...NOTE tests for 2 or higher\n"
"      if ERRORLEVEL 1 goto :wordpf          ...NOTE tests for 1 or higher\n"
"      goto :quit                            ...ERRORLEVEL=0 means ESCAPE\n"
"      ... follow with labels :lotus  :dbase  :wordpf in .BAT file \n"
"\n"
" copyright David Blum 1987, 1989";

/* to compile using TurboC and WTWG window toolkit:
 *		tcc -ms dosmenu.c wts.lib 
 */


#include <stdlib.h>
#include "wtwg.h"


/* slide()
 * utility routine. The argv list comes with element 0 pointing to prog name
 * and no terminal NULL.
 *
 * Slide moves all the argv's down 1 and creates a terminal NULL
 * also  it translates underscores to spaces
 */
static void slide ( int n, char **list );

static void slide ( int n, char **list )
	{
	char **next;
	while ( --n )
		{
		next = list +1;
		*list = *next;
		list = next;
		}
	*list = NULL;

	return;
	}


main (int argc, char **argv)
	{
	int code;
	char *title;
	char **list;

	winit ('T');

	if ( argc == 1 || argv[1][0] == '?' )
		{
		wclear();
		wputs (helptext);
		exit (0);
		}

	slide (argc, argv);

	if ( argv[0][0] == '!' )
		{
		/* first choice is a title
		 */
		title = (argv[0])+1;
		list  = argv +1;
		}
	else
		{
		title = NULL;
		list = argv;
		}

	code = wpicklist (title, list);

	/* the returned code is 0 for first parm, 1 for second, so forth,
	 * but if user picked ESCAPE, the returned value points to the NULL
	 */
	if ( argv[code] == NULL )
		{
		code = 0;
		}
	else
		{
		++code;
		}

	return (code);
	}
/*------------------------ end of DOSMENU.C -------------------------*/