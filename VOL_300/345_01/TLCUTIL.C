/* TLCUTIL.C - "The Last Cross-referencer" - General utility routines	*/
/*	Last Modified:	02/10/89											*/

/*
---------------------------------------------------------------------
Copyright (c) 1987-1989, Eduard Schwan Programs [esp] - All rights reserved
TLC (The Last C-Cross-Referencer) and TLP (same, but for Pascal) are
Cross-Reference Generators crafted and shot into the Public Domain by
Eduard Schwan.  The source code and executable program may be freely
distributed as long as the copyright/author notices remain intact, and
it is not used in part or whole as the basis of a commercial product.
Any comments, bug-fixes, or enhancements are welcome.
Also, if you find TLC and it's source code useful, a contribution of
$20 (check/money order) is encouraged!  Hopefully we will all see more
source code distributed!
	Eduard Schwan, 1112 Oceanic Drive, Encinitas, Calif. 92024
---------------------------------------------------------------------
*/

/*
HEADER:		The Last Cross-Referencer;
TITLE:		TLC/TLP - The Last Cross-Referencer;
VERSION:	1.01;

DESCRIPTION: "TLC/TLP.
			General utility routines";

KEYWORDS:	Utility, Cross-reference, C, Pascal, Apple, Macintosh, APW, Aztec;
SYSTEM:		Macintosh MPW, v3.0;
FILENAME:	TLCUTIL.C;
WARNINGS:	"Has not yet been ported to MS-DOS.
			Shareware, $20 Check/Money Order suggested.";

SEE-ALSO:	README.TLC,TLCHELP.DOC,TLPHELP.DOC;
AUTHORS:	Eduard Schwan;
COMPILERS:	AZTEC C65 v3.2b, APPLEIIGS APW C v1.0, APPLE MACINTOSH MPW C v3.0;
*/


/*-------------------------- include files ---------------------------*/

#include	<stdio.h>
#include	<errno.h>
#include	<memory.h>
#include	<ctype.h>
#ifdef macintosh
#include	<osutils.h>	/* for date/time routines */
#endif
#include	"tlc.h"


/*------------------------ external declarations ----------------------*/

#include	"tlc.ext"
#ifdef AppleIIgs
#include <shell.h>	   /* this has STOP() macro for check_user_abort() fn */
#endif

#ifdef AppleIIgs
extern long 	time();  /* these are in 2/time.lib in APW */
extern char *	ctime();
#endif


/*--------------------------- definitions ----------------------------*/

#ifndef isdigit
#define 	isdigit(c)		((c) >= '0' && (c) <= '9')
#endif


/*-------------------------- static variables ------------------------*/

static char ctime_str[DATE_SIZE]; /* ASCII date/time buffer for ctime() */

static char * week_days[] =
	{
	"Sun",
	"Mon",
	"Tue",
	"Wed",
	"Thu",
	"Fri",
	"Sat"
	};


/*---------------------------- functions -----------------------------*/


/*=================[ open_text_file ]===================*/

FILE * open_text_file(fname, options, error_ptr)
char*	fname;
char*	options;
int*	error_ptr;

	{ /* open_text_file() */
	FILE * fp;

	errno = 0;
	fp=fopen(fname, options);
	*error_ptr = errno;
debug(printf("open_text_file:fname='%s,%s' err=%x\n",fname,options,errno);)
	return(fp);
	} /* open_text_file() */



/*=================[ close_text_file ]===================*/

int close_text_file(fp, fname)
FILE*	fp;
char*	fname; /* NOTE: so far, only needed for debug */

	{ /* close_text_file() */
	int	err;
	
	errno = 0;
	fclose(fp);
	err = errno;
debug(printf("close_text_file:'%s' errno=%d\n",fname,err);)
	return(err);
	} /* close_text_file() */



/*=================[ do_emphasis ]===================*/

