/* YAHTZEE Dice game			Steve Ward
 * USAGE:
 *
 *	yahtzee [-dn] name1 name2 name3 ...
 * where the namei are player's names, prefixed by = for automated
 * (computer) players.
 *
 *	yahtzee
 * for solitaire.
 *
 * USE special function keys to select dice, roll selected dice, and end
 *  an inning; keypad up/down arrows select where to enter your score.
 *
 * See YAHTZEE.HLP for full instructions.
 *
 * COMPILE and CLINK with CIO.CRL!
 */

#define	ROLLX	39		/* Coordinates for light buttons.	*/
#define	DONEX	46
#define	PASSX	53
#define	OOPSX	60
#define	OOPSC	'R'
#define	PASSC	'Q'
#define	DONEC	'P'
#define	ROLLC	'J'

#define	GOX	45		/* Coordinates for prompt.		*/
#define	GOY	22
#define	GAMEY	20

#define	SCOREX	30		/* leftmost column for scores		*/
#define	DIEY	22
#define	LABLIN	24		/* Line for labels			*/
#define	PLAYERS	10		/* Max number of players		*/
#define	NONE	127		/* char-size illegal value.		*/

char Dice[5];
char nplayers;			/* actual number of players.		*/
char Potent[13];		/* Potential score...			*/
char PotKind, PotSum, PotStr;	/* Filled by Pot			*/
char PotMax, PotCnt;

struct Player
 { char Scores[13];		/* Scores, or 127 iff none yet.		*/
   char Select;			/* Currently selected category.		*/
   char Col;			/* Column number, for scores.		*/
   char Name[40];		/* Name of player.			*/
   int  Total;			/* total score.				*/
   int  Games;
 } Who[PLAYERS];

/* Initialize a player:							*/

iplayer(name, pl)
 char *name;	struct Player *pl;
 {	char i, *fake;	fake = "==0==";
	if ((!name[1]) && (*name == '=')) { name = fake; name[3]++; }
	for (i=0; i<13; i++) pl->Scores[i] = NONE;
	pl->Select = NONE;
	for (i=0; pl->Name[i] = name[i]; i++); }

Buzz() { putchar(7); }
MoveTo(x, y) { puts("\033Y"); putchar(y+32); putchar(x+32); }

char ShowX, ShowY, *ShowSc; int ShowTot;

Show1(n, flag)
 char n, flag;
 {	int val;
	MoveTo(ShowX, ShowY++);
	if (n == 255) val = ShowTot;	else val = ShowSc[n];
	if (!flag) { puts("\033p");	val = Potent[n]; }
	else if (flag == 44) puts("\033p");
	if (val == NONE) puts("  ? ");
	else { printf(" %2d", val); ShowTot += val; }
	if (!flag || (flag == 44)) puts("\033q"); }

Show(pl)		/* Show the scores of a player.		*/
 char pl;
 {	struct Player *p;	char cc, tot, f, j;
	Pot(pl);
	ShowX = (p = &Who[pl])->Col-1;	ShowY = 1;	ShowTot = 0;
	ShowSc = p->Scores;	cc = 0;		f = p->Select;
	Show1(cc++,f--);	Show1(cc++,f--);	Show1(cc++,f--);
	Show1(cc++,f--);	Show1(cc++,f--);	Show1(cc++,f--);
	tot = ShowTot;	Show1(255,44);	ShowTot = j = tot>=63? 35:0;
	Show1(255,44);
	tot = ShowTot = tot+j;	Show1(255,44);	ShowTot = tot;
	ShowY++;
	Show1(cc++,f--);	Show1(cc++,f--);	Show1(cc++,f--);
	Show1(cc++,f--);	Show1(cc++,f--);	Show1(cc++,f--);
	Show1(cc++,f--);
	p->Total = ShowTot;
	Show1(255,44); }

char Straight(die)	/* Gives longest straight starting at die	*/
 {	char i, j;
	for (i=1; i<6; i++)
	 { die++;
	   for (j=0; j<5; j++) if (Dice[j] == die) goto hit;
	   break;
   hit:	   continue; }
	return i; }

char Kind(die)		/* Gives number of dice of count die		*/
 {	char count, i;
	for (i=count=0; i<5; i++) if (die == Dice[i]) count++;
	return count; }

