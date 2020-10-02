/*----------------------------------------------------*- Fundamental -*-

Facility:		bugs(6)

File:			bugs.c

Associated files:	- (none)

Description:		Bugs me. Bugs anyone.

Notes:			Needs curses.

Author:			Steve Ward

Editor:			Anders Thulin
			Rydsvagen 288
			S-582 50 Linkoping
			SWEDEN

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

Edit history :

Vers  Ed   Date	       By                Comments
----  ---  ----------  ----------------  -------------------------------
 1.0    0  19xx-xx-xx  Steve Ward
 1.1    1  1990-03-16  Anders Thulin     Changed H19/H89 screen handling to
				         curses. Still buggy, though :-)

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*---  Configuration:  --------------------------------------------------

System configuration options:
=============================

  ANSI		ANSI C conformant compiler
  BSD		BSD Unix
  SYSV		AT&T System V.x Unix

If you have an ANSI compiler, defined ANSI only.  Otherwise, define
the alternative that most closely matches your environment.


Program configuration:
======================

  NBUGS		Max. number of bugs permitted


- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#define	ANSI	 1
#define	BSD	 0
#define	SYSV	 0

#define	NBUGS	25

/* - - End of configurations - - - - - - - - - - - - - - - - - - - - - - - - */

#include <curses.h>
#include <stdio.h>

#if ANSI
# include <stdlib.h>
 extern int getopt(int argc, char **argv, char *optstring);
 extern int optind;
#endif

#if BSD
 extern int getopt();
 extern void exit();
 extern int optind;
# define EXIT_FAILURE   1
# define EXIT_SUCCESS	0
#endif

#if SYSV
 extern int getopt();
 extern void exit();
 extern int optind;
# define EXIT_FAILURE   1
# define EXIT_SUCCESS	0
#endif

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

/*----------------------------------------------------------------------

Known problems:

curses

  The program relies on non-blocking calls to getch() so that it can
  quit as soon as the `user' presses a key.  Older implementations of
  curses don't have the function nodelay() which sets up for nonblocking
  calls to getch().

  It may be possible to remove the calls to getch() altogether.
  Instead, catch SIGINT (or whatever is used to indicate that user
  wants to quit) to make an orderly exit from curses.

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


/*
	"Bugs"
	written by Steve Ward for the H19/H89 display terminal
	BD says..."This one is WIERD!!!"
*/

#define BOT	0
#define LEFT	0
#define RADIUS2	21

int	Top,		/* Pagesize-1				*/
	Right;		/* Linelength-2				*/

struct bug {
  int X,Y;
  int Dir;	/* 0-down, 1-left, 2-up, 3-right.	*/
  int State;
} bugs[NBUGS];

char	Wflg, Cflg;
int	CurX, CurY;
int	CBugs;

int XMotion[20] = { 0, 1, 0, -1,  1, 1, -1, -1};
int YMotion[20] = {-1, 0, 1,  0, -1, 1,  1, -1};

/* Q & D fix for placech():  */

#define placech(ch, tx, ty) 		\
	do {				\
	  mvaddch((ty), (tx), (ch));	\
	  refresh();			\
	} while (0)

/*----------------------------------------------------------------------
  Local functions:
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#if __STDC__ != 0
 static int  alive(struct bug *bb);
 static void mkbug(struct *bb, int xp, int yp, int direc);
 static void r(struct bug *bb, int dx, int dy, int ch);
 static void randbug(struct bug *bb);
 static void step(struct bug *bb);
 static void turn(struct bug *bb);
#else
 static int  alive();
 static void mkbug();
 static void r();
 static void randbug();
 static void step();
 static void turn();
#endif

static int alive(bb)
struct bug *bb;
{
  int px,py;
  px = bb->X; py = bb->Y;
  switch((bb->Dir) & 03) {
    case 0: return(py>=BOT-4);
    case 1: return(px<=Right+6);
    case 2: return(py<=Top+4);
    case 3: return(px>=LEFT-4); 
  }
  /*NOTREACHED*/
}

