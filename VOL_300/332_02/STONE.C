/*----------------------------------------------------*- Fundamental -*-

Facility:		stone(6)

File:			stone.c

Associated files:	- (none)

Description:		Plays the game of Kalah.

Notes:			This program was originally available in two
			versions, one for dumb terminals and ttys
			(stone) and one for H19-type terminals
			(hstone).  The present version is a cleaned-
			up implementation of hstone, adapted for a
			curses-compatible terminal interface.

			The program will only work on terminals with
			at least 80 chars per line, and with at least
			24 lines.  The only reason for this limit
			is the screen layout - see printb()

Portability:		Edited to conform to X/Open Portability
			Guide, ed. 3

Authors:		Terry Hayes & Clark Baker
			Real-Time Systems Group
			MIT Lab for Computer Science

Editor:			Leor Zolman

			Anders Thulin
			Rydsvagen 288
			S-582 50 Linkoping
			SWEDEN

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

Edit history :

Vers  Ed   Date	       By               Comments
----  ---  ----------  ----------------  -------------------------------
 1.0    0  19xx-xx-xx  Hayes & Baker
 1.1    1  19xx-xx-xx  Leor Zolman
 1.2    2  1990-03-25  Anders Thulin

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*---  Configuration options:  ----------------------------------------

System configuration options:
=============================

  ANSI		ANSI C
  BSD		BSD Unix, SunOS 3.5
  SV2		AT&T UNIX System V.2
  XPG3		X/Open Portability Guide, ed. 3

If you have an ANSI C conformant compiler, define ANSI.  If not, choose
the definition that most closely matches your environment.


Program configuration options:
==============================

  TRACE		This definition should not be changed. It is used to
		remove all code that in the original version wrote
		tracing information to the screen. As these completely
		destroy the screen layout they have been removed.

		A good way of including them in the program would be to
		change them to write to a special trace window. This is
		left as an exercise for the reader.

  BEEP		See 'comments' below

  FUDGE (65)	Used to determine the total number of game tree nodes
		(game positions) to be examined -- computed as

		  FUDGE * chosen level of difficulty

  MAX_LEVEL (1000)

		The maximum level of difficulty allowed. FUDGE * MAX_LEVEL
		should not be larger than INT_MAX.

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#define	ANSI	1
#define	BSD	0
#define	SV2	0
#define	XPG3	0

#define	TRACE	0	
#define	BEEP()		beep()

#define	FUDGE		32
#define	MAX_LEVEL	1000

/* - - end of configuration options - - - - - - - - - - - - - - - - - - - */

#if ANSI
# include <ctype.h>
# include <curses.h>
# include <stdio.h>
# include <stdlib.h>
 extern int getopt(int argc, char *argv[], char optstring[]);
 extern int optind;
#endif

#if BSD
# include <ctype.h>
# include <curses.h>
# include <stdio.h>
 extern int getopt();
 extern int optind;
#endif

#if SV2
# include <ctype.h>
# include <curses.h>
# include <stdio.h>
 extern int getopt();
 extern int optind;
#endif

#if XPG3
# include <ctype.h>
# include <curses.h>
# include <stdio.h>
# include <stdlib.h>
 extern int getopt();
 extern int optind;
#endif

