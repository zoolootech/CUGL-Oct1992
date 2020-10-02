/****************************************************************/
/* Box border control routines of the PCcurses package.		*/
/*								*/
/****************************************************************/
/* This version of curses is based on ncurses, a curses version	*/
/* originally written by Pavel Curtis at Cornell University.	*/
/* I have made substantial changes to make it run on IBM PC's,	*/
/* and therefore consider myself free make it public domain.	*/
/*				Bjorn Larsson (bl@infovox.se)	*/
/****************************************************************/
/* 1.4 : Changed to draw a border. Use short wherever		*/
/*	 possible: Portability improvements:		900114	*/
/* 1.3:	 Released:					881005	*/
/****************************************************************/

#include <curses.h>
#include <curspriv.h>

char _curses_border_rcsid[] = "@(#)border.c     v.1.4  - 900114";

/****************************************************************/
/* Wborder(win, l, r, t, b, tl, tr, bl, br) draws a border in	*/
/* window win, with the specified characters as border edges:	*/
/* l	left							*/
/* r	right							*/
/* b	bottom							*/
/* t	top							*/
/* tl	top left corner						*/
/* tr	top right corner					*/
/* bl	bottom left corner					*/
/* br	bottom right corner					*/
/****************************************************************/

void wborder(win, l, r, t, b, tl, tr, bl, br)
  WINDOW	*win;
  int		 l;
  int		 r;
  int		 t;
  int		 b;
  int		 tl;
  int		 tr;
  int		 bl;
  int		 br;
  {
  short	 xmax, ymax, i;

  ymax = win->_maxy - 1;
  xmax = win->_maxx - 1;
  for (i = 1; i <= xmax-1;i++)
    {
    win->_line[0][i] =     t | win->_attrs;
    win->_line[ymax][i] =  b | win->_attrs;
    } /* for */
  for (i = 1;i <= ymax-1;i++)
    {
    win->_line[i][0] =     l | win->_attrs;
    win->_line[i][xmax] =  r | win->_attrs;
    } /* for */
  win->_line[0][0] =       tl | win->_attrs;
  win->_line[0][xmax] =    tr | win->_attrs;
  win->_line[ymax][0] =    bl | win->_attrs;
  win->_line[ymax][xmax] = br | win->_attrs;

  for (i=0; i <= ymax ; i++)
    {
    win->_minchng[i] = 0;
    win->_maxchng[i] = xmax;
    } /* for */
  } /* wborder */

/****************************************************************/
/* Border(l, r, t, b, tl, tr, bl, br) draws a border in stdscr,	*/
/* with the specified characters as border edges:		*/
/* l	left							*/
/* r	right							*/
/* b	bottom							*/
/* t	top							*/
/* tl	top left corner						*/
/* tr	top right corner					*/
/* bl	bottom left corner					*/
/* br	bottom right corner					*/
/* Don't make this a call to wborder() - it would require soo	*/
/* much stack for parameters...					*/
/****************************************************************/

void border(l, r, t, b, tl, tr, bl, br)
  int		 l;
  int		 r;
  int		 t;
  int		 b;
  int		 tl;
  int		 tr;
  int		 bl;
  int		 br;
  {
  short	 xmax, ymax, i;

  ymax = stdscr->_maxy - 1;
  xmax = stdscr->_maxx - 1;
  for (i = 1; i <= xmax-1;i++)
    {
    stdscr->_line[0][i] =     t | stdscr->_attrs;
    stdscr->_line[ymax][i] =  b | stdscr->_attrs;
    } /* for */
  for (i = 1;i <= ymax-1;i++)
    {
    stdscr->_line[i][0] =     l | stdscr->_attrs;
    stdscr->_line[i][xmax] =  r | stdscr->_attrs;
    } /* for */
  stdscr->_line[0][0] =       tl | stdscr->_attrs;
  stdscr->_line[0][xmax] =    tr | stdscr->_attrs;
  stdscr->_line[ymax][0] =    bl | stdscr->_attrs;
  stdscr->_line[ymax][xmax] = br | stdscr->_attrs;

  for (i=0; i <= ymax ; i++)
    {
    stdscr->_minchng[i] = 0;
    stdscr->_maxchng[i] = xmax;
    } /* for */
  } /* border */
