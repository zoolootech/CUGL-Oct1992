/*! wscanform() and wprintform()
 *
 * This file contains wscanform() and wprintform(),
 *	which are the two form-handling routines for the windows package
 *	Also contains slave routines that do small tasks for the two major()
 *
 * WSCANFORM -  collects all the initial values for the form into a buffer,
 *              draws the form, along with a highlighted selection bar
 *		accepts user keystrokes, which can either...
 *			move the highlight bar to another item
 *			enter new data to the item
 *			restore original data to fields.
 *			Save data and quit, or just plain quit
 *		if new data is entered, it is validated:
 *			numeric data is checked for correct format
 *			optional special editting functions are called
 *		when all data is entered,
 *			Entire form can be validated
 *			data stored to program areas
 *
 *
 * WPRINTFORM - MUCH SIMPLER, output only version.
 *		start with this routine to decipher wscanform
 *		gathers initial values into buffer,
 *		prints values and labels on screen,
 *		returns.
 *
 *
 *
 * See typedef WFORM, provided as a comment below:
 *
 * the table is terminated by a NULL in the wflabel position.
 *
 * FIRST LINE line of the table is the header: form title, final validation
 *
 * DATA LINES may be type 's' (string) 'c' (char) 'f' (float) or 'i' (int)
 *	doubles, long int, octal, hex, and pointers not supported.
 * a picklist may be specified instead of hand-typed entry.
 * a form item may be a 'pure label' ie, only the wflabel is printed,
 * the x,y positions for data items determine the screen start of the data
 *	the label is placed before the data.
 *	Be sure the label fits in the space you provided.
 *
 * LAST LINE is markded by a NULL in the wflabel position,
 *	provide an x,y size of the form
 *
 *	CAUTION: code strongly depends on first and last lines not being 'labels'
 *
 *	RETURNS:  ENTER or ESCAPE
 */
#include "wsys.h"



	/* FAR POINTER COMPARISON MACROS
	 *	These macros allow far ptrs to be compared looking only at offset part.
	 *
	 *  This improves code size and speed, but is dangerous...
	 * 	only will work if the segment portions of ptrs are same
	 *		(ie, NORMALIZE macro only applied once and all ptrs then computed)
	 *
	 * ALSO, don't use to compare against the NULL pointer, won't work.
	 * AND, don't even think about fooling with the extra ().
	 *
	 *	IF this coding practice scares you, 
	 *			convert the (void near*) to (void huge*) which is always safe
	 *
	 *	NOTE that simply comparing the pointers without either cast 
	 *		may also generate incorrect code in large model
	 *
	 *	REFERENCE: TURBO C user's guide, 'MEMORY MODELS...POINTERS'
	 *		note that the < and > macros are redundant, but included for clarity
	 */
#define FARPTR_EQ(aa,bb)  ( ((void near*)(aa))  ==  ((void near*)(bb)) ) 
#define FARPTR_GT(aa,bb)  ( ((void near*)(aa))  >   ((void near*)(bb)) ) 
#define FARPTR_LT(aa,bb)  ( ((void near*)(aa))  <   ((void near*)(bb)) ) 



char wfmatr_lbl = CYAN,					/* labels							 */
	 wfmatr_lbl_active =(LIGHTGRAY<<4),	/* label indicating active data item */
	 wfmatr_border = CYAN,				/* form border 						*/
	 wfmatr_title  = CYAN,				/* form title						*/
	 wfmatr_data   = CYAN,	 			/* inactive data					*/
	 wfmatr_data_active=LIGHTCYAN;		/*active data item (while typing) 	*/

int	 wfm_border_type = DOUBLE_BORDER;

char wfm_autoskip =0;					/* set to WGETS_SKIP for autoskip */