/*--- Comments on known problems: ------------------------------------------

curses

  Some older implementation of curses does not have the beep()
  function, which beeps at the user.

  On such systems, you may need to define BEEP to produce a
  beep in some other way, like fputc(0x07, stderr).

  If you cannot beep at all, try to flash the screen.

  If you cannot do either, just define BEEP to be empty, which is
  permitted behaviour for curses' beep().

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*--- Original notes on the program:  -----------------------------------

"STONE"
(otherwise known as "Awari")

This version written by
	Terry Hayes & Clark Baker
	Real-Time Systems Group
	MIT Lab for Computer Science
(and hacked up a little by Leor Zolman)

The algorithm used for STONE is a common one to Artificial Intelligence
people: the "Alpha-Beta" pruning heuristic. By searching up and down a tree
of possible moves and keeping record of the minimum and maximum scores from
the terminal static evaluations, it becomes possible to pinpoint move
variations which can in no way affect the outcome of the search.  Thus,
those variations can be simply discarded, saving expensive static evaluation
time. 

#if TRACE

To have the program print out some search statistics for every move
evaluation, type the command as

  A> stone -d

To also see a move-by-move trace of each terminal evaluation, type

  A> stones -a

#end 

THIS is the kind of program that lets C show its stuff; Powerful expression
operators and recursion combine to let a powerful algorithm be implemented
painlessly. 

And it's fun to play!

Rules of the game:

Each player has six pits in front of him and a "home" pit on one side (the
computer's home pit is on the left; your home pit is on the right.)

At the start of the game, all pits except the home pits are filled with n
stones, where n can be anything from 1 to 6. 

To make a move, a player picks one of the six pits on his side of the board
that has stones in it, and redistributes the stones one-by-one going
counter-clockwise around the board, starting with the pit following the one
picked.  The opponent's HOME pit is never deposited into. 

If the LAST stone happens to fall in that player's home pit, he moves again. 

If the LAST stone falls into an empty pit on the moving player's side of
board, then any stones in the pit OPPOSITE to that go into the moving
player's home pit. 

When either player clears the six pits on his side of the board, the game is
over.  The other player takes all stones in his six pits and places them in
his home pit.  Then, the player with the most stones in his home pit is the
winner. 

The six pits on the human side are numbered one to six from left to right;
the six pits on the computer's side are numbered one to six right-to-left. 

The standard game seems to be with three stones; less stones make it
somewhat easier (for both you AND the computer), while more stones
complicate the game.  As far as difficulty goes, well...it USED to be on a
scale of 1 to 50, but I couldn't win it at level 1.  So I changed it to
1-300, and couldn't win at level 1.  So I changed it to 1-1000, and if I
STILL can't win it at level 1, I think I'm gonna commit suicide. 

Good Luck!!!

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

Notes added by Anders Thulin:

The present program plays the game of Kalah.  It does not play the game of
Oware, which has slightly more complicated rules.  For a description of
Oware (as well as other games of the Mancala family), take a look at Ian
Lennox-Smith's series of articles in the British magazine Games & Puzzles,
no. 26-29 (July-October 1974), or any reliable book on board games (there
*are* unreliable ones!)

Kalah is usually played with 3-6 stones in each pit. The game with only one
stone in each pit is 'trivial' (won by the first player), as well as that
with two stones. It is reasonably easy to modify this program to check this
statement out. 

There is an interesting paper on a learning implementation of Kalah in
Machine Intelligence vol. 3, ed. D. Michie, Edinburgh 1974. The paper is
written by A. G. Bell; the title is 'Kalah on Atlas'.

For a description of the alpha-beta pruning algorithm, see almost any
introductory work on artificial intelligence. A good description is given by
David Levy in The Chess Computer Handbook, Batsford, London 1984.

For an interesting description of different minimax algorithms (including
the alpha-beta algorithm) see the article 'A Comparison of Minimax Tree
Search Algorithms' by Murray S. Campbell and T.A. Marsland in the journal
Artificial Intelligence, vol. 20 (1983), p. 347-367. This paper describes a
slightly 'better' alpha-beta algorithm, compared to that used in this
program.

Finally, the author of the original comments makes a peculiar statement in
his last lines in regard to the difficulty level of the program.

Changing the maximum difficulty level from 50, to 300, to 1000 does not
change the level of play on level 1. It only changes the size of the game
tree that is being examined.  The level number corresponds to the number of
positions (nodes) that should be examined.  Playing at level 1 always gives
the same performance, regardless of the maximum number of levels.

However, if you find that level 1 is too difficult, change the FUDGE
parameter above; it is used to determine the final number of nodes.  A FUDGE
factor of 1 (or even 0) makes for the easiest play.

I do really hope that he did discover this in time ...

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */

