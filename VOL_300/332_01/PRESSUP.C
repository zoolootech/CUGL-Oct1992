/*----------------------------------------------------*- Fundamental -*-

Facility:		pressup

File:			pressup.c

Associated files:	- (none)

Description:		Strategy board game

Portability:		[A Thulin:]

			Edited to conform to X/Open Portability
			Guide, ed. 3, 1988.

Author:			Prof. Steve Ward
			Director, Real-Time Systems Group
			MIT Lab for Computer Science
			Cambridge, Massachussetts, 02139

Editors:		Leor Zolman

			Anders Thulin
			Rydsvagen 288
			S-582 50 Linkoping
			SWEDEN

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

Edit history :

Vers  Ed   Date        By                Comments
----  ---  ----------  ----------------  -------------------------------
 1.0    0  19xx-xx-xx  Steve Ward
 1.1    1  19xx-xx-xx  Leor Zolman
 1.2    2  1989-10-25  Anders Thulin     Changed to curses-oriented
				         user interface.

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*---  Configuration options:

/* - - Configuration options:  - - - - - - - - - - - - - - - - - - - - - - */

/*
 *  Compile-time environment:
 *
 *	ANSI		ANSI C
 *	BSD		BSD Unix, SunOS 3.5
 *	SV2		AT&T UNIX System V.2
 *	XPG3		X/Open Portability Guide, ed. 3
 *	ZTC205		Zortech C 2.05
 *
 *  If you have an ANSI C conforming compiler, you only need to define 
 *  `ANSI'. If you don't, choose one of the other definitions.  Don't
 *  forget to define BEEP() correctly.
 *
 */

#define	ANSI		0	
#define BSD		0
#define	SV2		0
#define XPG3		0
#define ZTC205		1

/*
 *  Run-time environment:
 *
 *  BEEP()	See 'comments' below
 *
 */

#define	BEEP()	beep()

/* - - end of configuration options - - - - - - - - - - - - - - - - - - - */

#if ANSI
# include <ctype.h>
# include <curses.h>
# include <stdio.h>
# include <stdlib.h>
  extern int getopt(int argc, char **argv, char *optstring);
  extern int   optind;
  extern char *optarg;
#endif

#if BSD
# include <ctype.h>
# include <curses.h>
# include <stdio.h>
  extern int getopt();
  extern int   optind;
  extern char *optarg;
  extern long  strtol();
# define EXIT_FAILURE	1
# define EXIT_SUCCESS	0
#endif

#if SV2
# include <ctype.h>
# include <curses.h>
  extern int getopt();
  extern int   optind;
  extern char *optarg;
  extern long  strtol();
# define EXIT_FAILURE	1
# define EXIT_SUCCESS	0
#endif

#if XPG3
# include <ctype.h>
# include <curses.h>
# include <stdio.h>
# include <stdlib.h>
  extern int getopt();
  extern int   optind;
  extern char *optarg;
#endif

#if ZTC205
# include <ctype.h>
# include <curses.h>
# include <stdio.h>
  extern int getopt();
  extern int   optind;
  extern char *optarg;
# define EXIT_FAILURE	1
# define EXIT_SUCCESS	0
# define strtol(str, ptr, base)	((base) == 0 ? atoi(str) : (-1))
#endif

