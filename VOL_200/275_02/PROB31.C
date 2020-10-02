
/* prob31.c						*/
/* program for LCA31 option 't'				*/
/* calculate probabilities related to evolution		*/
/* Harold V. McIntosh, 10 August 1987			*/

/* references:							*/
/*								*/
/* W. John Wilbur, David J. Lipman and Shihab A. Shamma		*/
/* On the prediction of local patterns in cellular automata	*/
/* Physica 19D 397-410 (1986)					*/ 
/*								*/
/* Howard A. Gutowitz, Jonathan D. Victor and Bruce W. Knight	*/
/* Local structure theory for cellular automata			*/
/* Physica 28D 18-48 (1987)					*/

/*	Copyright (C) 1987	*/
/*	Copyright (C) 1988	*/
/*	Harold V. McIntosh	*/
/*	Gerardo Cisneros S.	*/

# define BROW	   13   /* row for bar charts		*/
# define EROW	   22   /* row for evolution synopsis	*/
# define TROW      99   /* column for t-triangle	*/
# define TCOL     205   /* column for t-triangle	*/

/* edit the probability screen	*/
edtri() {char c;
  videomode(COLGRAF);
  videopalette(YELREGR);

  while (0<1) {
  videocursor(0,0,0);
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
    case 'a': nblclr(); asfreq(TROW,TCOL,3); break;
    case 'e': pevolve(); break;
    case 'g': lifreq(50,TROW,TCOL,2); break;
    case 'G': lifreq(200,TROW,TCOL,1); break;
    case 'm': nblclr(); moncar(); break;
    case 's': spdiff(50,6); break;
    case 't': pdiff(100,6); break;
    case '1': nblclr(); oneblfreq(8*BROW,300,48); break;
    case '2': nblclr(); twoblfreq(8*BROW,300,48); break;
    case '3': nblclr(); thrblfreq(8*BROW,300,48); break;
    case '4': nblclr(); foublfreq(8*BROW,300,48); break;
    case '5': nblclr(); fivblfreq(8*BROW,300,48); break;
    case 'z': nblclr(); break;
    case '/': videomode(COLGRAF); videopalette(YELREGR); break;
    case '?': nblclr(); trmenu(); break;
    default: break;
    }; /* end switch */
  };   /* end while  */
  videopalette(WHCYMAG);
  videomode(T80X25);
}      /* end edtri  */

/* show menu */
trmenu() {
  videocursor(0,BROW,0);
  printf("a     - a priori estimates\n");
  printf("m,g,G - sample evolution\n");
  printf("s     - graph probabilities in stereo\n");
  printf("t     - graph 2 block probabilities\n");
  printf("12345 - n-block bar charts\n");
  printf("+-    - change color pallette\n");
  printf("e     - 16 lines evolution\n");
  printf("z/?   - clear panel, screen; show menu\n");
  printf("<cr>  - exit\n");
}

