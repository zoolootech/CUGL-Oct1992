/*
TITLE:		EMANDEL.C;
DATE:		7/2/88;
DESCRIPTION:	"HGA Mandelbrot explorer.";
VERSION:	1.09;
KEYWORDS:	Mandelbrot;
FILENAME:	EMANDEL.C;
WARNINGS:	"Requires HGA & Hard disk. 640K recommended. Very slow without FPU.";
SEE-ALSO:	MANDEL.DOC, EMDISPLAY.C EMDISPLAY.EXE, EMANDEL.EXE, EJULIA.EXE;
SYSTEM:		MS-DOS;
COMPILERS:	Aztec;
AUTHORS:	Dan Schechter;
 */

/* 
This program is copyright 1988, 1989 by Dan Schechter.
You may copy and distribute it only on a not-for-profit basis.
You may modify it providing that you give attribution to the original author
and document your modifications and distribute the modified version
only on a not-for-profit basis.


I provide this source code mainly for informational purposes. 
However, if you do wish to try to compile it, the following command lines
should work, if you have the Aztec C-86 compiler, version 4.10b.
The first line below compiles EMANDEL.EXE for a PC-AT with an 80287 FPU.
The next line compiles EJULIA.EXE for a PC-AT with an 80287 FPU.
The next line compiles EMANDEL.EXE for any PC, XT or AT with or without an 80287 FPU.
The last line compiles EJULIA.EXE for any PC, XT or AT with or without an 80287 FPU.
The first two use the +ef switch which make the compiler generate in-line
calls directly to the 80287. The second two use the sensing library.
Thus the second two will generate programs that run on any PC, with or
without an FPU, and will use the FPU if it is present; but the first two 
will generate programs that run faster because they do not use library 
calls for math.

c +l +2 +ef -n -lm87 -z3000 -dNUMLEVELS=301 emandel.c
c +l +2 +ef -n -lm87 -z3000 -o ejulia -dNUMLEVELS=301 -dJULIA emandel.c
c +l -n -lm87s -z3000 -dNUMLEVELS=301 emandel.c
c +l -n -lm87s -z3000 -o ejulia -dNUMLEVELS=301 -dJULIA emandel.c
 */

#define VERSION "1.09"
#define DATE "7/2/88"

char *version = "Ver. "VERSION"-c "DATE;

/* NOTES -->     
	
	Exit codes:
		0 No errors. Normal program termination.
		1 Unrecognized command line option.
		2 Wrong number of command line parameters.
		3 Insufficient RAM to allocate PDATA array.
		4 Insufficient disk space for .PIK file.
		5 Can't open file named on command line.
		6 Wrong program - Emandel/Ejulia.
		7 xlimit or ylimit larger than allocated memory space.
		8 height or xwidth larger than P_DATA_SIZE or P_DATA_NUM.
		  (7 and 8 should never happen. They would be software bugs.)
		9 No RAM for virtscreen.
		10 Invalid filename on command line.
		99 No errors. Normal program termination. 
		   User requested non-zero exit code.

 */
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <fcntl.h>

void	setmode(int), writedot(int,int), uppername(char *), del(char *s),
	cleardot(unsigned char *,int,int), mybatname(char *,char *), exitt(int),
	help(int), showlevels(int *,int *), flash(void), _s_p_l(int),
	load(char *), myjpname(char *), writejp(double,double), virtto0(int),
	reverse(void), clearscreen(int), 
	telldouble(int,int,double,double,int,int),
	revdot(int,int), sleep(int), soundoff(int), thresh_view(void), 
	writepict(void), getthresh(int *,int *), grid(void), countfree(void), 
	virt_write(char *), redraw(int *,int *,int,int), stripz(char *s),
	writebyte(int,int,int), lognew(int), parsename(char *), mem_full(int),
	revcross(int,int), mypctname(char *), mynewname(void), ng_help(int), 
	sw_scr(void), g_putc(int), newdrive(int), parsepath(char *),
	makebat(double,double,double,double,char *,int), g1_write(char *),
	blink(int,int,unsigned char *), restore(int *,int *), k_timer(int),
	g_write(char *), g_putch(int,int,int,unsigned char *), eratline(void);
int	tpr(int), inkey(void), levc(int), levtonum(int), getdata(int,int),
	auto_levels(int *,int *), newpath(char *), options(int), save(void),
	blink_getc(int,int,unsigned char *), _getth(int,int), 
	g_getreply(char *), calculate(void), hc_calculate(void);
char 	*timestring(int), *index(char *,int), *rindex(char *,int), 
	*backe(char *,char *), *numtolev(int), *g_gets(char *,int,char *), 
	*_katof_fill(char *, char *);
long	diskfree(void), filesize(char *), clock(void), kt_(void);
void	*malloc(), *abstoptr(long);
double	dsqroot(double), atof(char *), atoi_d(char *);

#define strchr index
#define strrchr rindex

#define _STRTODLEN_ 50	/* Maximum length of floating point strings. */
#define NORMAL 0
#define GRAPHICS 1
#define INDEXPORT 0x3B4
#define CONTROLPORT 0x3b8
#define DATAPORT 0x3b5
#define CONFIGPORT 0x3bf
#define SCREEN_ON 8
#define GRAPH 2
#define TEXT 0x20
#define SCREEN0 0xb0000l 	/* Screen address. */
#define SCREEN1 0xb8000l

#define PATHLEN 160			/* Maximum length of path strings. */
#define NAMELEN 13
#define BATCH_LENGTH 127    /* Longest allowable command line, for makebat() */
#define P_DATA_NUM 672		/* MUST be divisible by 8 */
#define P_DATA_SIZE 504		/* MUST be divisible by 8 */ /* or by 4? */
#define AREA 338688.0		/* 672 * 504 as a double  */
#define PIC_SIZE (xwidth8*height+P_DATA_NUM+160)
#define HEADER_LENGTH 127l		/* Length of data file header. 
					Used to figure full file size. */
#define eratline() g_write("\r");
#define G_DELAY 100			/* Constant for sleep() in setmode() */
#define B_DELAY 1			/* Constant for sleep() in blink() */
#define S_COUNT 25			/* Constant for soundoff() */
#define FLASH_DELAY 200			/* Constant for sleep() in flash() */
#define FONTADDR 0xffa6e		/* ROM font address. */
#define STRIPZ_C '$'			/* Used by stripz(). */
#define ASPECT .75

#ifndef NUMLEVELS	
#define NUMLEVELS 201	/* Beware of setting NUMLEVELS so high that stack space is exhausted */
#endif

#ifdef JULIA
#define ARGNUM 6
#else
#define ARGNUM 4
#endif

/* For emulating systems with less than 640K of ram, HOGMEM allocates
some memory just to tie it up. FREEBLOCKS reports the number of K of RAM
not used by the program. These are for diagnistic purposes only. */
#ifdef HOGMEM
#ifndef FREEBLOCKS
#define FREEBLOCKS
#endif
char *hog[512];
int hogmem=HOGMEM;
#endif

#ifdef FREEBLOCKS
unsigned char *freep[512];
int numfree;
char *stackp;
int stackcount;
#endif

unsigned xwidth;
unsigned height;
unsigned virt_size;
unsigned char *pdata[P_DATA_NUM];	/* The main data array. */
unsigned char *screen0;			/* Pointer to screen memory. */
unsigned char *screen1;
unsigned char *virtscreen;		/* The virtual screen */
unsigned char *font;			/* Pointer to ROM character font. */
char scr_num;				/* Active screen number. */
int g_th,g1_tv,g1_th;			/* Graphics text cursor coordinates. */
long old_file_size;		    /* Number of bytes in present .PIK file. */
long full_file_size;		    /* Total space required for .PIK file    */
int groupsize;				/* Size of one disk group */
long total_time;			/* Cumulative calculation time. */
long this_time;				/* Calc time this session. */
char ESC;				/* Immediate escape from g_gets() */
char beep;				/* Beep when done if true. */
char autoexit;				/* Exit when done if true. */
char nz_ex;			/* Flag to force exit() to return non-zero. */
char pathname[PATHLEN] ;
char filename[NAMELEN] ;
int drivenum;
unsigned xwidth8;
int ii,ylimit,xlimit,maxit=255;
double rcorner,icorner,bottom,increment;
double rjulia,ijulia;
double wsize=1.0;				/* % of possible area. */
double aspect=ASPECT;				/* ylimit/xlimit */
char working[60]; 			/* String for -WORKING- message. */

int gdata[12] = { 
	0x35, 0x2d, 0x2e, 0x7, 0x5b, 
	0x2, 0x57, 0x57, 0x2, 0x3, 0x0, 0x0 };