VOID do_emphasis(emph_type)
byte	emph_type;

	{ /* do_emphasis() */
	char * leadin;

	if (emph_type != EMPH_NONE)
		{ /* do some kind of leadin */
		if (emph_type >= EMPH_MAX)
			{ /* out of range */
			fprintf(stderr,
				"Error! DoEmphasis (%d) out of range!\n",emph_type);
			exit(1);
			}
		else
			{ /* send leadin sequence */
			emph_type--; /* 1..MAX ==> 0..MAX-1 for array indexing */
			leadin = emph_array[emph_type].emph_leadin;
			fprintf(out_file, leadin);
debug(printf("do_emph[%d] = '%s'\n",emph_type,leadin);)
			}
		}
	} /* do_emphasis() */



/*=================[ undo_emphasis ]===================*/

VOID undo_emphasis(emph_type)
byte	emph_type;

	{ /* undo_emphasis() */
	char * leadout;

	if (emph_type != EMPH_NONE)
		{ /* do some kind of leadin */
		if (emph_type >= EMPH_MAX)
			{ /* out of range */
			fprintf(stderr,
				"Error! UndoEmphasis (%d) out of range!\n",emph_type);
			exit(1);
			}
		else
			{ /* send leadout sequence */
			emph_type--; /* 1..MAX ==> 0..MAX-1 for array indexing */
			leadout = emph_array[emph_type].emph_leadout;
			fprintf(out_file, leadout);
debug(printf("do_emph[%d] = '%s'\n",emph_type,leadout);)
			}
		}
	} /* undo_emphasis() */



/*==================[ do_form_feed ]====================*/
/* WARNING! incremented line_num not passed back if !has_form_feed*/
/* does this matter?.. */
VOID do_form_feed(out_file, line_num)
FILE * out_file;
int    line_num;
{ /* do_form_feed() */
	if (parm_rec.has_form_feed)
		{ /* do a true form feed to printer */
		putc(CH_FF, out_file);
		}
	else
		{ /* fake a form feed by advancing n more lines */
		while (line_num < parm_rec.page_lines)
			{
			putc('\n', out_file);
			line_num++;
			}
		}
} /* do_form_feed() */


/*=====================[ indent ]=======================*/

VOID indent()

{ /* indent() */
pos_int k;

debug(puts("indent:");)
for (k=1; k<parm_rec.left_column; k++)
   putc(' ', out_file);
} /* indent() */



/*==================[ get_time ]========================*/

VOID get_time(time_str)
char	*time_str;

	{ /* get_time() */
#ifdef AppleIIgs
	static int	time_rec[10];  /* buffer that time() uses for date/time */

	time(&time_rec[0]);
	strcpy(time_str, ctime(&time_rec[0]));
	/* eliminate trailing \n if there */
	if (time_str[strlen(time_str)-1] < ' ')
		time_str[strlen(time_str)-1] = '\0';
#else
#ifdef macintosh
	DateTimeRec	date_rec;
	
	GetTime(&date_rec);
	sprintf(time_str, "%s %2d/%02d/%02d  %2d:%02d",
			week_days[date_rec.dayOfWeek-1],
			date_rec.month, date_rec.day, date_rec.year,
			date_rec.hour, date_rec.minute);
#endif
#endif
	} /* get_time() */



/*==================[ get_freemem ]========================*/

long get_freemem()

	{ /* get_freemem() */
#ifdef AppleIIgs
	return FreeMem();
#else
#ifdef macintosh
	return FreeMem();
#endif
#endif
	} /* get_freemem() */



/*==================[ check_user_abort ]===================*/

VOID check_user_abort()

	{ /* check_user_abort() */
#ifdef AppleIIgs
	if (STOP())
#else
#ifdef macintosh
	if (FALSE)
#else
	if (keypressed())
#endif
#endif
		{ /* clean up and exit */
		fprintf(stderr,"\n\nUser Break Anticipated and Understood!  Bye..\n");
		fflush(stderr);
		close(out_file);
		exit(1);
		}
	} /* check_user_abort() */



/*=================[ ok_to_print ]===============*/