#define	NR_PITS		6	
#define	BOARD_SIZE	(2*(NR_PITS+1))

#ifndef TRUE			/* should be defined in <curses.h> */
# define  TRUE		1
# define  FALSE		0
#endif

#define	MAX(i, j)	((i) > (j) ? (i) : (j))
#define	MIN(i, j)	((i) < (j) ? (i) : (j))

#define	P_FIRST		1			/* Player's first pit	*/
#define	P_KALAH		(P_FIRST+NR_PITS)	/* Player's kalah	*/
#define	C_FIRST		(P_KALAH+1)		/* Computer's first pit	*/
#define	C_KALAH		0			/* Computer's kalah	*/

typedef	char t_board[BOARD_SIZE];

/*----------------------------------------------------------------------
  Local variables:
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

int	 DEPTH;		/* The current depth of the playing tree 	*/
int	 MAXDEPTH;	/* The max depth reached during evaluation	*/
unsigned CALLS;		/* ??? Same as WIDTH ???			*/
unsigned TOTDEPTH;	/* ... */
unsigned WIDTH;		/* Total number of nodes evaluated		*/
unsigned TERM;		/* ... */
unsigned COUNT;		/* The max number of nodes to be examined	*/
int      NUM;		/* Nr of stones per pit at beginning of game	*/

int	pbegins;	/* true if player moves first	*/

unsigned total;		/* nr of moves actually considered		*/

#if TRACE
/*  Command line options:	*/

int      ab;		/* TRUE if printouts of ... */
int	 db;		/* TRUE if printouts of ... */

#endif /* TRACE */

/*----------------------------------------------------------------------
  Local routines:
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#if __STDC__ != 0
 static int  comp(char *board);
 static int  comp1(char *board, int who, unsigned count, int alpha, int beta); 
 static int  countnodes(char *board, int start); 
 static int  dmove(char *new, int move);
 static int  done(char *board);
 static void get_game_parameters(void);
 static int  get_players_move(char *board);
 static void initb(t_board board);
 static int  mmove(char *old, char *new, int mov);
 static int  mod(int i, int j);
 static void printb(t_board board);
#else
 static int  comp();
 static int  comp1(); 
 static int  countnodes(); 
 static int  dmove();
 static int  done();
 static void get_game_parameters();
 static int  get_players_move();
 static void initb();
 static int  mmove();
 static int  mod();
 static void printb();
#endif

/*----------------------------------------------------------------------

Routine:	dmove

Description:	Perform a move and print out the resulting board.
		Return TRUE or FALSE depending on whether the
		move was a continuation move or not (see mmove)

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

static int dmove(new, mov)
char *new;
int   mov;
{
  int i;
  i = mmove(new,new,mov);
  printb(new);
  return(i);
}

/*----------------------------------------------------------------------

Routine:	done

Description:	Returns TRUE if any side is out of stones

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

static int done(board)
char *board;
{
  int c_stones;		/* Stones in computer's pits 	*/
  int p_stones;		/* Stones in player's pits	*/
  int i;

  c_stones = p_stones = 0;

  for (i=0; i<NR_PITS; i++) {
    p_stones += board[P_FIRST+i];
    c_stones += board[C_FIRST+i];
  }

  return (p_stones == 0) || (c_stones == 0);
}

