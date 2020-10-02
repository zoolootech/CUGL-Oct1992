/*
TITLE:		Wall Clock;
DATE:		9/19/88;
DESCRIPTION:	"Analog clock face.";
VERSION:	1.10;
KEYWORDS:	Clock, Graphics, HGA, Hercules;
FILENAME:	WALCLOCK.C
WARNINGS:	"Requires Hercules monochrome graphics.";
SEE-ALSO:	WALCLOCK.DAT, WALCLOCK.EXE;
SYSTEM:		MS-DOS;
COMPILERS:	Aztec;
AUTHORS:	Dan Schechter;
 */
#include <stdio.h>
#include <time.h>
#include <math.h>

#define TICKTOCK	/* Delete this if you don't want the tick-tock. */
#define NOMATH		/* Delete this if the program will always be
			run on a computer with a floating-point coprocessor
			and your compiler has FPU support. With NOMATH
			defined, the program uses a table look-up to 
			position the hands. This makes the program about twice
			as large. If NOMATH is not defined, the program
			calculates the positions of hands on the fly, but
			an FPU is necessary if this is to be done fast enough
			to keep up with real time. */
#ifdef TICKTOCK
#include <signal.h>
#endif

#ifdef NOMATH
#define VERSION "Version 1.10-t 9/19/88" 
#else
#define VERSION "Version 1.10-fpu 9/19/88" 
#endif

#define CENTERH 360
#define CENTERV 174
#define TOPH 360
#define TOPV 25
#define LONG 125
#define SHORT 75

#define INDEXPORT 0x3B4
#define CONTROLPORT 0x3b8
#define DATAPORT 0x3b5
#define CONFIGPORT 0x3bf
#define SCREEN0 0xb0000 	/* Absolute address for page 0. */
#define SCREEN1 0xb8000 	/* Absolute address for page 1. */
#define ROMFONT 0xffa6e       /* Absolute address of IBM ROM character set. */

void drawclock(void), controller(void), line(int,int,int,int,int), 
     gocircle(int,int), _drawclock(void), 
     hand(int,int,int,int,int), writetd(int,int,int,int, struct tm *), 
     markers(void), g_write(char *,int,int), sigexit(int), cleardot(int,int), 
     reagantime(struct tm *), chinatime(void), menu(void), 
     stepclock(void), writedot(int,int), g_putch(int,int,int), sw_scr(int), 
     g_mode(void), t_mode(void), *abstoptr(unsigned long),
     tick(void), soundon(void), soundoff(void), tick_init(void);

int  sqroot(long);

char *day[7] = {
	"Sunday   ",
	"Monday   ",
	"Tuesday  ",
	"Wednesday",
	"Thursday ",
	"Friday   ",
	"Saturday "
};
char *month[12] = {
	"January ",
	"February ",
	"March ",
	"April ",
	"May ",
	"June ",
	"July ",
	"August ",
	"September ",
	"October ",
	"November ",
	"December "
};

int gdata[12] = { 
	0x35, 0x2d, 0x2e, 0x7, 0x5b, 0x2, 0x57, 0x57, 0x2, 0x3, 0x0, 0x0 
};
int tdata[12] = { 
	0x61, 0x50, 0x52, 0xf, 0x19, 0x6, 0x19, 0x19, 0x2, 0xd, 0xb, 0xc 
};

int h24[24] = { 53, 61, 68, 73, 76, 77, 76, 73, 69, 60, 52, 44,
		36, 28, 21, 16, 13, 12, 13, 16, 21, 28, 36, 44 };
int v24[24] = { 5, 20, 48, 86, 128, 171, 215, 256, 296, 318, 333, 340,
		333, 321, 293, 257, 214, 170, 127, 82, 45, 20, 5, 0 };
int h12[12] = { 61, 73, 77, 73, 61, 45, 29, 17, 13, 16, 28, 44 };
int v12[12] = { 20, 86, 171, 256, 318, 340, 318, 257, 170, 82, 20, 0 };

char *numbers12[12] = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", 
			"11", "12" };
char *numbers24[24] = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", 
			"11", "12", "13", "14", "15", "16", "17", "18", 
			"19", "20", "21", "22", "23", "24" };