int tdata[12] = { 
	0x61, 0x50, 0x52, 0xf, 0x19, 
	0x6, 0x19, 0x19, 0x2, 0xd, 0xb, 0xc };

char *_help[2] = {
#ifdef JULIA
"Ejulia from Kittensoft. ",
".\n\nUsage:\nEJULIA <Lr> <Ti> <Bi> <rj> <ij> [/M<m>] [/A<asp>] [/S<s>] [/N<fn>] [/X] [/B<n>]\n\
or:\nEJULIA <filename>\n"
#else
"Emandel from Kittensoft. ",
".\n\nUsage: EMANDEL <Lr> <Ti> <Bi> [/M<m>] [/S<s>] [/A<asp>] [/N<fn>] [/X] [/B<n>]\n\
   or: EMANDEL <filename>\n"
#endif
};

char *cpyr = "Copyright (c) 1988, 1989 by Dan Schechter";

char *beg = "\
This program is shareware. Please give copies to your friends.\n\
If you find it enjoyable please register your copy by sending $20 to:\n\n\
\040\040\040\040\057\134\137\057\134\t\t\tDan Schechter\n\
\040\040\040\050\376\040\322\040\376\051\t\t\tRoute 1 Box 19\n\
\360\360\360\360\040\304\312\304\040\360\360\360\360\t\t\tAmenia, North Dakota 58004\n\
\040\040\040\040\040\042\042\042\n";

int main(n,arg)
int n;
char **arg;
{
#ifdef FREEBLOCKS
	char stacktop,trash;
#endif
	int r,ex,i;
	char fv,c,*wp;
	long dfree;
	double tsize;
	
#ifdef HOGMEM
	char s[25];
	
	printf("hogmem? (%d) ",hogmem);
	gets(s);
	if (s[0]) hogmem=atoi(s);
	for (i=0;i<hogmem;i++) hog[i]=malloc(1024);
#endif
	system("break off");
	increment=1.0;
	font=abstoptr(FONTADDR);
	screen0=abstoptr(SCREEN0);
	screen1=abstoptr(SCREEN1);
	newdrive(-1);

	if (n==2) load(arg[1]);
	
	else {
		mynewname();
		while ((n>1)&&(arg[n-1][0]=='/')){
			n--;
			switch(tpr(arg[n][1])){
				case 'M': 
					maxit=atoi(arg[n]+2); 
					if (maxit>1023) maxit=1023;
					break;
				case 'N':
					parsename(arg[n]+2);
					break;
				case 'S':
					wsize=atof(arg[n]+2);
					if (wsize<0.05) wsize=0.05;
					if (wsize>1.0)  wsize=1.0;
					break;
				case 'A':
					aspect=atof(arg[n]+2);
					if ((aspect<.01)||(aspect>100))
						aspect=ASPECT;
					break;
				case 'B':
					beep= arg[n][2];
			beep = ((beep>='0')&&(beep<='9')) ? beep-48 : 0 ;
					break;
				case 'X':
					autoexit=1;;
					break;
				default:
			printf("\n*** Unregognized option: %s\n",arg[n]);
					ng_help(0);
					exitt(1);
			}
		}
		if (n!=ARGNUM){
 			ng_help(1);
			exitt(2);
		}
		
		height=ylimit=P_DATA_SIZE;
		xwidth=xlimit=P_DATA_NUM;
		if (aspect > ASPECT) xwidth=xlimit= ylimit/aspect;
		if (aspect < ASPECT) height=ylimit= xlimit*aspect;
		tsize= ( (double)xlimit*(double)ylimit )/AREA;
		if (wsize < tsize){
			xwidth=xlimit = xlimit*dsqroot(wsize)/dsqroot(tsize);
			height=ylimit = ylimit*dsqroot(wsize)/dsqroot(tsize);
		}
		else wsize=tsize;
		while(height%4) height++;
		while(xwidth%8) xwidth++;
		if ((height>P_DATA_SIZE)||(xwidth>P_DATA_NUM)){
			ng_help(4);
			exitt(8);
		}
		rcorner=atof(arg[1]);
		icorner=atof(arg[2]);
		bottom=atof(arg[3]);
		increment= (icorner - bottom)/ylimit;

#ifdef JULIA
		rjulia=atof(arg[4]);
		ijulia=atof(arg[5]);
#endif
		xwidth8=xwidth>>3;
		virt_size= xwidth8*height;
		if ((virtscreen=malloc(virt_size))==0){
			ng_help(2);
			exitt(9);
		}
		ex = (maxit>255)? height+(height>>2) : height ;
		for(i=0;i<xwidth;i++){
			pdata[i]=malloc(ex);
			if (pdata[i]==0){
				mem_full(i);
				break;
			}
			memset(pdata[i],0,ex);
		}
		setmode(GRAPHICS);
		clearscreen(0);
		clearscreen(1);
		i=0;
	}
	sprintf(working,"\rWorking on %s. Press ?? for help. ",filename);

	full_file_size = (maxit>255) ? height + (height>>2) : height;
	full_file_size *= xwidth;
	full_file_size += HEADER_LENGTH;
	old_file_size= filesize(filename);
	dfree=diskfree()+old_file_size;
	if (old_file_size % groupsize) 
		dfree += groupsize - (old_file_size % groupsize);
	if (full_file_size > dfree){
		setmode(NORMAL);
		printf("\n*** Insufficient disk space for %ld byte data file.\n%ld\n",full_file_size,dfree);
		for (i=0;i<xwidth;i++) free(pdata[i]);
		free(virtscreen);
		ng_help(0);
		exitt(4);
	}  
	
#ifdef FREEBLOCKS
	countfree();
	stackp = &stacktop;
	g_write(" FREEBLOCKS reminder. Call threshview/redraw for stack check. ");
	blink_getc(340,g_th,screen0);
#endif 
	
	if ((xlimit>xwidth)||(ylimit>height)){
		setmode(NORMAL);
		printf("*** Limits error.");
		printf("\nxl=%d pdn=%d yl=%d pds=%d",xlimit,xwidth,ylimit,height);
		exitt(7);
	}
	
	if ((autoexit)&&(!scr_num)) sw_scr();
_kee:	k_timer(1);
	ex = (maxit<=255) ? calculate() : hc_calculate() ;
	k_timer(0);

	if (ex) g_write("\rCalculation halted. ");
	else {
		g_write("\rCalculation completed. ");
		for(;beep;beep--) putchar(7);
		if (autoexit){
			if (scr_num) sw_scr();
			if (save()){
				setmode(NORMAL);
#ifdef JULIA
				printf("EJULIA from Kittensoft. %s.\n%s\n%s",version,cpyr,beg);
#else
				printf("EMANDEL from Kittensoft. %s.\n%s\n%s",version,cpyr,beg);
#endif
				exitt(0);
			}
			
		}
	}
	for (fv=0;;){
		r=blink_getc(340,g_th,screen0);
		if (r==21) break;
		if (r==3){
			if (scr_num) { flash(); continue; }
			if (!fv) {
				eratline();
				g_write(filename);
				g_write(" not saved. Exit? ");
				if ( g_getreply("YN")=='N' ) {
					g_write(" Well okay then... Calculation ");
					if (ex) g_write("halted. ");
					else g_write("completed. ");
					continue;
				}
			}
			for (i=xwidth;i;i--) free(pdata[i-1]);
			free(virtscreen);
			setmode (NORMAL);
#ifdef JULIA
			printf("EJULIA from Kittensoft. %s.\n%s\n%s",version,cpyr,beg);
#else
			printf("EMANDEL from Kittensoft. %s.\n%s\n%s",version,cpyr,beg);
#endif
			printf("\nCalc time: This session: %s ",timestring(1));
			printf("Total: %s [Rem: ",timestring(2));
			printf("%s] ",timestring(3));
			
			if (fv) printf("\n%s saved.\n",filename);
			else printf("\n%s not saved.\n",filename);
			
#ifdef FREEBLOCKS
			printf("\n\n%d free 1K blocks. Stackcount=%d",numfree,stackcount);
#endif
			if (nz_ex) exitt(99);
			exitt(0);
		}
		switch (options(r)){
			case 1: fv=1; break;
			case 'w': 
				if (ex) g_write("\rCalculation halted. ");
				else g_write("\rCalculation completed. ");
				break;
		}
	}
	if (ex) for (r=0;r<ylimit;r++) cleardot(screen0,r,ii);
	goto _kee;
}

