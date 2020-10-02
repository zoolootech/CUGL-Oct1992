char	copyright[] =
	"\n\t      Version 2.1, Copyright 1984, 1985 by Tom P Jansing \n\n";
/*
	ROBOTS.C - A game for any CRT terminal with an addressable
	cursor, with or without graphic features.  Written for the
	Small-C Version 2B compiler under CP/M-80.

	The game is set up for a 24x80 screen, although this can be
	changed by adjusting the ROWS and COLS values below, and perhaps
	reformatting some of the extra displays.  The only other
	terminal-dependent function is cursor addressing, handled
	by the locate(x,y) function defined just below.  This is currently
	set up for the popular (ESC) (=) (Y+32) (X+32) scheme found on
	Kaypro and Osborne computers, among others.

	Just like arcade games, this one gets harder as you get better
	at it.  If you have trouble however, it gets easier instead.

Gameplay:

	You ($) are in the middle of a galaxy populated with stars (*) and
	killer robots (%).  You can move around, but every time you do, the
	robots take a step toward you.  To survive, you must lure them into
	stars or each other, thereby destroying them.  If you outlive the
	last robot, you will be "rewarded" with a harder game.  For a real
	challenge, try winning five rounds in a row with 99 opponents!

History:

	Thå ideá ió takeî froí á populaò versioî aô thå Õniversity of Alberta,
	which in turn was reportedly inspired by a Basic version in an early
	issue of Byte.  To my knowledge, this is the first implementation
	in C.

By:     Tom Jansing		April 16, 1985
        86 Mission Avenue
	St. Albert, Alberta	or c/o any Edmonton-area RCP/M.
	Canada  T8N 1H8

	This program may be freely distributed for non-commercial use.

Version 2.1 improvements:

	Tightened up the code considerably (COM file is now 10k, not 14)
	by using alloc(), substituting a "tiny" printf() for the real one,
	and compacting the block letters.  Made the relative density of
	stars, and therefore the hardness of the game, a patchable variable.
	Added a flashier title page, and generally polished up the code.
	Aside from being a bit harder (see above), gameplay is basically
	unchanged from version 1.
*/

#include stdio.h

#define NOCCARGC		/* Tell compiler to skip arg count info	*/
#define STAR	'*'		/* On screen and in screen array	*/
#define SPACE	' '		/* Ditto				*/
#define HERO	'$'		/* Shown on screen only			*/
#define ROBOT	'%'		/* Ditto				*/
#define HEADSTRT 6		/* Minimum distance to robots at start	*/
#define TAKEN	-1		/* Used in creating robots		*/
#define ESC	27		/* Panic button				*/
#define ROWS	23		/* # of lines on screen - 1 (exactly!)	*/
#define COLS	78		/* # of columns on screen - 2		*/
#define MAXROBS	99		/* Max. # of robots (never >126 of em!)	*/
#define LENINT	2		/* Length of int variables, in bytes	*/

char	*screen,		/* The playing field			*/
	z,			/* Temporary variable			*/
	key;			/* Player's move			*/

int	stardens = 50;		/* Initial star density (relative)	*/
				/* 75=easy; 50=so-so; 25=downright hard	*/
char	junk1[] = "<-RelStarDensity";			/* For patching	*/

int	*rx,			/* Robot X positions			*/
	*ry,			/*   "   Y	"			*/
	*ralive,		/* Dead/alive flags for robots		*/
	hx, hy,			/* Hero's X and Y positions		*/
	dx, dy,			/* Offset from a robot to 'hero'	*/
	rxx, ryy,		/* X and Y of robot under consideration */
	dead,			/* Status of our hero			*/
	i, j, jj, zx, zy,	/* Temporary variables			*/
	robots,			/* Initial # of robots this round	*/
	rleft,			/* Remaining robots this round		*/
	oldrleft,		/*     "       "    last move		*/
	fillfac,		/* Screen is (fillfac/32767) full of *s */
	fillfac2;		/* fillfac = fillfac2(varies) * robots	*/

