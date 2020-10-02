/*-							-*- Fundamental -*-
 *
 *  Facility:			jotto(6)
 *
 *  File:			jotto.c
 *
 *  Associated files:		- [ none]
 *
 *  Description:		Plays the game of jotto
 *
 *  Notes:			No rules included.
 *
 *  Portability:		Edited to conform to X/Open Portability Guide,
 *				ed. 3, 1988
 *
 *  Author:			David M. Fogg
 *				2632 N.E. Fremont
 *				Portland, OR 97212
 *
 *  Editor:			Anders Thulin
 *				Rydsvagen 288
 *				S-582 50 Linkoping
 *				SWEDEN
 *
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
 *
 *  Copyright (c) 1981 by David M. Fogg
 *
 *  Permission is herewith granted for noncommercial distribution
 *  through the BDS User's Group; any and all forms of commercial
 *  redistribution are strenuously unwished-for.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
 *
 *  Edit history :
 *
 *  Vers  Ed   Date	   By		     Comments
 *  ----  ---  ----------  ----------------  -------------------------------
 *   1.0    0  1980-11-13  David M. Fogg     Creation day
 *          1  1980-11-18  David M. Fogg     Creation ... finished
 *          2  1980-11-10  David M. Fogg     Bugwork
 *   1.1    3  1988-10-25  Anders Thulin     Many BDS-isms removed to make
 *					     program K&R compatible. General
 *					     cleanup for portability. Added
 *					     some comments.
 *					
 */

/* - - Configuration options: - - - - - - - - - - - - - - - - - - - - - - - */

/*
 *	
 *  The following environments are supported:
 *
 *	ANSI		ANSI C
 *	BSD		Berkeley UNIX
 *	SV2		AT&T UNIX System V.2
 *	XPG3		X/Open Portability Guide, ed. 3
 *	ZTC205		MS-DOS + Zortech C 2.05
 *
 *  If you have an ANSI C conforming compiler, define ANSI. Otherwise,
 *  use the definition that best matches your environment.
 *
 */
	
#define	ANSI		0	/* 1 -> ANSI C, 0 -> K&R */
#define	BSD		0
#define	SV2		0
#define	XPG2		0
#define ZTC205		1

#if ANSI
# include <ctype.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <time.h>
#endif

#if BSD
# include <ctype.h>
# include <stdio.h>
# include <string.h>
 typedef long  time_t;
 extern void   exit();
 extern int    rand();
 extern void   srand();
 extern time_t time();		
#endif

#if SV2 
# include <ctype.h>
# include <stdio.h>
# include <string.h>
 typedef long  time_t;
 extern void   exit();
 extern int    rand();
 extern void   srand();
 extern time_t time();		
#endif

#if XPG3
# include <ctype.h>
# include <stdio.h>
# include <string.h>
 typedef long  time_t;	/*[???] or should it be time_t from <sys/types.h> ? */
 extern void   exit();
 extern int    rand();
 extern void   srand();
 extern time_t time();	
#endif

#if ZTC205
# include <ctype.h>
# include <stdio.h>
# include <string.h>
 typedef long  time_t;
 extern void   exit();
 extern int    rand();
 extern void   srand();
 extern time_t time();	
#endif

# include <curses.h>

/*
 *  Tweakables:
 *
 *  The reason ALPHA_SIZE has been declared as a macro is that some
 *  compilers handle sizeof("<string>") as a synonym to sizeof(char *),
 *  which is wrong.  ALPHA_SIZE should be defined to be the number
 *  of characters in the string ALPHABET.
 *
 */

#define WORD_FILE	"jotto.wds"	/* default word file */
#define MAXWORDS  	600		/* max nr of words in word file */

#define	ALPHABET	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define	ALPHA_SIZE	26

/* Code for RETURN/ENTER key -- some systems use \r, some \n */

#define RETURN		'\n'

/*
 *  Non-tweakables:
 *
 */

#ifndef TRUE
# define	TRUE	1
# define	FALSE	0
#endif

#define LETS	  5	/* letters/word */
#define ISCORE	  21	/* initial score */

/* screen layout parameters */