/*! local functions
 *
 *  print_labels:
 *	local function to draw the labels in the form
 *	with the current choice highlighted,
 *
 *  print_values:
 *	print on the screen the current values of all the data items.
 *
 *  get_user_data:
 *	pull user data (original values) into the work buffer
 *
 *  numeric2workarea:
 *	convert numeric data of any type into string.
 *	type and maximum length indicated by form table.
 *
 *  setup_window:
 *	scan the form table, compute size of window,
 *	fill in data type values in form table,
 *	compute size of workarea, allocate workarea buffers,
 *
 *
 */
static void 	W_NEAR    print_labels ( WFORM *form, WFORM *mark );
static void     W_NEAR    print_label_line ( WFORM *form, unsigned char attr );
static void 	W_NEAR    get_user_data ( WFORM *form, char *workarea );
static void 	W_NEAR    numeric2workarea
				(WFORM *form, char *workarea, void *data );

static void 	W_NEAR    print_values (WFORM *form, char *workarea );
static void     W_NEAR    print_single_value (WFORM *form, char *workarea );

static void     W_NEAR    setup_window
		(WFORM *formstart, WFORM **formlast,
		 char **workstart, int *worksize, char **worklast);




/* Use the 'reserved' byte in the form table to count the length of
 * the string data storage area.
 */
#define  wftype wfspecial


/* minimum width of a form, required by buttons
 */
#define MIN_WIDTH       41


/*! wscanform()
 *
 */
