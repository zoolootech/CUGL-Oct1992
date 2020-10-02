
/* lcau41.c				*/
/* (4,1) Linear Cellular Automaton	*/

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
/* 4 September 1987 - PROB41.C for option 't' [HVM]		*/
/* 21 October 1987 - program disks disappeared			*/
/* 24 December 1987 - program reconstructed from listings	*/
/* 20 February 1988 - RIJN41.C for option 'd' [HVM]		*/

# include <bdos.h>

# define COLGRAF     4  /* graph resolution			*/
# define T80X25      3  /* text resolution			*/
# define TRR	    16  /* row showing totalistic rule number	*/
# define TRC	    56  /* column for totalistic rule number	*/
# define XRR	    12  /* row displaying totalistic rule	*/
# define XRC	    56  /* column for totalistic rule		*/
# define WHCYMAG     1  /* color quad for normal screen		*/
# define YELREGR     2  /* color quad for alternative		*/
# define AL        320  /* array length (screen width)		*/
# define SL	    40	/* short array length			*/
# define TS	    10	/* distinct sums w/totalistic rule	*/
# define DS	    64  /* (number of distinct neighborhoods)	*/
# define KK	     4  /* number of states per cell		*/
# define NX         23	/* number of sample rules		*/

char xrule[NX][KK][KK][KK];

char ixrule[NX][DS]=

	"0001130003122323023030102111030121230003323213211011302112211302",	/* gliders among stills */
	"0001130130222132112122321212120102111201122311230301231030230321",	/* nice cross hatching */
	"0001130130222132110101010332020102111201122311230301321030230321",	/* very complex glider */
	"0010113231123333021220103301032113210101101221230033113223120103",	/* v. bars w/entanglement */
	"0012121132220120212330123102202020121112101220322110011310022330",	/* cycles on dgl bgrnd	*/
	"0020100001230000232301210123321233001123121232112333110012300230",	/* crosshatching */
	"0020103300113022002032300003303000201100101100100020100033002022",	/* bin ctr */
	"0032121132220120212330123102202020121112101220322110011310022330",	/* cycles on dgl bgrnd	*/
	"0100000220200110010000000120212001003032210200103233300302330000",	/* shuttle squeeze */
	"0100020010220131000220003020003001002020213230023230300000300010",	/* coo gldrs */
	"0120120120130133120120130133133220130133133233230133133233233230",	/* Perry's #45 */
	"0123011002000313130201120022032301210123132022101023332323210313",	/* crocodile skin */
	"0123012312100211011101110101313101023121133220110233123110021210",	/* mixture of types */
	"0123032103210123012302233111132103221112321133300311302020210323",	/* y/o on b/g */
	"0203001212230010000300201010000000033020222320200000002020200000",	/* slow glider - copies bar */
	"0203013021220020012201202120002200223122212211220013313032302333",	/* slow glider */
	"0203022213220121002010302212013201233002210332010303132021002113",	/* slow & fast gliders */
	"0203030213200201031323333233023200132331213223130233203011032120",	/* slo gl w/ many f gl */
	"0212011232210112001012301310133112212232322211210000123200333230",
	"0231231131121123231131121123123031121123123023001123123023003003",	/* Perry's blue background */
	"0300101011130131202233320002313020212123201221110232112022303132",	/* diagonal growth on mesh */
	"1010102020202020101011313131323232323121212132323231313130000000",	/* skewed triangle */
	"2121123121332313323212113122230113201130210123320020122112012332"	/* gliderettes & latice */

	;

char   tabl[20][SL];					/* workspace to construct table */
char   ascrule[KK][KK][KK];				/* ASCII transition values */
char   auxrule[KK][KK][KK];				/* auxiliary transition table */
char   rulstk[10][KK][KK][KK];				/* pushdown for rules */
char   prule1[8], prule2[8];				/* product rule specification */
char   trule[TS]="0130232113";
int    binrule[KK][KK][KK];
int    arr1[AL], arr2[AL];				/* line of cells */
int    ru, ru0, ru1, ru2;				/* rule cursor */
int    li, lj;						/* cell pointer */
int    rulptr;						/* rule pd ptr */
int    ix0, iy0;					/* origin for pen moves */
double wmul[KK], wmvl[KK];				/* left mass point */
double wmur[KK], wmvr[KK];				/* right mass point */
double bp[KK][KK][KK][KK];				/* bernstein polynomial */


