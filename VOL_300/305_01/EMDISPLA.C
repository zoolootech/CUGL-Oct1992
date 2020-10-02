/*
TITLE:		EMDISPLAY.C;
DATE:		6/21/88;
DESCRIPTION:	"Printer driver and display utility for Mandelbrot pictures.";
VERSION:	1.04;
KEYWORDS:	Mandelbrot;
FILENAME:	EMDISPLAY.C;
WARNINGS:	"Requires HGA. Supports Epson LX-800 only.";
SEE-ALSO:	MANDEL.DOC, EMANDEL.C, EMDISPLAY.EXE, EMANDEL.EXE, EJULIA.EXE;
SYSTEM:		MS-DOS;
COMPILERS:	Aztec;
AUTHORS:	Dan Schechter;
 */
 
#define DATE " 6/21/88 "
#define VERSION "1.04"

char *version = "Emdisplay by Kittensoft. Version " VERSION "-c" DATE ;
char *copyright = "\nCopyright (c) 1988, 1989 by Dan Schechter.\n";

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <setjmp.h>
#include <signal.h>

void	setmode(int), sleep(void), clearscreen(int), sendch(int),
	strUcpy(char *,char *), tellversion(void), printch(int),
	gl_build(int), virtto0(int), eratline(void),
	print(int), g_write(char *,int), g_putch(int,int,int);
int	getreply(char *), tpr(int), inkey(void), displaypict(char *) ;

#define NORMAL 0
#define GRAPHICS 1
#define INDEXPORT 0x3B4
#define CONTROLPORT 0x3b8
#define DATAPORT 0x3b5
#define CONFIGPORT 0x3bf
#define SCREEN_ON 8
#define GRAPH 2
#define TEXT 0x20
#define SCREEN0 0xb0000l 

#define FONTADDR 0xffa6e

#define P_DATA_NUM 672		/* MUST be divisible by 8 */
#define P_DATA_SIZE 504		/* MUST be divisible by 8 */
#define VIRT_SIZE 43680		/* 672/8 * 504+16	  */
#define SCR_H 348
#define LEADLINES 6

unsigned char *screen0addr;
unsigned char *font;
unsigned char virtscreen[VIRT_SIZE];
FILE *outfile = (void *)0;
void myputs(char *), sendch(int), help(void);
char *index(char *,int);
int look(int,int);
void exitt(int);

char *init = "\033@\0333\030\033?Z\005\033l\005"; 
				/* init, initsquare, set margin. */

char *mask[25];
char _mask[250];

int gdata[12] = { 
	0x35, 0x2d, 0x2e, 0x7, 0x5b, 
	0x2, 0x57, 0x57, 0x2, 0x3, 0x0, 0x0 };
int tdata[12] = { 
	0x61, 0x50, 0x52, 0xf, 0x19, 
	0x6, 0x19, 0x19, 0x2, 0xd, 0xb, 0xc };

char *cat = "\
\040\040\040\040\057\134\137\057\134\n\
\040\040\040\050\376\040\322\040\376\051\n\
\360\360\360\360\040\304\312\304\040\360\360\360\360\n\
\040\040\040\040\040\042\042\042\n";

char darkprint=0,titleblank=0,diskout=0,noask=0;
int leadlines=LEADLINES;
struct { int ax,bx,cx,dx,si,di,ds,es; } iregs,oregs;
jmp_buf env;