void setmode(q)
int q;
{
	int i;
	
	clearscreen(0);  
	switch (q) {
		case GRAPHICS:
				outportb(CONFIGPORT,3);
				outportb(CONTROLPORT,2);
				for(i=0;i<12;i++){
					outportb(INDEXPORT,i);
					outportb(DATAPORT,gdata[i]);
				}
				sleep(G_DELAY);
				outportb(CONTROLPORT,10);
				break;
		case NORMAL:
				outportb(CONTROLPORT,0);
				for(i=0;i<12;i++){
					outportb(INDEXPORT,i);
					outportb(DATAPORT,tdata[i]);
				}
				sleep(G_DELAY);
				outportb(CONTROLPORT,40);
				outportb(CONFIGPORT,0);
				scr_clear();
				break;
	}
}
void sw_scr()
{
	
	scr_num^=128;
	outportb(CONTROLPORT, 10 | scr_num);
}
void clearscreen(q)
int q;
{
	if (q)  memset(screen1,0,(unsigned)32768); 
	else {
		memset(screen0,0,(unsigned)32768); 
		memset(virtscreen,0,virt_size); 
	}
}

/* The following pair of functions will return a grossly wrong elapsed
time if they span from a leap year into the following new year. */

void k_timer(q)
int q;
{
	static long started;
	long t;
	
	if (q) { started=kt_(); return; }
	
	t= kt_() - started;
	this_time += t;
	total_time += t;
}
long kt_()
{
	register long kt;
	struct {
		short sec;
		short min;
		short hour;
		short trash1;
		short trash2;
		short year;
		short trash3;
		short yday;
		short trash4;
		short trash5;
	} t;
	
	dostime(&t);
	kt = (t.year - 88)*365+t.yday;
	kt *= 24;
	kt += t.hour;
	kt *= 60;
	kt += t.min;
	kt *= 60;
	return(kt + t.sec);
}
char *timestring(q)
int q;
{
	static char str[15],elipsis[4]="...",zero[8]="0:00:00";
	int m,s;
	long t;
	
	if (!t) return zero;
	switch (q) {
		case 1: t=this_time;
			break;
		case 2: t=total_time;
			break;
		case 3: 
			if (( (ii<<4) < xlimit)||( total_time < ii*3 )) 
				return elipsis;
			t= (total_time/ii)*(xlimit-ii);
			break;
	}
	s= t%60;
	t /= 60;
	sprintf(str,"%d:%02d:%02d",(int)t/60,(int)t%60,s);
	return(str);
}
int inkey()
{
	
	return(bdos(0x600,0xff,0));
}
void writedot(y,x)
int x,y;
{
	unsigned char bit;
	unsigned off;
	
	bit = 1<< (7-x%8);

	virtscreen[xwidth8*y + (x>>3)] |= bit;

	if (y%3==0) return;
	y-=((y/3)+1);
	off=(unsigned)( 0x2000 * (y&3) + 90*(y>>2) + (x>>3) );
	
	screen0[off] |= bit;
	screen1[off] |= bit;
}
void revdot(y,x)
int x,y;
{
	
	if (y%3==0) return;
	y-=((y/3)+1);
	screen0[( 0x2000 * (y&3) + 90*(y>>2) + (x>>3) )] ^= (1<<(7-x%8));
}
void cleardot(seg,y,x)
int x,y;
unsigned char *seg;
{
	unsigned char bit;
	
	bit = (~(1<< (7-x%8)));
	if (seg==screen0)
		virtscreen[xwidth8*y + (x>>3)] &= (bit);

	if (y%3==0) return;
	y-=((y/3)+1);
	seg[( 0x2000 * (y&3) + 90*(y>>2) + (x>>3) )] &= (bit);
}
int save()
{
	int fd,num,sw;
	long dfree;
	char s[80];
	
	sw = (maxit>255) ? height+(height>>2) : height;
	full_file_size = (long)sw * (long)ii + HEADER_LENGTH;
	old_file_size=filesize(filename);

	dfree=diskfree()+old_file_size;
	if (old_file_size % groupsize) 
		dfree += groupsize - (old_file_size % groupsize);
	if (full_file_size > dfree){
		sprintf(s,"\rInsufficient disk space for %ld byte file. %ld. ",full_file_size,dfree);
		g_write(s);
		return(0);
	}  
	
	if ((fd=open(filename,O_WRONLY|O_TRUNC))==-1){
		g_write("\rCannot open pik file. ");
		return(0);
	}
	g_write("Saving... ");
#ifdef JULIA
	write(fd,"J",1);
#else
	write(fd,"K",1);
#endif
	write(fd,(char *)&rcorner,8);
	write(fd,(char *)&icorner,8);
	write(fd,(char *)&xlimit,2);
	write(fd,(char *)&ylimit,2);
	write(fd,(char *)&xwidth,2);
	write(fd,(char *)&height,2);
	write(fd,(char *)&increment,8);
	write(fd,(char *)&aspect,8);
	write(fd,(char *)&wsize,8);
	write(fd,(char *)&maxit,2);
	write(fd,(char *)&ii,2);
	write(fd,(char *)&bottom,8);
	write(fd,(char *)&rjulia,8);
	write(fd,(char *)&ijulia,8);
	write(fd,(char *)&total_time,4);
	write(fd,&beep,1);
	write(fd,&autoexit,1);

	for (num=4;num;num--) write(fd,"KITTENSOFT ",11);

	for (num=0;num<ii;num++)
		write(fd, pdata[num], sw);

	close(fd);
	g_write(" Saved. ");
	return(1);
}
void load(com_arg)
char *com_arg;
{
	int fp,num,x,y,sw;
	char buf[PATHLEN],*wp,q; 

	strcpy(buf,com_arg);
	wp=strchr(buf,'.');
	if (wp) strcpy(wp,".PIK");
	else strcat(buf,".PIK");	/* Now buf should access the file. */
	parsename(buf);			/* Now filename should contain
						name without path. */
	
	if ((fp=open(buf,O_RDONLY))==(-1)){
		printf("\n*** Can't open file: %s\n",buf);
		ng_help(0);
		exitt(5);
	}
	if (( !read(fp,&q,1) ) ||
#ifdef JULIA
		(q!='J')) {
			printf("\n*** %s not an Ejulia file.\n",buf);
			ng_help(0);
			exitt(6);
	}
#else
		(q!='K')) {
			printf("\n*** %s not an Emandel file.\n",buf);
			ng_help(0);
			exitt(6);
	}
#endif
	read(fp,(char *)&rcorner,8);
	read(fp,(char *)&icorner,8);
	read(fp,(char *)&xlimit,2);
	read(fp,(char *)&ylimit,2);
	read(fp,(char *)&xwidth,2);
	read(fp,(char *)&height,2);
	read(fp,(char *)&increment,8);
	read(fp,(char *)&aspect,8);
	read(fp,(char *)&wsize,8);
	read(fp,(char *)&maxit,2);
	read(fp,(char *)&ii,2);
	read(fp,(char *)&bottom,8);
	read(fp,(char *)&rjulia,8);
	read(fp,(char *)&ijulia,8);
	read(fp,(char *)&total_time,4);
	read(fp,&beep,1);
	read(fp,&autoexit,1);
	if (ii==xlimit) autoexit=0;
	read(fp,buf,44);
	
	xwidth8=xwidth>>3;
	virt_size= xwidth8*height;
	if ((virtscreen=malloc(virt_size))==0){
		ng_help(2);
		exitt(9);
	}
	if (maxit>255) {
		num=sw= height+(height>>2);
	}
	else sw=num = height;
	
	for(x=0;x<xwidth;x++){
		pdata[x]=malloc(num);
		if (pdata[x]==0){
			mem_full(x);
			if (ii>xwidth) ii=xwidth;
			break;
		}
		memset(pdata[x],0,num);
	}

	printf("\nReading data file %s ",filename);
	for(num=0;num<ii;num++)
		read(fp, pdata[num], sw);

	close(fp);
	setmode(GRAPHICS);
	clearscreen(0);
	clearscreen(1);
	
	redraw(&x,&y,ii,0);
	
}
void soundoff(q)
int q;
{
	char buf[150];
	int r,i;
	
	for(i=S_COUNT;i;i--){
		if ((r=inkey())==0) sleep(B_DELAY);
		else break;
	}
	if (!r){
		sw_scr();
		g1_write("\014\
C  coordinates.\n\
M  Maxit.\n\
H  Horizontal position.\n\
S  Frame size & ratio.\n\
G  Magnification.\n\
J  Julia point.\n\
P  Drive:path.\n\
I  Increment.\n\
T  Time totals.\n\
N  File name.\n\
B  Beeper alarm.\n\
X  Autoexit status.\n\
%  Percent of frame completed.\n\
?  Help.\n\
\n\
Enter selection: ");
	
		r=blink_getc(g1_tv,g1_th,screen1);
		sw_scr();
		clearscreen(1);
	}
	buf[0]=0;
	switch(tpr(r)){
		case 'T':
			sprintf(buf,"\002 Time: %s ",timestring(1));
			sprintf(buf+strlen(buf),"* %s ",timestring(2));
			sprintf(buf+strlen(buf),"[%s] ",timestring(3));
			break;
		case 'N':
			sprintf(buf,"\002 fn: %s ",filename);
			break;
		case 'B':
			sprintf(buf,"\002 Beep=%d ",beep);
			break;
		case 'X':
			if (autoexit) sprintf(buf,"\002 aX:on ");
			else sprintf(buf,"\002 aX:off ");
			break;
		case '5':
		case '%':
			sprintf(buf,"\002 %d%% ", (int)( (100*(long)ii)/xlimit ) );
			break;
		case 'S':
			sprintf(buf,"\002 h=%d yl=%d w=%d xl=%d size=%g%c asp=%g%c ",height,ylimit,xwidth,xlimit,wsize,STRIPZ_C,aspect,STRIPZ_C);
			break;
		case 'C': 
			sprintf(buf,"\002 Lr=%g%c Ti=%g%c Rr=%g%c Bi=%g%c ",rcorner,STRIPZ_C,icorner,STRIPZ_C,rcorner+(xlimit*increment),STRIPZ_C,bottom,STRIPZ_C);
			break;  
#ifdef JULIA
		case 'J':
			sprintf(buf,"\002 rj=%g%c ij=%g%c ",rjulia,STRIPZ_C,ijulia,STRIPZ_C);
			break;
#endif
		case 'P':
			lognew(0);
			return;
		case 'I': 
			sprintf(buf,"\002 inc=%.3g ",increment);
			break;
		case 'G':
			sprintf(buf,"\002 mag=%.3g ",2.5/(increment*height));
			break;
		case 'M': 
			sprintf(buf,"\002 m=%d ",maxit);
			break;
		case 'H': 
			sprintf(buf,"\002 hz=%d ",ii);
			break;
		case '/':
		case '?':
			help(q);
			return;
	}
	stripz(buf);
	g_write(buf);
}
void stripz(s0)
char *s0;
{
	char *s;
	
	s=s0;
	while( s=index(s,STRIPZ_C) ){
		del(s--);
		s=backe(s,s0);
		if (!s) stripz(s0);
		while( *s == '0' ) del(s--);
	}
}
char *backe(s,s0)
char *s,*s0;
{
	char *wp;
	
	for (wp=s; wp != s0 ; wp--)
		switch(*wp){
			case ' ': return((char *)0);
			case '.': return(s);
			case 'e': return(wp-1);
	}
	return((char *)0);
}
	
