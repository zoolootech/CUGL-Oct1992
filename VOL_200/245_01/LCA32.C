
/* (3,2) linear cellular automaton	*/
/*					*/
/* Reference:				*/
/*					*/
/*    Kenneth E. Perry			*/
/*    Abstract Mathematical Art		*/
/*    BYTE				*/
/*    December, 1986			*/
/*    pages 181-192			*/

/*    Copyright (C) 1987		*/
/*    Harold V. McIntosh		*/
/*    Gerardo Cisneros S.		*/

/* G. Cisneros, 4.3.87 */
/* 10 April 1987 - Adapted for second neighbors [HVM] */
/* 25 April 1987 - Collection of sample rules [HVM] */
/* 26 April 1987 - Multiple menus [HVM] */
/* 28 April 1987 - version for XVI Feria de Puebla [HVM] */
/* 16 May 1987 - adapted for (3,2) [HVM] */

# include <bdos.h>

# define COLGRAF     4  /* graph resolution */
# define T80X25      3  /* text resolution */
# define WHCYMAG     1  /* color quad for normal screen */
# define AL        320  /* array length (screen width) */
# define DS	    11  /* number of distinct sums */
# define DT 	    12  /* DS + 1 */
# define NX         20	/* number of sample rules */

char xrule[] =

	"00012012000"
	"00021021000"
	"00100221120"
	"00100222120"
	"00102110120"

	"01002110110"
	"01002122012"
	"01010122220"
	"01022100222"
	"02001101100"

	"02010202120"
	"02011120110"
	"02121112202"
	"10022201212"
	"11021000021"

	"20010020211"
	"20101102010"
	"20200011222"
	"20210120222"
	"21000221212"

	;

char  xx[4], rule[DT];
int   arule[DS], arr1[AL], arr2[AL];

main()
{
int c, i, jj, n;
int  more = 'r';

    videopalette(WHCYMAG);				/* white/cyan/magenta */

    tuto();
    while (!kbdst()) jj=rand();				/* wait for keypress */
    c=kbdin();						/* ignore it */
    jj=rand()%NX;
    for (i=0; i<DS; i++) rule[i] = xrule[DS*jj+i];	/* random sample rule */
    rule[DS]=0;
    for (i=0; i<AL; i++) {				/* random initial array */
	if (i%4 == 0) c=rand();
	arr1[i]=c%3; c/=3;};
    videomode(T80X25);
    videoscroll(3,0,4,71,0,3);				/* menu on blue background */
    videoscroll(16,0,21,71,0,3);
    xmenu(jj+1);					/* show initial rule */

    while (more!='n') {					/* execute multiple runs */
    rmenu();
    lmenu();
    while (0<1) {					/* set up one run */
    c=kbdin();
    if (c=='g') break;					/* go draw graph */
    if (c=='q') more='n';				/* quit for good */
    if (more=='n') break;
    switch (c) {
        case 'r':					/* edit rule */	
	    xblnk();
	    edrule();
	    videocursor(0,3,0);
	    rmenu();
	    break;
        case 'l':					/* edit cell string */
	    xblnk();
	    edline();
	    videocursor(0,3,0);
	    lmenu();
	    break;
        case '#':					/* read stored rule */
	    xmenu(NX);
	    n=DS*((i=lim(1,numin(0),NX))-1);
	    xmenu(i);
	    for (i=0; i<DS; i++) rule[i] = xrule[n+i];
	    rmenu();
            break;
        case 'u':
	    xblnk();
	    for (i=0; i<AL; i++) arr1[i]=0;
	    arr1[AL/4]=1;
            arr1[AL/2]=2;
            arr1[(3*AL)/4]=1;
	    lmenu();
            break;
	case 'x':					/* random rule */
	    xblnk();
            for (i=0; i<DS; i++) {
            if (i%4 == 0) c = rand();
            rule[i] = '0'+(c%3);
            c/=3;
            };
	    rmenu();
	    break;
	case 'y':					/* random line */
	    xblnk();
	    for (i=0; i<AL; i++) {
            if (i%4 == 0) c = rand();
            arr1[i]=c%3;
            c/=3;
            };
            lmenu();
	    break;
        default: break;
        };
    };
    if (more=='n') break;
    do {
    evolve(rule);
    videocursor(0,0,0);
    scrstr("More?");
    videocursor(0,0,30);
    scrstr("y/n/cr");
    more=kbdin();
    } while (more=='\015');
    videomode(T80X25);					/* reset the screen */
    if (more=='n') break;
    };
}    