Pot(pl)		/* Compute potential scores for player.		*/
 char pl;
 {	char i,j,k;
	for (i=0; i<13; i++) Potent[i] = 0;
	for (i=0; i<5; i++) if (((j = Dice[i])>0) && (j<7)) Potent[j-1] += j;
	for (PotMax=PotSum=i=PotKind=0; i<5; i++)
	 { PotKind |= (1<<(j=Kind(k=Dice[i]))); PotSum += Dice[i];
	   if (j>PotMax) { PotMax=j; PotCnt=k; }}
	Potent[12] = PotSum;
	if ((PotKind & 0xC) == 0xC) Potent[8] = 25;
	if (PotKind & 0x38) Potent[6] = PotSum;
	if (PotKind & 0x30) Potent[7] = PotSum;
	if (PotKind & 0x20) Potent[11] = 50;
	for (i=PotStr=0; i<5; i++) if ((j = Straight(Dice[i])) > PotStr)
		PotStr=j;
	if (PotStr >= 4) Potent[9] = 30;
	if (PotStr == 5) Potent[10] = 40; }


Play(pl)		/* Make a player's move; returns 0 iff OK. */
 char pl;
 {	Pot(pl);
	Who[pl].Scores[Who[pl].Select] = Potent[Who[pl].Select]; }

Bd(f)
 char f;
 {	char i, *cc;
	cc = " . ";
	if (f == 1) { puts("\033F"); cc = "iii"; }
	else if (!f) puts("\033q");
	for (i=((80-SCOREX)/3); i--;) puts(cc);
	puts("\033p\033G\n"); }

Board()
 {	char i;
	printf("\033H\033J\033p\033G");
	printf(" H89 YAHTZEE 1.1            \033F");
	for (i=48; i--;) putchar('i'); puts("\033G\n");
	printf("        ACES        ADD 1's "); Bd(0);	/* Y = 1 */
	printf("        TWOS        ADD 2's "); Bd(0);
	printf("        THREES      ADD 3's "); Bd(0);
	printf("        FOURS       ADD 4's "); Bd(0);
	printf("        FIVES       ADD 5's "); Bd(0);
	printf("        SIXES       ADD 6's "); Bd(0);
	printf("   Subtotal  .  .  .  .  .  "); Bd(2);		/* Y = 7 */
	printf("   Bonus iff >= 63       35 ");	Bd(2);		/* Y = 8 */
	printf("TOTAL ABOVE   .  .  .  .  . ");	Bd(2);		/* Y = 9 */
	printf("                            \n");
       	printf("        3 of a kind     SUM ");	Bd(0);	/* Y = 11 */
	printf("        4 of a kind     SUM ");	Bd(0);
	printf("        Full House       25 ");	Bd(0);
	printf("        4 Straight       30 ");	Bd(0);
	printf("        5 Straight       40 ");	Bd(0);
	printf("        YAHTZEE          50 ");	Bd(0);
	printf("        Chance          SUM ");	Bd(0);
	printf("TOTAL SCORE   .  .  .  .  . ");	Bd(2);
	MoveTo(0, GAMEY);
	printf("GAMES   .  .  .  .  .  .  . "); Bd(1);
	puts("\033p\033F");
	MoveTo(ROLLX, LABLIN);	puts("i ROLL i");
	MoveTo(DONEX, LABLIN);	puts("i DONE i");
	MoveTo(PASSX, LABLIN);	puts("i      i");
	MoveTo(OOPSX, LABLIN);	puts("i OOPS i");
	puts("\033q\033G");
 }

#define	TOP	0
#define	MID	8
#define	BOT	16
Die(number, count)
 {	char i, j;
	if (count) puts("\033p\033F");
	for (i=0; i<3; i++)
	 { MoveTo(4+(7*number), DIEY+i);
	   switch (count+(i<<3))
		{	case TOP: case MID: case BOT:
			case TOP+1: case BOT+1:
			case MID+2:
			case MID+4:	puts("     "); break;
			case TOP+2:
			case TOP+3:	puts("^    "); break;
			case MID+1:
			case MID+3:
			case MID+5:	puts("  ^  "); break;
			case BOT+2:
			case BOT+3:	puts("    ^"); break;
			case TOP+4:	case BOT+4:
			case TOP+5:	case BOT+5:
			case TOP+6:	case MID+6:	case BOT+6:
					puts("^   ^"); break; }}
	Dice[number] = count;
	puts("\033G\033q"); }