void del(s)
char *s;
{
	while (*s) (*s++) = (*(s+1));
}

void telldouble(g,sn,d,e,i,j)
double d,e;
int i,j,g,sn;
{
	char buf[100];
	int n;
	
	n= (maxit>255)? getdata(j,i) : pdata[i][j];
	sprintf(buf,"\r%c-%d (?? for help.) r=%.15g i=%.15g itr=%d ",g,sn,d,e,n);
	g_write(buf);
}
void revcross(j,i)
int i,j;
{
	int w=ylimit,xw=xlimit;
	
	if (j%3==0) j++;
	
	while(xw) revdot(j,--xw);
	while(w) revdot(--w,i);
}
void grid()
{
	int dir='5',c;
	static int i,j,xoff,yoff,num=1,glide='s';

	*(unsigned char *)(abstoptr(0x417l))|=32;
	revcross(j,i);
	if (xoff||yoff) revcross(j+yoff,i+xoff);
	telldouble(glide,num,rcorner+(i*increment),icorner-(j*increment),i,j); 
	for(;;){
		if ((c=tpr(inkey()))==27) break;
		switch (c){
			case ' ': eratline(); break;
			case '/':
			case '?': soundoff(1); dir='5'; break;
			case 'L': lognew(1); dir='5'; break;
			case '.': 
				glide ^= 20;
				g_putch(glide,340,0,screen0);
				break;
			case '+': 
			case '*': 
			case '-': 
			case 8:
				switch(c){
					case '+': num++; break;
					case '*': num += 5; break;
					case '-': if (num!=1) num--; break;
					case 8:   num=1; break;
				}
				telldouble(glide,num,rcorner+(i*increment),icorner-(j*increment),i,j); 
				break;
#ifndef JULIA
			case 10:  
				rjulia= rcorner+i*increment;
				ijulia= icorner-j*increment;
				makebat(-2.0, 2.0, -2.0, ASPECT, (char *)0, 'j'); 
				rjulia=ijulia=0.0;
				break;
#endif
			case '#': makebat(rcorner+(i*increment),icorner-(j*increment),icorner-((j+yoff)*increment),(double)yoff/(double)xoff,(char *)0,0); 
				  break;
			case 2:
			case 19:
			case 4:
			case 21:
			case 18:
			case 12:
			case '0':
				if (xoff||yoff) revcross(j+yoff,i+xoff);
				
		/*S*/		if (c==19) { xoff-=4*num; yoff-=3*num; }
		/*U*/		if (c==21) yoff-=num; 
		/*L*/		if (c==12) xoff-=num; 
		/*B*/		if ((c==2)&&(xoff+4*num+i<xlimit)&&(yoff+3*num+j<ylimit))
					{ xoff+=4*num; yoff+=3*num; }
		/*D*/		if ((c==4)&&(yoff+j+num<ylimit))  yoff+=num; 
		/*R*/		if ((c==18)&&(xoff+i+num<xlimit)) xoff+=num; 
		/*0*/		if (c=='0') xoff=yoff=0; 
				
				if (xoff<0) xoff=0;
				if (yoff<0) yoff=0;
				if (xoff||yoff) revcross(j+yoff,i+xoff);
				break;
		}
		if ((c>='1')&&(c<='9')) dir=c;
		if (dir=='5') continue; 
		
		revcross(j,i);
		if (xoff||yoff) revcross(j+yoff,i+xoff);
		switch (dir) {
			case '8': j-=num; break;
			case '4': i-=num; break;
			case '6': i+=num; break;
			case '2': j+=num; break;
			case '1': i-=num; j+=num; break;
			case '3': i+=num; j+=num; break;
			case '7': i-=num; j-=num; break;
			case '9': i+=num; j-=num; break;
		}
		if (i<0) i=xlimit-xoff-1;
		if (i+xoff>=xlimit) i=0;
		if (j<0) j=ylimit-yoff-1;
		if (j+yoff>=ylimit) j=0;
		revcross(j,i);
		if (xoff||yoff) revcross(j+yoff,i+xoff);
		telldouble(glide,num,rcorner+(i*increment),icorner-(j*increment),i,j); 
		if (glide=='s') dir='5';
	}
	if (xoff||yoff) revcross(j+yoff,i+xoff);
	*(unsigned char *)(abstoptr(0x417l))^=32;
	revcross(j,i);
}
void thresh_view()
{
	int bottom[NUMLEVELS],top[NUMLEVELS],level,th,cut=0;
	int c;
	char r[10];
	unsigned char *tmpvirt;

	if (tmpvirt=malloc(virt_size))
		memcpy(tmpvirt,virtscreen,virt_size);
	th=g_th;
	for (level=0;level<NUMLEVELS;level++) top[level]=(-1);

	for(;;){
		g_write("\rTHRESHVIEW. Log, View, Grid, Write, Auto, Redraw, Mask, ?, or Cluster number: ");
		g_gets(r,10,"LVGARM?/");
		if (ESC) break;
		switch (tpr(r[0])) {
			case 'L': 
				  lognew(1); 
				  blink_getc(340,g_th,screen0);
				  continue;
			case 'V': showlevels(bottom,top); continue;
			case 'G': grid(); continue;
			case 'W': writepict(); continue;
			case 'A': if (!auto_levels(bottom,top)) continue;
			case 'R': redraw(top,bottom,ii,1); break;
			case 'M': cut++; virtto0(cut%3); break;
			case '/':
			case '?': help(2); continue;
		}
		level=atoi(r);
		if ((level==0)&&(r[0] != '0')) continue;
		if ((level<0)||(level>=NUMLEVELS)) continue;
		g_th=0;
		g_write(r);
		getthresh(bottom+level,top+level);
		if (ESC) continue;
		if (top[level]<bottom[level]) top[level]=(-1);
		redraw(top,bottom,ii,1);
	}
#ifdef FREEBLOCKS
	countfree();
#endif

	if (tmpvirt) {
		memcpy(virtscreen,tmpvirt,virt_size);
		virtto0(0);
		free(tmpvirt);
		g_th=th;
	}
	else {
		clearscreen(0);
		redraw(top,bottom,(ii==xlimit)?ii:ii+1,0);
		eratline();
	}
}
void virtto0(cut)
int cut;
{
	unsigned offv,off0,linev,line0;
	
	for(linev=line0=0;linev<ylimit;linev++){
		if ((linev%3)==cut) continue;
		offv= xwidth8*linev;
		off0= 0x2000 * (line0&3) + 90 * (line0>>2);
		memcpy(screen0+off0,virtscreen+offv,xwidth8);
		line0++;
	}
}
void getthresh(b,t)
int *b,*t;
{
	char buf[50];
	int c;
	
	if (*t==-1) sprintf(buf," Clear. New bottom? ");
	else sprintf(buf," bottom=%d top=%d  New bottom? ",*b,*t);
	g_write(buf);  
	c=*b;
	*b=_getth(*b,*t);
	if (ESC) return; 
	if (*b==-1) { *t=-1; return; }
	g_write("  New top? ");
	*t=_getth(*t,*b);
	if (ESC) { *b=c; return; }
	if (*t==-1) return; 
}	

