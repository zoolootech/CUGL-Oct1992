
/* program to analyze the de Bruijn diagram of a cellular */
/* automaton and report all the periodic states. */
/* version for totalistic (3,1), fourth generation */
/* [Harold V. McIntosh, 18 May 1987] */

# include <stdio.h>

# define MC	 6				/* maximum number of columns */
# define NS      7				/* number of distinct sums */
# define ML	24				/* pause after so many lines */

char arry[3][3][3][3][3][3][3][3][3];
int  rule[NS];
int  nc, nl;

main() {
int i;

printf("Rule number:\n");
printf("0..1..2\n");
for (i=0; i<NS; i++) rule[i]=getchar()-'0';

nc=0;
nl=0;

printf("\n - several minutes may elapse for each case - \n");

kwait(0); printf("Strings conforming to (4,0):"); kwait(0);
pass1a();
pass2i();
pass2o();
pass4();

kwait(0); printf("Strings conforming to (4,1):"); kwait(0);
pass1b();
pass2i();
pass2o();
pass4();

kwait(0); printf("Strings conforming to (4,2):"); kwait(0);
pass1c();
pass2i();
pass2o();
pass4();

kwait(0); printf("Strings conforming to (4,3):"); kwait(0);
pass1d();
pass2i();
pass2o();
pass4();

kwait(0); printf("Strings conforming to (4,4):"); kwait(0);
pass1e();
pass2i();
pass2o();
pass4();

} /* end of main */

pass1a() {			/* mark sequences conforming to (4,0) */
int i0, i1, i2, i3, i4, i5, i6, i7, i8;
int j0, j1, j2, j3, j4, j5, j6;
int k0, k1, k2, k3, k4;
int i, j, k;
printf(" pass1a\015");
for (i0=0; i0<3; i0++) {
for (i1=0; i1<3; i1++) {
for (i2=0; i2<3; i2++) {
for (i3=0; i3<3; i3++) {
for (i4=0; i4<3; i4++) {
for (i5=0; i5<3; i5++) {
for (i6=0; i6<3; i6++) {
for (i7=0; i7<3; i7++) {
for (i8=0; i8<3; i8++) {
j0=rule[i0+i1+i2];
j1=rule[i1+i2+i3];
j2=rule[i2+i3+i4];
j3=rule[i3+i4+i5];
j4=rule[i4+i5+i6];
j5=rule[i5+i6+i7];
j6=rule[i6+i7+i8];
k0=rule[j0+j1+j2];
k1=rule[j1+j2+j3];
k2=rule[j2+j3+j4];
k3=rule[j3+j4+j5];
k4=rule[j4+j5+j6];
i=rule[k0+k1+k2];
j=rule[k1+k2+k3];
k=rule[k2+k3+k4];
arry[i0][i1][i2][i3][i4][i5][i6][i7][i8]=rule[i+j+k]==i4?'Y':'N';
};};};};};};};};};
}

pass1b() {			/* mark sequences conforming to (4,1) */
int i0, i1, i2, i3, i4, i5, i6, i7, i8;
int j0, j1, j2, j3, j4, j5, j6;
int k0, k1, k2, k3 ,k4;
int i, j, k;
printf(" pass1a\015");
for (i0=0; i0<3; i0++) {
for (i1=0; i1<3; i1++) {
for (i2=0; i2<3; i2++) {
for (i3=0; i3<3; i3++) {
for (i4=0; i4<3; i4++) {
for (i5=0; i5<3; i5++) {
for (i6=0; i6<3; i6++) {
for (i7=0; i7<3; i7++) {
for (i8=0; i8<3; i8++) {
j0=rule[i0+i1+i2];
j1=rule[i1+i2+i3];
j2=rule[i2+i3+i4];
j3=rule[i3+i4+i5];
j4=rule[i4+i5+i6];
j5=rule[i5+i6+i7];
j6=rule[i6+i7+i8];
k0=rule[j0+j1+j2];
k1=rule[j1+j2+j3];
k2=rule[j2+j3+j4];
k3=rule[j3+j4+j5];
k4=rule[j4+j5+j6];
i=rule[k0+k1+k2];
j=rule[k1+k2+k3];
k=rule[k2+k3+k4];
arry[i0][i1][i2][i3][i4][i5][i6][i7][i8]=rule[i+j+k]==i3?'Y':'N';
};};};};};};};};};
}

