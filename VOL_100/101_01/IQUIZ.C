
/**********************************************************
 ***							***
 ***	Copyright (c) 1981 by David M. Fogg		***
 ***							***
 ***		2632 N.E. Fremont			***
 ***		Portland, OR 97212			***
 ***							***
 ***	    (503) 288-3502{HM} || 223-8033{WK}		***
 ***							***
 ***	Permission is herewith granted for non- 	***
 ***	commercial distribution through the BDS C	***
 ***	User's Group; any and all forms of commercial   ***
 ***	redistribution are strenuously unwished-for.	***
 ***							***
 **********************************************************/

/* ---> IQUIZ <--- : UCSD Pascal -> BDS C conversion

   24 Nov 80 - creation day
   29 Nov: more creation
   30 Nov: final touches

This version differs from the Pascal one in that the entire
question file is read into memory - which means: 1) that the
question file is NOT a random-access file, and 2) that
file maintenance is done with a standard editor, rather than
in this program.

      Original & adaptation conceived & coded by:
      David M. Fogg - Portland, Oregon

	 COPYRIGHT (C) 1980, DAVID M. FOGG

*/

#include <std.h>

#define  KBPORT   17	/* Keyboard Data Port *** SYSTEM DEPENDENCY ***/

#define  YQBAS	  11	/* y-ord of 1st maybe */
#define  STARCL   100	/* starting klok kount */
#define  QLINES   3
#define  MAYBES   4
#define  MAXQ	  333	/* maximum # of questions */
#define  KBDATA   17	/* Keyboard Data Port *** SYSTEM-DEPENDENT ***/
#define  FUGIT	  3000	/* default clock tarditude factor */
#define  ROUND	  10	/* default questions/round */

/*
   # # # # #  GLOBALS  # # # # #
*/
char *qline[MAXQ], *maybe[MAXQ];
char *curmeb[MAYBES];	   /* current answers in shuffled order */
int numrex;		   /* # records in memory */
int asked;		   /* # asked so far (quit when == numrex) */
BYTE answer;		   /* screen loc of answer */
int rec;		   /* current question from file */
int order[MAYBES];	   /* screen order of possible answers */
int tempus;
char higuy[20], loboy[20];
int hiscore, loscore;
char player[20];	   /* hi/low scorers, curr player */
int score;
int fugit;


main (ac, av)
int ac;
char *av[];
{
   BYTE round;		      /* # questions in a round */
   BYTE qnum;		      /* question # in round */
   BOOL missed; 	      /* whether s/he's missed once already */
   BYTE kount;		      /* kurrent klok kount */
   BYTE guess;		      /* current guess */
   char olgess; 	      /* prev guess (to tell if he hit a key) */
   char filnam[MAXNAM];
   BOOL noteof;
   char ibuf[BUFSIZ];
   char *memtop;	      /* hiest avail memloc 4 Q/A lns */
   char *mp;		      /* Mem Ptr used while loading Q/A lns */
   BYTE i;
   int l;

   putchar(' ');	/* force loading of local version */

   round = ROUND; fugit = FUGIT;
   noteof = YES;
   asked = numrex = 0;
   loscore = 9999; hiscore = -1;
   memtop = topofmem() - 1000;
   mp = sbrk(1);

/*
   ---> DETERMINE/OPEN/READ QUESTION/ANSWER FILE <---
*/
   if (ac < 2)
      strcpy(filnam, "iquiz.dat");
   else
      strcpy(filnam, av[1]);
   if (fopen(filnam, ibuf) == ERROR)
      errxit("Can't open file");

   while (noteof && numrex < MAXQ && mp < memtop)
      for (i = 0; i < QLINES+MAYBES; ++i) {
	 if (fgets(mp, ibuf) == 0) {
	    noteof = NO;
	    break;
	 }
	 if (i == 0) qline[numrex] = mp;
	 if (i == QLINES) maybe[numrex++] = mp;
	 l = strlen(mp);
	 mp[l-1] = NULL;
	 mp += l;
      }

   printf("\nExtra Memory: %u", memtop - mp);
   printf("\n%d questions\n\n", numrex);
   puts("Questions/round(default: 10) - "); scanf("%d", &round);
   if (round < 1) round = ROUND;
   puts("Clock delay rate (default: 3000) - "); scanf("%d", &fugit);
   if (fugit < 1) fugit = FUGIT;
   nrand(0, "Hit return someday..."); getchar();
   puts(CLEAR);

/*
   >>>--------->  MAIN LOOP  <---------<<<
*/
   do {
      puts("Player's name, or 'Q'(uit) - "); gets(player);
      if (toupper(player[0]) == 'Q')
	 break;
      puts("RETURN to start..."); getchar();
      score = 0; qnum = round;
      do {
	 puts(CLEAR); puts(CUROFF);
	 toxy(30,1); printf("< QUESTION # %d >", qnum);
	 toxy(5,3); printf("Score: %d", score);
	 toxy(50,3); puts("Count --->");
	 showq(rec = getq());
	 missed = NO; kount = STARCL; guess = -1;
	 do {
	    toxy(63,3); puts(VIDINV); printf("%3d ", kount); puts(VIDNOR);
	    guess = (inp(KBPORT) & 127) - '1';
	    if (guess < 0 || guess >= MAYBES) olgess = guess;
	    if (guess != answer && olgess != guess) {
	       hilite(3, guess * 2 + YQBAS, "{NO}");
	       olgess = guess;
	       if (missed)
		  kount = 1;
	       else {
		  kount = kount / 2 + 1;
		  missed = YES;
	       }
	    }
	    tempus = fugit; while (--tempus);
	 } while (--kount && guess != answer);
	 if (kount) ++kount;
	 score += kount;
	 toxy(5,3); printf("Score: %d", score);
	 toxy(63,3); puts(VIDINV); printf("%3d ", kount); puts(VIDNOR);
	 if (kount) {
	    hilite(3, answer * 2 + YQBAS, "YES!");
	 }
	 hilite(13, answer * 2 + YQBAS, maybe[rec]);
	 puts(CURON);
	 if (!getyn(10, 21, "Continue"))
	    qnum = 1;
	 toxy(10, 21);
	 if (kbhit()) getchar();
	 qline[rec] = NULL;
	 ++asked;
      } while (--qnum);
      shostat();
   } while (asked < numrex-round && toupper(player[0]) != 'Q');
   if (round > numrex-asked) {
      puts("\n\nToo few questions for a new round: RETURN to exit - ");
      getchar();
   }
   puts(POCRT);
}