int _getth(bt,ot)
int bt,ot;
{
	char buf[7];
	int n;
	
	g_gets(buf,7,"=CM<,");
	if (ESC) return(bt);
	switch(tpr(buf[0])){
		case '=': return(ot);
		case 'C': return(-1);
		case 'M': return(maxit);
		case '<': 
		case ',': return(maxit-1);
		case 0  : return(bt);
	}
	n=atoi(buf);
	if ((n<=0)||(n>maxit)) { ESC=1; return(bt); }
	return(n);
}
int auto_levels(b,t)
int *b,*t;
{
	int first,last,start,size,gap,stopval,tt;
	int i,val;
	char s[50];
	
	g_write("\rAuto-draw. First cluster number? ");
	g_gets(s,10,"");
	if (ESC) return(0);
	if (s[0]==0) { first=0; g_putc('0'); }
	else first=atoi(s);
	g_write(" Last cluster number? ");
	g_gets(s,10,"");
	if (ESC) return(0);
	if (s[0]==0) last=NUMLEVELS-1;
	else last=atoi(s);
			
	sprintf(s,"\rcluster %d-%d ",first,last);
	g_write(s);
	tt=g_th;
	g_write("Lowest pixel value? ");
	
	g_gets(s,10,"");
	if (ESC) return(0);
	if (tpr(s[0])=='C') {
		for (i=first;i<=last;i++) t[i]=(-1);
		return 1;
	}
	g_th=tt;
	g_write("v=");
	g_write(s);
	if (s[0]==0) { g_putc('1'); start=1; }
	else if ((start=atoi(s))<0) return(0);
	tt=g_th+1;
	g_write(" Cluster size? ");
	g_gets(s,10,"");
	if (ESC) return(0);
	g_th=tt;
	g_write("s=");
	g_write(s);
	if (s[0]==0) { size=1; g_putc('1'); }
	else if ((size=atoi(s))<=0) return(0);
	tt=g_th+1;
	g_write(" Cluster gap? ");
	g_gets(s,10,"");
	if (ESC) return(0);
	g_th=tt;
	g_write("g=");
	g_write(s);
	if (s[0]==0) { gap=1; g_putc('1'); }
	else if ((gap=atoi(s))<=0) return(0);
	g_write(" Largest pixel value? ");
	g_gets(s,10,"");
	if (ESC) return(0);
	stopval=atoi(s);
	if (stopval<=0) stopval=maxit-1;
	
	eratline();
	if ((first<0)||(last>=NUMLEVELS)||(first>=last)||(start>=maxit)) {
		g_write("Invalid parameter(s). Press any key. ");
		blink_getc(340,g_th,screen0);
		return(0);
	}
	for (i=first,val=start;i<=last;i++){
		b[i]=val;
		val+=(size-1);
		t[i]=val;
		if (t[i]>stopval){
			t[i]=(-1);
			break;
		}
		val+=(gap+1);
	}
	while (i<=last){
		t[i]=(-1);
		i++;
	}
	return(1);
}
void redraw(top,bottom,i,interrupt)
int *top,*bottom,i,interrupt;
{
	register unsigned char dat;
	int numl,h,v,level;
	unsigned char table[1024];
	
#ifdef FREEBLOCKS
	char trashh, trash;
	stackcount=stackp-&trash;
#endif
	g_write("\rPlease wait. ");
	if (!interrupt) {
		numl=1;
		top[0]=bottom[0]=maxit;
	}
	else {
		numl=NUMLEVELS;
		while (top[numl-1]==(-1)) {
			numl--;
			if (numl==0) {
				clearscreen(0);
				return;
			}
		}
	}
	if (maxit>255) for (v=0;v<1024;v++){
		for(level=0;level<numl;level++)
			if ((v>=bottom[level])&&(v<=top[level])){
				table[v]=1;
				break;
			}
		if (level==numl) table[v]=0;
		if (!(v&127)) g_putc('.');
	}
	else for (v=0;v<256;v++){
		for(level=0;level<numl;level++)
			if ((v>=bottom[level])&&(v<=top[level])){
				table[v]=1;
				break;
			}
		if (level==numl) table[v]=0;
	}
	if (interrupt) g_write("\rPress <BS> to interrupt... ");
	for (v=0,numl=height;v<height;v++,numl--){
		if ((interrupt)&&(inkey()==8)) break;
		revdot(v,719);
		if (maxit>255){
			for (h=0;h<i;h+=8){
				level=dat=0;
				for (;level<8;level++)
					dat |= (table[getdata(v,h+level)])<<(7-level);
				writebyte(dat,v,h>>3);
			}
		}
		else {
			for (h=0;h<i;h+=8){
				level=dat=0;
				for (;level<8;level++)
					dat |= (table[pdata[h+level][v]])<<(7-level);
				writebyte(dat,v,h>>3);
			}
		}
		revdot(v,719);
		if (!(numl%20)) g_putc(numl/20+64);
	}
}	
void writepict()
{
	char fn[NAMELEN],s[200];
	int f,sd,i;
	
	mypctname(fn);
	if (PIC_SIZE > diskfree()){
		g_write("\rInsufficient disk space. Press any key... ");
		blink_getc(340,g_th,screen0);
		return;
	} 
	if ((f=open(fn,O_WRONLY|O_TRUNC))==-1){
		g_write("\rCannot open file: ");
		g_write(fn);
		blink_getc(340,g_th,screen0);
		return;
	}
	sd=15;
	do {
#ifdef JULIA
		sprintf(s,"KITTENSOFT \001 Lr=%.*g%c Ti=%.*g%c Bi=%.*g%c rj=%.*g%c ij=%.*g%c",sd,rcorner,STRIPZ_C,sd,icorner,STRIPZ_C,sd,bottom,STRIPZ_C,sd,rjulia,STRIPZ_C,sd,ijulia,STRIPZ_C);
#else
		sprintf(s,"KITTENSOFT \001 Lr=%.*g%c Ti=%.*g%c Bi=%.*g%c mag=%.3g",sd,rcorner,STRIPZ_C,sd,icorner,STRIPZ_C,sd,bottom,STRIPZ_C,3.0/(icorner-bottom));
#endif
		sd--;
		stripz(s);
	} while (strlen(s)>P_DATA_NUM/8);
	
	eratline();
	for (i= (84-strlen(s))>>1; i; i--) g_putc(' ');
	g_write(s);
	sd=15;
	do {
#ifdef JULIA
		sprintf(s,"ejulia %.*g%c %.*g%c %.*g%c %.*g%c %.*g%c /m%d /s%.3g%c /a%.3g%c /n%s\n%c",sd,rcorner,STRIPZ_C,sd,icorner,STRIPZ_C,sd,bottom,STRIPZ_C,sd,rjulia,STRIPZ_C,sd,ijulia,STRIPZ_C,maxit,wsize,STRIPZ_C,aspect,STRIPZ_C,filename,26);
#else
		sprintf(s,"emandel %.*g%c %.*g%c %.*g%c /m%d /s%.3g%c /a%.3g%c /n%s\n%c",sd,rcorner,STRIPZ_C,sd,icorner,STRIPZ_C,sd,bottom,STRIPZ_C,maxit,wsize,STRIPZ_C,aspect,STRIPZ_C,filename,26);
#endif
		sd--;
		stripz(s);
	} while (strlen(s) > BATCH_LENGTH);
	while(strlen(s)<160) strcat(s,"\032");
	write(f,s,strlen(s));
	write(f,(char *)&height,2);
	write(f,(char *)&xwidth,2);
	write(f,virtscreen,virt_size);
	for (i=340;i<348;i++)
		write(f,screen0+ 0x2000 * (i&3) + 90*(i>>2),P_DATA_NUM/8);
	close(f);
	
	sprintf(s,"\rDone. fn=%s Press any key... ",fn);
	g_write(s);
	blink_getc(340,g_th,screen0);
}
void showlevels(b,t)
int *b,*t;
{
	int i,numl=NUMLEVELS;
	char gbuf[100];
	
	while (t[numl-1]==(-1)){
		numl--;
		if (!numl) break;
	}
	g_th=0;
	if (!numl) sprintf(gbuf,"All levels clear. ");
	else for (i=0,gbuf[0]=0;i<numl;i++){
		if (t[i]==(-1)) sprintf(gbuf+strlen(gbuf),"<%d>=cl ",i);
		else sprintf(gbuf+strlen(gbuf),"<%d>=%d-%d ",i,b[i],t[i]);
		if (strlen(gbuf)>75){
			if (i==numl-1) break;
			g_write(gbuf);
			if (blink_getc(340,g_th,screen0)==27) return;
			gbuf[0]=0;
		}
	}
	g_write(gbuf);
	g_th--;
	g_write(". ");
	blink_getc(340,g_th,screen0);
}
void mybatname(fn,prg)
char *fn,*prg;
{
	int fnum;

	for (fnum=0;fnum<=99;fnum++){
		sprintf(fn,"%s%d.bat",prg,fnum);
		if (access(fn,0)) break;
	}
}
void mypctname(fn)
char *fn;
{
	int fnum;
	char *wp,s[NAMELEN];
	
	strcpy(s,filename);
	if (wp=index(s,'.')) *wp=0;
	wp=rindex(s,'\\');
	if (wp==0) wp=s;
	*(wp+6)=0;
	for (fnum=0;fnum<=99;fnum++){
		sprintf(fn,"%s%d.PIC",s,fnum);
		if (access(fn,0)) break;
	}
}
void makebat(rleft,itop,bottom,asp,name,j)
double rleft,itop,bottom,asp;
char *name;
int j;
{
	char fn[50],pfn[50],s[15],batch[200],ms[12],ss[50],as[50];
	int newmax,dl,fd;
	double newsize;

#ifdef JULIA
	mybatname(fn,"ejulia");
#else
	if (j=='j') mybatname(fn,"ejulia");
	else mybatname(fn,"emand");
#endif

	strcpy(pfn,"/n");  
	if (name) {
		strcat(pfn,name);
		newmax=maxit;
		newsize=wsize; 
	}
	else {
		g_write("\rEnter .PIK file name or <CR> ");
		g_gets(pfn+2,40,"");
		if (ESC) { g_write("\rAborted. "); return; }
		if (pfn[2]==0) pfn[0]=0;
		if (j=='j'){
			newmax=255; 
		}
		else {
			g_write("  maxit? ");
			newmax=atoi(g_gets(s,10,""));
			if (ESC) { g_write("\rAborted. "); return; }
			if (newmax<=0) newmax=maxit;
		}
		g_write("  Size? (.05 to 1) ");
		g_gets(s,10,"");
		if (ESC) { g_write("\rAborted. "); return; }
		if (!s[0]) newsize=1.0;
		else newsize=atof(s); 
			
	}
	if (newmax==255) ms[0]=0;
	else sprintf(ms,"/m%d ",newmax);
	if (newsize==1.0) ss[0]=0;
	else sprintf(ss,"/s%g%c ",newsize,STRIPZ_C);
	if (asp==0.75) as[0]=0;
	else sprintf(as,"/a%g%c ",asp,STRIPZ_C);

	if ((fd=open(fn,O_WRONLY|O_TRUNC))==-1) {
		g_write("\rCan't make batch file. ");
		return;
	}
	dl=15;
	do {
#ifdef JULIA
	sprintf(batch,"ejulia %.*g%c %.*g%c %.*g%c %.*g%c %.*g%c %s%s%s%s",dl,rleft,STRIPZ_C,dl,itop,STRIPZ_C,dl,bottom,STRIPZ_C,dl,rjulia,STRIPZ_C,dl,ijulia,STRIPZ_C,ms,ss,as,pfn);
#else
	if (j=='j') sprintf(batch,"ejulia %.*g%c %.*g%c %.*g%c %.*g%c %.*g%c %s%s%s%s",dl,rleft,STRIPZ_C,dl,itop,STRIPZ_C,dl,bottom,STRIPZ_C,dl,rjulia,STRIPZ_C,dl,ijulia,STRIPZ_C,ms,ss,as,pfn);
	else sprintf(batch,"emandel %.*g%c %.*g%c %.*g%c %s%s%s%s",dl,rleft,STRIPZ_C,dl,itop,STRIPZ_C,dl,bottom,STRIPZ_C,ms,ss,as,pfn);
#endif
	stripz(batch);
	dl--;
	   } while (strlen(batch) > BATCH_LENGTH);

	sprintf(batch+strlen(batch),"\r\nIF NOT ERRORLEVEL 1 DEL %s\r\n",fn);
	write (fd,batch,strlen(batch));
	close(fd);
	
	g_write("\rBatch file name: ");
	g_write(fn);
	g_putc(' ');
}
void mynewname()
{
	int fnum;

	for (fnum=0;fnum<=999;fnum++){
#ifdef JULIA
		sprintf(filename,"EJ%d.PIK",fnum);
#else 
		sprintf(filename,"EM%d.PIK",fnum);
#endif
		if (access(filename,0)) break;
	}
}
/* v and h are pixel addresses, not col, line */
void g_write(s)
char *s;
{
	int temp;
	
	_s_p_l( strlen(s) + g_th - 89 );
	while (*s) {
		if (*s=='\r'){
			s++;
			g_th=0;
			continue;
		}
		g_putch(*s++,340,g_th++,screen0);
	}
	
	for ( temp=g_th ; temp<90 ; temp++ )
		g_putch(' ',340,temp,screen0);
	
}
void g_putch(c,y,x,scr)
int c,y,x;
unsigned char *scr;
{
	int i;
	
	for (i=0;i<8;i++,y++)
		scr[( 0x2000 * (y&3) + 90*(y>>2) + x)] = font[c*8+i];
	
}
void g_putc(c)
int c;
{
	char s[2];
	
	if (c=='\b'){
		g_th--;
		g_putch(' ',340,g_th,screen0);
		return;
	}
	s[0]=c;
	s[1]=0;
	g_write(s); 
}
void _s_p_l(n)
int n;
{
	int i;
	static int off[8] = {
		0x1de2,0x3de2,0x5de2,0x7de2,0x1e3c,0x3e3c,0x5e3c,0x7e3c };
	
	if (n<=0) return;
	for(i=0;i<8;i++) memcpy(screen0+off[i],screen0+off[i]+n,90-n);
	g_th -= n;
}
int tpr(c)
int c;
{
	if ((c>='a')&&(c<='z')) c ^= 32;
	return(c);
}
void uppername(s)
char *s;
{
	
	while (*s) (*s++)=tpr(*s);
}
void flash()
{
	g1_tv=100;
	g1_th=30;
	g1_write("Use ^S to restore screen.");
	sleep(FLASH_DELAY);
	g1_th=30;
	g1_write("                         ");
}
int options(r)
int r;
{
	char s[40];
	double t,f;
	int be;
	
	if (r==19) { sw_scr(); return(0); }
	if (scr_num) {
		if ((r==3)||(r=='?')) flash();
		return(0);		/* Ignore inputs on screen1. */
	}
	if (nz_ex) {
		nz_ex=0;
		g_write("nZ off. ");
	}
	switch(r){
		case 17: return(17); break;
		case 3:  g_write("(^Q)uit calculation before exit. "); break;
		case 12: lognew(1); break;
		case 23: return(save()); 
		case ' ': eratline(); break;
		case 7:  grid(); return('w');
		case 20: thresh_view(); return('w');
		case '?': soundoff(0); break;
		case 5:  
			nz_ex=1; 
			g_write("nZ on. ");
			break;
		case 2:
			sprintf(s,"Beep alarm: %d New value? ",beep);
			g_write(s);
			be=g_getreply("0123456789\r");
			if (be != '\r') beep=be-48;
			sprintf(s,"Beep alarm=%d ",beep);
			g_write(s);
			break;
		case 24:
			autoexit ^= 1;
			if (autoexit) g_write("autoeXit on. ");
			else g_write("autoeXit off. ");
			break;
		case 18:
			makebat(rcorner,icorner,bottom,aspect,filename,0);
			break;
		case 15:
			g_write("\rExpansion factor? (integer >= 1) ");
			f= ( atoi( g_gets(s,10,"") )  -1.0) /2.0;
			if (f < 0.0) {
				g_write(" Aborted. ");
				break;
			}
			t=((icorner-bottom)*f);
			makebat(rcorner-((((height<<2)/3)*increment)*f), icorner+t, bottom-t, ASPECT, (char *)0, 0 );
			break;
	}
	return(0);
}
int calculate()
{
	register double real,imag,a,b,a2,b2,t;
	register int count;
	int r,j;
	
	g_write(working);
	for(;ii<xlimit;ii++){
		real=rcorner+(ii*increment);
		for (j=0;j<ylimit;j++){
			if (r=inkey()){
				k_timer(0);
				r=options(r);
				k_timer(1);
				switch (r) {
					case 17: return(1);
					case 'w': g_write(working);
				}
			}
			writedot(j,ii);
			imag=icorner-(j*increment);
			a=real;
			b=imag;
			a2=a*a;
			b2=b*b;
			for (count=1;count<maxit;count++){
				t=a;
#ifdef JULIA
				a= a2 - b2 + rjulia ;
				b= 2*t*b + ijulia ;
#else
				a= a2 - b2 + real ;  
				b= 2*t*b + imag ;
#endif
				a2=a*a;
				b2=b*b;
				if ((a2+b2) > 4.0) break;    
			}
			cleardot(screen1,j,ii);
			if (count < maxit) cleardot(screen0,j,ii);
			pdata[ii][j]=count;
		}
	}
	return(0);
}
int hc_calculate()
{
	register double real,imag,a,b,a2,b2,t;
	register int count;
	int r,j,off;
	
	g_write(working);
	for(;ii<xlimit;ii++){
		real=rcorner+(ii*increment);
		for (j=0,off=(-1);j<ylimit;j++){
			if ((j&3)==0) off++;
			if (r=inkey()){
				k_timer(0);
				r=options(r);
				k_timer(1);
				switch (r) {
					case 17: return(1);
					case 'w': g_write(working);
				}
			}
			writedot(j,ii);
			imag=icorner-(j*increment);
			a=real;
			b=imag;
			a2=a*a;
			b2=b*b;
			for (count=1;count<maxit;count++){
				t=a;
#ifdef JULIA
				a= a2 - b2 + rjulia ;
				b= 2*t*b + ijulia ;
#else
				a= a2 - b2 + real ;  
				b= 2*t*b + imag ;
#endif
				a2=a*a;
				b2=b*b;
				if ((a2+b2) > 4.0) break;    
			}
			cleardot(screen1,j,ii);
			if (count < maxit) cleardot(screen0,j,ii);

			count <<= (6 - (j&3)*2 );
			pdata[ii][off++] |= count>>8;
			pdata[ii][off] = count & 0xff ;

		}
	}
	return(0);
}
int getdata(v,h)
int v,h;
{
	int off,val;
	
	off= v+(v>>2);
	
	val= pdata[h][off++]<<8;
	val |= pdata[h][off];
	
	val>>= ( 6 - ((v&3)<<1) );
	return(val & 1023);	
}
int blink_getc(v,h,scr)
int h,v;
unsigned char *scr;
{
	int i,c;
	
	if (h>=90) h=89;
	for(c=0;;){
		for(i=5;i;i--){
			if (c=inkey()) break; 
			sleep(B_DELAY);
		}
		blink(h,v,scr);
		if (c==0) for(i=5;i;i--){
			if (c=inkey()) break; 
			sleep(B_DELAY);
		}
		blink(h,v,scr);
		if (c) return(c);
	}
}
void sleep(t)
int t;
{
	long a;
	
	a=clock();
	while(clock()-a<t)
		;
}

