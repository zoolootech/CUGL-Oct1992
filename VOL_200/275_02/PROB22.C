
/* prob22.c						*/
/* program for LCA22 option 't'				*/
/* calculate probabilities related to evolution		*/
/* Harold V. McIntosh, 10 August 1987			*/
/* 29 February 1988 - adapted from LCA21 to LCA22	*/

/* references:							*/
/*								*/
/* W. John Wilbur, David J. Lipman and Shihab A. Shamma		*/
/* On the prediction of local patterns in cellular automata	*/
/* Physica 19D 397-410 (1986)					*/ 
/*								*/
/* Howard A. Gutowitz, Jonathan D. Victor and Bruce W. Knight	*/
/* Local structure theory for cellular automata			*/
/* Physica 28D 18-48 (1987)					*/

/*  Copyright (C) 1987  */
/*  Copyright (C) 1988  */
/*  Harold V. McIntosh  */
/*  Gerardo Cisneros S. */

# define BROW	    3	/* row for bar charts		*/
# define EROW	    1	/* row for evolution synopsis	*/
# define AORG 	    0	/* x-origin  of contour plot    */
# define BORG 	  109	/* x-origin  of 2-block param   */
# define CORG 	  219	/* x-origin  of Bernstein plot  */

/* edit the probability screen	*/
edtri() {char c;

  videomode(COLGRAF);
  videopalette(YELREGR);

  while (0<1) {
  videocursor(0,0,0);
  videoputc(':',2);
  scrrul();
  videocursor(0,0,36);
  videoputc('?',2);
  c=kbdin();
  if (c == '\015') break;
  videocursor(0,0,38);
  videoputc(c,2);
  videocursor(0,0,36);
  videoputc(' ',0);
  switch (c) {
    case '+': videopalette(WHCYMAG); break;
    case '-': videopalette(YELREGR); break;
    case 'a': asfreq(3); break;
    case 'e': pevolve(); break;
    case 'g': lifreq(50,2); break;
    case 'G': lifreq(200,1); break;
    case 'm': moncar(1,2); break;
    case 'M': moncar(50,1); break;
    case 'x': pdiff(100); break;
    case 'i': pidiff(100); break;
    case 'j': pjdiff(100); break;
    case 'y': pdiff3(100); break;
    case 'z': pdiff4(100); break;
    case 't': twoblockfreq(100); break;
    case '1': nblclr(); oneblfreq(8*BROW,300,48); break;
    case '2': nblclr(); twoblfreq(8*BROW,300,48); break;
    case '3': nblclr(); thrblfreq(8*BROW,300,48); break;
    case '4': nblclr(); foublfreq(8*BROW,300,48); break;
    case '5': nblclr(); fivblfreq(8*BROW,300,48); break;
    case '6': nblclr(); sixblfreq(8*BROW,300,48); break;
    case '?': videomode(COLGRAF); videopalette(YELREGR); trmenu(); break;
    case '/': videomode(COLGRAF); videopalette(YELREGR); break;
    default: break;
    }; /* end switch */
  };   /* end while  */
  videopalette(WHCYMAG);
  videomode(T80X25);
}      /* end edtri  */

/* show menu */
trmenu() {
  videoscroll(BROW,0,BROW+8,40,0,0);
  videocursor(0,BROW,0);
  printf("a       - a priori estimates\n");
  printf("m,M,g,G - sample evolution\n");
  printf("xyz     - selfconsistent probabilities\n");
  printf("xij     - iterated s-c probabilities\n");
  printf("t       - graph 2 block probabilities\n");
  printf("123456  - n-block bar charts\n");
  printf("+-      - change color pallette\n");
  printf("e       - 12 lines evolution\n");
  printf("/?(clear screen, show menu), <cr>(exit)\n");
}

/* show twelve lines of evolution at top of screen */
pevolve() {int i, j;
  videoscroll(EROW,0,EROW+1,40,0,0);
  asctobin();
  for (j=8*EROW; j<8*(EROW+2)-3; j++) {
    for (i=0; i<AL; i++) videodot(j,i,arr1[i]);
    onegen(AL);
    };
}

/* Clear a space for the n-block frequencies */
nblclr() {videoscroll(BROW,0,BROW+8,40,0,0);}

/* make a frame for a graph 		 */
/* (x,y) = lower left corner; e.g. (0,0) */
/* n     = dimension of frame            */
gfram(x,y,n) int x, y, n; {int i;

for (i=0; i<=n; i++) videodot(199-y-i,x,0);
for (i=0; i<=n; i++) videodot(199-y-i,x+n,0);
for (i=0; i<=n; i++) videodot(199-n-y,x+i,0);
for (i=0; i<=n; i++) videodot(199-y,x+i,0);

for (i=0; i<=n; i+=10) videodot(199-y-i,x,3);
for (i=0; i<=n; i+=10) videodot(199-y-i,x+n,3);
for (i=0; i<=n; i+=10) videodot(199-n-y,x+i,3);
for (i=0; i<=n; i+=10) videodot(199-y,x+i,3);
}

/* put a diagonal in a graph */
gdiag(x,y,n) int x, y, n; {int i;
for (i=0; i<=n; i+=2) videodot(199-y-i,x+i,3);
}

/* graph Bernstein polynomial */
bgraf(x,y,k,n) int x, y, k, n; {int i; double bern(), en, dp, p;
if (n==0) return;
en=(double)(n); dp=1.0/en;
for (i=0,p=0.0; i<n; i++,p+=dp) {videodot(199-y-(int)(en*bern(p,k)),x+i,1);};
}

