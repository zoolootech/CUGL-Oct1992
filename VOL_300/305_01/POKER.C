/*
TITLE:		POKER.C;
DATE:		5/15/88;
DESCRIPTION:	"Five-card draw poker.";
VERSION:	1.01;
KEYWORDS:	poker, cards;
FILENAME:	POKER.C;
WARNINGS:	"Requires IBM or compatible.";
SEE-ALSO:	CARDS.DOC, BLACKJACK.C;
SYSTEM:		MS-DOS;
COMPILERS:	Aztec;
AUTHORS:	Dan Schechter;
 */

/*
This program is copyright 1988, 1989 by Dan Schechter.
You may copy and distribute it only on a not-for-profit basis.
I provide this source code mainly for informational purposes but 
you may modify it providing that you give attribution to the original author
and document your modifications and distribute the modified version
only on a not-for-profit basis. Since it writes directly to screen
memory you must compile using large model. (I know how to get around
that now, but I am no longer updating this program.)
 */

/* BUGS: -->
		Problem in betq():
		if only two players remain in at start of final bets,
		and if the first folds (an unnatural, but legal thing to do),
		the second will bet before the program declares that only
		one player remains in.
		This does not materially affect the game.
 */

#include <stdio.h>
#include <color.h>
#include <time.h>

char *version = "\
POKER version 1.01 5/15/88 from KITTENSOFT.\n\
Copyright (c) 1987, 1988 by Dan Schechter\n\n";

int main(void);
int betq(int q);
int al_st_fl(int *dis,int lhand[][2],int special[][2],int *suit);
void kaydscrd(int who);
void pladscrd(void);
void evaluate(int who);
void setvnull(char lhand[][2],int values[]);
void setvpair(char lhand[][2],int values[]);
void setvtwo(char lhand[][2],int values[]);
void setvthre(char lhand[][2],int values[]);
void setvstra(char lhand[][2],int values[]);
void setvfull(char lhand[][2],int values[]);
void setvfour(char lhand[][2],int values[]);
int makebet(int highbet,int mytotal,int v[],int q,int who);
int getbet(int q,int *line,int pot,int bet);
int al_flushq(int who);
int al_stq(int who);
void prompt(char *s1,char *s2,char *s3);
void setcolor(int c);
void shuffle(void);
void deldeck(int n);
int rando(void);
int comp(char *a,char *b);
int compf(char *a,char *b);
int valcmp(char *aa,char *bb);
int myatoi(char *s);
char *itoa(int n,char *s);
int getreply(char *s);
void selectcolor(void);
int get2reply(char *s,int);
int tpr(int c);
int final_display(void);
int rotate(void);
void snaphand(int index);
void writecard(int v,int h,int card);
void colorchar(int v,int h,int suit,int c);
void scr_color(int v,int h,int c,int q);
void help(int q);
void sethelp(void);
void instructions(void);
void intro(char *str);
int mygetch(void);
int inkey(void);
void sendcurs(int v,int h);
void showtime(void);

#define COL2 45
#define COL1 5
#define CARDLINE(x) (1+6*x)
#define BETLINE 1

#define CLEARSCREEN scr_clear()
#define ERAEOL scr_eol()
#define sendcurs scr_curs

char *txt[53]={
	"2S",	/* 0 */
	"2H",	/* 1 */
	"2C",	/* 2 */
	"2D",	/* 3 */
	"3S",	/* 4 */
	"3H",	/* 5 */
	"3C",	/* 6 */
	"3D",	/* 7 */
	"4S",	/* 8 */
	"4H",	/* 9 */
	"4C",	/* 10 */
	"4D",	/* 11 */
	"5S",	/* 12 */
	"5H",	/* 13 */
	"5C",	/* 14 */
	"5D",	/* 15 */
	"6S",	/* 16 */
	"6H",	/* 17 */
	"6C",	/* 18 */
	"6D",	/* 19 */
	"7S",	/* 20 */
	"7H",	/* 21 */
	"7C",	/* 22 */
	"7D",	/* 23 */
	"8S",	/* 24 */
	"8H",	/* 25 */
	"8C",	/* 26 */
	"8D",	/* 27 */
	"9S",	/* 28 */
	"9H",	/* 29 */
	"9C",	/* 30 */
	"9D",	/* 31 */
	"TS",	/* 32 */
	"TH",	/* 33 */
	"TC",	/* 34 */
	"TD",	/* 35 */
	"JS",	/* 36 */
	"JH",	/* 37 */
	"JC",	/* 38 */
	"JD",	/* 39 */
	"QS",	/* 40 */
	"QH",	/* 41 */
	"QC",	/* 42 */
	"QD",	/* 43 */
	"KS",	/* 44 */
	"KH",	/* 45 */
	"KC",	/* 46 */
	"KD",	/* 47 */
	"AS",	/* 48 */
	"AH",	/* 49 */
	"AC",	/* 50 */
	"AD",	/* 51 */
	"  "
};
char *money;
char *computer = "Tchekalinsky" ;
char *_names_[13] = {
	" ",
	"Goneril",
	"Kent",
	"Dmitry",
	"Hermann",
	"Cisco",
	"Grushenka",
	"Nastasya",
	"Lady MacBeth",
	"Billy Budd",
	"Clytemnestra",
	"Gaspar",
	"Freya"
};
char *intro_strings[13] = {
	" ",
	"Princess Goneril.",
	"The Earl of Kent.",
	"Dmitry Fyodorovich Karamazov.",
	"My old chum, Hermann the German.",
	"Cisco, the singing hobo.",
	"The beautiful Grushenka.",
	"The madwoman, Nastasya Filipovna Barashkov.",
	"Lady MacBeth.",
	"Billy Budd, sailor.",
	"Clytemnestra, queen of Argos.",
	"Gaspar Ruiz.",
	"Freya, of the Seven Isles."
};
char *currency[8] = {
	"lira",
	"pesos",
	"rubles",
	"drachmas",
	"rupees",
	"yen",
	"c\242rdobas",
	"toothpicks"
};
char *shmaddr;
int color;
int revid_atr=116;
int norm_atr=112;
int helpcolor;			/* color for help screens */ 

struct _hand_ {
	int cards[5];
	int values[7];
	int discards[6];
	int balance;		/* plus or minus for amount of winnings or losings */
	int bluff;		/* bluff factor for computer hands */
	int in;			/* true if hand has not folded */
	char *name;
	char *description;
	int color;		/* color for screen messages */
};
int pot=0;
int deck[52];
int player;			/* hand[player] is player's hand */
extern int _attrib;
struct _hand_ hand[5];
unsigned char sidescreen[4000];

char cat[45] = {
	0x20, 0x20, 0x20, 0x20, 0x2f, 0x5c, 0x5f, 0x2f, 0x5c, '\n',
	0x20, 0x20, 0x20, 0x28, 0xfe, 0x20, 0xd2, 0x20, 0xfe, 0x29, '\n',
	0xf0, 0xf0, 0xf0, 0xf0, 0x20, 0xc4, 0xca, 0xc4, 0x20, 0xf0, 
	      0xf0, 0xf0, 0xf0, '\n',
	0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x22, 0x22, '\n', 0x00
};

