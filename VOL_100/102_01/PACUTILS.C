#include "pacdefs.h"
#include <bdscio.h>

char kbhit()
 {	char c;
	if ((inp(CSTAT) & CIMASK) == (CAHI ? CIMASK : 0))
	    switch(c = (inp(CDATA) & 0177))
		 {	case 'Q'-64:	Freeze = 0; break;
			case 'S'-64:	Freeze = 1; break;
			default:	InpChar = c;
		 }
	return InpChar;
 }

char getchar()
 {	char c;
	while (!(c = kbhit()));
	InpChar = 0;	return c;
 }

putchar(c)
 char c;
 {	do
		kbhit();
	while (Freeze || (inp(CSTAT) & COMASK) == (CAHI ? 0 : COMASK));
	outp(CDATA, c); }

rawio()
 {
	InpChar = Freeze = 0;
	rev = graph = 0;
 }


/* Initialize a board:						*/

in1(bd, y, cc)
 char bd[BRDY][BRDX];
 char *cc, y;
 {	char x;
	for (x=0; *cc; ) bd[y][x++] = *cc++;
	bd[y][x] = 0;
 }

initbd(bd)
 char bd[BRDY][BRDX];
 {	char i;
	in1(bd, 0, "OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO");
	in1(bd, 1, "O + + + * + + + + OOO + + + + * + + + O");
	in1(bd, 2, "O X OOO + OOOOO + OOO + OOOOO + OOO X O");
	in1(bd, 3, "O * + + * + * + * + + * + * + * + + * O");
	in1(bd, 4, "O + OOO + O + OOOOOOOOOOO + O + OOO + O");
	in1(bd, 5, "O + + + * O + + + OOO + + + O * + + + O");
	in1(bd, 6, "OOOOOOO + OOOOO + OOO + OOOOO + OOOOOOO");
	in1(bd, 7, "      O + O + + * + + * + + O + O      ");
	in1(bd, 8, "      O + O + OOO - - OOO + O + O      ");
	in1(bd, 9, "OOOOOOO + O + O         O + O + OOOOOOO");
	in1(bd, 10, "        * + * O         O * + *        ");
	in1(bd, 11, "OOOOOOO + O + O         O + O + OOOOOOO");
	in1(bd, 12, "      O + O + OOOOOOOOOOO + O + O      ");
	in1(bd, 13, "      O + O * + + + + + + * O + O      ");
	in1(bd, 14, "OOOOOOO + O + OOOOOOOOOOO + O + OOOOOOO");
	in1(bd, 15, "O + + + * + * + + OOO + + * + * + + + O");
	in1(bd, 16, "O X OOO + OOOOO + OOO + OOOOO + OOO X O");
	in1(bd, 17, "O + + O * + * + * + + * + * + * O + + O");
	in1(bd, 18, "OOO + O + O + OOOOOOOOOOO + O + O + OOO");
	in1(bd, 19, "O + * + + O + + + OOO + + + O + + * + O");
	in1(bd, 20, "O + OOOOOOOOOOO + OOO + OOOOOOOOOOO + O");
	in1(bd, 21, "O + + + + + + + * + + * + + + + + + + O");
	in1(bd, 22, "OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO");
}


/* Some display utilities					*/

POS(row, col)
 char row, col;
 {	putchar(033); putchar('Y'); putchar(row+32); putchar(col+32);
 }

PLOT(a, b, c)
 {	POS(a, b);
	putchar(c); }

SPLOT(a, b, c)
 char *c;
 {	POS(a, b);
	while (*c) putchar(*c++); }

/* reverse video on/off:				*/

revid(on)
 char on;
 {	if (on)
	 { if (!rev) { rev=1; printf("\033p"); }
	   return; }
	if (rev) { rev=0; printf("\033q"); }}

/* graphics mode on/off:				*/

graphics(on)
 char on;
 {	if (on)
	 { if (!graph) { graph=1; printf("\033F"); }
	   return; }
	if (graph) { graph=0; printf("\033G"); }}


/* Special version, for plotting the board:		*/

BPLOT(a, b, c)
 char *c;
 {	char rev, ch;
	rev = 0;
	POS(a, b);
	while (*c) switch(ch = *c++) {
	 case 'O':	graphics(0); revid(1); putchar(' '); continue;
	 default:	graphics(0); revid(0); putchar(ch); continue;
	 }
	graphics(0);	revid(0);
 }

update()
{
	POS(0, 52); printf("%6d", pscore);
	POS(21, 57); printf("%6d", goldcnt);
}

reinit()
{
	int locx, locy;
	char tmp;

	for (locy = 0; locy < BRDY; locy++)
	{
		for (locx = 0; locx < BRDX; locx++)
		{
			tmp = initbrd[locy][locx];
			brd[locy][locx] = tmp;
			if ((display[locy][locx] = tmp) == CHOICE)
			{
				display[locy][locx] = GOLD;
			};
		};
	};
	goldcnt = GOLDCNT;
	delay -= (delay / 10);	/* hot it up */
}

errgen(string)
char	*string;
{
	SPLOT(23,45,string);
}

dokill(mnum)
	int mnum;
{
	struct pac *mptr;

	PLOT(0, 0, BEEP);
	if (pacptr->danger == TRUE)
	{
		if (++killcnt == MAXMONSTER)
		{
			if (display[TRYPOS][TRXPOS] == GOLD)
			{
				goldcnt--;
			};
			display[TRYPOS][TRXPOS] = TREASURE;
			PLOT(TRYPOS, TRXPOS, TREASURE);
			killcnt = 0;
		};
		SPLOT(5, 45, "MONSTERS KILLED: ");
		POS(5, 62); printf("%1d", killcnt);
		mptr = (&monst[mnum]);
		mptr->ypos = MSTARTY;
		mptr->xpos = MSTARTX + (2 * mnum);
		mptr->stat = START;
		PLOT(mptr->ypos, mptr->xpos, MONSTER);
		pscore += KILLSCORE;
		return(GOTONE);
	};
	return(TURKEY);
}

