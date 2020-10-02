
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

/* ---> XWORD.C <--- : Crossword Puzzle Program : Z-19 version

   (C) Copyright 1980 by David M. Fogg @ New Day Computing - Portland, OR

   9 APR 80 - 1st pass
   25 oct 80: convert to BDS C 1st pass
   26 Oct: add final touches
   28 Oct: convert to VG MT
   29 Oct: chg to oclose() in WRITS:
   31 Oct: fix VG shift arrow prob;
	   add dubchk() to READS: & WRITS:;
	   chg putadv(..) 2 only move 2 nxt legal loc
   4 Nov: convert back to Z-19/DMF environment
   25 Nov: re-comp w/coin() as BIOS, not BDOS, call (1.4X comp);
	   make clue display automagickall
   27 Nov: fix ERASE & READS to disp clues after scrn regen
   3 Dec: chg dubchk() 2 wrk w/ lc 'y'; add scrn regen func;
	  improve getext() & reloc query.

 * HINTS: answer files are FILNAM.ANS
 *				ANSER array
 *				<1st ACROSS clue
 *				rest of ACROSS clues
 *				<1st DOWN clue
 *				rest of DOWN clues
 *				EOF
 *	Partially filled-out screens are FILNAM.EXT
 *		(user specifies EXT)
 *				SCREEN array current contents
 *
 *	All commands are single locase alfa chars;
 *	ucase alfa are put on screen @ curr position,
 *	& cursor is moved ACROSS or DOWN (current mode).
 *
 *	Any letters put on screen by cheating
 *	('peek' or 'giveup') are put in locase.
 */

#include <std.h>

#define MAXIZ	21		/* maximum rows/cols in puzzle */
#define AVLEN	35		/* avg clue length */
#define MAXLEN	65		/* max	"     "    */
#define MXCLUES MAXIZ*MAXIZ/2	/* max # ACROSS+DOWN clues */
#define CLUBS	MXCLUES*AVLEN	/* size of clubuf[] */

#define DX	3		/* horiz dist btwn chars */
#define DY	1		/* vert   "    "     "   */
#define XORG	1		/* scrn X-ord of upleft char */
#define YORG	0		/*  "   Y-ord "    "     "   */

/*
	---> command characters <---
*/
#define ACROSS	'p'		/* set mode to ACROSS */
#define DOWN	'q'		/*  "   "   "  DOWN */
#define GOBAK	'4'		/* command to move cursor left */
#define GOFWD	'6'		/*    "    "   "     "	  right */
#define GOUP	'8'		/*    "    "   "     "	  up */
#define GODN	'2'		/*    "    "   "     "	  down */
#define TOPLN	't'		/* goto top line in col */
#define BOTLN	'v'		/*  "   bott "	 "   "	*/
#define INITC	's'		/*  "   init col "  line */
#define FINC	'w'		/*  "   final "  "   "	 */
#define PEEK	'7'		/* put anser letter on scrn */
#define UNDO	'9'		/* erase a single letter */
#define ERASE	'3'		/*** erase screen ***/
#define GIVUP	'1'		/*** fill in whole screen ***/
#define WRITS	'u'		/* write scrn to file */
#define READS	'r'		/* read   "  from "   */
#define REGEN	'\t'		/* regenerate screen */
#define QUIT	'5'		/*** quit: exit program ***/
/*
	---> misc ASCII &c <---
*/
#define MT	'.'		/* empty cell on screen */
#define WALL	' '		/* wall cell */
#define DELIM	'<'		/* delimit anser<ACROSS<DOWN in file */
#define RUBIT	 0		/* erase scrn arg to scrini() */
#define RENUIT	-1		/* regen scrn arg to scrini() */
/*
	---> screen positions <---
*/   
#define ACY	22		/* ACROSS clue line */
#define DCY	23		/* DOWN    "    "   */
#define AHX	0		/* ACROSS hdr column */
#define DHX	2		/* DOWN    "    "    */
#define ACX	8		/* ACROSS clue	"    */
#define DCX	8		/* DOWN    "    "    */
#define FNY	0		/* filename line */
#define FNX	68		/*    "     column */
#define PRX	66		/* extension prompt col */
#define PRY	20		/*     "       "    line */
#define INX	65		/* Instructions column */
#define INY	3		/* first Instructions line */
#define L25X	5		/* funkey ID line X-ord */
#define L25Y	24		/*   "    "   "   Y-ord */


/*
	* * * * * *  G L O B A L S  * * * * * *
*/

char iobuf[BUFSIZ];	     /* file I/O buffer */

