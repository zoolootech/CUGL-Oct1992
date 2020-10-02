#define	MINTEL	2500	/* monster intelligence level (lower is smarter) */

/* dfp */
#define	BUFSIZ	100
#define CONTROL_C 0x03	/* only way to get out of this */
#define CLOCKRATE 4	/* CPU clock speed, in MHz */
#define	MAXPAC	3	/* maximum number of pacmen to start */
#define	MAXMONSTER	4	/* max number of monsters */
#define EMPTY	'E'
#define FULL	'F'
#define LEFT	'4'
#define RIGHT	'6'
#define	NORTH	'8'	/* means UP, but UP defined in vsinit() */
#define NNORTH	'w'
#define	DOWN	'2'
#define NDOWN	'x'
#define HALT	'5'
#define DELETE	'\177'
#define ABORT	('\\'-0x40)
#define QUIT	'q'
#define CNTLS	('S'-0x40)
#define BUF_SIZE	32
#define	TRUE	1
#define	FALSE	0
#define	UPINT	(-1)
#define	DOWNINT	1
#define	LEFTINT	(-2)
#define	RIGHTINT	2

/* Codes with 0200 bit set are special graphics characters, for board
 *   display (SAW)
 */

#define	PACMAN	('@' | 0200)
#define	MONSTER	('M' | 0200)
#define	RUNNER	('S' | 0200)
#define	TREASURE	('$' | 0200)
#define	CHOICE		('*' | 0200)
#define	GOLD		('+' | 0200)
#define	POTION		('X' | 0200)
#define	VACANT		(' ' | 0200)	/* space */
#define	WALL		('O' | 0200)
#define	GATE		('-' | 0200)
#define PUP	('^' | 0200)
#define PDOWN	('v' | 0200)
#define PLEFT	('<' | 0200)
#define PRIGHT	('>' | 0200)

#define	START	0
#define	RUN	1
#define	FAST	1
#define	SLOW	0
#define	PSTARTX	18
#define	PSTARTY	17
#define	MSTARTX	16	/* monster starting position */
#define	MSTARTY	10	/* monster starting position */
#define	MBEGINX	18	/* monster beginning position */
#define	MBEGINY	7	/* monster beginning position */
#define	TRYPOS	13
#define	TRXPOS	20
#define	GOTONE	1
#define	TURKEY	(-1)
#define	DUP	1
#define	DDOWN	4
#define	DRIGHT	3
#define	DLEFT	2
#define	DNULL	0
#define	BRDX	40
#define	BRDY	23
#define XWRAP	38
#define	TREASVAL	20
#define	KILLSCORE	10
#define	BEEP		07	/* ctrl-g */
#define	MSTARTINTVL	10
#define	POTINTVL	25
#define GOLDCNT	185

struct	pac
{
	int	xpos;	/* horizontal position */
	int	ypos;	/* vertical   position */
	int	dirn;	/* direction of travel */
	int	dirx;	/* desired direction of travel */
	int	speed;	/* FAST/SLOW	       */
	int	danger;	/* TRUE if can eat     */
	int	stat;	/* status		*/
};

#define nap(x)  

/* Global variable definitions (saw)			*/

/*
 * initbrd is used to re-initialize the display
 * array once a new game is started.
 */
char	initbrd[BRDY][BRDX];

/*
 * brd is kept for historical reasons.
 * It should only be used in the routine "which"
 * to determine the next move for a monster or
 * in the routine "monster" to determine if it
 * was a valid move. Admittedly this is redundant
 * and could be replaced by initbrd, but it is kept
 * so that someday additional intelligence or
 * optimization could be added to the choice of
 * the monster's next move. Hence, note the symbol
 * CHOICE at most points that a move decision
 * logically HAS to be made.
 */
char	brd[BRDY][BRDX];
/*
 * display reflects the screen on the player's
 * terminal at any point in time.
 */
char	display[BRDY][BRDX];

int lastchar;

char	combuf[BUFSIZ],
	message[81],	/* temporary message buffer */
	inbuf[2];

int	ppid,
	cpid,
	game,
	killcnt,
	vs_rows,
	vs_cols;

unsigned
	pscore;

/* long	timein;		*/
unsigned	timein;


/* from monster.c:			*/


int	rscore[MAXMONSTER];

struct pac
	monst[MAXMONSTER];



/*
 * global variables from pacman.c
 */


int	pacsymb,
	rounds,		/* time keeping mechanism */
	killflg,
	delay,
	potion,
	goldcnt,		/* no. of gold pieces remaining */
	potioncnt;

struct pac
	Pac;

struct pac
	pacstart;


struct pac
	*pacptr;

	int ospeed;		/* baud rate for crt (for tputs()) */

char	Freeze;			/* true on ^S typed.			*/
char	InpChar;
char	rev, graph;		/* reverse video, graphic modes.	*/

#define	SCFILE	"pacman.rec"	/* file is 512 bytes long.		*/
char	UpdSc;			/* 1 means update score file.		*/

struct Record {			/* for top 8 games:			*/
	int Score;		/* 64 bytes/record			*/
	char Name[62];
	} Scores[10];

char	jbuf[6];		/* setjmp/longjmp buffer		*/
