
/* rijn21.c						*/
/* program for lcau21 option 'd'			*/
/* display de Bruijn diagram - 1st, 2nd, & 3rd gen	*/
/* Harold V. McIntosh, 20 February 1988			*/
/* 21 February 1988 - add j0=i0; to Pass 4's [HVM]	*/

/*	Copyright (C) 1987	*/
/*	Copyright (C) 1988	*/
/*	Harold V. McIntosh	*/
/*	Gerardo Cisneros S.	*/

# define JX   0.8  /* x-center, de Bruijn diagram	*/
# define JY   0.5  /* y-center, de Bruijn diagram	*/
# define RI 0.455  /* inner radius, de Bruijn diagram	*/
# define RO 0.475  /* outer radius, de Bruijn diagram	*/
# define NW    24  /* pause after so many lines		*/

int  dd;		/* # points in de Bruijn ring	*/
int  mc, nc, nl;	/* column & line control	*/

/* edit the de Bruijn diagram */
edijn() {char c;

  clijn();
  dd=KK*KK;
  ijn(dd,KK,2);
  while (0<1) {
  videocursor(0,0,36);
  videoputc('?',2);
  c=kbdin();
  if (c=='\015') break;
  videocursor(0,0,38);
  videoputc(c,2);
  videocursor(0,0,36);
  videoputc(' ',2);
  videoscroll(1,0,24,14,0,0);
  videocursor(0,0,0);
  kwait(3);
  switch (c) {
    case 'A': dd=KK*KK; ijn(dd,KK,2); sijn(dd,0,1); break;
    case 'B': dd=KK*KK; ijn(dd,KK,2); sijn(dd,1,1); break;
    case 'C': dd=KK*KK; ijn(dd,KK,2); zijn(dd,0,1); break;
    case 'D': dd=KK*KK; ijn(dd,KK,2); zijn(dd,1,1); break;
    case 'E': dd=KK*KK; ijn(dd,KK,2); zijn(dd,2,1); break;
    case 'F': kwait(0); printf("Precursors 0*:");  kwait(0); xpass1(0); break;
    case 'G': kwait(0); printf("Precursors 1*:");  kwait(0); xpass1(1); break;
    case 'a': kwait(0); printf("(1,1) gliders:");  kwait(0); apass1(0); break;
    case 'b': kwait(0); printf("(1,0) static:");   kwait(0); apass1(1); break;
    case 'c': kwait(0); printf("(1,-1) gliders:"); kwait(0); apass1(2); break;
    case 'd': kwait(0); printf("(2,-2) gliders:"); kwait(0); bpass1(0); break;
    case 'e': kwait(0); printf("(2,-1) gliders:"); kwait(0); bpass1(1); break;
    case 'f': kwait(0); printf("(2,0) cycles:");   kwait(0); bpass1(2); break;
    case 'g': kwait(0); printf("(2,1) gliders:");  kwait(0); bpass1(3); break;
    case 'h': kwait(0); printf("(2,2) gliders:");  kwait(0); bpass1(4); break;
    case 'i': kwait(0); printf("(3,-3) gliders:"); kwait(0); cpass1(0); break;
    case 'j': kwait(0); printf("(3,-2) gliders:"); kwait(0); cpass1(1); break;
    case 'k': kwait(0); printf("(3,-1) gliders:"); kwait(0); cpass1(2); break;
    case 'l': kwait(0); printf("(3,0) cycles:");   kwait(0); cpass1(3); break;
    case 'm': kwait(0); printf("(3,1) gliders:");  kwait(0); cpass1(4); break;
    case 'n': kwait(0); printf("(3,2) gliders:");  kwait(0); cpass1(5); break;
    case 'o': kwait(0); printf("(3,3) gliders:");  kwait(0); cpass1(6); break;
    case 'p': kwait(0); printf("(4,-4) gliders:"); kwait(0); dpass1(0); break;
    case 'q': kwait(0); printf("(4,-3) gliders:"); kwait(0); dpass1(1); break;
    case 'r': kwait(0); printf("(4,-2) gliders:"); kwait(0); dpass1(2); break;
    case 's': kwait(0); printf("(4,-1) gliders:"); kwait(0); dpass1(3); break;
    case 't': kwait(0); printf("(4,0) cycles:");   kwait(0); dpass1(4); break;
    case 'u': kwait(0); printf("(4,1) gliders:");  kwait(0); dpass1(5); break;
    case 'v': kwait(0); printf("(4,2) gliders:");  kwait(0); dpass1(6); break;
    case 'w': kwait(0); printf("(4,3) gliders:");  kwait(0); dpass1(7); break;
    case 'x': kwait(0); printf("(4,4) gliders:");  kwait(0); dpass1(8); break;
    case '1': dd=KK*KK; clijn(); ijn(dd,KK,2); break;
    case '2': dd=KK*KK*KK; clijn(); ijn(dd,KK,2); break;
    case '3': dd=KK*KK*KK*KK; clijn(); ijn(dd,KK,2); break;
    case '4': dd=KK*KK*KK*KK*KK; clijn(); ijn(dd,KK,2); break;
    case '5': dd=KK*KK*KK*KK*KK*KK; clijn(); ijn(dd,KK,2); break;
    case '6': dd=KK*KK*KK*KK*KK*KK*KK; clijn(); ijn(dd,KK,2); break;
    case '+': videopalette(WHCYMAG); break;
    case '-': videopalette(YELREGR); break;
    case '/': case '?': clijn(); break;
    default: break;
    }; /* end switch */
  };   /* end while  */
  videopalette(WHCYMAG);
  videomode(T80X25);
}