int wscanform (char *title, WFORM *formstart)
	{

	/* pointers into the MENU tables
	 *	the following pointers point to which pointers to use
	 *	'item' is the line currently being examined
	 *	'mark' is the most recent user choice
	 *
	 */
	WFORM 	*item,		/* ptr to a   form line in table*/ 
			*mark,		/* ptr to the form line currently active */  
			*old_mark,	/* ptr to prev.    line */ 
			*formlast;	/* ptr to last valid (non-NULL) form line in table */

	WFORM *msitem, *mouse_mark;	/* mouse scratch areas */
	char  *mschar, *mouse_workarea;

	int 	key = 0, control_key =0, errors, done =0;

	char 	*workarea, *workstart, *worklast;
	int 	worksize;

	/* numeric (conversion) work areas
	 */
	int 	n;
	char 	numbers[40];	/* long enough to hold any float */

	/* picklist work pointer
	 */
	char *pickptr;
	
	char gets_behavior;		/* flag to govern wgets() */

	char save_title_atr;


	/* global validation function (see last line of form)
	 */
	int  (*glob_func)(WFORM *, char *);

	_NORMALIZE (formstart);		/* model-dependent */


	setup_window
		(formstart, &formlast, &workstart, &worksize, &worklast);

	if ( title != NULL )
		{
		save_title_atr = w0-> winboxcolor;
      	w0-> winboxcolor = wfmatr_title;
		wtitle (title);
		w0-> winboxcolor = save_title_atr;
		}



	/* setup the 'mark' ptr to indicate which field is active
	 */
	mark      = formstart;

	workarea = workstart;	/* point to work string */




	get_user_data (formstart, workstart);
	print_values  (formstart, workstart);

	/* write instructions
	 *
	 *
	 */
	n= w0->winymax;                                        /* options */
	wbutton_add ("ESCAPE",             1, n, 7, ESCAPE,     0);
	wbutton_add ("Ctrl-Original",      8, n, 14, CTRL('O'), 0);
	wbutton_add ("Ctrl-Save",         22, n, 10, CTRL('S'), 0);
	wbutton_add ("F1-HELP",           32, n,  8, FKEY(1),   0);

	old_mark = mark;
	print_labels (formstart, mark);

	/*! loop
	 *
	 *
	 *     main loop for form routine
	 *				{
	 *				get key
	 *				check for mouse field movement 
	 *				keyboard field movement (and/or termination cmds)
	 *				data entry
	 *				}
	 *
	 *
	 */
	while ( ! done )
	{
	whelp_ptr = mark-> wflabel;		/* setup help ptr for this form item */

	/* if we need to process a control key code from last time around, do it
	 * else, get next keystroke from keyboard 
	 */
	key =  ( control_key ) ? control_key : wgetc ();
	
	control_key = 0;		/* prevent re-use of control key */

	/*----------------- MOUSE FIELD MOVEMENT CONTROL -----------*/
	if ( key == MOUSE )
		{
		if ( (wmouse.wms_used & WMS_LEFT_RLS) && wmouse.wms_inwindow )
			{
			/* left button and mouse in window */

			/* scan  the form
			 * (and move through workarea simultaneously)
			 *
			 */
			mouse_mark = NULL;
			mouse_workarea = NULL;
			for (mschar = workstart, msitem = formstart;	
			     msitem-> wflabel !=NULL;					
			     mschar += msitem-> wflen, ++msitem )	/* keep them together */
				{
				/* as long as current entry is above
				 * mouse position, keep as a 'possible' selection
				 */
				if ( msitem-> wfy <= wmouse.wms_y
				     && msitem-> wfx <= wmouse.wms_x
				     && msitem-> wfuser != NULL
				   )
					{
					/* hang on to choice - will end up using highest one
					 */
					mouse_mark = msitem;
					mouse_workarea = mschar;
					}
				}	/* end scan for mouse line */
			
			if ( mouse_mark != NULL )		/* mouse found a data item */
				{
				/* move to 'moused' line. 
				 */
				mark = mouse_mark;
				workarea = mouse_workarea;
				key = ENTER;				/* triggers data entry below */
				}

			}
		}	/* end of MOUSE field movement control */

	/* KEYBOARD FIELD MOVEMENT CONTROL */
	
	switch (key) 
		{
	case (BACKTAB):
	case (UP_ARROW):
		/* loop backwards until a field is found
		 */
		while ( FARPTR_GT (mark, formstart )) 
		/* backwards until a field is found*/
			{ 
			--mark;
			/* leave loop when a data field is found (ie, not a label)
			 */
			if ( mark -> wfuser != NULL )   
				{
				/* keep the workarea pointer in the right place
				 */
				workarea -= mark->wflen;
				break;						/* break from while(mark) */
				}
			}
		break;		/* break from switch(key) */

	case (TAB):
	case (DOWN_ARROW):

		if ( FARPTR_EQ (mark, formlast) )  /*roll to top if at bottom */
			{
			mark = formstart;
			workarea = workstart;
			}
		else						/* advance until a field is found*/
		while (FARPTR_LT (mark, formlast) ) 		
			{ 
			/* keep the workarea pointer in the right place
			 * NOTE this has to be done before moving mark.
			 */
			workarea += mark->wflen;
			
			++mark;
			/* leave loop when a data field is found (ie, not a label)
			 */
			if ( mark -> wfuser != NULL )   
				{
				break;						/* break from while(mark) */
				}
			}

		break;		/* break from switch (key) */

	case ( ESCAPE ):

		wsetlocation ( WLOC_ATWIN, 2, w0->winymax +1 );
		if 	(ESCAPE == ( wpromptc (NULL,
								"Quit without saving or Continue form entry?",
				 				"CONTINUE", NULL) )
			)
			{
			done = ESCAPE;
			}
		
	case (HOME):
		mark = formstart;
		workarea = workstart;
		break;

	case (END):
		mark = formlast;
		workarea = worklast;
		break;

	case ( CTRL('S') ):
		/* see if data in form is valid
		 */
		glob_func = (formlast+1)-> wfvalidate;

		if ( glob_func != NULL)
			{
			errors = (*glob_func)(formstart, workstart);
			}
		else
			{
			errors = 0;
			}


		/* update and then quit
		 */
		if ( ! errors )
			{
			wsetlocation ( WLOC_ATWIN, 31, w0->winymax+1 );

			if 	('S' ==  (wpromptc (NULL, "Save new values ?","Save", NULL) ) )
				{
				/* update user data from workarea
				 */
				for (	item= formstart, workarea= workstart;
					item->wflabel != NULL;
					workarea += item->wflen,  ++item
				    )
					{
					if (item ->wfuser != NULL)
						{
						switch ( item ->wftype )
							{
						case ( 'c' ):
							*((char*)(item->wfuser))= *workarea;
							break;

						case ( 's' ):
							strcpy( (char*)(item->wfuser), workarea );
							break;

						default:
							sscanf (workarea, item->wformat, item->wfuser);
							}/* end switch */
						}
					}
				/* end of updating */
				done =ENTER;
				}	/* end if (key) */
			}       /* end if (errors) */

		break;

	case ( CTRL('O') ):
		/* refresh workarea with original data */
		wsetlocation ( WLOC_ATWIN, 10, w0->winymax +1);

		if 	('O'== (wpromptc (NULL, "Refresh form with original data?",
				"Original data", NULL))
			)
			{
			get_user_data   (formstart, workstart);
			print_values    (formstart, workstart);
			}
		break;
	default: ;
	}				// end switch (key)

	/* If we've changed lines 
	 *   by TAB or BACKTAB in pass thru loop),
	 * then need to draw label to indicate new active line.
	 */
	if ( !  FARPTR_EQ (old_mark, mark ) )
		{
		print_label_line ( old_mark, wfmatr_lbl );
		print_label_line ( mark,     wfmatr_lbl_active );
		wsetattr (wfmatr_lbl);	/* keep attribute set as wformattr */
		old_mark = mark;
		}
	/*----- end of FIELD MOVEMENT CONTROL -----------*/


	/*------------------- DATA ENTRY -------------------*/

	if ( (isascii(key) && isprint (key))
	     || key == RIGHT_ARROW 	|| key == DELETE
	     || key == INSERT 		|| key == ENTER
	   )
		{
		/* Start data entry at currently marked field.
		 * put key back in buffer for wgets() or for wpicklist() to use
		 */
		if ( key != ENTER )
			{
			wungetc(key);
			}
		control_key = 0;
		if ( mark-> wfpick != NULL )	/* this field uses a picklist */
			{
			/* adjust location of list to under and right of item
			 *
			 */
			wgoto ( mark ->wfx, mark -> wfy );
			wlocation.wloc_type = WLOC_ATCUR;
			wlocation.wloc_x    = 4;
			wlocation.wloc_y    = 1;

			n = wpicklist (NULL, mark->wfpick);

			pickptr = (mark->wfpick) [n];

			if ( pickptr != NULL )		/* not ESCAPE from user */
				{
				memcpy ( workarea, pickptr, mark->wflen );
				*(workarea +((mark->wflen)-1) ) = 0;
				control_key = ENTER;		/* move to next form line */
				}

			}	/*end get picklist */
		else
			{

			/* activate wgets()
			 * setup screen position, attribute changes, and form control flag
			 */
			switch ( mark->wftype )
				{
			case ( 'f' ):
			case ( 'e' ):
			case ( 'g' ):
			case ( 'E' ):
			case ( 'G' ):
					gets_behavior = WGETS_FORM + WGETS_INT + WGETS_DEC;
					break;
			case ( 'u' ):
			case ( 'i' ):
			case ( 'd' ):
					gets_behavior = WGETS_FORM + WGETS_INT;
					break;
			default:
					gets_behavior = WGETS_FORM;
				}
				/* end switch */
				
			gets_behavior  |=  wfm_autoskip;
			
			wgoto (mark->wfx, mark->wfy);
			
			wsetattr (wfmatr_data_active);		/* highlight */
			control_key = wgets ( mark-> wflen, workarea, gets_behavior );
			wsetattr (wfmatr_lbl);

			if  ( control_key == ESCAPE || control_key == CTRL('O') ) 
					{
					/*These requests apply to single field only, not whole form
					 */					
					control_key = 0;
					}

			/* If numeric, (ie, not string or char)		  
			 *    convert strings into numeric values and back again
			 *    so user can see whether his input was valid
			 * make sure the terminal nulls don't get lost.
			 */
			if ( ! (mark-> wftype == 's' || mark->wftype == 'c'))
				{
				sscanf (workarea, mark->wformat, (void *) numbers);
				numeric2workarea (mark, workarea, numbers);
				}
				/* end integer data conversion */
				/* end numeric data conversion */
			}  /* end if ... else... use wgets() to get data */


		/* if a wfvalidate() function was specified
		 * for this field, execute it now.
		 */
		if (  ( (mark->wfvalidate) != NULL )
			&&
			  ( 0 != (*mark->wfvalidate) (mark, workarea) )
			)
			{
			/* invalid data
			 */
			control_key = RIGHT_ARROW;	/*force to re-enter this item */
			}
			
		else
			{
			if ( control_key == ENTER )	/* returned code from wgets() */
				{
				control_key =TAB;		/* valid data ENTER'd, so move down */
				}
			}
		/* The string was highlighted while doing wgets()
		 * now replace it with a non-highlighted version
		 */
		print_single_value ( mark, workarea );

		}	/* end if ... get new data     END DATA ENTRY */




	}    /*end of while(!done) loop for menu control*/


	/* terminate form processing = cleanup
	 */

	free (workstart);

	wclose ();

	return (done);		/* wscanform */
	}








