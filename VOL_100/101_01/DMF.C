
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

/* ---> DMF.C <--- : personal BDS C system library functions

   15 OCT 80: convert from WhiteC version
   23 Oct: c/STRCMP/CMPSTR/ 2 elim BDS lib conflict; + INSTR
   26 Oct: + COIN, ZCHAR
   29 Oct: + OCLOSE: housekeeping close buff ofile
   4 Nov: NDC updates/fixes: add igcase to CMPSTR & to calls
	    in QUICK, REHEAP; fix REHEAP; fix OCLOSE
   19 Nov: + INSET: as INSTR, but 2nd arg is char (kinda sim Pascal sets)
   20 Nov: + GETYN: prompt/get Y/N answer
   22 Nov: + GETFIL: get/check filename
   25 Nov: chg COIN 2.X BDOS call -> X.X BIOS call
*/

#include <std.h>

/* NB: modules in alphabetical order */

/*------------------------------------------------------------*/
cmpstr (s, t, bf, ic)		     /* --<COMPARE 2 STRINGS>-- */
char *s, *t;   /* pointers to strings */
BOOL bf;       /* backward compare flag */
BOOL ic;       /* ignore case flag */
/*
	RETURNS: <0 if s<t, 0 if s==t, >0 if s>t
		(if bf==YES, s & t are switched)
	       (if ic==YES, s & t are tolowered)
*/
{
   if (ic) {
      for (; tolower(*s) == tolower(*t); s++, t++)
	 if (*s == NULL)
	    return (0);
      return (bf ? tolower(*t) - tolower(*s) : tolower(*s) - tolower(*t));
   }
   else {
      for (; *s == *t; s++, t++)
	 if (*s == NULL)
	    return (0);
      return (bf ? *t - *s : *s - *t);
   }
}


/*------------------------------------------------------------*/
char coin ()		   /* --<NON-ECHOING CONSOLE INPUT>-- */
{
   char c;

   return (bios(CONIN, 0));
}


/*------------------------------------------------------------*/
errxit (msg)		   /* --<PRINT MESSAGE & BOMB>-- */
char *msg;
{
   puts(VIDINV);
   puts("\007** "); puts(msg); puts(" **\007\n");
   puts(VIDNOR);
   exit ();
}


/*------------------------------------------------------------*/
getfil (prom, name)	/* --<GET/CHECK FILE name>-- */
char *prom, *name;
{
   int fd;
   char tnam[MAXLINE];

   do {
      printf("%s: ", prom); gets(tnam);
   } while (strlen(tnam) > MAXNAM);
   strcpy(name, tnam);

   fd = open(name, READ); close(fd);
   return(fd);
}


/*------------------------------------------------------------*/
BOOL getyn (x, y, prom) 	/* --<PROMPT 4 Y/N ANSWER>-- */
int x,y;
char *prom;
{
   char ans;

   if (x != 0) toxy(x, y);    /* set x=0 2 skip cursor pos */
   printf("%s (Y/N)? ", prom);

   do {
      ans = toupper(coin());
   } while (ans != 'Y' && ans != 'N');
   putchar(ans); putchar('\n');

   return (ans == 'Y' ? YES : NO);
}


/*------------------------------------------------------------*/
imax (a, b)		    /* --<RETURN int MAX OF a & b>-- */
int a, b;
{
   return (b > a ? b : a);
}


/*------------------------------------------------------------*/
imin (a, b)		    /* --<RETURN int MIN OF a & b>-- */
int a, b;
{
   return (b < a ? b : a);
}


/*------------------------------------------------------------*/
inset (str, chr)	   /* --<RET POS OF chr IN str>-- */
char *str, chr;
		  /* RET 0 if no match, or chr==NULL */
{
   int i, l;

   if (chr == NULL) return (0);

   l = strlen(str);
   for (i = 0; i < l; ++i)
      if (str[i] == chr) return (++i);

   return (0);
}


/*------------------------------------------------------------*/
instr (str, pat)	/* --<RET CHARPOS OF pat IN str>-- */
char *str, *pat;
		  /* RET 0 if no match, or pat == "" */
{
   int ls, lp, i, j;
   BOOL ok;

   lp = strlen(pat);
   if (lp == 0) return (0);

   ls = strlen(str) - lp + 1;

   for (i = 0; i < ls; ++i) {
      ok = YES;
      for (j = 0; j < lp; ++j)
	 if (*(str+i+j) != *(pat+j)) {
	    ok = NO;
	    break;
	 }
      if (ok) return (++i);
   }
   return (0);
}