char *numbers12_a[12] = { "11", "10", "9", "8", "7", "6", "5", "4", "3", 
			"2", "1", "12" };
char *numbers24_a[24] = { "23", "22", "21", "20", "19", "18", "17", "16", 
			"15", "14", "13", "12", "11", "10", "9", "8", "7", 
			"6", "5", "4", "3", "2", "1", "24" };

thymelimit[7] = { 60, 60, 24, 0, 12, 99, 7 };
monthlimit[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

unsigned char *screen, *screen0, *screen1, *font ; 
int *thyme[7];
char twentyfour=0,location='l';

int main()
{
	
	screen0=abstoptr(SCREEN0);
	screen=screen1=abstoptr(SCREEN1);
	font=abstoptr(ROMFONT);
	system("break off");
	g_mode();
#ifdef TICKTOCK
	signal (SIGINT, (void (*)(int)) sigexit);
	tick_init();
#endif
	drawclock();
	controller();
	t_mode();
	scr_clear();
	exit(0);
}

void drawclock()
{
	
	int x,y,off,cx=CENTERH,cy=CENTERV,ex=TOPH,ey=TOPV;
	long r,xr,yr,xx,yy;
	
	sw_scr(-1);
	xr=cx-ex;
	yr=cy-ey;
	r=sqroot((xr*xr)+(yr*yr))+1;
	off=(r*100)/141;
	
	gocircle(cy+(int)r-1,cx);
	gocircle(cy-(int)r+1,cx);
	
	for(x=1;x<=off;x++){
		xx=x;
		xx*=xx;
		y=sqroot(r*r-xx);
		gocircle(cy+y,cx+x);
		gocircle(cy+y,cx-x);
		gocircle(cy-y,cx+x);
		gocircle(cy-y,cx-x);
 	}
	
	gocircle(cy,cx+(int)r-1);
	gocircle(cy,cx-(int)r+1);
	
	for(y=1;y<=off;y++){
		yy=y;
		yy*=yy;
		x=sqroot(r*r-yy);
		gocircle(cy+y,cx+x);
		gocircle(cy+y,cx-x);
		gocircle(cy-y,cx+x);
		gocircle(cy-y,cx-x);
	}
	markers();
	g_write("WALLCLOCK - KITTENSOFT",340,0);
	g_write(VERSION,340,64);
	
	if (twentyfour)
		for (x=0;x<24;x++) {
			if (location=='a') 
				g_write(numbers24_a[x],v24[x],h24[x]);
			else 
				g_write(numbers24[x],v24[x],h24[x]);
		}
	else
		for (x=0;x<12;x++){
			if (location=='a')
				g_write(numbers12_a[x],v12[x],h12[x]);
			else 
				g_write(numbers12[x],v12[x],h12[x]);
		}
	menu();
	memcpy(screen1,screen0,32767);
}

void controller()
{
	int s,m,h,dh,dm,os,odm,odh,oos,oodm,oodh,q,h24;
	struct tm t;
	char meridian;
	
	thyme[0]=&(t.tm_sec);
	thyme[1]=&(t.tm_min);
	thyme[2]=&(t.tm_hour);
	thyme[3]=&(t.tm_mday);
	thyme[4]=&(t.tm_mon);
	thyme[5]=&(t.tm_year);
	thyme[6]=&(t.tm_wday);

	os=odm=odh=s=dm=dh=1;
	for(;;){
		switch (inkey()){
			case 'X':
			case 'q':
			case 'Q':
			case 'x': 
			case 3:
				return;
			case 'a': 
			case 'A':
				location='a';
				os=odm=odh=s=dm=dh=1;
				drawclock();
				break;
			case 'l':
			case 'L':
				location='l';
				os=odm=odh=s=dm=dh=1;
				drawclock();
				break;
			case 'c':
			case 'C':
				dostime(&t);
				location='c';
				os=odm=odh=s=dm=dh=1;
				drawclock();
				break;
			case 'r':
			case 'R':
				location='r';
				os=odm=odh=s=dm=dh=1;
				drawclock();
				break;
			case '\t':
				twentyfour ^=1;
				os=odm=odh=s=dm=dh=1;
				drawclock();
				break;
			case 0: break;
		}
		oos=os;
		oodm=odm;
		oodh=odh;
		os=s;
		odm=dm;
		odh=dh;
		switch (location) {
			case 'c': 
				chinatime(); 
				break;
			case 'r': 
				reagantime(&t); 
				break;
			case 'a':
			case 'l':
				do dostime(&t);
				while (os==t.tm_sec); 
				break;
		}
		sw_scr(0);
#ifdef TICKTOCK
		tick();
#endif
		s=t.tm_sec;
		m=t.tm_min;
		h=t.tm_hour;
		dh=h;
		if (twentyfour==0){		
			if (dh>11) dh-=12;
			if (h>11) meridian='P';
			else meridian='A';
			if (h>12) h-=12;
			if (h==0) h=12;
		}
		dh= dh*60+m;
		dm= m*6+s/10;
		
		hand(oos,60,LONG,0,'e');
		if (oodm!=dm) hand(oodm,360,LONG,1,'e');
		if (oodh!=dh) {
			if (twentyfour) hand(oodh,1440,SHORT,1,'e');
			else hand(oodh,720,SHORT,1,'e');
		}
		hand(s,60,LONG,0,'d');
		hand(dm,360,LONG,1,'d');
		if (twentyfour) hand(dh,1440,SHORT,1,'d');
		else hand(dh,720,SHORT,1,'d');
		writetd(s,m,h,meridian,&t);
	}
}

/* The following toggles the active screen (the one being displayed)
and also toggles the screen pointer (the one being written to) so that
the program is always displaying the screen not being written to. */

void sw_scr(q)
int q;
{
	static int scr;
	
	if (q==-1){
		screen=screen1;
		scr = 0;
		memset(screen0,0,32767);
		memset(screen1,0,32767);
		outportb(CONTROLPORT, 10 | scr);
		g_write("Please stand by...",100,30);
	}
	scr ^= 128;
	outportb(CONTROLPORT, 10 | scr);
	if (screen==screen0) screen=screen1;
	else screen=screen0;
}

#ifdef NOMATH
#include "clock.dat"

void hand(n,num,length,w,q)
int n,num,length,w,q;
{
	int v,h;
	
	switch (num) {
		case 60: n*=6; break;
		case 720: n*=2; break;
	}
	switch (length) {
		case LONG: 
			h=hlong[n];
			v=vlong[n];
			break;
		case SHORT:
			h=hshort[n];
			v=vshort[n];
			break;
	}

	if (location=='a') h=((h-CENTERH)*(-1))+CENTERH; /* Austrailia */
	
	line(h,v,CENTERH,CENTERV,q);

	if (w) {
		line(h+1,v,CENTERH+1,CENTERV,q);
		line(h,v+1,CENTERH,CENTERV+1,q);

/* The following 3 lines give the hands better bredth. But they are
very time consuming. Delete them if the Norton Utilities Sysinfo
program rates your computer at 2.5 or less, relative to an IBM-PC. 

		line(h+1,v+1,CENTERH+1,CENTERV+1,q);
		line(h-1,v,CENTERH-1,CENTERV,q);
		line(h-1,v+1,CENTERH-1,CENTERV+1,q); */
	}
}

#else

void hand(n,num,length,w,q)
int n,num,length,w,q;
{
	double angle,x,y,sin(),cos(),sweep;
	int v,h;
	
	sweep=(double)n/(double)num;
	angle= sweep *2.0*3.1415926536;  /* in radians */
	x= sin(angle)*(double)length;
	y= cos(angle)*(double)length;
	
	x*=1.5;
	h=(int)x+CENTERH;
	v=CENTERV-(int)y;

	if (location=='a') h=((h-CENTERH)*(-1))+CENTERH; /* Austrailia */
	
	line(h,v,CENTERH,CENTERV,q);
	if (w) {
		line(h+1,v,CENTERH+1,CENTERV,q);
		line(h,v+1,CENTERH,CENTERV+1,q);

/* The following 3 lines give the hands better bredth. But they are
very time consuming. Delete them if the Norton Utilities Sysinfo
program rates your computer at 2.0 or less, relative to an IBM-PC. 

		line(h+1,v+1,CENTERH+1,CENTERV+1,q);
		line(h-1,v,CENTERH-1,CENTERV,q);
		line(h-1,v+1,CENTERH-1,CENTERV+1,q); */
	}
}

#endif

void markers()
{
	double angle,x,y,x2,y2,sin(),cos(),sweep;
	int v,h,v2,h2,i;
	
	if (twentyfour) sweep=0.2617994;
	else sweep=0.5235988;
	for (angle=0.0;angle<6.2;angle+=sweep){
		x=sin(angle) * (LONG+7) * 1.5;
		y=cos(angle) * (LONG+7);
		x2=sin(angle) * (LONG+17) * 1.5;
		y2=cos(angle) * (LONG+17);
		h=(int)x+CENTERH;
		h2=(int)x2+CENTERH;
		v=CENTERV-(int)y;
		v2=CENTERV-(int)y2;
	
		line(h,v,h2,v2,'d');
	}
	for (angle=0.0,i=0;angle<6.18;angle+=0.10472){
		if ((i++)%5==0) continue;
		x=sin(angle) * (LONG+10) * 1.5;
		y=cos(angle) * (LONG+10);
		h=(int)x+CENTERH;
		v=CENTERV-(int)y;
	
		writedot(h,v);
	}
}

void line(h1,v1,h2,v2,m)
int h1,v1,m;
{
	long int i,x,y,x1,y1,x2,y2,xd,yd,xstepdir,ystepdir,stepsize;
	int h,v;
	
	x1=h1;
	x2=h2;
	y1=v1;
	y2=v2;
	xd=x2-x1;
	yd=y2-y1;
	if ((!(xd))&&(!(yd))) return;

	if (xd>0l) xstepdir=1l;
	else xstepdir=-1l;
	if (yd>0l) ystepdir=1l;
	else ystepdir=-1l;

	if ((xd*xd)>(yd*yd)){
		stepsize=(yd<<16)/xd;
		if (stepsize<0) stepsize*=-1;
		for(i=0,x=x1;x!=x2;i++,x+=xstepdir){
			y=y1+ystepdir*((stepsize*i)>>16);
			h=x;
			v=y;
			if (m=='d') writedot(h,v);
			else cleardot(h,v);
		}
	}
	else {
		stepsize=(xd<<16)/yd;
		if (stepsize<0) stepsize*=-1;
		for(i=0,y=y1;y!=y2;i++,y+=ystepdir){
			x=x1+xstepdir*((stepsize*i)>>16);
			h=x;
			v=y;
			if (m=='d') writedot(h,v);
			else cleardot(h,v);
		}
	}
	if (m=='d') writedot(CENTERH,CENTERV);
	else cleardot(CENTERH,CENTERV);
}

/* The following must cause a one-second pause. Check your compiler
documentation for the function clock(), and re-write this if necessary. */

void g_sleep()
{
	long a,clock();

	a=clock();
	while ( clock()-a < 100)
		;
}

void g_mode()
{
	int i;
	
	outportb(CONFIGPORT,3);
	outportb(CONTROLPORT,0);
	for(i=0;i<12;i++){
		outportb(INDEXPORT,i);
		outportb(DATAPORT,gdata[i]);
	}
	outportb(CONTROLPORT,2);
	g_sleep();
	memset(screen0,0,65535);
	outportb(CONTROLPORT,10);
}
void t_mode()
{
	int i;
	
	outportb(CONTROLPORT,0);
	for(i=0;i<12;i++){
		outportb(INDEXPORT,i);
		outportb(DATAPORT,tdata[i]);
	}
	g_sleep();
	outportb(CONTROLPORT,40);
}

/* The following are the "easy to read" write and clear dot functions. 
They are not used, however, in the interest of speed. The versions actually
used are logically the same but avoid the use of temporary variables.

void writedot(x,y)
int x,y;
{
	unsigned off;
	unsigned char c,bit;
	
	off=(unsigned)( 0x2000 * (y%4) + 90*(y/4) + x/8 );
	c=screen[off];
	bit = 1<< (7-x%8);
	c |= bit;
	screen[off]=c;
}
void cleardot(x,y)
int x,y;
{
	unsigned off;
	unsigned char c,bit;
	
	off=(unsigned)( 0x2000 * (y%4) + 90*(y/4) + x/8 );
	c=screen[off];
	bit = (~(1<< (7-x%8)));
	c &= bit;
	screen[off]=c;
}
void g_putch(c,h,v)
int c,h,v;
{
	int i;
	unsigned char scan_line;
	unsigned off;
	
	for (i=0;i<8;i++,v++){
		scan_line=font[c*8+i];
		off=(unsigned)( 0x2000 * (v%4) + 90*(v/4) + h );
		screen[off]= scan_line;
	}
}
 */

void writedot(x,y)
int x,y;
{
	
	screen[0x2000*(y%4)+90*(y/4)+x/8] |= (1<<(7-x%8));
}
void cleardot(x,y)
int x,y;
{
	
	screen[0x2000*(y%4)+90*(y/4)+x/8] &= (~(1<<(7-x%8)));
}

void g_putch(c,h,v)
int c,h,v;
{
	int i;
	
	for (i=0;i<8;i++,v++)
		screen[0x2000*(v%4)+90*(v/4)+h]= font[c*8+i];
}

void g_write(s,v,h)
char *s;
int h,v;
{
	
	while (*s) g_putch(*s++,h++,v);
}
void writetd(s,m,h,meridian,t)
int s,m,h,meridian;
struct tm *t;
{
	char buf[25],*wp;
	int i=0;
	
	if (h>=10) buf[i++]=h/10+48;
	buf[i++]= h%10+48;
	buf[i++]= ':';
	buf[i++]= m/10+48;
	buf[i++]= m%10+48;
	buf[i++]= ':';
	buf[i++]= s/10+48;
	buf[i++]= s%10+48;
	buf[i++]= ' ';
	if (twentyfour==0){
		buf[i++]= ' ';
		buf[i++]= meridian;
		buf[i++]= 'M';
		buf[i++]= ' ';
	}
	buf[i++]= 0;
	g_write(buf,36,1);

	g_write(day[t->tm_wday],0,1);
	strcpy(buf,month[t->tm_mon]);
	wp=buf;
	while (*wp!=' ') wp++;
	wp++;
	if (t->tm_mday>=10){
		(*wp++)=t->tm_mday/10+48;
		(*wp++)=t->tm_mday%10+48;
	}
	else (*wp++)=t->tm_mday+48;
	*wp=0;
	strcat(buf,"          ");
	
	g_write(buf,12,1);
	g_putch('1',1,24);
	g_putch('9',2,24);
	g_putch(t->tm_year/10+48,3,24);
	g_putch(t->tm_year%10+48,4,24);
}
void gocircle(y,x)
int y,x;
{
	x-=CENTERH;
	x+= (x/2);
	x+=CENTERH;
	
	writedot(x,y);
}
int sqroot(n)
long n;
{
	long r1,r2;
	r1=n;
	r2=1l;

	while (r1>r2){
		r1=(r1+r2)/2;
		r2=n/r1;
	}
	return((int)r1);
}

/* The following increments the time by one second. It is used only
by chinatime(), which begins with the real time but then moves
forward at a somewhat random pace. */

void stepclock()
{
	int i;
	
	(*thyme[0])++;
	for (i=0;i<6;i++){
		if ( *thyme[i]==thymelimit[i] ) {
			if (i==2){
				(*thyme[6])++;
				if (*thyme[6]==7) *thyme[6]=0;
			}
			if ((i==3) && (*thyme[i]!=monthlimit[*thyme[i]]))
				continue;
			*thyme[i]=0;
			if (i==3) *thyme[i]=1;
			if (i==5) break;
			(*thyme[i+1])++;
		}
	}
}

/* The following function is intended to sleep a random length of
time between 1/4 second and 1 1/4 seconds, more or less, to give an
uneven movement of the clock and simulate an inaccurate water clock.
This is intended as a joke. Water clocks are actually much more
accurate than this. Depending on the action of clock(), which varies
from one compiler to the next, you may have to revise this function. */

void chinatime()
{
	long a,clock();
	int delay;

	delay= rando()%100;
	if (delay<50) delay/=2;
	if (delay>75) delay+=50; 
	a=clock();  
	while( clock()-a < delay )
		;
	stepclock();

}
void reagantime(t)
struct tm *t;
{
	long a;clock();
	
	a=clock();
	while ( clock()-a <200 ) ;
	
	t->tm_sec = rando()%60;
	t->tm_min = rando()%60;
	t->tm_hour = rando()%24;
	t->tm_mday = rando()%40+1;
	t->tm_mon = rando()%12;
	t->tm_year = rando()%100;
	t->tm_wday = rando()%7;
}

/* A rudimentary but useable pseudo-random integer generator: */

int rando()
{
	static long zi;
	static int seed=1;
	
	struct tm T;
	if (seed) {
		seed=0;
		dostime(&T);
		zi=(long)T.tm_hsec*(long)T.tm_sec+(long)T.tm_hour*(long)T.tm_min;
	}
	zi*=23;
	zi+=17;
	zi%=32749;
	return((int)zi);
}

char *menutext[4][8] = {
{
"        Local time.",
"",
"",
       "Press your choice:",
       "(A)ustralian time.",
       "(C)hinese time.",
       "(R)eagan time.",
       "TAB: 24-hr format." },
      
{
"Chinese time. In China",
"the use of water clocks",
"makes time less accurate.",
       "Press your choice:",
       "(A)ustralian time.",
       "(L)ocal time.",
       "(R)eagan time.",
       "TAB: 24-hr format." },

{
"  Australian time. Due to",
"  the Coriolis force,",
"  clocks run backwards.",
       "Press your choice:",
       "(L)ocal time.",
       "(C)hinese time.",
       "(R)eagan time.",
       "TAB: 24-hr format." },

{
"   The time according to",
"   Ronald Reagan. He gets",
"   confused sometimes.",
       "Press your choice:",
       "(A)ustralian time.",
       "(C)hinese time.",
       "(L)ocal time.",
       "TAB: 24-hr format." }
};

void menu()
{
	int i,m,j;
	
	switch(location){
		case 'a': m=2; j=528; break;
		case 'c': m=1; j=512; break;
		case 'l': m=0; j=568; break;
		case 'r': m=3; j=536; break;
	}
	
	for (i=0;i<3;i++) g_write(menutext[m][i],i*10,65);
	for (i=3;i<8;i++) g_write(menutext[m][i],(i)*10,72);  
	if (twentyfour) g_write("12",70,77);
	g_write("X - Exit.",80,81);
	
	line(j,0,j,30,'d');
	line(j,30,568,30,'d');
	line(568,30,568,80,'d');
	line(568,80,640,80,'d');
	line(640,80,640,90,'d');
	line(640,90,719,90,'d');
}

/* The following function may be compiler-dependent. Alas that
C function libraries do not have a standardized inkey() function! */

int inkey()
{
	
	return(bdos(0x600,0xff,0));
}

/* If your compiler does not have abstoptr you can use this one: */

void *abstoptr(abs)
unsigned long abs;
{
	unsigned long h,l;
	
	l=abs&15L;
	h=abs>>4;
	h<<=16;
	return ( (void far *)(h|l) );
}

#ifdef TICKTOCK

int on_sound,off_sound;

void sigexit(q)
int q;
{
	t_mode();
	scr_clear();
	exit(0);
}

void tick()
{
	long adj;
	
	soundon();
	soundoff();
}
void soundon()
{
	int temp;
	
	outportb(0x43,0xb6);
	outportb(0x42, 1000 & 0xff);
	outportb(0x42, 1000 >> 8);
	
	outportb(0x61,on_sound);
}
void soundoff()
{
	int temp;
	
	outportb(0x61,off_sound);
}
void tick_init()
{

	off_sound = inportb(0x61);
	on_sound = off_sound | 3;
}
#endif
