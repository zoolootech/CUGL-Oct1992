/*
TITLE:		GEARS;
DATE:		9/8/88;
DESCRIPTION:	"Bicycle gear chart calculator.";
VERSION:	1.00;
KEYWORDS:	Bicycle, Gears;
FILENAME:	GEARS.C;
WARNINGS:	"Uses Aztec scr_ functions";
SEE-ALSO:	GEARS.EXE, GEARS.DOC;
SYSTEM:		MS-DOS;
COMPILERS:	Aztec;
AUTHORS:	Dan Schechter;
 */

/* GEARS is a bicycle gear chart calculator.
To port it to other systems/compilers, note the following:
All the functions beginning scr_ are Aztec C IBM-specific functions.
They require 100% PC-compatible computers. Their actions are the following:

scr_getc() causes program operation to halt until a key is pressed
and then returns that key. It is not necessary to press ENTER as would
be required in the case of getchar().

scr_clear() clears the screen and places the cursor at the upper left.

scr_curs(v,h) places the cursor at position v,h on the screen. Both
vertical and horizontal positions are numbered from 0.

scr_eol() erases from the current cursor position to the end of the line and 
scr_eos() erases from the current cursor position to the end of the screen. 

I presume that similar functions should be available (or easy to write)
on any system and any compiler. Good luck!
 */

#define VERSION "Version 1.00 9/8/88"

#define CHAINNUM 2		/* Default number of chain rings. */
#define MAXCHAIN 3		/* Maximum number of chain rings. */
#define FREENUM 6		/* Default number of freewheel sprockets. */
#define MAXFREE 8		/* Maximum ditto. */
#define CHAINCOL 0		/* Sxcreen column for chain cog numbers. */
#define FREELINE 7		/* Screen line for freewheel cog numbers. */
#define SLACKLINE 5		/* Screen line for slack. */
#define SLACKCOL 25		/* Screen column for slack. */
#define WHEEL 27		/* Default wheel size. */
#define HSTEP 7			/* Horizontal screen spacing. */
#define VSTEP 2			/* Vertical screen spacing. */

#define LEFT 203	/* char returned by scr_getc() on left arrow. */
#define RIGHT 205	/* ditto for right arrow. */
#define UP 200		/* ditto for up arrow. */
#define DOWN 208	/* ditto for down arrow. */
				
				/* If your system does not have arrow
				keys you can replace the above 4 with
				whatever keyboard characters you like. */

#define BS '\b'
#define TAB '\t'
#define A_CHAR '>'		/* Character to indicate auto-advance. */
#define XA_CHAR	'<'		/* Char to indicate no auto-advance. */

int chainnum=CHAINNUM,freenum=FREENUM,wheel=WHEEL;
int autoadv=1;

/* The following uses IBM character graphics to draw a cat. 
For non-IBM systems use this line instead:
char *cat = "";
 */
 
unsigned char cat[44] = {
	0x20, 0x20, 0x20, 0x20, 0x2f, 0x5c, 0x5f, 0x2f, 0x5c, '\n',
	0x20, 0x20, 0x20, 0x28, 0xfe, 0x20, 0xd2, 0x20, 0xfe, 0x29, '\n',
	0xf0, 0xf0, 0xf0, 0xf0, 0x20, 0xc4, 0xca, 0xc4, 0x20, 0xf0, 
	      0xf0, 0xf0, 0xf0, '\n',
	0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x22, 0x22, 0x00
};

/* If your compiler complains about long strings, you may have to break
up the following help message into two or three separate chunks. */

char *help_msg = 
"\nUsage: GEARS [w<w>] [c<c>] [f<f>]\n"
"<w> is wheel size,\n"
"<c> is number of chain rings,\n"
"<f> is number of freewheel gears.\n\n"
"You must list freewheel sprocket numbers across the top, \n"
"smallest on the left and largest on the right,\n"
"and chain ring sprocket numbers along the side, largest on top.\n"
"Crossover gears are marked with an asterisk in the chart\n"
"and are deleted from the numerical-order list below the chart.\n\n"
"Use ENTER or BACKSPACE to move the cursor to the next or previous gear.\n"
"Use left-arrow and right-arrow to move cursor between digits of a gear.\n"
"Use TAB to toggle auto-advance.\n"
"Press X to exit.\n"
"Use PrtScr to print screen display.\n"
;

