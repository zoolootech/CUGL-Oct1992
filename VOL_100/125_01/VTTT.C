/*   DISK CATALOG : HEADERS.DOC
	
C Users Group Volume Number 125 - VIDEO TERMINAL LIRARIES
Cataloged By Ben Rockefeller 5/15/85    

*/

/*           HEADER: CUG125.19; 

	      TITLE: VIDEO TERMINAL LIBRARIES;
            VERSION: 1.0;
       	       DATE: 5/15/85;  			
        DESCRIPTION: "Video Tic Tac Toe game";
	   KEYWORDS: game, video terminal routines;
             SYSTEM: Osborne 1, DEC vt52;
           FILENAME: VTTT.C;
	   WARNINGS: "Requires video terminal routines linked;
		     runs only on Osborne 1 and DEC vt52";
	   SEE-ALSO: VTTT.NRO;
            AUTHORS: Stephen L. Browning;
          COMPILERS: BDC C;
*/


/*
 *	Video Tic Tac Toe
 *
 *	A rewrite of Tic Tac Toe by Leor Zolman
 *	using the cursor addressing capabilities of a CRT.
 *
 *	Stephen L. Browning
 *	5723 North Parker Avenue
 *	Indianapolis, Indiana 46220
 */

#define TRUE	1
#define FALSE	0

/*
 *	To implement on a 52 X 24 display such as the Osborne,
 *	reduce each of the horizontal reference points by 10
 *	(ie subtract 10 from XR, WINX, LOSSX, and TIEX).
 */

#define XR	20		/* reference coordinate for play grid */
#define YR	0
#define CHW	8		/* width and height of x's and o's */
#define CHH	4
#define BLKW	(CHW+2)		/* size of spaces for x's and o's */
#define BLKH	(CHH+2)

#define WINX	15		/* positioning data for messages */
#define WINY	21
#define LOSSX	30
#define LOSSY	21
#define TIEX	50
#define TIEY	21
#define QRYX	0
#define QRYY	23

#define X	1
#define O	5
#define EMPTY	0

int wins;		/* number of times player has won */
int losses;		/* number of times player has lost */
int ties;		/* number of tie games */
char board[9];		/* playing board */
char xchar[CHH][CHW];	/* pattern definition for 'x' */
char ochar[CHH][CHW];	/* pattern definition for 'o' */
int playchar;		/* player's character - either X or O */
int compchar;		/* computer's character */
int winner[8][3];	/* definition of winning combinations */

main()
{
	int i;
	int newgame;
	int playfirst;

	wins = losses = ties = 0;
	initb(&xchar[0][0],"32,32,88,32,32,88,32,32");
	initb(&xchar[1][0],"32,32,32,88,88,32,32,32");
	initb(&xchar[2][0],"32,32,32,88,88,32,32,32");
	initb(&xchar[3][0],"32,32,88,32,32,88,32,32");
	initb(&ochar[0][0],"32,32,79,79,79,79,32,32");
	initb(&ochar[1][0],"32,32,79,32,32,79,32,32");
	initb(&ochar[2][0],"32,32,79,32,32,79,32,32");
	initb(&ochar[3][0],"32,32,79,79,79,79,32,32");
	initw(winner,"0,1,2,3,4,5,6,7,8,0,3,6,1,4,7,2,5,8,0,4,8,2,4,6");
	newgame = TRUE;
	playfirst = TRUE;
	srand(0);
	while (newgame) {
		initbrd();
		if (playfirst) {
			playchar = X;
			compchar = O;
		}
		else {
			playchar = O;
			compchar = X;
		}
		play(playfirst);
		score();
		newgame = query();
		playfirst = !playfirst;
	}
	clrscrn();
}

/*
 *	Initialize display and parameters
 */

initbrd()
{
	int i;

	clrscrn();
	for (i=0; i<(3*BLKW+2); ++i) {
		setcur(XR+i,YR+BLKH);
		putchar('-');
		setcur(XR+i,YR+2*BLKH+1);
		putchar('-');
	}
	for (i=0; i<(3*BLKH+2); ++i) {
		setcur(XR+BLKW,YR+i);
		putchar('|');
		setcur(XR+2*BLKW+1,YR+i);
		putchar('|');
	}
	setcur(XR+BLKW,YR+BLKH);
	putchar('+');
	setcur(XR+2*BLKW+1,YR+BLKH);
	putchar('+');
	setcur(XR+BLKW,YR+2*BLKH+1);
	putchar('+');
	setcur(XR+2*BLKW+1,YR+2*BLKH+1);
	putchar('+');
	setcur(XR+BLKW-1,YR+2*BLKH+2);
	putchar('1');
	setcur(XR+BLKW+1,YR+2*BLKH+2);
	putchar('2');
	setcur(XR+2*BLKW+2,YR+2*BLKH+2);
	putchar('3');
	setcur(XR+BLKW-1,YR+BLKH+1);
	putchar('4');
	setcur(XR+BLKW+1,YR+BLKH+1);
	putchar('5');
	setcur(XR+2*BLKW+2,YR+BLKH+1);
	putchar('6');
	setcur(XR+BLKW-1,YR+BLKH-1);
	putchar('7');
	setcur(XR+BLKW+1,YR+BLKH-1);
	putchar('8');
	setcur(XR+2*BLKW+2,YR+BLKH-1);
	putchar('9');
	for (i=0; i<9; ++i) board[i] = EMPTY;
}