edrule()						/* edit the rule */
{
char c;
int  i;

    videocursor(0,1,6);					/* get the rule */
    i=0;
    while (i<DS) {
        videoputc(rule[i],2);
        videoputc('\010',1);
        c = kbdin();
        if (c == '\015') break;
        switch (c) {
        case '0':  case '1': case '2':			/* state */
            rule[i++] = c;
            videoputc(c,1);
            break;
        case ' ':					/* space = advance */
            videoputc(rule[i++],1);
            break;
        case '\010':					/* backspace */
            if (i==0) break;
            videoputc(rule[i--],1);
            videoputc(c,1);
            videoputc(c,1);
            break;
	default: break;
        };
    };
}

edline() {						/* edit the line */

char c;
int  i, j, k, ii, jj;

    videocursor(0,16,0);
    scrstr("insert states using 0, 1, 2");
    videocursor(0,17,0);
    scrstr("move cursor with n(north), s(south), e(east), w(west), or");
    videocursor(0,18,0);
    scrstr("with keyboard arrows. Space, backspace move right and left.");
    videocursor(0,19,0);
    scrstr("( seeks left margin, < absolutely, { up one line, [ down one line");
    videocursor(0,20,0);
    scrstr(") seeks right margin, > absolutely, } up one line, ] down one line");
    videocursor(0,21,0);
    scrstr("carriage return exits");
    jj=4;
    ii=1;
    while (0<1) {
    ii=lim(1,ii,40);
    jj=lim(1,jj,8);
    j=jj-1;
    i=ii-1;
    videocursor(0,j+6,i);
    c=kbdin();
    if (c == '\015') {videoscroll(16,0,21,70,0,3); break;};
    switch (c) {
    case '0':  case '1': case '2':				/* enter  state */
        arr1[40*j+i] = c-'0';
	ii++;
	break;
    case 's': case '\012': case '\320':		  jj++; break;	/* down - next line */
    case 'n': case '\013': case '\310':		  jj--; break;	/* up   - last line */
    case 'e': case '\014': case '\315': case ' ': ii++; break;	/* space = advance  */
    case 'w': case '\010': case '\313':		  ii--; break;	/* backspace */
    case '<': ii=1;  jj=1;  break;  /* absolute left */
    case '{': ii=1;  jj--;  break;  /* left one row up */
    case '(': ii=1;         break;  /* left this row */
    case '[': ii=1;  jj++;  break;  /* left next row */
    case '>': ii=40; jj=40; break;  /* absolute right */
    case '}': ii=40; jj--;  break;  /* right one row up */
    case ')': ii=40;        break;  /* right this row */
    case ']': ii=40; jj++;  break;  /* right next row */
    default: break;
        };
    videocursor(0,j+6,0);
    for (k=0; k<40; k++) videoputc('0'+arr1[40*j+k],1);
    };
}