pass1c() {			/* mark sequences conforming to (4,2) */
int i0, i1, i2, i3, i4, i5, i6, i7, i8;
int j0, j1, j2, j3, j4, j5, j6;
int k0, k1, k2, k3 ,k4;
int i, j, k;
printf(" pass1a\015");
for (i0=0; i0<3; i0++) {
for (i1=0; i1<3; i1++) {
for (i2=0; i2<3; i2++) {
for (i3=0; i3<3; i3++) {
for (i4=0; i4<3; i4++) {
for (i5=0; i5<3; i5++) {
for (i6=0; i6<3; i6++) {
for (i7=0; i7<3; i7++) {
for (i8=0; i8<3; i8++) {
j0=rule[i0+i1+i2];
j1=rule[i1+i2+i3];
j2=rule[i2+i3+i4];
j3=rule[i3+i4+i5];
j4=rule[i4+i5+i6];
j5=rule[i5+i6+i7];
j6=rule[i6+i7+i8];
k0=rule[j0+j1+j2];
k1=rule[j1+j2+j3];
k2=rule[j2+j3+j4];
k3=rule[j3+j4+j5];
k4=rule[j4+j5+j6];
i=rule[k0+k1+k2];
j=rule[k1+k2+k3];
k=rule[k2+k3+k4];
arry[i0][i1][i2][i3][i4][i5][i6][i7][i8]=rule[i+j+k]==i2?'Y':'N';
};};};};};};};};};
}

pass1d() {			/* mark sequences conforming to (4,3) */
int i0, i1, i2, i3, i4, i5, i6, i7, i8;
int j0, j1, j2, j3, j4, j5, j6;
int k0, k1, k2, k3 ,k4;
int i, j, k;
printf(" pass1a\015");
for (i0=0; i0<3; i0++) {
for (i1=0; i1<3; i1++) {
for (i2=0; i2<3; i2++) {
for (i3=0; i3<3; i3++) {
for (i4=0; i4<3; i4++) {
for (i5=0; i5<3; i5++) {
for (i6=0; i6<3; i6++) {
for (i7=0; i7<3; i7++) {
for (i8=0; i8<3; i8++) {
j0=rule[i0+i1+i2];
j1=rule[i1+i2+i3];
j2=rule[i2+i3+i4];
j3=rule[i3+i4+i5];
j4=rule[i4+i5+i6];
j5=rule[i5+i6+i7];
j6=rule[i6+i7+i8];
k0=rule[j0+j1+j2];
k1=rule[j1+j2+j3];
k2=rule[j2+j3+j4];
k3=rule[j3+j4+j5];
k4=rule[j4+j5+j6];
i=rule[k0+k1+k2];
j=rule[k1+k2+k3];
k=rule[k2+k3+k4];
arry[i0][i1][i2][i3][i4][i5][i6][i7][i8]=rule[i+j+k]==i1?'Y':'N';
};};};};};};};};};
}

pass1e() {			/* mark sequences conforming to (4,4) */
int i0, i1, i2, i3, i4, i5, i6, i7, i8;
int j0, j1, j2, j3, j4, j5, j6;
int k0, k1, k2, k3 ,k4;
int i, j, k;
printf(" pass1a\015");
for (i0=0; i0<3; i0++) {
for (i1=0; i1<3; i1++) {
for (i2=0; i2<3; i2++) {
for (i3=0; i3<3; i3++) {
for (i4=0; i4<3; i4++) {
for (i5=0; i5<3; i5++) {
for (i6=0; i6<3; i6++) {
for (i7=0; i7<3; i7++) {
for (i8=0; i8<3; i8++) {
j0=rule[i0+i1+i2];
j1=rule[i1+i2+i3];
j2=rule[i2+i3+i4];
j3=rule[i3+i4+i5];
j4=rule[i4+i5+i6];
j5=rule[i5+i6+i7];
j6=rule[i6+i7+i8];
k0=rule[j0+j1+j2];
k1=rule[j1+j2+j3];
k2=rule[j2+j3+j4];
k3=rule[j3+j4+j5];
k4=rule[j4+j5+j6];
i=rule[k0+k1+k2];
j=rule[k1+k2+k3];
k=rule[k2+k3+k4];
arry[i0][i1][i2][i3][i4][i5][i6][i7][i8]=rule[i+j+k]==i0?'Y':'N';
};};};};};};};};};
}

