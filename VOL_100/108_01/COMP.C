
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

/* ---> COMP.C - COMPARE 2 FILES & LIST DIFFERENCES <--- */

/* 26 OCT 80: Adapt from WC version */

/***********************************************

usage: comp filea fileb [flags]

results in format: blank line/line #/filea line/fileb line

flags:	 -a: omit filea line
	 -b: omit fileb line
	 -l omit blank line & line #
	 -s output if lines same instead of diff
	    (turns on -b)

***********************************************/

#include <dmfio.h>

main (ac, av)
int ac;
char *av[];
{
   char *arg;				  /* pointo flag arg */
   BOOL skipno, skipa, skipb, shosam;	  /* option flags */
   char iobufa[BUFSIZ], iobufb[BUFSIZ];   /* file buffers */
   char lina[MAXLINE], linb[MAXLINE];	  /* line buffers */
   BOOL same;				  /* if lina==linb */
   BOOL eofa, eofb;			  /* wether EOF yet */
   BOOL msga, msgb;			  /* if EOF msg sent yet */
   unsigned lines;			  /* line counter */
   char eoform[30];			  /* format string 4 EOF msgs */

   skipno = skipa = skipb = shosam = eofa = eofb = msga = msgb = NO;
   lines = 0;
   strcpy(eoform, "\nEOF on %s after %u lines\n");

   if (ac < 3) {
      puts("usage: comp fileA fileB [options]\n");
      puts("options: -a: omit A lines; -b: omit B lines\n");
      puts("         -l: omit line #s; -s: list A lines if A=B\n");
      exit ();
   }

   if (fopen(av[1], iobufa) == ERROR)
      errxit("File A I/O Error");
   if (fopen(av[2], iobufb) == ERROR)
      errxit("File B I/O Error");

   while (ac > 3 && *(arg = av[--ac]) == '-')
      switch (toupper(*++arg)) {
	 case 'A': skipa = YES; break;
	 case 'S': shosam = YES;
	 case 'B': skipb = YES; break;
	 case 'L': skipno = YES; break;
	  default: errxit("Bad Flag");
      }

/*
	 >>------> MAIN LOOP <------<<
*/

   while (!(eofa && eofb)) {
      if (!eofa)
	 if (fgets(lina, iobufa) == NULL) eofa = YES;
      if (!eofb)
	 if (fgets(linb, iobufb) == NULL) eofb = YES;
      if (eofa && eofb)
	 break;

      ++lines;

      same = strcmp(lina, linb) == 0;
      if (shosam == same) {
	 if (!skipno) printf("\n--> %u\n", lines);
	 if (!skipa && !eofa) puts(lina);
	 if (!skipb && !eofb) puts(linb);
      }

      if (eofa && !msga) {
	 msga = YES;
	 printf(eoform, "File A", lines - 1);
      }
      if (eofb && !msgb) {
	 msgb = YES;
	 printf(eoform, "File B", lines - 1);
      }
   }

   if (msga)
      printf(eoform, "File B", lines);
   else if (msgb)
      printf(eoform, "File A", lines);
   else
      printf(eoform, "Both Files", lines);

   fclose(iobufa); fclose(iobufb);
}
