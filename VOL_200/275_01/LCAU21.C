
/* lcau21.c				*/
/* (2,1) Linear Cellular Automaton	*/

/* Reference:				*/
/*					*/
/*    Kenneth E. Perry			*/
/*    Abstract Mathematical Art		*/
/*    BYTE				*/
/*    December, 1986			*/
/*    pages 181-192			*/

/*    Copyright (C) 1987		*/
/*    Copyright (C) 1988		*/
/*    Harold V. McIntosh		*/
/*    Gerardo Cisneros S.		*/

/* G. Cisneros, 4.3.87						*/
/* 10 April 1987 - modified for (4,2) [HVM]			*/
/* 26 April 1987 - Multiple menus [HVM]				*/
/* 28 April 1987 - back modified to (4,1) [HVM]			*/
/* 28 April 1987 - version for XVI Feria de Puebla [HVM]	*/
/* 14 May 1987 - modified for (3,1) and general rule [HVM]	*/
/* 19 May 1987 - modified for (2,2) [HVM]			*/
/* 20 May 1987 - modified for (2,3) [HVM]			*/
/* 21 May 1987 - Wolfram rule number [HVM]			*/
/* 8 June 1987 - general rule for (4,1) [HVM]		 	*/
/* 12 June 1987 - cartesian product of (2,1) rules [HVM]	*/
/* 12 June 1987 - (2,1) rule with memory  [HVM]			*/
/* 14 June 1987 - individual cycles of evolution [HVM]		*/
/* 17 June 1987 - p-adic representation in plane [HVM]		*/
/* 22 June 1987 - 2 speed gliders via 16 transitions [HVM]	*/
/* 26 June 1987 - push, pop the rule [HVM]			*/
/* 26 June 1987 - conserve position of rule cursor [HVM]	*/
/* 27 June 1987 - incremental rule construction [HVM]		*/
/* 29 June 1987 - conserve position of cell pointer [HVM]	*/
/* 30 June 1987 - mark & unmark transitions, xchg x&X [HVM]	*/
/* 25 July 1987 - display and edit de Bruijn diagrams [HVM]	*/
/* 27 July 1987 - graph of transition probabilities [HVM]	*/
/* 4 September 1987 - PROB21.C for option 't' [HVM]		*/
/* 21 October 1987 - program disks disappeared			*/
/* 20 December 1987 - program reconstructed from listings	*/
/* 20 February 1988 - RIJN21.C for option 'd' [HVM]		*/

# include <bdos.h>

# define COLGRAF     4  /* graph resolution			*/
# define T80X25      3  /* text resolution			*/
# define WHCYMAG     1  /* color quad for normal screen		*/
# define YELREGR     2  /* alternative color palette 		*/
# define AL        320  /* array length (screen width)		*/
# define BD	    12	/* maximum number Bernstein coeffs	*/ 
# define TS	     4  /* distinct sums w/totalistic rule	*/
# define DS	     8  /* number of distinct neighborhoods	*/
# define KK	     2  /* number of states per cell		*/
# define NX          7	/* number of sample rules		*/

char xrule[NX][KK][KK][KK];

char ixrule[NX][DS]=

	"00110011",	/* interfaces of 2 vel	*/
	"00110011",
	"00111011",	/* mottled background	*/
	"00111001",	/* (0*102211200)* still */
	"01101000",	/* Rule 22		*/
	"10010010",	/* Rule 73		*/
	"11111101"	/* 2 glider on 1 bkgrnd	*/

	;

char   rule[DS+1], ascrule[KK][KK][KK];
char   trule[TS]="0000";
int    binrule[KK][KK][KK], arule[DS], arr1[AL], arr2[AL];
int    stat[KK], stal, stac, star;
int    ix0, iy0;					/* origin for pen moves */
double wmul[KK], wmvl[KK];				/* left mass point */
double wmur[KK], wmvr[KK];				/* right mass point */
double bp[BD];

