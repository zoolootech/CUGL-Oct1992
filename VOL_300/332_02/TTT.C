/*----------------------------------------------------*- Fundamental -*-

Facility:		ttt(6)

File:			ttt.c

Associated files:	- [ none]

Description:		Plays the game of tic-tac-toe.

Author:			Leor Zolman

Editor:			Anders Thulin
			Rydsvagen 288
			S-582 50 Linkoping
			SWEDEN

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

Edit history :

Vers  Ed   Date	       By                Comments
----  ---  ----------  ----------------  -------------------------------
 1.0    0  1979-09-xx  Leor Zolman	 Written in BDS C
 1.1    1  1988-10-25  Anders Thulin     Added some comments; cleaned
				         up generally

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*---  Configuration:  --------------------------------------------------

System configuration options:
=============================

Symbol:		Conformant systems:
	
ANSI		ANSI C
BSD		BSD Unix, SunOS 3.5
SV2		AT&T UNIX System V.2, AU/X
XPG3		X/Open Portability Guide, ed. 3

If you have an ANSI C compiler, define ANSI. If not, choose one
of the other symbols.

There are no program configuration options.

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#define	ANSI	1
#define	BSD	0
#define	SV2	0
#define	XPG3	0

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#if ANSI
# include <ctype.h>
# include <stdio.h>
# include <stdlib.h>
#endif

#if BSD
# include <ctype.h>
# include <stdio.h>
  extern int rand();
  extern int srand();		/* SunOS srand returns int? */
# define EXIT_FAILURE	1
# define EXIT_SUCCESS	0
#endif

#if SV2
# include <ctype.h>
# include <stdio.h>
  extern int  rand();
  extern void srand();
# define EXIT_FAILURE	1
# define EXIT_SUCCESS	0
#endif

#if XPG3
# include <ctype.h>
# include <stdio.h>
# include <stdlib.h>
#endif

/*---  Original comments to program:  ----------------------------------

BD Software presents:

Tic Tac Toe  (by exhaustive search)

written by Leor Zolman
September 1979

This program was written for use as the crux of an article in BYTE
(as yet to be published) on BDS C, structured programming and gaming.

It is also intended to get you addicted to C so you'll go out and buy
the BD Software C Compiler

only $110 on CP/M disk from:

 	tiny c associates
	post office box 269
	holmdel, new jersey 07733

The package includes the compiler, linking loader, library manager,
standard library of functions, many interesting sample programs (such as
"TELNET.C" for using your computer as a terminal and allowing I/O directly
from modem to disk and vice-versa.) Also included is a copy of the best
reference book available on the C language, written by the original
implementors at Bell Labs (the package may be purchased without the book
for $100.)

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#define	TRUE	1
#define	FALSE	0

#define	X	1	/* The pieces on the board */
#define O	5
#define EMPTY	0

char board[9];		/* the playing board */
char BEST;		/* move returned by "ttteval" */

int wins[8][3] = {	/* table of winning positions */
	{0,1,2},
	{3,4,5},
	{6,7,8},
	{0,3,6},
	{1,4,7},
	{2,5,8},
	{0,4,8},
	{2,4,6}
     };

/*----------------------------------------------------------------------
  Local functions:
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#if __STDC__ != 0
 static int  ask(char *s);
 static int  cats(void);
 static void clear(void);
 static void display(void);
 static int  getmove(void);
 static int  ttteval(int me, int him);
 static int  win(int p);
#else
 static int  ask();
 static int  cats();
 static void clear();
 static void display();
 static int  getmove();
 static int  ttteval();
 static int  win();
#endif

/*----------------------------------------------------------------------

Routine:	ask

Description:	writes the string 's' to the terminal, and reads a
		character in reply.

		If the character is 'y' or 'Y' TRUE is returned,
		otherwise FALSE is returned.

Suggestions for improvement:

		1. Instead of returning FALSE if an error occurs,
	           we could ask again until we get a correct response.

	        2. Rather than examining only the first character
		   read, we should skip all space characters until
		   we get something solid. If the line is empty,
		   ask again.

		3. Rather than testing for 'y' we should test for 'n'.
		   The reason is only that most languages use a
		   word that begins with 'n' for 'no', while very 
		   few languages use words beginning in 'y' for 'yes'.
		   Internationalization is the word! 

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

static int ask(s)
char *s;
{
  char string[160];	/* input string */
  char c;		/* the response character */

  /*  1.  Print the string to standard output:  */

  printf(s);

  /*  2.  Read a string from standard input:  */

  if (fgets(string, sizeof(string), stdin) == 0) {	/* EOF or error */
    return FALSE;
  }

  /*  3.  Examine first character in string:  */

  c = toupper(string[0]);

  return (c == 'Y');
}

