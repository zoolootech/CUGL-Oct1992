/*#title 'W_FORM    04/18/86' */
/****************************************************************
**                                                             **
**      file: W_FORM.C	 									   **
**                                                             **
**      library function:   w_form                             **
**                                                             **
**      programmer: George Woodley                             **
**                                                             **
*****************************************************************

    NAME
        w_form - Fill a c_window with a form from a file.

    SYNOPSIS
        success = w_form (x, y, fname_ptr, tab_int, stop_ptr);
			WORD success,			1 if success, 0 if error.
				 x, y;				Initial upper left-hand
					 				coordinates in writeable
					 				portion of window.
			char *fname_ptr;		Pointer to an ASCII file.
			WORD tab_int,			Tabs are set every TAB_INT chars.
				 *stop_ptr;			Pointer to a table filled in by 
					 				W_FORM.

    DESCRIPTION
        W_FORM takes an ASCII file as its input and fills a 
		Chris McVicar C_WINDOW with its contents.  The file must
		consist of lines (max 80 chars) of the following chars:
		* Normal chars are just printed.  If the result exceeds
		  window dimensions, chars will be wrapped around or
		  scrolled.
		* Tabs are expanded into spaces as directed by the TAB_INT
		  parameter.
		* LF's result in output of the line to the window.
		* CR's are discarded.
		* Extended ASCII chars may be used to form borders (or the
		  normal C_WINDOW border functions may be used).
		* The special char € (code 128 or 0x80) may be used to mark
		  a stop in the form.  The x, y coordinates of this char will
		  be stored in the table pointed to by STOP_PTR for each
		  occurrence of the char.  The table will be terminated by 
		  x = -1 (0xFFFF).  This char will be translated to a space
		  during display.
		* The char Control-Z (0x1A), if used, marks End-of-file.

		Thus, a form can be easily created using an editor and put
		up using W_FORM and the C Windowing Toolbox software.

    CAUTIONS
        Allocate two words for every occurrence of a stop char, plus
		one word for the sentinel.  Allocate at least one word.

		A line may not exceed 80 chars.
****************************************************************/
/*#eject */

#include "STD.H"
#include "C_WDEF.H"
#include "STDIO.H"
#include "STDLIB.H"


#define HT		0x09		/* horizontal tab */
#define SUB		0x1A		/* control-Z (EOF)*/
#define STOP	(BYTE)128	/* marks a stop */

static	BYTE file_buff[512];		/* file buffer */

/****************************************************************
    W_FORM function
****************************************************************/
WORD w_form (x, y, fname_ptr, tab_int, stop_ptr)

WORD x, y;
char *fname_ptr;
WORD tab_int, *stop_ptr;

{
	char	*buff_ptr,	/* pointer to line buffer */
			buff [84];	/* line composition buffer */
	BYTE	fc;			/* char read from file */
	BOOLEAN	ldone,		/* done with line */
			done;		/* done with file */
	WORD	count,		/* count of chars/line */
			f_ix,		/* file_buff index */
			fix_max,	/* max value of f_ix */
			success;	/* success of function */
	FILE	*handle;	/* file handle */

/* Attempt to open file in translated mode */
	if (!(handle = fopen (fname_ptr, "rt")))  {
		w_msg ("W_FORM - file not found");
		return (FALSE);			/* file does not exist */
		}
/* Read in the first buffer */
	if (!(fix_max = fread ((char *)file_buff, sizeof(BYTE), 
						   sizeof(file_buff), handle)))  {
		fclose (handle);
		w_msg ("W_FORM - file empty");
		return (FALSE);			/* file is empty */
		}

	f_ix = 0;
	success = TRUE;
	done = FALSE;

/* here for every line */
	while (!done)  {
		buff_ptr = buff;
		count = 0;
		ldone = FALSE;
/*#eject */
	/* here for every char */
		while (!ldone)  {
	/* check if file_buff has been processed */
			if (f_ix == fix_max)  {
				fix_max = fread ((char *)file_buff, sizeof(BYTE), 
							     sizeof(file_buff), handle);
				f_ix = 0;
				}
			fc = file_buff [f_ix++];

			if (!fix_max || (fc == SUB))
				ldone = done = TRUE;		/* if end-of-file */
			else {
				if (fc == STOP) {			/* if stop code */
					*stop_ptr++ = x + count;
					*stop_ptr++ = y;
					fc = ' ';				/* replace w space */
					}

				switch (fc) {		   /* dispatch on special char */

				case CR:			   /* carriage return */
					break;			   /* just in case we see one */

				case LF:				/* line-feed */
					*buff_ptr = '\0';			/* terminate line */
					ldone = TRUE;		
					break;

				case HT:				/* horizontal tab */
					if (count <= (80 - tab_int)) {
						*buff_ptr++ = ' ';
						count++;
						while (count % tab_int) {
							*buff_ptr++ = ' ';
							count++;
							}
						}
					break;

				default:				/* all other chars */
		/* store normal char in buffer */
					if (count <= 80) {	 /* do not overrun buffer */
						*buff_ptr++ = fc;
						count++;
						}
					}
				}
			}				/* end of char loop */
/*#eject */
	/* send assembled line to the window */
		if (count && !done)  {
			if (!(success = w_gotoxy (x, y)))
				done = TRUE;
			else
				w_write (buff);
			}
		y++;				/* advance row for each line */
		}					/* end of line loop */

/* close the file & return */
	*stop_ptr = 0xFFFF;		/* terminate stop table */
	fclose (handle);
	return (success);
	}