/* clear edijn screen and set up menu */
clijn() {
  videomode(COLGRAF);
  videopalette(YELREGR);
  videocursor(0,0,0);
  scrrul();
  videocursor(0,3,0);
  printf("de Bruijn ring\n\n");
  printf("abc   - (1,x)\n");
  printf("ABC   - (all)\n");
  printf("defgh - (2,x)\n");
  printf("ijklmno (3,x)\n");
  printf("pqrstuvwx (4)\n");
  printf("123456 -diagram\n");
  printf("DE   -  0*,1*\n");
  printf("FG   -  full n*\n");
  printf("+-   - pallette\n");
  printf("?/   - clr scrn\n");
  printf("<cr> - exit\n\n");
}

/* generate a de Bruijn diagram with m nodes, n links per node, color l */
ijn(m,n,l) int m, n, l; {
int    i, j;
double ii, jj, nn, x, y, t, h;
double sin(), cos();
nn=(double)(n);
t=6.28318/((double)(m));
h=0.5*t;
for (i=0; i<m; i++) {
  ii=(double)(i);
  for (j=0; j<n; j++) {
    jj=(double)(j);
    x=JX-RI*sin(ii*t+h);
    y=JY-RI*cos(ii*t+h);
    videoline(x,y,0);
    x=JX-RO*sin((nn*ii+jj)*t+h);
    y=JY-RO*cos((nn*ii+jj)*t+h);
    videoline(x,y,l);
    };
  };
}

/* insert a link into a de Bruijn diagram */
/* m - number of vertices */
/* u - initial vertex     */
/* v - terminal vertex    */
/* l - color of link	  */
lijnk(m,u,v,l) int m, u, v, l; {
double uu, vv, x, y, t, h;
double sin(), cos();
t=6.28318/((double)(m));
uu=t*((double)(u));
vv=t*((double)(v));
h=0.5*t;
x=JX-RI*sin(uu+h);
y=JY-RI*cos(uu+h);
videoline(x,y,0);
x=JX-RO*sin(vv+h);
y=JY-RO*cos(vv+h);
videoline(x,y,l);
}

/* generate a de Bruijn diagram showing antecedents of state k in color l */
sijn(m,k,l) int m, k, l; {
int    i0, i1, i2;
double th, x, y, t, h;
double sin(), cos();
t=6.28318/((double)(m));
h=0.5*t;
for (i0=0; i0<KK; i0++) {
for (i1=0; i1<KK; i1++) {
for (i2=0; i2<KK; i2++) {
  if (k==ascrule[i0][i1][i2]-'0') {
    th=((double)(i0*KK+i1))*t+h;
    x=JX-RI*sin(th);
    y=JY-RI*cos(th);
    videoline(x,y,0);
    th=((double)(i1*KK+i2))*t+h;
    x=JX-RO*sin(th);
    y=JY-RO*cos(th);
    videoline(x,y,l);
    };	   /* end if    */
  };};};   /* end for's */
}	   /* end sijn  */

/* generate a de Bruijn diagram showing the (1,k) links in color l */
zijn(m,k,l) int m, k, l; {
int    i, i0, i1, i2;
double th, x, y, t, h;
double sin(), cos();
t=6.28318/((double)(m));
h=0.5*t;
for (i0=0; i0<KK; i0++) {
for (i1=0; i1<KK; i1++) {
for (i2=0; i2<KK; i2++) {
  i=ascrule[i0][i1][i2]-'0';
  if ((k==0 && i==i0) || (k==1 && i==i1) || (k==2 && i==i2)) {
    th=((double)(i0*KK+i1))*t+h;
    x=JX-RI*sin(th);
    y=JY-RI*cos(th);
    videoline(x,y,0);
    th=((double)(i1*KK+i2))*t+h;
    x=JX-RO*sin(th);
    y=JY-RO*cos(th);
    videoline(x,y,l);
    };	   /* end if    */
  };};};   /* end for's */
}	   /* end zijn  */

/* plot graph on video screen 		   */
/* move pen from present position to (x,y) */
/* raised if l=0; color l if l>0	   */
videoline(x,y,l) double x, y; int l; {
int k, ax, ay, dx, dy, di, dj, x1, y1;
/* if (x<0.0) return; if (x>1.0) return */
/* if (y<0.0) return; if (y>1.0) return */
x1=(int)(199.0*(1.0-y));
y1=(int)(199.0*x);
dx=x1-ix0;
dy=y1-iy0;
ax=dx>=0?dx:-dx;
ay=dy>=0?dy:-dy;
di=dx>=0?1:-1; if (dx==0) di=0;
dj=dy>=0?1:-1; if (dy==0) dj=0;

if (l>0) {if (ax!=0 || ay!=0) {
  if (ax>=ay) {for (k=0; k<=ax; k++) videodot(ix0+di*k,60+iy0+(k*dy)/ax,l);}
	 else {for (k=0; k<=ay; k++) videodot(ix0+(k*dx)/ay,60+iy0+dj*k,l);};
  }; };
ix0=x1;
iy0=y1;
}

