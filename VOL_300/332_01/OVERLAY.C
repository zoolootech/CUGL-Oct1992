/****************************************************************/
/* Overlay() and overwrite() functions of the PCcurses package	*/
/*								*/
/****************************************************************/
/* This version of curses is based on ncurses, a curses version	*/
/* originally written by Pavel Curtis at Cornell University.	*/
/* I have made substantial changes to make it run on IBM PC's,	*/
/* and therefore consider myself free to make it public domain.	*/
/*				Bjorn Larsson (bl@infovox.se)	*/
/****************************************************************/
/* 1.4:  Overlaying window will not line up with over-		*/
/*	 layed window's origin, but at it's 'own' origin	*/
/*	 relative to the overlayed's origin. Use of short	*/
/*	 wherever possible. Portability improvements:	900114	*/
/* 1.3:	 MSC -W3, Turbo'C' -w -w-pro checks:		881005	*/
/* 1.2:	 Max limits off by 1. Fixed thanks to S. Creps:	881002	*/
/* 1.0:	 Release:					870515	*/
/****************************************************************/

#include <curses.h>
#include <curspriv.h>

char _curses_overlay_rcsid[] = "@(#)overlay.c    v.1.4  - 900114";

/****************************************************************/
/* Overlay() overwrites 'win1' upon 'win2', with 'win1' appea-	*/
/* ring in 'win2' at it own origin relative to 'win2's origin.	*/
/* This is a departure, but a desirable one, from the initial	*/
/* definition of this function. Overlay is transparent; blanks	*/
/* from 'win1' are not copied to 'win2'.			*/
/****************************************************************/

void overlay(win1, win2)
  WINDOW	*win1, *win2;
  {
  short		*minchng;
  short		*maxchng;
  short		*w1ptr;
  short		*w2ptr;
  short		 attrs;
  short		 col;
  short		 line;
  short		 last_line;
  short		 last_col;

  last_col = min(win1->_maxx + win1->_begx, win2->_maxx) - 1;
  last_line = min(win1->_maxy + win1->_begy, win2->_maxy) - 1;
  attrs = win2->_attrs & ATR_MSK;
  minchng = win2->_minchng + win1->_begy;
  maxchng = win2->_maxchng + win1->_begy;

  for(line = win1->_begy;  line <= last_line;  line++)
    {
    register short   fc, lc;

    w1ptr = win1->_line[line - win1->_begy];
    w2ptr = win2->_line[line] + win1->_begx;
    fc = _NO_CHANGE;

    for(col = win1->_begx;  col <= last_col;  col++)
      {
      if ((*w1ptr & CHR_MSK) != ' ')
	{
	*w2ptr = (*w1ptr & CHR_MSK) | attrs;
	if (fc == _NO_CHANGE)
	  fc = col;
	lc = col;
	} /* if */
      w1ptr++;
      w2ptr++;
      } /* for */

    if (*minchng == _NO_CHANGE)
      {
      *minchng = fc;
      *maxchng = lc;
      } /* if */
    else
      if (fc != _NO_CHANGE)
	{
	if (fc < *minchng)
	  *minchng = fc;
	if (lc > *maxchng)
	  *maxchng = lc;
	} /* else if */
    minchng++;
    maxchng++;
    } /* for */
  } /* overlay */

/****************************************************************/
/* Overwrite() overwrites 'win1' upon 'win2', with 'win1' ap-	*/
/* pearing in 'win2' at it own origin relative to 'win2's ori-	*/
/* gin. This is a departure, but a desirable one, from the	*/
/* initial definition of this function. Overwrite is non-trans-	*/
/* parent; blanks from 'win1' are copied to 'win2'.		*/
/****************************************************************/

void	overwrite(win1, win2)
  WINDOW	*win1, *win2;
  {
  short		*minchng;
  short		*maxchng;
  short		*w1ptr;
  short		*w2ptr;
  short		 attrs;
  short		 col;
  short		 line;
  short		 last_line;
  short		 last_col;

  last_col = min(win1->_maxx + win1->_begx, win2->_maxx) - 1;
  last_line = min(win1->_maxy + win1->_begy, win2->_maxy) - 1;
  attrs = win2->_attrs & ATR_MSK;
  minchng = win2->_minchng + win1->_begy;
  maxchng = win2->_maxchng + win1->_begy;

  for(line = win1->_begy;  line <= last_line;  line++)
    {
    register short   fc, lc;

    w1ptr = win1->_line[line - win1->_begy];
    w2ptr = win2->_line[line] + win1->_begx;
    fc = _NO_CHANGE;

    for(col = win1->_begx;  col <= last_col;  col++)
      {
      if ((*w1ptr & CHR_MSK) != (*w2ptr & CHR_MSK))
	{
	*w2ptr = (*w1ptr & CHR_MSK) | attrs;
	if (fc == _NO_CHANGE)
	  fc = col;
	lc = col;
	} /* if */
      w1ptr++;
      w2ptr++;
      } /* for */

    if (*minchng == _NO_CHANGE)
      {
      *minchng = fc;
      *maxchng = lc;
      } /* if */
    else
      if (fc != _NO_CHANGE)
	{
	if (fc < *minchng)
	  *minchng = fc;
	if (lc > *maxchng)
	  *maxchng = lc;
	} /* else if */
    minchng++;
    maxchng++;
    } /* for */
  } /* overwrite */
