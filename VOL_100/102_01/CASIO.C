/* Casio watch game		Steve Ward 1/82
 *
 * The object of the game is to "hit" digits that roll across a display
 * by incrementing a master digit (using the '4' key) and 'shooting' (with
 * the '6' key) every time the master digit matches one or more of the digits
 * in the display. After a certain number of digits have been 'hit', another
 * round will begin in which the digits roll by faster. If the row of digits
 * manages to completely fill up the display, which happens when you aren't
 * shooting fast enough, the round will end and you probably lose points. 
 * Points are scored for every hit, but points are also decucted for 'missing'
 * (when you shoot with a digit that doesn't exist in the display; this makes
 * it impossible to cheat by just incrementing and shooting as fast as you can
 * which would otherwise be a pretty good strategy.)
 */

#include "bdscio.h"
#define	SIZE	6		/* Size of shift array			*/
#define	TICS	80		/* Time scale -- adjust to processor speed */
#define	CODES	"0123456789n"	/* digit codes				*/

int	Tics,			/* Soft version of time scale		*/
	Time,			/* Current "Time" in tics		*/
	Score,			/* Current score			*/
	Best,			/* Best score				*/
	NCodes,			/* length of CODES			*/
	Losses,
	Speed,
	ISpeed,			/* Starting speed			*/
	Size,
	KeyDelt,		/* Time per key, computer typein.	*/
	KeyTime,
	xx;

char	Display[80],		/* the current shooting gallery		*/
	*Who,			/* Current player.			*/
	WhoArray[100],
	Update,			/* flag: 1 => update file.		*/
	IPlay,			/* flag: Computer playing.		*/
	NoRec,			/* Don't record this game.		*/
	Select;			/* The selected code.			*/


/* Storage of best scores:						*/

struct ScRec {			/* Each record 64 bytes long.	*/
	int Sc;
	char Name[62];
	} *BRec[10], BStore[10];

#define	RECSIZ	64
#define	SCFILE	"CASIO.REC"	/* Name of score file.			*/

/* Show the top 10 games:					*/

Show()
 {	int i, j;
	struct ScRec *p;
	printf("\033H\033JCasio Watch Game\r\n\n\tTOP PLAYERS:\r\n\n");
	for (i=0, j=1; i<10; i++)
	 { p = BRec[i];
	   if ((p->Name[0]) && (p->Sc))
		printf("%2d.\t%d\t%s\r\n", j++, p->Sc, p->Name);
	 }
	printf("\r\n");
 }


/* Read the score file:
 */

RdRec()
 {	int i, j, k;
	struct ScRec *p;
	for (j=0; j<10; j++)
	 { p = BRec[j] = &(BStore[j]);
	   p->Sc = 0;
	   p->Name[0] = 0; }
	i = open(SCFILE, 0);
	if (i >= 0)
		{ read(i, BStore, 5);		/* 2 structs/128b record */
		  close(i); }
	Update = 0;
 }

/* Write the score file:						*/

WrRec()
 {	int fid, i, j;
	char buf[640], *cc, *dd;
	if (!Update) return;
	if ((fid = creat(SCFILE)) < 0) return;
	printf("Updating the record ... ");
	for (cc = buf, i=0; i<10; i++)
	 { dd = BRec[i];
	   for (j=RECSIZ; j--;) *cc++ = *dd++; }
	write(fid, buf, 5);
	close(fid);
	Update = 0;		/* file is now up to date.		*/
	printf("\r\n");
 }


/* Update the score file with the current score:			*/

Record()
 {	char buf[100], *cc, *dd;
	int i, j;
	struct ScRec *p, *q;

	for (i=0; i<10; i++)
	 { p = BRec[i];
	   if (Score > (p->Sc))
		{ q = BRec[9];
		  for (j=9; j>i; j--) BRec[j] = BRec[j-1];
		  BRec[i] = q;

		  if (IPlay)
			{ printf("Wow, I made the number %d score!\r\n",
				i+1);
			  return; }

		  printf("Congratulations, you have made the number %d score!",
				i+1);

		  if (NoRec) return;

ren:		  printf("\r\nWho are you, for the record? ");
		  for (cc=buf;;)
			{ if ((i=getchar()) == 0177) goto ren;
			  if ((i == '4') || (i == '6'))
				if (cc == buf) continue;
			  if ((i == '\r') || (i == '\n')) break;
			  if (cc == &(buf[61])) ;
			  else { putchar(i); *cc++ = i; }}
		  *cc = 0;
		  if (buf[0])
		   {   	for (cc=WhoArray, dd=buf; *cc++ = *dd++;);
			Who = WhoArray;
			cc = buf; }
		  else cc = Who;

mine:		  for (dd = (q->Name); *dd++ = *cc++;);
		  q->Sc = Score;
		  Update = 1;
		  printf("\r\n");
		  return;
	 }}
 }

int kbhit()
 {	return inp(CSTAT)&CIMASK; }

char getchar()
 {	while (!kbhit());
	return 0177 & inp(CDATA); }

putchar(c)
 char c;
 {	while (!(inp(CSTAT) & COMASK));
	outp(CDATA, c); }

