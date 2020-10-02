/* wpulldn.c -
 *     source file for pulldown menus.
 *
 *
 *     method - user calls wpulldown () passing addr of top menu
 *		wpulldown - installs a keytrap routine (event_handler)
 *                        - draws the menu choices on the top line
 *			  - 'shrinks' the fullscreen window down 2 lines.
 *
 *
 *		event_handler()  scans every keystroke.
 *			  - detects if MOUSE on top line or is a menu key
 *			  - if currently doing menu,
 *				- sets flag indicating need to chagne choices
 *			  - if not currently doing menu,
 *			        - activates menu program.
 *
 *		menu_manager()
 *			  - reentrant program,
 *			       calls itself for nested menus.
 *			  - backs out if need to change main menu choice.
 *                        - displays menus and processes keystrokes.
 *
 *
 *     NOTES:
 *	        1) see definition of WMENU below for menu structure.
 *			2) user-driven re-entrant menu selections are avoided
 *			by setting OFF mu_enable before calling functions,
 *			and turning ON mu_enabel after exiting function.
 *
 * 
 */
#include "wsys.h"


int wpulldown_enable =1;		/* global var. for disabling menu */

#include <string.h>
#include <ctype.h>


/* in large model, may need to normalize menu pointers.
 */
#undef     NLZ_MENU

#ifdef __LARGE__
	#define NLZ_MENU(xx)  nlz_menu (xx)
#else
#ifdef __COMPACT__
	#define NLZ_MENU(xx)  nlz_menu (xx)
#endif		/* matches COMPACT */
#endif		/* matches LARGE */



#ifdef 		NLZ_MENU

	void nlz_menu (WMENU *m);

	void nlz_menu (WMENU *menu)
		{
		WMENU *item;


		for ( item= menu; item->mu_entry !=NULL; ++item )
			{

			if ( item->mu_menu )
				{
				/* normalize nested menus, too
				 */
				_NORMALIZE (item->mu_menu);
				nlz_menu (item->mu_menu);
				}
			_NORMALIZE (item->mu_entry);
			}
		return;	/* nlz_menu */
		}

#endif	/* NLZ_MENU */




#if 0
	/* non-compiling inclusion of WMENU structure.
	 *  - included for conveinience
	 */


struct WMENU_st
	{
	char 		*mu_entry;      /*text for menu line */
	void 		(*mu_func)(void);
	char 		*mu_help;       /*help 		     */
	struct WMENU_st 	*mu_menu;       /*nested menu        */
	unsigned char 	*mu_enable;	/*0=disable this entry*/
	int           	mu_highlight;   /*which letter (0-n) to highlight */
	int		mu_key;		/*keypress to select this one     */
	};

typedef struct WMENU_st  WMENU;




#endif /* non-compiling inclusion of WMENU structure. */



/* space alloted to each choice on top line
 */
#define MENU_SPACING 10

#define DOUBLE_LINE_CHAR 205


/* save any other keytrap routines, so event_handler can chain to them
 */
static  int (*oldtrap) (int) = NULL;


/* save ptr to top menu in nested structure
 * save number of  last entry in topmenu.
 */
static  WMENU  *topmenu;
static  int    W_NEAR max_topchoice = 0;





/* flags to control re=entrant calls
 *	( note that the keyboard trap is called whether in menu or out of it
 *      value is -1 if not doing menu, 0... max_topchoice if doing menu.
 */
static int W_NEAR      in_menu    = -1;
static int W_NEAR      savechoice = -1;


/* flag to identify topmost hanging menu
 */
static char W_NEAR     nested = 0;





/* internal subroutines
 */

	/* function to examine each keystroke
	 * call menu if indicated
	 */
static int event_handler (int);

	/* menu function
	 */
static int W_NEAR  execute_choice ( WMENU *menu );

	/* draw top line of screen
	 */
static void 	W_NEAR draw_topline (int);

	/* draw pulldown menus
	 */
static void W_NEAR draw_menu    (WMENU *m, int n);
static void W_NEAR draw_item  
			 (WMENU *menu, int m, unsigned char ch, unsigned char ch2);


	/* locate new choices
	 */
static int  W_NEAR find_choice (int key);
static void W_NEAR rotate  ( WMENU *menu, int *current, int max, int dir );


	/* check out next menu table prior to setting up it's window
	 *	 	returns: offset of first active choice in menu. (-1 if none).
	 *		alters:  size of menu box to draw, rows and cols.
	 */
static int W_NEAR any_active ( WMENU *menu, int *xmax, int *ymax );





