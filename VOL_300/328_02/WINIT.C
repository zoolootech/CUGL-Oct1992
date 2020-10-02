/*! winit ()
 *	setup video modes.  Initialize window system for selected mode.
 *      declare all the global variables for the window system.
 *	NOTE: This function does not write to the video screen.
 *		  No future changes should write to video...
 *		 This allows for wdvinit (DeskView) version, planned for future release
 *
 */

#include  "wscreen.h"
#include  "wsys.h"

#include <signal.h>




/*! global variables
 *
 *  these are declared extern in the headers as externs for type checking
 *	NOTE that this file conserves word alignment, regardless
 *	of whether -a switch is on or off.
 *	To do this, all declarations are in order: far *, long,  *,int, char.
 */



/* pointers to start of video storage (different for each monitor),
 *			current page  (different for each mode/monitor)
 *			and offset from page 0 to page 1
 */
unsigned char far *wvideo_ram =  NULL;
unsigned char far *wpage_ram  =  NULL;



WINDOW  *(w0) = NULL;

WINDOW  *(wfullscreen);


char    *whelp_ptr = NULL;



/* keyboard trap routine and pipes.
 */
int (*wkeytrap)(int) 	= NULL;
int (*wpipein)(void) 		= NULL;
void		(*wpipeout)( int ) 	= NULL;




struct WLOCATION wlocation = { 0, 0, WLOC_CENTER, 0 };






#ifndef TEXTONLY

	/* used only in graphics mode
	 */
	#ifdef __TURBOC__
		char 		*wpath_BGI = "C:\\TC"; 	/* DOS path to BGI drivers */
	#endif	/* TurboC location of BGI files */


	int 		wpxabsmax, wpyabsmax;     /* highest pixel numbers */
	#define		wpxchar 8
	#define		wpychar 8

	int	wegarowsize = 80;					/* 100 for superVGA 800*600 */


#endif	/* ! TEXTONLY */

char *whome_dir = "\0";		/* defualt is current dir */


unsigned int  	wpage_size = 0;

int		wxabsmax	 = 79;
int		wyabsmax 	 = 24;
int		wtabwidth	=8;

/* cursor lines: high byte is start line, low byte is stop line.
 * for CGA, change to 0x0607. Also change cursor size for 43- and 50-line modes
 */
unsigned int wcurscanln = 0x0C0D;		



/* mouse control structure
 */
WMOUSE wmouse = {0,0, 0,0,0,0, 0,0,0,0,  0,0,0,0 };



/* unget buffer
 */
int wunget_buffer = 0;







unsigned char 	wmenuattr   = GREEN;
unsigned char 	wmenuboxattr= GREEN;
unsigned char 	whelpattr   = (CYAN<<4);
unsigned char 	wbuttonattr = 0x70;  /* reverse video */





char 		wmonitor = 0x00;        /* H, V, E, or (text only) C */
char 		wega256  = 1;		/* 1= have 256k of RAM */

char 		wmode;   		/*  'T' or 'G' */


const struct WBOX_PATTERN wbox[] =
	{
	/* NW, Hor, NE, SW, Vert, SE */
	   0,  0,  0,  0,  0,  0        /*0 NO_BORDER 		*/
	,218,196,191,192,179,217	/*1 SINGLE_BORDER	*/
	,201,205,187,200,186,188 	/*2 DOUBLE_BORDER	*/
	,176,176,176,176,176,176	/*3 SPECKLE_BORDER	*/
	,' ',' ',' ',' ',' ',' '	/*4 SOLID_BORDER	*/

	,213,205,184,212,179,190	/*5 MIXED2H1V_BORDER	*/
	,214,196,183,211,186,189	/*6 MIXED1H2V_BORDER	*/

	,209,205,209,212,179,190        /*7 HANGING_BORDER	*/
	,213,205,184,207,179,207	/*8 STANDUP_BORDER	*/

	};





unsigned char	wlastpage =0; 	/* number of supported pages in curr.mode */
unsigned char   wnextpage =0;	/* display next window on indicated page */
unsigned char   wneedpage =0;