/* wprintform()
 *	like wscanform() but output only, and leaves window on screen
 *	This is obviously a simple program,
 *	it just calls the setup and output routines used by wscanform()
 *
 */


void wprintform (char *title, WFORM *formstart)
	{

	/* pointers into the MENU tables
	 *	the following pointers point to which pointers to use
	 *	'item' is the line currently being examined
	 *	'mark' is the most recent user choice
	 *
	 */
	WFORM   *formlast;

	char save_title_atr;



	char 	*workstart, *worklast;
	int 	worksize;


	_NORMALIZE (formstart);		/* model-dependent */




	setup_window
		(formstart, &formlast, &workstart, &worksize, &worklast);

	if ( title !=  NULL )
		{
		save_title_atr = w0-> winboxcolor;
      	w0-> winboxcolor = wfmatr_title;
		wtitle (title);
		w0-> winboxcolor = save_title_atr;
		}

	get_user_data (formstart, workstart);
	print_values  (formstart, workstart);


	print_labels (formstart, NULL);


	free (workstart);


	return;		/* wprintform */
	}
















/*! print_labels ()
 *	local function to draw the all the labels in the form
 *	with the current choice highlighted,
 */
static  void 	W_NEAR print_labels ( WFORM *form, WFORM *mark )
	{

	/* loop through the form
	 */

	do	{
		if ( FARPTR_EQ (form, mark) )
			{
			print_label_line ( form, wfmatr_lbl_active);
			}
		else
			{
			print_label_line ( form, wfmatr_lbl );
			}
		}
	while ( (++form) ->wflabel != NULL );

	wsetattr (wfmatr_lbl);

	return;	/* print_labels */
	}