int main(n,arg)
int n;
char **arg;
{
	char *filename,*scdir();
	void *abstoptr(),*malloc();
	int i,namecount,cut;
	int fd,stop=0,lf=030;
	char *wp;
	char *namelist[200];
	
	if ((n==2)&&(arg[1][0]=='?')&&(arg[1][1]==0)){
		help();
		exit(0);
	}
	if ((n>1)&&(arg[n-1][0]=='/')){
		n--;
		for (i=1;arg[n][i];i++) switch(tpr(arg[n][i])){
			case 'D': darkprint=1; break;
			case '^': leadlines=0; break;
			case 'Z': diskout=1; break;
			case 'N': noask=1; break;
			case '2': iregs.dx=1; break;
			case '3': iregs.dx=2; break;
			case '?': help(); exit(0);
			case 'F': 
				switch(arg[n][++i]){
					case '+': init[4]++; 
						  if (leadlines>1) leadlines-=2;
						  break;
					case '-': init[4]--; 
						  if (leadlines) leadlines+=2;
						  break;
					default: 
						myputs("\nInvalid F.\n");
						help();
						exit(0);
				}
				break;
		}
	}
	if (n==1) mask[0]="*.PIC";
	else {
		for (wp=_mask,i=1;i<n;i++){
			mask[i-1]=wp;
			strcpy(wp,arg[i]);
			if (!index(wp,'.')) strcat(wp,".PIC");
			wp+=strlen(wp)+1;
		}
		n--;
	}
	tellversion();
	if (diskout) {
		if ((outfile=fopen("EPT.DAT","w"))==0){
			myputs("Can't open EPT.DAT");
			exit(1);
		}
		else myputs("Disk file output.");
	}
	if (n==1) myputs("\nMask = ");
	else myputs("\nMasks = ");
	for (i=0;i<n;i++){
		myputs(mask[i]);
		putchar(' ');
	}
	putchar('\n');
	if (leadlines) myputs("Auto adv.");
	else myputs("Zero adv.");
	if (darkprint) myputs(" Dark print.");
	else myputs(" Light print.");
	if (titleblank) myputs(" No title.");
	if (!diskout) switch ( iregs.dx ){
		case 0: myputs(" LPT1"); break;
		case 1: myputs(" LPT2"); break;
		case 2: myputs(" LPT3"); break;
	}
	if (init[4]!=lf){
		myputs(" Lf");
		if (init[4]>lf) while ((lf++)<init[4]) putchar('+');
		else while ((lf--)>init[4]) putchar('-');
		putchar('.');
	}
	myputs("\nPress ^C to abort. Any other key to begin. ");
	if (scr_getc()==3) exit(0);
	
	screen0addr=abstoptr(SCREEN0);
	font=abstoptr(FONTADDR);

	for (i=namecount=0;i<n;i++){
		while (filename=scdir(mask[i])) {
			if ((namelist[namecount]=malloc(1+strlen(filename)))==0){
				myputs("malloc error.");
				exit(1);
			}
			strUcpy(namelist[namecount++],filename);
		}
	}
	if (!namecount) {
		myputs("\n\n   *** No files. ***\n");
		exit(1);
	}
	signal(SIGINT,(void (*)(int))exitt);	
	setmode(GRAPHICS);
	for(i=0;;i++){
		if (i==namecount) {
			if (noask) break;
			i=0;
		}
		if (i==-1) i=namecount-1;
		setjmp(env);
		if (displaypict(namelist[i])<0) {
			if (noask) continue;
			clearscreen(0);
			g_write(namelist[i],162);
			g_write("is not a PIC file.",174);
			g_write("Press ESC to exit. Any other key to continue...",186);
			switch (scr_getc()){
				case 27: 
				case 3:  exitt(1);
				default: continue;
			}
		}
		g_write(namelist[i],319);
		cut=0;
		if (noask) print(darkprint);
		else for(;;){
			g_write("(P)rint e(X)xit (M)ask (\032) (\033)",330);
			switch(getreply(" pbmx\231\313\310\307\317\033\315\320")) {
				case 'p': print(darkprint); break;
	/* ALT-P */		case 153: print(darkprint^1); break;
	/* La */		case 203:
	/* Ua */		case 200:
				case 'b': i-=2; break;
	/* Home */		case 199: i= -1; break;
	/* End */		case 207: i= namecount-2; break;
				case 'm': cut++; 
					  virtto0(cut%3); 
					  g_write(namelist[i],319);
					  continue;
				case ' ':
	/* Ra */		case 205:
	/* Da */		case 208: break;
				case 'x':
				case 27:  
				case 3:   exitt(0);
			}
			break;
		}
	}
	exitt(0);
}

/* v and h are pixel addresses, not col, line */
void g_write(s,v)
char *s;
int v;
{
	int gh=0;
	
	while (*s) g_putch(*s++,v,gh++);
	g_putch(' ',v,gh);
}
void g_putch(c,v,x)
int c,x,v;
{
	int i,j,y;
	unsigned char dat,mask;
	unsigned off;
	
	for (y=v-2;y<v+10;y++){
		off=(unsigned)( 0x2000 * (y%4) + 90*(y/4) + x);
		screen0addr[off]=0;
	}
	for (y=v,i=0;i<8;i++,y++){
		off=(unsigned)( 0x2000 * (y%4) + 90*(y/4) + x);
		dat=font[c*8+i];
		screen0addr[off]=dat;
	}
}