/* show sixteen lines of evolution on screen */
pevolve() {int i, j;
  videoscroll(EROW,0,EROW+1,40,0,0);
  asctobin();
  for (j=8*EROW; j<8*EROW+16; j++) {
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

/* display state frequencies in arr1 as a dot in triangle at (u,v) */
/* n = generations of evolution					   */
/* (u,v) = corner of triangle					   */
/* l = color of dot						   */
lifreq(n,u,v,l) int n, u, v, l; {
int i, ii;
int stat[KK];
double staf[KK], s;

s=1.0/((double)(AL));
asctobin();
for (ii=0; ii<n; ii++) {
  onegen(AL);
  for (i=0; i<KK; i++) stat[i]=0;
  for (i=0; i<AL; i++) (stat[arr1[i]])++;
  for (i=0; i<KK; i++) staf[i]=s*((double)stat[i]);
  videotrdot(u,v,staf[0],staf[1],staf[2],l);
  };
}

/* plot a single point on a triangular grid */
videotrdot(u,v,x,y,z,l) double x, y, z; int u, v, l; {double s;
s=199.0/(x+y+z); x=x*s; y=y*s; z=z*s;
videodot(u-(int)(0.433*z),v+(int)(0.250*(y+y+z)),l);
}

/* plot a stereo point on a triangular grid */
/* (u,v) screen location lower left corner  */
/* (x,y,z) triangular coordinates	    */
/* t       height			    */
/* l       color			    */
videostdot(u,v,x,y,z,t,l) double x, y, z, t; int u, v, l; {double s;
s=199.0/(x+y+z); x=x*s; y=y*s; z=z*s;
videodot(u-(int)(0.433*z),v+(int)(0.250*(y+y+z+t)),l);
}

/* set up a triangular gridwork */
/* (u,v) = corner of triangle	*/
/* n = number of rulings	*/
/* l = color of lines		*/
videotrgrid(u,v,n,l) int u, v, n, l; {
int i, j;
double a, b, c;
if(n==0) return;
for (i=0; i<=n;  i++) {
for (j=0; i+j<n; j++) {
  a=((double)(i)/(double)(n));
  b=((double)(j)/(double)(n));
  c=1.0-a-b;
  videotrdot(u,v,a,b,c,l);
  };};
}

/* "Monte Carlo" determination of probabilities */
moncar() {
int i, b[KK];
double bf[KK];
  asctobin();
  onegen(AL);
  for (i=0; i<KK; i++) b[i]=0;
  for (i=0; i<AL; i++) b[arr1[i]]++;
  for (i=0; i<KK; i++) bf[i]=((double)(b[i]))/((double)(AL));
  videocursor(0,BROW+8,0);
  printf("(montecarlo) "); 
  for (i=0; i<KK; i++) printf("%2d:%5.3f",i,bf[i]);
}

/* Generate coefficients of the Bernstein Polynomial */
berncoef() {int i, i0, i1, i2;
for ( i=0;  i<KK;  i++) {
for (i0=0; i0<KK; i0++) {
for (i1=0; i1<KK; i1++) {
for (i2=0; i2<KK; i2++) {
  bp[i][i0][i1][i2]=0.0;
  };};};};
for (i0=0; i0<KK; i0++) {
for (i1=0; i1<KK; i1++) {
for (i2=0; i2<KK; i2++) {
  bp[ascrule[i0][i1][i2]-'0'][i0][i1][i2]+=1.0;
  };};};
}

/* evaluate the nth generation Bernstein polynomial at point p */
double bern(i,x,y,z) int i; double x, y, z; {double s;
s=x*x*x*bp[i][0][0][0]+y*y*y*bp[i][1][1][1]+z*z*z*bp[i][2][2][2];
s+=x*x*y*(bp[i][0][0][1]+bp[i][0][1][0]+bp[i][1][0][0]);
s+=x*x*z*(bp[i][0][0][2]+bp[i][0][2][0]+bp[i][2][0][0]);
s+=x*y*y*(bp[i][0][1][1]+bp[i][1][0][1]+bp[i][1][1][0]);
s+=x*z*z*(bp[i][0][2][2]+bp[i][2][0][2]+bp[i][2][2][0]);
s+=y*y*z*(bp[i][1][1][2]+bp[i][1][2][1]+bp[i][2][1][1]);
s+=y*z*z*(bp[i][1][2][2]+bp[i][2][1][2]+bp[i][2][2][1]);
s+=x*y*z*(bp[i][0][1][2]+bp[i][0][2][1]+bp[i][1][0][2]+
	  bp[i][1][2][0]+bp[i][2][0][1]+bp[i][2][1][0]);
return s;
}

/* graph the probability differential for state l over a triangle */
pdiff(n,l) int n, l; {int i, j; double a, b, c, en, t, sqsu();
if (n==0) return;
en=1.0/((double)(n));
berncoef();
for (i=0; i<=n; i++) {
for (j=0; i+j<=n; j++) {
  a=((double)(i))*en;
  b=((double)(j))*en;
  c=1.0-a-b;
  t=sqsu(a,b,c);
  videotrdot(99,205,a,b,c,sqco(t,i));
  if (kbdst()) {kbdin(); return;};
  };};
}

/* stereo graph of probability differential for state l over a triangle */
spdiff(n,l) int n, l; {int i, j; double a, b, c, en, t, sqsu();
if (n==0) return;
en=1.0/((double)(n));
berncoef();
for (i=0; i<=n; i++) {
for (j=0; i+j<=n; j++) {
  a=((double)(i))*en;
  b=((double)(j))*en;
  c=1.0-a-b;
  t=sqsu(a,b,c);
  videostdot(99,5,a,b,c,-12.0*t,sqco(t,i));
  videostdot(99,105,a,b,c,12.0*t,sqco(t,i));
  if (kbdst()) {kbdin(); return;};
  };};
}

/* calculate the squaresum of the three polynomials at a point */
double sqsu(x,y,z) double x, y, z; {double u, v, w, bern();
u=bern(0,x,y,z)-x;
v=bern(1,x,y,z)-y;
w=bern(2,x,y,z)-z;
return (u*u+v*v+w*w);
}

/* generate squaresum compatible contour values */
int sqco(t,i) int i; double t; {int l;
if (t<0.0075) l=0; else {
if (t<0.0750) {if (i%2) l=0; else l=3;} else {
if (t<0.1250) l=3; else {
if (t<0.2500) {if (i%2) l=3; else l=2;} else {
if (t<0.5000) l=2; else {
if (t<0.7500) {if (i%2) l=2; else l=1;} else {
if (t<1.0000) l=1; else {
	      if (i%2) l=1; else l=0;
   }}}}}}}
return l;
}

/* graph the frequencies of ascrule in color l */
/* put dot at coordinates (u,v) */
asfreq(u,v,l) int u, v, l; {
int    i, j, i0, i1, i2;
int    stat[KK], stal, stac, star;		/* statistic counts */
double staf[KK], pp;

pp=1.0/((double)(KK*KK*KK));
stal=0; stac=0; star=0;
for (i=0; i<KK; i++) stat[i]=0;
for (i0=0; i0<KK; i0++) {
for (i1=0; i1<KK; i1++) {
for (i2=0; i2<KK; i2++) {
  j=ascrule[i0][i1][i2]-'0';
  stat[j]++;
  if(j==i0) star++;
  if(j==i1) stac++;
  if(j==i2) stal++;
  };};};
videocursor(0,BROW,0);
for (i=0; i<KK; i++) printf("%1d    - %4.2f\n",i,((double)stat[i])*pp);
printf("\n");
printf("left  - %4.2f\n",((double)stal)*pp);
printf("still - %4.2f\n",((double)stac)*pp);
printf("right - %4.2f\n",((double)star)*pp);
for (i=0; i<KK; i++) staf[i]=((double)stat[i])*pp;
videotrdot(u,v,staf[0],staf[1],staf[2],l);
}

/* evaluate the one-block probabilities after one generation */
onebl(x,a) double x[KK], a[KK]; {int i0, i1, i2, j0;

for (j0=0; j0<KK; j0++) x[j0]=0.0;

for (i0=0; i0<KK; i0++) {
for (i1=0; i1<KK; i1++) {
for (i2=0; i2<KK; i2++) {
  j0=ascrule[i0][i1][i2]-'0';
  x[j0]+=a[i0]*a[i1]*a[i2];
  };};};
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
videocursor(0,BROW+8,0);
printf("(1-block) "); 
for (i=0; i<KK; i++) printf("%2d:%5.3f ",i,op[i]);
videotrdot(TROW,TCOL,op[0],op[1],op[2],l);
}

/* evaluate the two-block probabilities after one generation */
twobl(x,a) double x[KK][KK], a[KK][KK]; {
int    i0, i1, i2, i3;
int    j0, j1;
double w, b[KK];

for (j0=0; j0<KK; j0++) {for (j1=0; j1<KK; j1++) x[j0][j1]=0.0;};

for (j0=0; j0<KK; j0++) {b[j0]=0.0; for (j1=0; j1<KK; j1++) b[j0]+=a[j0][j1];};

for (i0=0; i0<KK; i0++) {
for (i1=0; i1<KK; i1++) {
for (i2=0; i2<KK; i2++) {
for (i3=0; i3<KK; i3++) {
  j0=ascrule[i0][i1][i2]-'0';
  j1=ascrule[i1][i2][i3]-'0';
  w=a[i0][i1]*a[i1][i2]*a[i2][i3];
  if (w!=0.0) w/=b[i1]*b[i2];
  x[j0][j1]+=w;
  };};};};
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
videotrdot(TROW,TCOL,b[0],b[1],b[2],1);
  if (e<=0.0001) break;
  if (kbdst()) {kbdin(); break;};
  };
videocursor(0,BROW+8,0);
printf("(2-block) "); 
for (i=0; i<KK; i++) printf("%2d:%5.3f ",i,b[i]);
videotrdot(TROW,TCOL,b[0],b[1],b[2],3);
}

/* evaluate the three-block probabilities after one generation */
thrbl(x,a) double x[KK][KK][KK], a[KK][KK][KK]; {
int    i0, i1, i2, i3, i4;
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
  j0=ascrule[i0][i1][i2]-'0';
  j1=ascrule[i1][i2][i3]-'0';
  j2=ascrule[i2][i3][i4]-'0';
  w=a[i0][i1][i2]*a[i1][i2][i3]*a[i2][i3][i4];
  if (w!=0.0) w/=b[i1][i2]*b[i2][i3];
  x[j0][j1][j2]+=w;
  };};};};};
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
videocursor(0,BROW+8,0);
printf("(3-block) "); 
for (i=0; i<KK; i++) printf("%2d:%5.3f ",i,b[i]);
videotrdot(TROW,TCOL,op[0],op[1],op[2],l);