/* draw an individual label, in the indicated attribute.
 */
static void     W_NEAR    print_label_line ( WFORM *form, unsigned char attr )
	{
	int x;
	x= form->wfx -strlen(form->wflabel);
	if ( x < 0 )
		{
		x=0;
		}
	wgoto ( x, form-> wfy);

	wsetattr(attr);
	wputs (form->wflabel);
	return;		/* print_label_line */
	}



static void W_NEAR get_user_data ( WFORM *form, char *workarea )
	{


	/* loop through the form
	 */


	do	{
		if ( form->wfuser != NULL )
			{
			switch ( form ->wftype )
				{
			case ( 'c' ):
				*workarea = *((char *) (form->wfuser));
				*(workarea+1) =0;
				break;

			case ( 's' ):
				memcpy (workarea, ((char *)(form->wfuser)),
						form->wflen);

				break;

			default:
				numeric2workarea (form, workarea, form->wfuser);


				}  /* end switch */

			} /* end if user data */

		/* next item */
		workarea += form->wflen;
		++form;
		/* guarantee terminal NULL on last item
		 * (we've just moved past the area
		 */
		*(workarea -1) =0;
		}
	while ( form->wflabel != NULL );


	return; /* get_user_data */
	}


static void W_NEAR numeric2workarea (WFORM *form, char *workarea, void *data )
	{
	char scratch[80];


	switch ( form->wftype )
		{
	case ( 'f' ):
	case ( 'e' ):
	case ( 'g' ):
	case ( 'E' ):
	case ( 'G' ):
		sprintf (scratch, form->wformat,
			*((float*)(data)) );
			break;
	case ( 'i' ):
	case ( 'd' ):
		sprintf (scratch, form->wformat,
				 *((int*)(data)) );
		break;
	case ( 'u' ):
	case ( 'x' ):
	case ( 'X' ):
	case ( 'o' ):
		sprintf (scratch, form->wformat,
			*((unsigned int*)(data)) );
		break;
		}
		/* end swicth */

	memcpy (workarea, scratch, form->wflen);
	*(workarea + form->wflen -1) =0;

	return;  /* numeric2workarea */
	}


	/* loop through the form, print all user values onscreen.
	 */