Roll()
 {	char i;
	for (i=0; i<5; i++) if (!Dice[i]) Die(i, 1+rand()%6); }int	DTime;

Go(rolls, player)
 char rolls, player;
 {	char ch, i, save[5];
	Pot(player);
	for (i=0; i<5; i++) Dice[i] = 0;	Roll();
	Who[player].Select = 12;
	if (!Cycle(player, 1)) return 0;

Top:	for (i=0; i<5; i++) save[i] = Dice[i];
nsc:	Show(player);
	for (;;)
	{ MoveTo(GOX, GOY);
	  printf("\033q\033G%s's move: %d rolls left. \033K",
		Who[player].Name, rolls);
esc:	  if (Who[player].Name[0] == '=') ch = Auto(player, rolls);
	  else ch = getchar();
	  switch (ch)
	  { case 033:	goto esc;
	    case '8':	Cycle(player, -1);	Show(player);	continue;
	    case ' ':
	    case '2':	Cycle(player, 1);	Show(player);	continue;
	    case 'S': case 'T': case 'U': case 'V': case 'W':
	 		if (!rolls) Buzz();
			else Die(ch - 'S', 0); continue;
	    case 0177:
	    case OOPSC:	for (i=0; i<5; i++) Die(i, save[i]); continue;
	    case ROLLC:	if (rolls>0) { Roll(); rolls--; }
			else Buzz(); goto Top;
	    case DONEC:
	    case '\r':
	    case PASSC:	Play(player);
			Who[player].Select = NONE;	Show(player);
	    		return ch;
	    default:	putchar(7); continue; }}}

/* Make move for a computer player:				*/

int AutoPar[10];	/* Heuristic parameters -HA#, -HB#, etc.	*/

char Auto(pl, rolls)
 char pl, rolls;
 {	char gofor, choice, i, j, *sc;
	int value[13], target[6], best, n;
	sc = Who[pl].Scores;
	Pot(pl);
	MoveTo(GOX, GOY+1); printf("Hmmm ... lemme think.\033K",
		value[choice]);
	Delay(DTime);

	for (i=0; i<13; i++)
	 { if (sc[i] != NONE) value[i] = i-99;
	   else	if (i<6) value[i] = 2*(Potent[i]-3*(i+1));/* face counts */
	   else if (i==6) value[i] = Potent[i]-20;	/* 3 of a kind */
	   else if (i==7) value[i] = Potent[i]-15;	/* 4 of a kind */
	   else if (i==9) value[i] = Potent[i]-12;	/* 4 straight  */
	   else if (i==10) value[i] = Potent[i]-10;	/* 5 straight  */
	   else if (i==12) value[i] = Potent[i]-21-AutoPar[5];	/* CHANCE */
	   else value[i] = Potent[i]-10; }

	for (i=0; i<6; i++)
	 { if (sc[12] == NONE) target[i] = i+i; else target[i] = 0;
	   if (sc[i] == NONE) target[i] += i<<2; else target[i] -= 15;
	   if ((sc[8] == NONE) && (Kind(i+1) == 3)) target[i] += 6;
	   if (sc[11] == NONE) target[i] += 1<<(Kind(i+1));
	   target[i] += Kind(i+1)<<3; }

	best = -99; gofor = PotCnt; choice = Who[pl].Select;

	for (i=0; i<13; i++)
	 if (value[i] > best) { best = value[i]; choice = i; }
	Who[pl].Select = choice;
	Show(pl);

	if (rolls &&		/* Convert a straight??		*/
	    (Potent[9] > 0) &&
	    (Potent[10] == 0) &&
	    (sc[10] == NONE))
	 { for (i=0; i<5; i++)	/* Find the useless die.	*/
		{ j = Dice[i];	Dice[i] = 0;	Pot(pl);	Dice[i] = j;
		  if (Potent[9] > 0) { Die(i, 0); goto rollit; }}}
	Pot(pl);
	if ((choice > 7) && (choice < 12)) goto hit; /* End the inning.	*/

	if (rolls)
	 { for (i=0; i<6; i++) if ((n=(target[i]-27-AutoPar[0])) > best)
				{ gofor = i; choice = 255; best=n; }}

	if ((choice == 255) || (choice < 8)) goto maxim;/* See if we can help.	*/

flush:	if (!rolls) goto hit;		/* Least of evils...	*/
	for (i=0; i<5; i++) Die(i,0);	/* Re-roll entire hand.	*/
	goto rollit;

maxim:	if (!rolls) goto hit;		/* Maximize number of chosen dice. */
	for (i=0; i<5; i++)
	 if (Dice[i] && (Dice[i] != (gofor+1))) Die(i, 0);
rollit:	MoveTo(GOX, GOY+1); printf("Baby needs shoes ... %d/%d\033K",
		choice, best);
	Delay(DTime>>1);
	return ROLLC;

hit:	MoveTo(GOX, GOY+1); printf("I've decided ... %d/%d\033K",
		choice, best);
	Delay(DTime);
	return PASSC;
 }

