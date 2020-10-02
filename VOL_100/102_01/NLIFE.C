/* Yet another LIFE simulator				9/14/80 Steve Ward
 * Uses H19 graphics, 25th line to give a 50x80 field.
 * Usage:
 *	nlife <filename>		default is "nlife.dat"
 *
 * Commands:
 *	CURSOR MOTION:	^F, ^N, ^P, ^H, TAB, RETURN, keypad arrows, 0.
 *	SET POINTS:	period, keypad 5 (XORs)
 *	DELETE:		DELETE key.
 *	SAVE:		A-Z save relative to cursor in one of 26 slots.
 *	GET:		a-z fetch (XOR) relative to cursor.
 *	MISC:		^X write out A-Z slots in file and exit.
 *			^C quit, no file output.
 *			ENTER (keypad): step one simulation step.
 *			LINEFEED: run until character typed.
 *			^L redisplay screen.
 *			^Z clear LIFE space.
 *			KEY 9:  Rotate about cursor
 *			KEY 7:  Flip about cursor
 *			KEY 1:  Random 5x5 configuration around cursor
 *			KEY 3:  Random 10x10 configuration around cursor
 *
 * N.B: Left/right, top/bottom edges of 50x80 field are adjacent.
 */

#define	XSIZE	80	/* Horizontal size			*/
#define	YSIZE	50	/* Vertical size			*/
#define	LIVE	0x10	/* Live cell flag.			*/

int  LStep();		/* Assy-lang update routine.		*/
int  Fixup();
int  Forall();

int	CurX, CurY;	/* Current cell coordinates.		*/
char	DpyX, DpyY;	/* Current (raw) display coordinates	*/

char *row[YSIZE];		/* ptrs to each row.		*/

int	Rand1, Rand2, Rand3;	/* Random number seeds		*/
char	RanFlag;		/* Flag: initialize randoms	*/

struct {
 char bits[(XSIZE*YSIZE)/8]; } Memory[27];

char space[XSIZE*YSIZE];	/* the universe matrix		*/

MoveTo(x,y)
 {	puts("\033Y"); putchar(y+32); putchar(x+32); }

Split()
 {	puts("\033z"); exit(); }

/* Update a point on the screen:  assumes graphics mode.
 */

Updpy(x, y)
 {	char key, dpyy;
	dpyy = y & 0xFE;
	MoveTo(x, dpyy>>1);
	key = (((LIVE & (row[dpyy | 1])[x]))? 2:0) |
	        (((LIVE & (row[dpyy])[x]))? 1 : 0);
	switch(key) {
		case 0:	ReVid(0); key = ' '; break;
		case 3: ReVid(1); key = ' '; break;
		case 1:	ReVid(0); key = 'p'; break;
		case 2:	ReVid(1); key = 'p'; break; }
	putchar(key); }

/* Change a cell contents:
 */

CCell(x, y)
 {	Updpy(x, y); }

/* Set/clear reverse video mode:
 */

ReVid(flag)
 {	puts((flag != 0)? "\033p" : "\033q"); }


/*
	Blink the cursor, waiting for input, and return whatever
	was typed:
*/

int Blink()
 {	char *it, save, i;	int delay;
	it = (row[CurY])+CurX;
	save = *it;
	for (;;i++)
	 { switch (i &= 3)
		{ case 0:	*it = 0; break;
		  case 2:	*it = LIVE; break;
		  case 1:
		  case 3:	*it = save; break; }
	   Updpy(CurX, CurY);
	   for (delay=200; delay--; Rand1++)
		if (kbhit()) {  i = getchar();
				*it = save; Updpy(CurX, CurY); RanFlag=1;
				return i; }}}

Move(dx, dy)
 {	if ((CurX += dx) >= XSIZE) CurX = 0;
	if (CurX < 0) CurX = XSIZE-1;
	if ((CurY += dy) >= YSIZE) CurY = 0;
	if (CurY < 0) CurY = YSIZE-1; }

/* Put a random n by n configuration at cursor:
 */

RandConf(n)
 {	char x,y,i,j;
	x = CurX-(n>>1);
	y = CurY-(n>>1);
	if (RanFlag) nrand(-1, Rand1, Rand2, Rand3); Rand3=Rand2; Rand2=Rand1;
	RanFlag=0;
	for (j=0; j<=n; j++)
	 for (i=0; i<=n; i++) if (!(nrand(1) & 0x30))
				{ (row[j+y])[i+x] ^= LIVE; UpDpy(i+x,j+y); }}