/* approximation to sine */
double sin(x) double x; {
if (x<0.0) return(-sin(-x));
while (x>=6.28318) x-=6.28318;
if (x>=3.142) return(-sin(x-3.14159));
if (x>=1.571) return(sin(3.14159-x));
return(x*(1.0-0.166*(x*x-0.05*x*x)));
}

/* approximation to cosine */
double cos(x) double x; {double sin(); return(sin(x+1.57079));}

/* ONE21.C */

/* Pass 1a analyzes all the configurations which fulfil (1,-1+l) */
apass1(l) {
char arry[KK][KK][KK];
int  i,j,k,m;
mc=2;
asctobin();
printf(" Pass1a\015");
for (i=0; i<KK; i++) {
for (j=0; j<KK; j++) {
for (k=0; k<KK; k++) {
  switch (l) {
    case 0: m=i; break;
    case 1: m=j; break;
    case 2: m=k; break;
    default: break;};
  arry[i][j][k]=binrule[i][j][k]==m?'Y':'N';
  };};};
apass2i(arry);
apass2o(arry);
apass4(arry);
}

/* Pass 1x analyzes all the configurations mapping into a constant */
xpass1(c) int c; {
char arry[KK][KK][KK];
int  i,j,k;
mc=2;
asctobin();
printf(" Pass1a\015");
for (i=0; i<KK; i++) {
for (j=0; j<KK; j++) {
for (k=0; k<KK; k++) {
  arry[i][j][k]=binrule[i][j][k]==c?'Y':'N';
  };};};
apass2i(arry);
apass2o(arry);
apass4(arry);
}

/* Pass 2i flags all links with an incoming arrow */
/* Pass 2o flags all links with an outgoing arrow */
/* Then pass 3 discards all unflagged links */
/* Passes 2 and 3 alternate until no change is observed */

apass2i(arry) char arry[KK][KK][KK]; {int i,j,k,m;
do {
printf(" Pass2i\015");
for (i=0; i<KK; i++) {
for (j=0; j<KK; j++) {
for (k=0; k<KK; k++) {
if ((arry[i][j][k]&0x5F)=='Y') {for (m=0; m<KK; m++) arry[j][k][m]|=0x20;};
};};}; } while (apass3(arry)!=0); }

apass2o(arry) char arry[KK][KK][KK]; {int i,j,k,m;
do {
printf(" Pass2o\015");
for (i=0; i<KK; i++) {
for (j=0; j<KK; j++) {
for (k=0; k<KK; k++) {
if ((arry[i][j][k]&0x5F)=='Y') {for (m=0; m<KK; m++) arry[m][i][j]|=0x20;};
};};}; } while (apass3(arry)!=0); }

/* Pass 3 - erase flags, mark survivors, count changes */

int apass3(arry) char arry[KK][KK][KK]; {int i,j,k,l;
l=0;
printf(" Pass3 \015");
for (i=0; i<KK; i++) {
for (j=0; j<KK; j++) {
for (k=0; k<KK; k++) {
  switch (arry[i][j][k]) {
    case 'y': arry[i][j][k]='Y'; break;
    case 'Y': arry[i][j][k]='N'; l=1; break;
    case 'n': case 'N': arry[i][j][k]='N'; break;
    default: break; };
};};};
return l;
}

/* Pass4 - print loops which remain */
apass4(arry) char arry[KK][KK][KK]; {
int i0, i1, i2;
int j0, j1, j2;
int k, l, m;
printf(" pass4 \015");
for (i0=0; i0<KK; i0++) {
for (i1=0; i1<KK; i1++) {
for (i2=0; i2<KK; i2++) {
j0=i0; j1=i1; j2=i2; l=0; m=0;
do {
        if (arry[0][j1][j2]=='Y')
    {arry[0][j1][j2]='y';
    k=j2; j2=j1; j1=0; m=1;}
  else {if (arry[1][j1][j2]=='Y')
    {arry[1][j1][j2]='y';
    k=j2; j2=j1; j1=1; m=1;}
  else {l=1; if (m==1) {j0=j1; j1=j2; j2=k;}; };};
  } while (l==0);
l=0; 
m=0;
do {
        if (arry[j0][j1][0]=='y')
   {aprf(j0,j1,0);
   arry[j0][j1][0]='N';
   j0=j1; j1=0; m=1;}
  else {if (arry[j0][j1][1]=='y')
   {aprf(j0,j1,1);
   arry[j0][j1][1]='N';
   j0=j1; j1=1; m=1;}
  else {l=1;};};
  } while (l==0);
l=0;
do {
        if (arry[j0][j1][0]=='Y')
   {aprf(j0,j1,0);
   arry[j0][j1][0]='N';
   j0=j1; j1=0; m=1;}
  else {if (arry[j0][j1][1]=='Y')
   {aprf(j0,j1,1);
   arry[j0][j1][1]='N';
   j0=j1; j1=1; m=1;}
  else {l=1; if (m==1) kwait(0);};};
  } while (l==0);
};};};
}

