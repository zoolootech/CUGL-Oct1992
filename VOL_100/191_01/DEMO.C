/*#title DEMO.C    05/26/86 */
/****************************************************************
*****************************************************************
**                                                             **
**      file: DEMO.C	                                       **
**                                                             **
**      function:   Demonstrate W_FORM and W_CHATTR functions  **
**                                                             **
**      programmer: GVWoodley                                  **
**                                                             **
*****************************************************************
*****************************************************************
*/
 
#include "STD.H"
#include "C_WDEF.H"

#define	BEEPDEL		1000		/* beep delay */
#define LINE_LEN	31			/* line length */
 
extern char err_exit,	/* set to TRUE to return to dos on error */
			extend;		/* TRUE if extended char typed */

char		win_bg,		/* window background */
			bor_fg,		/* border foreground */
			bor_bg,		/* border background */
			text_fg,	/* text foreground */
			text_bg,	/* text background */
			get_fg;		/* color of text input by w_getstr() */
int			line_menu;
WORD		main_stop[7 * 2 + 1];	/* total of 7 stops */
/*#eject    */
/****************************************************************
    MAIN-LINE - Open LINE_MENU window,
				type a prompt message,
				get operator response,
				close window.
****************************************************************/
char main_line (msg_ptr)
char	*msg_ptr;					/* pointer to prompt message */
{
	char	cc;						/* console char */
	int		status;

	status = w_open (
				line_menu,
				main_stop[12], main_stop[13],	
	 							/* upper left-hand coords */
				CLR,			/* clear window */
				win_bg, 		/* window background */
				SNGL_LN,		/* border type */
				bor_fg, bor_bg);/* border colors */
	status = w_gotoxy (1, 0);
	w_write (msg_ptr);

	cc = toupper(keyin());
	w_close();						/* close line_menu window */

	return (cc);
	}






/****************************************************************
    MAIN PROGRAM
****************************************************************/
main (argc, argv)

WORD	argc;			/* count of arguments */
char	**argv;			/* pointer to argument pointers */

{
	BYTE	cc;			/* console char */
	BOOLEAN	success,	/* general-purpose success */
			done;		/* done with program */
	int		status,		/* status of window functions */
			main_menu;
	WORD	base,		/* base index */
			old_base;
/*#eject    */
/* initialize */
	old_base = 0;
	win_bg = NORM;
	bor_bg = NORM;
	bor_fg = REVERS;
	text_fg = NORM | BRIGHT;
	text_bg = REVERS;
	get_fg = REVERS | BRIGHT;

	w_init();					/* init C_WINDOW package */
/* define our windows */
	main_menu = w_def (78, 23, NOBORDER);
	line_menu = w_def (39, 3, BORDER);

	status = w_open (
				main_menu, 1, 1,	/* upper left-hand coords */
				CLR,				/* clear window the 1st time */
				win_bg, 			/* window background */
				XX,					/* border type */
				XX, XX);			/* border colors */
	status = w_form (
				0, 0,
				"demo.frm",			/* form name */
				8,					/* tab interval */
				main_stop);			/* stop table ptr */
/*#eject */	
/****************************************************************
    MAIN LOOP
****************************************************************/
	done = FALSE;
	while (!done)  {
/* mainmenu is being displayed */
		status = w_gotoxy (main_stop[12], main_stop[13]);
		cc = toupper(keyin());

/* here for selections 1 - 6 */
		base = (cc - '1') * 2;
		if (base <= 10)  {
	/* display the old line in normal video */
			w_chattr (main_stop[old_base], main_stop[old_base + 1],
				bor_bg, LINE_LEN);
	/* display the new line in reverse video */
			w_chattr (main_stop[base], main_stop[base + 1],
				text_bg, LINE_LEN);
			old_base = base;			/* update for next time */
			}

/* here if selection = X */
		else if (cc == 'X')  {
			if (main_line ("Are you sure? ") == 'Y') 
				done = TRUE;
			}

/* illegal selection */
		else
			beep (BEEPDEL);
		}
/* clean up & leave */
	w_close();
	exit (0);		/* normal exit */
	}