static void W_NEAR print_values (WFORM *form, char *workarea )
	{
	do	{
		if ( form->wfuser != NULL)
			{
			print_single_value ( form, workarea );
			
			wputc(';');	/* mark end of field */

			workarea += form-> wflen;
			}
		}
	while ( (++form) ->wflabel != NULL );

	return; 	/* print_values */
	}

/* print a single line of data, clear field that follows.
 */
static void W_NEAR print_single_value ( WFORM *form, char *workarea )
	{
	wgoto (form->wfx, form-> wfy);
	wputfl (workarea, (form->wflen) );

	return;		/* print_single_value */
	}



static void W_NEAR setup_window
		(WFORM *formstart, WFORM **formlast,
		 char **workstart, int *worksize, char **worklast)

	{
	int left, top, width, height;
	char *buffer;
	int buffsize;

	WFORM *item;





	/* scan the form table, calculate the size of the window
	 *	and the size of the work area
	 * Place the data type (single char) into wfreserved for later use.
	 *
	 */
	buffsize = 0;
	for (item = formstart;  item->wflabel != NULL; ++item   )
		{
		if (item->wfuser == NULL)
			{
			/* fields with no data, only labels */
			item->wflen =0;				/* prevent errors later */
			}
		else	{
			buffsize += item->wflen;
			}
		/* check data type against supported types,
		 * move single char type into form table
		 */
		item->wftype = *(item->wformat +
				strcspn ( item->wformat, "csidfegEGouxX") );


		_NORMALIZE ( (item->wfpick) );


		} /* end for... scanning table ... */

	*formlast = item -1;	/* last line before NULL line. */




	buffer 	 = wmalloc (buffsize, "form input");


	/* setup window location
	 */


	width   = item     ->wfx;	/* last entry gives lower/right */
	height  = item 	   ->wfy;


	width   = max ( width, MIN_WIDTH );
	width   = min ( width, wxabsmax-2 );	/* fits onscreen */


	wlocate ( &left, &top, width, height );


	wopen   ( left, top, width, height,
		wfmatr_lbl, wfm_border_type, wfmatr_border, WSAVE2RAM );


	/* point to last string in the workarea
	 * find item by going to the end of the workarea
	 * and the stepping backwards by the sizeof the last entry
	 */

	*worklast  = buffer +buffsize - ( (*formlast) ->wflen);


	/* setup other values in calling function's areas */

	*workstart = buffer;
	*worksize  = buffsize;

	return;	/* setup_window */
	}


/*-----------------END of WFORM.C--------------------------------------*/