/* print one link of the de Bruijn diagram */
aprf(i,j,k) int i, j, k; {
kwait(1);
printf("%1d",i);
printf("%1d",j);
printf("-");
printf("%1d",k);
printf("  ");
lijnk(dd,i*KK+j,j*KK+k,3);
}

/* TWO21.C */

/* Pass 1analyzes all sequences conforming to (2,-2+l)  */
bpass1(l) int l; {
char arry[KK][KK][KK][KK][KK];
int  i0, i1, i2, i3, i4;
int  i, j, k, m;
mc=2;
asctobin();
printf(" Pass1a\015");
for (i0=0; i0<KK; i0++) {
for (i1=0; i1<KK; i1++) {
for (i2=0; i2<KK; i2++) {
for (i3=0; i3<KK; i3++) {
for (i4=0; i4<KK; i4++) {
  i=binrule[i0][i1][i2];
  j=binrule[i1][i2][i3];
  k=binrule[i2][i3][i4];
  switch (l) {
    case 0: m=i0; break;
    case 1: m=i1; break;
    case 2: m=i2; break;
    case 3: m=i3; break;
    case 4: m=i4; break;
    default: break;};
  arry[i0][i1][i2][i3][i4]=binrule[i][j][k]==m?'Y':'N';
  };};};};};
bpass2i(arry);
bpass2o(arry);
bpass4(arry);
}

/* Passs 2i flags links which have an incoming arrow */
bpass2i(arry) char arry[KK][KK][KK][KK][KK]; {
int i0, i1, i2, i3, i4, m;
printf(" Pass2i\015");
do {
for (i0=0; i0<KK; i0++) {
for (i1=0; i1<KK; i1++) {
for (i2=0; i2<KK; i2++) {
for (i3=0; i3<KK; i3++) {
for (i4=0; i4<KK; i4++) {
if ((arry[i0][i1][i2][i3][i4]&0x5F)=='Y')
 {for (m=0; m<KK; m++) arry[i1][i2][i3][i4][m]|=0x20;};
};};};};};
} while (bpass3(arry)!=0); }

/* Passs 2o flags links which have an outgoing arrow */
bpass2o(arry) char arry[KK][KK][KK][KK][KK]; {
int i0, i1, i2, i3, i4, m;
do {
printf(" Pass2o\015");
for (i0=0; i0<KK; i0++) {
for (i1=0; i1<KK; i1++) {
for (i2=0; i2<KK; i2++) {
for (i3=0; i3<KK; i3++) {
for (i4=0; i4<KK; i4++) {
if ((arry[i0][i1][i2][i3][i4]&0x5F)=='Y')
 {for (m=0; m<KK; m++) arry[m][i0][i1][i2][i3]|=0x20;};
};};};};};
} while (bpass3(arry)!=0); }

/*  Pass 3 erases flags, marks survivors, counts channges */
int bpass3(arry) char arry[KK][KK][KK][KK][KK]; {
int i0, i1, i2, i3, i4, l;
printf(" Pass3 \015");
l=0;
for (i0=0; i0<KK; i0++) {
for (i1=0; i1<KK; i1++) {
for (i2=0; i2<KK; i2++) {
for (i3=0; i3<KK; i3++) {
for (i4=0; i4<KK; i4++) {
switch (arry[i0][i1][i2][i3][i4]) {
    case 'y': arry[i0][i1][i2][i3][i4]='Y'; break;
    case 'Y': arry[i0][i1][i2][i3][i4]='N'; l=1; break;
    case 'n': case 'N': arry[i0][i1][i2][i3][i4]='N'; break;
    default: break; };
};};};};};
return l;
}