/*
**  Locate cursor at given position on screen:  x=across, y=down.
**  ie, locate(0,0); for upper left corner, or
**	locate(40,12); for center of a 24x80 screen.
*/

locate(x,y) int x,y; {
    putchar(27);		/* For Kaypro, Osborne, etc.		*/
    putchar('=');
    putchar(y+32);
    putchar(x+32);
}

/*
**  Keyboard input functions (system-specific):
**  kbhit():	Return true if a key has been pressed but not read.
**  getkey():	Return, without echoing, a key which presumably has
**		already been pressed.
**  flushkbd():	Remove any 'typed-ahead' keys already waiting.
*/

char	ioret;					/* Last key pressed */
kbhit() {
    return ((ioret=cpm(6,-1)));
}

getkey() {
    return (ioret);
}

flushkbd() {
    while (kbhit())
	getkey();
}

main() {

    screen =	alloc(ROWS*COLS);
    rx =	alloc(MAXROBS*LENINT);		/* Get some memory */
    ry =	alloc(MAXROBS*LENINT);
    ralive =	alloc(MAXROBS*LENINT);
    if (screen==0 || rx==0 || ry==0 || ralive==0) {
	printf("Outta memory - TILT!");
	exit();
    }

    hello();			/* Display instructions			*/
    flushkbd();			/* Clear any pending keypresses		*/
    srand(0);			/* Seed random generator when key hit	*/
    cls();			/* Clear screen				*/

/*
**  Some initial settings.
*/

    fillfac2 = stardens;
    dead = NO;
    robots = 1;			/* Becomes 10 in a moment */

/*
**  Main loop.
*/

    for (;;) {			/* for (ever) */

/*
**  Adjust # of stars and robots up or down, depending on success
**  or failure of last round.  If he keeps losing with 10 robots,
**  increase star density (which makes it easier).  Or decrease
**  the # of stars if he keeps winning with 99 robots.  This way,
**  nobody can win many rounds in a row against 99.
*/

    if (dead)
	robots = (robots*7) / 10;
    else
	robots = (robots*10) / 7;

    if (robots < 10) {		/* No <10 robots, no >250 star density	    */
	robots = 10;
	fillfac2 = (fillfac2+=20)>250 ? 250:fillfac2;
    }

    if (robots > 99) {		/* No >99 robots, no <10 star density	    */
	robots = 99;
	fillfac2 = (fillfac2-=20)<10 ? 10:fillfac2;
    }

    fillfac = fillfac2 * robots;	/* Affects # of stars on field	    */
    rleft = robots;			/* All of them are left right now   */
    oldrleft = 0;			/* Force initial display of count   */

/*
**  Fill the screen with randomly placed stars.  (APL, eat your heart out!)
*/

    for (i=0; i<ROWS*COLS; screen[i++] = rand()<fillfac ? STAR:SPACE)
	;

/*
**  Start 'hero' at screen center, but not on top of a star.
*/

    hx = COLS / 2;
    hy = ROWS / 2;
    while (screen[subs(hx,hy)]==STAR)
	++hx;

/*
**  Display screen, scrolling from the bottom for effect.
*/

    locate(0,ROWS);
    putchar('\n');
    for (zy=0; zy<ROWS; ++zy) {
	for (zx=0; zx<COLS; putchar(screen[subs(zx++,zy)]))	/* One line */
	    ;
	putchar('\n');
    }

/*
**  Create some robots and display them on the screen.
*/

    for (j=0; j<robots; ) {		/* Note: missing loop increment	    */
	ralive[j] = YES;
	rxx = rand() % COLS;
	ryy = rand() % ROWS;
	if (screen[subs(rxx,ryy)]==SPACE && abs(rxx-hx)+abs(ryy-hy)>HEADSTRT) {
	    show(rxx,ryy,ROBOT);	/* (Above): Not too close to hero   */
	    screen[subs(rxx,ryy)]=TAKEN;
	    rx[j] = rxx;
	    ry[j] = ryy;
	    ++j;			/* Only if good spot found	    */
	}
    }
    clrflags();				/* Erase 'taken' flags in screen[]  */

    showcnt();
    showhero();
    dead = NO;

    flushkbd();

/*
**  Repeat the following moves until there are no more robots (or hero).
*/

    while (rleft) {

/*
**	If a key was hit, process the move.  If not, just wait.  Note:
**	'else' code could be added to provide time-dependent action.
*/

	if (kbhit()) {			/* Otherwise, keep looping & waiting*/
	    key=toupper(getkey());	/* Key hit, so get it.		    */

/*
**	    Move our hero as appropriate.
*/

	    switch (key) {	

	    case ESC:	alldone();			/* Get out fast	   */

	    case 'S':
	    case '4':	if (spotok(hx-1,hy))		/* Off screen?	   */
			    show(hx--,hy,SPACE);	/* Nope; move	   */
			else				/* No good; ignore */
			    continue;
			break;

	    case 'F':
	    case '6':	if (spotok(hx+1,hy))
			    show(hx++,hy,SPACE);
			else
			    continue;
			break;

	    case 'C':
	    case '2':	if (spotok(hx,hy+1))
			    show(hx,hy++,SPACE);
			else
			    continue;
			break;

	    case 'E':
	    case '8':	if (spotok(hx,hy-1))
			    show(hx,hy--,SPACE);
			else
			    continue;
			break;

	    case 'D':
	    case '5':	break;				/* Stay-put key	*/


	    case 'X':
	    case '1':	if (spotok(hx-1,hy+1))
			    show(hx--,hy++,SPACE);
			else
			    continue;
			break;

	    case 'V':
	    case '3':	if (spotok(hx+1,hy+1))
			    show(hx++,hy++,SPACE);
			else
			    continue;
			break;

	    case 'W':
	    case '7':	if (spotok(hx-1,hy-1))
			    show(hx--,hy--,SPACE);
			else
			    continue;
			break;

	    case 'R':
	    case '9':	if (spotok(hx+1,hy-1))
			    show(hx++,hy--,SPACE);
			else
			    continue;
			break;

	    default:	continue;		/* Wait for another key	*/
	    }

/*
**	    OK, he moved.  Show him in the new spot, and see if he jumped
**	    right into a robot or star.
*/

	    showhero();
	    checkhit();
	    if (dead)
		continue;

/*
**	    Move the robots.  Note: They appear to move simultaneously,
**	    though they're actually moved one at a time.  Because of this,
**	    allow robots to temporarily 'bump' each other while in transit.
*/

	    for (j=0; j<robots; ++j) {
		if (ralive[j]) {
		    dx = (rxx=rx[j]) - hx;
		    dy = (ryy=ry[j]) - hy;

		    z = screen[subs(rxx,ryy)];	/* This spot just taken?    */
		    if (z<0) {
			if (ralive[-1-z])
			    z = ROBOT;
			else
			    z = SPACE;		/* The usual case	    */
		    }

		    if (abs(dx)>abs(dy)) {	/* Move vertical or horiz?  */
			if (dx>0)
			    show(rxx--,ryy,z);  /* Vacate spot; show  */
			else			/* what we stepped on */
			    show(rxx++,ryy,z);
			}
		    else {
			if (dy>0)
			    show(rxx,ryy--,z);
			else
			    show(rxx,ryy++,z);
			}

/*
**		    Check to see if robot landed on another robot or a star.
*/

		    z = screen[subs(rxx,ryy)];	/* See what's there now     */
		    screen[subs(rxx,ryy)]= -1-j;/* Mark spot as occupied    */
		    if (z!=SPACE) {		/* If star or robot 	    */
			--rleft;		/* Another 1 bites the dust */
			show(rxx,ryy,SPACE);	/* Show on screen	    */
			ralive[j] = NO;		/*   and robot array	    */
			}
		    else
			show(rxx,ryy,ROBOT);

/*
**		    If this robot landed on another one, kill that one too.
*/

		    if (z<0) {
			z = -1-z;		/* Convert to robot index   */
			if (ralive[z]) {	/* Kill the innocent party  */
			    --rleft;		/*   if it was still alive  */
			    ralive[z] = NO;
			}
		    }

		    rx[j] = rxx;		/* Put THIS robot back into */
		    ry[j] = ryy;		/*   the array		    */
		}
	    }					/* Next robot		    */

/*
**	    Robots all moved.  Show the updated count and our hero again
**	    (to reset cursor, mainly).  More importantly, check for game over.
*/

	    showcnt();
	    showhero();

	    checkhit();				/* Check for collision	    */
	    clrflags();				/* Clean up robot droppings */
	    flushkbd();
	}					/* (if kbhit)		    */
    }						/* (while robots remain)    */

/*
**  End of round.
*/

    if (dead)
	youlose();
    else
	youwin();

    }						/* End of main loop	    */
}						/* End of main()	    */

