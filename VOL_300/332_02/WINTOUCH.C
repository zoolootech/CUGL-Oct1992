/****************************************************************/
/* Touchwin() routine of the PCcurses package			*/
/*								*/
/****************************************************************/
/* This version of curses is based on ncurses, a curses version	*/
/* originally written by Pavel Curtis at Cornell University.	*/
/* I have made substantial changes to make it run on IBM PC's,	*/
/* and therefore consider myself free to make it public domain.	*/
/*				Bjorn Larsson (bl@infovox.se)	*/
/****************************************************************/
/* 1.4:  Use of short wherever possible. Portability		*/
/*	 improvements:					900114	*/
/* 1.3:	 MSC -W3, Turbo'C' -w -w-pro checkes:		881005	*/
/* 1.2:	 Max limits off by 1. Fixed thanks to S. Creps:	881002	*/
/* 1.0:	 Release:					870515	*/
/****************************************************************/

#include <curses.h>
#include <curspriv.h>

char _curses_wintouch_rcsid[] = "@(#)wintouch.c   v.1.4  - 900114";

/****************************************************************/
/* Touchwin() marks all lines of window 'win' as changed, from	*/
/* the first to the last character on the line.			*/
/****************************************************************/

void touchwin(win)
  WINDOW	*win;
  {
  short	y;
  short maxy;
  short maxx;

  maxy = win->_maxy - 1;
  maxx = win->_maxx - 1;

  for (y = 0; y <= maxy; y++)
    {
    win->_minchng[y] = 0;
    win->_maxchng[y] = maxx;
    } /* for */
  } /* touchwin */