/* Pass 4 prints loops which remain */
bpass4(arry) char arry[KK][KK][KK][KK][KK]; {
int i0, i1, i2, i3, i4;
int j0, j1, j2, j3, j4, k, l, m;
printf(" pass4 \015");
for (i0=0; i0<KK; i0++) {
for (i1=0; i1<KK; i1++) {
for (i2=0; i2<KK; i2++) {
for (i3=0; i3<KK; i3++) {
for (i4=0; i4<KK; i4++) {
j0=i0; j1=i1; j2=i2; j3=i3; j4=i4; l=0; m=0;
do {
        if (arry[0][j1][j2][j3][j4]=='Y')
    {arry[0][j1][j2][j3][j4]='y';
    k=j4; j4=j3; j3=j2; j2=j1; j1=0; m=1;}
  else {if (arry[1][j1][j2][j3][j4]=='Y')
    {arry[1][j1][j2][j3][j4]='y';
    k=j4; j4=j3; j3=j2; j2=j1; j1=1; m=1;}
  else {l=1; if (m==1) {j0=j1; j1=j2; j2=j3; j3=j4; j4=k;}; };};
  } while (l==0);
l=0; 
m=0;
do {
        if (arry[j0][j1][j2][j3][0]=='y')
   {bprf(j0,j1,j2,j3,0);
   arry[j0][j1][j2][j3][0]='N';
   j0=j1; j1=j2; j2=j3; j3=0; m=1;}
  else {if (arry[j0][j1][j2][j3][1]=='y')
   {bprf(j0,j1,j2,j3,1);
   arry[j0][j1][j2][j3][1]='N';
   j0=j1; j1=j2; j2=j3; j3=1; m=1;}
  else {l=1;};};
  } while (l==0);
l=0;
do {
        if (arry[j0][j1][j2][j3][0]=='Y')
   {bprf(j0,j1,j2,j3,0);
   arry[j0][j1][j2][j3][0]='N';
   j0=j1; j1=j2; j2=j3; j3=0; m=1;}
  else {if (arry[j0][j1][j2][j3][1]=='Y')
   {bprf(j0,j1,j2,j3,1);
   arry[j0][j1][j2][j3][1]='N';
   j0=j1; j1=j2; j2=j3; j3=1; m=1;}
  else {l=1; if (m==1) {kwait(0);} ;};};
  } while (l==0);
};};};};};
}

/* print one of the links in a chain */
bprf(i0,i1,i2,i3,i4) int i0, i1,i2, i3, i4; {
kwait(1);
printf("%1d",i0);
printf("%1d",i1);
printf("%1d",i2);
printf("%1d",i3);
printf("-");
printf("%1d",i4);
printf(" ");
lijnk(dd,((i0*KK+i1)*KK+i2)*KK+i3,((i1*KK+i2)*KK+i3)*KK+i4,3);
}

/* pause at the end of a full screen	    */
/* kwait(0) - short or null line	    */
/* kwait(1) - split long lines as necessary */
/* kwait(2) - wait to continue		    */
/* kwait(3) - reset 			    */
kwait(i) int i; {
switch (i) {
  case 0: printf("\n"); nc=0; nl++; break;
  case 1: if (nc==mc) {printf("&\n"); nc=1; nl++;} else nc++; break;
  case 2: nl=NW; break;
  case 3: nc=0; nl=0; break;
  default: break;};
if (nl==NW) {
  nl=1;
  printf(" press any key to continue\015");
  while (kbdst()) {};
  kbdin();
  printf("                          \015");
  videoscroll(1,0,24,14,0,0);
  videocursor(0,0,0);
  printf("\n");
  };
}

/* THREE21.C */

/* Pass 1 marks sequences conforming to (3,-3+l) */
cpass1(l) int l; {
char arry[KK][KK][KK][KK][KK][KK][KK];
int  i0, i1, i2, i3, i4, i5, i6;
int  j0, j1, j2, j3, j4;
int  i, j, k, m;
mc=1;
asctobin();
printf(" pass1 \015");
for (i0=0; i0<KK; i0++) {
for (i1=0; i1<KK; i1++) {
for (i2=0; i2<KK; i2++) {
for (i3=0; i3<KK; i3++) {
for (i4=0; i4<KK; i4++) {
for (i5=0; i5<KK; i5++) {
for (i6=0; i6<KK; i6++) {
j0=binrule[i0][i1][i2];
j1=binrule[i1][i2][i3];
j2=binrule[i2][i3][i4];
j3=binrule[i3][i4][i5];
j4=binrule[i4][i5][i6];
i=binrule[j0][j1][j2];
j=binrule[j1][j2][j3];
k=binrule[j2][j3][j4];
switch (l) {
  case 0: m=i0; break;
  case 1: m=i1; break;
  case 2: m=i2; break;
  case 3: m=i3; break;
  case 4: m=i4; break;
  case 5: m=i5; break;
  case 6: m=i6; break;
  default: break;};
arry[i0][i1][i2][i3][i4][i5][i6]=binrule[i][j][k]==m?'Y':'N';
};};};};};};};
cpass2i(arry);
cpass2o(arry);
cpass4(arry);
}

/* Pass 2i flags links which have an incoming arrow */
cpass2i(arry) char arry[KK][KK][KK][KK][KK][KK][KK]; {
int i0, i1, i2, i3, i4, i5, i6, m;
do {
printf(" pass2i\015");
for (i0=0; i0<KK; i0++) {
for (i1=0; i1<KK; i1++) {
for (i2=0; i2<KK; i2++) {
for (i3=0; i3<KK; i3++) {
for (i4=0; i4<KK; i4++) {
for (i5=0; i5<KK; i5++) {
for (i6=0; i6<KK; i6++) {
if ((arry[i0][i1][i2][i3][i4][i5][i6]&0x5F)=='Y')
 {for (m=0; m<KK; m++) arry[i1][i2][i3][i4][i5][i6][m]|=0x20;};
};};};};};};};
} while (cpass3(arry)!=0); }

