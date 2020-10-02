/* wmacro.c
 *
 *	simple module for keyboard definable macros
 *	uses wpipestr_in & wpipestr_out
 */


#include "wsys.h"




/* array of 8-letter (+1for NULL) names and values of macros
 */
static WMACRO_NAMES name = {0};


static char installed = 0;
static char active    = 0;
static char recording = 0;
static char playing   = 0;


static int  (*oldtrap)(int) = NULL;
static int   trap     (int);
static void callwhendone (void);






void wmacro_install (void)
	{
	if ( !installed )
		{
		installed = 1;
		oldtrap   = wkeytrap;
		wkeytrap  = trap;
		}
	return;
	}


static trap ( int  key )
	{
	int n;
	int response;

	char filename[14];

	if ( oldtrap )
		{
		/* chain to any previous traps
		 */
		key = (*oldtrap)(key);
		}

	if ( active )
		{
		/* prevent processing keystrokes that were requested
		 * from this routine
		 */
		return (key);
		}


	active = 1;


	if ( key == ALT_EQ )
		{
		/* request to record a macro
		 */
		key = 0;
		if ( recording )
			{
			/* this ALT_EQ has stopped the macro recording
			 * wpipestr_out() has seen it and terminated
			 */
			recording =0;
			wpromptc ( "MACRO", "MACRO RECORDING TERMINATED",NULL );
			}
		else
			{
			response = wmacro_select ("Pick a macro to define");

			if ( response != ESCAPE )
				{
				n = response - ALT_1;

				response = wprompts ("MACRO",
					"Enter a name for the macro\n"
					"Recording begins after ENTER\n"
					"Recording ends following ALT =\n"
					"     ", name[n], 9);
				}
			if ( response != ESCAPE )
				{
				strcpy (filename, name[n] );
				strcat (filename, ".mcr"  );

				recording =1;
				wpipefout (  filename, ALT_EQ ) ;
				}

			}
		}
	else
	if ( (!recording)  && (!playing) && key >= ALT_1  && key <= ALT_0 )
		{
		/* request to play a macro
		 * NOTE above if statemnt guarantees no nested macro calls.
		 */
		n = key - ALT_1;
		key = 0;

		if ( *name[n] != 0 )
			{

			strcpy (filename, name[n] );
			strcat (filename, ".mcr"  );

			if ( 0 == wpipefin ( filename, callwhendone ) )
				{
				playing =1;
				}


			}

		}

	active = 0;


	return (key);	/* trap */
	}


static void callwhendone (void)
	{
	playing = 0;
	return;
	}


	/*-----------------------------------*/



/* wmacro_select
 *
 * screen layout is 46x12, 2 columns each: "ALT_n NAMETEXT   "
 * with ESCAPE in bottom center.
 *
 */
static char btxt[10][6] =
	{"ALT_1","ALT_2","ALT_3","ALT_4","ALT_5",
	 "ALT_6","ALT_7","ALT_8","ALT_9","ALT_0"};
#define BTN_LEN 6

int	wmacro_select (char * msg)
	{
	int     key, n, x, y;

	wopen (21,4, 43,13, wmenuattr, SINGLE_BORDER, wmenuattr, WSAVE2RAM);
	wtitle ("\xb4 MACROS \xc3");

	wgoto ( 3,1 );
	wputs (msg);


	for ( n=0, x=3, y=4;     n<10;     n++, y++ )
		{
		wbutton_add ( btxt[n], x, y, BTN_LEN, (ALT_1 + n), 0 );
		wgoto ( x+6, y );
		wputs ( name [n] );


		if ( n == 4 )
			{
			/* second column
			 */
			x = 21;
			y = 3;
			}
		}


	wbutton_add ( "ESCAPE", 20, 11, 7, ESCAPE, WBTN_BOX );

	do	{
		key = wgetc();

		/* convert char numbers to ALT_nmubers
		 */
		if ( key == '0' )
			{
			key = ALT_0;
			}
		else
		if ( key >= '1' && key <= '9' )
			{
			key += ( ALT_1 - '1' );
			}

		}
	while ( ! ( key == ESCAPE || ( key >= ALT_1 && key <= ALT_0 ) ) );

	wclose ();

	return (key);   /* wmacro_select */
	}


#ifdef __TURBOC__
	/* TurboC wants the declarations of the function and of the array to
	 * match exactly, which is 'strong type checking'
	 */
WMACRO_NAMES *(wmacro_names) (void)
	{
	return (&name);
	}
#else
	/* Microsoft C can't seem to handle an & in front of the array name.
	 */
WMACRO_NAMES *(wmacro_names) (void)
	{
	return (name);
	}
#endif	/* Microsoft C version */	



void wmacro_assign (char *filename, int key)
	{
	int n;

	if ( key < ALT_1  || key > ALT_0 )
		{
		werror ('W', "MACRO assign bad key");
		}

	n =  (key == ALT_0) ? 9 : key - ALT_1;


	memcpy ( name[n], filename, 8 );
	name[n][8] = 0;


	return;	/* wmacro_assign */
	}

/*-------------------- end of WMACRO.C --------------------- */