int main()
{
	int r,i,j,dis[8],line,dcount;
	char s[25],paws=0;
	
	for (dis[0]=r=0,i=1;i<5;i++,r=0){
		dis[i]=rando()%12+1;
		for (j=i-1;j>0;j--) if (dis[j]==dis[i]) r=1;
		if (r) i--;
	}
	for (i=0;i<5;i++) {
		hand[i].name=_names_[dis[i]];
		hand[i].description=intro_strings[dis[i]];
		hand[i].balance=0;
	}
	helpcolor =    2;
	hand[0].color= 15; 	/* display colors for color monitors. */
	hand[1].color= 10;
	hand[2].color= 11;
	hand[3].color= 12;
	hand[4].color= 14;
	intro(s);
	if (color=='c') setcolor(helpcolor);
	else {
		norm_atr=helpcolor=7;
		revid_atr=112;
	}
	myprintf("\
Good %s friend. My name is %s. Welcome to Petersburg.\n\
Allow me to introduce your table-mates:\n\n",s,computer);
	_attrib |= 8;
	for (i=1;i<5;i++)
		myprintf("\t\t  %s\n",hand[i].description);
	_attrib ^= 8;
	player=0;
	myprintf("\n\
The game is five-card draw poker.\n\
There is no minimum to open, and there are no wild cards.\n\n\
Would you like to play for:\n\n\
\t1..... Italian lira.    \t5..... Indian rupees.\n\
\t2..... Argentine pesos. \t6..... Japanese yen.\n\
\t3..... Russian rubles.  \t7..... Nicaraguan c\242rdobas.\n\
\t4..... Greek drachmas.  \t8..... Toothpicks.\n\n\
\t\t? ");

	r=get2reply("12345678",1);
	money=currency[r-49];
	myputchar('\n');
	goto shuf;
st:	CLEARSCREEN;
shuf:	shuffle();
	dcount=0;
	CLEARSCREEN;
	for (i=0;i<5;i++)
		for (j=0;j<5;j++)		/* deal cards */
			hand[j].cards[i]=deck[dcount++];
fdis:	for (i=0;i<5;i++) {
		qsort(hand[i].cards,5,sizeof(int),comp);     /* sort hands */
		evaluate(i);
	}
	for (i=0;i<5;i++) {
		hand[i].in=1;			/* new hand: everyone is in. */
		hand[i].balance-=10;		/* ante */
		hand[i].bluff=rando()%100;	/* bluff factor
					       meaningless for player's hand */
		for (j=0;j<6;j++) hand[i].discards[j]=0;
	}
	pot+=50;
	
	line=CARDLINE(2);
	sendcurs(line,COL1+10);   
	r=betq(1);	/* opening bets. 
				On return, all opening bets have been
				made. pot and balances have been adjusted.
				some players may have folded */
	setcolor(hand[player].color);
	myprintf("\nPress space bar... ");
	prompt("End of first round betting.","Press space bar.","");
	paws=1;
	get2reply(" ",2); 
	myputchar('\n');
	if (r==0) goto st;	/* no openers */
	if (r==1) goto drop;	/* all but one folded */
	
	for (i=0;i<5;i++) {
		if ((i==player)&&(hand[player].in)) {
			if (paws==0){
				setcolor(hand[player].color);
				myprintf("\nPress space bar... ");
				prompt("Press space bar to continue.","","");
				get2reply(" ",3);
				myputchar('\n');
				paws=1;
			}
			for (j=0;j<4000;j++) sidescreen[j]=shmaddr[j];
			scr_loc(&r,&j);
			pladscrd();
			scr_curs(r,j);
			for (j=0;j<4000;j++) shmaddr[j]=sidescreen[j];
			
		}
		else if (hand[i].in) {
			kaydscrd(i);	/* computer discards */  
			setcolor(hand[i].color);
			myprintf("\n\t-*> %s ",hand[i].name);
			switch (j=hand[i].discards[0]) {
				case 0: myprintf("stands pat."); break;
				case 1: myprintf("discards 1 card."); break;
				default: myprintf("discards %d cards.",j);
			}
			ERAEOL;
			paws=0;
		}
	}
	if (paws==0){
		setcolor(hand[player].color);
		myprintf("\nPress space bar... ");
		prompt("Press space bar to continue.","","");
		ERAEOL;
		if (hand[player].in) 
		    for (i=0;i<hand[player].discards[0];i++)
			writecard(2,COL1+(4-hand[player].discards[i+1])*6,52);
		get2reply(" ",4);
		myputchar('\n');
	}
	for (i=0;i<5;i++){		/* fill out hands after discarding */
		if (hand[i].in==0) continue;
		for (j=1;j <= hand[i].discards[0];j++)
			hand[i].cards[ hand[i].discards[j] ] = deck[dcount++];
	}
	for (i=0;i<5;i++){
		qsort(hand[i].cards,5,sizeof(int),comp);
		for(j=0;j<7;j++) hand[i].values[j]=0;
		if (hand[i].in) evaluate(i);
	}
	r=betq(2);		/* Final bets. */
	setcolor(hand[player].color);
	myprintf("\nPress space bar... ");
	prompt("Press space bar to continue.","","");
	get2reply(" ",5);

drop:	r=final_display();
	if (r=='n') exit(0);
	goto st;
}
/* ------------------------------------ BETQ */
int betq(q)
int q;
{
	int qq,r,i,j,line=7,whosebet,bet[6];
	int call;	/* Consecutive calls. When call==4 betting is done. */
	static numberin,opener;
	
	CLEARSCREEN;
	for (i=0;i<6;i++) bet[i]=0;	/* bet[5] is largest amount bet 
					bet[n] is how much the n-th player
					has put in so far. */
	sendcurs(line++,0);
	setcolor(hand[player].color);
	if (q==1) {
		myprintf("Opening bets: Ante is 10 %s.\n",money);
		opener=0;
		numberin=5;
	}
	else myprintf("Final bets:\n");
	
	whosebet=opener;
	for(call=(-1);;whosebet++){
	    whosebet %= 5;
	    if (q==1) {
		if (bet[5]==0) qq='o';
		else qq='t';
	    }
	    if (q==2) {
		if (bet[5]==0) qq='y';
		else qq= 'e';
	    }
	    if (hand[whosebet].in == 0) continue;
	    
	    if (whosebet!=player) {
		r=makebet(bet[5],bet[whosebet],hand[whosebet].values,qq,whosebet);
		setcolor(hand[whosebet].color);
		myprintf("\n\t--> %s ",hand[whosebet].name);
		switch(r){
			case ('p'|1024): myprintf("passes."); break;
			case ('f'|1024): myprintf("folds."); break;
			case ('c'|1024): myprintf("calls."); break;
			default: 
				if (bet[5]==0) {
					if (q==1) myprintf("opens with %d %s.",r,money);
					else myprintf("bets %d %s.",r,money);
				}
				else myprintf("raises %d %s.",r,money);
		}
		line++;
	    }
	    else r=getbet(qq,&line,pot,bet[5]-bet[player]);
	    
	    switch (r) {
		case ('p'|1024): 
			call++; 
			break;
		case ('f'|1024): 
			numberin--;
			hand[whosebet].in=0;
			break;
		case ('c'|1024): 
			call++;
			j= bet[5]-bet[whosebet];
			hand[whosebet].balance -= j;
			pot += j;
			bet[whosebet]=bet[5];
			break;
		default:
			call=0;
			if (bet[5]==0) opener=whosebet;
			j= bet[5]-bet[whosebet]+r;
			hand[whosebet].balance -= j;
			pot+=j;
			bet[whosebet] += j;
			bet[5]=bet[whosebet];
			break;
	    }
	    if ((bet[5]==0)&&((call+1)==numberin)) return(0);
	    if ((bet[5])&&(numberin==1)) return(1);
	    if ((call+1)==numberin) return(numberin);
	}
}
/* ------------------------------------------- AL_ST_FLUSH */
int al_st_fl(dis,lhand,special,suit)
int *dis,lhand[][2],special[][2],*suit;
{
	int *wp,*wpp,cnt,i,j;

	/* WARNING --> THIS ROUTINE WILL <SPLIT THE PAIR> */
	qsort(special[0],5,2*sizeof(int),compf); 

	*suit=0;
	i=special[0][1];
	j=special[4][1];
	if ((i==special[1][1])&&(i==special[2][1])&&(i==special[3][1]))
		*suit=i;
	if ((j==special[1][1])&&(j==special[2][1])&&(j==special[3][1]))
		*suit=j;

	if (*suit==0) return(0);
	
/* if it gets this far, there is an almost-flush in suit */
	if (i!=*suit) wp=wpp=special[1];
	else wp=wpp=special[0];
/* wp and wpp now point to a 4-card flush partial hand */
/* ----> NOW LOOK FOR AN ALMOST-STRAIGHT. NEED NOT BE CONTIGUOUS. */
	qsort(wp,4,2*sizeof(int),comp);
	for (wpp+=2,cnt=j=0;j<3;j++,wp+=2,wpp+=2){
		i=(*wp-*wpp);
		if (((i==0)||(i>2)) || 
			((i==2)&&(cnt))) { cnt=2; break; } 
		if (i==2) cnt=1;
	}
	if (cnt==2){
		if (special[0][1]!=*suit) wp=wpp=special[1];
		else wp=wpp=special[0];
		if (*(wp+8)==14) {
			*(wp+8)=1;
			qsort(wp,4,2*sizeof(int),comp);
			for (wpp+=2,cnt=j=0;j<3;j++,wp+=2,wpp+=2){
				i=(*wp-*wpp);
				if (((i==0)||(i>2)) || 
					((i==2)&&(cnt))) return(0); 
				if (i==2) cnt=1;
			}
		}
	}
/* if it gets this far there is a 4-flush/almost-straight */
	for (i=0;i<5;i++){
		if (lhand[i][1]!=*suit){
			dis[0]=1;
			dis[1]=i;
			return(1);
		}
	}
	myprintf(" ERROR in al_st_fl() ");
	return(-1);
}

