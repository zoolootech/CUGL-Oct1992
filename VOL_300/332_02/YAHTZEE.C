/*-							-*- Fundamental -*-
 *
 *  Facility:			yahtzee
 *
 *  File:			yahtzee.c
 *
 *  Associated files:		yahtzee.hlp 	-- man page
 *
 *  Description:		Dice game yahtzee
 *
 *  Portability:		Conforms to X/Open Portability Guide, ed. 3,
 *				I hope ...
 *
 *  Author:			Steve Ward
 *
 *  Editor:			Anders Thulin
 *				Rydsvagen 288
 *				S-582 50 Linkoping
 *				SWEDEN
 *
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
 *
 *  Edit history :
 *
 *  Vers  Ed   Date	   By		     Comments
 *  ----  ---  ----------  ----------------  -------------------------------
 *   1.0    0  19xx-xx-xx  Steve Ward
 *   1.1    1  1988-10-25  Anders Thulin     Original for H89 computer -
 *					     changed to curses. Cleaned
 *					     up user interface a bit.
 *
 */

/* - - Configuration options: - - - - - - - - - - - - - - - - - - - - - - */

/*
 *  Compile-time environment:
 *
 *	ANSI		ANSI C
 *	BSD		BSD Unix, SunOS 3.5
 *	SV2		AT&T UNIX System V.2
 *	XPG3		X/Open Portability Guide, ed. 3
 *	ZTC205		Zortech C 2.05
 *
 *  If you have an ANSI C conformant compiler, use ANSI. Otherwise choose
 *  the environment that best matches yours.
 *
 */

#define	ANSI		0
#define	BSD		0
#define	SV2		0
#define	XPG3		0
#define ZTC205		1

/* - - end of configuration - - - - - - - - - - - - - - - - - - - - - - - - */

/* --- Known problems: --------------------------------------------------

curses

	Some older implementation of curses dows not have any of the
	functions beep(), nodelay() and delay_output().

	It may be possible to emulate beep() through calls similar to
	fputc(0x07, stderr).

	It will, in general, not be possible to emulate nodelay(), as
	it changes the behaviour of getch() from blocking to non-blocking(),
	that is, if no character is immediately available, return ERR.

	delay_output() just waits for a number of seconds. It is only
	used in self-play mode, so it can safely be ignored.

------------------------------------------------------------------------*/

#if ANSI
# include <ctype.h>
# include <curses.h>
# include <stdlib.h>
# include <string.h>
# include <time.h>
#endif

#if BSD
# include <ctype.h>
# include <curses.h>
# include <string.h>
typedef long time_t;
#endif

#if SV2
# include <ctype.h>
# include <curses.h>
# include <string.h>
typedef long time_t;
#endif

#if XPG3
# include <ctype.h>
# include <curses.h>
# include <stdlib.h>
# include <string.h>
# include <time.h>
#endif

#if ZTC205
# include <ctype.h>
# include <curses.h>
# include <string.h>
# include <time.h>
#endif


/* Command characters used only by auto player:  */

#define	PASSC	-1	/* Accept roll */
#define	ROLLC	-2	

/* Coordinates for screen output - assumes 24 x 80 screen */

#define	GOX	45		/* Coordinates for prompt.		*/
#define	GOY	22
#define	GAMEY	20

#define	SCOREX	30		/* leftmost column for scores		*/
#define	DIEY	22

#define	PLAYERS	10		/* Max number of players		*/

#define	NONE	(-1)		/* illegal value.		*/

#define	TOP	0
#define	MID	8
#define	BOT	16

/*  Various variables:	*/

int	 AutoPar[10];	/* heuristic parameters -HA#, -HB#, etc.	*/
int      Dice[5];	/* current roll					*/
int	 DTime;		/* delay time between moves (in seconds)	*/
int	 nplayers;	/* number of players				*/
unsigned rand_seed;	/* random number seed				*/
int 	 ShowTot;

char Potent[13];		/* Potential score...			*/
char PotKind, PotSum, PotStr;	/* Filled by Pot			*/
char PotMax, PotCnt;
int  *ShowSc;
char ShowX, ShowY;

struct Player {
   int  Scores[13];		/* Scores, or 127 iff none yet.		*/
   int  Select;			/* Currently selected category.		*/
   int  Col;			/* Column number, for scores.		*/
   char Name[40];		/* Name of player.			*/
   int  Total;			/* total score.				*/
   int  Games;
 } Who[PLAYERS];

/*  Local routines:  */

