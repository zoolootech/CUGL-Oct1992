/*   DISK CATALOG : HEADERS.DOC
	
C Users Group Volume Number 125 - VIDEO TERMINAL LIRARIES
Cataloged By Ben Rockefeller 5/15/85    

*/

/*           HEADER: CUG125.05; 

	      TITLE: VIDEO TERMINAL LIBRARIES;
            VERSION: 1.0;
       	       DATE: 5/15/85;
        DESCRIPTION: "Rewrite of R.H. Halstead's Polish Pong
		      (PPONG.C) with conversion for vt52 terminal";
 	   KEYWORDS: game;
             SYSTEM: Osborne 1, DEC vt52;
           FILENAME: DEFLECT.C;
	   WARNINGS: "Requires video routines linked";
	   SEE-ALSO: DEFLECT.NRO;
            AUTHORS: Stephen L. Browning;
          COMPILERS: BDC C;
*/


/*
 *	Rewrite of R. H. Halstead's Polish Pong (PPONG.C)
 *	with conversions for VT52 terminal.
 *
 *	This version does not require the terminal to have
 *	a special graphics mode, just cursor addressing
 *	capabilities.
 *
 *	Stephen L. Browning
 *	5723 North Parker
 *	Indianapolis, IN 46220
 */

#include "a:bdscio.h"
#include "a:ascii.h"

#define MAXX	80		/* horizontal size of board */
#define MAXY	23		/* vertical size */
#define MAXTARG	20		/* no. of targets per game */
#define ISPEED	400		/* initial ball speed */
#define SPEDINC	100		/* increment/decrement in ball speed */

/*
 *	location of status strings
 */

#define TIMX	50		/* time */
#define TIMY	0
#define TARGX	30		/* targets */
#define TARGY	0
#define SPEEDX	10		/* current speed setting */
#define SPEEDY	0
#define BESTX	70		/* best score so far */
#define BESTY	0

#define MSPS	960		/* "millisecs" per second */

#define QUITCH	ETX		/* ^C exits program */
#define DELETE	0177		/* DELETE (rubout) restarts game */

#define XON	DC1		/* ^Q */
#define XOFF	DC3		/* ^S */

#define VBAR	'|'
#define HBAR	'-'
#define SLASH	'/'
#define BSLASH	'\\'
#define BALL	'O'
#define TARGET	'*'

char board[MAXX][MAXY];		/* board with current layout */
int best;			/* best score so far */
int inchar;			/* character input buffer */
int ballx,bally,ballxv,ballyv;	/* ball positioning data */
int speed,dist;			/* speed control */
int targleft;			/* no. of targets remaining */
int msecs,secs;			/* timers */
int newtime;			/* time remaining */
char bellflag;			/* turns off/on bell output */


main()
{
	int i;

	clrscrn();
	puts("Welcome to Deflection\n");
	for (i=0; i<30000; ++i) ;
	best = 0x7fff;
	speed = ISPEED;
	bellflag = TRUE;
	while (playgame());
	clrscrn();
}


playgame()
{
	int i,j;
	char buff[100];

	inchar = -1;		/* initially no character input */
	clrscrn();
	getchx();
	srand1("\nType any key to start game (h for help): ");
	i = getchar();
	if (i == QUITCH) {
		return(0);
	}
	if (tolower(i) == 'h') {
		help();
	}
	inchar = -1;		/* reset input buffer */
	initboard();
	targleft = MAXTARG;
	clrscrn();
	for (j=MAXY-1; j>=0; --j) {
		for (i=0; i<MAXX; ++i) putchx(board[i][j]);
		putchx('\n');
	}
	outs(SPEEDX-7,SPEEDY,"Speed: ");
	outs(TARGX-14,TARGY,"Targets Left: ");
	outs(TIMX-11,TIMY,"Time Used: ");
	if (best < 0x7fff) {
		sprintf(buff,"Best Time: %03d",best);
		outs(BESTX-11,BESTY,buff);
	}
	putspeed();
	puttarg();
	msecs = secs = 0;
	puttime();
	ouch(ballx,bally+1,BALL);
	while (moveball()) ;
	if (targleft == 0 && secs < best) best = secs;
	return(1);
}



/*
 *	initialize screen for new game
 */

initboard()
{
	int i,j;

	ballx = rand() % (MAXX-2) + 1;
	bally = rand() % (MAXY-2) + 1;
	ballxv = ballyv = 0;
	i = (rand() & 2) - 1;		/* i = -1 or +1 */
	if (rand() & 1) ballxv = i;
	else ballyv = i;
	for (i=0; i<MAXX; ++i)
		for (j=0; j<MAXY; ++j) board[i][j] = ' ';
	for (i=0; i<MAXX; ++i) board[i][0] = board[i][MAXY-1] = HBAR;
	for (i=0; i<MAXY; ++i) board[0][i] = board[MAXX-1][i] = VBAR;
	board[0][0] = board[0][MAXY-1] = board[MAXX-1][0] = board[MAXX-1][MAXY-1] = '+';
	board[rand()%(MAXX-2)+1][rand()%(MAXY-2)+1] = TARGET;
}



/*
 *	move the ball and record deflections
 */