/* -------------------------------------- KAYDSCRD */
void kaydscrd(who)
int who;
{
	int i,j,cts;
	int lhand[5][2],special[5][2],suit;
	
	for (i=0;i<5;i++) {
		lhand[i][0]=special[i][0]=txt[hand[who].cards[i]][0];
		lhand[i][1]=special[i][1]=txt[hand[who].cards[i]][1];
	}
	 
	/* BLUFF */
	if (hand[who].bluff==0){
		hand[who].discards[0]=0;
		return;
	}
	
	/* NO DRAW ON STRAIGHTS, FLUSHES, FULL HOUSES, OR ST-FLUSHES */
	if ((hand[who].values[0]==8)||(hand[who].values[0]==16)||(hand[who].values[0]==32)||(hand[who].values[0]==128)){
		hand[who].discards[0]=0;
		return;
	}
	/* FOUR OF A KIND */
	/* Always draw 1. Meaningless, but conceals value from opponents. */
	if (hand[who].values[0]==64) {
		hand[who].discards[0]=1;
		if (lhand[0][0]==lhand[1][0])
			hand[who].discards[1]=4;
		else 
			hand[who].discards[1]=0;
		return;
	}
	/* THREE OF A KIND */
	if (hand[who].values[0]==4){
		hand[who].discards[0]=2;               /* DRAW TWO CARDS */
		if ((lhand[1][0]==lhand[2][0])&&(lhand[1][0]==lhand[3][0])){
			hand[who].discards[1]=0;
			hand[who].discards[2]=4;
			return;
		}
		if (lhand[1][0]==lhand[0][0]){
			hand[who].discards[1]=3;
			hand[who].discards[2]=4;
		}
		else {
			hand[who].discards[1]=0;
			hand[who].discards[2]=1;
		}
		return;
	}
	/* TWO PAIR */
	if (hand[who].values[0]==2){
		hand[who].discards[0]=1;      /* DRAW ONE FOR THE ODD CARD */
		if (lhand[0][0]!=lhand[1][0]){
			hand[who].discards[1]=0;
			return;
		}
		if (lhand[2][0]!=lhand[3][0]) hand[who].discards[1]=2;
		else hand[who].discards[1]=4;
		return;
	}
	/* CHECK FOR ALMOST STRAIGHT FLUSH */
	/* WARNING --> THIS ROUTINE WILL <SPLIT THE PAIR> */
	if (al_st_fl(hand[who].discards,lhand,special,&suit)) return;

	/* PAIR */
	if (hand[who].values[0]==1){
		hand[who].discards[0]=3;         /* DRAW THREE ON A PAIR */
		for (i=0;lhand[i][0]!=lhand[i+1][0];i++);
		if (i==0){
			hand[who].discards[1]=2;
			hand[who].discards[2]=3;
			hand[who].discards[3]=4;
			return;
		}
		if (i==1){
			hand[who].discards[1]=0;
			hand[who].discards[2]=3;
			hand[who].discards[3]=4;
			return;
		}
		if (i==2){
			hand[who].discards[1]=0;
			hand[who].discards[2]=1;
			hand[who].discards[3]=4;
			return;
		}
		hand[who].discards[1]=0;
		hand[who].discards[2]=1;
		hand[who].discards[3]=2;
		return;
	}
	/* NULL HAND - THE ONLY REMAINING POSSIBILITY */
	/* CHECK FOR ALMOST-FLUSH */
	if (suit){
		hand[who].discards[0]=1;
		for (i=0;i<5;i++)
			if (lhand[i][1]!=suit) hand[who].discards[1]=i;
		return;
	}
	/* CHECK FOR ALMOST STRAIGHT */
	hand[who].discards[1]='x';
	if ((lhand[0][0]+1==lhand[1][0])&&(lhand[1][0]+1==lhand[2][0])&&
(lhand[2][0]+1==lhand[3][0])) hand[who].discards[1]=4;
	if ((lhand[1][0]+1==lhand[2][0])&&(lhand[2][0]+1==lhand[3][0])&&
(lhand[3][0]+1==lhand[4][0])) hand[who].discards[1]=0;
	if ((lhand[0][0]==2)&&(lhand[1][0]==3)&&(lhand[2][0]==4)&&
(lhand[4][0]==14)) hand[who].discards[1]=3;
	if (hand[who].discards[1]!='x'){
		hand[who].discards[0]=1;
		return;
	}
	/* BLUFF */
	hand[who].discards[1]=0;
	hand[who].discards[2]=1;
	hand[who].discards[3]=2;
	hand[who].discards[4]=3;

	switch (hand[who].bluff){
		case 0: hand[who].discards[0]=0; return;
		case 1: hand[who].discards[0]=1; return;
		case 2: hand[who].discards[0]=2; return;
		case 3: hand[who].discards[0]=3; return;
		default: hand[who].discards[0]=4; return;
	}
}