/* "Monte Carlo" determination of probabilities */
moncar(n,l) int n, l; {
int i, j, k, b[KK], bb[KK][KK];
double bf[KK], bbf[KK][KK];

nblclr();
gfram(BORG,0,100);
asctobin();
for (k=0; k<n; k++) {
  onegen(AL);
  for (i=0; i<KK; i++) b[i]=0;
  for (i=0; i<AL; i++) b[arr1[i]]++;
  for (i=0; i<KK; i++) bf[i]=((double)(b[i]))/((double)(AL));
  for (i=0; i<KK; i++) for (j=0; j<KK; j++) bb[i][j]=0;
  for (i=1; i<AL; i++) bb[arr1[i-1]][arr1[i]]++;
  bb[arr1[AL-1]][arr1[0]]++; 
  for (i=0; i<KK; i++) for (j=0; j<KK; j++) 
    bbf[i][j]=((double)(bb[i][j]))/((double)(AL));
  videodot(199-(int)(100.0*bbf[1][1]),BORG+(int)(100.0*bf[1]),l);
  };
 videocursor(0,BROW+7,0);
printf("(Monte Carlo) "); 
for (i=0; i<KK; i++) printf("%2d:%5.3f ",i,bf[i]);
videocursor(0,BROW+8,0);
for (i=0; i<KK; i++) for (j=0; j<KK; j++) 
  printf("%1d%1d:%5.3f ",i,j,bbf[i][j]);
}

/* Generate coefficients of 2nd generation Bernstein Polynomial */
berncoef() {
int i, i0, i1, i2, i3, i4;

for (i=0; i<BD; i++) bp[i]=0.0;
for (i0=0; i0<KK; i0++) {
for (i1=0; i1<KK; i1++) {
for (i2=0; i2<KK; i2++) {
for (i3=0; i3<KK; i3++) {
for (i4=0; i4<KK; i4++) {
  if (ascrule[i0][i1][i2][i3][i4]=='1') bp[i0+i1+i2+i3+i4]+=1.0;
  };};};};};
}

/* Generate coefficients of 3rd generation Bernstein Polynomial */
bernthrd() {
int i, i0, i1, i2, i3, i4, i5, i6, i7, i8;
int j0, j1, j2, j3, j4;

asctobin();
for (i=0; i<BD; i++) bp[i]=0.0;
for (i0=0; i0<KK; i0++) {
for (i1=0; i1<KK; i1++) {
for (i2=0; i2<KK; i2++) {
for (i3=0; i3<KK; i3++) {
for (i4=0; i4<KK; i4++) {
for (i5=0; i5<KK; i5++) {
for (i6=0; i6<KK; i6++) {
for (i7=0; i7<KK; i7++) {
for (i8=0; i8<KK; i8++) {
  j0=binrule[i0][i1][i2][i3][i4];
  j1=binrule[i1][i2][i3][i4][i5];
  j2=binrule[i2][i3][i4][i5][i6];
  j3=binrule[i3][i4][i5][i6][i7];
  j4=binrule[i4][i5][i6][i7][i8];
  if (ascrule[j0][j1][j2][j3][j4]=='1') bp[i0+i1+i2+i3+i4+i5+i6+i7+i8]+=1.0;
  };};};};};};};};};
}

/* Generate coefficients of 4th generation Bernstein Polynomial */
bernfrth() {
int i, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, ia, ib, ic;
int j0, j1, j2, j3, j4, j5, j6, j7, j8;
int k0, k1, k2, k3, k4;

asctobin();
for (i=0; i<BD; i++) bp[i]=0.0;
for (i0=0; i0<KK; i0++) {
for (i1=0; i1<KK; i1++) {
for (i2=0; i2<KK; i2++) {
for (i3=0; i3<KK; i3++) {
for (i4=0; i4<KK; i4++) {
for (i5=0; i5<KK; i5++) {
for (i6=0; i6<KK; i6++) {
for (i7=0; i7<KK; i7++) {
for (i8=0; i8<KK; i8++) {
for (i9=0; i9<KK; i9++) {
for (ia=0; ia<KK; ia++) {
for (ib=0; ib<KK; ib++) {
for (ic=0; ic<KK; ic++) {
  j0=binrule[i0][i1][i2][i3][i4];
  j1=binrule[i1][i2][i3][i4][i5];
  j2=binrule[i2][i3][i4][i5][i6];
  j3=binrule[i3][i4][i5][i6][i7];
  j4=binrule[i4][i5][i6][i7][i8];
  j5=binrule[i5][i6][i7][i8][i9];
  j6=binrule[i6][i7][i8][i9][ia];
  j7=binrule[i7][i8][i9][ia][ib];
  j8=binrule[i8][i9][ia][ib][ic];
  k0=binrule[j0][j1][j2][j3][j4];
  k1=binrule[j1][j2][j3][j4][j5];
  k2=binrule[j2][j3][j4][j5][j6];
  k3=binrule[j3][j4][j5][j6][j7];
  k4=binrule[j4][j5][j6][j7][j8];
  i=i0+i1+i2+i3+i4+i5+i6+i7+i8+i9+ia+ib+ic;
  if (ascrule[k0][k1][k2][k3][k4]=='1') bp[i]+=1.0;
  };};};};};};};};};};};};};
}