/*----------------------------------------------------------------------

Routine:	cats

Description:	This function returns true if all nine squares
		taken (usually called after checking for a win, so
		that all spaces filled indicates a cat's game)

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

static int cats()
{
  int i;

  for (i=0; i<9; ++i) {
    if (board[i] == EMPTY) {
      return FALSE;
    }
  }

  return TRUE;
}

/*----------------------------------------------------------------------

Routine:	clear

Description:	Empty the board

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

static void clear()
{
  int i;

  for (i=0; i<9; ++i) {
    board[i] = EMPTY;
  }
}

/*----------------------------------------------------------------------

Routine:	display

Description:	Displays the playing board.

Note:		The original version of the routine accepted
		a parameter that indicated if the pieces should
		be displayed (the normal case), or if the
		square numbers (1-9) should be shown instead.

		The square numbers were only shown at the
		beginning of the game, and usually scrolled out of
		sight, making it difficult to remember them.

		This version prints the square number for empty
		squares and the piece for occupied squares.

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

static void display()
{
  int r,c;	/* Not row and column, but almost ... */

  printf("\n\n");
  for (r=0; r<9; r+=3) {
    for (c=r; c< r+3; ++c) {
      putchar(' ');
      switch (board[c]) {
        default:
	  fprintf(stderr, "ttt(display): unknown piece (%d) on board (%d)",
			   board[c], c);
          exit(EXIT_FAILURE);

	case EMPTY:
	  printf("%1d", c+1);
	  break;

	case X:
	  putchar('X');
	  break;

	case O:
	  putchar('O');
	  break;
      }
      putchar(' ');
      if (c != r+2) {
        putchar('|');
      }
    }
    putchar('\n');
    if (r != 6) {
      printf("---+---+---\n");
    }
  }
  putchar('\n');
}

/*----------------------------------------------------------------------

Routine:	getmove

Description:	reads a string containing a digit, and returns
		the digit as result.

		If no digit could be read, -1 is returned.

Suggestions for improvement:

		See the description of ask() above!

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

static int getmove()
{
  char string[160];	/* input string */

  /*  1.  Read a string from standard input:  */

  if (fgets(string, sizeof(string), stdin) == 0) {	/* EOF or error */
    return -1;
  }

  /*  2.  Return value of digit:  */

  return string[0] - '0';
}

