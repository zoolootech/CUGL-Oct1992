
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
      ---> MSORT.C <--- : sort utility using QuickSort algorithm

  (C) Copyright 1980 by David M. Fogg @ New Day Computing - Portland, OR
*/

/* 29 Oct 80: convert WC->BDS */
/* 30 Oct - add ignore case code */
/* 31 Oct - refine chain code */
/* 5 Nov - fix avg len calc/disp in redlns() */
/* 6 Nov - add chain from CONCORD code */

/*
   INFILE is sorted to INFILE.A+, INFILE.B+... for merging
      by MERGE.
   If there is only 1 output file, then INFILE.A+ is renamed
      to INFILE.SRT, and the chain to MERGE is skipped.
   If INFILE has the extension .UNK (i.e., UNsorted Kwic index),
      then the final file name is INFILE.KWC (whether by renaming
      or passing to MERGE), and INFILE.UNK is killed.

   ODRIV is where the output file/s go\es. If it is specified,
      it will be passed to MERGE as part of its input file name.
   -I option: Ignore case, i.e., treat uppercase letters as lowercase.
      It's done this way rather then the reverse so that almost all
      of the ASCII special characters (punctuation, etc.) will come
      before the alphabetics in the collating sequence.
      This option, if used, will be passed on to MERGE.
      KWIC turns on this option when chaining to MSORT.
   -R option: sort in reverse (i.e., descending) order.
      This option, if used, will be passed on to merge.
   -L# option: '#' represents a number which is the average line
      length.  If omitted, a value of 30 will be assumed.  If the
      lines to be sorted are significantly longer or shorter, then
      the sort memory buffer can be most efficiently used by informing
      MSORT of the situation.
      KWIC calculates average length for its input file and passes it
      to MSORT.
*/

#include <dmfio.h>

#define MAXLEN	130	/* max length of a line */
#define MARGIN	2000	/* overhead for stack */
#define MINMEM	1024	/* smallest area OK 4 linptr + lbuf */
#define PTRSIZ	2	/* bytes per pointer */
#define AVGLIN	30	/* default average line length */

char ibuf[BUFSIZ];	/* ifile buffer */
char obuf[BUFSIZ];	/* ofile buffer */
BOOL eoflag;		/* ifile EOF flag */