/*--- Comments on known problems: -----------------------------------------

curses

	Some older implementations of curses may not have the beep()
	call. On these systems, you may be able to define BEEP() as
	addch(0x07), or perhaps fputc(0x07, stderr) or something
	similar.

	If you cannot do a beep, try a screen flash

	If you cannot do either, just define BEEP to be empty.

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/*  'man-page' based on comments in original program:

pressup(6)					pressup(6)

NAME
	pressup

SYNOPSIS
	pressup [ -f ] [ -d depth ] [ -b ]


DESCRIPTION

	The game of Press-Ups is played as follows: The board is a n by
	n array of pegs, each of which is standing up at the start of
	the game.  Pegs come in 3 colors: red (yours), blue (the
	machine's), and white (periods, actually; they're neutral.)

	The first player to move must `push down' a neutral peg. 
	Thereafter, players take turns pushing down pegs, where each peg
	pushed must be adjacent to the last one pushed. 

	The player moves the cursor to the wanted peg by using the
	`standard' keys h, j, k, and l to move right, down, up and left
	respectively.  The player then presses the selected key by 
	pressing the space bar.  If an illegal key has been selected,
	the program beeps or flashes the screen.

	As soon as a player gets all of his pegs down, he wins.  When
	there are no more legal moves to play, the player with the most
	of his own colored pegs down is the winner. 


OPTIONS

	-f	The program makes the first move

	-d depth
		Set search-depth to 'depth'. Default is 3.

	-b	Show evaluation of position (negative numbers means
		the player has the upper hand,  positive that
		the program thinks it's winning)

CAVEAT

	Watch out...at search depths of 6 or more, this program plays a
	mean game!!!

AUTHOR

	Steve Ward wrote the original program.  Leor Zolman and later
	Anders Thulin edited it.

BUGS

	(A. Thulin:)

	Some players may feel that the keys chosen for cursor movement
	(h,j,k and l) are less than intuitive to use. The reason they were
        chosen is that many other UNIX-games like hack, nethack,
        rogue and vi use these keys for the same purpose. It is left as
        an exercise to change it to handle arrow keys with curses.
	Complaints to /dev/null.
*/

#define	HDR		" Pressup 1.2 "

/*  Display definitions - depend on 24 x 80 screen:  */

#define	HDRX		1	/* HDR - Version line */
#define	HDRY		1

#define	BOARDX		1	/* Upper left corner of board	*/
#define	BOARDY		3

#define	STATUSX		1	
#define	STATUSY		15

#define	DEPTHX		30
#define	DEPTHY		BOARDY

#define	SCOREX		DEPTHX
#define	SCOREY		DEPTHY+2

#define	HELPX		DEPTHX
#define	HELPY		SCOREY+2

#define	STARTX		DEPTHX
#define	STARTY		HELPY+2

#define	EVALX		DEPTHX
#define	EVALY		STARTY+2

/*  Game definitions:  */

#define MAX_SIDE	13	/* Max dimension of board	*/

#define HISFIRST (Side*2+1)	/* His best first move	*/
#define MYFIRST (Side+Side/2-1) /* My best first move	*/

#define BACKSP 	0x08

int	BestMove;	/* Returned by search		*/
char	BFlag;		/* Debugging flag		*/
int	Depth;		/* Search depth (default = 3)	*/
char	FFlag;		/* -f option: machine goes first */
int	Helpflag;
int	Side = 7;	/* Current side of board	*/
char	Startflag;	/* True on first move only */

char   *image;

int 	Adj[16] = {-1,-1,-1,0,-1,1,0,-1,0,1,1,-1,1,0,1,1 };

typedef struct {
  char board[MAX_SIDE*MAX_SIDE];
  int  star;
  char red;
  char blue;
} BBOARD;

BBOARD initb;
BBOARD master, savebd;

/*  Local routines:  */

#if __STDC__ != 0
 void asknew(void);
 void boardcopy(BBOARD *p1, BBOARD *p2);
 int  CheckWin(BBOARD *bp);
 void dmove(int n);
 int  getmove(void);
 int  Help(void);
 void mmove(BBOARD *bp, int n);
 void pboard(BBOARD *bp);
 void print_square(BBOARD *bp, int row, int col, int invert);
 int  search(BBOARD *bp, int ddepth, int who, int alpha, int beta);
#else
 void asknew();
 void boardcopy();
 int  CheckWin();
 void dmove();
 int  getmove();
 int  Help();
 void mmove();
 void pboard();
 void print_square();
 int  search();
#endif

/*
 *  Routine:		asknew
 *
 *  Description:	Ask user if he wants a new game.  Quit if not.
 *
 */

void asknew()
{
  addstr("Another game? ");	
  refresh();
  if (toupper(getchar()) != 'Y') {
    move(LINES-1, 0);
    refresh();
    endwin();
    exit(EXIT_SUCCESS);
  }
}