Delay(when)			/* Wait for ...				*/
 {	int i;
	do	{ Time++;
		  for (i = Tics; i--;)
			if (kbhit() || (IPlay && (Time >= KeyTime))) return; }
	while (Time < when);
 }

RanDig()
 {	int i;
	i = nrand(1)>>5;
	i &= 0177;
	i %= NCodes;
	i++;
	return i;
 }

Round(speed, Left)
 {	int i, Alarm;
	char ca;
	Speed = speed;
	for (i=0; i<Size; i++) Display[i] = 0;
	Time = 0;
	if (IPlay) KeyTime = KeyDelt;
	Alarm = speed;
	printf("\r\n\t\t\tPartial score: %d   Digits left: %d   ",
		Score, Left);
	Dpy();

	do	{
		  Delay(Alarm);

		  if (IPlay)
			{
			  if (kbhit()) switch(getchar()) {
				case 'q':	case 'Q':
				case 'C'-64:	Losses = 0;
						return Left;
				default:	putchar(7);
				};

			  if (Time >= KeyTime)
				{ for (i=0; i<Size; i++)
					if (Select == Display[i])
						{ Shoot(); goto shot; }
				  if (++Select > NCodes) Select=1;
			shot:	  Dpy();
				  KeyTime += KeyDelt;
				}
			}

		  if (Time >= Alarm)
			{ Alarm += speed;
			  if (Display[Size-1])
				{ for (i=0; i<Size; i++)
					if (Display[i]) Left++;
				  return Left;	/* full -- he loses. */
				}
			  ca = 0;
			  for (i=(Size-1); i; i--)
				ca |= (Display[i] = Display[i-1]);
			  if (--Left >= 0) Display[0] = RanDig();
			  else
			   {	Display[0] = 0;	/* Space at end. */
				Left++;
				if (!ca) { Dpy(); return 0; }
			   }
			  Dpy(); }

		  else if (kbhit() && !IPlay) switch (0177 & getchar()) {
			case 'C':	case 'c':
					KeyTime = Time+KeyDelt;
					IPlay = 1;
					continue;
			case 'q':	case 'Q':
			case 'C'-64:	Losses=0; return Left;
			case ',':
			case '4':	if (++Select > NCodes) Select=1;
					puts("\33j\33Y. \33K\33k");
					Dpy(); continue;
			case '.':
			case '6':	Shoot(); continue;
			};
		} while (1);
 }

Game()
 {	int speed, i, Left;
	char *cc;

	speed = ISpeed;
	
	Score = 0;
	Select = 1;
	Losses = 3;

	Show();

	for (i=0; 1; i++)
	 { Left = 16;
	   printf("\r\nRound %d: score is %d.  Speed=%d   ",
		i+1, Score, speed);
	   while (Left = Round(speed, Left))
		{ if (--Losses <= 0) goto over;
		}
	   speed -= (speed >> 3); }

over:	printf("\r\nGame over.  Your score is %d\r\n", Score);

	Record();
 }

Shoot()
 {	int i, s;
	s = 10;
	for (i=(Size-1); i >= 0; i--, s += 10)
		if (Display[i] == Select) {		/* A hit.	*/
			  puts("\33j\33Y. \33K\33k");
			  while (i<(Size-1))
				 {
				   Display[i] = Display[i+1];
				   i++;
				 }
			  
			  Display[Size-1] = 0;
			  Score += s;
			  Dpy();
			  return; }
	puts("\33j\33Y. Missed!\33k");	 	/* Missed!	*/
	if ((Score -= 30) < 0) Score = 0;
 }

Ask(msg)
 {	int i;
agn:	printf("\r\n%s ", msg);
	while (!kbhit()) nrand(1);
	switch(i = getchar())
	 { case 'y':	printf("yes\r\n"); return 1;
	   case 'n':	printf("no\r\n"); return 0;
	   default:	printf(" y or n, please!"); goto agn; }
 }

Dpy()
 {	int i, j;
	char *cc;
	cc = CODES;
	cc--;
	printf("\r  ");
	for (i=0; i<Size; i++)
		{ if (j = Display[i]) putchar(cc[j]);
		  else putchar ('-'); }
	printf("   %c  (%d)  ", cc[Select], Losses);
 }

main(argc, argv)
 char **argv;
 {	int i, j;
	char *cc;
	Tics = TICS;		/* Initializations			*/
	for (NCodes=0, cc=CODES; cc[NCodes]; NCodes++);
	ISpeed = 140;		/* Starting speed.			*/
	Size = SIZE;
	KeyDelt = 60;
	Who = "The Mysterious CASIO Hustler";
	IPlay = 0;
	NoRec = 0;

	RdRec();

	for (i=1; i<argc; i++)
	 {	switch (*(cc = argv[i]))
			{
			  case 'S':	ISpeed = atoi(++cc); continue;
			  case 'Z':	Size = atoi(++cc); continue;
			  case 'K':	KeyDelt = atoi(++cc); continue;
			}
	 }
	do	{ Show();
		  printf("Instructions:\r\n");
		  printf("  4	increment 'gun' to next number\r\n");
		  printf("  6	shoot.\r\n");
		  printf("  q	quit\r\n");
		  printf("  c	Computer play\r\n");
		  if (!Ask("Another game? ")) break;
		  Game(); }
	while (1);
	WrRec();
	exit();
 }
