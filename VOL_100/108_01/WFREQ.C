
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

/* ---> WFREQ <---   (Word frequency program)

   Conceived & coded Nov 1980 by David Fogg - Portland, Oregon
   Copyright (C) 1980 by David Fogg & New Day Computing Co.

   11 Nov - create: from modification of CONOUT.C

   This program takes a file named <FILNAM>.CON consisting of lines
   containing: "WORD NUMBER"; the WORDs are sorted; the NUMBERs are
   ascending for identical consecutive words.  An output file named
   <FILNAM>.WFR is generated in which identical words have been re-
   moved and all their line numbers replaced with a count for each
   word; the count is placed first in the file, thus allowing this
   file to be sorted (by MSORT) if a frequency-ordered, rather than
   alphabetical, list is desired.

   The input file is deleted at the end.

   usage is: conout FILNAM [ODRIV]

*/

#include <std.h>

#define MAXWD 50	/* max wd size */

main (ac, av)
int ac;
char *av[];
{
   char ibuf[BUFSIZ];	      /* infile buffer */
   char obuf[BUFSIZ];	      /* outfile buffer */
   char ilin[MAXLINE];	      /* input line */
   char oword[MAXWD];	      /* previous word */
   char inam[15], onam[15];   /* in/out file names */
   unsigned freq;	      /* curr frequency */
   int spos;		      /* space pos in ilin */
   int wdno;		      /* word number */
   int argn;		      /* command line argument # */
   char *arg;		      /* pointo curr arg */

/* ---> INITIALIZATION <---
*/
   oword[0] = NULL;
   wdno = 0;

/* ---> GET ARGUMENT(S) <---
*/
   if (ac < 2) {
      puts("usage: wfreq IFILE [ODRIV]\n");
      puts("IFILE: IFILE.CON is input file\n");
      puts("ODRIV: (e.g., B:) where to put IFILE.WFR\n");
      exit ();
   }

   strcpy(inam, av[1]); 	       /* setup infile */
   strcat(inam, ".CON");
   if (fopen(inam, ibuf) == ERROR)
      errxit("Input file error");

   if (ac > 2 && *(av[2]+1) == ':') {  /* setup outfile */
      strcpy(onam, av[2]);
      if (strlen(onam) > 2) errxit("Bad output drive field");
      strcat(onam, av[1] + instr(av[1],":"));
   }
   else
      strcpy(onam, av[1]);
   strcat(onam, ".WFR");
   if (fcreat(onam, obuf) == ERROR)
      errxit("Output file error");

/*
   >>------> MAIN LOOP <------<<
*/
   while (fgets(ilin, ibuf)) {
      spos = instr(ilin, " ");
      ilin[spos-1] = NULL;
      if (strcmp(oword, ilin)) {
	 if (oword[0] != NULL)
	    fprintf(obuf, "%5u %s\n", freq, oword);
	 strcpy(oword, ilin);
	 freq = 1;
      }
      else
	 ++freq;
   }
   fprintf(obuf, "%5u %s\n", freq, oword);
   fclose(ibuf); oclose(obuf);
}