/*
 *  Routine:		boardcopy
 *
 *  Description:	Copy a board.
 *
 *  Note:		Originally copied each element of p1 to p2
 *			by hand, probably because C didn't allow
 *			assignments of whole structs.  It does now.
 *
 */

void boardcopy(p1, p2)
BBOARD *p1, *p2;
{
  *p2 = *p1;
}

/*
 *  Routine:		Checkwin
 *
 *  Description:	Check if any side has won, and return 1 if so.
 *			Also print the appropriate text. If no-one has
 *			won, return 0.
 *
 */

int CheckWin(bp)
BBOARD *bp;
{
  int i;
  i = search(bp, 1, 1, -32000, -32000);
  if (BestMove >= 0) return 0;

  move(BOARDY+Side+3, STATUSX);

  if (i>0) {
    addstr("I win! ");
  }
  if (i<0) {
    addstr("You win! ");
  }
  if (i==0) {
    addstr("Tie game! ");
  }
  return 1;
}

/*
 *  Routine:		dmove
 *
 *  Description:	display move #n
 *
 */

void dmove(n)
int n;
{
  mmove(&master,n);
  pboard(&master);
  boardcopy(&master,&savebd);
}

/*
 *  Routine:		getmove
 *
 *  Description:	Get player's next move, and return it as an
 *			integer (row*size + col).
 *
 *			Negative move indicates a quit.
 *
 */

int getmove()
{
  int chg_flag;		/* User changed coordinates */
  int cmd;		/* command char from user */
  int dr, dc;		/* offset from previous move */
  int m;		/* selected move */
  int move_selected;	/* TRUE when user has selected a legal move */
  int row, col;		/* current board coordinate */
  int rd,  cd;		/* offset to new board coordinate */
  int star;		/* previous move (< 0 if no previous move) */

  star = master.star;

  /*  1.  Prompt for move:  */

  move(BOARDY + Side + 1, STATUSX); 
  addstr("Select and press peg ... ");

  /*  2.  Loop until a legal square has been selected:  */

  if (star >= 0) {	/* start at previous move */
    row = star/Side;
    col = star%Side;
  } else {		/* else start in the middle */
    col = row = Side/2;
  }

  move_selected = FALSE; chg_flag = TRUE;
  while (!move_selected) {

    if (chg_flag) {	/* avoid unneccessary updates */
      print_square(&master, row, col, TRUE);
      refresh();
      chg_flag = FALSE;
    }

    cmd = getch();
    if (islower(cmd)) {
      cmd = toupper(cmd);
    }

    rd = cd = 0;
    switch(cmd) {
      default:		/* Unknown command  */
        BEEP();
        break;

      case ERR:		/* Problem or non-blocking getch() */
	break;		

      case ' ':			/* select current position */
        m  = row*Side + col;
        if (star < 0) { 	/* first move */
          if (master.board[m] == 0) {
            move_selected = TRUE;
          } else {
            BEEP();
          }
        } else {
          dr = abs(row - star/Side);
          dc = abs(col - star%Side);
          if (master.board[m] < 3 && dr < 2 && dc < 2) {
            move_selected = TRUE;
          } else {
            BEEP();
          }
        }
        break;

      case 'H':		/* move left  */
        if (col == 0) {
          BEEP();
        } else {
          cd = -1;
        }
	break;

      case 'J' :
        if (row == Side-1) {
          BEEP();
        } else {
          rd = 1;
        }     
        break;

      case 'K' :
        if (row == 0) {
          BEEP();
        } else {
          rd = -1;
        }
        break;

      case 'L':		/* move right */
        if (col == Side-1) {
          BEEP();
        } else {
          cd = 1;
        }
	break;

      case 'Q':		/* user wants to quit */
        m = -1;
        move_selected = TRUE;
        break;

      case 'Z':		/* He wants help ... */
        m = Help();
        move_selected = TRUE;
        break;
    }

    if (rd != 0 || cd != 0) {
      print_square(&master, row, col, FALSE);
      row += rd; col += cd;
      chg_flag = TRUE;
    }
  }

  return m;
}

