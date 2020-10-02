
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

/* --->  CONCORD  <--- : generate Concordance raw outfile */

/* 6 Nov 80: convert from KWIC */

/*
   Programs generates index lines from IFILE to IFILE.UNC
	for sorting to generate a concordance.

   Words in skip list or < minlen chars long are skipped;
	skip list is originally: A, AN, THE, AND, OR.

   Chars are output in uppercase.

   -A option: if on, defines word begin as 1st ALPHANUMERIC char,
	rather than first non-whitespace (NB: this affects word length).
   -C option: defeats auto-chain -> MSORT -> MERGE.
      NB: auto-chain turns on Ignore case option in MSORT & MERGE.
	  MSORT will kill IFILE.UNC; MERGE will kill IFILE.A+...
	  CONCORD calculates and passes on average line length to
	    MSORT for most efficient use of sort buffer.
   -L# option: reset minimum word length to '#'; default is 2.
   -Nc option: char 'c' at the beginning of a word causes that word to
      be skipped (NB: -A option turned off).
   -Oc option: char 'c' MUST be at the beginning of a word for that
      word to be used (NB: overrides -A & -N options).

   Additional arguments in input command line are taken as
	words to add to the skip list.

   If no ODRIV is specified, all files will be created on the input
      file drive; if ODRIV is specified, then the files will ping-pong
      between input file drive & ODRIVE. For example:

	 CONCORD INDEX.RAW B:	     (A:INDEX.RAW --> B:INDEX.UNC)
	 MSORT B:INDEX.UNC -L10   (B:INDEX.UNC --> A:INDEX.A+...)
	 MERGE INDEX B:INDEX.CON -I  (A:INDEX.A+... --> B:INDEX.CON)

*/

#include <std.h>

#define MAXLEN	130	/* max line length */
#define MSKIPS	30	/* max # of words in skiplist */
#define DEFMIN	2	/* default min word length */


