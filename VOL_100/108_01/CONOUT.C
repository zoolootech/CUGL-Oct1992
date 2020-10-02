
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

/* ---> CONOUT <---   (Gen final Concordance from raw sorted file)

   Conceived & coded Nov 1980 by David Fogg - Portland, Oregon
   Copyright (C) 1980 by David Fogg & New Day Computing Co.

   6 Nov - creation day
   7 Nov - add word numbers, printer width opt

   This program takes a file named <FILNAM>.CON consisting of lines
   containing: "WORD NUMBER"; the words are sorted; the numbers are
   ascending for identical consecutive words.  An output file named
   <FILNAM>.CCO is generated in which identical words have been re-
   moved and all their line numbers combined - i.e., a concordance.

   The input file is deleted at the end.

   usage is: conout FILNAM [ODRIV]

*/

#include <std.h>

#define MAXWD  50
#define DEFWID 75

main (ac, av)
int ac;
char *av[];
{
   char ibuf[BUFSIZ];	      /* infile buffer */
   char obuf[BUFSIZ];	      /* outfile buffer */
   char ilin[MAXLINE];	      /* input line */
   char oword[MAXWD];	      /* previous word */
   char inam[15], onam[15];   /* in/out file names */
   int spos;		      /* space pos in ilin */
   int lino;		      /* line number from ilin */
   int cpos;		      /* curr pos in output line */
   int fpos;		      /* first pos in out lns 4 curr wd */
   int ocols;		      /* # of output columns */
   int wdno;		      /* word number */
   int argn;		      /* command line argument # */
   char *arg;		      /* pointo curr arg */

/* ---> INITIALIZATION <---
*/
   strcpy(oword, "Potrzebie");
   ocols = DEFWID;
   wdno = 0;

/* ---> GET ARGUMENT(S) <---
*/
   if (ac < 2) {
      puts("usage: conout IFILE [ODRIV] [-W#]\n");
      puts("IFILE: IFILE.CON is input file\n");
      puts("ODRIV: (e.g., B:) where to put IFILE.CCO\n");
      puts("  -W#: set printer width to '#' (default 75)");
      exit ();
   }

   for (argn = 2; argn < ac; ++argn) {	   /* get/set options */
      arg = av[argn];
      if (*arg == '-')
	 switch (arg[1]) {
	    case 'W': ocols = atoi(arg+2);
		      if (ocols < 20)
			errxit("Bad printer width value");
		      break;
	    default :
	       errxit("Bad option value");
	 }
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
   strcat(onam, ".CCO");
   if (fcreat(onam, obuf) == ERROR)
      errxit("Output file error");

/*
   >>------> MAIN LOOP <------<<
*/
   while (fgets(ilin, ibuf)) {
      spos = instr(ilin, " ");
      ilin[spos-1] = NULL;
      lino = atoi(ilin+spos);
      if (strcmp(oword, ilin)) {
	 fprintf(obuf, "\n%4d %s %5d", ++wdno, ilin, lino);
	 fpos = strlen(ilin) + 6; cpos = fpos + 5;
	 strcpy(oword, ilin);
      }
      else {
	 if (cpos > ocols - 3) {
	    fputs("\n ", obuf); cpos = 0;
	    while (++cpos < fpos) putc(' ', obuf);
	 }
	 fprintf(obuf, "%5d", lino);
	 cpos += 5;
      }
   }

   fclose(ibuf); oclose(obuf);
}