void blink(h,v,scr)
int h,v;
unsigned char *scr;
{
	int i;
	
	for (i=0;i<8;i++,v++)
		scr[0x2000 * (v&3) + 90*(v>>2) + h] ^= 0xff;
}
char *g_gets(s,max,qr)
char *s,*qr;
int max;
{
	int i,j,c;
	
	ESC=0;
	max--;
	for (i=0;i<max;){
		c=blink_getc(340,g_th,screen0) ;
		if (c==13) break;
		if (c==27) { s[0]=0; ESC=1; return(s); }
		if (c==8) {
			if (!i) continue;
			g_putc(8);
			i--;
			continue;
		}
		if ((c<=32)||(c>126)) continue;
		s[i++]=c;
		g_putc(c);
		if (i==1)
			for (j=0;qr[j];j++) if (tpr(c)==qr[j]) {
				s[1]=0;
				return s;
			}
	}
	s[i]=0;
	return(s);
}

char *helptext[4] = { 
"\
MAIN COMMANDS:\n\
\n\
^B  Set/change beeper-alarm.\n\
^C  Exit from program.\n\
^E  nZ.\n\
^G  Enter Grid mode.\n\
^L  Log new drive:directory.\n\
^O  Oversized surrounding frame.\n\
^Q  Quit calculation mode.\n\
^R  Repeat current frame.\n\
^S  Blank/restore screen.\n\
^T  Thresh-view mode.\n\
^U  Resume calculation after ^Q.\n\
^W  Save data file.\n\
^X  Toggle auto-exit.\n\
<space bar> Erase prompt line.\n\n\n",

"\
GRID COMMANDS:\n\
\n\
Number pad keys move crosshairs.\n\
(Number pad 5 is stop.)\n\
\n\
+  Increase speed.\n\
*  Increase speed faster.\n\
-  Reduce speed.\n\
BS (Backspace) Set speed to 1.\n\
.  (Del) Toggle glide.\n\
\n\
^B  Open or enlarge grid window.\n\
^S  Shrink grid window.\n\
^U  Window bottom Up.\n\
^D  Window bottom Down.\n\
^L  Window right-edge Left.\n\
^R  Window right-edge Right.\n\
0   Close grid window.\n\
#   (Crosshatch) Write batch file for present grid window.\n\
^J  Write Julia batch file from Mandelbrot.\n\
\n\
<space bar> Erase prompt line.\n\
ESC  Exit Grid mode.\n\n",

"\
THRESH-VIEW COMMANDS:\n\
A  Auto-draw.\n\
G  Grid.\n\
R  Re-draw screen.\n\
M  Mask shift.\n\
V  View cluster values.\n\
L  Log new drive:directory.\n\
W  Write picture file of present display.\n\
?  Display this screen.\n\
ESC Exit Thresh-view mode.\n\
Cluster may be any number from 0 to $      \n\
=  Equalize top/bottom.\n\
C  Clear present cluster.\n\
M  maxit.\n\
<  maxit less 1.\n\
<backspace> Interrupt screen re-draw.\n\
\nAUTO-DRAW:\n\
To abort: Press Esc at any prompt.\n\
To clear: Enter C at START prompt.\n\
<CR> at FIRST prompt = 0\n\
<CR> at LAST prompt = $        \n\
<CR> at STOP prompt disables stop.\n\
<CR> at START, SIZE or GAP prompt = 1\n",

"\
\tDISPLAY COMMANDS:\n\
\t\n\
\t?B  Beep alarm.\n\
\t?C  Coordinates.\n\
\t?G  Magnification.\n\
\t?H  Horizontal position.\n\
\t?J  Julia point.\n\
\t?M  Maxit.\n\
\t?N  File name.\n\
\t?P  Path.\n\
\t?S  Frame size.\n\
\t?T  Time totals.\n\
\t?X  Autoexit status.\n\
\t?%  Percent completed.\n\
\t?I  Increment.\n\
\t??  Display this screen.",
};

