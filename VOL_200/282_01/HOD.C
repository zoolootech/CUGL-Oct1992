/*
TITLE:		HODGEPODGE;
DATE:		8/30/88;
DESCRIPTION:	"Makes waves.";
VERSION:	1.01;
KEYWORDS:	HODGEPODGE, Waves;
FILENAME:	HOD.C;
WARNINGS:	"Requires Hercules monochrome graphics. Hard disk recommended.";
SEE-ALSO:	HOD.DOC;
SYSTEM:		MS-DOS;
COMPILERS:	Aztec;
AUTHORS:	Dan Schechter;
REFERENCES:
	AUTHORS:	A.K. Dewdney;
	TITLE:		"Computer Recreations";
	CITATION:	"Scientific American, 259, pp 104-107 (August 1988)."
ENDREF;
 */

#define VERSION "Version 1.01 8/30/88"

/* Notes to programmers:

	This program compiles under Aztec C in large-code/large-data
	model. Some functions are used that may not exist or may have 
	other names in the libraries of other compilers.
	
	scr_getc() causes the program to halt and wait for a key to be
	pressed. It then returns that key. Functions keys and arrow keys
	which DOS returns as a null followed by a character are returned
	as a single character with the high bit set. But that feature
	is not used in this program. It differs from getchar() in that
	it does not wait for the ENTER key to be pressed. Some compilers
	call this function getch().
	
	abstoptr() takes an absolute address in the form of a long
	and returns a long pointer in segment:offset form, which can
	then be used to directly access memory.
	
	time() returns the DOS time and date packed into a long. In
	this program it is used only to seed the random number generator
	and to provide a little extra mixing "exercise" for it. 
	
	clock() returns the number of hundredths of a second since the
	beginning of the present day. In this program it is used only
	to provide setmode() with a one-second pause, needed when
	re-programming the HGA. The return type is a long.
	
	If the argument to either time() or clock() is non-zero, the return
	value will also be placed in the long pointed at by the argument.
	
	bdos() performs INT 21H. AH is set to its first parameter, DX is
	set to its second parameter, and CX is set to its third parameter.
	It returns the value in AL. In this program it is used as an
	inkey() function. inkey() polls the keyboard and returns 0 if
	no key has been pressed, otherwise it returns the value of the
	key that was pressed. It differs from scr_getc() in that inkey()
	does not cause program operation to stop if no key has been pressed.
	
	access() determines the accessibility of a disk file. In this
	program it is only used to check the existance of a file, and as
	called by this program it returns 0 if the named file does not
	exist, and non-zero otherwise.

		The first 7 #defines are user-changeable. 
		HEIGHT should not be greater than 80 and WIDTH
		should not be greater than 180, to fit in the
		screen area. Making them smaller will result in
		faster operation. Set MAXFRAME smaller if you have
		limited disk space. If HEIGHT==80 and WIDTH==180
		and MAXFRAME==200 then a full length movie will
		occupy 1440000 bytes of disk space. A good archiving
		program can squeeze that by perhaps half.
		
		K1, K2, and G are the defaults for k1, k2, and g
		respectively. They can be changed at compile time
		or set at run time on the command line. They 
		determine the characteristics of the hodgepodge.
		Likewise for RANDOM, a Kittensoft innovation.
	
	If you compile this program with TABLETEST defined, it will
	display the halftone graphics characters as defined in table.
	It will then wait for you to press any key and then exit.
	Use this feature to view the characters if you want to change
	them.

		Exit codes:
			0. Normal termination.
			1. File load failure.
			2. Unrecognized command line option.
			3. Invalid command line parameter value.
 */
#include <string.h>
#include <fcntl.h>

#define HEIGHT 80	/* Height of display in 4 X 6 pixel characters. */
#define WIDTH 120	/* Width of display in 4 X 6 pixel characters. */
#define MAXFRAME 200	/* Maximum number of frames in a movie to prevent 
			over-filling your disk. */
#define K1 2		/* Default. May be changed at run time. */
#define K2 3		/* Default. May be changed at run time. */
#define G 25		/* Default. May be changed at run time. */
#define RANDOM 0	/* Default. May be changed at run time. */