moveball()
{
	int i,nx,ny;

	dist = 0;
	i = inchar;
	if (i > 0) {
		inchar = -1;
		switch (i) {
		case SLASH:
		case BSLASH:
			if (board[ballx][bally] == ' ')
				board[ballx][bally] = i;
			else putchx(BEL);
			break;
		case 'd':
		case 'D':		/* delete character */
			i = board[ballx][bally];
			if (i == SLASH || i == BSLASH) board[ballx][bally] = ' ';
			else putchx(BEL);
			break;
		case 'f':
		case 'F':		/* faster */
			if (speed < 1000) {
				speed += SPEDINC;
				putspeed();
			}
			break;
		case 's':
		case 'S':		/* slower */
			if (speed > (SPEDINC + 50)) {
				speed -= SPEDINC;
				putspeed();
			}
			break;
		case DELETE:		/* new game */
			clrscrn();
			return(0);
			break;
		case 'h':
		case 'H':		/* help */
			help();
			return(0);
			break;
		default:
			putchx(BEL);
			break;
		}
	}
	switch (board[ballx][bally]) {
	case VBAR:
		ballxv = -ballxv;
		break;
	case HBAR:
		ballyv = -ballyv;
		break;
	case BSLASH:
		i = ballxv;
		ballxv = -ballyv;
		ballyv = -i;
		break;
	case SLASH:
		i = ballxv;
		ballxv = ballyv;
		ballyv = i;
		break;
	case TARGET:
		if (--targleft <= 0) {
			clrscrn();
			return(0);
		}
		puttarg();
		board[ballx][bally] = ' ';
		do {
			nx = rand() % (MAXX-2) + 1;
			ny = rand() % (MAXY-2) + 1;
		} while (board[nx][ny] != ' ');
		board[nx][ny] = TARGET;
		ouch(nx,ny+1,TARGET);
		break;
	}
	nx = ballx + ballxv;
	ny = bally + ballyv;
	ouch(nx,ny+1,BALL);
	ouch(ballx,bally+1,board[ballx][bally]);
	setcux(0,0);
	if (newtime) puttime();
	ballx = nx;
	bally = ny;
	/*
	 * now delay for awhile
	 */
	while (dist < (ballyv ? 22000 : 10000)) putchx(NUL);
	return(1);
}



/*
 *	explain the game
 */

help()
{
	clrscrn();
	printf("The object of the game is to deflect the ball\n");
	printf("by using the '/' and '\\' characters so that it\n");
	printf("hits the target.  The time it takes to eliminate\n");
	printf("20 targets is your score.\n\n");
	printf("Other characters recognized:\n\n");
	printf("\tf - increases the speed of play\n");
	printf("\ts - decreases the speed of play\n");
	printf("\td - deletes the character at the current position\n");
	printf("\th - prints this message\n");
	printf("\t^S - suspends execution\n");
	printf("\t^Q - resumes execution\n");
	printf("\tDELETE(rubout) - restarts the game\n");
	printf("\t^C - quit\n");
	printf("\nAny other characters will typically cause the bell\n");
	printf("to sound.  Typing ^G (bell) will toggle the flag\n");
	printf("which enables/disables the output of bells.\n\n");
	printf("Type a character to resume play: ");
	getchar();
	clrscrn();
}


/*
 *	Output character at location x,y
 */

ouch(x,y,c)
int x,y;
int c;
{
	setcux(x,y);
	putchx(c);
}


/*
 *	Position cursor to x,y - then output string
 */

outs(x,y,s)
int x,y;
char *s;
{
	setcux(x,y);
	puts(s);
}


/*
 *	Special version of puts - needed by srand1
 */

puts(s)
char *s;
{
	while (*s) putchx(*s++);
}


/*
 *	Special variation of putchar
 */

putchx(c)
int c;
{
	if (++msecs >= MSPS) {
		msecs = 0;
		++secs;
		newtime = TRUE;
	}
	dist += speed;
	if (inchar == -1) {
		inchar = getchx();
		switch (inchar) {
		case BEL:
			bellflag = !bellflag;
			break;
		case XOFF:
			while (inchar != XON) {
				inchar = getchx();
			}
			inchar = -1;
			break;
		}
	}
	if (c == '\0') return;
	if (c != BEL || bellflag) bios(4,c);
	if (c == '\n') bios(4,'\r');
}



/*
 *	special variation of getchar
 */

getchx()
{
	int c;

	if (bios(2)) {
		for (c=0; c<10; ++c) ;		/* debounce */
		c = bios(3) & 0177;
		if (c == QUITCH) {
			clrscrn();
			exit();
		}
		return(c);
	}
	return(-1);
}



/*
 *	print speed
 */

putspeed()
{
	char buff[10];

	sprintf(buff,"%03d",speed/10);
	buff[3] = '\0';
	outs(SPEEDX,SPEEDY,buff);
}


/*
 *	update target count
 */

puttarg()
{
	char buff[10];

	sprintf(buff,"%02d",targleft);
	buff[2] = '\0';
	outs(TARGX,TARGY,buff);
}


/*
 *	update time display
 */

puttime()
{
	char buff[10];

	sprintf(buff,"%03d",secs);
	buff[3] = '\0';
	outs(TIMX,TIMY,buff);
	newtime = FALSE;
}


/*
 *	Special version of set cursor which utilizes putchx.
 *	This version is for VT-52.
 *	Well, so much for portability.
 */

setcux(x,y)
int x,y;
{
	putchx(ESC);
	putchx('Y');
	putchx(23-y+' ');
	putchx(x+' ');
}