int rows, cols;        /* rows/cols in curr puzzle */
int crow, ccol;        /* current (cursor) row/column */
int nacc, ndown;       /* # of ACROSS/DOWN clues */
char scrn[MAXIZ][MAXIZ];    /* screen array */
char anser[MAXIZ][MAXIZ];   /* answer array */
char *aclu[MAXIZ][MAXIZ];   /* pointo ACROSS clues in clubuf */
char *dclu[MAXIZ][MAXIZ];   /*	 "    DOWN     "   "    "    */
char clubuf[CLUBS];		  /* clues buffer */


/* >>>------------->  M A I N	P R O G R A M  <-------------<<< */

main (ac, av)
int ac;
char *av[];
{
   char filnam[12], ansfil[15]; /* file names */
   int ansfd, scrfd;		/* file descriptors */
   char inch;			/* input char from kbd */
   int savr, savc;		/* save crow,ccol */
   char dir;			/* direction/clue flag: a/d */
   int arow, acol, drow, dcol;	/* last ACROSS/DOWN clue cursor lox */
   BOOL errms;

   rows = cols = crow = ccol = nacc = ndown = 0;
   arow = acol = drow = dcol = -1;
   errms = NO;

   if ((++av, --ac) != 1)
      errxit("usage: xword ansfilnam");
   strcpy(filnam, *av); strcpy(ansfil, *av);
   strcat(ansfil, ".ANS");
   if ((ansfd = fopen(ansfil, iobuf)) == ERROR)
      errxit(".ANS file not found");

   getans();	   /* read .ANS file & setup clue pointers */

   dir = scrini(filnam, NO);	/* initialize screen */
/*
	>>------> MAIN LOOP <------<<
*/
   FOREVER {
      cluput(arow, acol, drow, dcol);
      arow = drow = crow;
      acol = dcol = ccol;
      inch = zchar();
      if (errms) {
	 ereol(PRX, PRY);
	 errms = NO;
      }
      if (isupper(inch))
	 putadv(inch, dir);
      else
	 switch (inch) {
	    case ACROSS:	/* set dir (& print clue) */
	       toxy(AHX, ACY); puts("ACROSS:");
	       msgat(DHX, DCY, "DOWN:");
	       dir = ACROSS;
	       break;
	    case DOWN:
	       toxy(DHX, DCY); puts("DOWN:");
	       msgat(AHX, ACY, "ACROSS:");
	       dir = DOWN;
	       break;
	    case GOBAK: 	/* move one space left */
	       while (torc(crow, --ccol) == WALL);
	       break;
	    case GOFWD: 	 /* move one space right */
	       while (torc(crow, ++ccol) == WALL);
	       break;
	    case GODN:		 /* move one space down */
	       while (torc(++crow, ccol) == WALL);
	       break;
	    case GOUP:		 /* move one space up */
	       while (torc(--crow, ccol) == WALL);
	       break;
	    case TOPLN: 	/* move to limit u/d/l/r */
	    case BOTLN:
	    case INITC:
	    case FINC:
	       crow = (inch == TOPLN) ? 0 : (inch == BOTLN) ? rows - 1: crow;
	       ccol = (inch == INITC) ? 0 : (inch == FINC) ? cols - 1 : ccol;
	       while (torc(crow, ccol) == WALL) {
		  crow += (inch == TOPLN) - (inch == BOTLN);
		  ccol += (inch == INITC) - (inch == FINC);
	       }
	       break;
	    case PEEK:		/* peek: put anser ltr up */
	       putadv(tolower(anser[crow][ccol]), dir);
	       break;
	    case UNDO:		/* erase a single letter */
	       putadv(MT, dir);
	       break;
	    case ERASE: 	/* erase screen */
	       if (dubchk()) {
		  dir = scrini(filnam, RUBIT);
		  acol = dcol = arow = drow = -1;
	       }
	       break;
	    case GIVUP: 	/* giveup: fillin screen */
	       savr = crow; savc = ccol;
	       if (dubchk()) {
		  for (crow = 0; crow < rows; ++crow)
		  for (ccol = 0; ccol < cols; ++ccol)
		     if (scrn[crow][ccol] == MT)
			charon(scrn[crow][ccol] = tolower(anser[crow][ccol]));
	       }
	       torc(savr, savc);
	       break;
	    case WRITS: 	/* write screen to file */
	       savr = crow; savc = ccol;
	       if (dubchk())
		  if ((scrfd = getext(filnam, WRITE)) < 0) {
		     msgat(PRX, PRY, "FILE ERR");
		     errms = YES;
		  }
		  else {
		     for (crow = 0; crow < rows; ++crow) {
			for (ccol = 0; ccol < cols; ++ccol)
			   putc(scrn[crow][ccol], iobuf);
			fputs("\r\n", iobuf);
		     }
		     oclose(iobuf);
		     ereol(PRX, PRY);
		  }
	       torc(savr, savc);
	       break;
	    case READS: 	/* read screen from file */
	       savr = crow; savc = ccol;
	       if (dubchk())
		  if ((scrfd = getext(filnam, READ)) == ERROR) {
		     msgat(PRX, PRY, "NOT FOUND");
		     errms = YES;
		  }
		  else {
		     dir = scrini(filnam, scrfd);
		     acol = dcol = arow = drow = -1;
		     fclose(iobuf);
		  }
	       torc(savr, savc);
	       break;
	    case REGEN:
	       savr = crow; savc = ccol;
	       dir = scrini(filnam, RENUIT);
	       acol = dcol = arow = drow = -1;
	       torc(savr, savc);
	       break;
	    case QUIT:		/* quit: exit the program */
	       if (dubchk()) {
		  puts(POCRT);
		  sleep(13);
		  errxit("Bye");
	       }
	 } /* switch */
   } /* FOREVER */
} /* main */

