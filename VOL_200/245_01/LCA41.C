      
/* (4,1) Linear Cellular Automaton	*/

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

/* G. Cisneros, 4.3.87						*/
/* 10 April 1987 - modified for (4,2) [HVM]			*/
/* 26 April 1987 - Multiple menus [HVM]				*/
/* 28 April 1987 - back modified to (4,1) [HVM]			*/
/* 28 April 1987 - version for XVI Feria de Puebla [HVM]	*/

# include <bdos.h>

# define COLGRAF     4  /* graph resolution */
# define T80X25      3  /* text resolution */
# define WHCYMAG     1  /* color quad for normal screen */
# define AL        320  /* array length (screen width) */
# define DS	    10  /* number of distinct sums */
# define DT	    11  /* DS + 1 */
# define NX         51	/* number of sample rules */

char xrule[] 

	"0001321200"  /* gliders, cycles, changing after 8000 gen */
	"0003120121"  /* fishnet */
	"0020303113"  /* glider from Perry's article */
	"0023003220"  /* barriers and creepers */
	"0003120210"  /* #22 with color runs */

	"0010322132"  /* almost a barrier */
	"0012130032"  /* wiggles back and forth */
	"0012130000"  /* no gliders but class iv */
	"0303200131"  /* varicolored cells, blue barriers */
	"0031202003"  /* wiggles */

	"0031202023"  /* red and black */
	"0133330222"  /* checkerboard */
	"0331202003"  /* insettled */
	"0101212303"  /* reds fight greens */
	"0120133230"

	"0123003220"  /* barrier & irregular creeper */
	"0123022233"  /* green-balck cells */
	"0131123113"  /* gliders */
	"0202311130"  /* marvellous stripes */
	"0203011100"  /* open latticework */

	"0203101121"  /* black vs red-blue */
	"0203210031"  /* */
	"0223112303"  /* */
	"0223112313"  /* */
	"0230130120"  /* cycles and wiggles */

	"0231123002"  /* almost a bouncing shuttle */
	"0231123003"  /* nice variant */
	"1002120031"  /* mottled */
	"1012333130"  /* uniform */
	"1120311321"  /* pea soup */

	"1131301023"  /* threads */
	"1200003011"  /* gliders and walls */
	"1333011002"  /* stripey */
	"2011033320"  /* */
	"3020110010"  /* watch the lattice develop */

	"3021331332"  /* oodles of triangles */
	"3100323113"  /* pntd crct bd */
	"3111203022"  /* interesting barriers */
	"3120111330"  /* varicolored cells w/blue barrier */
	"3232221023"  /* interesting barriers */

	"3301123211"  /* black stripe */
	"3330121210"  /* varied */
	"3331202110"  /* venetian blind */
	"3332020321"  /* checkered table cloth */
	"3332121001"  /* small cells */

	"3332320120"  /* zebra glider */
	"3333120121"  /* another glider */
	"3333120122"  /* two speeds of glider */
	"3333122101"  /* aggressive green & stripes */
	"3333210120"  /* slow gliders */

	"3333220113"  /* triangles vs checkers */

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
int i, j, sum, sum0, sum1;

    videomode(COLGRAF);					/* erase the screen */
    videocursor(0,0,0);					/* top text line */
    scrstr("Rule: ");
    scrstr(rule);
    for (i=0; i<DS; i++) {arule[i] = rule[i]-'0';}
    for (j=8; j<200; j++) videodot(j,AL-1,2);
    for (j=8; j<200; j++) {				/* evolve for 192 generations */
        sum0 = arr1[AL-1] + arr1[0] + arr1[1];
        arr2[0] = arule[sum0];
        for (i=1; i<AL-1; i++) {
            sum = arr1[i-1] + arr1[i] + arr1[i+1];
            arr2[i] = arule[sum];
            };
        sum1 = arr1[AL-2] + arr1[AL-1] + arr1[0];
        arr2[AL-1] = arule[sum1];
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
    scrstr("          *** LIFE in One Dimension ***");
    videocursor(0,6,0);
    scrstr("Four States - Black(0), Cyan(1), Magenta(2), White(3).");
    videocursor(0,8,0);
    scrstr("First neighbors - one on each side, three altogether.");
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
    scrstr("      0..1..2..3");
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
