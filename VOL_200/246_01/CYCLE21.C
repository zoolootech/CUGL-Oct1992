

/* CYCLE21.C					 */
/* program to analyze the cycles of a cellular   */
/* automaton and report all the periodic states. */
/* [Harold V. McIntosh, 21 May 1987]		 */

# include <stdio.h>

# define KK	 2			/* number of states/cell	*/
# define NS      4			/* number of distinct sums	*/
# define NW	24			/* pause after so many lines	*/

int  arr1[16], arr2[16];
unsigned int arry[16384];
int  binrule[KK][KK][KK], rule[NS];
int  cy, cp, mc, nc, nl;

main() {
int  i, i0, i1, i2;

printf("  t(totalistic) w(Wolfram) b(binary) \015");
switch (kbdin()) {
  case 't':
    printf("\n\nRule number:\n\n");
    printf("0..1\n");
    for (i=0; i<NS; i++) rule[i]=kbdin()-'0';
    printf("\n");
    totobin();
    break;
  case 'w':					/* Wolfram rule # */
    printf("\n\n");
    i=numin(0);
    for (i0=0; i0<KK; i0++) {
    for (i1=0; i1<KK; i1++) {
    for (i2=0; i2<KK; i2++) {
      binrule[i0][i1][i2]=i%2; i/=2;
      };};};
    break;
  case 'b':
    printf("\n\n");
    printf("00001111\n");
    printf("00110011\n");
    printf("01010101\n");
    i0=0; i1=0; i2=0;
    for (i=0; i<8; i++) {
    binrule[i0][i1][i2]=kbdin()-'0';
    i2++;
    if (i2>KK-1) {i2=0; i1++;};
    if (i1>KK-1) {i1=0; i0++;};
    if (i0>KK-1) {i0=0;};
    };
    printf("\n\n");
    break;
  }

nc=0;
nl=0;

printf("Cycles of length 4"); kwait(0);
 mc=7;
 cy=4;
 cp=16;
 pass1();
 pass2();
 pass4();

printf("Cycles of length 5"); kwait(0);
 mc=6;
 cy=5;
 cp=32;
 pass1();
 pass2();
 pass4();

printf("Cycles of length 6"); kwait(0);
 mc=5;
 cy=6;
 cp=64;
 pass1();
 pass2();
 pass4();

printf("Cycles of length 7"); kwait(0);
 mc=4;
 cy=7;
 cp=128;
 pass1();
 pass2();
 pass4();

printf("Cycles of length 8"); kwait(0);
 mc=4;
 cy=8;
 cp=256;
 pass1();
 pass2();
 pass4();

printf("Cycles of length 9"); kwait(0);
 mc=3;
 cy=9;
 cp=512;
 pass1();
 pass2();
 pass4();

printf("Cycles of length 10"); kwait(0);
 mc=3;
 cy=10;
 cp=1024;
 pass1();
 pass2();
 pass4();

printf("Cycles of length 11"); kwait(0);
 mc=3;
 cy=11;
 cp=2048;
 pass1();
 pass2();
 pass4();

printf("Cycles of length 12"); kwait(0);
 mc=2;
 cy=12;
 cp=4096;
 pass1();
 pass2();
 pass4();

printf("Cycles of length 13"); kwait(0);
 mc=2;
 cy=13;
 cp=8192;
 pass1();
 pass2();
 pass4();

printf("Cycles of length 14"); kwait(0);
 mc=2;
 cy=14;
 cp=16384;
 pass1();
 pass2();
 pass4();

} /* end main */

/* Pass 1 makes arry[i] equal to the successor of i */

pass1() {int i, j, x;
  printf(" Pass1\015");
  for (j=0; j<cp; j++) {
    x=j; for (i=0; i<cy; i++) {arr1[cy-i-1]=x%KK; x/=KK;};
    evolve(cy);
    x=0; for (i=0; i<cy; i++) x=KK*x+arr2[i]; arry[j]=x;
  }; }

/* calculate one generation of evolution in a ring of length n */

evolve(n) int n; {
int i;
  arr2[0]=binrule[arr1[n-1]][arr1[0]][arr1[1]];
  for (i=1; i<n-1; i++) arr2[i]=binrule[arr1[i-1]][arr1[i]][arr1[i+1]];
  arr2[n-1]=binrule[arr1[n-2]][arr1[n-1]][arr1[0]];
}

/* Pass 2 flags all links with an incoming arrow */

pass2() {int j, m, x;
printf(" Pass2\015");
do {
m=0;
for (j=0; j<cp; j++) arry[j]|=0x8000;
for (j=0; j<cp; j++) {x=arry[j];
 if (x!=0xFFFF) arry[x&0x7FFF]&=0x7FFF;};
for (j=0; j<cp; j++) {
 if ((arry[j]>0x7FFF) && (arry[j]!=0xFFFF)) {arry[j]=0xFFFF; m=1;};};
 } while (m!=0);
}

/* pass4 - print loops which remain */

pass4() {
int j, x, y, m;
printf(" pass4 \015");
for (j=0; j<cp; j++) {
 x=j; y=arry[j]; arry[j]=0xFFFF; m=0;
 while (y!=0xFFFF) {prf(x,y); x=y; y=arry[x]; arry[x]=0xFFFF; m=1;};
 if (m==1) kwait(0);
 };
}

/* change totalistic rule to general rule */

totobin() {
int i0, i1, i2;
for (i0=0; i0<KK; i0++) {
for (i1=0; i1<KK; i1++) {
for (i2=0; i2<KK; i2++) {
binrule[i0][i1][i2]=rule[i0+i1+i2];
};};};
}

/* print the link */

prf(j,x) int j, x; {int i, y;
  kwait(1);
  y=j; for (i=0; i<cy; i++) {arr1[i]=y%KK; y/=KK;};
  for (i=0; i<cy; i++) printf("%1d",arr1[i]);
  printf("-");
  y=x; for (i=0; i<cy; i++) {arr1[i]=y%KK; y/=KK;};
  for (i=0; i<cy; i++) printf("%1d",arr1[i]);
  printf("  ");
}

/* print arry - for diagnostic purposes */
pall() {int j;
for (j=0; j<cp; j++) printf("%4d",arry[j]);
}

/* print cycle - for diagnostic purposes */
pcy() {int j;
for (j=0; j<cy; j++) printf("%1d",arr2[j]);
}

/* limit j to interval (i,k) */
int lim(i,j,k) int i, j, k;
    {if (i>=j) return i; if (k<=j) return k; return j;}

/* read number from keyboard */
int numin(n) int n; {char c;
  c=kbdin();
  if (c>='0'&&c<='9') return(numin(10*n+(c-'0'))); else return(n);
}

/* keyboard status */
kbdst() {return bdos(11)&0xFF;}

/* direct keyboard input, no echo */
kbdin() {int c;
if ((c=bdos(7)&0xFF)=='\0') c=(bdos(7)&0xFF)|0x80;
videoputc(c,1);
return c;}

/* pause at the end of a full screen */
/* kwait(0) - <cr><lf> and count it  */
/* kwait(1) - count columns          */

kwait(i) int i; {
switch (i) {
  case 0: printf("\n"); nc=0; nl++; break;
  case 1: if (nc==mc) {printf("&\n"); nc=1; nl++;} else nc++; break;
  default: break;};
if (nl==NW) {
  nl=0;
  printf(" press any key to continue\015");
  while (kbdst()) {};
  kbdin();
  printf("-                         \n");
  };
}

/* end */