/************* ---< S U B R O U T I N E S >--- *************/


getans ()		   /* --<READ .ANS FL & SETUP CLUES */
{
   char c;
   int	colt;
   char *lasacc, *firsdn;	/* -> last ACROSS/1st DOWN clubuf */
   char **pacc, **pdown;	/* pointers into pclues */
   char *pclues[MXCLUES];	/* initial clue pointers */

   colt = 0;

   while (((c = getc(iobuf)) != CEOF) && (c != DELIM))	/* first: anser[][] */
      if (c == '\n') {
	 cols = colt; colt = 0;
	 if (++rows > MAXIZ)
	    errxit("too many rows");
      }
      else if (achar(c)) {
	 anser[rows][colt] = c;
	 if (++colt > MAXIZ)
	    errxit("Too many columns");
      }

   nacc = getclu(pclues, &clubuf, MXCLUES, clubuf+CLUBS);    /* 2nd: ACROSS */
   lasacc = pclues[nacc - 1];
   firsdn = lasacc + strlen(lasacc) + 1;
   ndown = getclu(&pclues[nacc], firsdn, MXCLUES-nacc, clubuf+CLUBS);	/* 3rd: DOWN */

   pdown = (pacc = pclues) + nacc;	/* setup clue pointers */
   for (crow = 0; crow < rows; ++crow)
   for (ccol = 0; ccol < cols; ++ccol)
      if (anser[crow][ccol] != WALL) {
	 aclu[crow][ccol] =
	    (ccol == 0 || anser[crow][ccol-1] == WALL) ?
	       *(pacc++) : aclu[crow][ccol-1];
	 dclu[crow][ccol] =
	    (crow == 0 || anser[crow-1][ccol] == WALL) ?
	       *(pdown++) : dclu[crow-1][ccol];
      }
   fclose(iobuf);
}


getclu (pclu, cbuf, mxclu, clutop) /* --<READ A SET OF CLUES>-- */
char **pclu;
char *cbuf;
int mxclu;
char *clutop;
{
   char c;
   int nc, len;
   int nlins	;

   nlins = 0;

   FOREVER {
      len = 0;
      while ((c = getc(iobuf)) != CEOF && c != '\n' && c != DELIM)
	 if (c >= ' ')
	    *(cbuf + len++) = c;
      if (c == 0 || c == DELIM)
	 return(nlins); 		/* ---> EGRESS ---> */
      if (nlins > mxclu || cbuf + len > clutop)
	 errxit("Clue buffer overflow");
      pclu[nlins++] = cbuf;
      cbuf += ++len;
      *(cbuf-1) = '\0'; 	/* zap \r w/ string terminator */
   }
}


scrini (fnam, scrnfd)		 /* --<SCREEN INITIALIZE>-- */
char *fnam;
int scrnfd;    /* <0: regen scrn; 0: startup init; >0: read save file */
{
   char c;

   puts(CLEAR); puts(VIDINV); puts(BLINON); puts(PADNOR);
   toxy(L25X, L25Y);
   puts(" LEFT   TOP   SAVE   BOTT   RIGHT        ACROSS   DOWN   GET ");
   for (crow = 0; crow < rows; ++crow)
   for (ccol = 0; ccol < cols; ++ccol) {
      if (ccol == 0) {
	 torc(crow, ccol); puts(CURLEF);
      }
      if (scrnfd > 0)
	 do {
	    c = getc(iobuf); 
	 } while (!(isalpha(c) || c == WALL || c == MT));
      else if (scrnfd == 0)
	 c = (anser[crow][ccol] == WALL) ? WALL : MT;
      else
	 c = scrn[crow][ccol];

      if (c == WALL)
	 puts(VIDNOR);
      scrn[crow][ccol] = c;
      printf(" %c ", c);
      if (c == WALL)
	 puts(VIDINV);
   }
   msgat(FNX, FNY, fnam);
   toxy(AHX, ACY); puts("ACROSS:");
   msgat(DHX, DCY, "DOWN:");

   toxy(INX, INY); puts(   "Arrow keys:    ");
   toxy(INX, INY+1); puts( "   move cursor ");
   toxy(INX, INY+2); puts( "               ");
   toxy(INX, INY+3); puts( "IC: clue letter");
   toxy(INX, INY+4); puts( "DC: delete ltr ");
   toxy(INX, INY+5); puts( "DL: erase scrn ");
   toxy(INX, INY+6); puts( "IL: fillin scrn");
   toxy(INX, INY+7); puts( "TAB: regen scrn");
   toxy(INX, INY+8); puts( "HOME: quit prog");
   toxy(INX, INY+9); puts( "               ");
   toxy(INX, INY+10); puts("A-Z or a-z:    ");
   toxy(INX, INY+11); puts("fills in puzzle");

   crow = ccol = 0;
   while (torc(crow, ccol) == WALL)
      ++ccol;
   return(ACROSS);
}


