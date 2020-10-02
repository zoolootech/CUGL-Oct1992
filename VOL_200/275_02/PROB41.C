
/* prob41.c						*/
/* program for lcau41 option 't'			*/
/* calculate probabilities related to evolution		*/
/* Harold V. McIntosh, 10 August 1987			*/
/* 4 September 1987 - tetrahedral stereopair		*/
/* 30 December 1987 - contoured stereopair layers	*/

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

# define BROW	   14   /* row for bar charts		*/
# define EROW	   23   /* row for evolution synopsis	*/

/* edit the probability screen	*/

edtri() {char c; int i;
  videomode(COLGRAF);
  videopalette(YELREGR);

  videocursor(0,1,33);
  videoputc('2',3);
  videocursor(0,13,23);
  videoputc('0',3);
  videocursor(0,13,39);
  videoputc('1',3);

  tbase();
  edges(50,3);

  while (0<1) {
  videocursor(0,0,0);
  hscrrul();
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
    case 'B': tbase(); break;
    case 'E': tedge(); break;
    case 'a': nblclr(); asfreq(3); break;
    case 'e': pevolve(); break;
    case 'g': for (i=0, asctobin(); i<50; i++, onegen(AL)) lifreq(2); break;
    case 'G': for (i=0, asctobin(); i<200; i++, onegen(AL)) lifreq(1); break;
    case 'h': edges(80,3); break;
    case 'k': videohegrid(10,0); break;
    case 'm': nblclr(); moncar(); break;
    case 't': hpdiff(12,8); break;
    case 'p': elayer(40,0); break;
    case 'q': elayer(40,1); break;
    case 'r': elayer(40,2); break;
    case 's': elayer(40,3); break;
    case 'u': elayer(72,8); break;
    case 'v': olayer(72,8); break;
    case 'U': elayer(42,8); break;
    case 'V': olayer(42,8); break;
    case '1': nblclr(); oneblfreq(8*BROW,300,56); break;
    case '2': nblclr(); twoblfreq(8*BROW,300,56); break;
    case '3': nblclr(); thrblfreq(8*BROW,300,56); break;
    case '4': nblclr(); foublfreq(8*BROW,300,56); break;
    case '5': nblclr(); fivblfreq(8*BROW,300,56); break;
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
  printf("pqrs  - prob of states\n");
  printf("t,u,v - sq, odd-even\n");
  printf("12345 - n-block bar charts\n");
  printf("+-    - change color quad\n");
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

/* locations of the weighted masses */
/* for tetrahedron sitting on base  */
tbase() {
  wmul[0]=160.0; wmvl[0]=100.0;
  wmul[1]=310.0; wmvl[1]=100.0;
  wmul[2]=240.0; wmvl[2]=195.0;
  wmul[3]=250.0; wmvl[3]=157.0;

  wmur[0]= 10.0; wmvr[0]=100.0;
  wmur[1]=160.0; wmvr[1]=100.0;
  wmur[2]= 90.0; wmvr[2]=195.0;
  wmur[3]= 80.0; wmvr[3]=157.0;
}

/* locations of the weighted masses */
/* for tetrahedron standing on edge */
tedge() {
  wmul[0]=160.0; wmvl[0]=157.0;
  wmul[1]=310.0; wmvl[1]=157.0;
  wmul[2]=240.0; wmvl[2]=195.0;
  wmul[3]=250.0; wmvl[3]=100.0;

  wmur[0]= 10.0; wmvr[0]=157.0;
  wmur[1]=160.0; wmvr[1]=157.0;
  wmur[2]= 90.0; wmvr[2]=195.0;
  wmur[3]= 80.0; wmvr[3]=100.0;
}

/* plot a single point on a triangular grid */
videotrdot(u,v,x,y,z,l) double x, y, z; int u, v, l; {double s;
s=199.0/(x+y+z); x=x*s; y=y*s; z=z*s;
videodot(u-(int)(0.433*z),v+(int)(0.250*(y+y+z)),l);
}

/* plot a single point on a tetrahedral grid */
videohedot(a,b,p,l) double p[KK]; int a, b, l; {double s, u, v, w;
s=199.0/(p[0]+p[1]+p[2]+p[3]);
u=(p[1]+p[3])*s; v=p[2]*s; w=(p[1]-p[3])*s;
videodot(a-(int)(0.433*p[1]*s),b-50+(int)(0.250*(u+u+v)),l);
videodot(a-(int)(0.433*p[1]*s),b+50+(int)(0.250*(w+w+v)),l);
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
for (j=0; i+j<=n; j++) {
  a=((double)(i)/(double)(n));
  b=((double)(j)/(double)(n));
  c=1.0-a-b;
  videotrdot(u,v,a,b,c,l);
  };};
}

/* display state frequencies in arr1 as a dot in Bezier diagram */
lifreq(l) int l; {int i, stat[KK]; double staf[KK], s;
s=1.0/((double)(AL));
for (i=0; i<KK; i++) stat[i]=0;
for (i=0; i<AL; i++) (stat[arr1[i]])++;
for (i=0; i<KK; i++) staf[i]=s*((double)(stat[i]));
videomassdot(staf,l);
}

/* plot a single point as a weighted mass center */
videomassdot(x,l) double *x; int l; {double ul, vl, ur, vr; int i;
if (l>=KK) return;
for (i=0, ul=0.0; i<KK; i++) ul+=x[i]*wmul[i];
for (i=0, vl=0.0; i<KK; i++) vl+=x[i]*wmvl[i];
for (i=0, ur=0.0; i<KK; i++) ur+=x[i]*wmur[i];
for (i=0, vr=0.0; i<KK; i++) vr+=x[i]*wmvr[i];
videodot(199-(int)vl,(int)ul,l);
videodot(199-(int)vr,(int)ur,l);
}

/* plot a point in 4 triangles - face projections of a tetrahedron */
video4dots(x,l) int l; double x[KK]; {double u[KK-1];
u[0]=x[0]; u[1]=x[1]; u[2]=x[2]; videotrdot(99,200,u,l);
u[0]=x[1]; u[1]=x[2]; u[2]=x[3]; videotrdot(199,0,u,l);
u[0]=x[2]; u[1]=x[3]; u[2]=x[0]; videotrdot(149,100,u,l);
u[0]=x[3]; u[1]=x[0]; u[2]=x[1]; videotrdot(199,200,u,l);
}

/* set up a triangular gridwork of edge n in color l */
videohegrid(n,l) int n, l; {
int i0, i1, i2;
double x[KK], nn;

if (n==0) return;
nn=1.0/((double)(n));
for (i0=0, x[0]=0.0; i0<=n; i0++, x[0]+=nn) {
for (i1=0, x[1]=0.0; i0+i1<=n; i1++, x[1]+=nn) {
for (i2=0, x[2]=0.0; i0+i1+i2<=n; i2++, x[2]+=nn) {
  x[3]=1.0-x[0]-x[1]-x[2];
  videomassdot(x,l);
  };};};
}

/* show the skeleton framework of the tetrahedron */
/* n = points per edge				  */
/* l = color of edge				  */
edges(n,l) int n, l; {int i; double x[KK], nn;
if (n==0) return;
nn=1.0/((double)(n));
for (i=0, uvec(x,0); i<=n; i++, x[0]-=nn, x[1]+=nn) videomassdot(x,l);
for (i=0, uvec(x,0); i<=n; i++, x[0]-=nn, x[2]+=nn) videomassdot(x,l);
for (i=0, uvec(x,0); i<=n; i++, x[0]-=nn, x[3]+=nn) videomassdot(x,l);
for (i=0, uvec(x,1); i<=n; i++, x[1]-=nn, x[2]+=nn) videomassdot(x,l);
for (i=0, uvec(x,1); i<=n; i++, x[1]-=nn, x[3]+=nn) videomassdot(x,l);
for (i=0, uvec(x,2); i<=n; i++, x[2]-=nn, x[3]+=nn) videomassdot(x,l);
}

/* unit vector */
uvec(x,i) double *x; int i; {int j;
for (j=0; j<KK; j++) x[j]=0.0;
x[i]=1.0;
}

/* homogeneous vector */
hvec(x) double *x; {int i; double s;
s=1.0/((double)(KK));
for (i=0; i<KK; i++) x[i]=s;
}

/* layered contour map in stereoscopic tetrahedron */
/* n = length of edge  */
/* l = function option */
elayer(n,l) int n, l; {int m, nn;
  m=n/4; nn=4*m;
  pdiff(3*m,nn,l);
  pdiff(2*m,nn,l);
  pdiff(1*m,nn,l);
  pdiff(0*m,nn,l);
  edges(nn,3);
}

/* layered contour map in stereoscopic tetrahedron */
/* n = length of edge  */
/* l = function option */
olayer(n,l) int n, l; {int m, nn;
  m=n/8; nn=8*m;
  pdiff(7*m,nn,l);
  pdiff(5*m,nn,l);
  pdiff(3*m,nn,l);
  pdiff(1*m,nn,l);
  edges(nn,3);
}

/* "Monte Carlo" determination of probabilities */
moncar() {
int    i, b[KK];
double bf[KK];
  asctobin();
  onegen(AL);
  for (i=0; i<KK; i++) b[i]=0;
  for (i=0; i<AL; i++) b[arr1[i]]++;
  for (i=0; i<KK; i++) bf[i]=((double)(b[i]))/((double)(AL));
  videocursor(0,BROW+8,0);
  printf("(m-c) "); 
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
double bern(i,p) int i; double p[KK]; {int i0, i1, i2; double s;
s=0.0;
for (i0=0; i0<KK; i0++) {
for (i1=0; i1<KK; i1++) {
for (i2=0; i2<KK; i2++) {
  s+=p[i0]*p[i1]*p[i2]*bp[i][i0][i1][i2];
  };};};
return s;
}

/* graph the probability differential for state l over a triangle */
pdiff(i0,n,l) int i0, n, l; {
int    i1, i2;
double p[KK], en, bern(), sqsu();
if (n==0) return;
if (i0>n) return;
en=1.0/((double)(n));
p[0]=((double)(i0))*en;
berncoef();
for (i1=0, p[1]=0.0; i0+i1<=n; i1++, p[1]+=en) {
for (i2=0, p[2]=0.0; i0+i1+i2<=n; i2++, p[2]+=en) {
  p[3]=1.0-p[0]-p[1]-p[2];
  switch(l) {
    case 0: videomassdot(p,psign(bern(0,p)-p[0])); break;
    case 1: videomassdot(p,psign(bern(1,p)-p[1])); break;
    case 2: videomassdot(p,psign(bern(2,p)-p[2])); break;
    case 3: videomassdot(p,psign(bern(3,p)-p[3])); break;
    case 4: videomassdot(p,lico(bern(0,p),i1+i2)); break;
    case 5: videomassdot(p,lico(bern(1,p),i2)); break;
    case 6: videomassdot(p,lico(bern(2,p),i1)); break;
    case 7: videomassdot(p,lico(bern(3,p),i1)); break;
    case 8: videomassdot(p,sqco(sqsu(p),i1+i2)); break;
    default: break;
    };
  if (kbdst()) {kbdin(); return;};
  };};
}

/* graph the probability differential for state l over a tetrahedron	*/
/* n = number of points per edge					*/
/* l = color of dot							*/

hpdiff(n,l) int n, l; {
int    i, j, k;
double bern(), x[KK], nn;

if (n==0) return;
nn=1.0/((double)(n));
berncoef();
for (i=0, x[0]=0.0; i<=n; i++, x[0]+=nn) {
for (j=0, x[1]=0.0; i+j<=n; j++, x[1]+=nn) {
for (k=0, x[2]=0.0; i+j+k<=n; k++, x[2]+=nn) {
  x[3]=1.0-x[0]-x[1]-x[2];
  switch(l) {
    case 0: videomassdot(x,psign(bern(0,x)-x[0])); break;
    case 1: videomassdot(x,psign(bern(1,x)-x[1])); break;
    case 2: videomassdot(x,psign(bern(2,x)-x[2])); break;
    case 3: videomassdot(x,psign(bern(3,x)-x[3])); break;
    case 4: videomassdot(x,lico(bern(0,x),i+j)); break;
    case 5: videomassdot(x,lico(bern(1,x),j)); break;
    case 6: videomassdot(x,lico(bern(2,x),i)); break;
    case 7: videomassdot(x,lico(bern(3,x),i)); break;
    case 8: videomassdot(x,sqco(sqsu(x),i+j)); break;
    default: break;
    };
  if (kbdst()) {kbdin(); return;};
  };};};
}

/* choose a color for the contour level based on sign: red, yellow, green */
psign(x) double x; {if (x>0.01) return 1; if (x<-0.01) return 2; return 3;}

/* calculate the squaresum of the four polynomials at a point */
double sqsu(p) double p[KK]; {int i; double s, t, bern();
for (i=0, s=0.0; i<KK; i++) {t=bern(i,p)-p[i]; s+=t*t;};
return s;
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

/* generate linear contour values */
int lico (t,i) int i; double t; {int l;
if (t<0.1) l=0; else {
if (t<0.2) {if (i%2) l=0; else l=3;} else {
if (t<0.3) l=3; else {
if (t<0.4) {if (i%2) l=3; else l=2;} else {
if (t<0.5) l=2; else {
if (t<0.6) {if (i%2) l=2; else l=1;} else {
if (t<0.7) l=1; else {
	      if (i%2) l=1; else l=0;
   }}}}}}}
return l;
}

/* graph the frequencies of ascrule in color l */
asfreq(l) int l; {
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
for (i=0; i<KK; i++) printf("%2d    - %5.2f\n",i,((double)stat[i])*pp);
printf("\n");
printf("left  - %5.2f\n",((double)stal)*pp);
printf("still - %5.2f\n",((double)stac)*pp);
printf("right - %5.2f\n",((double)star)*pp);
for (i=0; i<KK; i++) staf[i]=((double)stat[i])*pp;
videomassdot(staf,l);
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
videodot(ll,m++,3);
videomassdot(op,2);

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
  if (op[2]<=0.001) break;
  if (op[3]<=0.001) break;
  if (e<=0.0000001) break;
  if (kbdst()) {kbdin(); break;};
  videomassdot(op,1);
  };
videocursor(0,BROW+8,0);
printf("(1-blk)"); 
for (i=0; i<KK; i++) printf("%2d:%5.3f",i,op[i]);
videomassdot(op,3);
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
hvec(b); videomassdot(b,2);

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
videomassdot(b,1);
  if (e<=0.0001) break;
  if (kbdst()) {kbdin(); break;};
  };
videocursor(0,BROW+8,0);
printf("(2-blk)"); 
for (i=0; i<KK; i++) printf("%2d:%5.3f",i,b[i]);
videomassdot(b,3);
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
hvec(b); videomassdot(b,2);

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

  for (i=0; i<KK; i++) {
    b[i]=0.0;
    for (j=0; j<KK; j++) {
    for (k=0; k<KK; k++) {
      b[i]+=op[i][j][k];
      };}; };
  videomassdot(b,1);
  if (e<=0.0001) break;
  if (kbdst()) {kbdin(); break;};
  };

videocursor(0,BROW+8,0);
printf("(3-blk)"); 
for (i=0; i<KK; i++) printf("%2d:%5.3f",i,b[i]);
videomassdot(b,3);

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
hvec(b); videomassdot(b,2);

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

  for (i0=0; i0<KK; i0++) {
    b[i0]=0.0;
    for (i1=0; i1<KK; i1++) {
    for (i2=0; i2<KK; i2++) {
    for (i3=0; i3<KK; i3++) {
      b[i0]+=op[i0][i1][i2][i3];
      };};}; };
  videomassdot(b,1);
  if (e<=0.0001) break;
  if (kbdst()) {kbdin(); break;};
  };

videocursor(0,BROW+8,0);
printf("(4-blk)"); 
for (i0=0; i0<KK; i0++) printf("%2d:%5.3f",i0,b[i0]);
videomassdot(b,3);

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
hvec(b); videomassdot(b,2);

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
  for (i0=0; i0<KK; i0++) {
    b[i0]=0.0;
    for (i1=0; i1<KK; i1++) {
    for (i2=0; i2<KK; i2++) {
    for (i3=0; i3<KK; i3++) {
    for (i4=0; i4<KK; i4++) {
    b[i0]+=op[i0][i1][i2][i3][i4];
    };};};}; };
  videomassdot(b,1);
  if (e<=0.0001) break;
  if (kbdst()) {kbdin(); break;};
  };
videocursor(0,BROW+8,0);
printf("(5-blk)"); 
for (i0=0; i0<KK; i0++) printf("%2d:%5.3f",i0,b[i0]);
videomassdot(b,3);

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