/*------------------------------------------------------------*/
oclose (iobuf)		      /* --<CLOSE BUFF O/P FILE>-- */
char *iobuf;
		  /* & do all housekeeping */
{
   putc(CPMEOF, iobuf);
   fflush(iobuf);
   fclose(iobuf);
}


/*------------------------------------------------------------*/
quick(linptr, bot, top, back, igcas)	       /* --<QUICKSORT>-- */

char **linptr; /* base of array of line pointers */
int bot, top;  /* range of elements to sort */
BOOL back;     /* if YES, sort in descending order */
BOOL igcas;    /* if YES, ignore case */
/*
	This routine calls itself recursively to sort
	the text lines pointed at by the segment of
	linptr between elements top and bot.
*/
{
   char *pivlin;	/* pointer to pivot line */
   int lo;
   int hi;
   int range;
   
   lo = bot - 1;
   hi = top;
   range = top - bot;

   if (range <= 0)		/* aught to do? */
      return;

   if (range == 1) {		/* only 2 elts? */
      if (cmpstr(linptr[bot], linptr[top], back, igcas) > 0)
	 swap(&linptr[bot], &linptr[top]);
      return;
   }

   swap(&linptr[top], &linptr[bot + range / 2]);
   pivlin = linptr[top];		/* set pivot line */

   while (lo < hi) {		/* QUICKSORT */
      for (++lo; cmpstr(linptr[lo], pivlin, back, igcas) < 0; ++lo)
	 ;
      for (--hi; hi > lo && cmpstr(linptr[hi], pivlin, back, igcas) > 0; --hi)
	 ;
      if (lo < hi)
	 swap(&linptr[lo], &linptr[hi]);
   }
   swap(&linptr[lo], &linptr[top]);	/* put pivot in midl */

   hi = lo;				/* fat pivot section */
   while (lo-- > bot && cmpstr(linptr[lo + 1], linptr[lo], back, igcas) == 0)
      ;
   while (hi++ < top && cmpstr(linptr[hi - 1], linptr[hi], back, igcas) == 0)
      ;

   if (lo - bot < top - hi) {	/* sort shorter segment first */
      quick(linptr, bot, lo, back, igcas);
      quick(linptr, hi, top, back, igcas);
   }
   else {
      quick(linptr, hi, top, back, igcas);
      quick(linptr, bot, lo, back, igcas);
   }
}


/*--------------------------------------------------------------*/
reheap (linps, nlins, back, igcas)     /* --<RE-FORM A HEAP>-- */
char *linps[]; /* pointers to lines */
int nlins;     /* # of lines */
BOOL back;     /* backward (=descending) collating sequence flag */
BOOL igcas;    /* ignore case flag */
/*
	The first line pointed to by linps is trickled down
	until it is in place; the rest of the lines are
	assumed to be in proper order.
*/
{
   int i;

   for (i = 1; i < nlins; ++i)
      if (cmpstr(linps[i-1], linps[i], back, igcas) > 0)
	 swap(&linps[i-1], &linps[i]);
      else
	 break;
}


/*------------------------------------------------------------*/
swap (a, b)			/* --<EXCHANGE 2 STRING POINTERS>-- */
char **a, **b;	/* pointers to string pointers */
{
   char *t;

   t = *a;
   *a = *b;
   *b = t;
}


/*------------------------------------------------------------*/
toxy (x, y)			/* --<POSITION CURSOR TO X,Y>-- */
char x, y;
				 /* --> Zenith Z-19 Version <-- */
{
   x = abs(x) % HSIZ + 32;
   y = abs(y) % VSIZ + 32;

   puts(CURLEA); putchar(y); putchar(x);
}
/*------------------------------------------------------------*/
char zchar ()			 /* --<GET Z-19 KB CHAR>-- */

			/* --> RET LC IF FUNKEY, ELSE UC <-- */
{
   char c;

   return (((c = coin()) == ESC) ? tolower(coin()) : toupper(c));
}
