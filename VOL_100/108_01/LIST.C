
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

/*
	  ---> TEXT FILE LISTING PROGRAM <---

   Coded by David M. Fogg @ New Day Computing Co., Portland, OR

	 *** (C) COPYRIGHT 1980, New Day Computing Co. ***

   24 OCT 80: creation day
   5 Nov - add optional line numbering
   6 NOV - Chg to xpand tabs
   21 Nov: Filter Form Feeds
   8 Jan 81: add line folding & opt line length spec
   6 Mar: chg DEFLL->128; chg puts->printf 2 show actual DEFLL

    USAGE: list filnam "Title String"  {list a single file}
	   list [no arguments]	 {list multiple files: title/filnam prompts}
		[no arguments] allows changing:
				 line numbering   (default: NO)
				 line length	  (default: 70)
				 initial formfeed (default: NO)

*/

#include <std.h>

#define MAXF   64	/* MAX # OF FILES */
#define MAXFNL 15	/* MAX FILNAM LENGTH (INCL NUL TERMINATOR) */
#define MAXLNS 60	/* MAX LINES/PAGE */
#define NSTARS 85	/* # OF ASTERISKS IN STAR LINE */
#define TABX	8	/* TAB EXPANSION MODULUS */
#define DEFLL  128	/* DEFAULT LINE LENGTH */

main (argc, argv)
int argc;
char *argv[];
{
   char fnbuf[MAXFNL*MAXF];	 /* file name buffer */
   char *pfnbuf;		 /* pointo curr pos in fnbuf */
   char *filnam[MAXF];		 /* file names (pointers) */
   char titbuf[100];		 /* page title (usu. incl. date) buffer */
   char *titl;			 /* pointer to title */
   char stars[NSTARS+1];	 /* asterisk line */
   int fno;			 /* file # being listed */
   int nfils;			 /* no. of files to list */
   int pagno;			 /* curr page # */
   int lino;			 /* curr line # on page */
   char iobuf[BUFSIZ];		 /* file I/O buffer */
   int filopn;			 /* holds fopen result */
   char lbuf[MAXLINE];		 /* input line buffer */
   BOOL dilin;			 /* whether to disp line #s */
   int lines;			 /* tot lines in curr file */
   int linlen;			 /* maximum line length */

   pfnbuf = fnbuf;
   titl = titbuf;
   setmem(stars, NSTARS, '*'); stars[NSTARS] = '\0';
   dilin = NO;
   linlen = DEFLL;

   if (argc > 1) {
      nfils = 1;
      filnam[0] = *++argv;
      if (argc > 2)
	 titl = *++argv;
      else
	 titbuf[0] = '\0';
   }
   else {
      dilin = getyn(0, 0, "Line Numbers");
      printf("Line Length (%d): ", DEFLL);
      scanf("%d", &linlen); if (linlen < 1) linlen = DEFLL;
      puts("\nTitle: "); gets(titl);
      nfils = 0;
      do {
	 printf("File # %2d : ", nfils + 1);
	 filnam[nfils] = gets(pfnbuf);
	 pfnbuf += strlen(pfnbuf) + 1;
      } while (strlen(filnam[nfils++]) > 0 && nfils < MAXF);
      --nfils;
      if (nfils == 0)
	 errxit("Nothing to do...quitting");
      puts("\nWant a Formfeed? ");
      if (toupper(getchar()) == 'Y') putc(FFEED, LST);
      putchar('\n');
   }


/*
	 >>------> MAIN PRINT LOOP <------<<
*/

   for (fno = 0; fno < nfils; ++fno) {
      if ((filopn = fopen(filnam[fno], iobuf)) == ERROR) {
	 printf("*** File I/O Error: %-15s\n", filnam[fno]);
	 continue;
      }
      pagno = 0; lines = 0;
      lino = MAXLNS + 3;
      printf("---> Now listing %-15s\n", filnam[fno]);
      while (fgets(lbuf, iobuf) != 0) {
	 if (lino > MAXLNS) {
	    if (pagno == 0) {
	       if (fno > 0) putc(FFEED, LST);
	       fputs(stars, LST); fputs("\n", LST);
	       lino = 1;
	    }
	    else {
	       putc(FFEED, LST);
	       lino = 0;
	    }
	    fprintf(LST, "\n---> Listing of: %-15s    ", filnam[fno]);
	    fprintf(LST, "%-35s  ", titl);
	    fprintf(LST, "Page %2d <---\n", ++pagno);
	    lino += 2;
	    if (pagno == 1) {
	       fputs("\n", LST); fputs(stars, LST); fputs("\n", LST);
	       lino += 2;
	    }
	    fputs("\n\n\n", LST); lino += 3;
	 }
	 if (dilin)
	    fprintf(LST, "%4d: ", ++lines);
	 oplin(lbuf, linlen, &lino);
	 ++lino;
      } /* while (fgets... */

      fclose(iobuf);
   } /* for (fno... */

   if (filopn != ERROR) putc(FFEED, LST);
} /* main() */


oplin (lbuf, len, lno)	  /* --<O/P A LINE W/ TAB XPANSION>-- */
char lbuf[];
int len;
int *lno;
{
   int cpos;	  /* curr printhead pos */
   int i;

   cpos = 0;
   for (i = 0; i < strlen(lbuf); ++i) {
      if (lbuf[i] == '\n')
	 fputs("\n", LST);
      else if (lbuf[i] == '\t') {
	 putc(' ', LST);
	 while (++cpos % TABX) putc(' ', LST);
      }
      else if (lbuf[i] != FFEED) {
	 putc(lbuf[i], LST);
	 if (++cpos > len) {
	    fputs("\\\n", LST);
	    cpos = 0;
	    ++*lno;
	 }
      }
   }
}