/* Pass 2o flags links which have an outgoing arrow */
cpass2o(arry) char arry[KK][KK][KK][KK][KK][KK][KK]; {
int i0, i1, i2, i3, i4, i5, i6, m;
do {
printf(" pass2o\015");
for (i0=0; i0<KK; i0++) {
for (i1=0; i1<KK; i1++) {
for (i2=0; i2<KK; i2++) {
for (i3=0; i3<KK; i3++) {
for (i4=0; i4<KK; i4++) {
for (i5=0; i5<KK; i5++) {
for (i6=0; i6<KK; i6++) {
if ((arry[i0][i1][i2][i3][i4][i5][i6]&0x5F)=='Y')
 {for (m=0; m<KK; m++) arry[m][i0][i1][i2][i3][i4][i5]|=0x20;};
};};};};};};};
} while (cpass3(arry)!=0); }

/*  Pass 3 erases flags, marks survivors, counts channges */
int cpass3(arry) char arry[KK][KK][KK][KK][KK][KK][KK]; {
int i0, i1, i2, i3, i4, i5, i6, l;
l=0;
printf(" pass3 \015");
for (i0=0; i0<KK; i0++) {
for (i1=0; i1<KK; i1++) {
for (i2=0; i2<KK; i2++) {
for (i3=0; i3<KK; i3++) {
for (i4=0; i4<KK; i4++) {
for (i5=0; i5<KK; i5++) {
for (i6=0; i6<KK; i6++) {
switch (arry[i0][i1][i2][i3][i4][i5][i6]) {
    case 'y': arry[i0][i1][i2][i3][i4][i5][i6]='Y'; break;
    case 'Y': arry[i0][i1][i2][i3][i4][i5][i6]='N'; l=1; break;
    case 'n': case 'N': arry[i0][i1][i2][i3][i4][i5][i6]='N'; break;
    default: break; };
};};};};};};};
return l; }

/* Pass 4 prints loops which remain */
cpass4(arry) char arry[KK][KK][KK][KK][KK][KK][KK]; {
int i0, i1, i2, i3, i4, i5, i6;
int j0, j1, j2, j3, j4, j5, j6;
int  k, l, m;
printf(" pass4 \015");
for (i0=0; i0<KK; i0++) {
for (i1=0; i1<KK; i1++) {
for (i2=0; i2<KK; i2++) {
for (i3=0; i3<KK; i3++) {
for (i4=0; i4<KK; i4++) {
for (i5=0; i5<KK; i5++) {
for (i6=0; i6<KK; i6++) {
j0=i0; j1=i1; j2=i2; j3=i3; j4=i4; j5=i5; j6=i6;
l=0;
m=0;
do {
        if (arry[0][j1][j2][j3][j4][j5][j6]=='Y')
    {arry[0][j1][j2][j3][j4][j5][j6]='y';
    k=j6; j6=j5; j5=j4; j4=j3; j3=j2; j2=j1; j1=0; m=1;}
  else {if (arry[1][j1][j2][j3][j4][j5][j6]=='Y')
    {arry[1][j1][j2][j3][j4][j5][j6]='y';
    k=j6; j6=j5; j5=j4; j4=j3; j3=j2; j2=j1; j1=1; m=1;}
  else {l=1;
    if (m==1) {j0=j1; j1=j2; j2=j3; j3=j4; j4=j5; j5=j6; j6=k;};
    };};
  } while (l==0);
l=0; 
m=0;
do {
        if (arry[j0][j1][j2][j3][j4][j5][0]=='y')
   {cprf(j0,j1,j2,j3,j4,j5,0);
   arry[j0][j1][j2][j3][j4][j5][0]='N';
   j0=j1; j1=j2; j2=j3; j3=j4; j4=j5; j5=0; m=1;}
  else {if (arry[j0][j1][j2][j3][j4][j5][1]=='y')
   {cprf(j0,j1,j2,j3,j4,j5,1);
   arry[j0][j1][j2][j3][j4][j5][1]='N';
   j0=j1; j1=j2; j2=j3; j3=j4; j4=j5; j5=1; m=1;}
  else {l=1;};};
  } while (l==0);
l=0;
do {
        if (arry[j0][j1][j2][j3][j4][j5][0]=='Y')
   {cprf(j0,j1,j2,j3,j4,j5,0);
   arry[j0][j1][j2][j3][j4][j5][0]='N';
   j0=j1; j1=j2; j2=j3; j3=j4; j4=j5; j5=0; m=1;}
  else {if (arry[j0][j1][j2][j3][j4][j5][1]=='Y')
   {cprf(j0,j1,j2,j3,j4,j5,1);
   arry[j0][j1][j2][j3][j4][j5][1]='N';
   j0=j1; j1=j2; j2=j3; j3=j4; j4=j5; j5=1; m=1;}
  else {l=1; if (m==1) kwait(0);};};
  } while (l==0);
};};};};};};};
}