/* installation routine
 */
void wpulldown ( WMENU *user_menu )
	{

	oldtrap = wkeytrap;
	wkeytrap = event_handler;



	topmenu = user_menu;



	/* in large and comapct models,
	 * make sure menus are normaliized
	 */

	#ifdef NLZ_MENU

		_NORMALIZE (topmenu);

		NLZ_MENU (topmenu);

	#endif /* NLZ_MENU */






	/* count items in the top menu, determine max # of items.
	 * note that max_topchoice = Last valid subscript number.
	 */
	for ( 	max_topchoice = 0; 
			(topmenu+max_topchoice) ->mu_entry != NULL;
			)
		{
		++max_topchoice;	/* not incremented after final line */
		}

	/* draw top line onscreen with no choice
	 */
	wpulldown_draw ();



	/* shrink the fullscreen window to avoid stepping on the 2 lines
	 */
	wfullscreen-> wintop   =2;
	wfullscreen-> winymax -=2;

	wfullscreen-> winy = min ( wfullscreen->winy,  wfullscreen->winymax);



	return ;	/* wpulldown */
	}




static int event_handler (int key)
	{
	int     choice;
	int 	menu_change;
	char 	*save_help;


	if  (oldtrap)
		{
		key = (*oldtrap) (key);
		}

	if ( savechoice >= 0 )
		{
		/* trying to back out of menu/function
		 * by continually providing ESCAPEs to the
		 * function that called wgetc()
		 *
		 * so setup one extra ESCAPE and return ESCAPE
		 */
		wungetc (ESCAPE);
		return  (ESCAPE);
		}


	
	choice = wpulldown_enable ? find_choice (key) : -1;



	while ( choice >= 0 )
		{
		if ( in_menu >= 0 )
			{
			/* need to back out
			 * of menu or any routines menu may have called.
			 *
			 * setup to feed repeated ESCAPES to routines.
			 *
			 */
			savechoice = choice;	/* save for reactivation  */

			choice  = -1;

			wungetc (ESCAPE);
			return  (ESCAPE);
			}
		else
			{
			/* not a re-entrant call - first time in menu.
			 *
			 * setup top line indicating menu choice,
			 * setup position for pulldown menu
			 */
			save_help = whelp_ptr;

			/* draw top line onscreen with no choice
			 */
			wopen ( 0,0, wxabsmax+1, 2,
				wmenuattr, NO_BORDER, 0, 0 );

			w0-> winputstyle &= ~(WPUTWRAP+WPUTSCROLL);

			draw_topline (choice);

			wgoto ( choice * MENU_SPACING, 0 );

			in_menu = choice;
			nested  = 0;
			menu_change = execute_choice (& topmenu[choice] );
			in_menu = -1;


			wabandon ();

			/* if another choice was made while executing menu,
			 * retrieve that choice
			 */
			if ( savechoice >= 0 )
				{
				choice  = savechoice;	/* do next choice */
				savechoice = -1;

				/* one extra ESCAPE was placed in the
				 * unget buffer
				 */
				wgetc ();

				}
			else
			if ( menu_change == LEFT_ARROW )
				{
				rotate (topmenu, &choice, max_topchoice, -1);
				}
			else
			if ( menu_change == RIGHT_ARROW )
				{
				rotate (topmenu, &choice, max_topchoice, +1);
				}
			else
				{
				choice = -1;
				}

			whelp_ptr = save_help;

			}


		/* key that invoked menu should not be passed back to caller.
		 */
		key = 0;

		/* restore the topline to show no 'selected' choice
		 */
		wpulldown_draw ();


		}



	return (key);                     /* event_handler */
	}




