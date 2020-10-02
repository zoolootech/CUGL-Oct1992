/*! wgets
 *
 *
 *	get a string form the console, echo it,
 *	and allow simple editing and recall of prior strings
 *
 *
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



/* local function: inch() moves x,y position 1 forward
 *		   backup() moves x,y 1 position back.
 *		   curse () toggles pseudo_cursor on/off.
 *		   show_insert (ON/OFF) places 'I' in upper right corner
 *		   rewrite() rewrite text onscreen
 *			from current position to end of buffer, padding ' '
 *			keystroke_OK () tests for numeric input if required 
 */
static void W_NEAR inch(int *px, int *py);
static void W_NEAR backup ( int *px, int *py );
static void W_NEAR curse (int x, int y);
static void W_NEAR show_insert (void);
static void W_NEAR  rewrite ( char *buffer, int x, int y, int endx, int endy );

static int  keystroke_OK ( int ch, char behavior, char *already_have );
	#define HAVE_DEC	0x01
	#define HAVE_SOME	0x02 

static  int 	W_NEAR insert_mode = 0;


int wgets(int nlimit, char *user_area, char behavior)
	{
	char *buffer, *buffer_start, *terminal_null;
	unsigned char save_style, save_flag;
	
	char already_have =0;		/* keep track of signs and points */

	char doing_form = behavior & WGETS_FORM;
	
	int 	x, y, startx, starty, endx, endy, n;
	int 	ch;
	char 	*ptr;
	int 	more =1;
	int 	ms_offset;	/* mouse offset from start of string */

	if ( nlimit <= 0)
		{
		return (ESCAPE);
		}

	/* clean up user string address and contents after end of valid string
	 * This makes sure that string is terminated by NULLs
	 * and remainder of buffer is also NULL (prevent garbage after CTRL-O)
	 */
	_NORMALIZE (user_area);		/*model-dependent*/
	for (n = strlen(user_area); n < nlimit; ++n )
		{
		user_area[n] = 0;		/*stuff 0's into remainder of string ver 1.1*/
		}

	if ( nlimit <= 0)
		{
		return (ESCAPE);
		}

	buffer_start = wmalloc ( nlimit, "wgets" );
	memcpy (buffer_start, user_area, nlimit);

	buffer = buffer_start;
	terminal_null = buffer_start + nlimit -1;

	save_style = w0->winputstyle;
	save_flag  = w0->winflag;
	startx = x = wherex();
	starty = y = wherey();


	/* figure # lines as intermediate to figuring stop postions onscreen
	 */
	n    = w0-> winxmax+1;             	/* bytes per line     */
	endy = nlimit / n;			/* # times line wraps */
	endx = x + (nlimit-(n*endy)) -1;	/* add in remainder   */

	if ( endx >= n )
		{
		/* beyond end of line
		 */
		endx -= n;
		endy++;
		}
	endy += y;				/* add #wraps to start*/




	if ( wmode == 'T' )
		{
		wcursor(OFF);
		}


	/* turn on wrap and scroll, turn ANSI chars off
	 */
	w0->winputstyle =  ( WPUTWRAP | WPUTSCROLL );

	show_insert ();


	rewrite ( user_area, startx, starty, endx, endy );


	while (more) {
		wgoto (x,y);

		/* toggle cursor on */
		curse (x, y);

		ch = wgetc();

		/* toggle cursor off */
		curse (x, y);

		if ( ch == MOUSE &&  (wmouse.wms_used & WMS_LEFT_RLS) )
			{
			if ( wmouse.wms_inwindow )
				{
				/* convert mouse x,y to new position
				 */

				/* compute byte offset from mouse
				 * to start of string
				 *
				 * NOTE mult (#lines below start) * linelen
				 */
				ms_offset = (wmouse.wms_y - starty)
						* (1+ w0-> winxmax )
					    + ( wmouse.wms_x - startx );

				/* see if mouse points inside the string.
				 */
				if ( ms_offset  >= 0
				   && ms_offset <= strlen(buffer_start)
				    )
					{
					/* mouse points inside string
					 */
					buffer = buffer_start + ms_offset;
					x      = wmouse.wms_x;
					y      = wmouse.wms_y;

					if ( ms_offset == 0 )		/* see if we're on first char */
						{
						already_have &= ( 0xff - HAVE_SOME );	
						}
					else
						{
						already_have |= HAVE_SOME;
						}
					}
				else		/* mouse outside this string, inside window */
					{
					if ( doing_form )	/* move to another form position */
						{
						more =0;
						}
					}


				}	/* end of mouse inside window */

			else
				{
				/* mouse outside of window -- see if INSERT clicked*/
				if ( wmouse.wms_xabs == 1+ w0->winleft + w0->winxmax
				 && wmouse.wms_yabs == w0->wintop -1 )
					{
					/* clicked on the insert indicator */
					wungetc (INSERT);

					}

				}

			}	/* end if char = MOUSE */

		else
		if (isascii (ch) && isprint(ch))		/* printable, not control code*/
			{
			if (  keystroke_OK ( ch, behavior, &already_have ) )
				{
				if (insert_mode)
					{
					/* push old characters back*/
					for (	ptr = terminal_null -1;
						FARPTR_GT (ptr, buffer);
						--ptr
						)
						{
						*(ptr) = *(ptr-1);
						}
					*buffer = ch;
					wputs ( buffer );		/* write new remaining portion */
					}
				else
					{
					/* overstrike mode -see if we're typing over a '.' 
					 */
					if ( *buffer == '.' )  
						{
						already_have &= ( 0xff - HAVE_DEC );	/* removal */
						}
					*buffer = ch;
					wputc ( ch );			/* write new single char   */
					
					}
				/* no need to clear rest of line,
				 * as inserting writes 1 byte more.
				 */
				inch(&x,&y);
				++buffer;

				already_have |= HAVE_SOME;
				}
			}
		else	/* not mouse or printable, must be keypad... */
		switch 	(ch) {

		case    (ENTER):
			more =0;
			break;

		case	(RIGHT_ARROW):
			/* wants next letter in old line*/
			ptr = buffer +1;
			if (FARPTR_LT (ptr, terminal_null) )
				{
				buffer = ptr;		/* advance */
				inch(&x,&y);
				
				if ( *ptr == 0 )		/* we're at end of a short string */
					{
					*(ptr+1) =0;		/* make sure there's a \0 here */	
					}
				
				already_have |= HAVE_SOME;
				
				}
			break;


		case	(CTRL('S')):				/* save data for form entry */
			if ( doing_form )
				{
				more = 0;
				} 
			break;
			
			
		case	(LEFT_ARROW):    /*back up cursor*/
		case 	(BACKSPACE):
			if ( FARPTR_GT (buffer, buffer_start) )
				/*byond start */
				{
				backup ( &x, &y );
				--buffer;
				if ( FARPTR_EQ (buffer, buffer_start ) )
					{
					already_have &= ( 0xff - HAVE_SOME );	/* on first ltr*/
					}
				if (ch == BACKSPACE)
					{
					/* rubout prev char -- first see if we're removing a '.' 
					 */
					if ( *buffer == '.' )  
						{
						already_have &= ( 0xff - HAVE_DEC );	/* removal */
						}
					/*remove prior character from buffer and slide leftward
					 */
					for (ptr =buffer;  
						FARPTR_LT (ptr, terminal_null);
						ptr++
						) 
						{
						*(ptr) = *(ptr+1);
						}
					/* rewrite remainder of string
					 */
					wgoto (x,y);
					rewrite (buffer,x,y,endx,endy);

					}

				}

			break;

		case 	(TAB):
		case  	(UP_ARROW):
		case 	(DN_ARROW):
		case	(BACKTAB):      /*request move to another field */

			if ( doing_form )
				{
				more = 0;
				}

			break;

		case 	(INSERT):
			insert_mode ^= 1; 	/* toggle (XOR) */
			show_insert ();
			break;

		case	(DELETE):	/* delete 1 from old buffer */
			
			if ( *buffer == '.' )		/* are we deleting a '.' ??? */  
				{
				already_have &= ( 0xff - HAVE_DEC );	/* removal */
				}
	
			/* slide everything after this one char to left
			 */
			for ( ptr =buffer;
			      FARPTR_LT (ptr, terminal_null);
			      ++ptr )
				{
				*(ptr) = *(ptr+1);
				}
			rewrite (buffer,x,y,endx,endy);
			break;

		case	(ESCAPE):
			/* request to quit */
			more =0;
			

		case (HOME):
			wgoto (startx, starty);
			x=startx;
			y=starty;
			buffer = buffer_start;
			break;
		
		case (END):
			while ( FARPTR_LT (buffer, terminal_null) )
				{
				++buffer;
				inch(&x,&y);
				if ( *buffer == 0 )		/* moving to last char in nonfull buff*/
					{
					*(buffer+1) = 0;
					break;				/* break from while */
					}	
				}
			wgoto (x,y);
			break;
		
		case ( CTRL_END ):
		case ( CTRL('Y') ):
			/* eliminate the remainder of the buffer
			 */
			*buffer = 0x0;
			rewrite (buffer,x,y,endx,endy);
			break;

		case ( CTRL('O') ):
			/* replace buffer with original contents
			 */
			memcpy (buffer_start, user_area, nlimit);
			x = startx;
			y = starty;
			wgoto (x,y);
			buffer = buffer_start;
                        rewrite (buffer,x,y,endx,endy);
			if ( doing_form )
				{
				more =0;		
				}

		}  /*end switch (ch) */

		if ( FARPTR_EQ (buffer, terminal_null) )
			{
			backup ( &x, &y );
			--buffer;
			
			if ( behavior & WGETS_SKIP )	/* test for autoskip at end */
				{
				more = 0;
				
				if ( doing_form )
					{
					/* in form mode, return ENTER to move to next field,
					 * and wungetc() to reuse the ch as first char in field.
					 */
					wungetc (ch);
					ch = ENTER;
					}
				}
			}


		}  /*end while - no more room or string terminated by user */



	/* wrapup */


	w0->winputstyle = save_style;
	w0->winflag     = save_flag;

	if ( w0->winflag & WFL_CURSOR )
		{
		wcursor (ON);
		}

	if ( insert_mode )
		{
		insert_mode = OFF;
		show_insert ();			/* erases onscreen 'I' marker */
		insert_mode = ON;
		}



	if ( ch != ESCAPE )
		{
		memcpy (user_area, buffer_start, nlimit);
		}
	else 	{
		/* ESCAPE means reject the newly typed values
		 * need to replace changed values onscreen with originals.
		 */
		wgoto ( startx, starty );

		rewrite ( user_area, startx, starty, endx, endy );
		}

	free (buffer_start);

	return(ch);
	} /*end of wgets*/