/*
 *  Routine:		Help
 *
 *  Description:	Give the poor soul some badly needed assistance
 *
 */

int Help()
{
  move(BOARDY+Side+1, STATUSX);
  addstr("I'm thinking for you...");
  Helpflag = 1;
  search(&master,Depth,-1,-32000,-32000);
  return BestMove;
}

/*
 *  Routine:		main
 *
 *  Description:	...
 *
 */

int main(argc, argv)
int   argc;
char *argv[];
{
  int   ch;		/* option char from argv	*/
  int	errflag;	/* TRUE if something went wrong in arg parsing */
  int   i,j;

  FFlag  = BFlag = 0;
  image = ".rbXRB";
  Depth = 3;
  errflag = FALSE;

  /* Do getopt instead:  */

  while ((ch =getopt(argc, argv, "bd:f")) != EOF) {
    switch(ch) {
      default :		/* unknown value returned from getopt() */
        fprintf(stderr, "fatal: unknown value returned from getopt(): %x\n",
                        ch);
        exit(EXIT_FAILURE);

      case '?':		/* unknown option letter - errmsg already printed */
        errflag = TRUE;
        break;

      case 'b':
        BFlag++;
        break;

      case 'd':		/* depth of search tree */
        Depth = (int) strtol(optarg, (char **)0, 0);
        break;

      case 'f':
        FFlag++;
        break;
    }
  }
  
  if (optind < argc) {
    fprintf(stderr, "unknown argument -- '%s'\n", argv[optind]);
    errflag = TRUE;
  }

  if (errflag) {
    fprintf(stderr, "usage: pressup [-b] [-d depth] [-f]\n");
    exit(EXIT_FAILURE);
  }


  initscr();
  cbreak();
  noecho();

ngame:
  Startflag = 1;
  Helpflag = 0;
  for (i=0; i<(Side*Side); i++) initb.board[i] = 0;
  for (j=1; j<(Side-1); j++) {
    initb.board[j] = 1;
    initb.board[(Side*Side-1)-j] = 1;
    initb.board[Side*j] = 2;
    initb.board[Side*j + Side-1] = 2;
  };
  initb.star = -1; initb.red = 0; initb.blue = 0;

  boardcopy(&initb, &master);
  pboard(&master);
  boardcopy(&master, &savebd);

  for(;;) {
    if (FFlag) {
      FFlag = 0;
      goto Mine;
    }
    if (CheckWin(&master)) {
      asknew();
      goto ngame;
    }
    i = getmove();
    if (i < 0) {	
      move(BOARDY+Side+3, STATUSX);
      addstr("You quit! ");
      asknew();
      goto ngame;
     }
    dmove(i);
    if (CheckWin(&master)) {
      asknew();
      goto ngame;
     }
Mine:
    move(BOARDY+Side+1, STATUSX);
    addstr("I'm thinking...                ");
    i = search(&master, Depth, 1, -32000, -32000);
    if (BFlag) {
      move(EVALX, EVALY); printw("Pos. eval = %4d", i); 
    }
    dmove(BestMove);
    if (i > 500) {
      move(STATUSY+1, STATUSX); addstr("I've got you!"); 
    }
    if (i < -500) {
      move(STATUSY+1, STATUSX); addstr("You've got me!"); 
    }
  }

  /*NOTREACHED*/  /* asknew() exits */
}

/*
 *  Routine:		mmove
 *
 *  Description:	Make a move
 *
 */

void mmove(bp,n)
BBOARD *bp;
int     n;
{
  int type;
  type = bp->board[n] += 3;
  if (type == 4) bp->red++;
  else if (type == 5) bp->blue++;
  bp->star = n;
}

/*
 *  Routine:		pboard
 *
 *  Description:	Update screen with new board, status, and everything.
 *
 */

