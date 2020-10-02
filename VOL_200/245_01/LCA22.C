
/* (2,2) Linear Cellular Automaton	*/

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
/* 14 May 1987 - modified for (3,1) and general rule [HVM]	*/
/* 19 May 1987 - modified for (2,2)				*/

# include <bdos.h>

# define COLGRAF     4  /* graph resolution			*/
# define T80X25      3  /* text resolution			*/
# define WHCYMAG     1  /* color quad for normal screen		*/
# define AL        320  /* array length (screen width)		*/
# define TS	     6 /* distinct sums w/totalistic rule	*/
# define DS	    32  /* number of distinct sums		*/
# define KK	     2  /* number of states per cell		*/
# define NX          8	/* number of sample rules		*/

char xrule[NX][KK][KK][KK][KK][KK];

char ixrule[NX][DS]=

	"00100110010000001011110001011101",	/*			*/
	"00110011101101111110111110000000",	/* interfaces of 2 vel	*/
	"01000110000011111011011000110100",	/* snowdot		*/
	"10110001010001010000101011110001",	/* two slopes		*/
	"10110100011001000000101101011000",	/* two speds of glidr	*/
	"11000100000010110111001011001100",	/* flotsam & jetsam	*/
	"11000100000010110111001011001000",	/*			*/
	"11010110011001111011101111100101"	/* dislocation		*/

	;

char  xx[4], rule[DS+1], ascrule[KK][KK][KK][KK][KK];
int   binrule[KK][KK][KK][KK][KK], arule[DS], arr1[AL], arr2[AL];
char  trule[TS]="000000";

main()
{
int  i, j, i0, i1, i2, i3, i4;
int  more = 'r';
char a, b, c;

for (i=0; i<NX; i++) {					/* copy to 5-index array */
i0=0; i1=0; i2=0; i3=0; i4=0;
for (j=0; j<DS; j++) {
  xrule[i][i0][i1][i2][i3][i4]=ixrule[i][j];
  i4++;
  if (i4==KK) {i4=0; i3++;};
  if (i3==KK) {i3=0; i2++;};
  if (i2==KK) {i2=0; i1++;};
  if (i1==KK) {i1=0; i0++;};
  if (i0==KK) {i0=0; };
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
	      evolve(rule);
	      };
	    videomode(T80X25);
	    rmenu();
	    lmenu();
	    break;
	case 't':					/* totalistic rule */
	    xblnk();
	    tmenu();
	    edtrule();
	    totoasc();
	    for (i0=0; i0<KK; i0++) {
	    for (i1=0; i1<KK; i1++) {
	    for (i2=0; i2<KK; i2++) {
	    for (i3=0; i3<KK; i3++) {
	    for (i4=0; i4<KK; i4++) {
	    ascrule[i0][i1][i2][i3][i4]=trule[i0+i1+i2+i3+i4];
	    };};};};};
	    videocursor(0,4,0);
	    rmenu();
	    xmenu(totonu(0));
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
	      evolve(rule);
	      };
	    videomode(T80X25);
	    rmenu();
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
	case 'x':					/* random rule */
	    xblnk();
	    for (i0=0; i0<KK; i0++) {
	    for (i1=0; i1<KK; i1++) {
	    for (i2=0; i2<KK; i2++) {
	    for (i3=0; i3<KK; i3++) {
	    for (i4=0; i4<KK; i4++) {
	      if ((KK*(KK*i0+i1)+i2)%4 == 0) i=rand();
	      ascrule[i0][i1][i2][i3][i4]='0'+i%KK;
	      i/=KK;
	    };};};};};
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
	    for (i3=0; i3<KK; i3++) {
	    for (i4=0; i4<KK; i4++) {
	    ascrule[i4][i3][i2][i1][i0]=ascrule[i0][i1][i2][i3][i4];      
	    };};};};};
	    break;
	case 'B':					/* begin barrier */
	    a=kbdin();
	    b=kbdin();
	    ascrule[0][0][0][a-'0'][b-'0']=a;
	    ascrule[0][0][1][a-'0'][b-'0']=a;
	    ascrule[0][0][2][a-'0'][b-'0']=a;
	    rmenu();
	    break;
	case 'E':					/* end barrier */
	    a=kbdin();
	    b=kbdin();
	    ascrule[0][0][a-'0'][b-'0'][0]=b;
	    ascrule[0][0][a-'0'][b-'0'][1]=b;
	    ascrule[0][0][a-'0'][b-'0'][2]=b;
	    rmenu();
	    break;
	case 'L':					/* left glider link */
	    a=kbdin();
	    b=kbdin();
	    c=kbdin();
	    ascrule[0][0][a-'0'][b-'0'][c-'0']=c;
	    rmenu();
	    break;
	case 'R':					/* left glider link */
	    a=kbdin();
	    b=kbdin();
	    c=kbdin();
	    ascrule[0][0][a-'0'][b-'0'][c-'0']=a;
	    rmenu();
	    break;
	case 'S':					/* still life link */
	    a=kbdin();
	    b=kbdin();
	    c=kbdin();
	    ascrule[0][0][a-'0'][b-'0'][c-'0']=b;
	    rmenu();
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
    evolve(rule);
    videocursor(0,0,0);
    scrstr("?");
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
int  i, i0, i1, i2, i3, i4;