static void W_NEAR inch (int *px, int *py)
	{

	register int x,y;
	x = *px;
	y = *py;

	if ( ++x > w0->winxmax )
		{
		x =0;
		if (++y > w0->winymax )
			{
			--y;
			wgoto(x,y);
			wscroll();
			}
		}

	*px =x;
	*py =y;

	return;	/* inch*/

	}

static void W_NEAR show_insert ()
	{
	unsigned char ch, at;




	if ( ! w0->winbox )
		return;

	at = w0->winboxcolor;

	if ( insert_mode )
		{
		ch = 'I';
		at |= BLINK;
		}
	else
		{
		ch = wbox [w0->winbox].ne;
		}


	wputcabs( w0->winleft  + w0->winxmax +1,
		  w0->wintop   -1,
		  ch, at,
		  WGOVERWRITE);

	return; 	/* show_insert */
	}


static void W_NEAR curse (int x, int y)
	{
	unsigned char new_state;




	#ifdef  TEXTONLY
		new_state = (w0-> winflag & WFL_CURSOR ) ?
						 OFF  : ON ;

		wcursor (new_state);



	#else	/* not TEXTONLY */

		if ( wmode == 'G' )
			{
			wputcabs (w0->winleft + x, w0->wintop +y,
			' ', 0xf0, WGXOR );
			}
		else
			{
			new_state = (w0-> winflag & WFL_CURSOR ) ?
						 OFF  : ON ;
			wcursor (new_state);
			}

	#endif	/* not TEXTONLY */


	return; 	/* curse () */

	}