#define XTITLE	  40	/* Title loc */
#define YTITLE	  1
#define XPROMPT   33	/* prompt loc */
#define YPROMPT   4
#define XSCORE	  3	/* score col */
#define XGUESS	  10	/* guess col */
#define XMATCH	  17	/* match col */
#define XSTATE	  20	/* status col */
#define YHEAD	  0	/* header row */
#define YFIRST	  2	/* first guess row */
#define XLABELS   33	/* list labels col */
#define XLISTS	  42	/* lists cols */
#define YYES	  6	/* confirmed letters row */
#define YNO	  8	/* eliminated letters row */
#define YPOSS	  10	/* possible letters row */
#define XINST	  30	/* instructions loc */
#define YINST	  13

/*
 *  Global variables
 *
 */

static int  wordok, giveup;
static int  score;
static char word[LETS+3];
static char guess[LETS+1], gots[LETS+1];
static int  count;
static char guesslis[ISCORE+1][LETS+1];
static int  changed;

static char nots[ALPHA_SIZE],
            maybes[ALPHA_SIZE];

static char alphabet[] = ALPHABET;

/*
 *  Local functions:
 *
 */

#ifdef __STDC__
 static void dochange(void);
 static void doguess(void);
 static void doscreen(void);
 static void dostate(int s);
 static int  getlet(void);
 static void getword(char *wd);
 static int  inset(char *string, int ch);
 static void inword(int x, int y, char *p, char *w);
 static void modgot (int old, int new);
 static int  nomore(void);
 static void printset(char *s);
 static void xfer (int l, char *src, char *dest);
#else
 static void dochange();
 static void doguess();
 static void doscreen();
 static void dostate();
 static int  getlet();
 static void getword();
 static int  inset();
 static void inword();
 static void modgot ();
 static void printset();
 static int  nomore();
 static void xfer ();
#endif

/*
 *  Routine:		dochange
 *
 *  Description:	Read change commands from the terminal and
 *			perform the changes.
 *
 */

static void dochange ()
{
   int  ch;
   int sc;
   char let;
   static char legals[] = {'A', '+', '=', '_', 'N', '+', RETURN, '\0'};

   changed = 0;
   wordok = FALSE;

   mvaddstr(YPROMPT, XPROMPT, "Change: "); refresh();
   do {
     if (islower(ch = getch())) {
       ch = toupper(ch);
     }    
   } while (inset(legals, ch) == 0);
   refresh();
   if (ch == 'A')
      return;
   else
      wordok = TRUE;

   while (ch != RETURN && changed < LETS) {
      ++changed;
      switch (ch) {
	 case '=': ch = '+'; break;
	 case '_': ch = '-';
      }
      addch(ch);
      let = getlet(); addch(' ');
      switch (ch) {
	 case '+': modgot(' ', let);        break;
	 case '-': modgot(let, ' ');        break;
	 case 'N': xfer(let, maybes, nots); break;
	 case 'M': xfer(let, nots, maybes); break;
      }
      refresh();
      do {
	 ch = toupper(getch());
      } while (inset(legals, ch) == 0);
      refresh();
   }
   if (changed > 0) {
      move(YPROMPT, XPROMPT); clrtoeol();
      move(YYES, XLISTS); addstr(gots);
      move(YNO, XLISTS); printset(nots);
      move(YPOSS, XLISTS); printset(maybes);
      for (sc = ISCORE; sc >= score ; --sc) dostate(sc);
      dochange();
   }
}

/*
 *  Routine:		doguess
 *
 *  Description:	Input guess from user and process it.
 *
 */

static void doguess ()	
{
  int i, j, y;
  char str[LETS+1];

  /*  1.  Get guess from user:  */

  y = YFIRST + ISCORE - score;
  move(y, XSCORE); printw("%4d", score);
  inword(XGUESS-1, y, " ", guess);

  /*  2.  Process guess:  */

  if (strcmp(guess, "?????") == 0) {
    giveup = TRUE;
  } else {
    giveup = FALSE;
    strcpy(str, word); count = 0;
    for (i = 0; i < LETS; ++i) {
      if ((j = inset(str, guess[i])) > 0) {
        ++count;
        str[j-1] = tolower(str[j-1]);
      }
    }
    printw("%3d", count);
    strcpy(guesslis[score], guess);
    dostate(score);
 }
}