/* Pass2i flags links which have an incoming arrow */
pass2i() {int i0, i1, i2, i3, i4, i5, i6, i7, i8, m;
do {
printf(" pass2i\015");
for (i0=0; i0<3; i0++) {
for (i1=0; i1<3; i1++) {
for (i2=0; i2<3; i2++) {
for (i3=0; i3<3; i3++) {
for (i4=0; i4<3; i4++) {
for (i5=0; i5<3; i5++) {
for (i6=0; i6<3; i6++) {
for (i7=0; i7<3; i7++) {
for (i8=0; i8<3; i8++) {
if ((arry[i0][i1][i2][i3][i4][i5][i6][i7][i8]&0x5F)=='Y')
 {for (m=0; m<3; m++) arry[i1][i2][i3][i4][i5][i6][i7][i8][m]|=0x20;};
};};};};};};};};};
} while (pass3()!=0); }

/* Pass2o flags links which have an outgoing arrow */
pass2o() {int i0, i1, i2, i3, i4, i5, i6, i7, i8, m;
do {
printf(" pass2o\015");
for (i0=0; i0<3; i0++) {
for (i1=0; i1<3; i1++) {
for (i2=0; i2<3; i2++) {
for (i3=0; i3<3; i3++) {
for (i4=0; i4<3; i4++) {
for (i5=0; i5<3; i5++) {
for (i6=0; i6<3; i6++) {
for (i7=0; i7<3; i7++) {
for (i8=0; i8<3; i8++) {
if ((arry[i0][i1][i2][i3][i4][i5][i6][i7][i8]&0x5F)=='Y')
 {for (m=0; m<3; m++) arry[m][i0][i1][i2][i3][i4][i5][i6][i7]|=0x20;};
};};};};};};};};};
} while (pass3()!=0); }

/* erase flags, mark survivors, count channges */
int pass3() {int i0, i1, i2, i3, i4, i5, i6, i7, i8, l;
l=0;
printf(" pass3 \015");
for (i0=0; i0<3; i0++) {
for (i1=0; i1<3; i1++) {
for (i2=0; i2<3; i2++) {
for (i3=0; i3<3; i3++) {
for (i4=0; i4<3; i4++) {
for (i5=0; i5<3; i5++) {
for (i6=0; i6<3; i6++) {
for (i7=0; i7<3; i7++) {
for (i8=0; i8<3; i8++) {
switch (arry[i0][i1][i2][i3][i4][i5][i6][i7][i8]) {
    case 'y': arry[i0][i1][i2][i3][i4][i5][i6][i7][i8]='Y'; break;
    case 'Y': arry[i0][i1][i2][i3][i4][i5][i6][i7][i8]='N'; l=1; break;
    case 'n': case 'N': arry[i0][i1][i2][i3][i4][i5][i6][i7][i8]='N'; break;
    default: break; };
};};};};};};};};};
return l; }

