/*
 * g r a f t e x t . c
 * -------------------
 * This is a hardware-independant, high-level part of the compatible
 * graphics library.
 * This module implements the text-handling functions.
 * The standard font is defined in "stdfont.h".
 *
 * release history
 * ---------------
 * May, 28. 1987    Begin development
 * Jun, 17. 1987    As support to the graphics library error system added
 *                  function "pgrerror", which is like perror but prints
 *                  errors occuring in the graphics library. It is included
 *                  in this module, because it needs text output functions.
 * Sep, 13. 1987    Implemention of the different write modes.
 *
 * Written by       Rainer Gerhards
 *                  Petronellastr. 6
 *                  D-5112 Baesweiler
 *                  West Germany
 *                  Phone (49) 2401 - 1601
 */

#include <stdio.h>
#include <string.h>
#include <dos.h>
#include <math.h>

#include "graphics.h"
#include "stdfont.h"

#ifdef	USEPROTT
	extern void	fputmsg(int, int, int, int, char *, int, int);
#endif

/*
 *	auxiluary function set for use with write modes other than WM_NORM
 */

/*
 * name          fputmsg
 *
 * synopsis      putmsg(x, y, foreg, backg, string, font, mode);
 *               int x, y;            starting coordinate (upper left corner)
 *               int foreg;           foreground color
 *               int backg;           background color
 *               char *string;        string to draw
 *               int font;            font-number to use
 *               int mode;            output mode (font modifier)
 *
 */
static void fputmsg(x, y, foreg, backg, string, font, mode)
int x,y;
int foreg;
int backg;
char *string;
int font;
int mode;
{
#define chrwidth 8 /* character width, STDFONT = 8 pixel  */
#define chrlen   8 /* character length, STDFONT = 8 pixel */

register unsigned bitmask;		/* to mask current font-bit	*/
register int xt;			/* x-offset in character	*/
register int yt;			/* y-offset in character	*/
register int modemodi;			/* x-modifier for selected mode	*/
register int cx, cy;			/* current x, y			*/

for( ; *string ; string++, x += chrwidth)	/* process string	*/
    for(yt = 0 ; yt < chrlen ; ++yt)		/* one char. line	*/
      for(  cy = y + yt, xt = chrwidth, bitmask = 0x01	/* pixel in this line	*/
          ; xt >= 0
          ; --xt, bitmask <<= 1)
        {
        switch(mode)
          {
          case OM_ITAL : /* italics */
                         modemodi = 8 - yt;
                         break;
          case OM_RITAL: /* reverse italics */
                         modemodi = yt;
                         break;
          default      : /* if mode is unknown, use OM_NORM */
                         modemodi = 0;
                         break;
          }
	curcolor = (stdchrst[(*string << 3) + yt] & bitmask) ? foreg : backg;
	cx = x + xt + modemodi;
        setpixel(cx, cy, selcolor(cx, cy));
        }
}


/*
 *	main function set, called by user program
 */


/*
 * name          putmsg
 *
 * synopsis      putmsg(x, y, foreg, backg, string, font, mode);
 *               int x, y;            starting coordinate (upper left corner)
 *               int foreg;           foreground color
 *               int backg;           background color
 *               char *string;        string to draw
 *               int font;            font-number to use
 *               int mode;            output mode (font modifier)
 *
 * description   This function draws the string pointed to by "string" on the
 *               graphics screen. The string must be \0-terminated. Drawing
 *               begins at x,y, which is the upper left corner of the first
 *               character in the string. The characters are painted in color
 *               foreg, the character-background is painted in color backg.
 *               The font to use must be specified in font. Currently only
 *               STDFONT is supported. Field mode contains the string output
 *               mode, which is a font modifier. This field may be used to
 *               select italics or other output modes. Currently only
 *               OM_NORM and OM_ITAL are supported.
 *
 * warning	 Although this function supports the write mode system, only
 *		 very little write modes makes sense. In inverting mode, e. q.
 *		 the whole character is inverted and as thus invisible!
 *		 I think you should switch to WM_NORM before doing any text-io.
 */