/*---------------------------  end of GLOBALS --------------------*/


static void cleanup (void);

static void closeall (void);


void	winit ( char newmode)
	{


	PSEUDOREGS



#ifdef TEXTONLY

	if ( newmode == 'G' )
		{
		perror ("TEXT ONLY ");
		exit ('W');
		}
#endif 	/* TEXTONLY */



	if ( ! wmonitor )
		{
		/* first call to window program */
		wgetmon();



		/* detect enhanced keyboard
		 */
		wkbd_init ();

		/* detect expanded memory, setup disk heap
		 */
		wheap_init ();


		/* ignore control break handling */
		if ( signal(SIGINT, SIG_IGN) == SIG_ERR )
			{
			/* installing signal handler failed */
			perror ("ctrl-c");
			exit('W');
			}


		/* stack exit routines - restorecrtmode, turn on cursor,
		 *
		 */
		atexit (cleanup);


		}
	else
		{
		/* This is a call to change screen mode -
		 * first have to free up all the window storage from
		 * last mode
		 */
		closeall();
		}


	/* disable pages - see winit_pages() to enable pages.
	 */
	wnextpage = wlastpage = 0;
	
	/* have to place HERCULES monitor in FULL mode 
	 * before writing any info to screen, 
	 * if second video page is needed.
	 */
	if ( (wmonitor == 'H') && wneedpage )
		{
		outp ( 0x3BF, 0x02 );	/* FULL mode */
		}



	if ( newmode == 'T' )
		{

		#ifndef TEXTONLY
			if (wmode == 'G')
				{
				wtextmode ();	/* restore CRT */
				}
		#endif



		wvideo_ram = EGA_TEXT_RAM;

		if ( wmonitor == 'H' || wmonitor == 'M' )
			{
			wvideo_ram = HERC_TEXT_RAM;
			}
		else
		if ( wmonitor == 'C' )
			{
			wcurscanln = 0x0607;		/* cursor size for CGA monitors */
			}
			
		wpage_ram = wvideo_ram;
		wxabsmax = 79;
		wyabsmax = 24;
		wmode = 'T';

		} /* end initializing newmode = T */

#ifndef TEXTONLY
	/* graphics mode init routine */

	else    /*( newmode == 'G' ) */
		{
		wmode = 'G';

		if ( wmonitor == 'H' )
			{
			wvideo_ram = HERC_GRAPH_RAM;
			}
		else
			{
			wvideo_ram = EGA_GRAPH_RAM;	
			}

		wpage_ram = wvideo_ram;					/* start on page 0 */

		wgraphmode ();
		
		wegarowsize = (wpxabsmax+1) / 8;		/* bytes per row */

		}	/* end initializing newmode = 'G' */

#endif /* end ifdef TEXTONLY */

	wfullscreen = wdefine
		      (0,0, wxabsmax+1, wyabsmax+1, 0x07, NO_BORDER, 0);

	wmouse_init();

	return; 	/* end winit */

	}












/*! cleanup ();
 *	automatic cleanup routines - restore the screen, cursor, etc...
 *
 */

static void cleanup (void)
		{

		winit ('T');

		wgoto (0, wyabsmax-1);
		wsetattr (0x07);


		wcursor (ON);


		/* restore ctrl-c checking */
		if ( signal(SIGINT, SIG_DFL) == SIG_ERR )
			{
			perror ("ctrl-c");
			exit(99);
			}

		/* cleanup disk heap and release expanded memory
		 */
		wheap_freeall ();

		return;
		}



static void closeall (void)
	{
	WINDOW *wp;
	WBUTTON  *b, *nb;

	while ( w0 )
		{
		/* free screen save area storage
		 */
		if ( w0-> winsave )
			{
			wheap_free ( w0-> winsave );
			}


		/* free button storage
		 */
		for ( b = w0-> winbutton; b; b = nb )
			{
			nb = b->Bchain;

			free (b);
			}





		wp  = w0;
		w0 = w0-> winchain;

		free (wp);

		}


	return;	/* closeall */
	}
	
	/*---------------- end of WINIT.C ----------------------*/