/*
 *  Routine:		doscreen
 *
 *  Description:	Clears the screen. Prints guess columns, some
 *			help text and various other stuff.
 *
 */

static void doscreen()
{
  clear();

  mvaddstr(YTITLE, XTITLE, "--> J O T T O  <--");

  mvaddstr(YHEAD,   XSCORE, "Score  Guess  #  STATE");
  mvaddstr(YHEAD+1, XSCORE, "=====  =====  =  =====");

  move(YYES, XLABELS+3);  addstr("YES:");

  move(YNO, XLABELS+4);   addstr("NO:");

  move(YPOSS, XLABELS+1); addstr("MAYBE:");
  move(YPOSS, XLISTS);    printset(maybes);

  mvaddstr(YINST,    XINST, "<????\?> to 'Your word:' gets random word");
  mvaddstr(YINST+1,  XINST, "<????\?> instead of guess aborts game");
  mvaddstr(YINST+2,  XINST, "Change:");
  mvaddstr(YINST+3,  XINST, "  <A> allows re-entry of bad word");
  mvaddstr(YINST+4,  XINST, "  <+X> adds 'X' to YES list");
  mvaddstr(YINST+5,  XINST, "  <-X> takes 'X' off YES list");
  mvaddstr(YINST+6,  XINST, "  <NX> move 'X' MAYBE->NO");
  mvaddstr(YINST+7,  XINST, "  <MX> move 'X' NO->MAYBE");
  mvaddstr(YINST+8,  XINST, "  <RETURN> performs changes (if any)");
  mvaddstr(YINST+10, XINST, "STATE: upcase=YES; locase=maybe; '.'=NO");
}


/*
 *  Routine:		dostate
 *
 *  Description:	Print the 'state' of the guessed word for
 *			score s.
 *
 */

static void dostate (s)
int s;
{
   int i, j;
   char ch;
   char ges[LETS+1], got[LETS+1];

   move(YFIRST + ISCORE - s, XSTATE);
   strcpy(ges, guesslis[s]);
   strcpy(got, gots);

   for (i = 0; i < LETS; ++i) {
      ch = ges[i];
      if ((j = inset(got, ch)) > 0) {
	 addch(ch);
	 strcpy(got+j-1, got+j);
      }
      else
	 addch((inset(nots, ch) > 0) ? '.' : tolower(ch));
   }
}

/*
 *  Routine:		getlet
 *
 *  Description:	Read a valid character from the terminal,
 *			convert it to upper case, and return it.
 *
 *			Non-valid characters are not echoed.
 *
 */

static int getlet()
{
  int ch;

  do {
    ch = getch();
    if (islower(ch)) {
      ch = toupper(ch);
    }
  } while (!isalpha(ch) && ch != '?');

  addch(ch);
  refresh();
  return (ch);
}


/*
 *  Routine:		getword
 *
 *  Parameter:		wd	storage space for word
 *
 *  Description:	Reads a LETS-letter word from terminal.
 *		  
 */

static void getword (wd)
char *wd;
{
  int i;

  refresh();

  for (i = 0; i < LETS; ++i) {
    wd[i] = getlet();
  }

  wd[LETS] = NULL;
}


/*
 *  Routine:		inset
 *
 *  Parameters:		str	a reference string
 *			c	a character
 *
 *  Description:	Return the position of the first occurrence of
 *			c in str. 
 *
 *			If c does not appear in str, 0 is returned.
 *
 *  Note:		inset("ABC", 'A') == 1
 *
 */

static int inset(str, c)
char *str;
int   c;
{
  char *p;

  if ((p = strchr(str, c)) == 0) {
    return 0;
  } else {
    return p-str+1;
  }
}

/*
 *  Routine:		inword
 *
 *  Parameters:		x, y	screen coordinates
 *			p	prompt string
 *			w	input word
 *
 *  Description:	Prints a prompt string, and reads one word from
 *			the console. One word contains exactly LETS
 *			letters.
 *
 */