getext (fnam, mode)	/* --<GET EXT & OPEN SCRN READ|WRITE FILE>-- */
char *fnam;
char mode;
{
   char scrfil[15];
   char fext[20];
   int i;

   toxy(PRX, PRY); puts(VIDNOR); puts("Ext: "); gets(fext);
   fext[3] = NULL;
   if ((i = inset(fext, '\r')) > 0) fext[--i] = NULL;
   puts(VIDINV);
   strcpy(scrfil, fnam); strcat(scrfil, "."); strcat(scrfil, fext);
   if (mode == READ)
      return (fopen(scrfil, iobuf));
   else
      return (fcreat(scrfil, iobuf));
}


putadv (nuch, dir)	/* --<PUT nuch IN/ON Scrn & ADVANCE>-- */
char nuch, dir;
{
   charon(scrn[crow][ccol] = nuch);

   if (dir == ACROSS) {
      if (ccol < cols - 1 && scrn[crow][ccol+1] != WALL)
	 torc(crow, ++ccol);
   }
   else
      if (crow < rows - 1 && scrn[crow+1][ccol] != WALL)
	 torc(++crow, ccol);
}


charon (ch)	/* --<PUT ch @ crow,ccol>-- */
char ch;
{
   torc(crow, ccol);
   putchar(ch);
   torc(crow, ccol);	/* put cursor atop ch */
}


torc (row, col) 	/* --<PUT CURSOR @ row,col>-- */
int row, col;
			/* update crow,ccol & ret scrn char */
{
   int xx, yy;

   crow = (row + rows) % rows;
   ccol = (col + cols) % cols;
   xx = (ccol * DX) + XORG; yy = crow;
   toxy(xx, yy);

   return(scrn[crow][ccol]);
}


msgat (x, y, msg)	/* --<WRITE msg @ x,y ON SCREEN>-- */
int x, y;
char *msg;
{
   toxy(x, y); puts(VIDNOR);
   puts(msg);
   puts(VIDINV);
   torc(crow, ccol);	/* put cursor back */
}


cluput (ar, ac, dr, dc) /* --<PUT NU CLUZE - MAYBE>-- */
int ar, ac, dr, dc;
{
   BOOL samewd;

   samewd = YES;
   if (ar == crow)
      for (; ac != ccol && samewd; (ac < ccol) ? ++ac : --ac)
	 if (scrn[ar][ac] == WALL)
	    samewd = NO;
   if (ar != crow || !samewd) {
      ereol(ACX, ACY);
      msgat(ACX, ACY, aclu[crow][ccol]);
}

   samewd = YES;
   if (dc == ccol)
      for (; dr != crow && samewd; (dr < crow) ? ++dr : --dr)
	 if (scrn[dr][dc] == WALL)
	    samewd = NO;
   if (dc != ccol || !samewd) {
      ereol(DCX, DCY);
      msgat(DCX, DCY, dclu[crow][ccol]);
   }
}


BOOL dubchk ()		 /* --<VERIFY DANGEROUS COMMAND>-- */
{
   char chkms[7];
   char ans;

   strcpy(chkms, "SURE? ");

   toxy(PRX, PRY);
   puts(chkms);
   ans = toupper(getchar());
   ereol(PRX, PRY);
   torc(crow, ccol);
   return ((ans == 'Y') ? YES : NO);
}


BOOL achar (c)		/* --<IS c A CHAR OR A WALL?>-- */
char c;
{
   return (isupper(c) || c == WALL);
}


ereol (x, y)		   /* --< POSITION TO x,y & ERASE TO EOL>-- */
int x, y;
{
   toxy(x, y); puts(EREOL);
}