static int W_NEAR execute_choice ( WMENU *menu )
	{
	WMENU *item;

	int   left, top, xmax, ymax;		/* window co-ords */

	int   choice = -1;
	int   newchoice;

	int key;

	int return_key = ESCAPE;

	unsigned char brightattr;

	int     box_type;


	box_type =  ( nested ) ?  SINGLE_BORDER : HANGING_BORDER;
	nested   =  1;

	/* setup help ptr
	 */
	whelp_ptr = menu-> mu_help;

	if ( menu->mu_func )
		{
		/* execute function specified if table, if present
		 */
		(* (menu->mu_func) )();
		return(ESCAPE);		/* quit execute_choice */
		}

	/* pick up ptr to nested menu
	 */
	menu = menu-> mu_menu;


	/* make sure at least one choice is active.
	 * 		Also, counts entries and length of entries in nested menu
	 */
	if (  (choice = any_active ( menu, &xmax, &ymax ))  < 0 )
		{
		/* no entries were active
		 */
		return ( wgetc() );		/* get next keystroke to move off this item */
		}

	/* setup position (1 to left, 1 down
	 * open window, draw choices
	 */
	wsetlocation ( WLOC_ATCUR, 1, 1 );

	wlocate ( &left, &top, xmax, ymax );
	wopen   ( left,  top, xmax, ymax, wmenuattr,
			box_type, wmenuattr, WSAVE2RAM );

	brightattr = wmenuattr | BRIGHT;


	draw_menu ( menu, choice );


	do 	{
		newchoice = choice;

		whelp_ptr = menu[choice].mu_help;

		key = wgetc ();


		if ( isascii (key) && isprint (key) )
			{
			/* match printable keystroke to menu entry
			 */
			key = toupper (key);

			for ( newchoice = 0, item = menu;
			      item->mu_entry !=NULL  && key != item->mu_key;
			      ++item, ++newchoice )
			      { /* empty */ }
				  
			if ( key != item->mu_key )
				{
				/* not found in above loop
				 */
				newchoice = choice;
				}
			else
				{
				/* choice was found, set up to execute it
				 */
				key    = ENTER;
				}
			}
		else
		if ( key == MOUSE  && wmouse.wms_inwindow )
			{
			if ( *  ( (menu+(wmouse.wms_y))->mu_enable) )
				{
				newchoice = wmouse.wms_y;

				if ( wmouse.wms_used & WMS_LEFT_RLS )
					{
					/* menu item was selected
					 */
					key = ENTER;
					}

				}

			}
		else
		if ( key == UP_ARROW )
			{
			rotate (menu, &newchoice, ymax, -1);
			}

                else
		if ( key == DN_ARROW )
			{
			rotate (menu, &newchoice, ymax, +1);
			}
		else
		if ( key == LEFT_ARROW || key == RIGHT_ARROW )
			{
			return_key = key;	/* pass back to caller */
			key = ESCAPE;
			}


		/* now, if necessary, redraw the menu to reflect changes
		 */
		if ( choice != newchoice )
			{
			draw_item ( menu,   choice, wmenuattr,  brightattr );
			draw_item ( menu,newchoice, wbuttonattr,0 );
			choice = newchoice;
			}


		/* execute if ENTER pressed, (or via mouse, or via letter)
		 */
		if ( key == ENTER )
			{
			wgoto ( 0, choice );

			wsetattr (0x07);

			return_key = execute_choice ( & menu[choice] );

			key = ESCAPE;
			}



		}
	while   ( key != ESCAPE );


	wclose ();


	return (return_key);		/* execute_choice */
	}









static void W_NEAR draw_topline ( int choice )
	{
	WMENU 	*item;
	int 	n_item;
	int 	x;
	int 	offset;


	wgoto (0,0);
	wclearline();

	for ( 	n_item=0, item= topmenu, x =0;
		(item->mu_entry) != NULL;
		++item, ++n_item, x += MENU_SPACING )
		{
		wgoto ( x, 0 );

		if ( n_item == choice )
			{
			wsetattr (wbuttonattr);
			}
		else	{
			wsetattr (wmenuattr);
			}
		wputs ( item->mu_entry );

		/* overwrite the highlighted letter
		 */
		if ( *(item->mu_enable) && n_item != choice )
			{
			offset = item->mu_highlight;
			wgoto ( x+ offset, 0 );
			wbright();
			wputc ( (item->mu_entry)[offset] );
			wdim();
			}


		}

	/* second line
	 */
	wgoto ( 0,1 );
	wsetattr (wmenuattr);
	for ( x = 0; x <= wxabsmax; ++x )
		{
		wputc (DOUBLE_LINE_CHAR);
		}
	return; 	/* draw_topline */
	}


/* draw top line onscreen with no choice indicated
 */
void wpulldown_draw ( void )
	{
	wopen ( 0,0, wxabsmax+1, 2, wmenuattr, NO_BORDER, 0, 0 );

	w0-> winputstyle &= ~(WPUTWRAP+WPUTSCROLL);

	draw_topline (-1);

	wabandon ();

	return;		/* wpulldown_draw */
	}