int ok_to_print()

	{ /* ok_to_print() */
/*debug(printf("okToPrint(): 1stPg=%d currPg=%d lastPg=%d\n",\
parm_rec.first_page, out_page_number, parm_rec.last_page);)*/
	if ((out_page_number >= parm_rec.first_page) &&
		(out_page_number <= parm_rec.last_page))
		return(TRUE);
	else
		return(FALSE);
	} /* ok_to_print() */



/*==================[ new_page ]===================*/

VOID new_page(do_output, who, page_ptr, line_ptr, file_name)
boolean		do_output;
char *		who;
pos_int *	page_ptr;
pos_int *	line_ptr;
char *		file_name;

	{ /* new_page() */

	/* next page, first line */
	(*page_ptr)++;
	*line_ptr = 0;

	if (ok_to_print() && do_output)
		{
		/* go to new page */
		do_form_feed(out_file, *line_ptr);
		}

	if (ok_to_print() && do_output)
		{
		/* do first line of heading */
		indent();
		do_emphasis(parm_rec.emph_heading);
		fprintf(out_file,"The Last Cross-referencer [%s]  %-6s      Page %u",
				TLC_VERSION, who, *page_ptr);
		if (file_name != NULL)
			fprintf(out_file,"  %s", file_name);
		undo_emphasis(parm_rec.emph_heading);
		putc('\n', out_file);
		}
	(*line_ptr)++;

	/* do second line of heading */
	if (ok_to_print() && do_output)
		{
		indent();
		do_emphasis(parm_rec.emph_heading);

		/* put date/time on line.. */
		if (strlen(ctime_str)==0)
			{
			get_time(ctime_str);
			}
		fprintf(out_file, ctime_str);

		/* put user heading on line if it exists */
		if (parm_rec.user_heading!=NULL)
			{
			fprintf(out_file, " -- %s", parm_rec.user_heading);
			}
		undo_emphasis(parm_rec.emph_heading);
		putc('\n', out_file);

		/* skip a line between heading and data lines */
		putc('\n', out_file);
		} /* if ok_to_print */
	(*line_ptr) += 2;

	} /* new_page() */



/*==================[ expand_str_chars ]===================*/

VOID expand_str_chars(str)
char *	str;

	{ /* expand_str_chars() */
	char		ch;
	pos_int 	accum_value, base, digit_count;
	char *		str2;

	/* start at beginning of string, and move string down as its converted */
	str2 = str;
	while ((ch=*str2) != '\0')
		{
		/* handle special escape characters? */
		if (ch == '\\')
			{
			ch = tolower(*++str2); /* get char after backslash */

			/* set base for possible octal/hex digit conversion */
			base = B_octal;
			digit_count = 3; /* three digits max */
			if (ch == 'x')
				{
				base = B_hex;
				digit_count = 2; /* two digits max */
				ch = tolower(*++str2); /* get 1st hex digit */
				}

			/* see if digits follow the backslash */
			if (isdigit(ch) || (ch>='a' && ch<='f' && base==B_hex))
				{ /* convert digits to a character value */
				accum_value = 0;
				do	{
					if (ch>='a' && ch<='f' && base==B_hex)
						ch -= 7; /* shift ASCII a..f down to 10..15 */
					accum_value = accum_value*base + (ch&0x0f);
debug(printf("expStr(): s='%s' val=%d\n",str2,accum_value);)
					ch = tolower(*++str2);
					digit_count--;
					} while (digit_count &&
						(isdigit(ch)||(base==B_hex && ch>='a' && ch<='f')));
				/* passed last digit, back up one for second increment */
				/* below, and put accumulated value in ch so that it   */
				/* will be put in the string at *str below..		   */
				str2--;
				ch = accum_value;
				}

			/* if it wasn't a '\nnn' sequence, just drop down & */
			/* copy the char after the '\' into string			*/
			}

		/* copy this character into the string & move on */
		*str++ = ch;
		str2++;
		} /*while*/

		/* terminate the string at the new end */
		*str = '\0';

	} /* expand_str_chars() */
