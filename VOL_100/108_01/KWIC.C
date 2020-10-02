
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

/* --->  K W I C  <--- : generate KeyWordInContext raw outfile

    29 OCT 80: convert from WC -> BDS
    30 Oct: add chain code
    31 Oct: reverse -C option function
    5 Nov: add -N -O options
    29 Jan 81: add -L option

    Programs generates rotated lines from IFILE to IFILE.UNK
	  for sorting to generate a KWIC Index.

    Words in skip list or == 1 char long are skipped;
	  skip list is originally: AN, THE, AND, OR.

    Although chars are output in the same case they are input,
	  they are internally converted to uppercase for skip list test.

    -A option: if on, defines word begin as 1st ALPHANUMERIC char,
	  rather than first non-whitespace (NB: this affects word length).
    -C option: defeats auto-chain -> MSORT -> MERGE.
	NB: auto-chain turns on Ignore case option in MSORT & MERGE.
	     MSORT will kill IFILE.UNK; MERGE will kill IFILE.A+...
	     KWIC calculates and passes on average line length to
		MSORT for most efficient use of sort buffer.
    -Ln option: set Line Length to 'n' in arg list to MSORT
	     (default: 40)
    -Nc option: char 'c' at the beginning of a word causes that word to
	be skipped (NB: -A option turned off).
    -Oc option: char 'c' MUST be at the beginning of a word for that
	word to be used (NB: overrides -A & -N options).

    Additional arguments in input command line are taken as
	  words to add to the skip list.

    If no ODRIV is specified, all files will be created on the input
	file drive; if ODRIV is specified, then the files will ping-pong
	between input file drive & ODRIVE. For example:

	    KWIC INDEX.RAW B:		   (A:INDEX.RAW --> B:INDEX.UNK)
	    MSORT B:INDEX.UNK -I -L40	 (B:INDEX.UNK --> A:INDEX.A+...)
	    MERGE INDEX B:INDEX.KWC -I	(A:INDEX.A+... --> B:INDEX.KWC)

*/

#include <std.h>

#define MAXLEN	130	 /* max line length */
#define MSKIPS	30	  /* max # of words in skiplist */
#define DEFLEN	40	  /* default line length */

main (ac, av)
int ac;
char *av[];
{
    char ilin[MAXLEN];	  /* input line buffer */
    char tlin[MAXLEN];	  /* test line buffer {UCASE ilin} */
    int llen;		     /* length of current line */
    int curwd;		    /* pos of 1st char of curr wd */
    int wdlen;		    /* length of curr wd */
    int i;
    BOOL alfon; 	    /* whether to start words @ 1st alfanum */
    BOOL only;		    /* ONLY-words-starting-w/a-char flag */
    BOOL not;		     /* NOT  ditto above */
    char onlych;	    /* ONLY start char allowed */
    char notch; 	    /* NOT allowed 2b start char */
    int nxarg;
    char ibuf[BUFSIZ];	  /* infile buffer */
    char obuf[BUFSIZ];	  /* outfile buffer */
    int nskips; 	    /* # of words in default skip list */
    char *skips[MSKIPS]; /* pointers to skip list words */
    BOOL gosort;	    /* to chain or not to chain */
    char inam[15];	   /* input file name */
    char onam[15];	   /* output file name */
    int ppos;		     /* period position in onam */
    int argn;		     /* curr arg# in options code */
    char *arg;		    /* curr arg in options code */
    char avlen[20];	   /* str vrsn of avg line len of ofile */

/*
	  ---> INITIALIZATION <---
*/
    gosort = YES; alfon = only = not = NO;
    notch = onlych = NULL;
    nskips = 4;
    nxarg = 2;
    strcpy(avlen, "-L40");

    skips[0] ="AN"; skips[1] ="THE"; skips[2] ="AND"; skips[3] ="OR";
 
    if (ac < 2) {
	puts("usage: kwic IFILE [ODRIV] [-A -C -Ln -Nc -Oc] [words]\n");
	puts("IFILE: name of input file\n");
	puts("ODRIV: (e.g., B:) where to put IFILE.UNK\n");
	puts("   -A: start words @ 1st ALFANUMERIC, 'stead of 1st non-blank\n");	puts("   -C: don't CHAIN to msort & merge\n");
	puts("  -Ln: avg. line length arg for msort (default: 40)\n");
	puts("  -Nc: NOT use words that start with <c>\n");
	puts("  -Oc: ONLY use words that start with <c>\n");
	puts("words: added to skip list (an the and or)\n");
	exit ();
    }

    strcpy(inam, av[1]);
    if (fopen(inam, ibuf) == ERROR)
	errxit("Input file error");

    if (ac > 2 && *(av[2]+1) == ':') {	    /* set up outfile name */
	strcpy(onam, av[2]);
	if (strlen(onam) > 2) errxit("Bad output drive field");
	strcat(onam, inam + instr(inam, ":"));
	++nxarg;
    }
    else
	strcpy(onam, inam);
    if (ppos = instr(onam, "."))
	strcpy(onam + ppos, "UNK");
    else
	strcat(onam, ".UNK");

    if (fcreat(onam, obuf) == ERROR)
	errxit("Output file error");


    /*
	---> GET OPTIONS <---
    */
    for (argn = 1; argn < ac; ++argn) {
	arg = av[argn];
	if (*arg == '-')
	    switch (arg[1]) {
		case 'C':
		    gosort = NO; ++nxarg;
		    break;
		case 'A':
		    alfon = YES; ++nxarg;
		    break;
		case 'L':
		    strcpy(avlen, arg);
		    break;
		case 'N':
		    not = YES; notch = arg[2]; ++nxarg;
		    alfon = only = NO; onlych = NULL;
		    break;
		case 'O':
		    only = YES; onlych = arg[2]; ++nxarg;
		    alfon = not = NO; notch = NULL;
		    break;
		default :
		    errxit("Bad option value");
	    }
    }

    while (nxarg < ac)			   /* get xtra skip wds (if any) */
	skips[nskips++] = av[nxarg++];

/*
	  ---> MAIN LOOP <---
*/
    while (fgets(ilin, ibuf) != 0) {
	llen = strlen(ilin); ilin[--llen] = '\0';
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
	    if ((only && ilin[curwd] == onlych)
	    || (!only && wdlen > 1
	      && noskip(tlin+curwd, wdlen, skips, nskips)
	      && (!not || ilin[curwd] != notch))) {
		fprintf(obuf, "%s // ", ilin + curwd + only);
		for (i = 1; i < curwd;	++i)
		    putc(ilin[i-1], obuf);
		fputs("\n", obuf);
	    }
	    curwd += ++wdlen;
	} /* while (curwd... */
    } /* while (fgets... */
    fclose(ibuf); oclose(obuf);

    if (gosort) {			       /* ---> msort? */
	inam[instr(inam, ":")] = NULL;
	printf("\n--> msort %s %s -I %s\n", onam, inam, avlen);
	execl("msort", onam, inam, "-I", avlen, 0);
    }
}


BOOL noskip (wd, len, skips, nskips)	 /* see if wd in skips */
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
