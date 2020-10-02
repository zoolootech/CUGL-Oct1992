/* form demonstration program.
 * This program sets up 2 different data entry form, 
 *	one is easy (few fancy options)
 *  one is harder (fancy options included).
 */

#include <stdlib.h>
#include <string.h>




#include "wtwg.h"


static int    integer_data = 0;
static float  float_data   = 0.0;
static char   text_data[]  = "whatever you want";
static char   date_data[11]     = "01/01/1989";
static char   time_data[9]      = "00:00:00";
 

WFORM easy_form[] = 
	{
	WFORMENTRY_F (float_data),     20, 2,		/* set x&y position of data */
	WFORMENTRY_I (integer_data),   20, 3,
	WFORMENTRY_S (text_data),      20, 4,
	WFORMENTRY_DT(date_data),	   20, 6,
	WFORMENTRY_TM(time_data),      20, 7,

	WFORMENTRY_END(),              60, 10		/* width & height of form */
	};


/*--------------------------------------------------------------------*/

		/* setup a slightly more sophisticated form.
		 */


char *choices[] =
	{
	"First",
	"Second",
	"Third",
	"Fourth",
	"Fifth",
	"Sixth",
	"Seventh",
	"Eighth",
	"Ninth",
	"Tenth",
	"Eleventh",
	"Twelvth",
	"Thirteenth",
	"Fourteenth",
	"Fifteenth",
	"Sixteenth",
	"Seventeenth",
	"Eighteenth",
	"Nineteenth",
	"Twentieth",
	"Twenty-First",
	"Twenty-Second",
	"Twenty-Third",
	"Twenty-Fourth",
	NULL
	};




char animal[40] = "Sheep", 
	 choice[10] = "Zero";

int  pos_int = 1;
unsigned int hex = 0x01;


/* special functions for validating custom data types in dext form.
 */
int no_cow (WFORM *form, char *buffer);
int no_neg (WFORM *form, char *buffer);


WFORM hard_form[] =
	{
	/* label              user    picklist  format, func,       len, 	 HOLD x    y  */ 
	 "POSITIVE INTEGER= ",&pos_int, NULL,   "%i",  	no_neg,   3,     0,   20,  4
	,"HEXADECIMAL= ",     &hex, NULL,       "%x",	NULL,	      7,     0,   20,  6
	,"A Label",    	      NULL,   NULL,     NULL,  	NULL,	      0,     0,   20,  8
	,"NOT A COW: ",       animal, NULL,     "%s", 	no_cow, sizeof(animal),0, 20, 10
	,"SELECTION: ",       choice,choices,   "%s", 	NULL,   sizeof(choice),0,20, 15
	,NULL,                NULL,   NULL,     NULL,  	wfvall,			0,       0,40, 20
	};