/* evaluate the nth generation Bernstein polynomial at point p */
double bern(p,n) double p; int n; {double q, s, x, r; int i, d;
d=4*n-3;
if (p>0.99) return bp[d];
q=1.0-p; r=p/q; s=0.0; x=1.0;
for (i=0; i<d;  i++) x*=q;
for (i=0; i<=d; i++, x*=r) s+=bp[i]*x;
s*=0.9998;
return(s+0.0001);
}

/* graph the probability of the second generation */
pdiff(n) int n; {
berncoef();
gfram(CORG,0,n);
gdiag(CORG,0,n);
bgraf(CORG,0,2,n);
}

/* graph the iterated probability of the second generation */
pidiff(n) int n; {int i; double bern(), en, p;
en=1.0/((double)(n));
berncoef();
gdiag(CORG,0,n);
for (i=0; i<=n; i++) {
  p=((double)(i))*en;
  videodot(199-(int)(100.0*bern(bern(p,2),2)),CORG+(int)(100.0*p),3);
  };
}

/* graph the twice iterated probability of the second generation */
pjdiff(n) int n; {int i; double bern(), en, p;
en=1.0/((double)(n));
berncoef();
gdiag(CORG,0,n);
for (i=0; i<=n; i++) {
  p=((double)(i))*en;
  videodot(199-(int)(100.0*bern(bern(bern(p,2),2),2)),CORG+(int)(100.0*p),3);
  };
}

/* graph the probability of the third generation */
pdiff3(n) int n; {
bernthrd();
gfram(CORG,0,n);
gdiag(CORG,0,n);
bgraf(CORG,0,3,n);
}

/* graph the probability of the fourth generation */
pdiff4(n) int n; {
bernfrth();
gfram(CORG,0,n);
gdiag(CORG,0,n);
bgraf(CORG,0,4,n);
}

/* display frequencies in ascrule in color l */
asfreq(l) int l; {
int    i, j, i0, i1, i2, i3, i4;
int    stat[KK], stall, stal, stac, star, starr;	/* statistic counts */
double staf[KK], pp;

videoscroll(BROW,0,BROW+8,40,0,0);
videocursor(0,BROW,0);
pp=1.0/((double)(KK*KK*KK*KK*KK));
stall=0; stal=0; stac=0; star=0; starr=0;
for (i=0; i<KK; i++) stat[i]=0;
for (i0=0; i0<KK; i0++) {
for (i1=0; i1<KK; i1++) {
for (i2=0; i2<KK; i2++) {
for (i3=0; i3<KK; i3++) {
for (i4=0; i4<KK; i4++) {
  j=ascrule[i0][i1][i2][i3][i4]-'0';
  stat[j]++;
  if(j==i0) starr++;
  if(j==i1) star++;
  if(j==i2) stac++;
  if(j==i3) stal++;
  if(j==i4) stall++;
  };};};};};
for (i=0; i<KK; i++) printf("%2d    - %5.2f\n",i,((double)stat[i])*pp);
printf("\n");
printf("lleft - %5.2f\n",((double)stall)*pp);
printf("left  - %5.2f\n",((double)stal)*pp);
printf("still - %5.2f\n",((double)stac)*pp);
printf("right - %5.2f\n",((double)star)*pp);
printf("rright- %5.2f\n",((double)starr)*pp);
for (i=0; i<KK; i++) staf[i]=((double)stat[i])*pp;
j=199-(int)(100.0*staf[1]);
i=CORG+(int)(100.0*staf[1]);
if (j>=2) videodot(j-2,i,l);
if (i>=2) videodot(j,i-2,l);
videodot(j,i,1);
if (i<=318) videodot(j,i+2,l);
if (j<=198) videodot(j+2,i,l);
}

/* display state frequencies in arr1 as a point on probability graph */
/* n = number of points to plot */
/* l = color of plotted point   */
lifreq
(n,l) int n, l; {int     i,  j, os, ns;
double of, nf, s;

s=100.0/((double)AL);
asctobin();
for (i=0; i<n; i++) {
  os=0; ns=0;
  for (j=0; j<AL; j++) if (arr1[j]==1) os++;
  onegen(AL);
  for (j=0; j<AL; j++) if (arr1[j]==1) ns++;
  of=(double)os;
  nf=(double)ns;
  videodot(199-(int)(s*nf),CORG+(int)(s*of),l);
  };
}

/* evaluate the parameters for two-block probabilities after one generation */
twoblock(z) double *z; {
int    i, i0, i1, i2, i3, i4, i5;
int    j, j0, j1;
double w, x, y, p, q, pp;
double a[KK][KK], b[KK], c[KK][KK];

p=z[0]; q=1.0-p; pp=z[1];
a[1][1]=pp; a[1][0]=a[0][1]=p-pp; a[0][0]=1.0-2*p+pp;
b[1]=p; b[0]=q;
for (i=0; i<KK; i++) {for (j=0; j<KK; j++) c[i][j]=0.0;};
for (i0=0; i0<KK; i0++) {
for (i1=0; i1<KK; i1++) {
for (i2=0; i2<KK; i2++) {
for (i3=0; i3<KK; i3++) {
for (i4=0; i4<KK; i4++) {
for (i5=0; i5<KK; i5++) {
  j0=ascrule[i0][i1][i2][i3][i4]-'0';
  j1=ascrule[i1][i2][i3][i4][i5]-'0';
  x=a[i0][i1]*a[i1][i2]*a[i2][i3]*a[i3][i4]*a[i4][i5];
  y=b[i1]*b[i2]*b[i3]*b[i4];
  w=0.0; if (y>0.00001) w=x/y;
  c[j0][j1]+=w;
  };};};};};};
z[0]=c[1][0]+c[1][1]; z[1]=c[1][1];
}