#if __STDC__
  int  Auto(int pl, int rolls);
  void Bd(int f);
  void Board(void);
  void Center(char *text, int centx, int y, int xsize);
  int  Cycle(int pl, int delta);
  void Delay(int sec);
  void Die(int number, int count);
  int  Go(int rolls, int player);
  void iplayer(char *name, struct Player *pl);
  int  Kind(int die);
  void Play(int pl);
  void Pot(int pl);
  void Roll(void);
  void Show(int pl);
  void Show1(int n, int flag);
  int  Straight(int die);
#else
  int  Auto();
  void Bd();
  void Board();
  void Center();
  int  Cycle();
  void Delay();
  void Die();
  int  Go();
  void iplayer();
  int  Kind();
  void Play();
  void Pot();
  void Roll();
  void Show();
  void Show1();
  int  Straight();
#endif

/*
 *  Routine:		Auto
 *
 *  Description:	Make move for a computer player.  
 *
 *			Note that the routine handles selection of dice
 *			to reroll internally, without bothering to 
 *			go through 'Go()'.
 *
 *			Strategy is somewhat unclear.
 *
 */

int Auto(pl, rolls)
int  pl;
char rolls;
{
  int  choice;
  int i;
  int *sc;
  char gofor, j;
  int value[13], target[6], best, n;

  sc = Who[pl].Scores;
  Pot(pl);
  mvaddstr(GOY+1, GOX, "Hmmm ... lemme think."); clrtoeol();
  Delay(DTime);

  for (i=0; i<13; i++) {
    if (sc[i] != NONE) value[i] = i-99;
    else if (i<6) value[i] = 2*(Potent[i]-3*(i+1));/* face counts */
    else if (i==6) value[i] = Potent[i]-20;	/* 3 of a kind */
    else if (i==7) value[i] = Potent[i]-15;	/* 4 of a kind */
    else if (i==9) value[i] = Potent[i]-12;	/* 4 straight  */
    else if (i==10) value[i] = Potent[i]-10;	/* 5 straight  */
    else if (i==12) value[i] = Potent[i]-21-AutoPar[5];	/* CHANCE */
    else value[i] = Potent[i]-10;
  }

  for (i=0; i<6; i++) {
    if (sc[12] == NONE) target[i] = i+i; else target[i] = 0;
    if (sc[i] == NONE) target[i] += i<<2; else target[i] -= 15;
    if ((sc[8] == NONE) && (Kind(i+1) == 3)) target[i] += 6;
    if (sc[11] == NONE) target[i] += 1<<(Kind(i+1));
    target[i] += Kind(i+1)<<3;
  }

  best = -99; gofor = PotCnt; choice = Who[pl].Select;

  for (i=0; i<13; i++) {
    if (value[i] > best) {
      best = value[i]; choice = i;
    }
  }
  Who[pl].Select = choice;
  Show(pl);

  if (rolls &&		/* Convert a straight??		*/
      (Potent[9] > 0) &&
      (Potent[10] == 0) &&
      (sc[10] == NONE)) {
    for (i=0; i<5; i++) {	/* Find the useless die.	*/
      j = Dice[i];
      Dice[i] = 0;
      Pot(pl);
      Dice[i] = j;
      if (Potent[9] > 0) {
        Die(i, 0);
        goto rollit;
      }
    }
  }

  Pot(pl);
  if ((choice > 7) && (choice < 12)) goto hit; /* End the inning.	*/

  if (rolls) {
    for (i=0; i<6; i++) {
      if ((n=(target[i]-27-AutoPar[0])) > best)	{
        gofor = i;
        choice = 255;
        best=n;
      }
    }
  }

  if ((choice == 255) || (choice < 8))
    goto maxim;		/* See if we can help.	*/


  if (!rolls) goto hit;		/* Least of evils...	*/
  for (i=0; i<5; i++) Die(i,0);	/* Re-roll entire hand.	*/
  goto rollit;

maxim:
  if (!rolls) goto hit;		/* Maximize number of chosen dice. */
  for (i=0; i<5; i++)
    if (Dice[i] && (Dice[i] != (gofor+1)))
      Die(i, 0);

rollit:
  move(GOY+1, GOX);
  printw("Baby needs shoes ... %d/%d", choice, best);
  clrtoeol();
  Delay(DTime/2);
  return ROLLC;

hit:
  move(GOY+1, GOX);
  printw("I've decided ... %d/%d", choice, best); 
  clrtoeol();
  Delay(DTime);
  return PASSC;
}

/*
 *  Routine:		Bd
 *
 *  Description:	Display empty board line.  Type of line depends
 *			on argument.
 *
 */