i=6; i0=0; i1=0, i2=0; i3=0; i4=0;
    while (0<1) {
        videocursor(0,3,i);
        c = kbdin();
        if (c == '\015') break;				/* carriage return exits */
        switch (c) {
        case '0':  case '1':				/* state */
	    ascrule[i0][i1][i2][i3][i4] = c;
    	    i4++;
	    if (i4==KK) {i4=0; i3++;};
	    if (i3==KK) {i3=0; i2++;};
	    if (i2==KK) {i2=0; i1++;};
	    if (i1==KK) {i1=0; i0++;};
	    if (i0==KK) {i0=0; };
            videocattr(0,c,3,1);
            if (i<6+DS) i++;
            break;
        case ' ': case '\315':				/* space = advance */
    	    i4++;
	    if (i4==KK) {i4=0; i3++;};
	    if (i3==KK) {i3=0; i2++;};
	    if (i2==KK) {i2=0; i1++;};
	    if (i1==KK) {i1=0; i0++;};
	    if (i0==KK) {i0=0; };
            if (i<6+DS) i++;
            break;
        case '\010': case '\313':			/* backspace */
	    if (i4!=0) i4--; else {i4=KK-1;
	    if (i3!=0) i3--; else {i3=KK-1;
	    if (i2!=0) i2--; else {i2=KK-1;
	    if (i1!=0) i1--; else {i1=KK-1;
	    if (i0!=0) i0--; else {i0=KK-1;
	    };};};};};
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
	    i++; break;
          case ' ': case '\315':			/* space = advance */
	    i++; break;
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
    scrstr("insert states using 0, 1");
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
    case '0':  case '1':	    /* enter  state */
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
evolve(rule) char *rule; {
int i0, i1, i2, i3, i4, i, j;
  videomode(COLGRAF);					/* erase the screen */
  videocursor(0,0,0);					/* top text line */
  scrstr(":");
  scrrul();
  for (i=0; i<DS; i++) {arule[i] = rule[i]-'0';};
  for (i0=0; i0<KK; i0++) {
  for (i1=0; i1<KK; i1++) {
  for (i2=0; i2<KK; i2++) {
  for (i3=0; i3<KK; i3++) {
  for (i4=0; i4<KK; i4++) {
    binrule[i0][i1][i2][i3][i4]=ascrule[i0][i1][i2][i3][i4]-'0';
    };};};};};
  for (j=8; j<200; j++) videodot(j,AL-1,2);
  for (j=8; j<200; j++) {				/* evolve for 192 generations */
    arr2[0]=binrule[arr1[AL-2]][arr1[AL-1]][arr1[0]][arr1[1]][arr1[2]];
    arr2[1]=binrule[arr1[AL-1]][arr1[0]][arr1[1]][arr1[2]][arr1[3]];
    for (i=2; i<AL-2; i++) {
      arr2[i]=binrule[arr1[i-2]][arr1[i-1]][arr1[i]][arr1[i+1]][arr1[i+2]]; };
    arr2[AL-2]=binrule[arr1[AL-4]][arr1[AL-3]][arr1[AL-2]][arr1[AL-1]][arr1[0]];
    arr2[AL-1]=binrule[arr1[AL-3]][arr1[AL-2]][arr1[AL-1]][arr1[0]][arr1[1]];
  for (i=0; i<AL; i++) {videodot(j,i,arr1[i]); arr1[i]=arr2[i];};
  if (kbdst()) {kbdin(); break;};
      }
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
    scrstr("<Copyright (C) 1987 - H.V. McIntosh, G. Cisneros S.>");
    videocursor(0,4,0);
    scrstr("        ***** LIFE in One Dimension *****");
    videocursor(0,6,0);
    scrstr("Two States - Black(0), Cyan(1).");
    videocursor(0,8,0);
    scrstr("Second neighbors - two on each side, five altogether.");
    videocursor(0,10,0);
    scrstr("Complete transition rule - random, edited, or stored.");
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

/* rule menu */
rmenu() {
    videocursor(0,0,0);
    scrstr("      ....1111....1111....1111....1111");
    videocursor(0,1,0);
    scrstr("      ..11..11..11..11..11..11..11..11");
    videocursor(0,2,0);
    scrstr("      01010101010101010101010101010101");
    videocursor(0,3,0);
    scrstr("Rule: ");
    scrrul();
    if (istot()==1) {scrstr("     "); printf("%5d",totonu(0));};
    videocursor(0,5,0);
    scrstr("    r(rule), l(line), #nn(stored rule), g(graph), q(quit),");
    videocursor(0,6,0);
    scrstr("             x(random rule), y(random line), u(unit line),");
    videocursor(0,7,0);
    scrstr("             @nn(tot/rule), $nn(dzn tot/rules), t(ed tot/rule).");
    videocursor(0,5,0);
    }

/* totalistic rule menu*/
tmenu() {
    videocursor(0,2,50);
    scrstr("      0....1");
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

/* display rule number */
xmenu(n) int n;
{int i, nn;  
    nn=sprintf(xx,"%3d",n);
    videocursor(0,1,40);
    for (i=0; i<nn; i++) videoputc(xx[i],1);
    videocursor(0,1,40); }

/* clear screen space for rule number */
xblnk() {
    videocursor(0,1,40);
    scrstr("    ");
    videocursor(0,5,0); }

/* copy saved rule #n into active rule */
xtoasc(n) int n; { int i0, i1, i2, i3, i4;
    xmenu(n+1);
    for (i0=0; i0<KK; i0++) {
    for (i1=0; i1<KK; i1++) {
    for (i2=0; i2<KK; i2++) {
    for (i3=0; i3<KK; i3++) {
    for (i4=0; i4<KK; i4++) {
    ascrule[i0][i1][i2][i3][i4] = xrule[n][i0][i1][i2][i3][i4];	/* random sample rule */
    };};};};}; }

/* change totalistic rule to general rule */
totoasc() {
int i0, i1, i2, i3, i4;
for (i0=0; i0<3; i0++) {
for (i1=0; i1<3; i1++) {
for (i2=0; i2<3; i2++) {
for (i3=0; i3<3; i3++) {
for (i4=0; i4<3; i4++) {
ascrule[i0][i1][i2][i3][i4]=trule[i0+i1+i2+i3+i4];
};};};};};
}

/* change decimal totalistic rule to sum values */
nutoto(x) int x; {int i;
for (i=0; i<TS; i++) {trule[i]=x%2+'0'; x/=2;};}

/* change sum values to decimal totalistic rule */
int totonu(i) int i; {int r;
  if (i<TS) r=(trule[i]-'0')+2*totonu(i+1); else r=0;
  return r; }

/* test whether a rule is totalistic */
int istot() {int i0, i1, i2, i3, i4, l;
    l=1;
    trule[0]=ascrule[0][0][0][0][0];
    trule[1]=ascrule[1][0][0][0][0];
    trule[2]=ascrule[1][1][0][0][0];
    trule[3]=ascrule[1][1][1][0][0];
    trule[4]=ascrule[1][1][1][1][0];
    trule[5]=ascrule[1][1][1][1][1];
    for (i0=0; i0<KK; i0++) {
    for (i1=0; i1<KK; i1++) {
    for (i2=0; i2<KK; i2++) {
    for (i3=0; i3<KK; i3++) {
    for (i4=0; i4<KK; i4++) {
    if (ascrule[i0][i1][i2][i3][i4]!=trule[i0+i1+i2+i3+i4]) l=0;      
    };};};};};
    return l; }

/* limit j to interval (i,k) */
int lim(i,j,k) int i, j, k;
    {if (i>=j) return i; if (k<=j) return k; return j;}

/* display the rule number on the screen */
scrrul() {int i0, i1, i2, i3, i4;
  for (i0=0; i0<KK; i0++) {
  for (i1=0; i1<KK; i1++) {
  for (i2=0; i2<KK; i2++) {
  for (i3=0; i3<KK; i3++) {
  for (i4=0; i4<KK; i4++) {
  videoputc(ascrule[i0][i1][i2][i3][i4],1);      
  };};};};}; }

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

/* end */