/* graph the 2-block parameter differences to find self-consistent values */
/* nxn points 								  */
twoblockfreq(n) int n; {
int i, j, l;
double op[2], np[2], s, t, u, v;

gfram(0,0,100);

s=1.0/((double)(n));
op[1]=s;
for (i=1; i<n; i++) {
  op[0]=s;
  for (j=1; j<n; j++) {
    np[0]=op[0]; np[1]=op[1]; twoblock(np);
    u=np[0]-op[0]; v=np[1]-op[1];
    t=u*u+v*v;
    if (t<0.0025) l=0; else {
    if (t<0.0675) {if((i+j)%2) l=0; else l=3;} else {
    if (t<0.3906) l=3; else {
    if (t<0.7500) {if((i+j)%2) l=3; else l=2;} else {
    if (t<1.5000) l=2; else {
    if (t<3.0000) {if((i+j)%2) l=2; else l=1;} else {
    if (t<6.0000) l=1; else {
                  if((i+j)%2) l=1; else l=0;
       }}}}}}}
    videodot(199-i,j,l);
    if (kbdst()) {kbdin(); return;};
    op[0]+=s;
    };  /* end for-j */
  op[1]+=s;
  };    /* end for-i */
}

/* evaluate the one-block probabilities after one generation */
onebl(x,a) double x[KK], a[KK]; {int i0, i1, i2, i3, i4, j0;

for (j0=0; j0<KK; j0++) x[j0]=0.0;

for (i0=0; i0<KK; i0++) {
for (i1=0; i1<KK; i1++) {
for (i2=0; i2<KK; i2++) {
for (i3=0; i3<KK; i3++) {
for (i4=0; i4<KK; i4++) {
  j0=ascrule[i0][i1][i2][i3][i4]-'0';
  x[j0]+=a[i0]*a[i1]*a[i2]*a[i3]*a[i4];
  };};};};};
}

/* iterate the 1-block parameters to find self-consistent values */
/* graph the iterative steps in bar-chart form			 */
/* ll - initial line	*/
/* mm - length of line	*/
/* nn - number of lines	*/
oneblfreq(ll,mm,nn) int ll, mm, nn; {
int    ii, i, l, m, n;
double op[KK], np[KK];
double d, e, f, s;

m=0;
f=(double)mm;
s=1.0/((double)(KK));
n=(int)(f*s);
videodot(ll,m++,3);
for (i=0; i<KK; i++) {op[i]=s; for (l=0; l<n; l++) videodot(ll,m++,i);};

for (ii=1; ii<=nn; ii++) {
  e=0.0; m=0;
  onebl(np,op);
  videodot(ll+ii,m++,3);
  for (i=0; i<KK; i++) {
    n=(int)(f*np[i]); if (n>0) for (l=0; l<n; l++) videodot(ll+ii,m++,i);
    d=op[i]-np[i];
    e+=d*d;
    op[i]=np[i];
    };
  videodot(ll+ii,m++,3);
  if (op[0]<=0.001) break;
  if (op[1]<=0.001) break;
  if (e<=0.0000001) break;
  if (kbdst()) {kbdin(); break;};
  };
videocursor(0,BROW+7,0);
printf("(1-block) "); 
for (i=0; i<KK; i++) printf("%2d:%5.3f ",i,op[i]);
}

/* evaluate the two-block probabilities after one generation */
twobl(x,a) double x[KK][KK], a[KK][KK]; {
int    i0, i1, i2, i3, i4, i5;
int    j0, j1;
double w, b[KK];

for (j0=0; j0<KK; j0++) {for (j1=0; j1<KK; j1++) x[j0][j1]=0.0;};

for (j0=0; j0<KK; j0++) {b[j0]=0.0; for (j1=0; j1<KK; j1++) b[j0]+=a[j0][j1];};

for (i0=0; i0<KK; i0++) {
for (i1=0; i1<KK; i1++) {
for (i2=0; i2<KK; i2++) {
for (i3=0; i3<KK; i3++) {
for (i4=0; i4<KK; i4++) {
for (i5=0; i5<KK; i5++) {
  j0=ascrule[i0][i1][i2][i3][i4]-'0';
  j1=ascrule[i1][i2][i3][i4][i5]-'0';
  w=a[i0][i1]*a[i1][i2]*a[i2][i3]*a[i3][i4]*a[i4][i5];
  if (w!=0.0) w/=b[i1]*b[i2]*b[i3]*b[i4];
  x[j0][j1]+=w;
  };};};};};};
}