void setmode(q)
int q;
{
	unsigned i;
	
	switch (q) {
		case GRAPHICS:
				putchar(10);
				outportb(CONFIGPORT,3);
				outportb(CONTROLPORT,2);
				for(i=0;i<12;i++){
					outportb(INDEXPORT,i);
					outportb(DATAPORT,gdata[i]);
				}
				sleep();
				clearscreen(0);
				outportb(CONTROLPORT,10);
				break;
		case NORMAL:
				clearscreen(0);
				outportb(CONTROLPORT,0);
				for(i=0;i<12;i++){
					outportb(INDEXPORT,i);
					outportb(DATAPORT,tdata[i]);
				}
				sleep();
				outportb(CONTROLPORT,61);
				outportb(CONFIGPORT,0);
				scr_clear();
				break;
	}
}
void sleep()
{
	long i,clock();
	
	i=clock();
	while ( clock()-i <100 );
}
int displaypict(name)
char *name;
{
	FILE *fp;
	int f;
	unsigned pic_size;
	int i,height,xwidth,topskip,leftskip;
	unsigned char scanline[P_DATA_NUM];
	char trash[160];
	
	clearscreen(1);
	if ((f=open(name,O_RDONLY,0))==(-1)) return(-1);
	
	read(f,&trash,160);
	read(f,(char *)&height,2);
	read(f,(char *)&xwidth,2);
	if ((height>P_DATA_SIZE)||(height<=0)||(xwidth>P_DATA_NUM)||(xwidth<=0))
		return(-2);
	
	topskip= (P_DATA_SIZE - height)/2;	/* scan lines */
	leftskip= (P_DATA_NUM - xwidth)/16;	/* bytes */
	
	pic_size= (long)height*(long)xwidth/8l;
	
	read(f,virtscreen,pic_size);
	for (i=0;i<8;i++)
		read(f,virtscreen+VIRT_SIZE-P_DATA_NUM+(i*(P_DATA_NUM/8)),P_DATA_NUM/8);
	close(f);
	
	for (i=height;i>=0;i--){
		memcpy(scanline,virtscreen+i*(xwidth/8),xwidth/8);
		memset(virtscreen+i*(xwidth/8),0,xwidth/8);
		memcpy(virtscreen+((i+topskip)*(P_DATA_NUM/8))+leftskip,scanline,xwidth/8);
	}
	virtto0(0);
	return(0);
}
void clearscreen(q)
int q;
{
	switch (q){
		case 0:	memset(screen0addr,0,(unsigned)32768); 
		case 1:	memset(virtscreen,0,(unsigned)VIRT_SIZE); 
	}
}
int tpr(c)
int c;
{
	if ((c>='a')&&(c<='z')) c ^= 32;
	return(c);
}
void myputs(s)
char *s;
{
	while (*s) putchar(*s++);
}

int look(v,h)
int h,v;
{
	unsigned off;
	
	off = (P_DATA_NUM/8)*v + h/8;
	
	return(virtscreen[off]);
}
void help()
{
	myputs("\
-------------------------------------------\n");
	tellversion();
	myputs("\
View Emandel pictures and print on Epson LX-800 printer.\n\
-------------------------------------------\n\
Usage: EMDISPLAY <filename> [/<options>]\n\
Filename may contain wild cards. Filetype PIC is assumed but may be overridden.\n");
	myputs("\
                                Options:\n\
F+ Increase line feed by 1/216 in.  |   2  Use LPT2.\n\
F- Decrease line feed by 1/216 in.  |   3  Use LPT3.\n\
^  Don't advance paper at top.      |   D  Print dark.\n\
Z  Send output to disk file.        |   N  Print without asking.\n"
"?  Help. Show this message.\n\
-------------------------------------------------------------------\n\
See MANDEL.DOC for detailed instructions.");
}