int main(n,arg)
int n;
char **arg;
{
	int v,h,index,position;	/* index is present cog, position is 0 or 1 */
	char datastring[(MAXCHAIN+MAXFREE)*2];
	int c;
	void help(),display();
	
	while (n>1){
		n--;
		switch(tlr(arg[n][0])){
			case 'w':
				wheel=atoi(arg[n]+1);
				break;
			case 'c':
				chainnum=atoi(arg[n]+1);
				break;
			case 'f':
				freenum=atoi(arg[n]+1);
				break;
			case '?':
				printf(help_msg);
				exit(0);
			default:
				printf("Unrecognized parameter: %s",arg[n]);
				exit(1);
		}
	}
	if ((wheel<=0)||(wheel>50)){
		printf ("Wheel size out of bounds.");
		exit(1);
	}
	if ((chainnum<=0)||(chainnum>MAXCHAIN)){
		printf("Number of chain rings out of bounds.");
		exit(1);
	}
	if ((freenum<=0)||(freenum>MAXFREE)){
		printf("Number of freewheel gears out of bounds.");
		exit(1);
	}
	
	scr_clear();
	
	printf("%s\nKittensoft bicycle gear chart calculator. %s\nWheel is %d inches.",cat,VERSION,wheel);
	
	for(h=0;h<(chainnum+freenum)<<1;h++) datastring[h]=' ';
	display(datastring);
	scr_curs(FREELINE,CHAINCOL+HSTEP);
	for(position=index=0;;){
		switch(tlr(c=scr_getc())){
			case 'x':
			case 3:
				scr_curs(FREELINE+VSTEP*chainnum+7,0);
				exit(0);
			case DOWN:
			case '\r':
				position=0;
				index++;
				if (index==chainnum+freenum) index=0;
				break;
			case TAB:
				autoadv ^= 1;
				scr_curs(FREELINE,0);
				putchar( autoadv ? A_CHAR : XA_CHAR );
				break;
			case UP:
			case BS:
				position=0;
				index--;
				if (index==-1) index=chainnum+freenum-1;
				break;
			case LEFT:
				if (position) position=0;
				break;
			case RIGHT:
				if (!position) position=1;
				break;
			case '?':
			case '/':
				help(datastring);
				break;
			default:
				if (((c<'0')||(c>'9'))&&(c!=' ')) break;
				datastring[(index<<1) + position]=c;
				if (!position) position=1;
				else {
					position=0;
					if (autoadv){
						index++;
						if (index==chainnum+freenum) index=0; 
					}
				}
				display(datastring);
				break;
		}
		if (index<freenum){
			v=FREELINE;
			h= CHAINCOL + HSTEP + index*HSTEP + position;
		}
		else {
			h=CHAINCOL+position;
			v= FREELINE + VSTEP + VSTEP*(index-freenum);
		}
		scr_curs(v,h);
	}
}
int cmp(a,b)
double *a,*b;
{
	if ((*a)==(*b)) return 0;
	return( ((*a)<(*b))? 1:-1 );
}
int getsprocket(s,i)
int i;
char *s;
{
	int n;

	i<<=1;
	if ((s[i]==' ')&&(s[i+1]==' ')) return(-1);
	
	n= s[i++]-48;
	if (n<0) n=0;
	
	if (s[i]==' ') return n;
	n*=10;
	n+= s[i]-48;
	
	return n;
}
double calculate(c,f)
int c,f;
{

	return (((double)c/(double)f)*(double)wheel);
}
void help(s)
char *s;
{
	int h,v;
	
	scr_clear();
	printf(help_msg);
	printf("\nPRESS ANY KEY TO RETURN TO THE PROGRAM. ");
	scr_getc();
	scr_clear();
	printf("%s\nKittensoft bicycle gear chart calculator. %s\nWheel is %d inches.",cat,VERSION,wheel);
	display(s);
}
int tlr(c)
int c;
{
	if ((c>='A')&&(c<='Z')) c|=32;
	return c;
}