/* iterate the 2-block parameters to find self-consistent values */
/* graph the iterative steps in bar-chart form			 */
/* ll - initial line	*/
/* mm - length of line	*/
/* nn - number of lines	*/
twoblfreq(ll,mm,nn) int ll, mm, nn; {
int    ii, i, j, l, m, n;
double op[KK][KK], np[KK][KK];
double b[KK], d, e, f, s;

m=0;
f=(double)mm;
s=1.0/((double)(KK*KK));
n=(int)(f*s);
videodot(ll,m++,3);
for (i=0; i<KK; i++) {
for (j=0; j<KK; j++) {
  op[i][j]=s;
  for (l=0; l<n; l++) videodot(ll,m++,j);
  };};
videodot(ll,m++,3);

for (i=0; i<KK; i++) {b[i]=0.0; for (j=0; j<KK; j++) b[i]+=op[i][j];};
videodot(199-(int)(100.0*op[1][1]),(int)(100.0*b[1]),1);

for (ii=1; ii<=nn; ii++) {
  e=0.0; m=0;
  twobl(np,op);
  videodot(ll+ii,m++,3);
  for (i=0; i<KK; i++) {
  for (j=0; j<KK; j++) {
    n=(int)(f*np[i][j]);
    if (n>0) for (l=0; l<n; l++) videodot(ll+ii,m++,j);
    d=op[i][j]-np[i][j];
    e+=d<0.0?-d:d;
    op[i][j]=np[i][j];
    };};
  videodot(ll+ii,m++,3);
  for (i=0; i<KK; i++) {b[i]=0.0; for (j=0; j<KK; j++) b[i]+=op[i][j];};
  videodot(199-(int)(100.0*op[1][1]),(int)(100.0*b[1]),2);
  if (e<=0.0001) break;
  if (kbdst()) {kbdin(); break;};
  };
videocursor(0,BROW+7,0);
printf("(2-block) "); 
for (i=0; i<KK; i++) printf("%2d:%5.3f ",i,b[i]);
videocursor(0,BROW+8,0);
for (i=0; i<KK; i++) for (j=0; j<KK; j++) 
		printf("%1d%1d:%5.3f ",i,j,op[i][j]);
videodot(199-(int)(100.0*op[1][1]),(int)(100.0*b[1]),3);
videodot(199-(int)(100.0*op[1][1]),BORG+(int)(100.0*b[1]),1);
}

/* evaluate the three-block probabilities after one generation */
thrbl(x,a) double x[KK][KK][KK], a[KK][KK][KK]; {
int    i0, i1, i2, i3, i4, i5, i6;
int    j0, j1, j2;
double w, b[KK][KK];

for (j0=0; j0<KK; j0++) {
for (j1=0; j1<KK; j1++) {
  for (j2=0; j2<KK; j2++) x[j0][j1][j2]=0.0;
  b[j0][j1]=0.0;
  for (j2=0; j2<KK; j2++) b[j0][j1]+=a[j0][j1][j2];  
  };};

for (i0=0; i0<KK; i0++) {
for (i1=0; i1<KK; i1++) {
for (i2=0; i2<KK; i2++) {
for (i3=0; i3<KK; i3++) {
for (i4=0; i4<KK; i4++) {
for (i5=0; i5<KK; i5++) {
for (i6=0; i6<KK; i6++) {
  j0=ascrule[i0][i1][i2][i3][i4]-'0';
  j1=ascrule[i1][i2][i3][i4][i5]-'0';
  j2=ascrule[i2][i3][i4][i5][i6]-'0';
  w=a[i0][i1][i2]*a[i1][i2][i3]*a[i2][i3][i4]*a[i3][i4][i5]*a[i4][i5][i6];
  if (w!=0.0) w/=b[i1][i2]*b[i2][i3]*b[i3][i4]*b[i4][i5];
  x[j0][j1][j2]+=w;
  };};};};};};};
}

/* iterate the 3-block parameters to find self-consistent values */
/* ll - initial line	*/
/* mm - length of line	*/
/* nn - number of lines	*/
thrblfreq(ll,mm,nn) int ll, mm, nn; {
int    ii, i, j, k, l, m, n;
double op[KK][KK][KK], np[KK][KK][KK];
double b[KK], bb[KK][KK], d, e, f, s;

m=0;
f=(double)mm;
s=1.0/((double)(KK*KK*KK));
n=(int)(f*s);
videodot(ll,m++,3);
for (i=0; i<KK; i++) {
for (j=0; j<KK; j++) {
for (k=0; k<KK; k++) {
  op[i][j][k]=s;
  for (l=0; l<n; l++) videodot(ll,m++,k);
  };};};
videodot(ll,m++,3);

for (ii=1; ii<=nn; ii++) {
  e=0.0; m=0;
  thrbl(np,op);
  videodot(ll+ii,m++,3);
  for (i=0; i<KK; i++) {
  for (j=0; j<KK; j++) {
  for (k=0; k<KK; k++) {
    n=(int)(f*np[i][j][k]);
    if (n>0) for (l=0; l<n; l++) videodot(ll+ii,m++,k);
    d=op[i][j][k]-np[i][j][k];
    e+=d<0.0?-d:d;
    op[i][j][k]=np[i][j][k];
    };};};
  videodot(ll+ii,m++,3);
  if (e<=0.0001) break;
  if (kbdst()) {kbdin(); break;};
  };

for (i=0; i<KK; i++) {
  b[i]=0.0;
  for (j=0; j<KK; j++) {
  for (k=0; k<KK; k++) {
    b[i]+=op[i][j][k];
    };}; };
videocursor(0,BROW+7,0);
printf("(3-block) "); 
for (i=0; i<KK; i++) printf("%2d:%5.3f ",i,b[i]);

for (i=0; i<KK; i++) {
for (j=0; j<KK; j++) {
  bb[i][j]=0.0;
  for (k=0; k<KK; k++) {
    bb[i][j]+=op[i][j][k];
    };}; };

videocursor(0,BROW+8,0);
for (i=0; i<KK; i++) for (j=0; j<KK; j++) 
  printf("%1d%1d:%5.3f ",i,j,bb[i][j]);
videodot(199-(int)(100.0*bb[1][1]),BORG+(int)(100.0*b[1]),2);
}

