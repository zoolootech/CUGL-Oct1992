#include "pacdefs.h"
#include "bdscio.h"


char kbhit()
 {	char c;
	if (inp(CSTAT) & CIMASK) switch(c = (inp(CDATA) & 0177))
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
 {	if (c & 0200) switch (c &= 0177)
	 { case 'O':	graphics(0); revid(1); putch(' '); return;
	   case '+':	graphics(1); revid(0); putch('^'); return;
	   case 'X':	graphics(1); revid(0); putch('~'); return;
	   case 'M':	graphics(1); revid(0); putch('i'); return;
	   default:	break; }

	graphics(0); revid(0);
	putch(c); }

putch(c)
 char c;
 {	do kbhit(); while (Freeze || !(inp(CSTAT) & COMASK));
	outp(CDATA, c); }

rawio()
 {
	InpChar = Freeze = 0;
	rev = graph = 0;
	putch(033); putch('x'); putch('5');
 }


/* Initialize a board:
 * Copies char array, setting 0200 ("graphics") bit.		*/

in1(bd, y, cc)
 char bd[BRDY][BRDX];
 char *cc, y;
 {	char x;
	for (x=0; *cc; ) bd[y][x++] = 0200 | *cc++;
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
	 { if (!rev) { rev=1; putch(033); putch('p'); }
	   return; }
	if (rev) { rev=0; putch(033); putch('q'); }}

/* graphics mode on/off:				*/

graphics(on)
 char on;
 {	if (on)
	 { if (!graph) { graph=1; putch(033); putch('F'); }
	   return; }
	if (graph) { graph=0; putch(033); putch('G'); }}



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
	delay -= (delay / 5);	/* hot it up */
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
printf("Attention: you are in a dungeon, being chased by %c monsters!\r\n",
	MONSTER);
printf("There are gold coins %c scattered uniformly in the dungeon.\r\n",
	GOLD);
printf("One magic potion is available at each spot marked %c. Each potion will\r\n",
	POTION);
printf("enable you to kill monsters (%c) by touch for a limited duration. It will also\r\n", RUNNER);
printf("scare them away. When you kill a monster it is regenerated, but this takes\r\n");
printf("time. You can also regenerate yourself %d times. Killing all the monsters\r\n", MAXPAC);
printf("results in further treasure appearing magically somewhere in the dungeon,\r\n");
printf("marked by %c. There is a magic tunnel connecting the center left and\r\n", TREASURE);
printf("center right parts of the dungeon. The monsters know about it! ");
printf("Monsters get\r\n");
printf("smarter, and the game gets faster, as your score gets higher.\r\n");
printf("Use keypad arrows to move around.  Type keypad 5 to start game.\r\n");	escore(0);
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
	if (game != 0)
	{
		graphics(0); revid(1); rev=0;
		line = 1;
		POS(line++, 20);
		printf("                             ");
		POS(line++, 20);
		printf("  G A M E   O V E R          ");
		POS(line++, 20);
		printf("                             ");
		POS(line++, 20);
		printf("  Your score: %-5u          ", pscore);
		POS(line, 20);
		printf("                             \033q\r\n");
	};
	escore(pscore);
	longjmp(jbuf,1);	/* start over --LZ */
/*
	leave();
*/
}

/*
 * leave -- flush buffers,kill the Child, reset tty, and delete tempfile
 */

leave()
{
	wscore();		/* Write out score file.		*/
	POS(23, 0);
	revid(0);	graphics(0);
	printf("\033G\033q\033y5");
	exit(0);
}

/*
 * init -- does global initialization and spawns a child process to read
 *      the input terminal.
 */

init(argc, argv)
 char **argv;
{
	unsigned tries;
	int *p, i;
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
	while ((game == 0) && (tries++ < (CLOCKRATE * 15000)))
	{
		poll(1);
	};

	if (tries >= (CLOCKRATE * 15000)) game = 1;   /* the default game.  */

	p = 0;			/* init random number generator	*/
	for (i=300; i--; tries += *p++);
	srand(tries);

	delay = (1000 * CLOCKRATE);

	goldcnt = GOLDCNT;
	pscore = 0;
	clr();
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
		game = 1;
		pacptr->dirn = DNULL;
		break;

	case ABORT:
	case DELETE:
	case QUIT:
		over();
		break;

	case CNTLS:
		stop = 1;
		continue;

	case '7':
	case '9':
	case '1':
	case '3':
		game = 1;
		break;

	case CONTROL_C:
		leave();

	default:
		continue;
	}
}

int getrand(range)
 int range;
{
	return rand() % range;
}


/* saw -- a fake.				*/

sleep(secs)
 {	int i;
	while (secs-- > 0)
	  for (i=(CLOCKRATE * 15000); i--; );
 }