main()
{
int  i, j, i0, i1, i2;
int  more = 'r';
char a, b, c;

for (i=0; i<NX; i++) {					/* copy to 3-index array */
i0=0; i1=0; i2=0;
for (j=0; j<DS; j++) {
  xrule[i][i0][i1][i2]=ixrule[i][j];
  i2++;
  if (i2>KK-1) {i2=0; i1++;};
  if (i1>KK-1) {i1=0; i0++;};
  if (i0>KK-1) {i0=0; };
};};


    videopalette(WHCYMAG);				/* white/cyan/magenta */

    tuto();
    while (!kbdst()) rand();				/* wait for keypress */
    kbdin();						/* ignore it */
    videomode(T80X25);
    videoscroll(3,0,5,71,0,3);				/* menu on blue background */
    videoscroll(19,0,24,71,0,3);
    xtoasc(rand()%NX);
    rule[DS]=0;
    ranlin();						/* random initial array */

    while (more!='n') {					/* execute multiple runs */
    rmenu();
    lmenu();
    while (0<1) {					/* set up one run */
    c=kbdin();
    if (c=='g') break;					/* go draw graph */
    if (c=='q') more='n';				/* quit for good */
    if (more=='n') break;
    switch (c) {
	case '@':					/* numbered tot rule */
	    nutoto(numin(0));
	    totoasc();
	    rmenu();
	    videocursor(0,4,0);
	    break;
	case '$':					/* dozen totalistics */
	    j=numin(0);
	    for (i=0; i<12; i++) {
	      nutoto(j+i);
	      totoasc();
	      ranlin();
	      evolve();
	      };
	    videomode(T80X25);
	    rmenu();
	    lmenu();
	    break;
	case 'T':					/* totalistic rule */
	    xblnk();
	    tmenu();
	    edtrule();
	    totoasc();
	    for (i0=0; i0<KK; i0++) {
	    for (i1=0; i1<KK; i1++) {
	    for (i2=0; i2<KK; i2++) {
	    ascrule[i0][i1][i2]=trule[i0+i1+i2];
	    };};};
	    videocursor(0,4,0);
	    rmenu();
	    xmenu(totonu(0));
	    break;
	case 't':					/* probability calculation */
	    edtri();
	    rmenu();
	    lmenu();
	    break;
        case 'r':					/* edit rule */	
	    xblnk();
	    edrule();
	    videocursor(0,4,0);
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
	    xtoasc(lim(1,numin(0),NX)-1);
	    rmenu();
            break;
	case 'D':					/* run through samples */
	    for (i=0; i<NX; i++) {
	      xmenu(i);
	      xtoasc(i);
	      ranlin();
	      evolve();
	      };
	    videomode(T80X25);
	    rmenu();
	    break;
	case 'd':					/* de Bruijn diagram */
	    edijn();
	    rmenu();
	    lmenu();
	    break;
        case 'u':					/* sparse init arry */
	    xblnk();
	    for (i=0; i<AL; i++) arr1[i]=0;
	    arr1[AL/4]=1;
            arr1[AL/2]=1;
            arr1[(3*AL)/4]=1;
            arr1[(3*AL)/4+2]=1;
	    lmenu();
            break;
	case 'w':					/* Wolfram rulw # */
	    i=numin(0);
	    wmenu(i);
	    for (i0=0; i0<KK; i0++) {
	    for (i1=0; i1<KK; i1++) {
	    for (i2=0; i2<KK; i2++) {
	      ascrule[i0][i1][i2]='0'+i%2;
	      i/=2;
	    };};};
	    rmenu();
	    break;
	case 'x':					/* random rule */
	    xblnk();
	    for (i0=0; i0<KK; i0++) {
	    for (i1=0; i1<KK; i1++) {
	    for (i2=0; i2<KK; i2++) {
	      if ((KK*(KK*i0+i1)+i2)%4 == 0) i=rand();
	      ascrule[i0][i1][i2]='0'+i%2;
	      i/=2;
	    };};};
	    rmenu();
	    break;
	case 'y':					/* random line */
	    xblnk();
	    ranlin();
            lmenu();
	    break;
	case 'Y':					/* symmetrize rule */
	    for (i0=0; i0<KK; i0++) {
	    for (i1=0; i1<KK; i1++) {
	    for (i2=0; i2<KK; i2++) {
	    ascrule[i2][i1][i0]=ascrule[i0][i1][i2];      
	    };};};
	    break;
	case 'B':					/* begin barrier */
	    a=kbdin();
	    b=kbdin();
	    ascrule[0][a-'0'][b-'0']=a;
	    ascrule[1][a-'0'][b-'0']=a;
	    ascrule[2][a-'0'][b-'0']=a;
	    rmenu();
	    break;
	case 'E':					/* end barrier */
	    a=kbdin();
	    b=kbdin();
	    ascrule[a-'0'][b-'0'][0]=b;
	    ascrule[a-'0'][b-'0'][1]=b;
	    ascrule[a-'0'][b-'0'][2]=b;
	    rmenu();
	    break;
	case 'L':					/* left glider link */
	    a=kbdin();
	    b=kbdin();
	    c=kbdin();
	    ascrule[a-'0'][b-'0'][c-'0']=c;
	    rmenu();
	    break;
	case 'R':					/* left glider link */
	    a=kbdin();
	    b=kbdin();
	    c=kbdin();
	    ascrule[a-'0'][b-'0'][c-'0']=a;
	    rmenu();
	    break;
	case 'S':					/* still life link */
	    a=kbdin();
	    b=kbdin();
	    c=kbdin();
	    ascrule[a-'0'][b-'0'][c-'0']=b;
	    rmenu();
	    break;
	case '.':					/* one cycle of evolution */
	    asctobin();
	    onegen(AL);
	    lmenu();
	    break;
	case '=':
	    for (i=1; i<8;  i++) {
	    for (j=0; j<40; j++) arr1[40*i+j]=arr1[j];};
	    lmenu();
	    break;
	case '~':
	    for (i=1; i<16;  i++) {
	    for (j=0; j<20; j++) arr1[20*i+j]=arr1[j];};
	    lmenu();
	    break;
        default: break;
        };
    };
    if (more=='n') break;
    do {
    evolve();
    videocursor(0,0,0);
    scrstr("More?");
    videocursor(0,0,34);
    scrstr("y/n/cr");
    more=kbdin();
    } while (more=='\015');
    videomode(T80X25);					/* reset the screen */
    if (more=='n') break;
    };
  videomode(T80X25);}    