/* evaluate the four-block probabilities after one generation */
foubl(x,a) double x[KK][KK][KK][KK], a[KK][KK][KK][KK]; {
int    i0, i1, i2, i3, i4, i5, i6, i7;
int    j0, j1, j2, j3;
double w, b[KK][KK][KK];

for (j0=0; j0<KK; j0++) {
for (j1=0; j1<KK; j1++) {
for (j2=0; j2<KK; j2++) {
  for (j3=0; j3<KK; j3++) x[j0][j1][j2][j3]=0.0;
  b[j0][j1][j2]=0.0;
  for (j3=0; j3<KK; j3++) b[j0][j1][j2]+=a[j0][j1][j2][j3];  
  };};};

for (i0=0; i0<KK; i0++) {
for (i1=0; i1<KK; i1++) {
for (i2=0; i2<KK; i2++) {
for (i3=0; i3<KK; i3++) {
for (i4=0; i4<KK; i4++) {
for (i5=0; i5<KK; i5++) {
for (i6=0; i6<KK; i6++) {
for (i7=0; i7<KK; i7++) {
  j0=ascrule[i0][i1][i2][i3][i4]-'0';
  j1=ascrule[i1][i2][i3][i4][i5]-'0';
  j2=ascrule[i2][i3][i4][i5][i6]-'0';
  j3=ascrule[i3][i4][i5][i6][i7]-'0';
  w=a[i0][i1][i2][i3];
  w*=a[i1][i2][i3][i4];
  w*=a[i2][i3][i4][i5];
  w*=a[i3][i4][i5][i6];
  w*=a[i4][i5][i6][i7];
  if (w!=0.0) w/=b[i1][i2][i3]*b[i2][i3][i4]*b[i3][i4][i5]*b[i4][i5][i6];
  x[j0][j1][j2][j3]+=w;
  };};};};};};};};
}

/* iterate the 4-block parameters to find self-consistent values */
/* ll - initial line	*/
/* mm - length of line	*/
/* nn - number of lines	*/
foublfreq(ll,mm,nn) int ll, mm, nn; {
int    ii, i0, i1, i2, i3, l, m, n;
double op[KK][KK][KK][KK], np[KK][KK][KK][KK];
double b[KK], bb[KK][KK], d, e, f, s;

m=0;
f=(double)mm;
s=1.0/((double)(KK*KK*KK*KK));
n=(int)(f*s);
videodot(ll,m++,3);
for (i0=0; i0<KK; i0++) {
for (i1=0; i1<KK; i1++) {
for (i2=0; i2<KK; i2++) {
for (i3=0; i3<KK; i3++) {
  op[i0][i1][i2][i3]=s;
  for (l=0; l<n; l++) videodot(ll,m++,i3);
  };};};};
videodot(ll,m++,3);

for (ii=1; ii<=nn; ii++) {
  e=0.0; m=0;
  foubl(np,op);
  videodot(ll+ii,m++,3);
  for (i0=0; i0<KK; i0++) {
  for (i1=0; i1<KK; i1++) {
  for (i2=0; i2<KK; i2++) {
  for (i3=0; i3<KK; i3++) {
    n=(int)(f*np[i0][i1][i2][i3]);
    if (n>0) for (l=0; l<n; l++) videodot(ll+ii,m++,i3);
    d=op[i0][i1][i2][i3]-np[i0][i1][i2][i3];
    e+=d<0.0?-d:d;
    op[i0][i1][i2][i3]=np[i0][i1][i2][i3];
    };};};};
  videodot(ll+ii,m++,3);
  if (e<=0.0001) break;
  if (kbdst()) {kbdin(); break;};
  };

for (i0=0; i0<KK; i0++) {
  b[i0]=0.0;
  for (i1=0; i1<KK; i1++) {
  for (i2=0; i2<KK; i2++) {
  for (i3=0; i3<KK; i3++) {
    b[i0]+=op[i0][i1][i2][i3];
    };};}; };

videocursor(0,BROW+7,0);
printf("(4-block) "); 
for (i0=0; i0<KK; i0++) printf("%2d:%5.3f ",i0,b[i0]);

for (i0=0; i0<KK; i0++) {
for (i1=0; i1<KK; i1++) {
  bb[i0][i1]=0.0;
  for (i2=0; i2<KK; i2++) {
  for (i3=0; i3<KK; i3++) {
    bb[i0][i1]+=op[i0][i1][i2][i3];
    };}; };};

videocursor(0,BROW+8,0);
for (i0=0; i0<KK; i0++) for (i1=0; i1<KK; i1++) 
  printf("%1d%1d:%5.3f ",i0,i1,bb[i0][i1]);
videodot(199-(int)(100.0*bb[1][1]),BORG+(int)(100.0*b[1]),2);
}