/*----------------------------------------------------------------------

Routine:	get_game_parameters

Description:	ask user for various param values

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

static void get_game_parameters()
{
  char string[80];
  int  i;

  i = 17;
  while (i < LINES) {
    move(i, 0); clrtoeol();
    i++;
  }

  /*  1.  Choose difficulty level:	*/

  do {
    move(18, 0); printw("Enter difficulty level (1-%d): ", MAX_LEVEL);
    clrtoeol(); refresh();
    if ( getstr(string) == ERR) {
      printw("Error: getstr() returns error\n");  
    }
    i = atoi(string);
    if (i<1 || i>MAX_LEVEL) {
      printw("Error: level %d is out of range!\n", i);
    }
  } while (i<1 || i>MAX_LEVEL);;

  COUNT = i * FUDGE;

  /*  2.  Choose number of stones:	*/

  do {
    move(19, 0); addstr("Number of stones : "); clrtoeol();
    refresh();
    if (getstr(string) == ERR) {
      printw("Error: getstr() returns ERR\n");
    }
    i = atoi(string);
    if (i < 1) {
      printw("Error: can't play with %d stones!\n", i);
    }
  } while (i < 1);;
  NUM = i;

  /*  3.  Choose who's first to move:	*/

  do {
    move(20, 0); addstr("Do you want to go first (y or n)? "); clrtoeol();
    refresh();
    getstr(string);
    i = toupper(string[0]);
    if (i != 'Y' && i != 'N') {
      addstr("Please input 'y' or 'n'!\n");
    }
  } while (i != 'Y' && i != 'N');
  pbegins = (i == 'Y');

  /*  4.  Remove the questions:  */

  move(18, 0); clrtoeol();
  move(19, 0); clrtoeol();
  move(20, 0); clrtoeol(); 
}

/*----------------------------------------------------------------------

Routine:	get_players_move

Description:	requests a legal move from the user, and returns
		it as function value.

		legal moves for the user is in the range 
		1..NR_PITS.

		If the user enters a 'q' the special move -1
		is returned.

Problems:	The atoi() routine does not set errno on all systems.

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

static int get_players_move(board)
char *board;
{
  char  string[80];
  int	p_move;

  p_move = 0;
  while (p_move == 0) {
    move(20, 0); addstr("Your move: "); clrtoeol();
    refresh();
    getstr(string);
    if (toupper(string[0]) == 'Q') {
      p_move = -1;
    } else {
      p_move = atoi(string);
      if (p_move < 1 || p_move > NR_PITS || board[p_move] == 0) {
        addstr("Illegal!!"); BEEP();
        p_move = 0;
      }
    }
  }
  move(21, 0); clrtoeol();	/* remove any remaining 'Illegal!' lines */

  return p_move;
}

/*----------------------------------------------------------------------

Routine:	main

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */

int main(argc,argv)
int   argc;
char *argv[];
{
  int	pmove;		/* player's latest move		*/
  int	cmove;		/* computer's latest move	*/

  int	hum;		/* user's score		*/
  int	com;		/* program's score	*/

  t_board board;	/* game board		*/

  int	i;		/* scratch variables	*/
  int	errflag;
  int	c;
  char	string[100];

#if TRACE
  char  *optchars = "ab";
#else
  char  *optchars = "";
#endif

  /*  1.  Get command line arguments:		*/

#if TRACE
  ab = db = 0;
#endif
  errflag = 0;

  while ((c  = getopt(argc, argv, optchars)) != EOF) {
    switch (c) {
      default :
        fprintf(stderr, "stone(main): error in getopt()");
        exit(1);

      case '?' :	/* getopt() found illegal option letter */
        errflag = TRUE;
  	break;		/* has already complained		*/

#if TRACE
      case 'a' :
	ab = TRUE;
	break;

      case 'd' :
	ab = db = TRUE;
	break;
#endif 
    }
  }

  if (errflag || optind < argc) { /* error or arguments */
#if TRACE
    fprintf(stderr, "usage: stone [-ad]\n");
#else
    fprintf(stderr, "usage: stone\n");
#endif
    exit(1);    
  }

  initscr();

  /*  2.  Keep playing as long as user wants to:	*/

  do {
  
    /*  3.  Display an empty board:  */

    initb(board);
    printb(board);

    /*  4.  Get game parameters:	*/

    get_game_parameters();
    initb(board);

    if (!pbegins) goto first;

    /*  5.  Main move loop:	*/

    while (!done(board)) {

      /*  6.  Get player's move		*/

      do {
        pmove = get_players_move(board);
        if (pmove == -1) {		/* user entered 'q' */
          goto quit;
        }
      } while (!dmove(board, pmove) && !done(board));

      /*  7.  Get computer's move:	*/

first:
      {
        int first = 0, y, x;

        move(20, 0); addstr("I'm thinking..."); clrtoeol();
        while (!done(board)) {
          cmove = comp(board);

          if (first == 0) {
            move(19, 0); printw("Computer moves: %d", cmove-(NR_PITS+1));
              clrtoeol();
            getyx(stdscr, y, x);
            first++;
          } else {
            move(y, x);
            printw(", %d", cmove-(NR_PITS+1));
            getyx(stdscr, y, x);
          }
          
          if (dmove(board, cmove)) {
            move(20, 0); clrtoeol();	/* Not thinking anymore */
            break;
          }
        }
      }

    } /* end of main move loop */

    /*  8.  Game is over. Sum up the results:	*/

    com = board[C_KALAH]; 
    hum = board[P_KALAH];
    for (i = 1; i <= NR_PITS; i++) { 
      hum += board[i];
      com += board[NR_PITS+1+i];
    }

    move(22, 0); printw("Score:   me %d  you %d . . . ", com, hum);
    if (com > hum) {
      addstr("I win\n");
    }
    else if (com == hum) {
      addstr("game is a draw\n");
    } else {
      addstr("you win\n");
    }

quit:
    move(23, 0); addstr("New game (y or n): "); clrtoeol();
    refresh();
    getstr(string);

  } while (toupper(string[0]) == 'Y');

  endwin();  

  return 0;
}