/*
**  Return a single subscript from X and Y screen positions.  Note:  Any
**  decent compiler that handles multiple-dimension arrays should 
**  produce code almost as efficient with this method anyway.
*/

subs(x,y) int x,y; {
    return (y*COLS+x);
}

/*
**  Display hero on screen in current spot, with cursor on top for visibility.
*/

showhero() {
    locate(hx,hy);
    putchar(HERO);
    putchar('\b');
}

/*
**  Show number of robots remaining, but only if number has changed.
*/

showcnt() {
    if (rleft != oldrleft) {

	locate(0,ROWS);
	printf("There are ");
	putint(rleft);
	printf(" out of ");
	putint(robots);
	printf(" robots left.   ");
	oldrleft = rleft;

	locate(COLS-12,ROWS);
	printf("(ESC=quit)");
    }
}

/*
**  Display a character on the screen in a specified spot.
*/

show(x,y,c) int x,y; char c; {
    locate(x,y);
    putchar(c);
}

/*
**  Is our hero trying to jump off the screen?
*/

spotok(x,y) int x,y; {
    if (x<0 || x>=COLS || y<0 || y>=ROWS)
	return (NO);
    else
	return (YES);
}

/*
**  Clear the robots' place markers in the screen array (not to be
**    confused with the screen itself).
*/