/* evaluate the five-block probabilities after one generation */
fivbl(x,a) double x[KK][KK][KK][KK][KK], a[KK][KK][KK][KK][KK]; {
int    i0, i1, i2, i3, i4, i5, i6, i7, i8;
int    j0, j1, j2, j3, j4;
double w, b[KK][KK][KK][KK];

for (j0=0; j0<KK; j0++) {
for (j1=0; j1<KK; j1++) {
for (j2=0; j2<KK; j2++) {
for (j3=0; j3<KK; j3++) {
  for (j4=0; j4<KK; j4++) x[j0][j1][j2][j3][j4]=0.0;
  b[j0][j1][j2][j3]=0.0;
  for (j4=0; j4<KK; j4++) b[j0][j1][j2][j3]+=a[j0][j1][j2][j3][j4];  
  };};};};

for (i0=0; i0<KK; i0++) {
for (i1=0; i1<KK; i1++) {
for (i2=0; i2<KK; i2++) {
for (i3=0; i3<KK; i3++) {
for (i4=0; i4<KK; i4++) {
for (i5=0; i5<KK; i5++) {
for (i6=0; i6<KK; i6++) {
for (i7=0; i7<KK; i7++) {
for (i8=0; i8<KK; i8++) {
  j0=ascrule[i0][i1][i2][i3][i4]-'0';
  j1=ascrule[i1][i2][i3][i4][i5]-'0';
  j2=ascrule[i2][i3][i4][i5][i6]-'0';
  j3=ascrule[i3][i4][i5][i6][i7]-'0';
  j4=ascrule[i4][i5][i6][i7][i8]-'0';
  w=a[i0][i1][i2][i3][i4];
  w*=a[i1][i2][i3][i4][i5];
  w*=a[i2][i3][i4][i5][i6];
  w*=a[i3][i4][i5][i6][i7];
  w*=a[i4][i5][i6][i7][i8];
  if (w!=0.0) w/=b[i1][i2][i3][i4]*b[i2][i3][i4][i5]*b[i3][i4][i5][i6]*b[i4][i5][i6][i7];
  x[j0][j1][j2][j3][j4]+=w;
  };};};};};};};};};
}

/* iterate the 5-block parameters to find self-consistent values */
/* ll - initial line	*/
/* mm - length of line	*/
/* nn - number of lines	*/
fivblfreq(ll,mm,nn) int ll, mm, nn; {
int    ii, i0, i1, i2, i3, i4, l, m, n;
double op[KK][KK][KK][KK][KK], np[KK][KK][KK][KK][KK];
double b[KK], bb[KK][KK], d, e, f, s;

m=0;
f=(double)mm;
s=1.0/((double)(KK*KK*KK*KK*KK));
n=(int)(f*s);
videodot(ll,m++,3);
for (i0=0; i0<KK; i0++) {
for (i1=0; i1<KK; i1++) {
for (i2=0; i2<KK; i2++) {
for (i3=0; i3<KK; i3++) {
for (i4=0; i4<KK; i4++) {
  op[i0][i1][i2][i3][i4]=s;
  for (l=0; l<n; l++) videodot(ll,m++,i4);
  };};};};};
videodot(ll,m++,3);

for (ii=1; ii<=nn; ii++) {
  e=0.0; m=0;
  fivbl(np,op);
  videodot(ll+ii,m++,3);
  for (i0=0; i0<KK; i0++) {
  for (i1=0; i1<KK; i1++) {
  for (i2=0; i2<KK; i2++) {
  for (i3=0; i3<KK; i3++) {
  for (i4=0; i4<KK; i4++) {
    n=(int)(f*np[i0][i1][i2][i3][i4]);
    if (n>0) for (l=0; l<n; l++) videodot(ll+ii,m++,i4);
    d=op[i0][i1][i2][i3][i4]-np[i0][i1][i2][i3][i4];
    e+=d<0.0?-d:d;
    op[i0][i1][i2][i3][i4]=np[i0][i1][i2][i3][i4];
    };};};};};
  videodot(ll+ii,m++,3);
  if (e<=0.0001) break;
  if (kbdst()) {kbdin(); break;};
  };
for (i0=0; i0<KK; i0++) {
  b[i0]=0.0;
  for (i1=0; i1<KK; i1++) {
  for (i2=0; i2<KK; i2++) {
  for (i3=0; i3<KK; i3++) {
  for (i4=0; i4<KK; i4++) {
    b[i0]+=op[i0][i1][i2][i3][i4];
    };};};}; };
videocursor(0,BROW+7,0);
printf("(5-block) "); 
for (i0=0; i0<KK; i0++) printf("%2d:%5.3f ",i0,b[i0]);

for (i0=0; i0<KK; i0++) {
for (i1=0; i1<KK; i1++) {
  bb[i0][i1]=0.0;
  for (i2=0; i2<KK; i2++) {
  for (i3=0; i3<KK; i3++) {
  for (i4=0; i4<KK; i4++) {
    bb[i0][i1]+=op[i0][i1][i2][i3][i4];
    };}; };};};

videocursor(0,BROW+8,0);
for (i0=0; i0<KK; i0++) for (i1=0; i1<KK; i1++) 
  printf("%1d%1d:%5.3f ",i0,i1,bb[i0][i1]);
videodot(199-(int)(100.0*bb[1][1]),BORG+(int)(100.0*b[1]),2);
}