void print(darkprint)
int darkprint;
{
	int i,j,line,dat,v,darktoggle=0;
		
	for (i=0;init[i];i++) {
		if (i==4) sendch(init[4]-darkprint);
		else sendch(init[i]);
	}
	i=0;
	for(;i<leadlines;i++) sendch('\n');
	gl_build(-2);
	for (line=0;line<P_DATA_NUM;line+=8){
		if (titleblank) 
			for (i=16;i;i--) gl_build(0);
		for (v=(titleblank)?(P_DATA_SIZE-1):P_DATA_SIZE+15;v>=0;v--){
			dat=look(v,line);
			gl_build(dat);
		}
		gl_build(-1);

		if (scr_poll()!=(-1)){
			if (scr_getc()==3){
				sendch(12);
				return;
			}
		}
		if (darkprint){
			darktoggle ^= 1;
			if (darktoggle){
				sendch(13);
				sendch(27);
				sendch('J');
				sendch(1);
				line-=8;
				continue;
			}
		}
		sendch(13);
		sendch(10);
	}
	sendch(10);
	sendch(12);	/* Form feed. */
}
void sendch(c)
int c;
{
	if (outfile) putc(c,outfile);
	else printch(c);
}
void printch(c)
int c;
{
	
	iregs.ax= 2<<8;
	sysint(0x17,&iregs,&oregs);
	if ( (oregs.ax>>8) & 32) {
		putchar(7);
		g_write("Printer out of paper. (A)bort e(X)it (R)etry? ",308);
		switch( getreply("xra\003")){
			case 'x': 
			case 3:   exitt(1);
			case 'r': 
				  g_write("                                               ",308);
				  printch(c);
				  return;
			case 'a': longjmp(env,'p');
		}
	}
	if ( (oregs.ax>>8) & 8) {
		putchar(7);
		g_write("Printer I/O error. (A)bort e(X)it (R)etry? ",308);
		switch( getreply("xra\003")){
			case 'x': 
			case 3:   exitt(1);
			case 'r': 
				  g_write("                                               ",308);
				  printch(c);
				  return;
			case 'a': longjmp(env,'e');
		}
	}
	iregs.ax=c;
	sysint(0x17,&iregs,&oregs);
}
int getreply(s)
char *s;
{
	int c;
	unsigned char *p;
	
	for(;;){
		c=scr_getc();
		if ((c>='A')&&(c<='Z')) c ^= 32;
		for (p=(unsigned char *)s;*p;p++) if (*p==c) return c;
	}
}
void exitt(q)
int q;
{
	setmode(NORMAL);
	signal(SIGINT,SIG_DFL);
	myputs(cat);
	tellversion(); 
	exit(q);
}
void strUcpy(s1,s2)
char *s1,*s2;
{
	while (*s2) *(s1++) = tpr( *s2++ );
	*s1=0;
}
void tellversion()
{
	int t,mask;

	myputs(version);
	myputs(copyright);
}
void gl_build(c)
int c;
{
	static int pline[P_DATA_SIZE+100],plinecount;
	int i;
	
	if (c==-2) {
		plinecount=0;
		return;
	}
	if (c==-1) {
		if (plinecount==0) return;
		while ( !pline[plinecount-1] ) {
			plinecount--;
			if (plinecount==0) return;
		}
		sendch(27);
		sendch('Z');
		sendch(plinecount%256);
		sendch(plinecount/256);
		for(i=0;i<plinecount;i++) sendch( pline[i] );
		plinecount=0;
		return;
	}
	pline[plinecount++]=c;
}
void virtto0(cut)
int cut;
{
	int line0,linevirt;
	unsigned off0,offvirt;
	
	for (line0=linevirt=0;line0<SCR_H;linevirt++){
		if ((line0<SCR_H-8)&&(linevirt%3==cut)) continue;
		if ((line0==SCR_H-8)&&(linevirt>=P_DATA_SIZE)&&(linevirt<P_DATA_SIZE+8)) 
			continue;
		off0=(unsigned)( 0x2000 * (line0%4) + 90*(line0/4) );
		offvirt= (unsigned)(P_DATA_NUM/8)*linevirt ;
		memcpy(screen0addr+off0,virtscreen+offvirt,P_DATA_NUM/8);
		line0++;
	}
}
