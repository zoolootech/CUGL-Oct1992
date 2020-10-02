/****************************************************************/
/* Low-level I/O functions of the PCcurses package, 'C' version	*/
/****************************************************************/
/*	 NOTE! NOTE! NOTE! NOTE! NOTE! NOTE! NOTE! NOTE!	*/
/* This version is for a 68000 stand-alone VT100 version	*/
/****************************************************************/
/* This version of curses is based on ncurses, a curses version	*/
/* originally written by Pavel Curtis at Cornell University.	*/
/* I have made substantial changes to make it run on IBM PC's,	*/
/* and therefore consider myself free make it public domain.	*/
/*				Bjorn Larsson (bl@infovox.se)	*/
/****************************************************************/
/* 1.4:  Functional:					900114	*/
/****************************************************************/

#include <curses.h>
#include <curspriv.h>

typedef struct
  {
  char *name;				/* Device/file name */
  int (* inch)  ();			/* Address of input routine */
  int (* outch) ();			/* Address of output routine */
  int (* intst) ();			/* Address of input test routine */
  int    id;
  } _chan_desc;

static char _curses_curse68_rcsid[] = "@(#)curses68.c   v.1.4  - 900114";

extern	_chan_desc _chan_tab[];		/* This one may be accessed */

static	char	clearseq[] = "\033[2J\033[H";
static	char	seqbuf[20];

static	char	isrev	= 0;

static	int	myrow = 1000;
static	int	mycol = 1000;

/****************************************************************/
/* _Cursescattr() writes char 'chr' with attributes 'attr' to	*/
/* the current cursor location.					*/
/****************************************************************/

void _cursescattr(chr, attr)
  char	chr;
  char	attr;
  {
  if ((attr & 0x70) == 0x70)
    {
    if(!isrev)
      {
      (*_chan_tab[1].outch)(0x1b);
      (*_chan_tab[1].outch)('[');
      (*_chan_tab[1].outch)('7');
      (*_chan_tab[1].outch)('m');
      isrev = 1;
      } /* if */
    }
  else
    {
    if (isrev)
      {
      (*_chan_tab[1].outch)(0x1b);
      (*_chan_tab[1].outch)('[');
      (*_chan_tab[1].outch)('0');
      (*_chan_tab[1].outch)('m');
      isrev = 0;
      } /* if */
    } /* else */
  if ((_cursvar.cursrow < LINES-1) || (_cursvar.curscol < COLS-1))
    {
    (*_chan_tab[1].outch)(chr);
    if (mycol++ >= 80)
      {
      mycol = 0;
      myrow++;
      } /* if */
    } /* if */
  } /* _cursescattr */	

/****************************************************************/
/* _Cursescursor() sets the cursor position in video page 0.	*/
/* 'row' and 'column' are the cursor address. If 'row' is set	*/
/*  to 25, no cursor at	all is displayed.			*/
/****************************************************************/

void _cursescursor(row, column)
  int	row;
  int	column;
  {
  char	*p;
  char	buf[15];

  if ((row == myrow) && (column == mycol))
    return;
  (*_chan_tab[1].outch)(0x1b);
  sprintf(seqbuf,"\033[%d;%dH",row+1, column+1);
  for (p = seqbuf; *p; p++)
    (*_chan_tab[1].outch)(*p);
  myrow = row;
  mycol = column;
  }/* _cursescursor */

/****************************************************************/
/* _Cursesgcols() returns the current number of columns on the	*/
/* screen.							*/
/****************************************************************/

int _cursesgcols()
  {
  return(80);
  } /* _cursesgcols */

/****************************************************************/
/* _Cursesputc() outputs character 'chr' to screen in tty	*/
/* fashion. If a colour mode is active, the character is writ-	*/
/* ten with colour 'colour'.					*/
/****************************************************************/

void _cursesputc(chr, color)
  char	chr;
  char	color;
  {
  if (_cursvar.cursrow >= LINES-1)
    return;
  if (_cursvar.curscol >= COLS-1)
    return;
  (*_chan_tab[1].outch)(chr);
  if (mycol++ >= 80)
    {
    mycol = 0;
    myrow++;
    } /* if */
  } /* _cursesputc */

/****************************************************************/
/* _Cursesscroll() scrolls a window in the current page up or	*/
/*  down. Urow, lcol, lrow,rcol are the window coordinates.	*/
/* Lines is the number of lines to scroll. If 0, clears the	*/
/* window, if < 0 scrolls down, > 0 scrolls up. Blanks areas	*/
/* that are left, and sets character attributes to attr. If in	*/
/* a colour graphics mode, fills them with the colour 'attr'	*/
/* instead.							*/
/****************************************************************/

void _cursesscroll(urow, lcol, lrow, rcol, lines, attr)
  int	urow;
  int	lcol;
  int	lrow;
  int	rcol;
  int	lines;
  char attr;
  {
  char	*p;

  for (p = clearseq; *p; p++)
    (*_chan_tab[1].outch)(*p);
  } /* _cursesscroll */

/****************************************************************/
/* _Cursesgcmode() returns the current cursor type. Bits 8-15	*/
/* of the return value is the start scan row, and bits 0-7 is	*/
/* the end scan row.						*/
/****************************************************************/

int _cursesgcmode()
  {
  return(0x0f00);
  } /* _cursesgcmode */

/****************************************************************/
/* _Cursescmode() sets the cursor type to begin in scan line	*/
/* startrow and end in scan line endrow. Both values should be	*/
/* 0-31.							*/
/****************************************************************/

void _cursescmode(startrow, endrow)
  int	startrow;
  int	endrow;
  {
  } /* _cursescmode */

/****************************************************************/
/* _Curseskey() returns the next key code struck at the key-	*/
/* board. If the low 8 bits are 0, the upper bits contain the	*/
/* extended character code. If bit 0-7 are non-zero, the upper	*/
/* bits = 0.							*/
/****************************************************************/

int _curseskey()
  {
  return ((*_chan_tab[1].inch)());
  } /* _curseskey */

/****************************************************************/
/* _Curseskeytst() returns 1 if a keyboard character is avail-	*/
/* able, 0 otherwise.						*/
/****************************************************************/

char _curseskeytst()
  {
  return((*_chan_tab[1].intst)());
  } /*_curseskeytst */

/****************************************************************/
/* _Cursesgcb() returns 1 if MSDOS BREAK CHECK is on, else 0.	*/
/****************************************************************/

int _cursesgcb()
  {
  return(1);
  } /* _cursesgcb */

/****************************************************************/
/* _Cursesscb() sets MSDOS BREAK CHECK according to 'setting'.	*/
/****************************************************************/

void _cursesscb(setting)
  int setting;
  {
  } /* _cursesscb */

#undef getch

/****************************************************************/
/* Getch() read one character from the keyboard without any	*/
/* interpretation whatever.					*/
/****************************************************************/

int	getch()
  {
  return ((*_chan_tab[1].inch)());
  } /* getch */