#ifdef USEVOID
void
#endif
putmsg(x, y, foreg, backg, string, font, mode)
int x,y;
int foreg;
int backg;
char *string;
int font;
int mode;
{
#define chrwidth 8 /* character width, STDFONT = 8 pixel  */
#define chrlen   8 /* character length, STDFONT = 8 pixel */

register unsigned bitmask;        /* to mask current font-bit */
register int xt;                  /* x-offset in character */
register int yt;                  /* y-offset in character */
register int modemodi;            /* x-modifier for selected mode */

if(writemod != WM_NORM)
  fputmsg(x, y, foreg, backg, string, font, mode);
else
  {
  #ifdef EGAGRAF
  setewm2();
  #endif
  #ifdef HERCGRAF
  foreg = foreg & 1;
  #endif
  for( ; *string ; string++, x += chrwidth)    /* process string */
    #if (HERCGRAF || EGAGRAF) /* && */
    if((mode == OM_NORM) && (chrwidth == 8) && !(x & 0x0007))
      for(yt = 0 ; yt < chrlen ; ++yt)
	#if EGAGRAF
	  {
	  /* I now overwrite the wohle character line with the
	   * background color, and then write the character line itself
	   * in foreground color. You may also clear the background with
	   * only (~charcter_line), but there seems to be no speed
	   * difference. So I use the first method, because it needs one
	   * word less stack-space (pick up character line only once!).
	   */
	  setbyte(x, y + yt, backg, 0xff);
	  setbyte(x, y + yt, foreg, stdchrst[(*string << 3) + yt]);
	  }
	#else
	  setbyte(x, y + yt, foreg ? stdchrst[(*string << 3) + yt]
				   : ~stdchrst[(*string << 3) + yt]);
	#endif
    else
    #endif
      for(yt = 0 ; yt < chrlen ; yt++)           /* one char. line */
	for(  xt = chrwidth, bitmask = 0x01      /* pixel in this line */
	    ; xt >= 0
	    ; --xt, bitmask <<= 1)
	  {
	  switch(mode)
	    {
	    case OM_ITAL : /* italics */
			   modemodi = 8 - yt;
			   break;
	    case OM_RITAL: /* reverse italics */
			   modemodi = yt;
			   break;
	    default      : /* if mode is unknown, use OM_NORM */
			   modemodi = 0;
			   break;
	    }
	  #ifdef EGAGRAF   /* select set-pixel function */
	  qsetpix
	  #else
	  setpixel
	  #endif
		  (x + xt + modemodi, y + yt,
		  (stdchrst[(*string << 3) + yt] & bitmask) ? foreg : backg);
	  }
  #ifdef EGAGRAF
  rsestdwm();
  #endif
  }
}


#ifdef USEVOID
void
#endif
grperror(fil, msg)
FILE *fil;
char *msg;
/*
 * name          grperror
 *
 * synopsis      grperror(fil);
 *               FILE *fil;      file to put error-message in (or NULL if to
 *                               put on current display).
 *               char *msg;      error-message header text (like perror).
 *
 * description   This function prints the last detected error that occured
 *               in the graphics library.
 *               It has the ablety to write to a user text file or to the
 *               current graphics screen. Writing to a file may be usefull
 *               for not destroying your graphics display. You chose between
 *               this both modes by your file pointer: if it is NULL, the
 *               function writes to the display, otherwise it uses your file
 *               pointer to write to that file.
 *               No write errors are reported.
 *               The function writes first the user-specified message, than
 *               the internal error-message.
 */
{
register int nxtpos;      /* next output position */
register int errind;      /* error index, used to index err_msgs[] */
static char *err_msgs[] = {
                          #if (COUNTRY == 1) || (COUNTRY == 44)
                            "no error occured.",
                            "fatal but unexplainable error.",
                            "no or not enough coordinates given.",
			    "wrong write mode requested",
                            "error number out of range."
                          #endif
                          #if (COUNTRY == 41) || (COUNTRY == 49)
                            "kein Fehler aufgetreten.",
                            "allgemeiner fataler Fehler.",
                            "keine oder nicht genuegend Koordinaten gegeben.",
			    "flaschen Schreibmodus angefordert",
                            "Fehlernummer ausserhalb Wertbereich."
                          #endif
                          };
#define MAXERNUM (sizeof(err_msgs) / sizeof(char *) - 1 - 1)
/* highest valid error number (num. messages - out of range message) */

errind = (gr_error > MAXERNUM) ? MAXERNUM : gr_error;
if(fil == NULL)  /* where to write? */
  {              /* to graphics screen */
  putmsg(1+X_LOW, Y_LOW, BLACK, WHITE, msg, STDFONT, OM_NORM);
  nxtpos = strlen(msg) * chrlen;
  putmsg(X_LOW + nxtpos, Y_LOW, BLACK, WHITE, ":", STDFONT, OM_NORM);
  putmsg(X_LOW + nxtpos + chrlen, Y_LOW, BLACK, WHITE, err_msgs[errind],
         STDFONT, OM_NORM);
  }
else             /* write to a file */
  fprintf(fil, "%s:%s", msg, err_msgs[errind]);
}