/* evaluate the six-block probabilities after one generation */
sixbl(x,a) double x[KK][KK][KK][KK][KK][KK], a[KK][KK][KK][KK][KK][KK]; {
int    i0, i1, i2, i3, i4, i5, i6, i7, i8, i9;
int    j0, j1, j2, j3, j4, j5;
double w, b[KK][KK][KK][KK][KK];

for (j0=0; j0<KK; j0++) {
for (j1=0; j1<KK; j1++) {
for (j2=0; j2<KK; j2++) {
for (j3=0; j3<KK; j3++) {
for (j4=0; j4<KK; j4++) {
  for (j5=0; j5<KK; j5++) x[j0][j1][j2][j3][j4][j5]=0.0;
  b[j0][j1][j2][j3][j4]=0.0;
  for (j5=0; j5<KK; j5++) b[j0][j1][j2][j3][j4]+=a[j0][j1][j2][j3][j4][j5];  
  };};};};};

for (i0=0; i0<KK; i0++) {
for (i1=0; i1<KK; i1++) {
for (i2=0; i2<KK; i2++) {
for (i3=0; i3<KK; i3++) {
for (i4=0; i4<KK; i4++) {
for (i5=0; i5<KK; i5++) {
for (i6=0; i6<KK; i6++) {
for (i7=0; i7<KK; i7++) {
for (i8=0; i8<KK; i8++) {
for (i9=0; i9<KK; i9++) {
  j0=ascrule[i0][i1][i2][i3][i4]-'0';
  j1=ascrule[i1][i2][i3][i4][i5]-'0';
  j2=ascrule[i2][i3][i4][i5][i6]-'0';
  j3=ascrule[i3][i4][i5][i6][i7]-'0';
  j4=ascrule[i4][i5][i6][i7][i8]-'0';
  j5=ascrule[i5][i6][i7][i8][i9]-'0';
  w=a[i0][i1][i2][i3][i4][i5];
  w*=a[i1][i2][i3][i4][i5][i6];
  w*=a[i2][i3][i4][i5][i6][i7];
  w*=a[i3][i4][i5][i6][i7][i8];
  w*=a[i4][i5][i6][i7][i8][i9];
  if (w!=0.0) w/=b[i1][i2][i3][i4][i5]*b[i2][i3][i4][i5][i6]*b[i3][i4][i5][i6][i7]*b[i4][i5][i6][i7][i8];
  x[j0][j1][j2][j3][j4][j5]+=w;
  };};};};};};};};};};
}

/* iterate the 6-block parameters to find self-consistent values */
/* ll - initial line	*/
/* mm - length of line	*/
/* nn - number of lines	*/
sixblfreq(ll,mm,nn) int ll, mm, nn; {
int    ii, i0, i1, i2, i3, i4, i5, l, m, n;
double op[KK][KK][KK][KK][KK][KK];
double np[KK][KK][KK][KK][KK][KK];
double b[KK], bb[KK][KK], d, e, f, s;

m=0;
f=(double)mm;
s=1.0/((double)(KK*KK*KK*KK*KK*KK));
n=(int)(f*s);
videodot(ll,m++,3);
for (i0=0; i0<KK; i0++) {
for (i1=0; i1<KK; i1++) {
for (i2=0; i2<KK; i2++) {
for (i3=0; i3<KK; i3++) {
for (i4=0; i4<KK; i4++) {
for (i5=0; i5<KK; i5++) {
  op[i0][i1][i2][i3][i4][i5]=s;
  for (l=0; l<n; l++) videodot(ll,m++,i5);
  };};};};};};
videodot(ll,m++,3);

for (ii=1; ii<=nn; ii++) {
  e=0.0; m=0;
  sixbl(np,op);
  videodot(ll+ii,m++,3);
  for (i0=0; i0<KK; i0++) {
  for (i1=0; i1<KK; i1++) {
  for (i2=0; i2<KK; i2++) {
  for (i3=0; i3<KK; i3++) {
  for (i4=0; i4<KK; i4++) {
  for (i5=0; i5<KK; i5++) {
    n=(int)(f*np[i0][i1][i2][i3][i4][i5]);
    if (n>0) for (l=0; l<n; l++) videodot(ll+ii,m++,i5);
    d=op[i0][i1][i2][i3][i4][i5]-np[i0][i1][i2][i3][i4][i5];
    e+=d<0.0?-d:d;
    op[i0][i1][i2][i3][i4][i5]=np[i0][i1][i2][i3][i4][i5];
    };};};};};};
  videodot(ll+ii,m++,3);
  if (e<=0.0001) break;
  if (kbdst()) {kbdin(); break;};
  };
for (i0=0; i0<KK; i0++) {
  b[i0]=0.0;
  for (i1=0; i1<KK; i1++) {
  for (i2=0; i2<KK; i2++) {
  for (i3=0; i3<KK; i3++) {
  for (i4=0; i4<KK; i4++) {
  for (i5=0; i5<KK; i5++) {
    b[i0]+=op[i0][i1][i2][i3][i4][i5];
    };};};};}; };
videocursor(0,BROW+7,0);
printf("(6-block) "); 
for (i0=0; i0<KK; i0++) printf("%2d:%5.3f ",i0,b[i0]);

for (i0=0; i0<KK; i0++) {
for (i1=0; i1<KK; i1++) {
  bb[i0][i1]=0.0;
  for (i2=0; i2<KK; i2++) {
  for (i3=0; i3<KK; i3++) {
  for (i4=0; i4<KK; i4++) {
  for (i5=0; i5<KK; i5++) {
    bb[i0][i1]+=op[i0][i1][i2][i3][i4][i5];
    };}; };};};};

videocursor(0,BROW+8,0);
for (i0=0; i0<KK; i0++) for (i1=0; i1<KK; i1++) 
  printf("%1d%1d:%5.3f ",i0,i1,bb[i0][i1]);
videodot(199-(int)(100.0*bb[1][1]),BORG+(int)(100.0*b[1]),2);
}

/* end */
