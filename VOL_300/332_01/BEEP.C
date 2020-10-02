/****************************************************************/
/*								*/
/* Beep() and flash() routines of the PCcurses package		*/
/*								*/
/****************************************************************/
/* This version of curses is based on ncurses, a curses version	*/
/* originally written by Pavel Curtis at Cornell University.	*/
/* I have made substantial changes to make it run on IBM PC's,	*/
/* and therefore consider myself free to make it public domain.	*/
/*				Bjorn Larsson (bl@infovox.se)	*/
/****************************************************************/
/* 1.4:  Use short wherever possible. Portability		*/
/*	 improvements:					900114	*/
/* 1.3:	 MSC -W3, Turbo'C' -w -w-pro checkes:		881005	*/
/* 1.2:	 Rcsid[] string for maintenance:		881002	*/
/* 1.0:	 Release:					870515	*/
/****************************************************************/

#include <curses.h>
#include <curspriv.h>

char _curses_beep_rcsid[] = "@(#)beep.c       v.1.4  - 900114";

/****************************************************************/
/* Beep() sounds the terminal bell.				*/
/****************************************************************/

void	beep()
  {
  _cursesputc('\007',0);
  } /* beep */

/****************************************************************/
/* Flash() flashes the terminal screen.				*/
/****************************************************************/

void	flash()
  {
  short i;

  _cursesscroll(0,0,LINES-1,COLS-1,0,0x70);
  for (i=0;i<10000;)
    i++;
  _cursesscroll(0,0,LINES-1,COLS-1,0,0x0);
  wrefresh(curscr);
  } /* flash */