#define STATENUM 128	/* The number of states. Since the state number
			is divided by 8 to arrive at a display number
			and there are 16 display characters, and since
			disk storage of movies uses one nibble per cell
			STATENUM should not be changed without revising
			the entire display and movie storage schemes. */

#define TEXT 0		/* Used for Hercules display. */
#define GRAPHICS 1		/* Used for Hercules display. */
#define INDEXPORT 0x3B4		/* Used for Hercules display. */
#define CONTROLPORT 0x3b8	/* Used for Hercules display. */
#define DATAPORT 0x3b5		/* Used for Hercules display. */
#define CONFIGPORT 0x3bf	/* Used for Hercules display. */
#define G_DELAY 100		/* Used for Hercules display. */
#define FONTADDR 0xffa6e	/* ROM font address. */
#define SCREEN 0xb0000		/* Hercules screen 0 address. */
#define SCREEN1 0xb8000		/* Hercules screen 1 address. */

#define HEALTHY 1
#define INFECTED 0
#define ILL -1

long time(long *);
void *abstoptr();
int main(int n,char **arg);
void table_init(void);
void screen_put4c(int c1,int c2,int c3, int c4, int v,int h);
int work(unsigned char o[HEIGHT][WIDTH]);
void screenwrite(unsigned char old[HEIGHT][WIDTH],int count);
void sleep(int n);
int rando(void);
int inkey(void);
int infected_neighbors(unsigned char a[HEIGHT][WIDTH],int v,int h);
int ill_neighbors(unsigned char a[HEIGHT][WIDTH],int v,int h);
int n_sum(unsigned char a[HEIGHT][WIDTH],int v,int h);
int health(int a);
void setmode(int q);
int clearscreen(void);
int g_putch(int c,int y);
char *itoa(int n);
int blank(void);
void newname(char *s,int q);
int playback(char *s);
void sw_scr(void);
void g_printf(int v,char *ctrl,...);
int _kdput(char *p,int v);
void erl(void);
int load(unsigned char old[HEIGHT][WIDTH], unsigned char datbuf[HEIGHT*WIDTH], int *count, char *s);
void prompt(void);

int gdata[12] = { 		/* Constants for programming Hercules card. */
	0x35, 0x2d, 0x2e, 0x7, 0x5b, 
	0x2, 0x57, 0x57, 0x2, 0x3, 0x0, 0x0 
};
int tdata[12] = { 
	0x61, 0x50, 0x52, 0xf, 0x19, 
	0x6, 0x19, 0x19, 0x2, 0xd, 0xb, 0xc 
};
unsigned char table[16][4] = {
/*0*/	{ 0,0,0,0 },		/* This table defines the characters	    */
/*1*/	{ 0,8,0,0 },		/* that will represent the states. Actual   */
/*3*/	{ 16,2,8,0 },		/* state numbers are divided by 8,	    */
/*4*/	{ 8,1,16,2 },		/* requiring 16 display characters.	    */
/*6*/	{ 40,2,5,16 },		/* Integers in this table must not be	    */
/*7*/	{ 40,18,5,16 },		/* greater than 63. table_init() will	    */
/*9*/	{ 36,18,10,37 },	/* shift all these numbers to create tables */
/*10*/	{ 42,20,42,17 },	/* 1, 2, 3, and 4 so that             	    */
/*12*/	{ 42,21,42,21 },	/* characters may be quickly OR'd together  */
/*13*/	{ 42,46,42,42 },	/* and written to the display in full       */
/*15*/	{ 43,42,43,43 },	/* bytes. You may alter this table	    */
/*16*/	{ 43,53,45,21 },	/* to change the appearance of the display  */
/*18*/	{ 47,53,43,61 },	/* without otherwise affecting program	    */
/*20*/	{ 47,61,47,61 },	/* operation.				    */
/*22*/	{ 63,61,47,63 },
/*24*/	{ 63,63,63,63 }
};

#define CAT "\n\040\040\040\040\057\134\137\057\134\n\040\040\040\050\376\040\322\040\376\051\n\360\360\360\360\040\304\312\304\040\360\360\360\360\n\040\040\040\040\040\042\042\042"