/* print one of the individual links in a chain */
cprf(i0,i1,i2,i3,i4,i5,i6)
int i0, i1, i2, i3, i4, i5, i6; {int i, j;
kwait(1);
printf("%1d",i0);
printf("%1d",i1);
printf("%1d",i2);
printf("%1d",i3);
printf("%1d",i4);
printf("%1d",i5);
printf("-");
printf("%1d",i6);
printf(" ");
i=((((i0*KK+i1)*KK+i2)*KK+i3)*KK+i4)*KK+i5;
j=((((i1*KK+i2)*KK+i3)*KK+i4)*KK+i5)*KK+i6;
lijnk(dd,i,j,3);
}

/* FOUR21.C */

/* mark sequences conforming to (4,-4+l) */
dpass1(l) int l; {
char arry[KK][KK][KK][KK][KK][KK][KK][KK][KK];
int i0, i1, i2, i3, i4, i5, i6, i7, i8;
int j0, j1, j2, j3, j4, j5, j6;
int k0, k1, k2, k3, k4;
int i, j, k, m;
mc=1;
asctobin();
printf(" pass1 \015");
for (i0=0; i0<KK; i0++) {
for (i1=0; i1<KK; i1++) {
for (i2=0; i2<KK; i2++) {
for (i3=0; i3<KK; i3++) {
for (i4=0; i4<KK; i4++) {
for (i5=0; i5<KK; i5++) {
for (i6=0; i6<KK; i6++) {
for (i7=0; i7<KK; i7++) {
for (i8=0; i8<KK; i8++) {
j0=binrule[i0][i1][i2];
j1=binrule[i1][i2][i3];
j2=binrule[i2][i3][i4];
j3=binrule[i3][i4][i5];
j4=binrule[i4][i5][i6];
j5=binrule[i5][i6][i7];
j6=binrule[i6][i7][i8];
k0=binrule[j0][j1][j2];
k1=binrule[j1][j2][j3];
k2=binrule[j2][j3][j4];
k3=binrule[j3][j4][j5];
k4=binrule[j4][j5][j6];
i=binrule[k0][k1][k2];
j=binrule[k1][k2][k3];
k=binrule[k2][k3][k4];
switch (l) {
  case 0: m=i0; break;
  case 1: m=i1; break;
  case 2: m=i2; break;
  case 3: m=i3; break;
  case 4: m=i4; break;
  case 5: m=i5; break;
  case 6: m=i6; break;
  case 7: m=i7; break;
  case 8: m=i8; break;
  default: break;};
arry[i0][i1][i2][i3][i4][i5][i6][i7][i8]=binrule[i][j][k]==m?'Y':'N';
};};};};};};};};};
dpass2i(arry);
dpass2o(arry);
dpass4(arry);
}

/* Pass 2i flags links which have an incoming arrow */
dpass2i(arry) char arry[KK][KK][KK][KK][KK][KK][KK][KK][KK]; {
int i0, i1, i2, i3, i4, i5, i6, i7, i8, m;
do {
printf(" pass2i\015");
for (i0=0; i0<KK; i0++) {
for (i1=0; i1<KK; i1++) {
for (i2=0; i2<KK; i2++) {
for (i3=0; i3<KK; i3++) {
for (i4=0; i4<KK; i4++) {
for (i5=0; i5<KK; i5++) {
for (i6=0; i6<KK; i6++) {
for (i7=0; i7<KK; i7++) {
for (i8=0; i8<KK; i8++) {
if ((arry[i0][i1][i2][i3][i4][i5][i6][i7][i8]&0x5F)=='Y')
 {for (m=0; m<KK; m++) arry[i1][i2][i3][i4][i5][i6][i7][i8][m]|=0x20;};
};};};};};};};};};
} while (dpass3(arry)!=0); }

/* Pass 2o flags links which have an outgoing arrow */
dpass2o(arry) char arry[KK][KK][KK][KK][KK][KK][KK][KK][KK]; {
int i0, i1, i2, i3, i4, i5, i6, i7, i8, m;
do {
printf(" pass2o\015");
for (i0=0; i0<KK; i0++) {
for (i1=0; i1<KK; i1++) {
for (i2=0; i2<KK; i2++) {
for (i3=0; i3<KK; i3++) {
for (i4=0; i4<KK; i4++) {
for (i5=0; i5<KK; i5++) {
for (i6=0; i6<KK; i6++) {
for (i7=0; i7<KK; i7++) {
for (i8=0; i8<KK; i8++) {
if ((arry[i0][i1][i2][i3][i4][i5][i6][i7][i8]&0x5F)=='Y')
 {for (m=0; m<KK; m++) arry[m][i0][i1][i2][i3][i4][i5][i6][i7]|=0x20;};
};};};};};};};};};
} while (dpass3(arry)!=0); }