void Bd(f)
int f;
{
  char *cc;
  int   i;

  cc = " . ";
  if (f == 1) {
    cc = "===";
  } else if (!f) { 
#if 0
    puts("\033q");	/* normal mode */
#endif
  }
  for (i=((COLS-SCOREX)/3); i--;) {
    addstr(cc);
  }
#if 0
  puts("\033p\033G");	/* inverse mode, no graphics */
#endif
}

/*
 *  Routine:		Board
 *
 *  Description:	Print empty score-sheet
 *
 */

void Board()
{
  int i;

  clear();
  addstr("   YAHTZEE 1.1              ");
  for (i=48; i--;) {
    addch('=');
  }
  move( 1, 0); addstr("        ACES        ADD 1's "); Bd(0);
  move( 2, 0); addstr("        TWOS        ADD 2's "); Bd(0);
  move( 3, 0); addstr("        THREES      ADD 3's "); Bd(0);
  move( 4, 0); addstr("        FOURS       ADD 4's "); Bd(0);
  move( 5, 0); addstr("        FIVES       ADD 5's "); Bd(0);
  move( 6, 0); addstr("        SIXES       ADD 6's "); Bd(0);
  move( 7, 0); addstr("   Subtotal  .  .  .  .  .  "); Bd(2);
  move( 8, 0); addstr("   Bonus iff >= 63       35 "); Bd(2);
  move( 9, 0); addstr("TOTAL ABOVE   .  .  .  .  . "); Bd(2);

  move(11, 0); addstr("        3 of a kind     SUM "); Bd(0);
  move(12, 0); addstr("        4 of a kind     SUM "); Bd(0);
  move(13, 0); addstr("        Full House       25 "); Bd(0);
  move(14, 0); addstr("        4 Straight       30 "); Bd(0);
  move(15, 0); addstr("        5 Straight       40 "); Bd(0);
  move(16, 0); addstr("        YAHTZEE          50 "); Bd(0);
  move(17, 0); addstr("        Chance          SUM "); Bd(0);
  move(18, 0); addstr("TOTAL SCORE   .  .  .  .  . "); Bd(2);

  move(GAMEY, 0); addstr("GAMES   .  .  .  .  .  .  . "); Bd(1);
}

/*
 *  Routine:		Center
 *
 *  Description:	Write a string centered about the x coordinate.
 *
 */

void Center(text, centx, y, xsize)
char *text;
int   centx, y, xsize;
{
  int l;

  l = strlen(text);
  if (l > xsize) l = xsize;

  move(y, centx - l/2);
  addch(' ');
  while (*text && l--) addch(*text++);
  addch(' ');
}

/*
 *  Routine:		Cycle
 *
 *  Description:	Alter the screen selection of a player. The
 *			delta parameter is 1 for moving downwards, or
 *			-1 for upwards.
 *
 */

int Cycle(pl, delta)	
int pl;
int delta;
{
  int tries;	/* Remaining nr of tries to find empty selection */
  int s;	/* Player's current/new selection */

  s  = Who[pl].Select;
  tries = 14;
  do {
    s += delta;
    if (s < 0) {
      s = 12;
    } else if (s > 12) {
      s = 0;
    }
  } while (--tries > 0 && (Who[pl].Scores[s] != NONE));
  Who[pl].Select = s;

  return tries;	/* 0 if no more selections available */
}

/*
 *  Routine:		Delay
 *
 *  Description:	Wait for a number of seconds
 *
 */

void Delay(s)
int s;
{
#ifndef __POWERC
  delay_output(1000*s);
#endif
}

/*
 *  Routine:		Die
 *
 *  Description:	Print a die face with COUNT dots in the NUMBER
 *			place.
 *
 */

void Die(number, count)
int number;
int count;
{
  int i;

  if (count != 0) {
    attron(A_REVERSE);
  }

  for (i=0; i<3; i++) {			/* Once for each row on the die */
    move(DIEY+i, 4+(7*number));
    switch (i*8 + count) {
      case TOP+0:	/* count is 0 (empty) for re-rolled */
      case MID+0:
      case BOT+0:
      case TOP+1:
      case BOT+1:
      case MID+2:
      case MID+4:
        addstr("     ");
        break;
      case TOP+2:
      case TOP+3:
        addstr("*    ");
        break;
      case MID+1:
      case MID+3:
      case MID+5:
        addstr("  *  ");
        break;
      case BOT+2:
      case BOT+3:
        addstr("    *");
        break;
      case TOP+4:
      case BOT+4:
      case TOP+5:
      case BOT+5:
      case TOP+6:
      case MID+6:
      case BOT+6:
        addstr("*   *");
        break;
    }
  }
  Dice[number] = count;

  if (count != 0) {
    attroff(A_REVERSE);
  }
}