main() {
int  i, j, i0, i1, i2, i3, i4, i5;
int  more = 'r';
char a, b, c;

ru=6; ru0=0; ru1=0; ru2=0;
li=SL/2; lj=0;
rulptr=0;

for (i=0; i<NX; i++) {					/* copy to 3-index array */
i0=0; i1=0; i2=0;
for (j=0; j<DS; j++) {
  xrule[i][i0][i1][i2]=ixrule[i][j];
  i2++;
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
    ranlin();						/* random initial array */
    auxblnk();						/* uncomitted transitions */

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
	case '.':					/* one cycle of evolution */
	    asctobin();
	    onegen(AL);
	    clmenu();
	    pprob();
	    break;
	case ',':					/* one cycle of evolution */
	    videomode(COLGRAF);
	    pgrid();
	    for (i=0; i<200; i++) {pprob(); onegen(AL);};
	    videodot(190,195,3);
	    kbdin();
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
	case 't':					/* triangular probability graph */
	    edtri();
	    rmenu();
	    lmenu();
	    break;
	case 'p':					/* product of two (2,1) rules */
	    nutowo1(numin(0));
	    nutowo2(numin(0));
	    for (i0=0; i0<2; i0++) {
	    for (i1=0; i1<2; i1++) {
	    for (i2=0; i2<2; i2++) {
	    for (i3=0; i3<2; i3++) {
	    for (i4=0; i4<2; i4++) {
	    for (i5=0; i5<2; i5++) {
	    ascrule[2*i0+i1][2*i2+i3][2*i4+i5]='0'+2*(prule1[4*i0+2*i2+i4]-'0')+(prule2[4*i1+2*i3+i5]-'0');
	    };};};};};};
	    xblnk();
	    rmenu();
	    break;
	case 'm':					/* (2,1) rules with memory */
	    nutowo1(numin(0));
	    for (i0=0; i0<2; i0++) {
	    for (i1=0; i1<2; i1++) {
	    for (i2=0; i2<2; i2++) {
	    for (i3=0; i3<2; i3++) {
	    for (i4=0; i4<2; i4++) {
	    for (i5=0; i5<2; i5++) {
	    ascrule[2*i0+i1][2*i2+i3][2*i4+i5]='0'+2*(prule1[4*i0+2*i2+i4]-'0')+i2;
	    };};};};};};
	    xblnk();
	    rmenu();
	    break;
        case 'r':					/* edit rule */	
	    xblnk();
	    edrule();
	    videocursor(0,4,0);
	    rmenu();
	    break;
        case 'l':					/* edit cell string */
	    xblnk();
	    edline(8,40);
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
	case 'X':					/* random rule */
	    xblnk();
	    i=rand();
	    for (i0=0; i0<KK; i0++) {
	    for (i1=0; i1<KK; i1++) {
	    for (i2=0; i2<KK; i2++) {
	      if (i == 0) i=rand();
	      ascrule[i0][i1][i2]='0'+i%KK;
	      i/=KK;
	    };};};
	    rmenu();
	    break;
	case 'x':					/* random rule */
	    xblnk();
	    for (i0=0; i0<KK; i0++) {
	    for (i1=0; i1<KK; i1++) {
	    for (i2=0; i2<KK; i2++) {
	      if (i == 0) i=rand();
	      if (auxrule[i0][i1][i2]==' ') {
		ascrule[i0][i1][i2]='0'+i%KK;
		i/=KK;
		};
	      };};};
	    rmenu();
	    break;
	case 'y':					/* random line */
	    xblnk();
	    ranlin();
            lmenu();
	    break;
	case 'z':
	    for (i=0; i<AL; i++) arr1[i]=0;
	    lmenu();
	    break;
	case 'Y':					/* symmetrize rule */
	    for (i0=0; i0<KK; i0++) {
	    for (i1=0; i1<KK; i1++) {
	    for (i2=0; i2<KK; i2++) {
	    ascrule[i2][i1][i0]=ascrule[i0][i1][i2];      
	    };};};
	    rmenu();
	    break;
	case 'B':					/* begin barrier */
	    a=kbdin();
	    b=kbdin();
	    ascrule[0][a-'0'][b-'0']=a;
	    ascrule[1][a-'0'][b-'0']=a;
	    ascrule[2][a-'0'][b-'0']=a;
	    ascrule[3][a-'0'][b-'0']=a;
	    rmenu();
	    break;
	case 'E':					/* end barrier */
	    a=kbdin();
	    b=kbdin();
	    ascrule[a-'0'][b-'0'][0]=b;
	    ascrule[a-'0'][b-'0'][1]=b;
	    ascrule[a-'0'][b-'0'][2]=b;
	    ascrule[a-'0'][b-'0'][3]=b;
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
	case 'U':					/* push rule */
	    if (rulptr<10) rulptr++;
	    for (i0=0; i0<KK; i0++) {
	    for (i1=0; i1<KK; i1++) {
	    for (i2=0; i2<KK; i2++) {
	    rulstk[rulptr][i0][i1][i2]=ascrule[i0][i1][i2];
	    };};};
	    xmenu(rulptr);
	    break;
	case 'V':					/* pop rule */
	    for (i0=0; i0<KK; i0++) {
	    for (i1=0; i1<KK; i1++) {
	    for (i2=0; i2<KK; i2++) {
	    ascrule[i0][i1][i2]=rulstk[rulptr][i0][i1][i2];
	    };};};
	    if (rulptr>0) rulptr--;
	    rmenu();
	    break;
	case 'G':					/* fetch rule */
	    for (i0=0; i0<KK; i0++) {
	    for (i1=0; i1<KK; i1++) {
	    for (i2=0; i2<KK; i2++) {
	    ascrule[i0][i1][i2]=rulstk[rulptr][i0][i1][i2];
	    };};};
	    rmenu();
	    break;
	case '1':
	    ascrule[0][0][0]='0';
	    ascrule[0][0][1]='0';
	    ascrule[0][0][2]='0';
	    ascrule[0][0][3]='1';
	    ascrule[0][1][0]='1';
	    ascrule[0][1][1]='2';
	    ascrule[0][1][3]='1';
	    ascrule[0][2][0]='3';
	    ascrule[0][3][1]='1';
	    ascrule[1][0][0]='1';
	    ascrule[1][1][1]='0';
	    rmenu();
	    break;
	case '2':
	    ascrule[0][0][0]='0';
	    ascrule[0][0][1]='2';
	    ascrule[0][0][2]='0';
	    ascrule[0][0][3]='3';
	    ascrule[0][1][0]='0';
	    ascrule[0][1][3]='2';
	    ascrule[0][2][0]='1';
	    ascrule[0][2][2]='2';
	    ascrule[0][3][0]='0';
	    ascrule[1][0][0]='0';
	    ascrule[1][0][3]='3';
	    ascrule[1][3][0]='0';
	    ascrule[2][0][0]='0';
	    ascrule[2][0][3]='3';
	    ascrule[2][2][0]='2';
	    ascrule[3][0][0]='0';
	    rmenu();
	    break;
	case '=':
	    for (i=1; i<8;  i++) {
	    for (j=0; j<SL; j++) arr1[SL*i+j]=arr1[j];};
	    lmenu();
	    break;
	case ':':
	    for (i=1; i<16;  i++) {
	    for (j=0; j<20; j++) arr1[20*i+j]=arr1[j];};
	    lmenu();
	    break;
	case ';':
	    for (i=1; i<32;  i++) {
	    for (j=0; j<10; j++) arr1[10*i+j]=arr1[j];};
	    lmenu();
	    break;
        default: break;
        };
    };
    if (more=='n') break;
    do {
    evolve();
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
edrule() {char c;
  while (0<1) {
    cscrrul();
    videocursor(0,3,ru);
    c=kbdin();
    if (c=='\015') break;				/* carriage return exits */
    switch (c) {
    case '0': case '1': case '2': case '3':		/* state */
      ascrule[ru0][ru1][ru2]=c;
      videocattr(0,c,3,1);
      rupl();
      break;
    case '\011':					/* tab = next quad */
      ru2=0; ru1=0; ru0++;
      if (ru0==KK) ru0=0;
      ru=KK*KK*ru0+6;
      break;
    case ' ': case '\315': rupl(); break;		/* space = advance */
    case '\010': case '\313': rumi(); break;		/* backspace */
    case '\013': case '\310': auxrule[ru0][ru1][ru2]='+'; break; /* up arrow */
    case '\012': case '\320': auxrule[ru0][ru1][ru2]=' '; break; /* down arrow */
    default: break;
    };
  };
}

/* advance the rule cursor */
rupl() {
  ru2++;
  if (ru2==KK) {ru2=0; ru1++;};
  if (ru1==KK) {ru1=0; ru0++;};
  if (ru0==KK) {ru0=0; };
  ru=KK*(KK*ru0+ru1)+ru2+6;
  }

/* retract the rule cursor */
rumi() {
  if (ru2!=0) ru2--; else {ru2=KK-1;
  if (ru1!=0) ru1--; else {ru1=KK-1;
  if (ru0!=0) ru0--; else {ru0=KK-1;
     };};};
  ru=KK*(KK*ru0+ru1)+ru2+6;
  }

/* edit totalistic rule */
edtrule() {char c; int  i, j;
  i=0;
  while (i<TS) {
    c=trule[i];
    videocursor(0,TRR,TRC+i);
    videocattr(0,c,3,1);
    c=kbdin();
    if (c=='\015') break;
    switch (c) {
      case '0': case '1': case '2': case '3':		/* state */
        trule[i]=c;
        videocattr(0,c,2,1);
        i++; break;
      case ' ': case '\315':				/* space = advance */
        i++; break;
      case '\010': case '\313':				/* backspace */
        if (i!=0) i--;
        break;
      case 'x':						/* random rule */
	i=rand();
	for (j=0; j<TS; j++) {
	  if (i==0) i=rand();
	  trule[j]='0'+i%KK;
	  i/=KK;
	  };
	tmenu();
	break;
      default: break;
      };
    };
}

/* edit the line of cells */
edline(nr,nc) int nr, nc; {
char c;
int  i, j, k, l, m;
int  aa, bb, cc;
int  xx, yy, zz;

  m=0;
  videocursor(0,19,0);
  scrstr("insert states with 0, 1, 2, 3; move cursor");
  videocursor(0,20,0);
  scrstr("with keyboard arrows, space or backspace.");
  videocursor(0,21,0);
  scrstr("carriage return exits");
  while (0<1) {
  videocursor(0,lj+9,li);
  c=kbdin();
  if (c == '\015') {videoscroll(19,0,24,70,0,3); break;};
  switch (c) {
  case '0': case '1': case '2': case '3': arr1[nc*lj+li]=c-'0'; break;	/* enter state */
  case '\012': case '\320':		if (lj<nr-1) lj++; break;	/* down    */
  case '\013': case '\310':		if (lj>0)    lj--; break;	/* up      */
  case '\014': case '\315': case ' ':	if (li<nc-1) li++; break;	/* forward */
  case '\010': case '\313':		if (li>0)    li--; break;	/* back    */
  case '<': li=0;    break; /* left margin */
  case '>': li=nc-1; break; /* right margin */
  case 'z': for (k=0; k<nc; k++) arr1[nc*lj+k]=0; break;			/* clear row */
  case 'Z': for (k=0; k<AL; k++) arr1[k]=0; lmenu(); break;
  case '^': for (k=0; k<nc; k++) arr1[nc*lj+k]=arr1[nc*(lj-1)+k]; break;
  case 'x': auxblnk(); break;				 /* uncomitted transitions */
  case 'q': ulmenu(3); break;				 /* uniform line menu */
  case '=':						 /* insert point transition */
    if (lj==0) break;
    yy=li+nc*(lj-1);
    xx=li==0?yy+nc-1:yy-1;
    zz=li==nc-1?yy-nc+1:yy+1;
    c=arr1[yy+nc]+'0';
    ascrule[arr1[xx]][arr1[yy]][arr1[zz]]=c;
    auxrule[arr1[xx]][arr1[yy]][arr1[zz]]=c;
    rmenu(); 
    videocursor(0,lj+9,li);
    videocattr(0,arr1[yy+nc],3,1);
    cscrrul();
    break;
  case '*':						/* insert transition */
    if (lj==0) break;
    for (i=0; i<nc; i++) {
      yy=i+nc*(lj-1);
      xx=i==0?yy+nc-1:yy-1;
      zz=i==nc-1?yy-nc+1:yy+1;
      c=arr1[yy+nc]+'0';
      ascrule[arr1[xx]][arr1[yy]][arr1[zz]]=c;
      auxrule[arr1[xx]][arr1[yy]][arr1[zz]]=c;
      videocattr(0,arr1[yy+nc],3,1);
      };
    rmenu(); 
    videocursor(0,lj+9,li);
    cscrrul();
    break;
  case '.':						/* point evolution */
    if (lj==0) break;
    yy=li+nc*(lj-1);
    xx=li==0?yy+nc-1:yy-1;
    zz=li==nc-1?yy-nc+1:yy+1;
    arr1[yy+nc]=ascrule[arr1[xx]][arr1[yy]][arr1[zz]]-'0';
    break;
  case '?':						/* evolution of whole line */
    if (lj==nr-1) {for (i=0; i<AL-nc; i++) arr1[i]=arr1[i+nc]; lj--; lmenu(); };
    lj++;
    for (i=0; i<nc; i++) {
      yy=i+nc*(lj-1);
      xx=i==0?yy+nc-1:yy-1;
      zz=i==nc-1?yy-nc+1:yy+1;
      arr1[yy+nc]=ascrule[arr1[xx]][arr1[yy]][arr1[zz]]-'0';
      };
    break;
  case '/':						/* conditional evolution */
    if (lj==nr-1) {for (k=0; k<AL-nc; k++) arr1[k]=arr1[k+nc]; lj--; lmenu(); };
    lj++;
    for (i=0; i<nc; i++) {
      j=0;
      yy=i+nc*(lj-1);
      xx=i==0?yy+nc-1:yy-1;
      zz=i==nc-1?yy-nc+1:yy+1;
      aa=arr1[xx]; if (aa>KK-1) j=KK;
      bb=arr1[yy]; if (bb>KK-1) j=KK;
      cc=arr1[zz]; if (cc>KK-1) j=KK;
      if (auxrule[aa][bb][cc]==' ') j=KK;
      if (j==0) j=ascrule[aa][bb][cc]-'0';
      arr1[yy+nc]=j;
      };
    break;
  case 'c':						/* test consistency */
    for (i=0; i<nc; i++) {
      videocursor(0,9,i);
      k=arr1[i];
      videocattr(0,k==0?'.':k+'0',7,1);
      };
    for (j=1; j<8;  j++) {
    for (i=0; i<nc; i++) {
      yy=i+nc*(j-1);
      xx=i==0?yy+nc+1:yy+1;
      aa=arr1[xx];
      bb=arr1[yy];
      cc=arr1[zz];
      k=arr1[yy+nc];
      videocursor(0,j+9,i);
      videocattr(0,k==0?'.':k+'0',k==ascrule[aa][bb][cc]-'0'?7:4,1);
      };};
    m=1;
    break;
  case 'C':						/* test consistency */
    for (i=0; i<nc; i++) {
      videocursor(0,9,i);
      k=arr1[i];
      videocattr(0,k==0?'.':k+'0',7,1);
      };
    for (j=1; j<8;  j++) {
    for (i=0; i<nc; i++) {
      yy=i+nc*(j-1);
      xx=i==0?yy+nc+1:yy+1;
      aa=arr1[xx];
      bb=arr1[yy];
      cc=arr1[zz];
      k=arr1[yy+nc];
      videocursor(0,j+9,i);
      videocattr(0,k==0?'.':k+'0',
        (k==ascrule[aa][bb][cc]-'0' && auxrule[aa][bb][cc]!=' ')?7:4,1);
      };};
    m=1;
    break;
  default: break;
      }; /* end switch */
  if (m==0) for (k=0; k<nc; k++) {
    videocursor(0,lj+9,k);
    l=arr1[nc*lj+k]+'0';
    videoputc(l=='0'?'.':l,1);
    };
  m=0;
  }; /* end while */
} /* end edline */

/* display a screen of evolution */
evolve() {int i, j;
  videomode(COLGRAF);					/* erase the screen */
  videocursor(0,0,0);					/* top text line */
  scrstr(":");
  hscrrul();
  asctobin();
  for (j=8; j<200; j++) videodot(j,AL-1,2);
  for (j=8; j<200; j++) {				/* evolve for 192 generations */
    for (i=0; i<AL; i++) videodot(j,i,arr1[i]);
    onegen(AL);
    if (kbdst()) {kbdin(); break;};
    };
}

/* copy ascrule over to binrul */
asctobin() {int i0, i1, i2;
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
  for (i=0; i<j; i++) arr1[i]=arr2[i];
}

/* map the automaton state into the unit square */
pprob() {int i, j; float x, y, z, k;
i=AL/2; k=(float)KK; z=(float)((197/(KK*KK))*KK*KK);
x=0.0; for (j=1; j<30; j++) x=(x+(float)arr1[i+j])/k;
y=0.0; for (j=1; j<30; j++) y=(y+(float)arr1[i-j])/k;
x=(x+(float)arr1[i])/k;
videodot((int)(z*(1.0-y)),(int)(z*x),1);
}

/* set up a grid for the unit square */
pgrid() {int i, j, k, l;
k=KK*KK+1;
l=195/(k-1);
for (i=0; i<k; i++) {
for (j=0; j<k; j++) {
videodot(l*i,l*j,2);
};};
}

/* tutorial and Help screen */
tuto() {
    videomode(T80X25);
    videocursor(0,2,0);
    scrstr("<Copyright (C) 1987, 1988 - H.V.McIntosh, G.Cisneros S.>");
    videocursor(0,4,0);
    scrstr("             *** LIFE in One Dimension ***");
    videocursor(0,6,0);
    scrstr("Four States - Black(0), Cyan(1), Magenta(2), White(3).");
    videocursor(0,8,0);
    scrstr("First neighbors - one on each side, three altogether.");
    videocursor(0,10,0);
    scrstr("Complete transition rule - random, edited, or stored.");
    videocursor(0,12,0);
    scrstr("Totalistic transition rule - random, edited, or stored.");
    videocursor(0,14,0);
    scrstr("Initial Cellular Array - random, edited, or patterned.");
    videocursor(0,16,0);
    scrstr("Submenus in options t(probabilities) and d(de Bruijn)");
    videocursor(0,17,0);
    scrstr("will be displayed in response to typing ?");
    videocursor(0,19,0);
    scrstr("Use any key to terminate a display in progress.");
    videocursor(0,22,0);
    scrstr("now, ... press any key to continue.");
}

/* rule menu */
rmenu() {
    videocursor(0,0,0);
    scrstr("      0000000000000000111111111111111122222222222222223333333333333333");
    videocursor(0,1,0);
    scrstr("      0000111122223333000011112222333300001111222233330000111122223333");
    videocursor(0,2,0);
    scrstr("      0123012301230123012301230123012301230123012301230123012301230123");
    videocursor(0,3,0);
    scrstr("Rule: ");
    scrrul();
    if (istot()==1) {videocursor(0,TRR,TRC); xmenu(totonu(0));};
    videocursor(0,5,0);
    scrstr("    r(rule), l(line), #nn(stored), g(graph), q(quit), t(prob),");
    videocursor(0,6,0);
    scrstr("           x(rand rule), y(rand line), u(unit line), d(deBruijn)");
    videocursor(0,7,0);
    scrstr("           @nn(tot/rule), $nn(12 tot/rules), T(ed tot/rule).");
    videocursor(0,5,0);
    }

/* totalistic rule menu*/
tmenu() {
    videocursor(0,TRR-2,TRC-6);
    scrstr("      totalistic rule");
    videocursor(0,TRR-1,TRC-6);
    scrstr("      0..1..2..3");
    videocursor(0,TRR,TRC-6);
    scrstr("rule: ");
    tscrrul();
    videocursor(0,TRR,TRC);
    }

/* plain line menu */
lmenu() {int i, j; char c;
  for (j=0; SL*j<AL; j++) {videocursor(0,9+j,0);
  for (i=0; i<SL; i++) {c='0'+arr1[SL*j+i]; videoputc(c=='0'?'.':c,1); }; };
  videocursor(0,5,0); }

/* uniform line menu */
ulmenu(l) int l; {int i, j, k;
  for (j=0; j*SL<AL; j++) {
  for (i=0; i<SL;    i++) {
    videocursor(0,9+j,i);
    k=arr1[j*SL+i];
    videocattr(0,k==0?'.':k+'0',l,1);
    };};
  }

/* color line menu */
clmenu() {int c, i, j;
  for (j=0; SL*j<AL; j++) {
    for (i=0; i<SL; i++) {
      videocursor(0,9+j,i);
      c=arr1[SL*j+i];
      videocattr(0,c==0?'.':'0'+c,color(c),1);
      };};
  videocursor(0,5,0); }

/* get compatible color */
int color(i) int i; {
  switch (i) {
    case 0:  return 2;
    case 1:  return 3;
    case 2:  return 5;
    case 3:  return 7;
    default: return 6;
};}

/* display rule number */
xmenu(n) int n; {
char xx[6];
int i, nn;  
    nn=sprintf(xx,"%5u",n);
    videocursor(0,XRR,XRC);
    for (i=0; i<nn; i++) videoputc(xx[i],1);
    videocursor(0,XRR,XRC); }

/* clear screen space for rule number */
xblnk() {videocursor(0,XRR,XRC); scrstr("     "); videocursor(0,5,0); }

/* copy saved rule #n into active rule */
xtoasc(n) int n; {int i0, i1, i2;
    xmenu(n+1);
    for (i0=0; i0<KK; i0++) {
    for (i1=0; i1<KK; i1++) {
    for (i2=0; i2<KK; i2++) {
    ascrule[i0][i1][i2]=xrule[n][i0][i1][i2];
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
for (i=0; i<TS; i++) {trule[i]=x%KK+'0'; x/=KK;};}

/* change sum values to decimal totalistic rule */
int totonu(i) int i; {int r;
  if (i<TS) r=(trule[i]-'0')+KK*totonu(i+1); else r=0;
  return r; }

/* test whether a rule is totalistic */
int istot() {int i0, i1, i2, l;
    l=1;
    trule[0]=ascrule[0][0][0];
    trule[1]=ascrule[1][0][0];
    trule[2]=ascrule[2][0][0];
    trule[3]=ascrule[3][0][0];
    trule[4]=ascrule[3][1][0];
    trule[5]=ascrule[3][2][0];
    trule[6]=ascrule[3][3][0];
    trule[7]=ascrule[3][3][1];
    trule[8]=ascrule[3][3][2];
    trule[9]=ascrule[3][3][3];
    for (i0=0; i0<KK; i0++) {
    for (i1=0; i1<KK; i1++) {
    for (i2=0; i2<KK; i2++) {
    if (ascrule[i0][i1][i2]!=trule[i0+i1+i2]) l=0;
    };};};
    return l; }

/* generate a random line of cells in arr1 */
ranlin() {int i, c;
for (i=0; i<AL; i++) {
  if (i%8 == 0) c=rand();
  arr1[i]=c%KK; c/=KK;};
}

/* clear auxrule to blanks */
auxblnk() {int i0, i1, i2;
for (i0=0; i0<KK; i0++) {
for (i1=0; i1<KK; i1++) {
for (i2=0; i2<KK; i2++) {
auxrule[i0][i1][i2]=' ';
};};};
}

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

/* display the rule number on the screen in glorious technicolor */
cscrrul() {int i, i0, i1, i2;
  i=6;
  for (i0=0; i0<KK; i0++) {
  for (i1=0; i1<KK; i1++) {
  for (i2=0; i2<KK; i2++) {
  videocursor(0,3,i++);
  videocattr(0,ascrule[i0][i1][i2],auxrule[i0][i1][i2]==' '?3:6,1);
  };};}; }

/* display the hexadecimal rule number on the screen */
hscrrul() {char i; int i0, i1, i2;
  for (i0=0; i0<KK; i0++) {
  for (i1=0; i1<KK; i1++) {
  for (i2=0; i2<KK; i2+=2) {
  i=ascrule[i0][i1][i2]+4*(ascrule[i0][i1][i2+1]-'0');
  if (i>'9') i=i+('A'-':');
  videoputc(i,1);      
  };};}; }

/* display totalistic rule number by sum */
tscrrul() {int i;
  for (i=0; i<TS; i++) {videoputc(trule[i],1); }; }

/* change decimal totalistic rule to sum values */
nutowo1(x) int x; {int i;
for (i=0; i<TS; i++) {prule1[i]=x%2+'0'; x/=2;};}

/* change Wolfram rule number decimal rule number */
int wotonu1(i) int i; {int r;
  if (i<TS) r=(prule1[i]-'0')+2*totonu(i+1); else r=0;
  return r; }

/* change decimal number to Wolfram rule number */
nutowo2(x) int x; {int i;
for (i=0; i<TS; i++) {prule2[i]=x%2+'0'; x/=2;};}

/* change sum values to decimal totalistic rule */
int wotonu2(i) int i; {int r;
  if (i<TS) r=(prule2[i]-'0')+2*totonu(i+1); else r=0;
  return r; }

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

# include "prob41.c"
# include "rijn41.c"

/* end */