static void W_NEAR draw_menu ( WMENU *menu, int choice )
	{
	int n;
	WMENU *item;

	unsigned char   brightattr;

	int x;


	brightattr = wmenuattr | BRIGHT;

	wsetattr (wmenuattr);
	wclear();


	for (  	n= 0, item =menu; item-> mu_entry !=NULL ; ++n, ++item      )
		{

		wgoto ( 0,n );
		if ( n == choice )
			{
			wsetattr ( wbuttonattr );
			wputs (item->mu_entry);
			wclearline ();
			}
		else
			{

			wsetattr (wmenuattr);
			wputs (item->mu_entry);
			if ( (item->mu_enable)!=NULL  && *(item->mu_enable)!= 0 )
				{
				x = item->mu_highlight;
				wgoto (x, n );
				wsetattr (brightattr);
				wputc (item->mu_entry [x]);
				}
			}
		}

	return;	/* draw_menu */
	}


static void W_NEAR  draw_item (
		WMENU *menu,  int choice,
		unsigned char line_attr,
		unsigned char ltr_attr   )
	{

	int x;
	WMENU *item;


	item = menu+choice;
	x    = item->mu_highlight;




	wgoto ( 0, choice );
	wsetattr (line_attr);
	wclearline();
	wputs (item-> mu_entry);


	if ( ltr_attr )
		{
		wgoto (x, choice);
		wsetattr ( ltr_attr );
		wputc (item-> mu_entry[x] );
		}

	return;	/* draw_item */
	}




/* look for user choices in the top menu line
 * allow either a mouse LEFT-click in the top line
 * or any ALT-key or FKEY that's in the top menu
 *
 * NOTE - this routine picks up any mouse activity on the top line.
 *	  uses in_menu to prevent rapid cycling in and out of same menu.
 */
static int W_NEAR   find_choice (int key)
	{
	int choice;
	WMENU *item;
	
	int  scratch;

	if ( any_active( topmenu, &scratch, &scratch ) < 0 ) 	return -1;

	choice = -1;	/* default = not a choice */


	if ( key == MOUSE  && wmouse.wms_yabs == 0 )
		{
		choice = wmouse.wms_xabs / MENU_SPACING;
		choice = min ( max_topchoice, choice );


		item = topmenu+choice;
		if (  (item-> mu_enable)==NULL || *(item-> mu_enable)==0 )
			{
			choice = -1;		/* inactive */
			}
		if ( choice == in_menu )
			{
			/* this is just minor mouse mvt on the active choice.
			 * don't force ESCAPEs for the menu just to re-enter
			 * same one.
			 */
			choice = -1;
			}
		}
	else
	if ( isALT (key)  ||  ( FKEY(1)<= key && key <= FKEY(10) ) )
		{
		/* search menu to see if it's a hot key
		 */
		for ( 	item =topmenu; item->mu_entry  != NULL; ++item )
			{
			++choice;       /* starts at -1, see above  */
			if ( key == item-> mu_key )
				{
				if ( (item->mu_enable)==NULL ||  *(item->mu_enable)==0 )
					{
					choice = -1;
					}
				break;		/* quit for() loop */
				}
			}
		/* check results of loop - see if terminated without finding
		 */
		if ( (key != item-> mu_key) )
			{
			choice = -1;
			}
		}

	return (choice);	/* find_choice */

	}



/* rotate() - function to move menu ptr up or down
 * 		bypasses inactive choices
 */
static void W_NEAR  rotate ( WMENU *menu, int *current,  int max, int dir )
	{
	int   n;
	char  *ptr;

	int   done =0;

	n          = *current;

	while (  ! done  )
		{
		n += dir;

		if ( n < 0 )
			{
			n = max;
			}
		if ( n > max )
			{
			n = 0;
			}
		/* test to see if enable ptr is non-NULL and not pointing to 0 */
		if ( NULL != (ptr=( (menu +n )->mu_enable )) )
			{
			done = *ptr;
			} 


		}

	*current = n;

	return;	/* rotate */
	}


static int W_NEAR any_active ( WMENU *menu, int *xmax, int *ymax )
	{
	int first_active = -1;
	int x=0, y=0;
	WMENU *item;
	char  *text_ptr;
	
		
	for ( item = menu; NULL != (text_ptr =(item-> mu_entry)); ++item )
		{
		if 	( 	( first_active < 0        )
			&&  ( item->mu_enable != NULL ) 
			&&  ( *(item-> mu_enable) != 0) 
			)
			{
			first_active =y;		/* finds first active line. */
			}
		++y;
		x = max ( x,  strlen ( text_ptr ) );	
		}
	
	*xmax = x+1;
	*ymax = y;
	
	return  first_active;	/* end of any_active() */
	}	
	
/*------------------- end of WPULLDN.C -------------------*/