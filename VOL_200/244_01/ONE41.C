
/* program to analyze the de Bruijn diagram of a cellular */
/* automaton and report all the periodic states. */
/* states with period 1 and displacements zero (1,0) or one (1,1) */
/* are analyzed for a four-state, nearest neighbor (4,1) automaton */
/* [Harold V. McIntosh, 4 May 1987] */

# include <stdio.h>

# define MC	 9				/* maximum number of columns */
# define NS     10				/* number of distinct sums */
# define NW	24				/* pause after so many lines */

char arry[4][4][4];
int  rule[NS];
int  nc, nl;

main() {char c; int i;

printf("Rule number:\n\n");
printf("0..1..2..3\n");
for (i=0; i<NS; i++) rule[i]=getchar()-'0';

nc=0;
nl=0;

do {
printf("  a=still  b=glider  0,1,2,3=constant  q=quit\015");
c=kbdin();
switch (c) {
case 'a':
printf("Sequences satisfying the condition (1,0):       "); kwait(0);
pass1a(); pass2i(); pass2o(); pass4(); break;
case 'b':
printf("Sequences satisfying the condition (1,1):       "); kwait(0);
pass1b(); pass2i(); pass2o(); pass4(); break;
case '0':
printf("Sequences mapping into a constant string of 0's:"); kwait(0);
pass1x(0); pass2i(); pass2o(); pass4(); break;
case '1':
printf("Sequences mapping into a constant string of 1's:"); kwait(0);
pass1x(1); pass2i(); pass2o(); pass4(); break;
case '2':
printf("Sequences mapping into a constant string of 2's:"); kwait(0);
pass1x(2); pass2i(); pass2o(); pass4(); break;
case '3':
printf("Sequences mapping into a constant string of 3's:"); kwait(0);
pass1x(3); pass2i(); pass2o(); pass4(); break;
default: break; };
 } while (c!='q');

} /* end main */

/* Pass 1a marks all the configurations which fulfil (1,0) */
pass1a() {int i,j,k;
  printf(" Pass1a\015");
  for (i=0; i<4; i++) {
  for (j=0; j<4; j++) {
  for (k=0; k<4; k++) {
  arry[i][j][k]=rule[i+j+k]==j?'Y':'N';
  };};}; }

/* Pass 1b marks all the configurations which fulfil (1,1) */
pass1b() {int i,j,k;
  printf(" Pass1b\015");
  for (i=0; i<4; i++) {
  for (j=0; j<4; j++) {
  for (k=0; k<4; k++) {
  arry[i][j][k]=rule[i+j+k]==i?'Y':'N';
  };};}; }

/* Pass 1x marks all the configurations mapping into a constant */
pass1x(c) int c; {int i,j,k;
  printf(" Pass1a\015");
  for (i=0; i<4; i++) {
  for (j=0; j<4; j++) {
  for (k=0; k<4; k++) {
  arry[i][j][k]=rule[i+j+k]==c?'Y':'N';
  };};}; }

/* Pass 2i flags all links with an incoming arrow */
/* Pass 2o flags all links with an outgoing arrow */
/* Then pass 3 discards all unflagged links */
/* Passes 2 and 3 alternate until no change is observed */

pass2i() {int i, j, k, m;
do {
printf(" Pass2i\015");
for (i=0; i<4; i++) {
for (j=0; j<4; j++) {
for (k=0; k<4; k++) {
if ((arry[i][j][k]&0x5F)=='Y') {for (m=0; m<4; m++) arry[j][k][m]|=0x20;};
};};}; } while (pass3()!=0); }

pass2o() {int i, j, k, m;
do {
printf(" Pass2o\015");
for (i=0; i<4; i++) {
for (j=0; j<4; j++) {
for (k=0; k<4; k++) {
if ((arry[i][j][k]&0x5F)=='Y') {for (m=0; m<4; m++) arry[m][i][j]|=0x20;};
};};} } while (pass3()!=0); }

/* Pass 3 - erase flags, mark survivors, count changes */