/* edit the rule */
edrule() {
char c;
int  i, i0, i1, i2;

i=6; i0=0; i1=0, i2=0;
    while (0<1) {
        videocursor(0,3,i);
        c = kbdin();
        if (c == '\015') break;				/* carriage return exits */
        switch (c) {
        case '0':  case '1':				/* state */
	    ascrule[i0][i1][i2] = c;
    	    i2++;
	    if (i2>KK-1) {i2=0; i1++;};
	    if (i1>KK-1) {i1=0; i0++;};
	    if (i0>KK-1) {i2=0; };
            videocattr(0,c,3,1);
            if (i<6+DS) i++;
            break;
        case ' ': case '\315':				/* space = advance */
    	    i2++;
	    if (i2>KK-1) {i2=0; i1++;};
	    if (i1>KK-1) {i1=0; i0++;};
	    if (i0>KK-1) {i2=0; };
            if (i<6+DS) i++;
            break;
        case '\010': case '\313':			/* backspace */
	    if (i2!=0) i2--; else {i2=KK-1;
	    if (i1!=0) i1--; else {i1=KK-1;
	    if (i0!=0) i0--; else {i0=KK-1;
	    };};};
            if (i>6) i--;
            break;
	default: break;
        };
    };
}

/* edit totalistic rule */
edtrule() {char c; int  i;
    i=0;
    while (i<TS) {
	c=trule[i];
	videocursor(0,3,56+i);
        videocattr(0,c,3,1);
        c = kbdin();
        if (c == '\015') break;
        switch (c) {
          case '0': case '1':				/* state */
            trule[i]=c;
            videocattr(0,c,5,1);
	    i++;
            break;
          case ' ': case '\315':			/* space = advance */
	    i++;
            break;
          case '\010': case '\313':			/* backspace */
            if (i!=0) i--;
	    break;
	  default: break;
          };
    };
}

