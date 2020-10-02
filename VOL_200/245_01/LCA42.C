
/* (4,2) linear cellular automaton	*/
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

# include <bdos.h>

# define COLGRAF     4  /* graph resolution */
# define T80X25      3  /* text resolution */
# define WHCYMAG     1  /* color quad for normal screen */
# define AL        320  /* array length (screen width) */
# define DS	    16  /* number of distinct sums */
# define NX         91	/* number of sample rules */

char xrule[] =

	"0000012233331111"  /* interesting background */
	"0000021232311000"  /* nice gliders & 2-sided cycles */
	"0000112232211000"  /* class iv stills */
	"0000121232310000"  /* cycles */
	"0000122313221000"  /* small black, big green blotches */

	"0001310000012100"  /* class iv w/zigzag */
	"0010321000321010"  /* glider class iv dies */
	"0010032000132011"  /* binary fission */
	"0010102010332233"  /* slow red glider */
	"0010132000231011"  /* */

	"0010230200100001"  /* small class iv periodic */
	"0012030200100220"  /* localized */
	"0010132000132011"  /* interesting binary fission */
	"0013331303033023"  /* very delicate - blue background */
	"0023111031322011"  /* dies quickly class iv */

	"0023323310033133"  /* more regular but delicate */
	"0100002323111111"  /* red automaton various barriers */
	"0100002232311111"  /* barriers and red (2,1) */
	"0100010323212121"  /* interesting mixed */
	"0100021121101101"  /* natural barriers */

	"0100021123301101"  /* strong & fragile barriers */
	"0100320333033113"  /* good vs evil */
	"0102220002123003"  /* noteworthy */
	"0110001323211113"  /* cycles */
	"0110013230203233"  /* blue vs red - striking */

	"0110031111222333"  /* blue-green split in red foreg. */
	"0120330111101331"  /* also delicate */
	"0010321000321010"  /* long period reflector - glider */
	"0020013111222033"  /* glider on red background */
	"0020221113030110"  /* a new style */

	"0100002323111001"  /* red systeb between barriers */
	"0100002323211121"  /* more colorful */
	"0100012222233300"  /* green and red */
	"0100121233320000"  /* blue triangles w/red */
	"0100122233331111"  /* crazy stripes */

	"0100211001022220"  /* stills & gliders, no blue */
	"0100311022223330"  /* neat, try variations */
	"0110012222233331"  /* green and red compete */
	"0110122033301111"  /* even crazier stripes */
	"0111003221022222"  /* rec w/other colors struggling */

	"0111003222102333"  /* variant w/blue cores /*
	"0133333112111220"  /* bears looking at again */
	"0200010323003133"  /* conflict of blue vs black */
	"0200012303223303"  /* black vs blue */
	"0200001323211112"  /* gliders and cycles */

	"0200122102233233"  /* unstable */
	"0200110123123010"  /* class iv w/red background */
	"0201212113312303"  /* delicate */
	"0210012222223331"  /* big green and red compete */
	"0220010323203233"  /* blue background w/cycl & glidr */

	"0230013022200303"  /* class iv on green */
	"0230013022200313"  /* less prolific & dies out */
	"0232020000100012"  /* green dominated by blue */
	"0200211301023220"  /* black spots */
	"0310020330100001"  /* interesting zigzag */

	"0310020101103102"  /* */
	"0310020101103122"  /* jungle pyramids */
	"0310102020230000"  /* still on green */
	"0313320330100001"  /* */
	"0310020101103122"  /* jungle pyramids */

	"0100211001022220"  /* stills and gliders */
	"1002033112212122"  /* cycles on green background; slow glider */
	"1200012222233000"  /* dying gliders & glider generator */
	"1202000213121123"  /* gliders on striped background */
	"1210012222233000"  /* also interesting - dying gliders */

	"1211133030313323"  /* glider on blue */
	"1222330131213111"  /* another delicate */
	"1222330131213113"  /* similar but jitters */
	"2000013111213333"  /* blue glider on black */
	"2013300233223300"  /* so-so cycles */

	"2110013112033331"  /* wedge diminishes to glider */
	"2120320203223021"  /* glider & local */
	"2120320203223023"  /* glider and local variant */
	"2200013112033331"  /* variant */	
	"2213331303033023"  /* delicate but more prolific */

	"2222003031000213"  /* green and black strips & stills */
	"2222003031000210"  /* has a bouncing glider */
	"2300013111213330"  /* macroscopic glider */
	"2313113000001333"  /* vivid */
	"2322022110002201"  /* stripey class iv */

	"3003003300123120"  /* class iv blue/black monochrome */
	"3030111032310222"  /* class iv cycles - dies quickly */
	"3031300300330012"  /* some triangle conflicts */
	"3110220211131302"  /* chinese lantern */
	"3100002022011123"  /* striking reiangles in matrix */

	"3120202022011123"  /* blue triangles in green matrix */
	"3122320211331223"  /* pleasant */
	"3223212330100010"  /* be patient - it regularizes */
	"3312020131133323"  /* interesting barrier conflict */
	"3320001132030011"  /* 3 color cycle w/glider */

	"2332212222200001"  /* triangles */

	;

char  xx[4], rule[DS+1];
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
	arr1[i]=c&3; c>>=2;};
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
            arr1[(3*AL)/4]=3;
	    lmenu();
            break;
	case 'x':					/* random rule */
	    xblnk();
            for (i=0; i<DS; i++) {
            if (i%4 == 0) c = rand();
            rule[i] = '0'+(c & 3);
            c >>= 2;
            };
	    rmenu();
	    break;
	case 'y':					/* random line */
	    xblnk();
	    for (i=0; i<AL; i++) {
            if (i%4 == 0) c = rand();
            arr1[i] = c & 3;
            c >>= 2;
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
        case '0':  case '1': case '2': case '3':	/* state */
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
    scrstr("insert states using 0, 1, 2, 3");
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
    case '0':  case '1': case '2': case '3':		/* enter  state */
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
    scrstr("Four States - Black(0), Cyan(1), Magenta(2), White(3).");
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
    scrstr("      0....1....2....3");
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