/*
   =========< SUBROUTINES >=========
*/

hilite (x, y, msg)	   /* ---<PRINT msg IN INVIDEO>--- */
BYTE x, y;
char *msg;
{
   toxy(x, y); printf("%s%s%s", VIDINV, msg, VIDNOR);
}


getq () 		   /* ---<RETURN RANDOM QUESTION #>--- */
{
   int r;

   r = nrand(1) % numrex;

   while (qline[r] == NULL) r = (r + 1) % numrex;

   return (r);
}


showq (qno)		   /* ---<SHOW QUESTION & MAYBES>--- */
int qno;
{
   BYTE i, t;
   char *l;
   int qlen;
   char anset[10];

   qlen = 0;
   strcpy(anset, "0123");

   for (i = 0; i < MAYBES; ++i) {      /* shuffle answers */
      t = nrand(1) % MAYBES;
      while (inset(anset, t + '0') == 0) t = (t+1) % MAYBES;
      order[i] = t;
      anset[t] = 'X';
   }
   answer = order[0];

   l = maybe[qno];		    /* retrieve answers in shuffled order */
   for (i = 0; i < MAYBES; ++i) {
      curmeb[order[i]] = l;
      l += strlen(l) + 1;
   }

   l = qline[qno];	      /* display question & answers */
   for (i = 0; i < QLINES; ++i) {
      toxy(5, 6 + i); puts(l);
      qlen += strlen(l);
      l += strlen(l) + 1;
   }
   for (i = 0; i < MAYBES; ++i) {
      toxy(9, i * 2 + YQBAS);
      printf("%d : %s", i + 1, curmeb[i]);
   }

   tempus = fugit * 9 + qlen * 99;
   while (--tempus);	/* givum time to read it b4 starting the clock */
}


shostat ()	     /* ---<DISPLAY STANDINGS>--- */
{
   toxy(10,21); puts("RETURN to see standings - "); getchar();

   if (score > hiscore) {
      hiscore = score; strcpy(higuy, player);
   }
   if (score < loscore) {
      loscore = score; strcpy(loboy, player);
   }

   puts(CLEAR);
   printf("High scorer: %-20s   {%4d }\n\n", higuy, hiscore);
   printf("Curr player: %-20s   {%4d }\n\n", player, score);
   printf(" Low scorer: %-20s   {%4d }\n\n", loboy, loscore);
   printf("QUESTIONS - used: %d   left: %d\n\n", asked, numrex - asked);
}

putchar(c)	  /* ---<MAKE putchar() BE putch()>--- */
char c;
{
   putch(c);
}