/* edit the line */
edline() {
char c;
int  i, j, k, ii, jj;

    videocursor(0,19,0);
    scrstr("insert states using 0, 1,");
    videocursor(0,20,0);
    scrstr("move cursor with n(north), s(south), e(east), w(west), or");
    videocursor(0,21,0);
    scrstr("with keyboard arrows. Space, backspace move right and left.");
    videocursor(0,22,0);
    scrstr("( seeks left margin, < absolutely, { up one line, [ down one line");
    videocursor(0,23,0);
    scrstr(") seeks right margin, > absolutely, } up one line, ] down one line");
    videocursor(0,24,0);
    scrstr("carriage return exits");
    jj=4;
    ii=1;
    while (0<1) {
    ii=lim(1,ii,40);
    jj=lim(1,jj,8);
    j=jj-1;
    i=ii-1;
    videocursor(0,j+9,i);
    c=kbdin();
    if (c == '\015') {videoscroll(19,0,24,70,0,3); break;};
    switch (c) {
    case '0':  case '1':		/* enter  state */
       arr1[40*j+i]=c-'0'; ii++; break;
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
    videocursor(0,j+9,0);
    for (k=0; k<40; k++) videoputc('0'+arr1[40*j+k],1);
    };
}

/* display a screen of evolution */
evolve() {
int i, j;
  videomode(COLGRAF);					/* erase the screen */
  videocursor(0,0,0);					/* top text line */
  scrstr("Rule: ");
  scrrul();
  woruno(0,28);						/* Wolfram rule number */
  asctobin();
  for (j=8; j<200; j++) videodot(j,AL-1,2);
  for (j=8; j<200; j++) {				/* evolve for 192 generations */
    for (i=0; i<AL; i++) videodot(j,i,arr1[i]);
    onegen(AL);
    if (kbdst()) {kbdin(); break;};
    };
}

/* copy ascrule over to binrul */
asctobin() {int i, i0, i1, i2;
    for (i=0; i<DS; i++) {arule[i] = rule[i]-'0';};
    for (i0=0; i0<KK; i0++) {
    for (i1=0; i1<KK; i1++) {
    for (i2=0; i2<KK; i2++) {
      binrule[i0][i1][i2]=ascrule[i0][i1][i2]-'0';
      };};};
}

/* evolution for one generation */
onegen(j) int j; {int i;
  if (j<2) return;
  arr2[0]=binrule[arr1[j-1]][arr1[0]][arr1[1]];
  for (i=1; i<j-1; i++) arr2[i]=binrule[arr1[i-1]][arr1[i]][arr1[i+1]];
  arr2[j-1]=binrule[arr1[j-2]][arr1[j-1]][arr1[0]];
  for (i=0;  i<j; i++) arr1[i]=arr2[i];
}

/* generate a random line of cells in arr1 */
ranlin() {int i, c;
for (i=0; i<AL; i++) {
  if (i%8 == 0) c=rand();
  arr1[i]=c%2; c/=2;};
}

/* tutorial and Help screen */
tuto() {
    videomode(T80X25);
    videocursor(0,2,0);
    scrstr("<Copyright (C) 1987, 1988 - H.V.McIntosh, G.Cisneros S.>");
    videocursor(0,4,0);
    scrstr("              *** LIFE in One Dimension ***");
    videocursor(0,6,0);
    scrstr("Two States - Black(0), Cyan(1).");
    videocursor(0,8,0);
    scrstr("First neighbors - one on each side, three altogether.");
    videocursor(0,10,0);
    scrstr("Complete transition rule - random, edited, or stored.");
    videocursor(0,12,0);
    scrstr("Initial Cellular Array - random, edited, or patterned.");
    videocursor(0,14,0);
    scrstr("Submenus in options t(probabilities) and d(de Bruijn)");
    videocursor(0,15,0);
    scrstr("will be displayed in response to typing ?");
    videocursor(0,17,0);
    scrstr("Use any key to terminate a display in progress.");
    videocursor(0,21,0);
    scrstr("Now, ... press any key to continue.");
}

/* rule menu */
rmenu() {
    videocursor(0,0,0);
    scrstr("      00001111");
    videocursor(0,1,0);
    scrstr("      00110011");
    videocursor(0,2,0);
    scrstr("      01010101");
    videocursor(0,3,0);
    scrstr("Rule: ");
    scrrul();
    videocursor(0,3,16);
    printf("  ");
    videocursor(0,3,16);
    if (istot()==1) printf("%2d",totonu(0));
    videocursor(0,5,0);
    scrstr("   r(rule), l(line), #nn(stored), g(graph), q(quit), t(prob),");
    videocursor(0,6,0);
    scrstr("   x(rand rule), y(rand line), u(unit line), d(deBruijn)");
    videocursor(0,7,0);
    scrstr("   wnn(Wolfram #), @nn(tot/rul), $nn(12 tot/rul), T(ed tot/rul).");
    videocursor(0,5,0);
    }

/* totalistic rule menu*/
tmenu() {
    videocursor(0,2,50);
    scrstr("      0..1");
    videocursor(0,3,50);
    scrstr("rule: ");
    tscrrul();
    videocursor(0,3,56);
    }

/* line menu */
lmenu() {int i, j;
    for (j=0; 40*j<AL; j++) {
	videocursor(0,9+j,0);
	for (i=0; i<40; i++) videoputc('0'+arr1[40*j+i],1);
	}
    videocursor(0,5,0); }

/* display Wolfram rule number */
wmenu(n) int n;
{char xx[4]; int i, nn;  
    nn=sprintf(xx,"%3d",n);
    videocursor(0,2,40);
    for (i=0; i<nn; i++) videoputc(xx[i],1);
    videocursor(0,2,40); }

/* display rule number */
xmenu(n) int n;
{char xx[4]; int i, nn;  
    nn=sprintf(xx,"%3d",n);
    videocursor(0,1,40);
    for (i=0; i<nn; i++) videoputc(xx[i],1);
    videocursor(0,1,40); }

/* clear screen space for rule numbers */
xblnk() {
    videocursor(0,1,40);
    scrstr("     ");
    videocursor(0,2,40);
    scrstr("     ");
    videocursor(0,5,0); }

/* copy saved rule #n into active rule */
xtoasc(n) int n; { int i0, i1, i2;
    xmenu(n+1);
    for (i0=0; i0<KK; i0++) {
    for (i1=0; i1<KK; i1++) {
    for (i2=0; i2<KK; i2++) {
    ascrule[i0][i1][i2] = xrule[n][i0][i1][i2];		/* random sample rule */
    };};}; }

/* change totalistic rule to general rule */
totoasc() {
int i0, i1, i2;
for (i0=0; i0<KK; i0++) {
for (i1=0; i1<KK; i1++) {
for (i2=0; i2<KK; i2++) {
ascrule[i0][i1][i2]=trule[i0+i1+i2];
};};};
}

/* change decimal totalistic rule to sum values */
nutoto(x) int x; {int i;
for (i=0; i<TS; i++) {trule[i]=x%2+'0'; x/=2;};}

/* change sum values to decimal totalistic rule */
int totonu(i) int i; {int r;
  if (i<TS) r=(trule[i]-'0')+2*totonu(i+1); else r=0;
  return r; }

/* test whether a rule is totalistic */
int istot() {int i0, i1, i2, l;
    l=1;
    trule[0]=ascrule[0][0][0];
    trule[1]=ascrule[1][0][0];
    trule[2]=ascrule[1][1][0];
    trule[3]=ascrule[1][1][1];
    for (i0=0; i0<KK; i0++) {
    for (i1=0; i1<KK; i1++) {
    for (i2=0; i2<KK; i2++) {
    if (ascrule[i0][i1][i2]!=trule[i0+i1+i2]) l=0;      
    };};};
    return l; }

/* limit j to interval (i,k) */
int lim(i,j,k) int i, j, k;
    {if (i>=j) return i; if (k<=j) return k; return j;}

/* display the rule number on the screen */
scrrul() {int i0, i1, i2;
  for (i0=0; i0<KK; i0++) {
  for (i1=0; i1<KK; i1++) {
  for (i2=0; i2<KK; i2++) {
  videoputc(ascrule[i0][i1][i2],1);      
  };};}; }

/* turn ascrule into its Wolfram number and place on screen at (i,j) */
woruno(i,j) int i, j; {
char xx[4];
int  k, l, i0, i1, i2;
  k=0;
  for (i0=KK-1; i0>=0; i0--) {
  for (i1=KK-1; i1>=0; i1--) {
  for (i2=KK-1; i2>=0; i2--) {
    k=2*k+(ascrule[i0][i1][i2]-'0');
    };};};
  k=sprintf(xx,"%3d",k);
  videocursor(0,i,j);
  for (l=0; l<k; l++) videoputc(xx[l],1);
}

/* display totalistic rule number by sum */
tscrrul() {int i;
  for (i=0; i<TS; i++) {videoputc(trule[i]); }; }

/* write a string in graphics mode */
scrstr(s) char *s;
  {for (; *s != '\0'; s++) videoputc(*s,1); }

/* keyboard status */
int kbdst() {return(bdos(11) & 0xFF);}

/* direct keyboard input, no echo */
kbdin() {int c;
  if ((c = bdos(7) & 0xFF) == '\0') c = (bdos(7) & 0xFF) | 0x80;
  return(c);
}

/* read number from keyboard */
int numin(n) int n; {char c;
  c=kbdin();
  if (c>='0'&&c<='9') return(numin(10*n+(c-'0'))); else return(n);
}

# include "prob21.c"
# include "rijn21.c"

/* end */