char *HELP = "\
USAGE: HOD [A<k1>] [B<k2>] [G<g>] [M<n>] [I<nn>] [P[<fn>]] [R[<fn>]]\n\
Parameters may be in any order. All are integers.\n\
Defaults: k1=2, k2=3, g=25.\n\n\
<k1> is infected cells divisor.\n\
<k2> is ill cells divisor.\n\
<g> is infection rate.\n\
<nn> is an alternate initializing factor. (1 to number of cells.)\n\
<n> is a randomizing factor. (1 to number of cells.)\n\n\
R tells the program you want to load a file.\n\
P tells the program you want to play back a movie.\n\
<fn> is a file name to load or play back.\n\n\
During program operation:\n\
# (crosshatch) Toggles movie recording.\n\
$ to set movie stop frame.\n\
^W Writes the current data to a file.\n\
^D Writes the data to a file and then exits.\n\
^R Reads data from a file.\n\
^B Toggles screen blanking.\n\
^S Pause.\n\
^C Exits from the program.";

char *PRESS_ANY_KEY = "\
                                               PRESS ANY KEY...";

unsigned char table1[16][4],table2[16][4],table3[16][4],table4[16][4],table5[16][4];
int k1=K1, k2=K2, g=G, random=RANDOM;
int g_th,movie,stopframe;
unsigned char *font, *screen, *screen1, *st;