/* ----------------------------- PLADSCRD */

void pladscrd()
{
	int line,r,i,j;
	
	if (hand[player].in==0) return;
	
	CLEARSCREEN;
	setcolor(hand[player].color);
	prompt("Discard.","Press ENTER when done.","");
	line=7;
	sendcurs(line,7);
	myprintf("1     2     3     4     5\n\n\n\
Discard. List cards by number. Press DEL to start over. "); 
	line++;
	do {
		for (i=0;i<6;i++) hand[player].discards[i]=0;
		for (i=0;;){
			r=get2reply("12345\r\177",6);
			if ((r==13)||(r==127)) break;
			r-=49;
			for (j=1;j<=i;j++) if(hand[player].discards[j]==r) continue;
			hand[player].discards[++i]=r;
			scr_color(line,COL1+2+6*(r),24,0);
		}
		sendcurs(line,0);
		ERAEOL;
		sendcurs(line+2,56);
		ERAEOL;
	} while (r==127);
	hand[player].discards[0]=i;
				/* now player's discards are marked, 
				   but not yet filled */
	for (i=1;i<6;i++) hand[player].discards[i]= 4-hand[player].discards[i];
	sendcurs(7,1);
	ERAEOL;
	myprintf("\n");
	ERAEOL;
	myprintf("\n\n");
	ERAEOL;
}

/* ----------------------------- EVALUATE */

void evaluate(who)
int who;
{
	char lhand[5][2];
	int i,j,a,b,c,d,e,aq,bq,cq,dq,eq;
	int pair,twopair,three,straight,flush,full,four,stflush;
	
	pair= twopair= three= straight= flush=0;
	full= four= stflush=0;
	for (i=0;i<7;i++) hand[who].values[i]=0; 
	
	for (i=0;i<5;i++) {
		lhand[i][0]=txt[hand[who].cards[i]][0];
		lhand[i][1]=txt[hand[who].cards[i]][1];
	}
	for (i=0;i<5;i++){
		switch (lhand[i][0]) {
			case 'T': lhand[i][0]=10; break;
			case 'J': lhand[i][0]=11; break;
			case 'Q': lhand[i][0]=12; break;
			case 'K': lhand[i][0]=13; break;
			case 'A': lhand[i][0]=14; break;
			default:  lhand[i][0]-=48;
		}
	}
	/* LOOK FOR FLUSH */
	if ((lhand[0][1]==lhand[1][1])&&(lhand[0][1]==lhand[2][1])&&
(lhand[0][1]==lhand[3][1])&&(lhand[0][1]==lhand[4][1])) flush=1;
	
	/* LOOK FOR STRAIGHT */
	if ((lhand[0][0]+4==lhand[4][0])&&(lhand[1][0]+3==lhand[4][0])&&
(lhand[2][0]+2==lhand[4][0])&&(lhand[3][0]+1==lhand[4][0])) straight=1;
	if ((lhand[0][0]==2)&&(lhand[1][0]==3)&&(lhand[2][0]==4)&&
(lhand[3][0]==5)&&(lhand[4][0]==14)) straight=1;
	
	/* LOOK FOR STRAIGHT FLUSH */
	if ((flush==1)&&(straight==1)) stflush=1;
	
	/* LOOK FOR FOUR-OF-A-KIND */
	aq= bq= cq= dq= eq=0;
	a=lhand[0][0];
	b=lhand[1][0];
	c=lhand[2][0];
	d=lhand[3][0];
	e=lhand[4][0];
	if (((a==b)&&(a==c)&&(a==d))||((b==c)&&(b==d)&&(b==e))) four=1;
	
	/* LOOK FOR FULL HOUSE */
	aq=1;
	for (i=1;i<5;i++) if (lhand[i][0]!=lhand[i-1][0]) aq++;
	if ((aq==2)&&(four==0)) full=1;

	/* LOOK FOR TWO PAIR AND THREE-OF-A-KIND */
	if (aq==3){
		bq=0;
		for (i=0;i<5;i++){
			cq=0;
			for (j=0;j<5;j++){
				if (i==j) continue;
				if (lhand[i][0]==lhand[j][0]) cq++;
			}
			if (cq==0) bq++; /* bq SHOULD = # OF UNMATCHED CARDS */
		}
		if (bq==2) three=1;
		if (bq==1) twopair=1;
		if ((bq==0)||(bq>2)) {
			myprintf("\nError in TWOPAIR/THREE routine.\n");
			exit(1);
		}
	}
	
	/* LOOK FOR PAIR */
	if (aq==4) pair=1;
	
	/* SET values[0] TO INDICATE TYPE OF HAND */
	if (pair==1) hand[who].values[0]=1;
	if (twopair==1) hand[who].values[0]=2;
	if (three==1) hand[who].values[0]=4;
	if (straight==1) hand[who].values[0]=8;
	if (flush==1) hand[who].values[0]=16;
	if (full==1) hand[who].values[0]=32;
	if (four==1) hand[who].values[0]=64;
	if (stflush==1) hand[who].values[0]=128;
	
	/* SEND OUT FOR THE REST OF values[] */
	if (hand[who].values[0]==0) setvnull(lhand,hand[who].values);	
	if (hand[who].values[0]==1) setvpair(lhand,hand[who].values);
	if (hand[who].values[0]==2) setvtwo(lhand,hand[who].values);
	if (hand[who].values[0]==4) setvthre(lhand,hand[who].values);
	if (hand[who].values[0]==8) setvstra(lhand,hand[who].values);
	if (hand[who].values[0]==16) setvnull(lhand,hand[who].values); /* FLUSH SAME AS NULL */
	if (hand[who].values[0]==32) setvfull(lhand,hand[who].values);
	if (hand[who].values[0]==64) setvfour(lhand,hand[who].values);
	if (hand[who].values[0]==128) setvstra(lhand,hand[who].values); /* ST-FLUSH SAME AS STRAIGHT */


}
/* --------------------------- SETV */
void setvnull(lhand,values)
char lhand[][2];
int values[];
{
	int i;
	for (i=1;i<6;i++) values[i]=lhand[5-i][0];
}
void setvpair(lhand,values)
char lhand[][2];
int values[];
{
	int i,j;

	for (i=1;i<6;i++) values[i]=0;
	for (i=0;i<4;i++) if (lhand[i][0]==lhand[i+1][0]) values[1]=lhand[i][0]; 
	j=2;
	if (lhand[4][0]!=lhand[3][0]) values[j++]=lhand[4][0];
	for (i=3;i>0;i--) {
		if ((lhand[i][0]!=lhand[i-1][0])&&(lhand[i][0]!=lhand[i+1][0])){
			values[j++]=lhand[i][0];
		}
	}
	if (lhand[0][0]!=lhand[1][0]) values[j++]=lhand[0][0];
}
void setvtwo(lhand,values)
char lhand[][2];
int values[];
{
	int i,j;
	j=1;

	for (i=4;i>0;i--)
		if (lhand[i][0]==lhand[i-1][0]) values[j++]=lhand[i][0];
	if (lhand[4][0]!=lhand[3][0]) values[j++]=lhand[4][0];
	for (i=3;i>0;i--) 
		if ((lhand[i][0]!=lhand[i-1][0])&&(lhand[i][0]!=lhand[i+1][0]))
			values[j++]=lhand[i][0];
	if (lhand[0][0]!=lhand[1][0]) values[j++]=lhand[0][0];
	while (j<6) (values[j++])=0;
}
void setvthre(lhand,values)
char lhand[][2];
int values[];
{
	int i,j;

	for (i=0;i<4;i++) if (lhand[i][0]==lhand[i+1][0]) values[1]=lhand[i][0];
	j=2;
	if (lhand[4][0]!=lhand[3][0]) values[j++]=lhand[4][0];
	for (i=3;i>0;i--) 
		if ((lhand[i][0]!=lhand[i-1][0])&&(lhand[i][0]!=lhand[i+1][0]))
			values[j++]=lhand[i][0];
	if (lhand[0][0]!=lhand[1][0]) values[j++]=lhand[0][0];
	while (j<6) (values[j++])=0;
}
void setvstra(lhand,values)
char lhand[][2];
int values[];
{
	int i;
	if ((lhand[0][0]==2)&&(lhand[4][0]==14)) values[1]=5;
	else values[1]=lhand[4][0];
	for (i=2;i<6;i++) values[i]=0;
}
void setvfull(lhand,values)
char lhand[][2];
int values[];
{
	int i;
	for (i=3;i>0;i--) 
		if ((lhand[i][0]==lhand[i+1][0])&&(lhand[i][0]==lhand[i-1][0])){
			values[1]=lhand[i][0];
			break;
		}
	if (i==1) values[2]=lhand[4][0];
	else values[2]=lhand[0][0];
	
	values[3]= values[4]= values[5]=0;
}
void setvfour(lhand,values)
char lhand[][2];
int values[];
{
	int i;
	values[1]=lhand[1][0];
	if (lhand[1][0]!=lhand[0][0]) values[2]=lhand[0][0];
	else values[2]=lhand[4][0];
	for (i=3;i<6;i++) values[i]=0;
}