int pass3() {int i, j, k, l;
l=0;
printf(" Pass3\015");
for (i=0; i<4; i++) {
for (j=0; j<4; j++) {
for (k=0; k<4; k++) {
  switch (arry[i][j][k]) {
    case 'y': arry[i][j][k]='Y'; break;
    case 'Y': arry[i][j][k]='N'; l=1; break;
    case 'n': case 'N': arry[i][j][k]='N'; break;
    default: break; };
};};};
return l;
}

/* pass4 - print loops which remain */
pass4() {
int i0, i1, i2;
int j0, j1, j2, k, l, m;
printf(" pass4 \015");
for (i0=0; i0<4; i0++) {
for (i1=0; i1<4; i1++) {
for (i2=0; i2<4; i2++) {
j1=i1; j2=i2;l=0; m=0;
do {
        if (arry[0][j1][j2]=='Y')
    {arry[0][j1][j2]='y';
    k=j2; j2=j1; j1=0; m=1;}
  else {if (arry[1][j1][j2]=='Y')
    {arry[1][j1][j2]='y';
    k=j2; j2=j1; j1=1; m=1;}
  else {if (arry[2][j1][j2]=='Y')
    {arry[2][j1][j2]='y';
    k=j2; j2=j1; j1=2; m=1;}
  else {if (arry[3][j1][j2]=='Y')
    {arry[3][j1][j2]='y';
    k=j2; j2=j1; j1=3; m=1;}
  else {l=1; if (m==1) {j0=j1; j1=j2; j2=k;}; };};};};
  } while (l==0);
l=0; 
m=0;
do {
        if (arry[j0][j1][0]=='y')
   {prf(j0,j1,0);
   arry[j0][j1][0]='N';
   j0=j1; j1=0; m=1;}
  else {if (arry[j0][j1][1]=='y')
   {prf(j0,j1,1);
   arry[j0][j1][1]='N';
   j0=j1; j1=1; m=1;}
  else {if (arry[j0][j1][2]=='y')
   {prf(j0,j1,2);
   arry[j0][j1][2]='N';
   j0=j1; j1=2; m=1;}
  else {if (arry[j0][j1][3]=='y')
   {prf(j0,j1,3);
   arry[j0][j1][3]='N';
   j0=j1; j1=3; m=1;}
  else {l=1;};};};};
  } while (l==0);
l=0;
do {
        if (arry[j0][j1][0]=='Y')
   {prf(j0,j1,0);
   arry[j0][j1][0]='N';
   j0=j1; j1=0; m=1;}
  else {if (arry[j0][j1][1]=='Y')
   {prf(j0,j1,1);
   arry[j0][j1][1]='N';
   j0=j1; j1=1; m=1;}
  else {if (arry[j0][j1][2]=='Y')
   {prf(j0,j1,2);
   arry[j0][j1][2]='N';
   j0=j1; j1=2; m=1;}
  else {if (arry[j0][j1][3]=='Y')
   {prf(j0,j1,3);
   arry[j0][j1][3]='N';
   j0=j1; j1=3; m=1;}
  else {l=1; if (m==1) kwait(0);};};};};
  } while (l==0);
};};};
}

/* print one link of the de Bruijn diagram */
prf(i,j,k) int i, j, k; {
kwait(1);
printf("%1d",i);
printf("%1d",j);
printf("-");
printf("%1d",k);
printf("  ");}

/* print the whole array */
/* mostly for debugging */
pall() {int i, j, k;
for (i=0; i<4; i++) {
for (j=0; j<4; j++) {
for (k=0; k<4; k++) {
printf("%c",arry[i][j][k]);
};};};
printf("\n");
}

/* keyboard status */
kbdst() {return bdos(11)&0xFF;}

/* direct keyboard input, no echo */
kbdin() {int c;
if ((c=bdos(7)&0xFF)=='\0') c=(bdos(7)&0xFF)|0x80;
return c;}

/* pause at the end of a full screen */
kwait(i) int i; {
switch (i) {
  case 0: printf("\n"); nc=0; nl++; break;
  case 1: if (nc==MC) {printf("&\n"); nc=1; nl++;} else nc++; break;
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
