
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

/* ---> JOTTO - converted from Pascal version

   Copyright (c) 1980, by David M. Fogg - Portland, Ore.

   13 Nov 80: creation day
   18 Nov: creation...finished
   19 Nov: bugwork
*/

#include <std.h>

#define LETS	  5	/* letters/word */
#define ISCORE	  21	/* initial score */
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
#define MAXWORDS  600	/* max # of words in computer list */

/*
   * * *  G L O B A L S  * * *
*/
   BOOL wordok, giveup;
   int score;
   char word[LETS+3];
   char guess[LETS+1], gots[LETS+1];
   char nots[27], maybes[27];
   int count;
   char guesslis[ISCORE+1][LETS+1];
   int changed;

main (ac, av)
int ac;
char *av[];
{
   int numwords;
   char wordlist[MAXWORDS+1][LETS+1];
   char ibuf[BUFSIZ];
   char wfil[15];		 /* word file name */
   char c;

   if (ac < 2)				  /* set wordfile name */
      strcpy(wfil, "jotto.wds");
   else
      strcpy(wfil, av[1]);

   if (fopen(wfil, ibuf) == ERROR)	  /* open wordfile */
      errxit("Word file I/O error");

   numwords = 0;			  /* read wordfile */
   while (fgets(word, ibuf) != NULL) {
      word[LETS] = NULL;
      strcpy(wordlist[numwords++], word);
   }
   fclose(ibuf);

   nrand(0, "Hit RETURN sometime...");	  /* crank up random # gen */
   getchar(); puts("thanx\n");

   FOREVER {				  /* --> MAIN LOOP <-- */
      score = ISCORE;
      nots[0] = NULL;
      strcpy(maybes, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
      strcpy(gots, "     ");

      doscreen();
      inword(XPROMPT, YPROMPT, "Your word: ", word);
      if (strcmp(word, "?????") == 0)
	 strcpy(word, wordlist[nrand(1) % numwords]);
      toxy(XPROMPT, YPROMPT); puts("                    ");
      do {
	 doguess();
	 if (strcmp(guess, word) != 0 && !giveup) dochange();
	 if (wordok) --score;
      } while (strcmp(guess, word) != 0 && score > 0 && !giveup);
      toxy(XPROMPT, YPROMPT - 1);
      if (strcmp(guess, word))
	 printf("The word was: %s.", word);
      else
	 printf("Congrats: you got %d points!", ++score);

      if (nomore()) break;
   }
}


/*
   =========<<< SUBROUTINES >>>=========
*/

doscreen ()		      /*** --<SET UP THE SCREEN>-- ***/
{
   puts(CLEAR);
   toxy(XTITLE, YTITLE); puts("--> J O T T O  <--");
   toxy(XSCORE, YHEAD); puts("SCORE  GUESS  #  STATE\n");
   toxy(XSCORE, YHEAD+1); puts("=====  =====  =  =====");
   toxy(XLABELS+3, YYES); puts("YES:");
   toxy(XLABELS+4, YNO); puts("NO:");
   toxy(XLABELS+1, YPOSS); puts("MAYBE:");
   toxy(XLISTS, YPOSS); printset(maybes);
   toxy(XINST, YINST); puts("<?????> to 'Your word:' gets random word");
   toxy(XINST, YINST+1); puts("<?????> instead of guess aborts game");
   toxy(XINST, YINST+2); puts("Change:");
   toxy(XINST+2, YINST+3); puts("<A> allows re-entry of bad word");
   toxy(XINST+2, YINST+4); puts("<+X> adds 'X' to YES list");
   toxy(XINST+2, YINST+5); puts("<-X> takes 'X' off YES list");
   toxy(XINST+2, YINST+6); puts("<NX> move 'X' MAYBE->NO");
   toxy(XINST+2, YINST+7); puts("<MX> move 'X' NO->MAYBE");
   toxy(XINST+2, YINST+8); puts("<RETURN> performs changes (if any)");
   toxy(XINST, YINST+10); puts("STATE: upcase=YES; locase=maybe; '.'=NO");
}


printset (s)		/*** --<OUTPUT s IN ALFA ORDER>-- ***/
char *s;
	 /* O/P space if ltr missing, else ltr if in gots, else tolower */
{
   char str;

   for (str = 'A'; str <= 'Z'; ++str)
      if (inset(s, str))
	 putchar(inset(gots, str) ? str : tolower(str));
      else
	 putchar(' ');
}


inword (x, y, p, w)	   /*** --<TO x/y, PROMPT p, KB INP->w>-- ***/
int x, y;
char *p, *w;
{
   toxy(x, y); puts(p); getword(w);
}


getword (wd)		   /*** --<GET 5-LETTER wd FROM KB>-- ***/
char *wd;
{
   int i;

   for (i = 0; i < LETS; ++i) wd[i] = getlet();

   wd[LETS] = NULL;
}


char getlet ()			/*** --<GET A VALID KB LETTER>-- ***/
{
   char ch;

   do {
      ch = toupper(coin());
   } while (!isalpha(ch) && ch != '?' && ch != '/');
   if (ch == '/') ch = '?';
   putchar(ch);
   return (ch);
}


doguess ()	     /*** --<PROCESS PLAYER'S GUESS>-- ***/
{
   int i, j, y;
   char str[LETS+1];

   y = YFIRST + ISCORE - score;

   toxy(XSCORE, y); printf("%4d", score);
   inword(XGUESS-1, y, " ", guess);
   if (strcmp(guess, "?????") == 0)
      giveup = YES;
   else {
      giveup = NO;
      strcpy(str, word); count = 0;
      for (i = 0; i < LETS; ++i) {
	 if ((j = inset(str, guess[i])) > 0) {
	    ++count;
	    str[j-1] = tolower(str[j-1]);
	 }
      }
      printf("%3d", count);
      strcpy(guesslis[score], guess);
      dostate(score);
   }
}


dostate (s)		      /*** --<UPD STATE FOR SCORE s>-- ***/
int s;
{
   int i, j;
   char ch;
   char ges[LETS+1], got[LETS+1];

   toxy(XSTATE, YFIRST + ISCORE - s);
   strcpy(ges, guesslis[s]); strcpy(got, gots);

   for (i = 0; i < LETS; ++i) {
      ch = ges[i];
      if ((j = inset(got, ch)) > 0) {
	 putchar(ch);
	 strcpy(got+j-1, got+j);
      }
      else
	 putchar((inset(nots, ch) > 0) ? '.' : tolower(ch));
   }
}


modgot (old, new)	      /*** --<MODIFY old->new IN gots>-- ***/
char old, new;
{
   int j;

   if ((j = inset(gots, old)) > 0) gots[--j] = new;
}


xfer (l, src, dest)	      /*** --<XFER l FROM src->dest STRING>-- ***/
char l;
char *src, *dest;
{
   int j;

   if ((j = inset(src, l)) > 0) {
      strcpy(src+j-1, src+j);
      dest[j = strlen(dest)] = l; dest[++j] = NULL;
   }
}


dochange ()			 /*** --<DO CHANGE(S)>-- ***/
{
   int sc;
   char let, ch;

   changed = 0;
   wordok = NO;

   toxy(XPROMPT, YPROMPT); puts("Change: ");
   do {
      ch = toupper(coin());
   } while (inset("A+=-_NM\r", ch) == 0);
   if (ch == 'A')
      return;
   else
      wordok = YES;

   while (ch != '\r' && changed < LETS) {
      ++changed;
      switch (ch) {
	 case '=': ch = '+'; break;
	 case '_': ch = '-';
      }
      putchar(ch);
      let = getlet(); putchar(' ');
      switch (ch) {
	 case '+': modgot(' ', let); break;
	 case '-': modgot(let, ' '); break;
	 case 'N': xfer(let, maybes, nots); break;
	 case 'M': xfer(let, nots, maybes);
      }
      do {
	 ch = toupper(coin());
      } while (inset("A+=-_NM\r", ch) == 0);
   }
   if (changed > 0) {
      toxy(XPROMPT, YPROMPT); puts(EREOL);
      toxy(XLISTS, YYES); puts(gots);
      toxy(XLISTS, YNO); printset(nots);
      toxy(XLISTS, YPOSS); printset(maybes);
      for (sc = ISCORE; sc >= score ; --sc) dostate(sc);
      dochange();
   }
}


BOOL nomore ()		   /*** --<SEE IF DONE PLAYING>-- ***/
{
   char ch;

   toxy(XPROMPT, YPROMPT); puts("Play again? ");

   do {
      ch = toupper(coin());
   } while (ch != 'Y' && ch != 'N');
   putchar(ch);

   return (ch == 'N' ? YES : NO);
}