/*----------------------------------------------------------------------

Routine:	mmove

Description:	Perform the move 'move' on the 'old' board, and
		put the new board in 'new'. 

		Return FALSE only if the move ended in a kalah.
		This indicates that the mover may move again.
		Return TRUE otherwise.

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

static int mmove(old, new, mov)
char *old; 
char *new;
int   mov;
{
  int i; 
  int who;

  /*  1.  Check that 'move' is legal on 'old':	*/

  if ( (mov < P_FIRST) || (mov == P_KALAH) ||
       (mov >= BOARD_SIZE)      ||
       old[mov] == 0) {
    printf("Bad arg to mmove: %d",mov);
  }

  /*  2.  Do some setting up:	*/

  total++;	/* Total number of moves made during tree search */

  for (i = 0; i < BOARD_SIZE; ++i) {
    new[i] = old[i];
  }

  who = (mov < P_KALAH ? 1 : 0);  /* 1 == player, 0 == computer */

  /*  3.  Make move:	*/

  i = old[mov];	/* 'i' is the stones 'in hand' */
  new[mov] = 0;

  while (i--) {
    mov = mod(mov, who);
    ++new[mov];
  }

  /*  3.  Did we make a capture? */

  if (new[mov] == 1 && who == (mov < 7 ? 1 : 0) && mov && mov != 7) {
    new[who*P_KALAH] += new[14-mov];
    new[14-mov] = 0;
  }

  /*  4.  Did the move end in a kalah? */

  return (!(mov == C_KALAH || mov == P_KALAH));

}

/*----------------------------------------------------------------------

Routine:	mod

Description:	Return the number of the pit after 'i'.

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

static int mod(i, player)
int i,player;
{
  /*  1.  Get next pit number: */

  ++i;

  /*  2.  Only player may use player's own kalah:	*/

  if (i == P_KALAH) return( player ? P_KALAH : P_KALAH+1);
  if (i >= BOARD_SIZE) return ( player ? P_FIRST : C_KALAH);

  return(i);
}