for (i=0; i<KK; i++) {
for (j=0; j<KK; j++) {
  bb[i][j]=0.0;
  for (k=0; k<KK; k++) {
    bb[i][j]+=op[i][j][k];
    };}; };

/*videocursor(0,BROW+9,0);*/
/*for (i=0; i<KK; i++) for (j=0; j<KK; j++)*/ 
/*  printf("%1d%1d:%5.3f ",i,j,bb[i][j]);*/
/*videodot(199-(int)(100.0*bb[1][1]),BORG+(int)(100.0*b[1]),2);*/
}

/* evaluate the four-block probabilities after one generation */
foubl(x,a) double x[KK][KK][KK][KK], a[KK][KK][KK][KK]; {
int    i0, i1, i2, i3, i4, i5;
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
  j0=ascrule[i0][i1][i2]-'0';
  j1=ascrule[i1][i2][i3]-'0';
  j2=ascrule[i2][i3][i4]-'0';
  j3=ascrule[i3][i4][i5]-'0';
  w=a[i0][i1][i2][i3]*a[i1][i2][i3][i4]*a[i2][i3][i4][i5];
  if (w!=0.0) w/=b[i1][i2][i3]*b[i2][i3][i4];
  x[j0][j1][j2][j3]+=w;
  };};};};};};
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