clrflags() {
    for (jj=0; jj<robots; jj++)
	screen[subs(rx[jj],ry[jj])] = SPACE;
}

/*
**  See if our hero hit a star or robot.
*/

checkhit() {
    if (screen[subs(hx,hy)]!=SPACE) {		/* Hit star or >1 robots?   */
	dead = YES;				/* That's all, folks!	    */
	rleft = 0;				/* Force end of round	    */
    }
    else {
	for (jj=0; jj<robots; jj++) {
	    if (ralive[jj] && hx==rx[jj] && hy==ry[jj]) {    /* Direct hit? */
		dead = YES;
		rleft = 0;
	    }
	}
    }
}

/*
**  Clean up and exit program.
*/

alldone() {
    cls();
    exit();
}

/*
**  Clear screen by scrolling (needs no customizing this way).
*/

cls() {
    int n;
    for (n=0; n<ROWS+ROWS+2; ++n)
	putchar('\n');
}

/*
**  Instructions.  Split in 2 parts to avoid "literal queue overflow" with CV2.
*/

hello() {
  cls();
  locate(0,0);
  xpand("8 8%5 6%3 8%5 6%3 8%3 7%");
  xpand("1\t1 2%4 2%3 2%4 2%3 2%4 2%3 2%4 2%2 2%1 2%1 2%2 2%5 2%");
  xpand("1\t1 2%4 2%3 2%4 2%3 2%4 2%3 2%4 2%5 2%5 2%");
  xpand("1\t1 7%4 2%4 2%3 7%4 2%4 2%5 2%6 7%");
  xpand("1\t1 2%2 2%5 2%4 2%3 2%4 2%3 2%4 2%5 2%9 3 2%");
  xpand("1\t1 2%3 2%4 2%4 2%3 2%4 2%3 2%4 2%5 2%5 2%5 2%");
  xpand("1\t3%4 3%3 6%3 8%5 6%6 2%6 7%");
  printf(copyright);	/* Honor system */
  hello2();
}

