/*
TITLE:		BLACKJACK.C;
DATE:		1/10/89;
DESCRIPTION:	"Blackjack.";
VERSION:	1.03;
KEYWORDS:	blackjack, cards;
FILENAME:	BLACKJACK.C;
WARNINGS:	"Requires IBM or compatible.";
SEE-ALSO:	CARDS.DOC, POKER.C;
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
only on a not-for-profit basis. Compiles under small model.
 */
#include <stdio.h>
char *version = "\
BLACKJACK from Kittensoft. Version 1.03 1/10/89\n\
Copyright (c) 1987, 1988, 1989, by Dan Schechter\n\
Route 1 Box 19\n\
Amenia, North Dakota 58004\n\n\
This program is shareware. Pass copies along to your friends.\n\
a single $5 contribution covers both BLACKJACK and POKER.\n" ;

#define KLINE 14
#define SLINE 2
#define BOFF 4
#define TOFF 5
#define DEALERSTAND 17
#define PROMPTLINE 22
#define MINCARD 15	/* Minimum number of cards needed to play a hand. */
int deck[52];
int *numused,except,color,oldattrib;
char far *shmaddr;
extern int _attrib;

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

main()
{
	int 	i,bet=0,payoff,st,kt,kcard[15],scard[15],kcol,scol,
		cardcnt,kcount,scount,bjk;
	
	oldattrib=_attrib;
	
	if (( getmode() &15 ) ==7){
		shmaddr=(char far *)0xb0000000;
		color='m';
	}
	else {
		shmaddr=(char far *)0xb8000000;
		_attrib=10;
		color='c';
	}
	payoff=0;
	numused= &cardcnt;
	scr_clear();
	tprintf("%s\n",version);
	tprintf("\
Bet limit is 999 dollars.\n\
Dealer stands on 17.\n\
Dealer wins all draws.\n\
The deck is shuffled if fewer than %d cards remain.\n\
Good luck!\n\n\
Please place your bet. ",MINCARD);	
	shuffle();
	cardcnt=0;
	goto gb;
	
shuf:	shuffle();
	cardcnt=0;
	tprintf("New Deck.");
bett:	;
	tprintf("\nYour balance is %d dollars.\n\
Please place your bet. <CR> to repeat previous bet. ",payoff);
gb:	bet=getbet(bet);
	scr_clear();
	scr_curs(0,25);
	tprintf("Your Balance is %d. ",payoff);
	if (bet) tprintf("Your bet is now %d.",bet);
	else tprintf("No bet on this hand.");
	kcol=scol=scount=kcount=0;
	scr_curs(0,0);
	tprintf("YOUR CARDS");
	scr_curs(KLINE-2,0);
	tprintf("DEALER'S CARDS");
	scard[scount++]=deck[cardcnt++];
	kcard[kcount++]=deck[cardcnt++];
	scard[scount++]=deck[cardcnt++];
	except=cardcnt;
	kcard[kcount++]=deck[cardcnt++];

	writecard(SLINE,scol,scard[0]);
	scol+=6;
	writecard(SLINE,scol,scard[1]);
	scol+=6;
	writecard(KLINE,kcol,kcard[0]);
	kcol+=6;
	
	st=countup(scard,scount);
	bjk=0;
	if (st==21){
		scr_curs(SLINE+BOFF,0);
		tprintf("Blackjack!");
		bjk=1;
	}
	kt=countup(kcard,kcount);
	if (kt==21){
		writecard(KLINE,kcol,kcard[1]);
		bjk=2;
	}
	if (bjk) goto bust;
	scr_curs(PROMPTLINE,0);
	tprintf("(H)it or (S)tand? ");  
	scr_eol();
htst:	if (getreply("hs")=='h'){
		scard[scount]=deck[cardcnt++];
		writecard(SLINE,scol,scard[scount++]);
		scol+=6;
		if ((st=countup(scard,scount))>21) goto bust;
		goto htst;
	}
bust:	if (bjk==2) {
		scr_curs(KLINE+BOFF,0);
		tprintf("Blackjack!");
	}
	else {
		writecard(KLINE,kcol,kcard[1]);
		kcol+=6;
	}
	if (bjk) goto bkjk;
	if (st<=21){
		while ((kt=countup(kcard,kcount))<DEALERSTAND){
			kcard[kcount]=deck[cardcnt++];
			writecard(KLINE,kcol,kcard[kcount++]);
			kcol+=6;
		}
	}
	else kt=countup(kcard,kcount);
	scr_curs(SLINE+TOFF,0);
	tprintf("Total: %d",st);
	if (st>21) tprintf("     You bust.");
	scr_eol();

	scr_curs(KLINE+TOFF,0);
	tprintf("Total: %d",kt);
	if (kt>21) tprintf("     I bust.");
bkjk:	;
	scr_curs(PROMPTLINE,0);
	scr_eol();
	if ((bjk==2)|((bjk==0)&((st>21)|((kt>=st)&(kt<=21))))){
		payoff-=bet;
		tprintf("You lose.\n");
	}
	else {
		payoff+=bet;
		tprintf("You win.\n");
	}
	if (payoff>30000){
		tprintf("You have won %d dollars. The casino is busted. Sob!",payoff);
		_attrib=oldattrib;
		exit(0);
	}
	if (payoff<-30000){
		tprintf("You have lost %d dollars. No more credit!",payoff*-1);
		_attrib=oldattrib;
		exit(0);
	}
	tprintf("Your balance is %d dollars. Another hand? ",payoff);
	scr_eol();
	except= 0;
	if (getreply("yn")=='y'){
		scr_clear();
		if (cardcnt>52-MINCARD) goto shuf;
		goto bett;
	}
	if (payoff<0) tprintf("\r\
%d dollars will be electronically deducted from your bank account.",payoff*-1);
	if (payoff>0) tprintf("\r\
I'll mail you an I.O.U. for %d dollars from Argentina.",payoff);
	if (payoff==0) tprintf("\rGosh. You came out even!");
	scr_eol();
	_attrib=oldattrib;
	exit(0);
}
countup(hand,n)
int hand[],n;
{
	int i,aces,c,total;
	
	for (i=aces=total=0;i<n;i++){
		c=txt[hand[i]][0];
		if (c>'B') c=':';  /* for ten */
		if (c=='A'){
			c='1';
			aces++;
		}
		total+=(c-48);
	}
	for (i=0;i<aces;i++) if (total<=11) total+=10;
	return(total);
}
/* -------------------------------- SHUFFLE */
shuffle()
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
deldeck(n)
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
int getreply(s)
char *s;
{
	int i,c;
	
	for(;;){
		c=scr_getc();
		if ((c==3)||(c==-2)) {
			_attrib=oldattrib; 
			tprintf("\nQuitter!\n");
			exit(1);
		}
		c=tlr(c);
		for (i=0;s[i];i++) if (c==s[i]) {
			scr_putc(c);
			return(c);
		}
	}
}
writecard(v,h,card)
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
colorchar(v,h,suit,c)
int v,h,suit,c;
{
	switch (suit) {
		case 'H': 
		case 'D': scr_color(v,h,c,1); break;
		default:  scr_color(v,h,c,0);
	}
}
scr_color(v,h,c,q)
int v,h,c,q;
{
	int off;

	off = 160*v+ 2*h;
	shmaddr[off++]=c;
	if (color=='m') shmaddr[off] = q ? 112 : 7 ;
	else shmaddr[off] = q ? 116 : 112 ;
}
int tlr(c)
int c;
{
	if ((c>='A')&&(c<='Z')) c+=32;
	return(c);
}
getmode()
{
	struct { unsigned ax,bx,cx,dx,si,di,ds,es; } regs;
	
	regs.bx=regs.cx=regs.dx=regs.si=regs.di=regs.ds=regs.es=0;
	regs.ax = 15<<8;
	sysint(0x10,&regs,&regs);
	
	return (regs.ax&255);
}
int tprintf(ctrl,parml)
char *ctrl;
unsigned parml;
{
	char *p;
	
	p= (char *)(&parml);
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
		if (*ctrl=='\n') scr_putc('\r');
		scr_putc( *(ctrl++));
	}
	return;
}
int _kstrput(p)
char **p;
{
	int i=0;
	
	while ((*p)[i]){
		if ( (*p)[i]=='\n' ) scr_putc('\r');
		scr_putc((*p)[i++]) ;
	}
}
_mydput(p)
char *p;
{
	int t,n,mask;
	
	n= *((int *)p);
	if (n<0){
		scr_putc('-');
		n = -n;
	}
	for(mask = 10000;mask!=1;mask/=10)
		if (mask<=n) break;
	for(;mask;mask/=10){
		t = n/mask;
		n -= t*mask;
		scr_putc(t+48);
	}
}
getbet(b)
int b;
{
	int a=0,c,q=1;
	
	for(;;){
		c=getreply("1234567890\r");
		if (c=='\r') {
			if (q) return b;
			return a;
		}
		q=0;
		a *= 10;
		a += c-48;
		if (a>999) {
			tprintf("\nLimit is 999. Please start over: ");
			a=0;
		}
	}
}