videocursor(0,BROW+8,0);
printf("(4-block) "); 
for (i0=0; i0<KK; i0++) printf("%2d:%5.3f ",i0,b[i0]);
videotrdot(TROW,TCOL,op[0],op[1],op[2],l);

for (i0=0; i0<KK; i0++) {
for (i1=0; i1<KK; i1++) {
  bb[i0][i1]=0.0;
  for (i2=0; i2<KK; i2++) {
  for (i3=0; i3<KK; i3++) {
    bb[i0][i1]+=op[i0][i1][i2][i3];
    };}; };};

/*videocursor(0,BROW+9,0);*/
/*for (i0=0; i0<KK; i0++) for (i1=0; i1<KK; i1++)*/ 
/*  printf("%1d%1d:%5.3f ",i0,i1,bb[i0][i1]);*/
/*videodot(199-(int)(100.0*bb[1][1]),BORG+(int)(100.0*b[1]),2);*/
}

/* evaluate the five-block probabilities after one generation */
fivbl(x,a) double x[KK][KK][KK][KK][KK], a[KK][KK][KK][KK][KK]; {
int    i0, i1, i2, i3, i4, i5, i6;
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
  j0=ascrule[i0][i1][i2]-'0';
  j1=ascrule[i1][i2][i3]-'0';
  j2=ascrule[i2][i3][i4]-'0';
  j3=ascrule[i3][i4][i5]-'0';
  j4=ascrule[i4][i5][i6]-'0';
  w=a[i0][i1][i2][i3][i4]*a[i1][i2][i3][i4][i5]*a[i2][i3][i4][i5][i6];
  if (w!=0.0) w/=b[i1][i2][i3][i4]*b[i2][i3][i4][i5];
  x[j0][j1][j2][j3][j4]+=w;
  };};};};};};};
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
videocursor(0,BROW+8,0);
printf("(5-block) "); 
for (i0=0; i0<KK; i0++) printf("%2d:%5.3f ",i0,b[i0]);
videotrdot(TROW,TCOL,op[0],op[1],op[2],l);

for (i0=0; i0<KK; i0++) {
for (i1=0; i1<KK; i1++) {
  bb[i0][i1]=0.0;
  for (i2=0; i2<KK; i2++) {
  for (i3=0; i3<KK; i3++) {
  for (i4=0; i4<KK; i4++) {
    bb[i0][i1]+=op[i0][i1][i2][i3][i4];
    };}; };};};

/*videocursor(0,BROW+9,0);*/
/*for (i0=0; i0<KK; i0++) for (i1=0; i1<KK; i1++) */
/*  printf("%1d%1d:%5.3f ",i0,i1,bb[i0][i1]);*/
/*videodot(199-(int)(100.0*bb[1][1]),BORG+(int)(100.0*b[1]),2);*/
}

/* end */
