/* wpipe.c
 *
 *	wpipefin () - this function 'pipes' the contents of a file
 *			as input to the program, bypassing the keyboard
 *
 *	use: setup a file with input data.
 *	     call wpipefin() to install the pipe.
 *	     subsequent calls to wgetc() or wreadc() will access the pipe.
 *	     the pipe removes itself when completed
 *
 * 	wpipefout () - this function traps keystrokes into a file
 *			 continues to trap keystrokes
 *			 until stopcode is pressed
 *
 * NOTE: mouse use causes value of wmouse to be saved/restored
 *
 *
 */
#include "wsys.h"


static  int 		reader (void);
static  void W_NEAR   getnext (void);
static  int 		in_installed =0;
static  WMOUSE  	nextmouse;
static  int    	 	nextch;
static  void		(*callhome)(void) = NULL;
static	FILE		*inf = NULL;




int  wpipefin ( char *filename, void (*u_callhome)(void) )
	{

	if ( in_installed )
		{
		return (-2);
		}

	inf = fopen ( filename, "rb" );
	if ( ! inf )
		{
		return (-1);
		}

	getnext();

	if ( ! nextch )
		{
		return (-1);	/* empty file or mouse error */
		}

	wpipein = reader;
	callhome = u_callhome;
	in_installed = 1;



	return (0);	/* wpipefin */
	}



static  int  reader (void)
	{
	int pipeval;

	pipeval =  nextch ;

	if ( pipeval == MOUSE )
		{
		memcpy ( &wmouse, &nextmouse, sizeof(WMOUSE) );
		}

	/* note that the pipe has to read one keystroke ahead
	 * so that it knows when to un-install itself
	 */
	getnext();



	return 	(pipeval);
	}



static void W_NEAR getnext (void)
	{
	int n;
	if (1 == ( n= fread ( &nextch, sizeof (int), 1, inf))  )
		{
		/* successfull read
		 */
		if ( nextch == MOUSE )
			{
			n=fread ( &nextmouse, sizeof (WMOUSE), 1, inf);
			}
		}

	if ( n != 1 )
		{
		/* end of file */
		nextch = 0;
		fclose (inf);
		wpipein = NULL;
		in_installed =0;

		/* inform calling routine that end of pipe is reached
		 */
		if ( callhome )
			{
			(*callhome)();
			}

		}



	return; /* getnext */
	}


	/*------------------  input pipe -----------------*/


	/*------------------  output pipe -----------------*/


static void writer (int x);
static int  stopcode = 0;
static FILE *outf;
static int out_installed =0;



void wpipefout ( char *filename, int ustopcode )
	{
	if ( out_installed )
		{
		return;
		}

	if ( NULL == (outf= fopen (filename, "wb") ) )
		{
		werror ('W',"WMACRO - output file error");
		}
	stopcode = ustopcode;

	wpipeout = writer;

	out_installed = 1;

	return; /* wpipefout */
	}





static void writer (int key )
	{
	int n;

	if ( key == stopcode )
		{
		/* un-install the pipe
		 */
		wpipeout = NULL;
		out_installed = 0;
		fclose (outf);
		}
	else
		{
		if ( 1 == (n=fwrite (&key, sizeof(int), 1, outf) ) )
			{
			if ( key == MOUSE )
				{
				n=fwrite (&wmouse, sizeof(WMOUSE), 1, outf);
				}
			}
		if ( n != 1 )
			{
			/* file write error */
			werror ('W',"WMACRO - output file error");
			}

		}

	return;	/* writer */
	}




/*--------------------- END of WPIPE.C ---------------------*/