main (ac, av)
int ac;
char *av[];
{
   char ilin[MAXLEN];	/* input line buffer */
   char tlin[MAXLEN];	/* test line buffer {UCASE ilin} */
   int llen;		/* length of current line */
   int curwd;		/* pos of 1st char of curr wd */
   int wdlen;		/* length of curr wd */
   int i;
   BOOL alfon;		/* whether to start words @ 1st alfanum */
   BOOL only;		/* ONLY-words-starting-w/a-char flag */
   BOOL not;		/* NOT	ditto above */
   char onlych; 	/* ONLY start char allowed */
   char notch;		/* NOT allowed 2b start char */
   int nxarg;
   char ibuf[BUFSIZ];	/* infile buffer */
   char obuf[BUFSIZ];	/* outfile buffer */
   int nskips;		/* # of words in default skip list */
   char *skips[MSKIPS]; /* pointers to skip list words */
   BOOL gosort; 	/* to chain or not to chain */
   char inam[15];	/* input file name */
   char onam[15];	/* output file name */
   int ppos;		/* period position in onam */
   int argn;		/* curr arg# in options code */
   char *arg;		/* curr arg in options code */
   unsigned tc, tl;	/* tot chars, lins in ifile so far */
   int lino;		/* lines output so far */
   char avlen[20];	/* str vrsn of avg line len of ofile */
   int minlen;		/* min word length */
   char punct[20];	/* punctuation characters */
   char test[2];	/* test 1-char string for punctuation chk */
   int newd;		/* hold val of curwd */

/*
	---> INITIALIZATION <---
*/
   gosort = YES; alfon = only = not = NO;
   test[1] = notch = onlych = NULL;
   nskips = 5;
   nxarg = 2;
   lino = tl = tc = 0;
   strcpy(avlen, "-L");
   minlen = DEFMIN;
   punct[0] = 0x22 /*"*/; punct[1] = NULL; strcat(punct, "!)}]>\';:?.,");

   skips[0] ="AN"; skips[1] ="THE"; skips[2] ="AND"; skips[3] ="OR";
   skips[4] ="A";

   if (ac < 2) {
      puts("usage: concord IFILE [ODRIV] [-A -C -Nc -Oc] [words]\n");
      puts("IFILE: name of input file\n");
      puts("ODRIV: (e.g., B:) where to put IFILE.UNC\n");
      puts("   -A: start words @ 1st ALFANUMERIC, 'stead of 1st non-blank\n");
      puts("   -C: don't CHAIN to msort & merge\n");
      puts("  -L#: set minimum word length to # (default=2)\n");
      puts("  -Nc: NOT use words that start with <c>\n");
      puts("  -Oc: ONLY use words that start with <c>\n");
      puts("words: added to skip list (an the and or)\n");
      exit ();
   }

   strcpy(inam, av[1]);
   if (fopen(inam, ibuf) == ERROR)
      errxit("Input file error");

   if (ac > 2 && *(av[2]+1) == ':') {	  /* set up outfile name */
      strcpy(onam, av[2]);
      if (strlen(onam) > 2) errxit("Bad output drive field");
      strcat(onam, inam + instr(inam, ":"));
      ++nxarg;
   }
   else
      strcpy(onam, inam);
   if (ppos = instr(onam, "."))
      strcpy(onam + ppos, "UNC");
   else
      strcat(onam, ".UNC");

   if (fcreat(onam, obuf) == ERROR)
      errxit("Output file error");


   /*
      ---> GET OPTIONS <---
   */
   for (argn = 1; argn < ac; ++argn) {
      arg = av[argn];
      if (*arg == '-')
	 switch (arg[1]) {
	    case 'C': gosort = NO; ++nxarg;
		      break;
	    case 'A': alfon = YES; ++nxarg;
		      break;
	    case 'L': minlen = atoi(arg + 2);
		      if (minlen < 1)
			errxit("Bad minimum word length value");
		      ++nxarg;
		      break;
	    case 'N': not = YES; notch = arg[2];
		      alfon = only = NO; onlych = NULL;
		      ++nxarg;
		      break;
	    case 'O': only = YES; onlych = arg[2];
		      alfon = not = NO; notch = NULL;
		      ++nxarg;
		      break;
	    default :
	       errxit("Bad option value");
	 }
   }

   while (nxarg < ac)		     /* get xtra skip wds (if any) */
      skips[nskips++] = av[nxarg++];

/*
	---> MAIN LOOP <---
*/
   while (fgets(ilin, ibuf) != 0) {
      llen = strlen(ilin); ilin[--llen] = '\0';
      ++tl;
      for (i = 0; i < llen; ++i)
	 tlin[i] = toupper(ilin[i]);
      tlin[llen] = NULL;
      curwd = 0;
      while (curwd < llen) {
	 while (isspace(tlin[curwd]) && ++curwd < llen)
	    ;
	 while (alfon
	    && !isalpha(tlin[curwd]) && !isdigit(tlin[curwd])
	    && ++curwd < llen)
	    ;
	 wdlen = 1;
	 while (!isspace(tlin[curwd+wdlen]) && curwd + wdlen < llen)
	    ++wdlen;
	 newd = curwd + wdlen;
	 do {
	    test[0] = tlin[curwd + --wdlen];
	 } while (instr(punct, test));
	 ++wdlen;
	 if ((only && ilin[curwd] == onlych)
	 || (!only && wdlen >= minlen
	   && noskip(tlin+curwd, wdlen, skips, nskips)
	   && (!not || ilin[curwd] != notch))) {
	    tc += wdlen; ++lino;
	    tlin[curwd+wdlen] = NULL;
	    fprintf(obuf, "%s %5d\n", tlin+curwd+only, tl);
	 }
	 curwd = ++newd;
      } /* while (curwd... */
   } /* while (fgets... */
   fclose(ibuf); oclose(obuf);

   if (gosort) {		       /* ---> msort? */
      inam[instr(inam, ":")] = NULL;
      sprintf(avlen+2, "%d", tc / lino + 7);
      printf("\n--> msort %s %s %s\n", onam, inam, avlen);
      execl("msort", onam, inam, avlen, 0);
   }
}


BOOL noskip (wd, len, skips, nskips)	/* see if wd in skips */
char *wd;
int len;
char *skips[];
int nskips;
{
   int i;
   char twd[MAXLEN];

   strcpy(twd, wd); twd[len] = '\0';

   for (i = 0; i < nskips; ++i)
      if (strcmp(skips[i], twd) == 0)
	 return(NO);
   return(YES);
}