/*
 *	Play one game
 */

play(playturn)
int playturn;
{
	int i;
	int bestmove;
	int round;

	round = 0;
	while (round < 9) {
		if (playturn) {
			eralin(QRYY);
			setcur(QRYX,QRYY);
			puts("Your move (1-9)? ");
			i = getchar() - '0';
			if (i < 1 || i > 9 || board[i-1] != EMPTY) {
				eralin(QRYY);
				setcur(QRYX,QRYY);
				puts("Illegal move");
				for (i=0; i<7000; ++i) ;
				continue;
			}
			board[i-1] = playchar;
			put(playchar,i);
			if (won(playchar)) {
				++wins;
				break;
			}
			playturn = !playturn;
		}
		else {
			if (round == 0) bestmove = rand() % 9;
			else if (round == 1) {
				if (board[4] != EMPTY) {
					bestmove = rand()%2*6 + rand()%2*2;
				}
				else bestmove = 4;
			}
			else ttteval(compchar,playchar,&bestmove);
			board[bestmove] = compchar;
			put(compchar,bestmove+1);
			if (won(compchar)) {
				++losses;
				break;
			}
			playturn = !playturn;
		}
		++round;
	}
	if (tied()) ++ties;
}


/*
 *	Put the appropriate character pattern in the
 *	designated block
 */

put(c,blk)
char c;
int blk;
{
	char *p;
	int xref, yref;
	int x, y;

	xref = XR + (((blk-1)%3)*BLKW) + ((blk-1)%3) + 1;
	yref = YR + (((9-blk)/3)*BLKH) + ((9-blk)/3) + 1;
	p = c == X ? xchar : ochar;
	for (y=0; y<CHH; ++y) {
		setcur(xref,yref+y);
		for (x=0; x<CHW; ++x) putchar(*p++);
	}
}


/*
 *	Ask about another game
 */

query()
{
	int c;

	do {
		eralin(QRYY);
		setcur(QRYX,QRYY);
		puts("Another game (y or n)? ");
	} while ((c=toupper(getchar())) != 'Y' && c != 'N');
	if (c == 'Y') return(TRUE);
	return(FALSE);
}


/*
 *	Display the current score
 */

score()
{
	eralin(WINY);
	setcur(WINX,WINY);
	printf("Wins = %03d",wins);
	setcur(LOSSX,LOSSY);
	printf("Losses = %03d",losses);
	setcur(TIEX,TIEY);
	printf("Ties = %03d",ties);
}

/*
 *	Test for tie game
 */

tied()
{
	int i;

	for (i=0; i<9; ++i) {
		if (board[i] == EMPTY) return(FALSE);
	}
	return(TRUE);
}


/*
 *	Evaluate possible future moves
 */

ttteval(cc,pc,pbest)
int cc;		/* computer's character */
int pc;		/* player's character */
int *pbest;	/* pointer to bestmove */
{
	int i;
	int safemove;
	int status;
	int loseflag;

	if (won(cc)) return(1);
	if (won(pc)) return(-1);
	if (tied()) return(0);
	loseflag = 1;
	for (i=0; i<9; ++i) {
		if (board[i] != EMPTY) continue;
		board[i] = cc;		/* try the move */
		status = ttteval(pc,cc,pbest);
		board[i] = 0;		/* restore empty space */
		if (status == -1) {
			*pbest = i;
			return(1);
		}
		if (status == 1) continue;
		loseflag = 0;
		safemove = i;
	}
	*pbest = safemove;
	return(-loseflag);
}


/*
 *	Test for game won
 */

won(c)
int c;
{
	int i;

	for (i=0; i<8; ++i) {
		if (board[winner[i][0]] == c &&
		    board[winner[i][1]] == c &&
		    board[winner[i][2]] == c) return(TRUE);
	}
	return(FALSE);
}