/*
 *  Routine:		Go
 *
 *  Description:	Command interpreter. Accepts commands either
 *			from player or auto-player and performs them.
 *
 *  Note:		Auto-player does some internal command handling
 *
 */

int Go(rolls, player)
int rolls, player;
{
  int  ch;		/* command */
  int  i;
  int  save[5];		/* copy of latest roll  */
  int  reroll;		/* true if user has selected dice to reroll */

  Pot(player);
  for (i=0; i<5; i++) {
    Dice[i] = 0;
  }
  Roll();
  Who[player].Select = 12;

  if (!Cycle(player, 1)) return 0;

Top:
  for (i=0; i<5; i++) save[i] = Dice[i];

  Show(player);
  reroll = FALSE;

  for (;;) {
    move(GOY, GOX);
    printw("%s's move: %d rolls left. ", Who[player].Name, rolls);
    clrtoeol();
esc:
    /*  n.  Get and execute command:  */

    refresh();
    if (Who[player].Name[0] == '=') {
      ch = Auto(player, rolls);
    } else {
      ch = toupper(getch());
    }

    switch (ch) {
      case 033:
        goto esc;

      case ' ':			/* Go 'down' */
        Cycle(player, 1);
        Show(player);
        continue;

      case '1':			/* select die to be rerolled */
      case '2':
      case '3':
      case '4':
      case '5':
        if (rolls == 0) {	/* sorry - no rolls left */
          beep();
        } else {
          Die(ch - '1', 0);
	  reroll = TRUE;
        }
        continue;

      case 0x08:		/* Backspace */
      case 0x7F:		/* Delete == undo selection */
        for (i=0; i<5; i++) Die(i, save[i]);
        continue;

      case ROLLC:		/* make new roll - from Auto() */
          if (rolls>0) { Roll(); rolls--; }
	  else beep();
          goto Top;

      case '\r':
      case '\n':
      case PASSC:
	if (reroll) {	/* Make new roll */
          if (rolls>0) { Roll(); rolls--; }
	  else beep();
          goto Top;
	} else {	/* Accept latest roll & selection */
          Play(player);
          Who[player].Select = NONE;
          Show(player);
          return ch;
        }

      default:		/* unknown input */
        beep();
        continue;
    }
  }
}


/*
 *  Routine:		iplayer
 *
 *  Description:	Initialize a player
 *
 */

void iplayer(name, pl)
char          *name;
struct Player *pl;
{
  char i, *fake;

  fake = "==0==";
  if (!name[1] && *name == '=') {
    name = fake; name[3]++;
  }
  for (i=0; i<13; i++) {
    pl->Scores[i] = NONE;
  }
  pl->Select = NONE;

  strcpy(pl->Name, name);  
}

/*
 *  Routine:		Kind
 *
 *  Description:	Gives number of dice of count die
 *
 */

int Kind(die)		
int die;
{
  int count, i;

  count = 0;
  for (i=0; i<5; i++) {
    if (die == Dice[i]) {
      count++;
    }
  }

  return count;
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
  int arg;
  int i;
  char j,*carg;
  int n;

  /*  0.  Initialize:  */

  for (i=0; i<PLAYERS; i++) Who[i].Games = 0;

  for (i=0; i<10; i++) AutoPar[i] = 0;	
  DTime = 2;		

  initscr();
  cbreak();			/* return keypresses immediately */
  nodelay(stdscr, FALSE);	/* read blocks */
  
  rand_seed = time((time_t *)0);

  for (arg=1; (arg<argc) && (*(carg = argv[arg]) == '-'); arg++) {
    switch (*++carg) {
      case 'R':
	rand_seed = atoi(++carg);
        continue;
      case 'H':
        i = (*++carg)-'A';
	AutoPar[i] = atoi(++carg);
        continue;
      case 'D':
        DTime = atoi(++carg)|1;
        continue;
      default:
        continue;
    }
  }

  srand(rand_seed);

Again:
  nplayers = 0;
  for (i=arg; i<argc; i++) {
    iplayer(argv[i], &Who[nplayers++]);
  }
  if (nplayers == 0) {
    iplayer("You", &Who[nplayers++]);
  }
  n = (80-SCOREX)/nplayers;
  for (i=SCOREX+(n/2), j=0; j<nplayers; j++, i += n) Who[j].Col = i;
  Board();
  for (j=0; j<nplayers; j++) Center(Who[j].Name, Who[j].Col, 0, n-1);
  for (i=0; i<nplayers; i++) Show(i);

  for (;;) {
    for (i=0; i<nplayers; i++) {
      if (!Go(2, i)) {
        for (i=n=0; i<nplayers; i++)
          if (Who[i].Total > n)
            n = Who[i].Total;
        for (i=0; i<nplayers; i++)
          if (Who[i].Total >= n) {
            move(GAMEY, Who[i].Col);
            standout();	  
            printw("%3d", ++(Who[i].Games));
          }
        move(GOY+1, GOX);
        for (i=0; i<nplayers; i++)
	  if (Who[i].Total >= n)
            printw("%s ", Who[i].Name);
        beep();
        addstr("WINS!"); 
        standend();
        addstr(" Play again?"); clrtoeol();
        if (toupper(getch()) == 'Y') goto Again;
        endwin();
        exit(0);
      }
    }
  }
}