Delay(n)
 {	while (n--) if (kbhit()) { getchar(); break; }}

int Cycle(pl, delta)		/* Move a player's Select ... 0 iff done. */
 char pl; int delta;
 {	char *sel, *sc, done;	int ss;
	ss = *(sel = &(Who[pl].Select));	sc = &(Who[pl].Scores);
	done = 14;
	do	{ if ((ss += delta) < 0) ss = 12;
		  if (ss >= 13) ss = 0; }
	while (--done && (sc[ss] != NONE));
	*sel = ss;
	return done; }

Center(text, centx, y, xsize)
 char *text, centx, y, xsize;
 {	char i;
	for (i=0; text[i]; i++);
	if (i > xsize) i = xsize;
	MoveTo(centx - (i>>1), y);
	putchar(' ');
	while (*text && i--) putchar(*text++);
	putchar(' '); }

main(argc, argv)
 char **argv;
 {	char i,j,arg,*carg;	int n, *m;
	for (i=0; i<PLAYERS; i++) Who[i].Games = 0;
	for (i=m=0; i<200; i++) n += *m++;
	srand(n);
	for (i=0; i<10; i++) AutoPar[i] = 0;	DTime = 5000;
/*
	TTYMode(30);			/* ^C, FLOW, STRIP, EXPAND */
*/
	puts("\033x1\033q\033G\033y7\033x5");

	for (arg=1; (arg<argc) && (*(carg = argv[arg]) == '-'); arg++)
	 switch (*++carg)
	  { case 'R':	srand(atoi(++carg)); continue;
	    case 'H':	i = (*++carg)-'A';
			AutoPar[i] = atoi(++carg); continue;
	    case 'D':	DTime = atoi(++carg)|1; continue;
	    default:	continue;
	  }
Again:	nplayers = 0;
	for (i=arg; i<argc; i++) iplayer(argv[i], &Who[nplayers++]);
	if (!nplayers) iplayer("You", &Who[nplayers++]);
	n = (80-SCOREX)/nplayers;
	for (i=SCOREX+(n>>1), j=0; j<nplayers; j++, i += n) Who[j].Col = i;
	Board();
	for (j=0; j<nplayers; j++) Center(Who[j].Name, Who[j].Col, 0, n-1);
	for (i=0; i<nplayers; i++) Show(i);
	for (;;) for (i=0; i<nplayers; i++)
		if (!Go(2, i))
			{ for (i=n=0; i<nplayers; i++)
				if (Who[i].Total > n) n = Who[i].Total;
			  for (i=0; i<nplayers; i++)
				if (Who[i].Total >= n)
				 { MoveTo(Who[i].Col, GAMEY);
				   printf("\033p%3d", ++(Who[i].Games)); }
			  MoveTo(GOX, GOY+1);
			  for (i=0; i<nplayers; i++)
				if (Who[i].Total >= n)
				   printf("%s ", Who[i].Name);
			  printf("\007WINS! \033q Play again? \033K");
			  if (getchar() == 'y') goto Again;
			  puts("\033Y6 \033J\033y5\033y1\033q\033G");
			  exit(); }
 }