hello2() {
  printf("\t\t$  <--\tThis is you.  At all costs, avoid the\n");
  printf("\t\t%  <--\tRobots, which close in on you.  Also,\n");  /* K&R: %% */
  printf("\t\t*  <--\tStars kill anything that touches them.\n\n");

  printf("Use the keys shown on the right to move around:   -->\t");
	printf("W E R\t\t7 8 9\n");
  printf("For example, press R or 9 to move to the upper\t\t");
	printf("S D F\t or \t4 5 6\n");
  printf("right, or D or 5 to stay put.\t\t\t\tX C V\t\t1 2 3\n\n");

  printf("You will have between 10 and 99 robots to deal with.  ");
  printf("Every time you make a\nmove, they close in on you.  ");
  printf("Try to lure them into stars or each other to\ndestroy them.  ");
  printf("If you manage to outlive the robots, you will be \"rewarded\"\n");
  printf("with a harder game.  Beware - this game never ends...  ");
  printf("never ends...  never en\n\n\t\t\t\tInsert Coin: ");
}

you() {
    int repeat;
    locate(0,ROWS);
    putchar('\n');
    blanklines(10);
    xpand("2\t5 3Y3 3Y4 5O5 3U3 3U6 ");
    xpand("2\t6 3Y1 3Y4 7O4 3U3 3U6 ");
    xpand("2\t7 5Y4 3O3 3O3 3U3 3U6 ");
    for (repeat=0; repeat<3; ++repeat)
	xpand("2\t8 3Y5 3O3 3O3 3U3 3U6 ");
    xpand("2\t8 3Y6 7O5 7U7 ");
    xpand("2\t8 3Y7 5O7 5U8 ");
    blanklines(4);
    }

youlose() {
    you();
    xpand("2\t3L9 1 5O6 8S3 9E");
    xpand("2\t3L9 7O4 8S4 9E");
    xpand("2\t3L8 3O3 3O3 2S9 1 3E6 ");
    xpand("2\t3L8 3O3 3O4 7S4 9E");
    xpand("2\t3L8 3O3 3O5 7S3 9E");
    xpand("2\t3L8 3O3 3O9 1 2S3 3E6 ");
    xpand("2\t9L3 7O5 8S3 9E");
    xpand("2\t9L4 5O5 8S4 9E");
    blanklines(1);
    }

youwin() {
    you();
    xpand("2\t5 2W5 2W3 9I3 3N4 2N6 ");
    xpand("2\t5 2W5 2W3 9I3 4N3 2N6 ");
    xpand("2\t5 2W5 2W6 3I6 4N3 2N6 ");
    xpand("2\t5 2W5 2W6 3I6 2N1 3N1 2N6 ");
    xpand("2\t5 2W2 1W2 2W6 3I6 2N1 3N1 2N6 ");
    xpand("2\t5 2W1 3W1 2W6 3I6 2N3 4N6 ");
    xpand("2\t6 7W4 9I3 2N3 4N6 ");
    xpand("2\t7 5W5 9I3 2N4 3N6 ");
    blanklines(1);
    }

/*
**  Print n blank lines at same speed as the block letters.
*/

blanklines(n) int n; {
    while(n--)
	xpand("2\t9 9 9 9 8 ");
    }

/*
**  Expand cryptic strings into block letters.
*/

xpand(code) char *code; {
    char xbyte, xletter;
    while ((xbyte=*code++)) {		/* Until end of string	*/
	xbyte -= '0';			/* Convert to binary	*/
	xletter = *code++;		/* Get following char	*/
	while (xbyte--)
	    putchar(xletter);
    }
    putchar('\n');			/* Newline when done	*/
}

/*
**  Poor man's printf() - takes up less room than the standard version.
**  Also, a nifty little routine (courtesy K&R) to print an int value in lieu.
*/

printf(wisdom) char *wisdom; {
    while (*wisdom)
	putchar(*wisdom++);
}

putint(n) int n; {
    int i;
    if ((i=n/10)!=0)
	putint(i);
    putchar(n%10+'0');
}

#include RANDOM.C
#include ABS.C
#include ALLOCMEM.C
#asm
	LINK	CV2LIB		; LASM opcode - CV2LIB & CV2CALL precompiled.
#endasm