/* erase flags, mark survivors, count channges */
int dpass3(arry) char arry[KK][KK][KK][KK][KK][KK][KK][KK][KK]; {
int i0, i1, i2, i3, i4, i5, i6, i7, i8, l;
l=0;
printf(" pass3 \015");
for (i0=0; i0<KK; i0++) {
for (i1=0; i1<KK; i1++) {
for (i2=0; i2<KK; i2++) {
for (i3=0; i3<KK; i3++) {
for (i4=0; i4<KK; i4++) {
for (i5=0; i5<KK; i5++) {
for (i6=0; i6<KK; i6++) {
for (i7=0; i7<KK; i7++) {
for (i8=0; i8<KK; i8++) {
switch (arry[i0][i1][i2][i3][i4][i5][i6][i7][i8]) {
    case 'y': arry[i0][i1][i2][i3][i4][i5][i6][i7][i8]='Y'; break;
    case 'Y': arry[i0][i1][i2][i3][i4][i5][i6][i7][i8]='N'; l=1; break;
    case 'n': case 'N': arry[i0][i1][i2][i3][i4][i5][i6][i7][i8]='N'; break;
    default: break; };
};};};};};};};};};
return l; }

/* Pass4 - print loops which remain */
dpass4(arry) char arry[KK][KK][KK][KK][KK][KK][KK][KK][KK]; {
int i0, i1, i2, i3, i4, i5, i6, i7, i8;
int j0, j1, j2, j3, j4, j5, j6, j7, j8;
int k, l, m;
printf(" pass4 \015");
for (i0=0; i0<KK; i0++) {
for (i1=0; i1<KK; i1++) {
for (i2=0; i2<KK; i2++) {
for (i3=0; i3<KK; i3++) {
for (i4=0; i4<KK; i4++) {
for (i5=0; i5<KK; i5++) {
for (i6=0; i6<KK; i6++) {
for (i7=0; i7<KK; i7++) {
for (i8=0; i8<KK; i8++) {
j0=i0; j1=i1; j2=i2; j3=i3; j4=i4; j5=i5; j6=i6; j7=i7; j8=i8;
l=0;
m=0;
do {
        if (arry[0][j1][j2][j3][j4][j5][j6][j7][j8]=='Y')
    {arry[0][j1][j2][j3][j4][j5][j6][j7][j8]='y';
    k=j8; j8=j7; j7=j6; j6=j5; j5=j4; j4=j3; j3=j2; j2=j1; j1=0; m=1;}
  else {if (arry[1][j1][j2][j3][j4][j5][j6][j7][j8]=='Y')
    {arry[1][j1][j2][j3][j4][j5][j6][j7][j8]='y';
    k=j8; j8=j7; j7=j6; j6=j5; j5=j4; j4=j3; j3=j2; j2=j1; j1=1; m=1;}
  else {l=1;
    if (m==1) {j0=j1; j1=j2; j2=j3; j3=j4; j4=j5; j5=j6; j6=j7; j7=j8; j8=k;};
    };};
  } while (l==0);
l=0; 
m=0;
do {
        if (arry[j0][j1][j2][j3][j4][j5][j6][j7][0]=='y')
   {dprf(j0,j1,j2,j3,j4,j5,j6,j7,0);
   arry[j0][j1][j2][j3][j4][j5][j6][j7][0]='N';
   j0=j1; j1=j2; j2=j3; j3=j4; j4=j5; j5=j6; j6=j7; j7=0; m=1;}
  else {if (arry[j0][j1][j2][j3][j4][j5][j6][j7][1]=='y')
   {dprf(j0,j1,j2,j3,j4,j5,j6,j7,1);
   arry[j0][j1][j2][j3][j4][j5][j6][j7][1]='N';
   j0=j1; j1=j2; j2=j3; j3=j4; j4=j5; j5=j6; j6=j7; j7=1; m=1;}
  else {l=1;};};
  } while (l==0);
l=0;
do {
        if (arry[j0][j1][j2][j3][j4][j5][j6][j7][0]=='Y')
   {dprf(j0,j1,j2,j3,j4,j5,j6,j7,0);
   arry[j0][j1][j2][j3][j4][j5][j6][j7][0]='N';
   j0=j1; j1=j2; j2=j3; j3=j4; j4=j5; j5=j6; j6=j7; j7=0; m=1;}
  else {if (arry[j0][j1][j2][j3][j4][j5][j6][j7][1]=='Y')
   {dprf(j0,j1,j2,j3,j4,j5,j6,j7,1);
   arry[j0][j1][j2][j3][j4][j5][j6][j7][1]='N';
   j0=j1; j1=j2; j2=j3; j3=j4; j4=j5; j5=j6; j6=j7; j7=1; m=1;}
  else {l=1; if (m==1) kwait(0);};};
  } while (l==0);
};};};};};};};};};
}

/* print one of the individual links in a chain */
dprf(i0,i1,i2,i3,i4,i5,i6,i7,i8)
int i0, i1, i2, i3, i4, i5, i6, i7, i8; {
int i, j;
kwait(1);
printf("%1d",i0);
printf("%1d",i1);
printf("%1d",i2);
printf("%1d",i3);
printf("%1d",i4);
printf("%1d",i5);
printf("%1d",i6);
printf("%1d",i7);
printf("-");
printf("%1d",i8);
printf(" ");
i=((((((i0*KK+i1)*KK+i2)*KK+i3)*KK+i4)*KK+i5)*KK+i6)*KK+i7;
j=((((((i1*KK+i2)*KK+i3)*KK+i4)*KK+i5)*KK+i6)*KK+i7)*KK+i8;
lijnk(dd,i,j,3);
}

/* end */