void display(s)
char *s;
{
	int i,j,v,h,count,q;
	int chain,free,cmp();
	int chains[MAXCHAIN],frees[MAXFREE];
	double t,tt,g[MAXCHAIN*MAXFREE],calculate(int,int);
	
	for(i=j=0,v=FREELINE;i<freenum;i++,j+=2){
		h=CHAINCOL + HSTEP + i*HSTEP;
		scr_curs(v,h);
		if ((s[j]==' ')&&(s[j+1]==' ')) printf("? ");
		else {
			putchar(s[j]);
			putchar(s[j+1]);
		}
	}
	for(i=0,h=CHAINCOL;i<chainnum;i++,j+=2){
		v=FREELINE + VSTEP + i*VSTEP;
		scr_curs(v,h);
		if ((s[j]==' ')&&(s[j+1]==' ')) printf("? ");
		else {
			putchar(s[j]);
			putchar(s[j+1]);
		}
	}
	for(i=count=0;i<freenum;i++){
		h= CHAINCOL + HSTEP + i*HSTEP;
		free=frees[i]= getsprocket(s,i);
		for(j=q=0;j<chainnum;j++,q=0){
			v= FREELINE + VSTEP + j*VSTEP;
			scr_curs(v,h);
			printf("       \b\b\b\b\b\b\b");
			chain=chains[j]=getsprocket(s,freenum+j);
			if (((i==freenum-1)&&(!j))||((!i)&&(j==chainnum-1))){
				putchar('*');
				q=1;
			}
			if ((chain<=0)||(free<=0)) putchar('?');
			else {
				t=calculate(chain,free);
				if (!q) g[count++]=t;
				printf("%-5.1f",t);
			}
		}
	}
	h=CHAINCOL + HSTEP + 4 + freenum*HSTEP;
	for (i=1;i<chainnum;i++){
		v= FREELINE + i*VSTEP +1;
		scr_curs(v,h);
		t=getsprocket(s,freenum+i);
		tt=getsprocket(s,freenum+i-1);
		if ((tt>0.0)&&(t>0.0))
			printf("%.0f%%", ((tt-t)/tt)*100.0 );
		else putchar('?');
		scr_eol();
	}
	v=FREELINE + VSTEP +chainnum*VSTEP;
	for(i=1;i<freenum;i++){
		h=CHAINCOL + i*HSTEP + HSTEP/2 +1;
		scr_curs(v,h);
		t=getsprocket(s,i-1);
		tt=getsprocket(s,i);
		if ((tt>0.0)&&(t>0.0))
			printf("%-.0f%%     ", ((tt-t)/tt)*100.0);
		else printf("?     ");
	}
	printf("\n\n");
	qsort(g,count,sizeof(double),cmp);
	for (i=0;i<count;i++) printf("%7.1f",g[i]);
	scr_eol();
	printf("\n   ");
	for(i=1;i<count;i++)
		printf("%6.0f%%",((g[i-1]-g[i])/g[i-1])*100.0);
	scr_eol();
	scr_eos();
	for(i=free=q=0;i<freenum-1;i++) {
		if ((frees[i]<=0)||(frees[i+1]<=0)) q=1;
		else if (frees[i]>frees[i+1]) free=1;
	}
	for(i=chain=0;i<chainnum-1;i++) {
		if ((chains[i]<=0)||(chains[i+1]<=0)) q=1;
		else if (chains[i]<chains[i+1]) chain=1;
	}
	scr_curs(0,12);
	if (free) printf("Warning: freewheel out of order. ");
	if (chain) printf("Warning: chain ring out of order.");
	scr_eol();
	scr_curs(SLACKLINE,SLACKCOL);
	if (q) printf("Chart incomplete.");
	else if ((!free)&&(!chain))
		printf("Slack %d", (chains[0]+frees[freenum-1])-(chains[chainnum-1]+frees[0]));
	scr_eol(); 
	scr_curs(FREELINE,0);
	putchar( autoadv ? A_CHAR : XA_CHAR );
	
}