/*----------------------------------------------------------------------

Routine:	initb

Description:	Initialize the board to starting position

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

static void initb(board)
t_board board;
{
  int i;

  for (i=0; i<BOARD_SIZE; i++) {
    board[i] = NUM;
  }
  board[P_KALAH] = board[C_KALAH] = 0;
  printb(board);
}

/*----------------------------------------------------------------------

Routine:	printb

Description:	Display the kalah board on the screen.

		The board is displayed in lines 0-16,
		columns 0-77.

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

static char screen[17][78+1] = { /* Remember trailing \0! */
"+----------------------------------------------------------------------------+",
"|                                                                            |",
"|       ME        6       5       4       3       2       1                  |",
"|              +-----+ +-----+ +-----+ +-----+ +-----+ +-----+               |",
"|              |     | |     | |     | |     | |     | |     |               |",
"|              |     | |     | |     | |     | |     | |     |               |",
"|              |     | |     | |     | |     | |     | |     |               |",
"|              +-----+ +-----+ +-----+ +-----+ +-----+ +-----+               |",
"|                                                                            |",
"|              +-----+ +-----+ +-----+ +-----+ +-----+ +-----+               |",
"|              |     | |     | |     | |     | |     | |     |               |",
"|              |     | |     | |     | |     | |     | |     |               |",
"|              |     | |     | |     | |     | |     | |     |               |",
"|              +-----+ +-----+ +-----+ +-----+ +-----+ +-----+               |",
"|                 1       2       3       4       5       6        YOU       |",
"|                                                                            |",
"+----------------------------------------------------------------------------+"
};

/* 'centres' of the 0-13 pits */

static int ypos[14] = { 5, 11, 11, 11, 11, 11, 11, 11,  5,  5,  5,  5,  5,  5};
static int xpos[14] = { 8, 17, 25, 33, 41, 49, 57, 65, 57, 49, 41, 33, 25, 17};

static void printb(board)
t_board board;
{
  int i;

  /*  1.  Print empty board layout:  */

  for (i=0; i<17; i++) {
    move(i,0);
    addstr(screen[i]);
  }
  
  /*  2.  Fill in pit contents:  */

  for (i=0; i<BOARD_SIZE; i++) {
    move(ypos[i], xpos[i]);
    printw("%3d", board[i]);
  }

  /*  3.  Update board:  */

  refresh();
}

/*----------------------------------------------------------------------

Routine:	comp

Description:	given the board 'board', calulate the best
		next move and return it as result.

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

static int comp(board)
char *board;
{
  int score;
  int bestscore,best;
  char temp[14];
  int i;
  unsigned nodes;

  DEPTH = MAXDEPTH = CALLS = TOTDEPTH = WIDTH = TERM = 0;
  total = 0;

  /*  1. Only one possible move? Then select that:	*/

  if ((i = countnodes(board, C_FIRST)) == 1) {
    for (best = C_FIRST; best < C_FIRST+NR_PITS-1; ++best) {
      if (board[best]) {
        return(best);
      }
    }
  }

#if 0	/* DEBUG only */
  if (i < 1) {
    printw("ABORT! comp entered when no moves possible!");
    exit(1);
  }
#endif

  /*  2. Try each move that is left. Select the best one:	*/

  nodes = COUNT/i;
  bestscore = -10000;		/* should be MIN_INT */
  for (i = 13; i > P_KALAH; --i) {
    if (board[i]) {
      score = mmove(board,temp,i);
#if !TRACE
      score = comp1(temp, score, nodes, bestscore, 10000);
#else
      score = comp1(temp, score, nodes, db? -10000 : bestscore, 10000);
      if (db > 0) {
        printf("MOVE: %2d SCORE: %5d",
				i-P_KALAH,
			 (score>=1000)?(score-1000):
			  ((score<= -1000)?(score+1000):score));
	if (score > 1000 || score < -1000) {
          printf("  for SURE");
        }
        printf("\n");
      }
#endif
      if (score > bestscore) {
        bestscore = score;
        best = i;
      }
    }
  }

 /* - -

Indicate if we think we're winning or losing.

Comment by A. Thulin:

The program is sometimes wrong, altough it often is difficult to refute it,
especially with many stones.  The best strategy for the player to win a game
that the program thinks it has won, seems to be to play a 'waiting' game.

The fact that the program *is* wrong indicates an error somewhere, but it
does not appear to be an obvious one...

- -*/

  move(18, 0);
  if (bestscore > 1000) {
    addstr("(I think I've got you!)");
  } else if (bestscore < -1000) {
    addstr("(I think you've got me...)");
  } else {
    clrtoeol();
  }