/* ------------------------------------- MAKEBET */
/* 
v[6] is values for the computer's hand
q is a character as follows:
	q=='o' if player has passed the opener and it is the computer's
		turn to offer an opener.
	q=='y' if the computer has opened and now it is the computer's
		turn to bet first at the final bet.
	q=='t' if the player has made a bet and the computer must decide
		whether to call, fold, or raise. (at the opening)
	q=='e' same as above, but at the final betting.
bet is the amount of the bet the player has made, which the computer is
	being challanged upon to match.
pot is the amount of the pot, not counting the pending bet.
bluff is the bluff index.

This function returns an integer as follows:
	('p'|1024) if the computer is to pass. Legal only if q=='o' or q=='y'
	('f'|1024) for fold. 
	('c'|1024) is for call. 
	Otherwise the amount of the bet between 1 and 50.                */


int makebet(highbet,mytotal,v,q,who)
int mytotal,highbet,who;
int q,v[];
{
	int b2,bet,bluff;
	
	bet=highbet-mytotal;
	bluff=hand[who].bluff;
	if (q=='o'){               /* computer's turn to open */
		if (bluff>97) return('p'|1024);
		if (v[0]>=8) return(50);
		if (bluff==0) return(50);
		if ((v[0]>=2)|| ((v[0]==1)&&(v[2]>10)) ) return(25);
		if (bluff<=2) return(25);
		return(('p'|1024));
	}
	if (q=='y') {			/* Round 2 computer's turn to open. */
		if (bluff>97) return('p'|1024);
		if (bluff==0) return(50);
		if (v[0]>=8)  return(50);
		if (bluff<=2) return(25);
		if ((v[0]>2)||((v[0]==2)&&(v[1]>=10))) return(25);
		return('p'|1024);
	}
	if (q=='t'){             /* Round 1 normal bet. */
		if ((bluff)&&(v[0]<=4)&&(mytotal>150)) return('c'|1024);
		if (mytotal>250) return('c'|1024);
		if (bluff==0) return(50);
		if (v[0]>=8) return(50);
		if (bluff<=2) {
			if (mytotal>=25) return('c'|1024);
			return(25);
		}
		if ((v[0]>2)||((v[0]==2)&&(v[1]>=10))) return(25);
		if (v[0]==0){
			if (al_flushq(who)) return('c'|1024);
			if (al_stq(who)) return('c'|1024);
			if (bet< ((bluff/25)+10)) return('c'|1024);
			return('f'|1024);
		}
		return(('c'|1024));
	}
	if (q=='e'){   		/* Round 2 normal bet. */
			/* Set a reasonable limit: */
		if ((bluff)&&(v[0]<=4)&&(mytotal>150)) return('c'|1024);
		if (mytotal>250) return('c'|1024); 
			/* Bluff: */
		if (bluff==0) return(50);
			/* Low bluff: */
		if (bluff>97) return('c'|1024);
			/* Bet real hands: */
		if (v[0]>8) return(50);
		if (v[0]==4) {
			if (bluff%10==0) return(25);
			return(50);
		}
		if (v[0]==4){
			if (bluff%10) return(50);
			return(25);
		}
		if (bluff<=2) return(25);
			/* when to fold: */
		if ((v[0]==0)||((v[0]==1)&&(v[1]<=10))) { /* worthless hand */
			if (bet<(bluff%25)+5) return(('c'|1024));
			if (bet<(pot/25)) return(('c'|1024));
			return(('f'|1024));
		}
		return('c'|1024);
	}
}
/* Note A -- The effect of the step referenced is that if the player opens
   with a token bet, to try to grab the ante, and if the computer fails
   to raise the bet, the player will not necessarily know that the computer
   has a poor hand.                                                   */

int getbet(q,line,pot,bet)
int q, *line, bet, pot;
{
	char buf[50];
	int n,i;
	
	setcolor(hand[player].color);
	myprintf("\n\nThere are now %d %s in the pot.",pot,money);
inpu:	switch (q) {
		case 'o': 
			myprintf("\nEnter (P)ass or the amount of your opening bet: ");
			prompt("Pass or bet.","","");
			break;
		case 'y':
			myprintf("\nEnter (P)ass or the amount of your bet: ");
			prompt("Pass or bet.","","");
			break;
		case 't':
		case 'e':
			myprintf("\nBet is %d. Enter (C)all, (F)old, or the amount of your raise: ",bet);
			mysprintf(buf,"%d %s has been bet.",bet,money);
			prompt(buf,"Call, fold, or raise.","");
			break;
		default: 
			myprintf("\nError in getbet().");
			exit(1);
	}
	for (i=0;;i++){
		buf[i]=get2reply("\b\r\177fpc0123456789",q);
		if (buf[i]==8) {
			if (i==0) continue;
			i-=2;
			myprintf("\b \b");
			continue;
		}
		myputchar(tpr(buf[i]));
		if (buf[i]==13) { buf[i]=0; break; }
		if (buf[0]=='f') return(('f'|1024));
		if (buf[0]=='p'){
			if ((q=='o')||(q=='y')) return(('p'|1024));
			myprintf("\nLegal inputs are C, F, or a bet. "); 
			i=(-1);
			continue;
		}
		if (buf[0]=='c'){
			if ((q=='t')||(q=='e')) return(('c'|1024));
			myprintf("\nLegal inputs are P, F, or a bet. "); 
			i=(-1);
			continue;
		}
		if ((buf[i]==127)||(i>25)) {
			myprintf("\nDeleted. Start over: ");
			i=(-1);
			continue;
		}
		if ((buf[i]<'0')||(buf[i]>'9')) {
			myprintf("\b \b"); 
			i--;
			continue;
		}		
	}
	n=myatoi(buf);
	if (n>50){
		myprintf("\nLimit is 50 %s. Try again. ",money);
		goto inpu;
	}
	if (n==0){
		if (q=='o') return(('p'|1024));
		else return(('c'|1024));
	}
	return(n);
}

