/* filter.c--filters ASCII text files.
 * Reads specified text file, filters as directed and displays
 * on console or puts into optional output file.
 * Copyright 6/85 by N.T.Carnevale
 * Permission granted for nonprofit use only
 */


#include printf.h


#define VERSION "ASCII file filter v.3.2 7/17/85\n"
#define USAGE1	"Usage:  filter sourcefile {options} {destfile}\n"
#define USAGE2	"Options:  t  tab compression\n"
#define USAGE3	"	   c  add CR before LF\n"
#define USAGE4	"	   l  add LF after CR\n"
#define USAGE5	"	   n  add \\n after \\n (double space)\n"
#define USAGE6	"	   xb strip blanks\n"
#define USAGE7	"	   xc remove CR\n"
#define USAGE8	"	   xl remove LF\n"
#define USAGE9	"	   xn convert \\n\\n to \\n (single space)\n"


/* if using Software Toolworks' compiler, */
#define C80

#ifdef C80
#include c80def.h
FILE fp;	/* channel # for source file */
#else
FILE fp;	/* pointer to source file */
#define CR 0x0d
#define LF 0x0a
#endif


/* the command sentinels */
#define DASH	'-'	/* signals start of option list */
#define XTRACT	'X'
#define TABCMP	'T'
#define BLANX	'B'
#define CARRET	'C'
#define LINFED	'L'
#define NULIN	'N'


/* file-related */
char *filnam;		/* file name input from command line */
FILE srcfil,dstfil;	/* source and destination files */


/* for unget() and xgetc() */
int lastc=NULL;


/* flags that control filter */
boolean tabflg=FALSE,		/* default settings */
	passblanks=TRUE,	/* tab compression off, */ 
	addlf=FALSE,		/* pass blanks, LF, CR, NL */
	passlf=TRUE,		/* DON'T add LF, CR, NL */
	addcr=FALSE,
	passcr=TRUE,
	addnl=FALSE,
	passnl=TRUE;


main(argc,argv)
int argc;
char *argv[];
{
	int argnum;
	char *cmdstrng;
	int filcount;	/* how many files are open */

	fprintf(stderr,VERSION);
	if (argc < 2) errexit();

	/* process command line */
	for (argnum=1; argnum<argc; ++argnum) {
		cmdstrng=argv[argnum];
		if (cmdstrng[0]==DASH) procop(cmdstrng);
		else if (filcount==0) { /* open source file */
			if (srcfil=filexists(cmdstrng,READ_BINARY)) {
				filcount=1;
				printf("Source file is %s\n",cmdstrng);
			}
		}
		else if (filcount==1) { /* open dest file */
			if (dstfil=filexists(cmdstrng,WRITE_BINARY)) {
				filcount=2;
				printf("Output file is %s\n",cmdstrng);
			}
		}
	}

	if (filcount==0) errexit();	/* quit if no source */
	if (filcount==1) dstfil=0;	/* output to console if no dest */

	dowork();

	if (filcount==2) {
		putc(CPMEOF,dstfil);	/* mark end of file */
		fclose(dstfil);
	}
}


procop(ops)	/* process the option string */
char *ops;
{
	char opc;
	boolean pass;	/* TRUE if char is to be passed by the filter */

	while (*ops) {
		pass=TRUE;
		if ((opc=toupper(*ops++))==XTRACT) {
			pass=FALSE;
			opc=toupper(*ops++);
		}
		switch (opc) {
			case TABCMP:	tabflg=TRUE;		break;
			case BLANX:	passblanks=pass;	break;
			case LINFED:	addlf = passlf = pass;	break;
			case CARRET:	addcr = passcr = pass;	break;
			case NULIN:	addnl = passnl = pass;	break;
			default:	break;
		}
	}
}


FILE filexists(fname,action)
/* return pointer to the file that is opened, NULL if not successful */
char *fname,*action;
{
	FILE temp;

	if ( ! (temp=fopen(fname,action)) ) 
		printf("Cannot open %s\n",fname);
	return(temp);
}


dowork()
{
  int xgetc();
  int c;

/* handy code */
#define NEXT_c (c=xgetc(srcfil))

  while (NEXT_c != EOF) {
	/* pass tabs and all printable characters except blanks */
	if ((c=='\t') || ((c>' ') && (c<='~'))) {
		putc(c,dstfil);
		continue;
	}
	/* handle blanks, LF, CR, and CP/M NEWLINE */
	switch (c) {
	    case ' ':	if (passblank) putc(c,dstfil);
			break;
	    case LF:	if (addcr) putc(CR,dstfil);
			if (passlf) putc(c,dstfil);
			break;
	    case CR:	if (passnl && (!addnl)) {
			    if (passcr) putc(c,dstfil);
			    if (addlf) putc(LF,dstfil);
			    break;
			}
			if (addnl) { /* if nextchar==LF, output LF CR LF */
			    putc(c,dstfil);
			    if (NEXT_c==LF) {
				putc(c,dstfil);
				putc(CR,dstfil);
				putc(c,dstfil);
			    }	/* else put next char back */
			    else ungetc(c);
			    break;
			}
			if (!passnl) { /* pass every other CR LF */
			    putc(c,dstfil);	/* pass first CR regardless */
			    /* if nextchar is LF, look for a second CR LF */
			    if (NEXT_c != LF) { /* not first CR LF */
				ungetc(c);
				break;
			    }
			    /* first CR LF found--pass it & look for 2nd */
			    putc(c,dstfil);	/* pass first CR LF */
			    if (NEXT_c != CR) { /* look for next CR */
				ungetc(c);
				break;
			    }
			    if (NEXT_c==LF) break;	/* eat second CR LF */
			    putc(CR,dstfil);		/* not CR LF */
			    ungetc(c);
			    break;
			}
			/* omitted tabifying--do some other time */
	}
  }
}


ungetc(cx)
int cx;
{
	lastc=cx;
}


int xgetc(fil)
FILE fil;
/* similar to buffered getchar() on p.162 of Kernighan & Ritchie 
   returns -1 when end of file encountered */
{
	int temp;
#define BUFSIZE 512
	static char buf[BUFSIZE];
	static char *bufp = buf;
	static int n=0;

	if (lastc != NULL) {
		temp=lastc;
		lastc=NULL;
		return((temp!=CPMEOF) ? temp:EOF);
	}
	if (n==0) {	/* buffer empty */
		n=read(fil,buf,BUFSIZE);
		bufp=buf;
	}
	if ((temp = *bufp++) == CPMEOF) n=0;
	return ((--n>=0) ? temp:EOF);
}


errexit()
{
	fprintf(stderr,USAGE1);
	fprintf(stderr,USAGE2);
	fprintf(stderr,USAGE3);
	fprintf(stderr,USAGE4);
	fprintf(stderr,USAGE5);
	fprintf(stderr,USAGE6);
	fprintf(stderr,USAGE7);
	fprintf(stderr,USAGE8);
	fprintf(stderr,USAGE9);
	exit(1);
}