static void W_NEAR backup ( int *px, int *py )
	{
	register int x = *px;
	register int y = *py;


	if (--x < 0)  {  /*at start of line*/
		/* goto end of last line */
		x = w0-> winxmax ;
		if (--y < 0)
			{
			x= 0;	/*start of window*/
			y= 0;
			}
		}

	*px =x;
	*py =y;

	return;	/* backup */
	}

/* write string to screen and pad remainder of area with ' '
 */

static void W_NEAR  rewrite ( char *buffer, int x, int y, int endx, int endy )
	{
	register int nleft;
	
	nleft =   (w0->winxmax+1)*(endy-y)  + (endx-x);
	wputfl ( buffer, nleft );		/* fixed len string put */


	return;		/* rewrite */
	}


static int  keystroke_OK ( int ch, char behavior, char *already_have )
	{
	int ch_OK = 1;	
	char temp_have = *already_have;
	
	if ( behavior & WGETS_INT )
		{
		if 	( ! isdigit (ch) )  
			{
			if ( ch== '.' 			&& ! (temp_have & HAVE_DEC) )
				{
				temp_have |= HAVE_DEC;
				}
			else
			if ( ! ( (ch=='-' || ch=='+') && ! (temp_have & HAVE_SOME)  )   ) 
				{
				ch_OK = 0;
				}
			}
		}
	
	*already_have = temp_have;
	
	return  ch_OK;
	}





/*------------------------- end of WGETS.C ------------------------------*/