main(ac, av)
int ac;
char *av[];
{
   char **linptr;		/* ptr to line ptrs */
   char *lbuf;			/* ptr to begin of line buf */
   int nlins;			/* # inp lines read */
   BOOL backwd; 		/* backward order sort flag */
   int maxlns;			/* max # of lines allowed */
   int avlen;			/* average line length */
   char *memtop;		/* top of data area */
   unsigned avail;		/* available bytes 4 linptr & lbuf */
   char inam[15];		/* input file name */
   char onam[15];		/* output file name(s) */
   char ext[5]; 		/* final file name extension */
   int cloc;			/* change ltr loc in onam */
   int xloc;			/* xtension loc in inam */
   int argn;			/* curr comm ln arg # */
   char *arg;			/*  "    "   "   " */
   BOOL igcas;			/* ignore case flag */

   _allocp = NULL;
   backwd = NO; avlen = AVGLIN;
   igcas = eoflag = NO;
   strcpy(ext, ".SRT");

   if (ac < 2) {
      puts("usage: msort IFILE [ODRIV] [-I -R -L]\n");
      puts("IFILE: file to sort\n");
      puts("ODRIV: (e.g., B:) where to put IFILE.X+ (X=A,B,C...\n");
      puts("   -I: Ignore case (treat uppercase as lowercase)\n");
      puts("   -R: Reverse order sort\n");
      puts("  -L#: average Line Length = # (default 30)\n");
      exit ();
   }

   strcpy(inam, av[1]);
   if (fopen(inam, ibuf) == ERROR)
      errxit("Input file error");

   for (argn = 2; argn < ac; ++argn) {	      /* get/set options */
      arg = av[argn];
      if (*arg == '-')
	 switch (arg[1]) {
	    case 'I':
	       igcas = YES;
	       break;
	    case 'R': 
	       backwd = YES;
	       break;
	    case 'L':
	       if ((avlen = atoi(arg+2)) < 1)
		  errxit("Bad average length value");
	       break;
	    default :
	       errxit("Bad option value");
	 }
   }

   if (ac > 2 && *(av[2]+1) == ':') {	  /* set up outfile name */
      strcpy(onam, av[2]);
      if (strlen(onam) > 2) errxit("Bad output drive field");
      strcat(onam, inam + instr(inam, ":"));
   }
   else
      strcpy(onam, inam + instr(inam, ":"));
   if (cloc = instr(onam, "."))
      strcpy(onam + cloc, "A+");
   else
      strcat(onam, ".A+");

   cloc = instr(onam, ".A+");

   memtop = topofmem() - MARGIN;
   if ((avail = memtop - endext()) < MINMEM)
      errxit("not enough memory");
   maxlns = avail / (avlen + PTRSIZ) - 1;
   if ((linptr = alloc(avail)) == 0)
      errxit("Memory allocation error");
   lbuf = linptr + maxlns;		/* lbuf starts abuv its ptrs */
   printf("Memory available: %u   Max lines: %d\n", avail, maxlns);

   /*
      >>------> MAIN LOOP <------<<
   */
   while ((nlins = redlns(linptr, lbuf, maxlns, memtop)) > 0) {
      printf(" %d lines read\n", nlins);
      quick(linptr, 0, nlins - 1, backwd, igcas);
      ritlns(linptr, nlins, onam);
      onam[cloc] = ++onam[cloc];
   }

/*
      >>>---------> EGRESS
*/
   fclose(ibuf);

   if (xloc = instr(inam, ".UNK")) {
      unlink(inam);
      strcpy(ext, ".KWC");
   }
   else if (xloc = instr(inam, ".UNC")) {
      unlink(inam);
      strcpy(ext, ".CON");
   }
   else
      xloc = instr(inam, ".");

   if (xloc)
      strcpy(inam+xloc, ext+1);
   else
      strcat(inam, ext);

   if (onam[cloc] == 'B') {		/* only 1 file: rename */
      onam[cloc] = 'A';
      unlink(inam);	   /* avoid multiple files w/ same name!!! */
      if (rename(onam, inam) == ERROR)
	 errxit("File rename error");
   }
   else {				/* >1 file: --> merge */
      onam[--cloc] = NULL;
      if (igcas) {
	 printf("\n--> merge %s %s -I\n", onam, inam);
	 execl("merge", onam, inam, "-I", 0);
      }
      else {
	 printf("\n--> merge %s %s\n", onam, inam);
	 execl("merge", onam, inam, 0);
      }
   }
}

/************************************************************/

redlns (linptr, lbuf, maxlns, mtop)	 /* <--- INPUT LINES <--- */
char **linptr;
char *lbuf;
int maxlns;
char *mtop;

/***   Reads input lines from ifile into heap area
  **   Pointer to each line goes into linptr
  **   RETURNS: number of lines read
*/
{
   int len;
   unsigned avl, nlines;
   char *pbuf;			/* pointo curr pos in lbuf */
   int rv;			/* ret val from fgets */

   nlines = 0;
   pbuf = lbuf;

   if (eoflag) return (0);

   while (nlines < maxlns && pbuf+MAXLEN < mtop) {
      rv = fgets(pbuf, ibuf);
      if (rv == 0) {
	 eoflag = YES;
	 break;
      }
      len = strlen(pbuf);
      linptr[nlines++] = pbuf;
      pbuf += len;
      *(pbuf-1) = '\0';      /* zap newline */
   }
   if (nlines) {
      avl = pbuf - lbuf; avl = avl / nlines;
      printf("Average line length = %u\n", avl);
   }
   return (nlines);
}


ritlns (linptr, nlins, onam)	/* ---> WRITE OUTPUT LINES ---> */
char **linptr;
int nlins;
char *onam;
{
   if (fcreat(onam, obuf) == ERROR)
      errxit("error creating o/p file");

   while (nlins-- > 0)
      fprintf(obuf, "%s\n", *linptr++);
   oclose(obuf);
}