void help(q)
int q;
{
	int i,j;
	char *wp1,*wp2,num[10],s[100];
	
	sprintf(num,"%d",NUMLEVELS-1);
	wp1=strchr(helptext[2],'$');
	wp2=strchr(wp1+1,'$');
	if (wp1 && wp2) for(i=0;num[i];i++,wp1++,wp2++) *wp1=*wp2=num[i];

	sw_scr();
#ifdef JULIA
	sprintf(s,"\014Ejulia from KITTENSOFT. %s.\n\n",version);
#else
	sprintf(s,"\014Emandel from KITTENSOFT. %s.\n\n",version);
#endif
	g1_write(s);
	g1_write(helptext[q]);
	g1_write("\nSEE MANDEL.DOC FOR DETAILED INSTRUCTIONS  \002  PRESS ANY KEY TO RETURN TO ");
	
	switch(q){
		case 0: g1_write("THE PROGRAM. "); 
			i=g1_th;
			j=g1_tv;
			g1_tv=24;
			vline(24,215,36); 
			g1_write(helptext[3]);
			break;
		case 1: g1_write("GRID. "); 
			i=g1_th;
			j=g1_tv;
			g1_tv=24;
			vline(24,215,37); 
			g1_write(helptext[3]);
			break;
		case 2: g1_write("THRESHVIEW. ");
			i=g1_th;
			j=g1_tv;
			hline(8912,0,20);
			hline(11342,0,39);
			hline(21154,0,39);
			hline(29346,0,39);
			hline(13502,0,9);
			break;
	}
	blink_getc(j,i,screen1);
	sw_scr();
	clearscreen(1);
}
void g1_write(s)
char *s;
{
	
	while (*s) {
		if (*s=='\014'){
			clearscreen(1);
			g1_th=g1_tv=0;
			s++;
			continue;
		}
		if (*s=='\n'){
			g1_th=0;
			g1_tv+=12;
			s++;
			continue;
		}
		if (*s=='\t'){
			g1_th=40;
			s++;
			continue;
		}
		g_putch(*s++,g1_tv,g1_th++,screen1);
	}
}