#if TRACE
  if (db) {
    printf("\nCount: %u\n",total);
    printf("Maximum depth: %d\nAverage depth: %u\n", MAXDEPTH,TOTDEPTH/CALLS);
    printf("Terminal Evaluations: %u\nPlayed out: %u\n",WIDTH,TERM);
  }
#endif

  return(best);
}

/*----------------------------------------------------------------------

Routine:	comp1

Description:	The real alpha-beta minmax evaluator

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

static int comp1(board, who, count, alpha, beta)
char     *board;	/* current board */
int       who;		/* who is to move */
unsigned  count; 	/* max nr of positions to examine */
int       alpha,	/* ... */
          beta;		/* ... */
{
  int i;
  int turn,new;
  char temp[14];
  unsigned nodes;

  ++DEPTH;
  MAXDEPTH = MAX(MAXDEPTH,DEPTH); 

  /*  1.  If no nodes left, evaluate position statically:	*/

  if (count < 1) {		/* No remaining nodes ... */
    ++CALLS;
    TOTDEPTH += DEPTH;
    ++WIDTH;
    --DEPTH;

    new = board[C_KALAH]-board[P_KALAH];	
    for (i = 1; i < P_KALAH; ++i) {
      turn = MIN(P_KALAH-i,board[i]);
      new -= 2*turn - board[i];
    }
    for (i = P_KALAH+1; i < 14; ++i) {
      turn = MAX(14-i,board[i]);
      new += 2*turn - board[i];
    }
    if (board[C_KALAH] > 6*NUM) {
      return new+1000;
    }
    if (board[P_KALAH] > 6*NUM) {
      return new-1000;
    }
    return(new);
  }

  /*  2.  No more moves to be examined?		*/

  if (done(board)) {
    ++CALLS;
    TOTDEPTH += DEPTH;
    ++TERM;
    --DEPTH;

    new = board[0]+board[8]+board[9]+board[10]+board[11]+board[12]+board[13]
         -board[1]-board[2]-board[3]-board[4]-board[5]-board[6]-board[7];
    if (new < 0) new -= 1000;
    if (new > 0) new += 1000;
    return(new);
  }

  /*  3.  Check moves further down in tree:	*/

#if 0  /* DEBUG printout only */
  if (countnodes(board, 8-who*7) == 0) {
    printf("BREAKOFF: EMPTY BOARD! who = %d\n", who);
    printf("done(board) = %d\n", done(board));
    for (i=0; i<BOARD_SIZE; i++) {
      printf("%4d", board[i]);
    }
    exit(1);
  }
#endif

  nodes = count/countnodes(board,8-who*7);
  for (i = 7*(1-who)+6; i > 7*(1-who); --i)
    if (board[i]) {
      turn = mmove(board,temp,i);
      new = comp1(temp,(turn ? 1-who : who),nodes,alpha,beta);
#if TRACE
      if (ab) printf("DEPTH: %2d  MOVE: %2d  NEW: %4d  ALPHA: %6d  BETA: %6d\n",DEPTH,i,new,alpha,beta);
#endif
      if (who) {
        beta = MIN(new,beta);
        if (beta <= alpha) {
          DEPTH--; 
          return(beta);
        }
      }
      else {
        alpha = MAX(new,alpha);
        if (alpha >= beta) {
          DEPTH--;
          return(alpha);
        }
      }
    }
  --DEPTH;

  return(who ? beta : alpha);
}


/*----------------------------------------------------------------------

Routine:	countnodes

Description:	How many moves are possible?

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

static int countnodes(board, start) 
int start;
char *board; 
{
  int i, num;

  num = 0;
  for (i=start; i < start + NR_PITS; i++)
    num += (board[i] > 0 ? 1 : 0);
  return(num);
}