static void inword (x, y, p, w)
int x, y;
char *p, *w;
{
  mvaddstr(y, x, p); getword(w);
}

/*
 *  Routine:		main
 *
 *  Description:	...
 *
 */

int main (ac, av)
int ac;
char *av[];
{
  FILE *wf;
  int numwords;
  char wordlist[MAXWORDS+1][LETS+1];
  char wfil[15];		 /* word file name */

  /*  0.  Read the jotto words:  */

  if (ac < 2)				  /* set wordfile name */
    strcpy(wfil, WORD_FILE);
  else
    strcpy(wfil, av[1]);

  if ((wf = fopen(wfil, "r")) == NULL) {	/* open word file */
    fprintf(stderr, "jotto: failed to open word file '%s'\n", wfil);
    exit(1);
  }

  numwords = 0;			  /* read wordfile */
  while (fgets(word, sizeof(word), wf) != NULL) {
    word[LETS] = '\0';
    strcpy(wordlist[numwords++], word);
  }
  fclose(wf);

  /*  1.  Initialize various things:  */

  initscr();		/* init curses */
  noecho();
  cbreak();

  srand((unsigned int) time((time_t *) 0));

  /*  2.  Main loop:  */
  
  while (TRUE)  {	
    score = ISCORE;
    nots[0] = NULL;
    strcpy(maybes, alphabet);
    strcpy(gots, "     ");

    doscreen();
    inword(XPROMPT, YPROMPT, "Your word: ", word);
    if (strcmp(word, "?????") == 0) {
      strcpy(word, wordlist[rand() % numwords]);
    }
    mvaddstr(YPROMPT, XPROMPT, "                    ");
    do {
      doguess();
      if (strcmp(guess, word) != 0 && !giveup) dochange();
      if (wordok) --score;
    } while (strcmp(guess, word) != 0 && score > 0 && !giveup);
    move(YPROMPT-1, XPROMPT);
    if (strcmp(guess, word))
      printw("The word was: %s.", word);
    else
      printw("Congrats: you got %d points!", ++score);

    if (nomore()) break;
  }

  endwin();	

  return 0;
}

/*
 *  Routine:		modgot
 *
 *  Parameters:		old
 *			new
 *
 *  Description:	if gots[] contains the character old, then
 *			replace it with the character 'new'.
 *
 */

static void modgot (old, new)
char old, new;
{
   int j;

   if ((j = inset(gots, old)) > 0) gots[--j] = new;
}


/*
 *  Routine:		nomore
 *
 *  Description:	Ask if the user wishes another game. 
 *			Return TRUE if he doesn't.
 *
 */

static int nomore ()
{
  int ch;

  mvaddstr(YPROMPT, XPROMPT, "Play again? [y/n] ");

  refresh();

  do {
     if (islower(ch = getch())) {
        ch = toupper(ch);
     }
  } while (ch != 'Y' && ch != 'N');
  addch(ch);

  refresh();

  return (ch == 'N' ? TRUE : FALSE);
}

/*
 *  Routine:		printset
 *
 *  Parameter:		s 	a string with uppercase letters, not
 *				necessarily in alphabetical order.
 *
 *  Description:	Print the letters in s in alphabetical order
 *			leaving spaces between non-consecutive letters.
 *
 *			Letters in s that also are in gots[] are printed
 *			in upper case. Otherwise in lower case.
 *
 */

static void printset (s)
char *s;
{
  int i;
  char c;

  for (i = 0; i < ALPHA_SIZE; i++) {	
    c = alphabet[i];
    if (inset(s, c)) {
      addch(inset(gots, c) ? c : tolower(c));
    } else {
      addch(' ');
    }
  }
}

/*
 *  Routine:		xfer
 *
 *  Parameters:		l	A character
 *			src	The source string
 *			dest	The destination string
 *
 *  Description:	Remove the character l from src and add it to
 *			dest.
 *
 *			If l does not appear in src, do nothing.
 *
 */

static void xfer (l, src, dest)
char l;
char *src, *dest;
{
  int j;

  if ((j = inset(src, l)) > 0) {
    strcpy(src+j-1, src+j);
    dest[j = strlen(dest)] = l; dest[++j] = NULL;
  }
}