/*
 * clr -- issues an escape sequence to clear the display
*/

clr()
{
	putchar(033);	putchar('E');
}

/*
 *	display initial instructions
 */

instruct()
{
	clr();
	POS(0, 0);
	printf("Attention: you are in a dungeon, being chased by monsters!\r\n\n");
	printf("There are gold coins scattered uniformly in the dungeon, marked by \"+\".\r\n");
	printf("One magic potion is available at each spot marked \"X\". Each potion will\r\n");
	printf("enable you to kill monsters by touch for a limited duration. It will also\r\n");
	printf("scare them away. When you kill a monster it is regenerated, but this takes\r\n");
	printf("time. You can also regenerate yourself %d times. Killing all the monsters\r\n", MAXPAC);
	printf("results in further treasure appearing magically somewhere in the dungeon,\r\n");
	printf("marked by \"$\". There is a magic tunnel connecting the center left and\r\n");
	printf("center right parts of the dungeon. The monsters know about it!\r\n\n");
	printf("        Keypad: 4       to move left\r\n");
	printf("                6       to move right\r\n");
	printf("                8 or w  to move up\r\n");
	printf("                2 or x  to move down\r\n");
	printf("                5       to halt \r\n");
	printf("                q       to quit\r\n\n");
	printf("                1       normal game\r\n");
	printf("                3       blinking monsters\r\n");
	printf("                7       intelligent monsters\r\n");
	printf("                9       blinking intelligent monsters\r\n");
}

/*
 * over -- game over processing
 */

over()
{
	int i;
	int line;
	int scorefile;


	poll(0);	/* flush and discard input from player */
	clr();

	/* high score to date processing */
	graphics(1);

	if (game != 0)
	{
		line = 7;
		POS(line++, 20);
		printf("faaaaaaaaaaaaaaaaaaaaaaaaaaac");
		POS(line++, 20);
		printf("`                           `");
		POS(line++, 20);
		printf("` G A M E   O V E R         `");
		POS(line++, 20);
		printf("`                           `");
		POS(line++, 20);
		printf("` \33GGame type: %1d\33F              `",game);

		POS(line++, 20);
		printf("`                           `");
		POS(line++, 20);
		printf("` \33GYour score: %-5u\33F         `", pscore);
		POS(line, 20);
		printf("eaaaaaaaaaaaaaaaaaaaaaaaaaaad");
	};
	graphics(0);
	leave();
}

/*
 * leave -- flush buffers,kill the Child, reset tty, and delete tempfile
 */

leave()
{
	POS(23, 0);
	exit(0);
}

/*
 * init -- does global initialization and spawns a child process to read
 *      the input terminal.
 */

init(argc, argv)
 char **argv;
{
	int  *p, i;
	unsigned tries;
	char *arg;

	tries = 0;			/* previous initializations */
	lastchar = DELETE;
	initbd(initbrd);
	initbd(brd);
	initbd(display);
	killcnt = 0;

	pacptr = &Pac;
	pacsymb = PACMAN;

	pacstart.xpos =	PSTARTX;	pacstart.ypos = PSTARTY;
	pacstart.dirn = DNULL;		pacstart.speed = SLOW;
	pacstart.danger = FALSE;
	pacstart.stat = 0;		/* ? */

/* Set up TTY stuff (deleted)			*/

	rawio();

	/*
	 * New game starts here
	 */

	game = 0;
	instruct();
	while (game == 0)
	{
		poll(1);
	};

	p = 0;			/* init random number generator	*/
	for (i=300; i--; tries += *p++);
	srand(tries);

	delay = (1000 * CLOCKRATE);

	goldcnt = GOLDCNT;
	pscore = 0;
	clr();

	for (i=1; i<argc; i++) switch(*(arg = argv[i]))
	 { case 'D':	delay = atoi(++arg); continue;
	 }

}

/*
 * poll -- read characters sent by input subprocess and set global flags
 */

poll(sltime)
{
	int stop;
	int charcnt;
	int junk;

	stop = 0;

	while (kbhit()) switch(combuf[0] = 0177 & getchar())
	{
	case LEFT:
		pacptr->dirn = DLEFT;
		break;

	case RIGHT:
		pacptr->dirn = DRIGHT;
		break;

	case NORTH:
	case NNORTH:
		pacptr->dirn = DUP;
		break;

	case DOWN:
	case NDOWN:
		pacptr->dirn = DDOWN;
		break;

	case HALT:
		pacptr->dirn = DNULL;
		break;

	case ABORT:
	case DELETE:
	case QUIT:
	case CONTROL_C:
		over();
		break;

	case CNTLS:
		stop = 1;
		continue;

	case GAME1:
		game = 1;
		break;

	case GAME2:
		game = 2;
		break;

	case GAME3:
		game = 3;
		break;

	case GAME4:
		game = 4;
		break;

	default:
		continue;
	}
}

int getrand(range)
{
	return rand() % range;
}


/* dfp - real function for tputs function to use */
putch( ch )
	int ch;
{
	putchar( ch );
}

/* saw -- a fake.				*/

sleep(secs)
 {	int i;
	while (secs-- > 0)
	  for (i=(1500 * CLOCKRATE); i--; );
 }