int al_flushq(who)
int who;
{
	int local[5],i;
	
	for (i=0;i<5;i++)
		local[i]=txt[ hand[who].cards[i] ][1];
	qsort(local,5,sizeof(int),comp);
	
	if ((local[0]==local[1])&&(local[1]==local[2])&&(local[2]==local[3]))
		return(1);
	if ((local[1]==local[2])&&(local[2]==local[3])&&(local[3]==local[4]))
		return(1);
	return(0);
}
int al_stq(who)
int who;
{
	int local[5],i;
	
	for (i=0;i<5;i++){
		local[i]= txt[ hand[who].cards[i] ][0];
		switch (local[i]) {
			case 'T': local[i]=10+48; break;
			case 'J': local[i]=11+48; break;
			case 'Q': local[i]=12+48; break;
			case 'K': local[i]=13+48; break;
			case 'A': local[i]=14+48; break;
		}
	}
	
	if ((local[0]+1==local[1])&&(local[1]+1==local[2])&&(local[2]+1==local[3]))
		return(1);
	if ((local[1]+1==local[2])&&(local[2]+1==local[3])&&(local[3]+1==local[4]))
		return(1);
	return(0);
}

char *pfh_777 = "Press \"?\" for help." ;
char *yc_777 = "     Your cards:" ;
char *folded_777 = "Folded" ;

void prompt(s1,s2,s3)
char *s1,*s2,*s3;
{
	int i,j;
	char *p[5];
	
	p[1]=s1;
	p[2]=s2;
	p[3]=s3;
	p[4]=pfh_777; 

	for (i=0;i<80;i++)
		for (j=0;j<7;j++)
			scr_color(j,i,' ',0);

	for (i=0;yc_777[i];i++) 
		scr_color(0,i,yc_777[i],0);

	if (hand[player].in){
		for (i=0,j=4;i<5;i++,j--)
			writecard(2,COL1+i*6,hand[player].cards[j]);
	}
	else {
		for (i=0,j=4;i<5;i++,j--)
			writecard(2,COL1+i*6,52);
		for (i=0;folded_777[i];i++) 
			scr_color(3,i+13,folded_777[i],0);
  	}
	
	for(i=41;i<79;i++) scr_color(0,i,205,0);
	for(i=41;i<79;i++) scr_color(5,i,205,0);
	for(i=1;i<5;i++) scr_color(i,40,186,0);
	for(i=1;i<5;i++) scr_color(i,79,186,0);
	scr_color(0,40,201,0);
	scr_color(0,79,187,0);
	scr_color(5,40,200,0);
	scr_color(5,79,188,0);
	
	for (i=41;i<=42;i++) 
		for (j=1;j<=4;j++)
			scr_color(j,i,' ',0);
	for (j=1;j<=4;j++) {
		for (i=0;p[j][i];i++) 
			scr_color(j,i+43,p[j][i],0);
		i+=43;
		while (i<79)
			scr_color(j,i++,' ',0); 
	}
}
void setcolor(c)
int c;
{
	if (color=='c') _attrib=c;
}

/* -------------------------------- SHUFFLE */
void shuffle()
{
	int i,j,k,num,hold[52];
	
	for (i=0;i<52;i++) deck[i]=i;
	
	for (k=0;k<3;k++) {
		for (num=51,i=0;i<52;i++,num--){
			if (num==0) j=0;
			else j=rando()%num+1;
			hold[i]=deck[j];
			deldeck(j);
		}
		for (i=0;i<52;i++) deck[i]=hold[i];
	}
	j=rando()%45+3;
	for (i=0;i<52;i++,j++) {
		if (j==52) j=0;
		deck[i]=hold[j];
	}
}
void deldeck(n)
int n;
{
	while (n<51) {
		deck[n]=deck[n+1];
		n++;
	}
}
int rando()
{
	static long zi;
	static int mark=1;
	long time();
	
	if (mark) {
		mark=0;
		zi = time((long *)0);
		zi = zi & 16383;
	}
	zi*=23;
	zi+=17;
	zi%=32749;
	return((int)zi);
}
/* ------------------------------ COMP */
int comp(a,b)
char *a,*b;
{
	return(*(int *)a-*(int *)b);
}
int compf(a,b)
char *a,*b;
{
	a+=2;
	b+=2;
	return(*(int *)a-*(int *)b);
}
int valcmp(aa,bb)
char *aa,*bb;
{
	int i,*a,*b;
	
	a=(int *)aa;
	b=(int *)bb;
	for (i=0;i<6;i++){
		if (a[i]>b[i]) return(-1);
		if (b[i]>a[i]) return(1);
	}
	return(0);
}
int myatoi(s)
char *s;
{
	int n=0;
	
	while(*s==' ') s++;
	for(;;s++){
		if ((*s<'0')||(*s>'9')) return(n);
		n*=10;
		n+= *s-48;
	}
}

char *itoa(n,s)
int n;
char *s;
{
	s+=10;
	*s=0;
	for(;;){
		*(--s) = n%10+48;
		n/=10;
		if (n==0) return(s);
	}
}
int get2reply(s,q)
char *s;
int q;
{
	int i,c;
	
	for(;;){
		c=mygetch();
		if ((c=='?')||(c=='/')) { help(q); continue; }
		if ((c>='A')&&(c<='Z')) c+=32;
		for (i=0;s[i];i++) if (c==s[i]) return(c);
	}
}
int tpr(c)
int c;
{

	if ((c>='a')&&(c<='z')) c-=32;
	return(c);
}
int final_display()
{
	char *wp;
	int i,j,k,m,line,col,tie,helpflag=1;
	int order[5][7];
	
	for (i=0;i<5;i++) {
		hand[i].values[6]=i;
		for (j=0;j<7;j++) order[i][j]=(hand[i].values[j]);
		if (hand[i].in==0) 
			for (j=0;j<6;j++) hand[i].values[j]=order[i][j]=0;
	}
	qsort((char *) order, 5, 7*sizeof(int), valcmp);
	
	CLEARSCREEN;
	line=CARDLINE(0)-1;
	col=COL1;
	
	if (hand[order[1][6]].in == 0){  /* Only one hand in. No showdown. */
		setcolor(hand[ order[0][6] ].color);
		myprintf("Only one player remaining in.\n");
		if (order[0][6]!=player) 
			myprintf("%s takes", hand[ order[0][6] ].name ); 
		else myprintf("You take");
		myprintf(" the pot of %d %s.",pot,money); 
		helpflag=tie=0;
	}
	else for (i=0;i<5;i++){
		sendcurs(line,col);
		j=order[i][6];
		setcolor(hand[j].color);
		if (player==j) myprintf("Your cards:");
		else myprintf("%s\'s cards:", hand[j].name );
		
		line++;
		if (hand[j].in)
			for (m=0,k=4;m<5;m++,k--)
				writecard(line,col+m*6,hand[j].cards[k]);
		else {
			for (m=0;m<5;m++)
				writecard(line,col+m*6,52);
			sendcurs(line+1,col+8);
			myprintf("Folded");
		}
		line += 5;
		if (i==4) break;
/* problems here: */
		if (((valcmp((char *)order[i],(char *)order[i+1]))&&(col==COL1))||((i==3)&&(col==COL1))){
			sendcurs(line,5);
			if (i==0) myprintf("Take the pot");
			else myprintf("Split the pot");
			myprintf(" of %d %s.",pot,money); 
			line=CARDLINE(0)-1;
			col=COL2;
			tie=i;
		}
	}
	
	/* Settle the pot */
	for (i=0;i<=tie;i++)
		hand[order[i][6]].balance += pot/(tie+1);
	pot%= (tie+1);  /* Sets pot to zero but leaves the odd chips in. */
	
	/* Print out totals: */
	sendcurs(17,0);
	setcolor(hand[player].color);
	myprintf("Your balance is %d %s.\n",hand[player].balance,money);
	for (i=0;i<5;i++){
		if (i==player) continue;
		setcolor(hand[i].color);
		myprintf("%s\'s balance is %d %s.\n",hand[i].name,hand[i].balance,money);
	}
	setcolor(hand[player].color);
	rotate();
	player--;
	if (player==(-1)) player=4;
	setcolor(hand[player].color);
	myprintf("Would you like to play another hand? ");
	i=get2reply("yn",8+helpflag); 
	return (i);
}

