/*
 * HYPHEN.C
 *
 * Hyphenation module. Adapted from the Bell Labs hyphenation algorithm.
 * Suffix and digram tables taken from several public domain sources,
 * including DECUS RUNOFF.
 *
 * Basically a toy until a decent exception list is added.
 *
 * Edits:
 *    01 13-May-81 RBD  Initial edit.
 */
#include <stdio.h>
#include <ctype.h>

#define NULL 0

/*
 * Custom C control construct: cycle/recycle/quit
 */

#define cycle while(1) {
#define recycle continue
#define quit break
#define endcycle }

/*
 * External statics and functions.
 */
extern char *wdstart, *wdend;          /* Pointers to 1st and last chars */
extern char *hyptr[];                  /* Table of ptrs to hyp pos's */
/* extern char tolower(); */                /* Fold character to lower case */

/*
 * Internal statics.
 */
static char *hyend;                    /* Working scan pointer */
static char **hyp;                     /* Scan ptr for hyptr table */

/*
 * Digram tables, table of pointers to suffix tables.
 */
extern char bxh[26][13], bxxh[26][13], xxh[26][13], xhx[26][13], hxx[26][13];
extern char *suftab[];

#define THRESH 160                     /* Digram goodness threshold */

/*
 * Mark word for hyphenation.
 *
 * Inputs:
 *       wp = pointer to start of word to hyphenate.
 *
 * Outputs:
 *       returns 0
 *       wdstart --> first letter
 *       wdend   --> last letter
 *       hyend   --> letter before first hyphenation break
 *       hyptr[] --> locations where hyphenation is OK
 */
hyphen(wp)
char *wp;
   {
   register char *ch;
   register char **h;
   register int i;
/*
 * Skip by leading punctuation
 */
   ch = wp;
/******** TEMP
   while(punct(*ch++))
      ;
*/
   ch++;    /* Fake it */
/*
*********/

/*
 * If word doesn't start with alpha, don't hyphenate.
 */
   if (!isalpha(*--ch))
      return;
/*
 * Mark the special places in the word.
 */
   wdstart = ch++;                     /* wdstart --> 1st alpha */
   while(isalpha(*ch++))
      ;
   hyend = wdend = --ch-1;             /* hyend --> last letter */

/******** TEMP **********
   while(punct(*ch++))  */               /* (skip trailing punctuation) */
   ch++; /* Fake it */
/*      ;
*************************/
/*
 * Don't hyphenate unless that was trailing punctuation, not
 * punctuation within a word. Assumes word ends in NULL. Also,
 * don't hyphenate if word is shorter than 5 characters.
 */
   if (*--ch)
      return;
   if ((wdend-wdstart-4) < 0)
      return;

   hyp = hyptr;                        /* hyp --> first cell in hyptr */
   *hyp = NULL;                        /* hyptr[0] <-- 0 */

   suffix();                           /* Mark suffix based hyp points */
   digram();                           /* Mark usable digrams in the word */
   *hyp++ = NULL;                      /* Terminate the list with a NULL */

   /*
    * Do a bubble sort on the hyphenation pointers in hyptr[]
    * so they point to spots in the word from left to right.
    */
   if (*hyptr)
      for(i = 1; i;)
         {
         i = 0;
         for(h = hyptr+1; *h != 0; h++)
            {
            if (*(h-1) > *h)
               {
               i++;
               ch = *h; *h = *(h-1); *(h-1) = ch;
               }
            }
         }
   }

/*
 * Suffix test and processing. Handles multiple suffices, such as
 * "ationalism".
 */
suffix()
   {
   register char *s, *s0, *w;
   char ch;
   char *chkvow();

   /*
    * Do this from right to left, recognizing suffix forms which make
    * up a concatenated multi-suffix.
    */
   cycle
      /*
       * Make ch = lower case last character for this suffix try.
       */
      if (!isalpha(ch = *hyend))       /* Done if non-alpha */
         return(0);
      /*
       * Point s0 at suffix table for the last letter in this word.
       * If there is no suffix table, return failure.
       */
      if ((s0 = suftab[tolower(ch)-'a']) == NULL)
         return(0);
      /*
       * Test word for match to nodes in suffix table. Done if end of
       * table reached.
       */
      cycle
         if ((ch = *s0 & 017) == '\0') /* ch = # chars in this node */
            return(0);                 /* Done if none (end of table) */
         s = s0 + ch - 1;              /* s --> last char in node */
         w = hyend - 1;                /* w --> next to last char in word */

         while(((s > s0) && (w >= wdstart)) && ((*s & 0177) == tolower(*w)))
            {
            s--;
            w--;
            }

         if (s == s0)                  /* If the entire suffix matched ... */
            quit;                      /* ... quit the cycle. Found one. */
         s0 += ch;                     /* No match, go to next node */
      endcycle

      s = s0 + ch - 1;                 /* s --> last char in node */
      w = hyend;                       /* w --> last char in word */
      if (*s0 & 0200) goto mark;       /* If hyp flag set, mark hyp spot */
      /*
       * Mark the places where it can be hyphenated.
       */
      while(s > s0)
         {
         --w;
         if (*s-- & 0200)              /* If this is a legal hyp spot */
            {
mark:                                  /* Mark it as a hyphenation spot */
            hyend = w - 1;             /* hyend --> word b4 hyp spot */
            /*
             * If bit 6 set in first cell, it means
             * supress actual hyphenation.
             */
            if (*s0 & 0100)            /* If supress, keep moving left */
               continue;
            if (!chkvow(w))            /* Done if no vowels precede this */
               return(0);
            *hyp++ = w;                /* Fill in a hyphenation pointer */
            }
         }
/*
 * End of suffix node. If bit 5 set in first cell, supress multi
 * suffix processing, done. Otherwise, look further to the left
 * for more suffices and hyphenation points.
 */
      if (*s0 & 040)
         return(0);
   endcycle
   }

/*
 * Test if the supplied letter is a vowel.
 */
vowel(ch)
int ch;
   {
   switch(tolower(ch))
      {
      case 'a':
      case 'e':
      case 'i':
      case 'o':
      case 'u':
      case 'y':
         return(1);
      default:
         return(0);
      }
   }

/*
 * Check if any letters to the left of *w are vowels.
 */
char *chkvow(w)
char *w;
   {
   while(--w >= wdstart) if(vowel(*w)) return(w);
   return(0);
   }

/*
 * Digram processing. Magic here.
 */
digram()
   {
   register char *w;
   register int val;
   char *nhyend, *maxw;
   int maxval;
   char *chkvow();

   cycle
      if (!(w=chkvow(hyend+1))) return;
      hyend = w;
      if (!(w=chkvow(hyend))) return;
      nhyend = w;
      maxval = 0;
      w--;
      while((++w < hyend) && (w < (wdend-1)))
         {
         val = 1;
         if (w == wdstart) val *= dilook('a',*w,bxh);
         else if(w == wdstart+1) val *= dilook(*(w-1),*w,bxxh);
         else val *= dilook(*(w-1),*w,xxh);
         val *= dilook(*w, *(w+1), xhx);
         val *= dilook(*(w+1), *(w+2), hxx);
         if (val > maxval)
            {
            maxval = val;
            maxw = w + 1;
            }
         }
      hyend = nhyend;
      if (maxval > THRESH)*hyp++ = maxw;
   endcycle
}

dilook(a,b,t)
char a, b;
char t[26][13];
   {
   register char i, j;

   i = t[tolower(a)-'a'][(j = tolower(b)-'a')/2];
   if (!(j & 01))i >>= 4;
   return(i & 017);
   }