/*
 *  Routine:		Play
 *
 *  Description:	Make a player's move.
 *
 */

void Play(pl)		
int pl;
{
  Pot(pl);
  Who[pl].Scores[Who[pl].Select] = Potent[Who[pl].Select];
}

/*
 *  Routine:		Pot
 *
 *  Description:	Compute potential scores for player.
 *
 *  Notes:		A. Thulin:
 *			Odd that the parameter isn't used ...
 */

void Pot(pl)		
int pl;
{
  int i,j,k;

  for (i=0; i<13; i++) Potent[i] = 0;
  for (i=0; i<5; i++) if (((j = Dice[i])>0) && (j<7)) Potent[j-1] += j;
  for (PotMax=PotSum=i=PotKind=0; i<5; i++)
   { PotKind |= (1<<(j=Kind(k=Dice[i]))); PotSum += Dice[i];
     if (j>PotMax) { PotMax=j; PotCnt=k; }}
  Potent[12] = PotSum;
  if ((PotKind & 0xC) == 0xC) Potent[8] = 25;
  if (PotKind & 0x38) Potent[6] = PotSum;
  if (PotKind & 0x30) Potent[7] = PotSum;
  if (PotKind & 0x20) Potent[11] = 50;
  for (i=PotStr=0; i<5; i++) if ((j = Straight(Dice[i])) > PotStr)
  	PotStr=j;
  if (PotStr >= 4) Potent[9] = 30;
  if (PotStr == 5) Potent[10] = 40;
}

/*
 *  Routine:		Roll
 *
 *  Description:	Roll the dice ...
 *
 */

void Roll()
{
  int i;
  for (i=0; i<5; i++) {
    if (Dice[i] == 0) {
      Die(i, 1+rand()%6);
    }
  }
}

/*
 *  Routine:		Show
 *
 *  Description:	Show the scores of a player
 *
 */

void Show(pl)
char pl;
{
  int            f;
  struct Player *p;
  int cc;
  char tot, j;

  Pot(pl);
  p = &Who[pl];
  ShowX = p->Col-1;	ShowY = 1;	ShowTot = 0;
  ShowSc = p->Scores;	cc = 0;		f = p->Select;
  Show1(cc++,f--);
  Show1(cc++,f--);
  Show1(cc++,f--);
  Show1(cc++,f--);
  Show1(cc++,f--);
  Show1(cc++,f--);
  tot = ShowTot;
  Show1(255,44);
  ShowTot = j = tot>=63? 35:0;
  Show1(255,44);
  tot = ShowTot = tot+j;
  Show1(255,44);
  ShowTot = tot;
  ShowY++;
  Show1(cc++,f--);
  Show1(cc++,f--);
  Show1(cc++,f--);
  Show1(cc++,f--);
  Show1(cc++,f--);
  Show1(cc++,f--);
  Show1(cc++,f--);
  p->Total = ShowTot;
  Show1(255,44);
}

/*
 *  Routine:		Show1
 *
 *  Description:	...
 *
 */

void Show1(n, flag)
int n, flag;
{
  int val;

  move(ShowY++, ShowX);

  if (n == 255) {
    val = ShowTot;
  } else {
    val = ShowSc[n];
  }

  if (!flag) { 
    standout();
    val = Potent[n];
  } else if (flag == 44) { 
    standout();
  }
  if (val == NONE) { 
    addstr("  ? ");
  } else { 
    printw(" %2d", val);
    ShowTot += val;
  }
  if (!flag || (flag == 44)) {
    standend();
  }
}

/*
 *  Routine:		Straight
 *
 *  Description:	Gives longest straight starting at die
 *
 */

int Straight(die)	
int die;
{
  int i, j;

  for (i=1; i<6; i++) {
    die++;
    for (j=0; j<5; j++) {
      if (Dice[j] == die) goto hit;
    }
    break;
hit:
    continue;
  }
  return i;
}