/* pass4 - print loops which remain */
pass4() {
int i0, i1, i2, i3, i4, i5, i6, i7, i8;
int j0, j1, j2, j3, j4, j5, j6, j7, j8;
int k, l, m;
printf(" pass4 \015");
for (i0=0; i0<3; i0++) {
for (i1=0; i1<3; i1++) {
for (i2=0; i2<3; i2++) {
for (i3=0; i3<3; i3++) {
for (i4=0; i4<3; i4++) {
for (i5=0; i5<3; i5++) {
for (i6=0; i6<3; i6++) {
for (i7=0; i7<3; i7++) {
for (i8=0; i8<3; i8++) {
j1=i1; j2=i2; j3=i3; j4=i4; j5=i5; j6=i6; j7=i7; j8=i8;
l=0;
m=0;
do {
        if (arry[0][j1][j2][j3][j4][j5][j6][j7][j8]=='Y')
    {arry[0][j1][j2][j3][j4][j5][j6][j7][j8]='y';
    k=j8; j8=j7; j7=j6; j6=j5; j5=j4; j4=j3; j3=j2; j2=j1; j1=0; m=1;}
  else {if (arry[1][j1][j2][j3][j4][j5][j6][j7][j8]=='Y')
    {arry[1][j1][j2][j3][j4][j5][j6][j7][j8]='y';
    k=j8; j8=j7; j7=j6; j6=j5; j5=j4; j4=j3; j3=j2; j2=j1; j1=1; m=1;}
  else {if (arry[2][j1][j2][j3][j4][j5][j6][j7][j8]=='Y')
    {arry[2][j1][j2][j3][j4][j5][j6][j7][j8]='y';
    k=j8; j8=j7; j7=j6; j6=j5; j5=j4; j4=j3; j3=j2; j2=j1; j1=2; m=1;}
  else {l=1;
    if (m==1) {j0=j1; j1=j2; j2=j3; j3=j4; j4=j5; j5=j6; j6=j7; j7=j8; j8=k;};
    };};};
  } while (l==0);
l=0; 
m=0;
do {
        if (arry[j0][j1][j2][j3][j4][j5][j6][j7][0]=='y')
   {prf(j0,j1,j2,j3,j4,j5,j6,j7,0);
   arry[j0][j1][j2][j3][j4][j5][j6][j7][0]='N';
   j0=j1; j1=j2; j2=j3; j3=j4; j4=j5; j5=j6; j6=j7; j7=0; m=1;}
  else {if (arry[j0][j1][j2][j3][j4][j5][j6][j7][1]=='y')
   {prf(j0,j1,j2,j3,j4,j5,j6,j7,1);
   arry[j0][j1][j2][j3][j4][j5][j6][j7][1]='N';
   j0=j1; j1=j2; j2=j3; j3=j4; j4=j5; j5=j6; j6=j7; j7=1; m=1;}
  else {if (arry[j0][j1][j2][j3][j4][j5][j6][j7][2]=='y')
   {prf(j0,j1,j2,j3,j4,j5,j6,j7,2);
   arry[j0][j1][j2][j3][j4][j5][j6][j7][2]='N';
   j0=j1; j1=j2; j2=j3; j3=j4; j4=j5; j5=j6; j6=j7; j7=2; m=1;}
  else {l=1;};};};
  } while (l==0);
l=0;
do {
        if (arry[j0][j1][j2][j3][j4][j5][j6][j7][0]=='Y')
   {prf(j0,j1,j2,j3,j4,j5,j6,j7,0);
   arry[j0][j1][j2][j3][j4][j5][j6][j7][0]='N';
   j0=j1; j1=j2; j2=j3; j3=j4; j4=j5; j5=j6; j6=j7; j7=0; m=1;}
  else {if (arry[j0][j1][j2][j3][j4][j5][j6][j7][1]=='Y')
   {prf(j0,j1,j2,j3,j4,j5,j6,j7,1);
   arry[j0][j1][j2][j3][j4][j5][j6][j7][1]='N';
   j0=j1; j1=j2; j2=j3; j3=j4; j4=j5; j5=j6; j6=j7; j7=1; m=1;}
  else {if (arry[j0][j1][j2][j3][j4][j5][j6][j7][2]=='Y')
   {prf(j0,j1,j2,j3,j4,j5,j6,j7,2);
   arry[j0][j1][j2][j3][j4][j5][j6][j7][2]='N';
   j0=j1; j1=j2; j2=j3; j3=j4; j4=j5; j5=j6; j6=j7; j7=2; m=1;}
  else {l=1; if (m==1) kwait(0);};};};
  } while (l==0);
};};};};};};};};};
}

/* print one of the individual links in a chain */
prf(i0,i1,i2,i3,i4,i5,i6,i7,i8)
int i0, i1, i2, i3, i4, i5, i6, i7, i8; {
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
printf(" ");}

/* print the whole list of links - impractical except for debugging  */
pall() {
int i0, i1, i2, i3, i4, i5, i6, i7, i8;
for (i0=0; i0<3; i0++) {
for (i1=0; i1<3; i1++) {
for (i2=0; i2<3; i2++) {
for (i3=0; i3<3; i3++) {
for (i4=0; i4<3; i4++) {
for (i5=0; i5<3; i5++) {
for (i6=0; i6<3; i6++) {
for (i7=0; i7<3; i7++) {
for (i8=0; i8<3; i8++) {
printf("%c",arry[i0][i1][i2][i3][i4][i5][i6][i7][i8]);
};};};};};};};};};
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
  case 1: {if (nc==MC) {printf("&\n"); nc=1; nl++;} else nc++;}; break;
  default: break;};
if (nl==ML) {
  nl=0;
  printf(" press any key to continue\015");
  while (kbdst()) {};
  kbdin();
  printf("-                         \n");
  };
}

/* end */