evolve(rule)						/* display a screen of evolution */
char *rule;
{
int i, j, sum, sum0, sum1, sum2, sum3;

    videomode(COLGRAF);					/* erase the screen */
    videocursor(0,0,0);					/* top text line */
    scrstr("Rule: ");
    scrstr(rule);
    for (i=0; i<DS; i++) {arule[i] = rule[i]-'0';}
    for (j=8; j<200; j++) videodot(j,AL-1,2);
    for (j=8; j<200; j++) {				/* evolve for 192 generations */
        sum0 = arr1[AL-2] + arr1[AL-1] + arr1[0] + arr1[1] + arr1[2];
        sum1 = arr1[AL-1] + arr1[0] + arr1[1] + arr1[2] + arr1[3];
        arr2[0] = arule[sum0];
        arr2[1] = arule[sum1];
        for (i=2; i<AL-2; i++) {
            sum = arr1[i-2] + arr1[i-1] + arr1[i] + arr1[i+1] + arr1[i+2];
            arr2[i] = arule[sum];
            };
        sum2 = arr1[AL-3] + arr1[AL-2] + arr1[AL-1] + arr1[0] + arr1[1];
        sum3 = arr1[AL-4] + arr1[AL-3] + arr1[AL-2] + arr1[AL-1] + arr1[0];
        arr2[AL-1] = arule[sum2];
        arr2[AL-2] = arule[sum3];
        for (i=0;  i<AL; i++) {videodot(j,i,arr1[i]); arr1[i] = arr2[i];};
	if (kbdst()) {kbdin(); break;};
        }
}

tuto()							/* tutorial and Help screen */
{
    videomode(T80X25);
    videocursor(0,2,0);
    scrstr("<Copyright (C) 1987 - H.V. McIntosh, G. Cisneros S.>");
    videocursor(0,4,0);
    scrstr("        ***** LIFE in One Dimension *****");
    videocursor(0,6,0);
    scrstr("hree States - Black(0), Cyan(1), Magenta(2).");
    videocursor(0,8,0);
    scrstr("Second neighbors - two on each side, five altogether.");
    videocursor(0,10,0);
    scrstr("Totalistic transition rule - random, edited, or stored.");
    videocursor(0,12,0);
    scrstr("Initial Cellular Array - random, edited, or patterned.");
    videocursor(0,14,0);
    scrstr("Some rules are fragile and require several tries before");
    videocursor(0,15,0);
    scrstr("manifesting an interesting evolutionary pattern.");
    videocursor(0,17,0);
    scrstr("Use any key to terminate a display in progress.");
    videocursor(0,21,0);
    scrstr("now, press any key to continue ...");
}

rmenu() {						/* rule menu */
    videocursor(0,0,0);
    scrstr("      0....1....2");
    videocursor(0,1,0);
    scrstr("Rule: "); scrstr(rule);
    videocursor(0,3,0);
    scrstr("    r(rule), l(line), #nn(stored rule), g(graph), q(quit)");
    videocursor(0,4,0);
    scrstr("             x(random rule), y(random line), u(unit line).");
    videocursor(0,3,0);
    }

lmenu() {						/* line menu */
int i, j;
    for (j=0; 40*j<AL; j++) {
	videocursor(0,6+j,0);
	for (i=0; i<40; i++) videoputc('0'+arr1[40*j+i],1);
	}
    videocursor(0,3,0);
    }

xmenu(n) int n;						/* display rule number */
{int i, nn;  
    nn=sprintf(xx,"%3d",n);
    videocursor(0,1,30);
    for (i=0; i<nn; i++) videoputc(xx[i],1);
    videocursor(0,1,30);
}

xblnk() {						/* clear rule number */
    videocursor(0,1,30);
    scrstr("    ");
    videocursor(0,3,0);
}

int lim(i,j,k)
int i, j, k;						/* limit j to interval (i,k) */
    {if (i>=j) return i; if (k<=j) return k; return j;}

scrstr(s)						/* write a string in graphics mode */
char *s;
{ for (; *s != '\0'; s++) videoputc(*s,1); }

int kbdst()						/* keyboard status */
{return(bdos(11) & 0xFF);}

kbdin() {						/* direct keyboard input, no echo */
int c;
    if ((c = bdos(7) & 0xFF) == '\0') c = (bdos(7) & 0xFF) | 0x80;
    return(c);
}

int numin(n)						/* read number from keyboard */
int n; {char c;
    c=kbdin();
    if (c>='0'&&c<='9') return(numin(10*n+(c-'0'))); else return(n);
}

/* end */