static void step(bb)
struct bug *bb;
{
  switch ((*bb).State) {
    case 0: r(bb,1,0,' '); r(bb,1,-1,'\\'); (*bb).State++; break;
    case 1:	r(bb,-1,0,' ');
			r(bb,-1,-1,'/');
			(*bb).State++;	break;
    case 2:	r(bb,1,1,' ');
			r(bb,1,0,'\\');
			(*bb).State++;	break;
    case 3:	r(bb,-1,1,' ');
			r(bb,-1,0,'/');
			(*bb).State++;	break;
    case 4:	r(bb,0,-1,'0');
			r(bb,0,0,'O');
			r(bb,1,-1,'/');
			r(bb,-1,-1,'\\');
			r(bb,1,0,'/');
			r(bb,-1,0,'\\');
			(*bb).State++;	break;
    case 5:	r(bb,1,2,' ');
			r(bb,1,1,'\\');
			(*bb).State++;	break;
    case 6:	r(bb,-1,2,' ');
			r(bb,-1,1,'/');
			(*bb).State++;	break;
    case 7:	r(bb,1,1,'/');
			r(bb,0,1,' ');
			r(bb,-1,1,'\\');
			switch (((*bb).Dir) & 03) {
			 case 0: (*bb).Y--; break;
			 case 2: (*bb).Y++; break;
			 case 1: (*bb).X++; break;
			 case 3: (*bb).X--; break; }
			(*bb).State = 0;	break;
/* Diagonal movement:						*/

    case 20:	r(bb,1,1,' ');
			r(bb,1,0,'-');
			(*bb).State++;	break;
    case 21:	r(bb,-1,-1,' ');
			r(bb,0,-1,'|');
			(*bb).State++;	break;
    case 22:	r(bb,0,1,' ');
			r(bb,1,1,'/');
			(*bb).State++;	break;
    case 23:	r(bb,-1,0,' ');
			r(bb,-1,-1,'/');
			(*bb).State++;	break;
    case 24:	r(bb,1,-1,'0');
			r(bb,0,0,'O');
			r(bb,1,1,' ');
			r(bb,0,1,'|');
			r(bb,-1,-1,' ');
			r(bb,-1,0,'-');
			r(bb,1,0,'|');
			r(bb,0,-1,'-');
			(*bb).State++;	break;
    case 25:	r(bb,-1,2,' ');
			r(bb,0,2,'/');
			(*bb).State++;	break;
    case 26:	r(bb,-2,1,' ');
			r(bb,-2,0,'/');
			(*bb).State++;	break;
    case 27:	r(bb,-1,1,' ');
			r(bb,0,2,' ');
			r(bb,-2,0,' ');
			r(bb,1,0,'|');
			r(bb,0,-1,'-');
			switch (((*bb).Dir)& 03) {
			 case 0: (*bb).X++; (*bb).Y--; break;
			 case 1: (*bb).X++; (*bb).Y++; break;
			 case 2: (*bb).X--; (*bb).Y++; break;
			 case 3: (*bb).X--; (*bb).Y--; break; }
			(*bb).State = 20;	break;

/* turn from diag to orthogonal (45 deg CCW)				*/

    case 40:	r(bb,-1,0,' ');
			r(bb,-2,0,'/');
			(*bb).State++; break;
    case 41:	r(bb,-1,0,'O');
			r(bb,-1,2,' ');
			r(bb,-1,1,'|');
			r(bb,-2,0,'\\');
			r(bb,-2,1,'\\');
			(*bb).State++; break;
    case 42:	r(bb,1,1,' ');
			r(bb,0,1,'\\');
			r(bb,-1,1,'\\');
			r(bb,-2,0,' ');
			r(bb,-2,-1,'/');
			r(bb,0,-1,'/');
			(*bb).Dir = (((*bb).Dir)+1) & 03;
			(*bb).State = 0; break;

/* Turn from ortho to diagonal:					*/

    case 50:	r(bb,-1,0,' ');
			r(bb,-1,-1,'/');
			(*bb).State++; break;
    case 51:	r(bb,-1,1,' ');
			r(bb,-1,0,'/');
			(*bb).State++; break;
    case 52:	r(bb,1,2,' ');
			r(bb,0,1,'|');
			r(bb,-1,1,'O');
			r(bb,1,0,' ');
			r(bb,-1,2,' ');
			r(bb,0,2,'/');
			r(bb,-1,0,' ');
			r(bb,-2,0,'/');
			r(bb,-2,1,'-');
			(*bb).State++; break;
    case 53:	r(bb,0,2,' ');
			r(bb,-1,2,'|');
			r(bb,-2,0,' ');
			r(bb,-1,0,'-');
			(*bb).Dir = (((*bb).Dir) | 04);
			(*bb).State = 20; break;
  }
}

static void mkbug(bb, xp, yp, direc)
struct bug *bb;
int xp, yp, direc;
{
  bb->X = xp;
  bb->Y = yp;
  bb->State = 0;
  bb->Dir = direc;

  if (direc < 4) {
    r(bb,0,0,'0');
    r(bb,0,1,'O');
    r(bb,1,0,'/');
    r(bb,1,1,'/');
    r(bb,1,2,'/');
    r(bb,-1,2,'\\');
    r(bb,-1,1,'\\');
    r(bb,-1,0,'\\');
  } else {
    bb->State = 20;
    r(bb,0,0,'0');
    r(bb,1,1,'/');
    r(bb,-1,-1,'/');
    r(bb,0,1,'|');
    r(bb,-1,0,'-');
    r(bb,-1,1,'O');
    r(bb,-1,2,'|');
    r(bb,-2,1,'-');
  }
}