rotate()
{
	struct _hand_ temp;
	int i;
	
	temp=hand[0];
	for(i=0;i<4;i++) hand[i]=hand[i+1];
	hand[4]=temp;
}

void writecard(v,h,card)
int v,h,card;
{
	int n,s;
	
	n=txt[card][0];
	s=txt[card][1];
	
	scr_color(v,h,218,0);
	scr_color(v,h+1,196,0);
	scr_color(v,h+2,196,0);
	scr_color(v,h+3,196,0);
	scr_color(v,h+4,191,0);
	scr_color(v+1,h,179,0);
	colorchar(v+1,h+1,s,' ');
	colorchar(v+1,h+2,s,' ');
	colorchar(v+1,h+3,s,' ');
	scr_color(v+1,h+4,179,0);
	scr_color(v+2,h,179,0);
	colorchar(v+2,h+1,s,' ');
	colorchar(v+2,h+2,s,' ');
	colorchar(v+2,h+3,s,' ');
	scr_color(v+2,h+4,179,0);
	scr_color(v+3,h,192,0);
	scr_color(v+3,h+1,196,0);
	scr_color(v+3,h+2,196,0);
	scr_color(v+3,h+3,196,0);
	scr_color(v+3,h+4,217,0);
	if (n=='T'){
		colorchar(v+1,h+1,s,'1');
		colorchar(v+1,h+2,s,'0');
	}
	else colorchar(v+1,h+2,s,n);  
	switch(s){
		case 'H': colorchar(v+2,h+2,s,3); break;
		case 'D': colorchar(v+2,h+2,s,4); break;
		case 'C': colorchar(v+2,h+2,s,5); break;
		case 'S': colorchar(v+2,h+2,s,6); break;
	}
}
void colorchar(v,h,suit,c)
int v,h,suit,c;
{
	switch (suit) {
		case 'H': 
		case 'D': scr_color(v,h,c,1); break;
		default:  scr_color(v,h,c,0);
	}
}
void scr_color(v,h,c,q)
int v,h,c,q;
{
	int off;

	off = 160*v+ 2*h;
	shmaddr[off]=c;
	switch(q){
		case 2:
			shmaddr[off+1]=helpcolor;
			break;
		case 1:
			shmaddr[off+1]=revid_atr;
			break;
		case 0:
			shmaddr[off+1]=norm_atr;
			break;
	}
}

void help(q)
int q;
{
	unsigned off;
	int i,h,v;
	static char *corent = "\
Use the BACKSPACE key to make corrections or the DEL key to start over.\n\
Press the ENTER (or RETURN) key to enter the amount you have indicated.";
	static char *yorn = "\
On returning to the game, answer \"Y\" to play another hand\n\
or \"N\" to exit from the program.\n";
	
	for (off=0;off<4000;off++) sidescreen[off]=shmaddr[off];
	
	scr_loc(&v,&h);

	CLEARSCREEN;
	setcolor(helpcolor);
	switch (q) {
		case 1:
	myprintf("Five-card draw poker is a gambling game.\n\
You must select which currency you wish to play for.\n");
	break;
		case 2:
	myprintf("The first round of betting has concluded.\n\
The program has paused to allow you to see how much each player has bet.\n");
	break;
		case 3:
	myprintf("\
At this point one or more players have discarded.\n\
The program has paused to let you see how many cards each has thrown out.\n\
Because discarding is done in rotation, there may be one or more\n\
players that have not yet discarded.\n\
They will do so after you.\n"); 
	break;
		case 4:
	myprintf("The program has paused to let you see how many cards have been\n");
	if (hand[player].in) {
		myprintf("\
thrown out by the players that came after you in the rotation order.\n\
\nWhen you continue, new cards will be dealt to you\n\
to replace any you have discarded.\n");
	}	
	else myprintf("thrown out by each player.\n");
	break;
		case 5:
	myprintf("Betting has concluded.\n\
The program has paused to let you see how much each player has bet.\n");
	break;
		case 6:
	myprintf("\
You may discard any number of cards.\n\
Or you may \"stand pat\" by discarding no cards.\n\n\
Using the numerals at the top of your keyboard, indicate the card(s)\n\
you wish to throw out.\n\
The leftmost card is \"1\", the rightmost card is \"5\"\n\n\
If you accidentally indicate a card you do not wish to discard,\n\
press the DEL key and start over.\n\n\
When you have correctly indicated ALL the cards you wish to throw out\n\
press the ENTER key. (May be marked \"RETURN\" on some keyboards.)\n");
	break;
		case 'o':
	myprintf("\
It is your turn to open the betting for this hand. Some poker rules require \n\
\"jacks or better\" to open. This version does not require any minimum hand to \n\
open. You may open on a bluff if you wish. Opening the betting merely means \n\
placing the first bet.\n\n\
If you do not wish to open, you may press P for PASS. You remain in the\n\
game and the next player has the chance to open. If no player opens, \n\
new hands are dealt and the game proceeds as with a new hand.\n\n\
To enter a bet, type the amount of your bet. The limit is 50 %s.\n\
An opening bet of zero is the same as PASS.\n\n\
%s",money,corent);
	break;
		case 'y':
	myprintf("\
This is the second (and final) round of betting. \n\
It is your turn to place the first bet.\n\n\
If you do not wish to bet, you may press P for PASS. You will remain in the \n\
game and the next player will have a chance to bet. If no player bets, the \n\
game proceeds directly to the \"showdown\" and the highest hand wins the pot.\n\n\
To enter a bet, type the amount of your bet. The limit is 50 %s.\n\
A bet of zero is the same as PASS.\n\n\
%s",money,corent);
	break;
		case 't':
		case 'e':
	myprintf("\
The %s round of betting is in progress.\n\
A bet has been placed by another player.\n\
You must either CALL the bet, FOLD your hand, or enter a raise.\n\
If you press C for CALL, it has the effect of matching the bet.\n\
You are putting that amount of money into the pot and you remain in the game.\n\
If you press F for FOLD, you are not putting any more money in the pot, but \n\
you forfeit whatever money you have already put in, and you are out of the \n\
game for the rest of this hand.\n\n\
If you raise (by entering a number from 1 to 50) you are putting that number \n\
of %s into the pot, PLUS the amount of the pending bet. \n\
Thus if there is a pending bet of 25 %s and you enter 25, you are\n\
actually putting 50 %s into the pot.\n\n\
To enter a raise, type the amount. The limit is 50 %s.\n\
A raise of zero is the same as CALL.\n\n\
%s", (q=='t') ? "opening" : "final" ,money,money,money,money,corent);
	break;
		case 9:
	myprintf("\
The winning hand is shown in the left column.\n\n\
The losing hands are shown in the right column, in descending order of\n\
strength.\n\n\
A player who has folded does not have to show his cards. Therefore you only\n\
see the cards of players who have remained in until the end.\n\n\
%s",yorn);
	break;
		case 8:
	myprintf("\
If every player but one has folded, rather than calling the bet of the one\n\
remaining player, that player wins the pot and does not have to show his\n\
cards. If you want to see the cards in the winning hand, you (or some other\n\
player) must call the bet.\n\n\
Thus, with only one player remaining in at the end of this hand, you are not\n\
shown any cards. Maddening, isn't it?\n\n\
%s",yorn);
	break;
		default:
	myprintf("\nError in help().\n");
	exit(1);

	}
	myprintf("\n\n\
     Now press \"H\" to see the order of poker hands.\n\
     Or press any other key to return to the game... ");
	
	for(;;){
		showtime();
		if ((i=inkey())!=-1) break;
	}
	if (tpr(i)=='H') instructions();
	
	for (off=0;off<4000;off++) shmaddr[off]=sidescreen[off];
	scr_curs(v,h);
}

