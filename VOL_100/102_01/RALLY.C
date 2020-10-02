/*
	H19 RALLY Game,		5/80	Steve Ward

	Works ONLY on Heathkit/Zenith H19/Z19 terminal
	 			(or H89 computer)

	Command format:

	A>rally [-rn] [-b] [mapname] [-dx] <cr>

	where:	"n"  is an optional seed for the random number generator
		     (results in exactly the same minor track deviations each
		     time it is given with a particular track);
		     If "n" is omitted, then track deviations are totally
		     random for each session (but same for each run in any
		     single session.)

		"-b" is a debugging option doing Steve-knows-what.

		"mapname" specifies the map file to use for the track
		    (defaults to "rally.map").

		"x"  is a delay count for use in putting in a delay after
		     every "insert line" sequence is sent to the H19. This
		     should only be used if your H19 hangs in the middle of
		     a game--which only happens if you're running the H19 at
		     19,200 baud (like I am.) (The delay defaults to DELAYV.)
*/


#include "bdscio.h"		/* Get std console parameters	*/

#define DELAYV  25		/* Should be 0 if not running at 19,200 baud */
#define	CARY	16		/* Y position of car.		*/
#define	IBAD	0
#define	LSPEED	7		/* Line 25 label posns		*/
#define	LMILES	20
#define	MAXSPD	9
#define	SPDSCL	128
#define	TENTHS	10		/* Number of lines per mile.	*/
#define	TICMIN	1920		/* Number of tics per minute.	*/

char	Free[10000], *Freep;

char	CRTFrz, CRTChr;
int	Miles;
char	Pavement, Freeze, BFlag;
char	CurX, CurY, SignY;
int	CarX, CarDX;
char	RevFlg, AltFlg;
int	Speed, Tenths;
char	Image[CARY*80+80];
char	*ImPtr, *ImEnd;
int	Seed;
int	Ranno;
char	InBuf[BUFSIZ], SavChr;
int	SpTime[MAXSPD+1];

int	ddelay;

struct Road {
	struct Node *Next;
	char	active;
	char	Token;
	int	Windy;
	int	Curvy;
	int	Age;
	int	ToGo;
	char	Holes;
	char	X;
	char	dx;
	char	width; } Road1, Road2;

struct Sign {
	struct Node *Next;
	char key;
	char text[0]; };

struct Fork {
	struct Node *Next;
	char key;
	char *Branch; };

struct Dist {
	struct Node *Next;
	char key;
	char wid, curve, wind;
	int miles; };

union Node {
	struct Dist;
	struct Fork;
	struct Sign; } *Tag[128];


/*
	Write a character to the terminal, handling X-ON/X-OFF
	protocol and not going into a busy loop if the terminal
	isn't ready to send a character, but rather just returning
	in that case to let the caller do more crunching and try
	again later.
*/

putchar(c)
 {	char stat, ch;
	for(;;)
	 { if ((CIMASK & (stat = inp(CSTAT))) == (CAHI ? CIMASK : 0))
	    switch(ch = (0177 & inp(CDATA))) {
		case 'S'-64:	CRTFrz=1; break;
		case 'Q'-64:	CRTFrz=0; break;
		case 'C'-64:	puts("\033z"); exit();
		default:	CRTChr=ch; }
	   if (CRTFrz) continue;
	   if ((stat & COMASK) == (CAHI ? COMASK : 0))
		 { if (c) outp(CDATA,c); return; }
	 }
}


char *new(size)
 {	char *rr;
	rr = Freep; Freep += size; return rr; }

struct Dist *NRoad(widx, curv, windx, dist)
 {	struct Dist *rr;
	rr = new(sizeof *rr);
	rr->key = 'D';		rr->Next = 0;
	rr->miles = dist;	rr->wid = widx;
	rr->curve = curv;	rr->wind = windx;
	return rr; }

struct Sign *NSign(txt)
 char *txt;
 {	int leng; char *cc, *dd;
	struct Sign *ss;
	leng = sizeof *ss; leng++;
	for (cc=txt; *cc++; leng++);
	ss = new(leng);		ss->key = 'S';		ss->Next = 0;
	dd = &(ss->text); for (cc=txt; *dd++ = *cc++;);
	return ss; }

struct Fork *NFork(kk)
 {	struct Fork *ff;
	ff = new(sizeof *ff); ff->key = kk;
	ff->Next = 0;	ff->Branch = 0;
	return ff; }

PrNode(nn)
 struct Node *nn;
 {	printf("Node %x: %c -> %x \r\n", nn, nn->key, nn->Next); }

char rdc()
 {	char ch;
	if (ch = SavChr) { SavChr=0; return ch; };
	return (getc(InBuf)); }

char pkc()
 {	return (SavChr = rdc()); }

int rdn()
 {	int ans, ch;
	ans = 0;
	while (isdigit(pkc()))	ans = ans*10 + (rdc() - '0');
	return ans; }

struct Dist *LRoad()
 {	int w, c, iwid, dd;
	char ch;
	dd = rdn();
	w = -1; c = -1; iwid = 20;
	while (pkc() != '\n') switch(rdc())
	 {	case '~':	c++; continue;
		case 'W':	iwid = rdn(); continue;
		case '!':	w++; continue;
		default:	continue; }
	return NRoad(iwid, c, w, dd);
 }

struct Dist *Load(name)
 char *name;
 {	char c