/*----------------------------------------------------------------------

Routine:	main

Description:	main program

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

int main()
{
  int i;
  int mefirst;			/* 1: computer goes first; 0: human */
  int turn;			/* counts number of moves played */
  int finished;			/* true if game is over */
  int mypiece, hispiece;	/* who has x or o */
  int mywins, hiswins, catwins;	/* game count */
  int t;

  /*  1.  Initialize:  */

  srand(0);			/* initialize random generator */
  mywins = hiswins = catwins = 0;
  mefirst = TRUE;		/* let human go first (see 2a below) */

  printf("\n\nWelcome to BDS Tic Tac Toe!!\n");
  printf("(In this version, X always goes first.\n");

  /*  2.  Outer game loop:  */

  do {

    /*  2a.  Set up for new game:  */

    finished = FALSE;
    mefirst = !mefirst;		/* reverse who goes first */
    turn = 0;
    clear();			/* clear the game board */

    if (mefirst) {
      printf("I go first...\n");
      display();		
    } else {
      printf("You go first...\n");
    }
    mypiece = mefirst ? X : O; 	/* set who has got what */
    hispiece  = mefirst ? O : X;
    if (!mefirst) goto hismove;

    /*  2b.  Inner game loop:  */

    while (!finished) {

      /*  2c.  Computer's move:  */

      if (turn == 0) {	/* Random opening move */
        BEST = rand() % 9;
      }

      if (turn == 1) {	/* Response to player's opening move */   
        if (board[4] != EMPTY) {	/* Center is occupied */
          BEST = rand() % 2 * 6 +	rand() % 2 * 2;
        } else {			/* Grab center */
          BEST = 4;
        }
      }

      if (turn > 1) {	/* OK, we're into the game... */    
        t = ttteval(mypiece, hispiece);
        if (t == 1) printf("I've got ya!\n");
      }

      board[BEST] = mypiece;  	/* make the move */
      ++turn;

      printf("I move to %d\n", BEST+1);

      if (win(mypiece)) {
        ++mywins;
        printf("\nI win!!!\n");
        finished = TRUE;
      } else if (cats()) {
        ++catwins;
        printf("\nMeee-ow!\n");
        finished = TRUE;
      }

hismove:
      if (!finished) {

        /*  2b.  Player to move:  */
 
        display();

        do {
          printf("Your move (1-9) ? ");
          i = getmove();
          if (i < 1 || i > 9 || board[i-1]) {
            printf("\nIllegal!\n");
            i = 0;	/* indicates erroneous move */
          }
        } while (i == 0);

        board[i-1] = hispiece;
        ++turn;
        display();
        if (win(hispiece)) {
          ++hiswins;
          printf("\nYou beat me!!\n");
          finished = TRUE;
        } else if (cats()) {
          ++catwins;
          printf("\nOne for Morris.\n");
          finished = TRUE;
        }
      }

    }  /* Inner game loop */

  } while (ask("\nAnother game (y/n) ? "));

  /*  3.  Print final statistics:  */

  printf("\n\nOK...Final score:\n");
  printf("You won %d game", hiswins);
  if (hiswins != 1) putchar('s');

  printf(";\nI won %d game", mywins);
  if (mywins != 1) putchar('s');

  printf(";\nThe Cat got %d game",catwins);
  if (catwins != 1) putchar('s');

  printf(".\nSee ya later!!\n");

  return EXIT_SUCCESS;
}

/*----------------------------------------------------------------------

Routine:	ttteval

Description:	The function "ttteval" returns an evaluation of
		player x's position on the tic-tac-toe board. If he's
		in a winning position, 1 is returned. If the best he
		can hope for is a cat's game, 0 is returned. And, if
		he's sure to lose if player y plays correctly, -1 is
		returned. In any case, the best move available to 
		player x is left in external variable BEST upon return
		from ttteval.

		Note that a value of -1 is often returned while
		recursive searching branches down into all possible
		wins and losses, but with the program in the shape
		it appears here, the outermost-level call to ttteval
		(from the main program) will never produce a return
		value of -1, since the computer has decided not to be
		able to lose (the obviously logical choice of any
		self-respecting problem-solver.)

		In any case, the main program still bothers to 
		consider the possibility of losing, so that if you
		want to try inserting your own "ttteval" routine
		in place of the one given, it dosn't have to be
		perfect in order to work with the main program.
		Tic-tac-toe is, of course, just about the only game
		in which it is feasible to do an exhaustive search;
		most game evaluation algorithms only go so deep and
		then make a "static" evaluation, or estimate of the
		player's status at the terminal position. That is how
		all decent chess playing programs today work.

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

static int ttteval(me, him)
int me;
int him;
{
  int i,safemove;
  int v,loseflag;
	
  /*  1.  Is game terminated?  */  

  if (win(me))  return 1;
  if (win(him)) return -1;
  if (cats())   return 0;

  /*  2.  Try out all moves and see what happens:  */

  loseflag = 1;
  for (i=0; i<9; ++i) {
    if (board[i] == EMPTY) {	/* a legal move */
      board[i] = me;		/* try the move...*/
      v = ttteval(him,me);	/* ...evaluate the resulting position */
      board[i] = EMPTY;		/* and undo the move */

      if (v == -1) { 		/* if we force a loss, yey! */
        BEST = i;
        return 1;
       }

      if (v) continue;   	/* uh oh! we shouldn't get beaten! */
      loseflag = 0;		/* cat's game guaranteed at least */
      safemove = i;
    }
  }
  BEST = safemove;
  return -loseflag;		/* if loseflag is true, this returns
				   -1 to indicate a loss; else zero
				   is returned (cat's game.)    */
}

/*----------------------------------------------------------------------

Routine:	win

Description:	returns true (non-zero) if player p has three in a row
		on the board:

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

static int win(p)
int p;
{
  int i;
  for (i=0; i<8; ++i) {
    if (board[wins[i][0]] == p &&
        board[wins[i][1]] == p &&
        board[wins[i][2]] == p) return 1;
  }
  return 0;
}