Edit()
 {	char ch, *cc;
	CurX = XSIZE>>1;
	CurY = YSIZE>>1;
	puts("\033F");
	for (;;)
	 {  switch(ch = Blink())
	     {	case '.':
		case '.':	(row[CurY])[CurX] = LIVE;
				Updpy(CurX++, CurY);
		case ' ':
		case '6':
		case 'F'-64:	Move(1,0); continue;
		case '4':
		case 'H'-64:	Move(-1,0); continue;
		case '7':	Flip(); continue;
		case '9':	Rotate(); continue;
		case 'X'-64:	return;
		case '5':	(row[CurY])[CurX] ^= LIVE; continue;
		case 0177:	Move(-1,0);
				(row[CurY])[CurX] = 0;
				Updpy(CurX, CurY); continue;
		case '\r':	CurX = 0; Move(0,1); continue;
		case '8':
		case 'P'-64:	Move(0,-1); continue;
		case '2':
		case 'N'-64:	Move(0,1); continue;
		case '\n':	while (!kbhit()) LStep(space); continue;
		case '\\':	while (1) {	ch = 100; RandConf(5);
						while (ch--)
						 { if (kbhit()) goto endit;
						   LStep(space); }}
				endit: continue;
		case 'Z'-64:	clear(); continue;
		case 'L'-64:	display(); continue;
		case 'C'-64:	Split();
		case '\t':	CurX = (CurX + 8) & 0xF8;
				Move(0,0); continue;
		case '/':	LStep(space); continue;
		case '0':	if (CurX|CurY) CurX = CurY = 0;
				else { CurX=(XSIZE>>1); CurY=(YSIZE>>1); }
				continue;
		case '1':	RandConf(5); continue;
		case '3':	RandConf(10); continue;
		default:	if ((ch >= 'a') && (ch <= 'z'))
				 { Unsave(&(Memory[ch-'a'])); continue; }
				if ((ch >= 'A') && (ch <= 'Z'))
				 { Save(&(Memory[ch-'A'])); continue; }
				putchar(7); continue;
	    }}
 }

main(argc,argv)
 char **argv;
 {	char *cc, x, y, *name;
	int FileSize, fileid;
	FileSize = (26*(XSIZE/8)*YSIZE + 127) >> 7; /* File size, in blocks */
	setmem(Memory, 26*(XSIZE/8)*YSIZE, 0);

	fileid = -1;
	name = "nlife.dat";
	if (argc > 1) name = argv[1];
	if ((fileid = open(name, 2)) == -1)
		{ puts("Creating file "); puts(name); puts("\r\n");
		  fileid = creat(name); }
	else { seek(fileid,0,0); read(fileid, Memory, FileSize); }

	cc = space;
	for (y=0; y<YSIZE; y++)
	 { row[y] = cc; cc += XSIZE; }

	puts("\033x1\033x5");
	clear();
	Edit();
	if (fileid != -1)
	 { seek(fileid,0,0); write(fileid, Memory, FileSize); close(fileid); }
	Split(); }

clear()
{	puts("\033H\033J\033Y8 \033K");
	setmem(space,(XSIZE*YSIZE),0);
	CurX = (XSIZE>>1); CurY = (YSIZE>>1); }


/* Display the current generation */

display()
{	char x, y, *cc;
	cc = space;
	puts("\033F\033Y8 \033K\033H\033J");
	for (y=0; y<YSIZE; y += 2)
	 { for (x=0; x<XSIZE; x++, cc++)
		if ((*cc | (cc[XSIZE])) & LIVE) Updpy(x,y);
	   cc += XSIZE; }}

Unsave(where)
 char *where;
 {	char *p, bit, x, y;
	bit = 0; where--; y = CurY;
	do {	p = row[y] + (x = CurX);
		do {	if (!(bit <<= 1)) { bit = 1; where++; }
			if (bit & *where) { *p ^= LIVE; Updpy(x,y); }
			p++;
			if (++x == XSIZE) { x = 0; p = row[y]; }
		   } while (x != CurX);
		if (++y == YSIZE) y = 0;
	   } while (y != CurY); }

Save(where)
 char *where;
 {	char *p, bit, x, y;
	bit = 0; where--; y = CurY;
	do {	p = row[y] + (x = CurX);
		do {	if (!(bit <<= 1)) { bit = 1; *++where = 0; }
			if (*p) *where |= bit;
			p++;
			if (++x == XSIZE) { x = 0; p = row[y]; }
		   } while (x != CurX);
		if (++y == YSIZE) y = 0;
	   } while (y != CurY); }

char *Cell();

Rot1(x,y)
 {	*Cell(space, CurX+CurY-y, CurY+x-CurX) |= (LIVE>>1); }
 
Rotate()
 {	Forall(space, &Rot1); Fixup(space); }

Flip1(x,y)
 {	*Cell(space, CurX+y-CurY, CurY+x-CurX) |= (LIVE>>1); }

Flip()
 {	Forall(space, &Flip1); Fixup(space); }

getchar()
{
	char c;	
	return bios(3);
}

putchar(c)
char c;
{
	return bios(4,c);
}

kbhit()
{
	return bios(2);
}