main ()
	{
	char key;

	int testmode;
	int n;
	char *user_choice;
	char buffer[25];
	
	winit ('T');
	
	wclear();
	testmode = wpromptc ( "> DEMO <", "Select a mode",
				"Text", "Graphics", NULL );


	if ( testmode == 'G' )
		{
		winit ('G');
		}
	else
	if ( testmode == ESCAPE )
		{
		exit (0);
		}
	whelp_install ( "demoform" ); 	/* pass name of help file */

	wmsdrag ();		/* install feature to make all windows draggable by mouse */

	wclear ();
	wputs ( "DEMO of picklists, string input, and form input\n"
			"    -click upper left corner of any window to move it with mouse\n"
			"    -during string input, click upper right to set INSERT mode\n"
			"         or use the INSERT key\n"
			"    -right mouse button is equivalent to ESCAPE key\n"
		);
		
	/* PART I - demo wpicklist () function for selecting 
	 * a string from a NULL-terminated list
	 * Note call to wsetlocation() forces wpicklist() location onscreen.
	 *	see wtwg.h for details on wsetlocation.
	 */
	wsetlocation ( WLOC_ATXY, 3, 7 );
	n = wpicklist ( "Pick One",  choices );
	user_choice = choices [n];
	
	if ( user_choice == NULL )
		{
		user_choice = "You pressed ESCAPE";
		}
	else
		{
		wpromptc ( "YOUR CHOICE", user_choice, NULL );
		}
		 
		 
	/* PART II - demo wprompts() for single string input.
	 */	 
	buffer[0] =0;	/* make sure you don't prime wgets() with garbage data */
	
	wprompts ("PROMPT FOR TEXT", "Your name? ", buffer, sizeof(buffer));
	
	wpromptc (" YOU TYPED ", buffer, NULL );	 
		 

	/* PART III - demonstrate form input using easy_form
	 */
	wfm_autoskip = 0;	/* actually, 0 is default, this line not needed */
	
	wscanform( "AUTOSKIP OFF", easy_form );
	
	wfm_autoskip = WFM_AUTOSKIP; 
	wscanform( "AUTOSKIP ON", easy_form );

	wopen ( 5,5, 60, 15, (RED<<4)+YELLOW, SPECKLE_BORDER,(RED<<4)+YELLOW, 0);
	wtitle ( " form results " );
	wprintf ("\ntext:%s, int:%i, float:%f, date:%s, time:%s",
			text_data, integer_data, float_data, date_data, time_data);
	wgetc();
	wclose();
		
	wfm_autoskip = 0;	
	
	
	/* PART III of demo: trickier form stuff 
	 */
	if ( wmonitor == 'E' || wmonitor == 'V' ) 
		{
		/* splashy (ugly) color demo
		 */
		wfmatr_lbl = (MAGENTA<<4)+YELLOW ;
		wfmatr_lbl_active = (RED<<4)+YELLOW;
		wfmatr_border = ( LIGHTRED<<4 )+YELLOW ;	/* BLINKS in text mode */
		wfmatr_title = ( LIGHTGRAY <<4 )+RED;
		wfmatr_data = (MAGENTA<<4) + LIGHTCYAN;
		wfmatr_data_active = (CYAN<<4);
		wfm_border_type = SPECKLE_BORDER;
		}

	n = wscanform ( "any form title here", hard_form);

	wopen ( 2,2, wxabsmax-2, 5, (LIGHTGRAY<<4)+RED,
		SINGLE_BORDER, (LIGHTGRAY<<4)+RED, WSAVE2RAM );
	
	wprintf ("YOU ENTERED name=%s, positive int data =%i, "
			 "choice=%s, hex data = %x; and you pressed %s",
			animal,  pos_int, choice, hex,
			(n == ESCAPE)? "ESCAPE" : "ENTER" );
	wgetc();

	wclose();

	wprintform ("display only version", hard_form);
	wgetc();

	wclose();	/* closes the window wopen'd by wprintform */


	wclear();
	wgoto ( wxabsmax/2 -3, wyabsmax/2);		/* approx. center */
	wputs ("GOODBYE");

	wgetc();
	
	/* the next line forces the linker to include the floating point library
	 * wscanform uses f.p. library ONLY if float data is specified
	 * 		but it is coded in a way that the linker won't necessarily recognize
	 *		as requiring f.p. (dynamically built arg list to vsprintf)
	 *		so if you have f.p. data in your form, 
	 *		your program MUST do some f.p. character conversion.
	 *
	 */
	float_data = atof ("1.234");
	wclear();

	return(0);	/* main */
	}



/*  demo how to write any type of validation function for special form types.
 *	This function rejects the word "COW"
 */
int no_cow (WFORM *form, char *buffer)
	{
	int rc =0;
	if (memicmp (buffer, "cow", 3) == 0)
		{
		wform_showerror (form, "Illegal data -- \"cow\"");
		strcpy ( buffer, "sheep");
		rc = 1;
		}
	return (rc);	/* no_cow */
	}



/* This function rejects negative numbers.
 */
int no_neg (WFORM *form, char *buffer)
	{
	int rc = 0;
	if ( atoi (buffer) < 0)
		{
		wform_showerror (form, "No negative numbers, please.");
		wform_showerror (form, NULL);
		rc =1;
		}
	return (rc);	/* no_neg */
	}