static void r(bb, dx, dy, ch)
struct bug *bb;
int         dx, dy;
char        ch;
{
  int tx, ty, direc;

  direc = (bb->Dir) & 03;
  if ((direc == 1) || (direc == 3)) {
    switch (ch) {
      case '/':	 ch = '\\'; break;
      case '\\': ch = '/'; break;
      case '|':	 ch = '-'; break;
      case '-':	 ch = '|'; break;
      default:	 break;
    }
  }

  switch (direc) {
    case 0: tx = dx+bb->X; ty = dy+bb->Y; break;
    case 2: tx = bb->X-dx; ty = bb->Y-dy; break;
    case 1: tx = bb->X-dy; ty = bb->Y+dx; break;
    case 3: tx = bb->X+dy; ty = bb->Y-dx; break;
  }
  placech(ch,tx,ty); 
}


static void randbug(bb)
struct bug *bb;
{
  int x,y,dir;
  dir = rand()%8;
  x = rand()%80;
  y = rand()%24;
  if (Cflg) {
    x = (Right-LEFT)/2;
    y = (Top-BOT)/2;
  } else switch(dir & 03) {
    case 0: y=24+4; break;
    case 2: y = -4; break;
    case 1: x = -4; break;
    case 3: x=80+4; break;
  }
  mkbug(bb,x,y,dir);
}

static void turn(bb)
struct bug *bb;
{
  if (bb->State == 0) {
    bb->State = 50;
  } else if (bb->State == 20) {
    bb->State = 40;
  }
  return;
}

int main(argc,argv)
int   argc;
char *argv[];
{
  int c;
  int errflg;
  int i,j,xdist,ydist,xmot,ymot;
  char *arg;

  CurX = 1000; CurY = 1000;
  Wflg = 0; Cflg = 0;
  CBugs = 5;

  errflg = 0;
  while ((c = getopt(argc, argv, "cCwW")) != EOF) {
    switch(c) {
      case '?':
        errflg = 1;
        break;

      case 'c':
      case 'C':
        Cflg++;
        break;

      case 'w':
      case 'W' :
        Wflg++;
        break;
    }
  }

  if (optind < argc) {
    CBugs = atoi(argv[optind++]);
  }
  if (optind < argc) {
    fprintf(stderr, "error: unexpected argument '%s'\n", argv[optind]);
    errflg = 1;
  }
  if (errflg != 0) {
    fprintf(stderr, "usage: [-c] [-w] [nr_of_bugs]\n");
    exit(EXIT_FAILURE);
  }

  if (CBugs>NBUGS) CBugs=NBUGS;

  initscr();
  noecho();
  nodelay(stdscr, TRUE);
  cbreak();
  clear();
  Top = LINES-1; Right = COLS-2;  

  if (Wflg) {
    for (i=LEFT; i<Right; i++) placech('-',i,Top);
    for (i=Top; i>BOT; i--) placech('|',Right,i);
    for (i=LEFT; i<Right; i++) placech('-',i,BOT);
    for (i=Top; i>BOT; i--) placech('|',LEFT,i);
  }

  for (i=0; i<CBugs; i++) {
    randbug(&bugs[i]);
  }

  /*  3.  Bug!  */

  for (;;) {

    /*  Had enough?  */

    if (getch() != ERR) {	/* Key hit - quit */
      break;
    }

    i = rand() % CBugs;
    if (!alive(&bugs[i])) {
      randbug(&bugs[i]);
    } else {
      step(&bugs[i]);
      j = (bugs[i]).State;
      xmot = XMotion[(bugs[i]).Dir];
      ymot = YMotion[(bugs[i]).Dir];
      if ((j == 0) || (j == 20)) {
        if (Wflg) {
	  xdist = (bugs[i]).X;
	  ydist = xdist-Right;
	  if ((xmot>0) && (ydist<0) && (ydist*ydist < RADIUS2))
	    turn(&bugs[i]);
	  ydist = xdist-LEFT;
	  if ((xmot<0)&&(ydist>0) && (ydist*ydist < RADIUS2))
	    turn(&bugs[i]);
	  xdist = (bugs[i]).Y;
	  ydist = xdist-Top;
	  if ((ymot>0)&&(ydist<0) && (ydist*ydist < RADIUS2))
	    turn(&bugs[i]);
	  ydist = xdist-BOT;
	  if ((ymot<0)&&(ydist>0) && (ydist*ydist < RADIUS2))
	    turn(&bugs[i]);
        }
      }

      if ((j == 0) || (j == 20)) {
        for (j=0;j<CBugs;j++) {
          if (j==i)
            continue;
	  xdist = (bugs[j]).X - (bugs[i]).X;
	  ydist = (bugs[j]).Y - (bugs[i]).Y;
	  if ((xdist*xdist+ydist*ydist) < 21) {
            if (((xdist*xmot)<=0) && ((ydist*ymot)<=0))
	      continue;
	    turn(&bugs[i]);
	    break;
          }
        }
        if (!(rand()%15))
          turn(&bugs[i]);
      }
    }
  }

  /*  4.  Clean up and exit:  */

  endwin();

  return EXIT_SUCCESS;
}