int main(n,arg)
int n;
char **arg;
{
	int i,j,count=0,quit=0;
	unsigned char old[HEIGHT][WIDTH],dat;
	unsigned char datbuf[HEIGHT*WIDTH];
	int fd,mfd,datcount,frame;
	char fn[250];
	
	table_init();
#ifdef TABLETEST
	for (i=0;i<WIDTH;i++) for (j=0;j<HEIGHT;j++) old[j][i]=0;
	for (j=0;j<HEIGHT;j++) old[j][0]=STATENUM-1;
	for (j=0;j<16;j++){
		old[j*4][2]=j*8;
		for(i=10;i<20;i++) 
			old[j*4][i]=old[j*4+1][i]=old[j*4+2][i]=j*8;
	}
#else
	for (i=0;i<WIDTH;i++) for (j=0;j<HEIGHT;j++) 
		old[j][i]=rando()%STATENUM;
#endif
	st=screen=abstoptr(SCREEN);
	screen1=abstoptr(SCREEN1);
	font=abstoptr(FONTADDR);
	while (n>1) {
		n--;
		switch(tlr(arg[n][0])){
			case 'a':
				k1= atoi(arg[n]+1);
				break;
			case 'b':
				k2= atoi(arg[n]+1);
				break;
			case 'g':
				g= atoi(arg[n]+1);
				break;
			case 'm':
				random= atoi(arg[n]+1);
				break;
			case 'r':
				if (load(old,datbuf,&count,arg[n]+1)==-1){
					puts("Load failed.");
					exitt(1);
				}
				break;
			case 'i':
				for (i=0;i<WIDTH;i++) for (j=0;j<HEIGHT;j++) 
					old[j][i]=0;
				i= atoi(arg[n]+1);
				while(i--){
					j= rando()%HEIGHT;
					i= rando()%WIDTH;
					old[j][i]=rando()%STATENUM;
				}
				break;
			case 'p':
				exitt (playback(arg[n]+1));
			default:
				puts(HELP);
				exit(2);
		}
	}
	if ((k1<=0)||(k2<=0)||(g<0)||(random<0)) {
		puts(HELP);
		exit(3);
	}
	setmode(GRAPHICS);
	prompt();
	for(screenwrite(old,count);;){
		switch (work(old)){
			case '?':
				  setmode(TEXT);
				  puts(HELP);
				  puts(PRESS_ANY_KEY);
				  scr_getc();
				  setmode(GRAPHICS);
				  prompt();
				  continue;
			case '#':
				  movie ^= 1;
				  if (!movie) close(mfd);
				  else {
				  	frame=0;
				  	newname(fn,'m');
					if ((mfd=open(fn,O_WRONLY|O_TRUNC))==-1)
				  		movie=0;
				  	else {
						write(mfd,&count,sizeof(int));
						write(mfd,&k1,sizeof(int));
						write(mfd,&k2,sizeof(int));
						write(mfd,&g ,sizeof(int));
						write(mfd,&random,sizeof(int));
					}
				  }
				  continue;  
			case '$':
				  setmode(TEXT);
				  puts("Enter movie stop frame:");
				  gets(fn);
				  i= atoi(fn);
				  if (i>frame) stopframe=i;
				  setmode(GRAPHICS);
				  prompt();
				  continue;
	/* ^R */	case 18:
				  setmode(TEXT);
				  if (load(old,datbuf,&count,"")==-1){
				  	puts("Load failed. Press any key.");
				  	if (scr_getc()==3) exitt(1);
				  }
				  setmode(GRAPHICS);
				  prompt();
				  continue; 
	/* ^D */	case 4:
				  quit=2;
	/* ^W */	case 23:
				  newname(fn,0);
				  if ((fd=open(fn,O_WRONLY | O_TRUNC))!=-1){
				  	for(i=datcount=0;i<WIDTH;i++)
				  		for(j=0;j<HEIGHT;j++)
				  			datbuf[datcount++]=old[j][i];
				  	write(fd,&count,sizeof(int));
				  	write(fd,&k1,sizeof(int));
				  	write(fd,&k2,sizeof(int));
				  	write(fd,&g,sizeof(int));
				  	write(fd,&random,sizeof(int));
				  	write(fd,datbuf,HEIGHT*WIDTH);
				  	close(fd);
				  	g_printf(340,"  Data saved. ");
				  } 
				  else {
				  	g_printf(340,"  File failure. Data not saved. ");
				  	quit=0;
				  }
				  if (quit) break;
				  continue;
	/* ^C */	case 3:
				  quit=1;
				  break;
			case 0:
				  break;
			default:
				  continue;
		}
		if (quit) break;
		screenwrite(old,++count);
		if (movie) {
			for(i=datcount=0;i<WIDTH;i+=4) for(j=0;j<HEIGHT;j++) {
	datbuf[datcount++]=((old[j][i]>>3)<<4) | (old[j][i+1]>>3); 
	datbuf[datcount++]=((old[j][i+2]>>3)<<4) | (old[j][i+3]>>3); 
			}
			if ((write(mfd,datbuf,WIDTH*HEIGHT/2))!=WIDTH*HEIGHT/2) {
				close(mfd);
				movie=g_th=0;
				g_printf(328,"Movie file write error or disk full. ");
			}
			else {
				g_printf(340,"Movie frame %d ",frame++);
				if ((frame==MAXFRAME)||(frame==stopframe)){
					close(mfd);
					movie=0;
				}
			}
		} 
	}
	setmode(TEXT);
	if (quit==2) puts("Data saved.\n");
	exitt(0);
}
void table_init()
{
	int i,j;
	
	for (i=0;i<16;i++) for(j=0;j<4;j++) {
		table1[i][j]= table[i][j]<<2;
		table2[i][j]= table[i][j]>>4;
		table3[i][j]= table[i][j]<<4;
		table4[i][j]= table[i][j]>>2;
		table5[i][j]= table[i][j]<<6;
	}
}