void g1_putchar(c)
int c;
{
	char s[2];
	
	s[0]=c;
	s[1]=0;
	g1_write(s); 
}
int g_getreply(s)
char *s;
{
	char *wp;
	int c;
	
	for(;;){
		g_putc(c=blink_getc(340,g_th,screen0)) ;
		c=tpr(c);
		for(wp=s;*wp;wp++) if (*wp==c) return(c);
	}
}
void writebyte(dat,y,x)   
int x,y,dat;
{
	
	virtscreen[xwidth8*y + x]=dat;
	
	if (y%3==0) return;
	y-=((y/3)+1);

	screen0[0x2000 * (y&3) + 90*(y>>2) + x]=dat;
	
}
#ifdef FREEBLOCKS
void countfree()
{
	int i;
	
	for (i=0;i<512;i++)
		if ((freep[i]=malloc(1024))==0) break;
		
	numfree=i;	
	while (i--) free(freep[i]);
}
#endif

long diskfree()
{
	struct {
		int ax,bx,cx,dx,si,di,ds,es;
	} reg_s;
	
	reg_s.ax = 0x36 << 8;
	reg_s.bx = reg_s.cx = reg_s.dx = reg_s.si = reg_s.di = reg_s.es = reg_s.ds = 0;
	sysint(0x21,&reg_s,&reg_s);
	groupsize = reg_s.ax * reg_s.cx;

	return ((long)reg_s.bx * (long)groupsize);
} 
void lognew(q)
int q;
{
	char s[200],*wp;
	
	if (q==1){
		sprintf(s,"Current path is %c:\\%s  Enter new path: ",drivenum+65,pathname);
		g_write(s);
		g_gets(s,25,"");
		parsepath(s);
	}
	sprintf(s," Path> %c:\\%s ",drivenum+65,pathname);
	g_write(s);
}	
int newpath(s)
char *s;
{
	int q;
	
	if (s) if (chdir(s)==(-1)) return(-1);
	
	if (!getcwd(pathname,PATHLEN-1)) return(-1);
	else return(0);
	
}

void newdrive(d)
int d;
{

	if (d==(-1)){
		drivenum=bdos(0x19,0,0);
		newpath((char *)0);
		return;
	}
	bdos(0xe,d,d);
	newdrive(-1);
}
void parsename(s)
char *s;
{
	char *wp;
	int fd;
	
	uppername(s);

	if (wp=strrchr(s,'\\')) wp++;
	else {
		if (wp=strchr(s,':')) wp++;
		else wp=s;
	}
	strcpy (filename,wp);

	if (wp=index(filename,'.')) *wp=0;
	strcat(filename,".PIK"); 
	if ((strlen(filename)>=NAMELEN)||( ((fd=open(filename,O_WRONLY|O_CREAT))==-1) )){
		printf("\n*** %s not a valid file name.\n",filename);
		ng_help(0);
		exitt(10);
	}
	close(fd);
	if ( filesize(filename)==0L ) unlink(filename);
}
void parsepath(s)
char *s;
{
	char *wp;
	
	uppername(s);
	wp=strrchr(s,':');
	if (wp) {
		if (wp-1 != s) return;
		newdrive(s[0]-65);
		wp++;
	}
	else (wp=s);
	if (*wp) newpath(wp);
}
long filesize(n)
char *n;
{
	long size;
	int fd;
	
	if ((fd=open(n,O_RDONLY))==-1) return(0L);

	size=lseek(fd,0L,2);
	close(fd);
	
	return(size);
}

void exitt(q)
{

#ifdef HOGMEM
	for (;hogmem;hogmem--) free(hog[hogmem-1]);
#endif
	exit(q);
}
/* Aztec C-86 version 4.10b dated 11-12-87 has a bad bug in atof().
The following is my workaround. It is probably inefficient but seems
to be accurate enough for the purposes of this program. */
double atof(s)
char *s;
{
	char 	is[_STRTODLEN_], /* string to hold the integer portion */
		fs[_STRTODLEN_], /* string to hold the fractional portion */
		es[_STRTODLEN_]; /* string to hold the exponent portion */
	double 	ii,		/* the integer part as a double */
		fi,		/* the fractional part as a double */
		efac;		/* a factor, such that e3 becomes efac=1000, e45 becomes ecac=10^45, e-17 becomes efac=10^(-17), etc. */
	int 	ei,		/* the power of 10 */
		flen,		/* the length of the string representing the fractional part */
		sign=1,		/* sign=0 if the number is < 0. */
		esign=1;	/* esign=0 if the exponent is < 0 */
	
	
	is[0]=fs[0]=es[0]= '\0';
	while((*s==' ')||(*s=='\t')) s++;  
	
	switch (*s){
		case '-': sign = 0;
		case '+': s++;
	}
	
	if ((*s)&&(*s != '.')) s= _katof_fill(s,is);
	if (*s == '.') s= _katof_fill(++s,fs);
	if ((*s == 'e')||(*s == 'E')) {
		switch (*(++s)){
			case '-': esign = 0;
			case '+': s++;
		}
		s= _katof_fill(s,es);
	}
	ii= atoi_d(is);
	fi= atoi_d(fs);
	ei= esign ? atoi(es) : -atoi(es);
	
	efac=1.0;
	if (ei>0) for(;ei;ei--) efac *= 10.0;	/* Clumsey. But this is just a make-do function. */
	if (ei<0) for(;ei;ei++) efac /= 10.0;	/* Ditto. */
	
	flen= strlen(fs);
	while(flen--) fi /= 10.0;		/* Ditto. */
	
	if (!sign) efac = -efac;
	return ( (ii + fi) * efac );
}

double atoi_d(s)
char *s;
{
	double n=0.0;
	for (;*s;s++){
		n *= 10.0;
		n += *s - 48;
	}
	return(n);
}
 
char *_katof_fill(from,to)
char *from,*to;
{
	int count=0;
	
	while((*from>='0')&&(*from<='9')&&(++count<_STRTODLEN_)) 
		(*to++)=(*from++);
	*to=0;
	return(from);
}
void mem_full(n)
int n;
{
	int c;
	
	if (autoexit){
		ng_help(5);
		exitt(3);
	}
	while(n%8) free( pdata[--n] );
	xwidth=xlimit=n;
	aspect= (double)ylimit/(double)xlimit;
	wsize= ( (double)xlimit*(double)ylimit )/AREA;

	printf("\007\
*** Insufficient memory. ***\n\
*** Reducing frame size. ***\n\
    New size = %g New aspect ratio = %g\n\n\
    (P)rocede or (A)bort? ",wsize,aspect);
	
	for(;;){
		switch( c=inkey() ){
			case 'a':
			case 'A':
				while(--n>=0) free(pdata[n]);
				free(virtscreen);
				ng_help(3);
				exitt(3);
			case 'p':
			case 'P': return;
			case 0:   break;
			default: putchar(c);
		}
	}
}
double dsqroot(n)
double n;
{
	double r1,r2;
	r1=n=n*1000000.0;
	r2=1.0;

	while (r1>r2+0.00001){
		r1=(r1+r2)/2.0;
		r2=n/r1;
	}
	return(r1/1000.0);
}
void ng_help(q)
int q;
{
	static char *mp[6] = {
		"",
		"\n*** Incorrect number of arguments.\n",
		"\n*** No RAM available.\n",
		" Aborted at user request.\n",
		"\n*** Size/aspect error.\n",
		"\n*** Insufficient memory.\n"
	};
	
	printf("%s\n%s%s.\n%s%s\nSee MANDEL.DOC for instructions.\n",mp[q],_help[0],version,cpyr,_help[1]);
}
hline(v,h1,h2)   /* v is offset of screen line. 
		h1 and h2 are character position, not pixel number. */
int v,h1,h2;
{
	
	for(;h1<=h2;h1++)
		screen1[v+h1] = 0xff;
}
vline(v1,v2,h)   /* v1 and v2 are vertical pixel numbers.
		h is character number, not pixel number. */
int v1,v2,h;
{
	
	for(;v1<=v2;v1++)
		screen1[0x2000 * (v1&3) + 90*(v1>>2) +h]= 24;
}
