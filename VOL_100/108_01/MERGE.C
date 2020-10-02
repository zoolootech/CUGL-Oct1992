
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

/* --->  MERGE.C  <--- : merge utility for use w/ msort

    30 Oct 80: convert WC->BDS; add ignore case code
    4 Feb 81: chg MORDER; MAXLEN --> MAXLINE

    SYNOPSIS: sort mfiles to ofile
	NB: merge file names have format mfile.X+
	    (where X = A, B, C, ... )
*/

#include <std.h>

#define MORDER	7	/* merge order: max merge files/pass */

main(ac, av)
int ac;
char *av[];
{
   char lbuf[MAXLINE * MORDER];  /* merge line buffer */
   char *lb;			/* tempointer into lbuf */
   int len;
   char *linptr[MORDER];	/* pointers into lbuf */
   char inam[15];		/* input file name(s) */
   int nfils;		       /* # of merge files */
   int nf, fno;
   BOOL nokill; 		/* have mercy on infiles flag */
   BOOL back;			/* backward order flag */
   BOOL igcas;		      /* ignore case flag */
   int tlins;		      /* tot lines writ out */
   int cloc;		      /* chg loc in inam */
   char ibuf[MORDER][BUFSIZ]; /* input buffers */
   char obuf[BUFSIZ];	      /* output buffer */
   int argn;		      /* curr comm ln arg # */
   char *arg;		      /* curr comm ln arg ptr */

   nfils = tlins = 0;
   igcas = nokill = back = NO;

   if (ac < 3) {
      puts("usage: merge MFILE OFILE [-I -K -R]\n");
      puts("MFILE: input from MFILE.A+, MFILE.B+, etc.\n");
      puts("OFILE: output file name\n");
      puts("   -I: ignore case (treat uppercase as lowercase)\n");
      puts("   -K: don't Kill merge files\n");
      puts("   -R: Reverse order merge\n");
      exit ();
   }

/*
   ---> GET OPTIONS <---
*/
   for (argn = 3; argn < ac; ++argn) {
      arg = av[argn];
      if (*arg == '-')
	 switch (arg[1]) {
	    case 'I':
	       igcas = YES;
	       break;
	    case 'K':
	       nokill = YES;
	       break;
	    case 'R':
	       back = YES;
	       break;
	    default :
	       errxit("Bad option value");
	 }
   }

   strcpy(inam, av[1]); strcat(inam,".A+");
   cloc = instr(inam,".A+");

   if (fcreat(av[2], obuf) == ERROR)
      errxit("Output file error");

   while (nfils < MORDER) {		 /* open files & read 1st lns */
      if (fopen(inam, ibuf[nfils]) == ERROR)
	 break;
      lb = lbuf + nfils * MAXLINE;
      if (fgets(lb, ibuf[nfils]) != 0) {
	 len = strlen(lb);
	 *(lb + --len) = NULL;	/* replace \n w/ string terminator */
	 linptr[nfils++] = lb;
      }
      else
	 fclose(ibuf[nfils]);  /* close mt file */
      ++inam[cloc];	      /* bump filename */
   }

   strcpy(inam, av[1]); strcat(inam,".A+");   /* rewind filenames */

   if (nfils == 0)
      errxit("no input files");

   if (nfils == 1) {
      fputs(lbuf, obuf);
      fputs("\n", obuf);
      while (fgets(lbuf, ibuf[0]) != 0)
	 fputs(lbuf, obuf);	 /* trivial case: copy only merge fl->STDOUT */
      oclose(obuf);
      fclose(ibuf[0]);
      if (!nokill)
	 unlink(inam);
      errxit("1 file merged");
   }

   quick(linptr, 0, nfils - 1, back, igcas);   /* form initial heap */

/*
	---> MAIN LOOP <---
*/
   nf = nfils;

   while (nf) {
      lb = linptr[0];
      fprintf(obuf, "%s\n", lb);
      ++tlins;
      fno = (lb - lbuf) / MAXLINE;	/* calc file # */
      if (fgets(lb, ibuf[fno]) != 0) {
	 len = strlen(lb);
	 *(lb + --len) = NULL;
      }
      else {
	 fclose(ibuf[fno]);
	 if (!nokill) {
	    inam[cloc] = 'A' + fno;
	    unlink(inam);
	 }
	 linptr[0] = linptr[--nf];
      }
      reheap(linptr, nf, back, igcas);	       /* put 1st ln in its place */
   }
   printf("%d files merged     %d lines writ\n", nfils, tlins);
   oclose(obuf);
}