void pboard(bp)
BBOARD *bp;
{
  int row, col;		/*  Board coordinates */

  /*  1.  Program name and version:  */

  move(HDRY, HDRX); addstr(HDR);

  /*  2.  Board:  */

  for (row=0; row<Side; row++) {
    for (col=0; col<Side; col++) {
      print_square(bp, row, col, FALSE);
    }
  }

  /*  3.  Print score, search depth and other info:  */

  move(DEPTHY, DEPTHX);
  printw("Search Depth: %2d moves", Depth);

  move(SCOREY, SCOREX);
  printw("Score:    Blue (me) %2d,   Red (you)  %2d", master.blue, master.red);

  if (Helpflag) {
    move(HELPY, HELPX);
    addstr("You've had help!");
  }
  
  if (Startflag) {
    move(STARTY, STARTX);
    addstr(FFlag ? "I go first" : "You go first");
    Startflag = 0;
  }
}

/*
 *  Routine:		print_square
 *
 *  Description:	Print a square on the given board. 
 *
 */

void print_square(bp, row, col, invert)
BBOARD *bp;
int     row;
int     col;
int     invert;
{
  int	n;	/* internal coordinate */

  move(BOARDY+row, BOARDX+3*col);	/* Each square is 3 char wide */
  
  if (invert) {
    standout();
  }

  n = row*Side + col;      
  if (bp->star == n) {
    addstr(" * ");
  } else {
    printw(" %c ", image[bp->board[n]]);
  }

  if (invert) {
    standend();
  }

 /*
  *  Leave cursor *on* square. 
  *  This is quite good for systems with line cursors. Non-blinking
  *  block cursors give a rather odd impression, though.
  *
  */

  move(BOARDY+row, BOARDX+3*col+1);
}

/*
 *  Routine:		search
 *
 *  Description:	Alpha-beta pruning search
 *
 *			Put best move for 'who' into BestMove. Return
 *			estimated strength of position.
 *
 */

int search (bp,ddepth,who,alpha,beta)
BBOARD *bp;
int ddepth;
int who;
int alpha;
int beta;
{
  int i,j,k;
  int myalpha,hisalpha;
  int best;
  int num;
  int bestmove, ii, jj, n;
  int SavStar;
  int SavBlue;
  int SavRed;
  int Save;
  char moves[9];

  best = -32000;
  SavStar = bp->star;
  SavBlue = bp->blue;
  SavRed  = bp->red;
  BestMove = -1;		/* No best move yet...	*/

  if (SavStar < 0) {		/* special case opening moves	*/
    BestMove = HISFIRST;
    if (who > 0) BestMove = MYFIRST;
    return 0;
  };

  if (!ddepth--)
    return(who * (bp->blue - bp->red));

  if (bp->blue == (Side*2-4) || bp->red == (Side*2-4))
    return(who*(bp->blue - bp->red)*1000);

  /* alpha-beta pruning   */
  if (who>0) {
    myalpha = bp->blue; hisalpha = bp->red; 
  } else {
    myalpha = bp->red; hisalpha = bp->blue; 
  }
  myalpha += ddepth;  /* Most optimistic estimate. */
  if (myalpha > (Side*2-4)) myalpha = (Side*2-4);
  if (myalpha == (Side*2-4)) myalpha = 1000*(myalpha-hisalpha);
  else myalpha -= hisalpha;
  if (myalpha <= alpha) return best;

  k = bp->star;
  i = k%Side;
  j = k/Side;
  num = 0;
  for (n=0; n<8; n++) {		/* Try squares 'around' last move */
    if ((ii = i+Adj[n+n]) < 0 || ii >= Side) continue;
    if ((jj = j+Adj[n+n+1]) < 0 || jj >= Side) continue;
    moves[num] = jj*Side + ii;
    if (bp->board[moves[num]] < 3) num++; 
  }
  if (num == 0) return(who*(bp->blue - bp->red)*1000);
  bestmove = moves[0];
  for (i=0; i<num; i++) {
    Save = bp->board[moves[i]];
    mmove(bp,moves[i]);
    k = -search(bp,ddepth,-who,beta,alpha);
    bp->board[moves[i]] = Save;
    bp->blue = SavBlue; bp->red = SavRed; bp->star = SavStar;
    if (k > alpha) alpha = k;
    if (k > best) { best = k; bestmove = moves[i]; }
    if (k>100) break;
  }
  BestMove = bestmove;
  return best;
}