void instructions()
{
	int r;
	
	CLEARSCREEN;
	myprintf("\
For complete rules of 5-card draw poker see Hoyle's Rules of Games.\n\
The order of poker hands is as follows:\n\n\
Straight-flush. (Any straight all in one suit.)\n\
Four-of-a-kind.\n\
Full house. (Three-of-a-kind plus a pair.)\n\
Flush. (Any 5 cards all of the same suit.)\n\
Straight. (5 cards all in numerical order, as 3,4,5,6,7 or 8,9,10,J,Q.)\n\
Three-of-a-kind.\n\
Two pair.\n\
Pair.\n\n\
For tie breaking higher cards beat lower cards and the ace is high,\n\
except that in a straight, an ace may be either high or low.\n\
There is no precidence of suits: All suits are equal.\n\
In a full house the 3-of-a-kind has higher precidence than the pair.\n\
In the event of a draw, the pot is split.\n\n\
Press any key... ");
	while ((r=inkey())==-1) showtime();
}
void intro(str)
char *str;
{
	char s[50],*abstoptr();
	int hour;
	struct tm t;
	
	CLEARSCREEN;
	myprintf("%s%s\
This program is shareware.\n\
You may make as many copies as you like.\n\
Please pass copies along to your friends.\n\
If you find it enjoyable please contribute $5 to help support the future\n\
of play-before-you-pay software.\n\n\
\tDan Schechter\n\
\tRoute 1 Box 19\n\
\tAmenia, North Dakota, 58004\n\n\
Manù thanks.\n\n\
Press any key to begin the game... ",version,cat);
	hour=mygetch();
	CLEARSCREEN;
	dostime(&t);
	hour=t.tm_hour;
	strcpy(str,"morning");
	if (hour>=12) strcpy(str,"afternoon");
	if (hour>=18) strcpy(str,"evening");

	if (( getmode() &15 ) ==7) color='m';
	else color='c';

	if (color=='m') shmaddr=abstoptr(0xb0000l);
	else shmaddr=abstoptr(0xb8000l);
	CLEARSCREEN;
	setcolor(helpcolor);

}
int mygetch()
{
	int c;

	do {
		while (scr_poll()==-1) rando();
		c=scr_getc();
		if ((c==3)||(c==-2)) exit(0);
		if (c==211) c=127;
		if (c==203) c=8;
	} while (c>127);
	return(c);
}
int inkey()
{
	int c;
	
	c=scr_poll();
	if (c==-1) return(-1);
	c=scr_getc();
	if (c>127) return(-1);
	if ((c==3)||(c==-2)) exit(0);
	return(c);

}

void showtime()
{
	struct tm t;
	char s[50];
	int i=0,hour;
	
	dostime(&t);
	hour=t.tm_hour;
	if (hour>=13) hour-=12;
	if (hour==0) hour=12;
	if (hour>=10) s[i++]='1';
	s[i++]=hour%10+48;
	s[i++]=':';
	s[i++]= t.tm_min/10+48;
	s[i++]= t.tm_min%10+48;
	s[i++]= ':';
	s[i++]= t.tm_sec/10+48;
	s[i++]= t.tm_sec%10+48;
	s[i]=0; 

	for(i=0;i<8;i++) scr_color(24,i+36,s[i],2); 


}
getmode()
{
	struct { unsigned ax,bx,cx,dx,si,di,ds,es; } regs;
	
	regs.bx=regs.cx=regs.dx=regs.si=regs.di=regs.ds=regs.es=0;
	regs.ax = 15<<8;
	sysint(0x10,&regs,&regs);
	
	return (regs.ax&255);
}
/* ------------------------------------------- FORMAT: 
I got carried away with wanting to minimize executable file size
So I sometimes use the following functions. */

static int (*fun)();
int myformat(f,ctrl,p)
int (*f)();
char *ctrl;
char *p;
{
	fun=f;
	
	while (*ctrl) {
		if ((*ctrl)=='%') {
			ctrl++;
			switch(*ctrl++){
				case 's': 
					_kstrput(p);
					p += sizeof(char *);
					break;
				case 'd': 
					_mydput(p);
					p += sizeof(int);
					break;
				case 0:
					return;
			}
			continue;
		}
		(*fun)(*(ctrl++));
	}
}
_kstrput(p)
char **p;
{
	int i=0;
	
	while ((*p)[i])
		(*fun) ((*p)[i++]);
}
_mydput(p)
char *p;
{
	int t,n,mask;
	
	n= *((int *)p);
	if (n<0){
		(*fun)('-');
		n = -n;
	}
	for(mask = 10000;mask!=1;mask/=10)
		if (mask<=n) break;
	for(;mask;mask/=10){
		t = n/mask;
		n -= t*mask;
		(*fun)(t+48);
	}
}
/* ------------------------------------------- PRINTF: */
myprintf(ctrl,parml)
char *ctrl;
unsigned parml;
{
	int myputchar();
	myformat(myputchar,ctrl,&parml);
}
int myputchar(c)
int c;
{
	if (c=='\t') myprintf("        ");
	else scr_putc(c);
	if (c=='\n') scr_putc('\r');
}
/* ------------------------------------------- SPRINTF: */
static char *qb;
mysprintf(s,ctrl,parml)
char *s,*ctrl;
unsigned parml;
{
	static int _ccp();

	qb = s;
	myformat(_ccp,ctrl,&parml);
	*qb = 0;
}
static _ccp(c)
{
	*qb++ = c;
}
/* ------------------------------------------- */