int work(o)
unsigned char o[HEIGHT][WIDTH];
{
	int i,j,k;
	unsigned char n[HEIGHT][WIDTH];
	
	for (i=0;i<WIDTH;i++){
		if (!(i&7)) {
			g_putch('.',340);
			switch (k=inkey()) {
	/* ^S */		case 19:
					g_printf(340,"  Paused... ");
					scr_getc();
					break;
	/* ^B */		case 2:
					blank();
					break;
				case 0:
					break;
				default:
					return k;
			}
		}
		for (j=0;j<HEIGHT;j++)
			switch (health(o[j][i])){
				case HEALTHY:
					k= infected_neighbors(o,j,i)/k1 + ill_neighbors(o,j,i)/k2;
					n[j][i]=k;
					break;
				case INFECTED:
					k= (infected_neighbors(o,j,i)+ill_neighbors(o,j,i));
					if (k) k=n_sum(o,j,i)/k +g;
					else k=10000;
					n[j][i]= (k>=STATENUM)? STATENUM-1 : k;
					break;
				case ILL:
					n[j][i]=0;
					break;
			}
	}
	if (random){
		for(k= (int)(time((long *)0)&7L)   ;k;k--)
			rando();	/* Exercise the random number generator. */
		for(k=random;k;k--){
			j= rando()%HEIGHT;
			i= rando()%WIDTH;
			n[j][i]=rando()%STATENUM;
		}
	}
	memcpy(o,n,HEIGHT*WIDTH);
	return 0;
}
void screenwrite(old,count)
unsigned char old[HEIGHT][WIDTH];
int count;
{
	int i,j;
	
	for (i=0;i<WIDTH;i+=4) for (j=0;j<HEIGHT;j++) 
		screen_put4c(old[j][i]>>3,old[j][i+1]>>3,old[j][i+2]>>3,old[j][i+3]>>3,j*4,i/4);
#ifdef TABLETEST
	scr_getc();
	setmode(TEXT);
	exit(0);
#endif
	g_printf(340,"\rTime=%d ",count);
}
void sleep(n)
int n;
{
	long clock(),a;
	
	a=clock();
	while( clock()-a< n)
		;
}
int rando()		/* This is a quick-and-dirty but acceptable
			pseudo-random generator for non-critical use. 
			Be wary of using it where truely random
			numbers are needed. */
{
	static long zi;
	static int mark=1;
	
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
int inkey()
{
	
	return(bdos(0x600,0xff,0));
}
int infected_neighbors(a,v,h)
int v,h;
unsigned char a[HEIGHT][WIDTH];
{
	int i,j,n=0;
	
	for(i=h-1;i<=h+1;i++){
		if ((i<0)||(i>=WIDTH)) continue;
		for (j=v-1;j<=v+1;j++){
			if ((i==h)&&(j==v)) continue;
			if ((j<0)||(j>=HEIGHT)) continue;
			if ((a[j][i]>0)&&(a[j][i]!=STATENUM-1)) n++;
		}
	}
	return n;
}
int ill_neighbors(a,v,h)
int v,h;
unsigned char a[HEIGHT][WIDTH];
{
	int i,j,n=0;
	
	for(i=h-1;i<=h+1;i++){
		if ((i<0)||(i>=WIDTH)) continue;
		for (j=v-1;j<=v+1;j++){
			if ((i==h)&&(j==v)) continue;
			if ((j<0)||(j>=HEIGHT)) continue;
			if (a[j][i]==STATENUM-1) n++;
		}
	}
	return n;
}

int n_sum(a,v,h)
int v,h;
unsigned char a[HEIGHT][WIDTH];
{
	int i,j,n=0;
	
	for(i=h-1;i<=h+1;i++){
		if ((i<0)||(i>=WIDTH)) continue;
		for (j=v-1;j<=v+1;j++){
			if ((j<0)||(j>=HEIGHT)) continue;
			n += a[j][i];
		}
	}
	return n;
}
int health(a)
int a;
{
	if (a==0) return HEALTHY;
	if (a==STATENUM-1) return ILL;
	return INFECTED;
}
char *itoa(n)
int n;
{
	static char s[25];
	int i=0;
	
	s[i++]= n/10000+48;
	s[i++]= (n/1000)%10+48;
	s[i++]= (n/100)%10+48;
	s[i++]= (n/10)%10+48;
	s[i++]= n%10+48;
	s[i]=0;
	return s;
}
void newname(s,q)
char *s;
int q;
{
	int fnum,t;
	
	t=g_th;
	g_printf(17,"\r                         ");
	g_printf(33,"\r                         ");
	g_printf(25,"\r   Save to what drive?   ");
	g_th-=2;
	if (q=='m') strcpy(s,"X:\\HOD..MAT");
	else strcpy(s,"X:\\HOD..DAT");
	s[0]=g_putch(scr_getc(),25);
	for (fnum='A';fnum<='Z';fnum++){
		s[6]=fnum;
		if (access(s,0)) break;
	}
	g_th=t;
}
int playback(s)
char *s;
{
	int i,j,fd,count,frame=0,bufcount;
	unsigned char mbuf[HEIGHT*WIDTH/2];
	char fn[250];
	
	if (!s[0]) {
		puts("Playback file name?");
		gets(fn);
	}
	else strcpy(fn,s);
	
	if (!strchr(fn,'.')) strcat(fn,".MAT");
	if ((fd=open(fn,O_RDONLY))==-1) return 2;
	
	setmode(GRAPHICS);
	read(fd,&count,sizeof(int));
	read(fd,&k1,sizeof(int));
	read(fd,&k2,sizeof(int));
	read(fd,&g ,sizeof(int));
	read(fd,&random,sizeof(int));
	sw_scr();
	prompt();
	sw_scr();
	prompt();
	while (read(fd,mbuf,HEIGHT*WIDTH/2)==HEIGHT*WIDTH/2){
		sw_scr();
		switch (inkey()){
	/* ^C */	case 3:
				setmode(TEXT);
				return 0;
	/* ^S */	case 19:
				scr_getc();
				break;
			case '?':
				setmode(TEXT);
				puts(HELP);
				puts(PRESS_ANY_KEY);
				scr_getc();
				setmode(GRAPHICS);
				break;
			case 0:	
				break;
		}
		for (i=bufcount=0;i<WIDTH/4;i++) 
			for (j=0;j<HEIGHT;j++,bufcount+=2) 
	screen_put4c(mbuf[bufcount]>>4,mbuf[bufcount]&15,mbuf[bufcount+1]>>4,mbuf[bufcount+1]&15,j*4,i);
		g_th=0;
		g_printf(340,"Time=%d    Movie frame=%d         ",count++,frame++);
	}
	setmode(TEXT);
	return 0;
}

/* The following extremely abridged version of printf() lets me use
the %d conversion without all the overhead of the real thing. */

void g_printf(v,ctrl,parml)
char *ctrl;
int v;
unsigned parml;
{
	char *p;
	
	p= (char *)(&parml);
	while (*ctrl) {
		if ((*ctrl)=='%') {
			ctrl+=2;
			_kdput(p,v);
			p += sizeof(int);
			continue;
		}
		if (*ctrl=='\r'){
			g_th=0;
			erl(); 
			ctrl++;
			continue;
		}
		g_putch( *(ctrl++),v);
	}
}
_kdput(p,v)
char *p;
int v;
{
	int t,n,mask;
	
	n= *((int *)p);
	if (n<0){
		g_putch('-',v);
		n = -n;
	}
	for(mask = 10000; mask!=1; mask/=10)
		if (mask<=n) break;
	for(;mask;mask/=10){
		t = n/mask;
		n -= t*mask;
		g_putch(t+48,v);
	}
}
int load(old,datbuf,count,s)
unsigned char old[HEIGHT][WIDTH], datbuf[HEIGHT*WIDTH];
int *count;
char *s;
{
	int i,j,datcount,fd;
	char fn[250];
	
	if (!s[0]) {
		puts("Load file name?");
		gets(fn);
	}
	else strcpy(fn,s);
	if (!strchr(fn,'.')) strcat(fn,".DAT");
	if ((fn[0])&&((fd=open(fn,O_RDONLY))==-1)) return -1;
	else {
		read(fd,count,sizeof(int));
		read(fd,&k1,sizeof(int));
		read(fd,&k2,sizeof(int));
		read(fd,&g,sizeof(int));
		read(fd,&random,sizeof(int));
		read(fd,datbuf,HEIGHT*WIDTH);
		close(fd);
		for(i=datcount=0;i<WIDTH;i++)
			for(j=0;j<HEIGHT;j++)
				old[j][i]=datbuf[datcount++];
	}
}
exitt(e)
int e;
{
	puts("\
The Hodgepodge machine\n\
from Scientific American August 1988, p. 104, by A.K. Dewdney.\n\n\
This version by Dan Schechter.\n\
Public domain software from Kittensoft. " VERSION CAT);

	exit(e);
}
int tlr(c)
int c;
{
	if (c>='A') c |= 32;
	return c;
}
void prompt()
{
	g_printf(328,"\rPress ^C to exit. Press ? for help.   k1=%d k2=%d g=%d",k1,k2,g); 
	if (stopframe) g_printf(328," s=%d",stopframe);
	if (random) g_printf(328," r=%d",random);
}

/* All the rest of the functions are Hercules-dependent. */

/* The following function erases the bottom line by writing directly to
screen memory. It affects only page 0. It is not elegant, but it is
faster than using g_printf() with a long line of spaces. */

void erl()
{
	int i;
	static unsigned off[8] = {
		0x1de2,0x3de2,0x5de2,0x7de2,0x1e3c,0x3e3c,0x5e3c,0x7e3c };
	
	for(i=0;i<8;i++) memset(screen+off[i],0,90);
}

/* The following function simultaneously switches the screen that is
on display and the screen that is being written to by all the
direct-access screen functions. It is used only by the playback() function
to give an effect more like animation by assuring that the screen being
written to is not the one being viewed. */

void sw_scr()
{
	static int scr_num;
	
	scr_num^=128;
	outportb(CONTROLPORT, 10 | scr_num);
	if (scr_num) screen=st;
	else screen=screen1;
}

/* The following function toggles the screen display on and off. I like
to allow the user to blank the screen so as to avoid phosphor burn. */

blank()
{
	static blank=0;
	
	blank ^= 1;
	if (blank) outportb(CONTROLPORT,2);
	else outportb(CONTROLPORT,10);
}

/* The following function sets the HGA to graphics or text mode, as needed. */

void setmode(q)
int q;
{
	int i;
	
	switch (q) {
		case GRAPHICS:
				outportb(CONFIGPORT,3);
				outportb(CONTROLPORT,2);
				for(i=0;i<12;i++){
					outportb(INDEXPORT,i);
					outportb(DATAPORT,gdata[i]);
				}
				sleep(G_DELAY);
				clearscreen();  
				outportb(CONTROLPORT,10);
				break;
		case TEXT:
				outportb(CONTROLPORT,0);
				for(i=0;i<12;i++){
					outportb(INDEXPORT,i);
					outportb(DATAPORT,tdata[i]);
				}
				sleep(G_DELAY);
				scr_clear();
				outportb(CONTROLPORT,40);
				outportb(CONFIGPORT,0);
				break;
	}
}

/* The following function clears the two Hercules graphics pages. */

clearscreen()
{
	memset(screen,0,(unsigned)32768);
	memset(screen1,0,(unsigned)32768);
}

/* The following function writes a character to the graphics screen.
It uses the IBM-ROM character set. The horizontal character position
is set by the global variable g_th. y is the vertical scan line of the
top of the character. c is the character to be written. */

int g_putch(c,y)
int c,y;
{
	int i;
	
	for (i=0;i<8;i++,y++)
		screen[( 0x2000 * (y&3) + 90*(y>>2) + g_th)] = font[c*8+i];
	g_th++;
	return(c);
	
}

/* The following function writes 4 half-tone graphics characters to screen
memory. The actual character set is defined in the array named table at
the beginning of the program. table1, table2, table3, table4, and table5
are created in table_init() by appropriate bit-shifting. Since the characters
are 4 pixels high and 6 pixels wide, to achieve a square aspect ratio
on the screen, the only efficient way to get them to the screen, as far as I
know, is to OR four of them into 3 bytes and write the 3 bytes to memory.
the table arrays are used to gain speed. The constant 0x1ffe in the for()
loop moves the offset to the next scan line. The HGA mixes scan lines in
memory so that line 0 is immediately followed by line 4, which is followed 
by line 8, etc., and the beginning of line 1 is 0x2000 bytes above the
beginning of line 0, and line 2 is 0x2000 bytes above line 1, etc. I think
this is because the HGA actually directs the cathode ray to the scan lines
in the order 0, 4, 8, 12, ... to the bottom of the screen, then lines
1, 5, 9, 13, ... then 2, 6, 10, 14, ... etc. */

void screen_put4c(c1,c2,c3,c4,v,h)
int c1,c2,c3,c4,v,h;
{
	int i,off;
	
	off= 0x2000 * (v&3) + 90*(v>>2) + h*3;
	for(i=0;i<4;i++,off += 0x1ffe){
		screen[off++]= table1[c1][i] | table2[c2][i];
		screen[off++]= table3[c2][i] | table4[c3][i];
		screen[off]= table5[c3][i] | table[c4][i];
	}
}
