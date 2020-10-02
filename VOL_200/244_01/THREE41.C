
/* program to analyze the de Bruijn diagram of a cellular */
/* automaton and report all the periodic states. */
/* version for totalistic (4,1), third generation */
/* [Harold V. McIntosh, 4 May 1987] */

# include <stdio.h>

# define MC	 8				/* maximum number of columns */
# define NS     10				/* number of distinct sums */
# define ML	24				/* pause after so many lines */

char arry[4][4][4][4][4][4][4];
int  rule[NS] = 0,1,0,0,1,2,1,2,3,3;
int  nc, nl;

main() {
int i;

printf("Rule number:\n");
printf("0..1..2..3\n");
for (i=0; i<NS; i++) rule[i]=getchar()-'0';

nc=0;
nl=0;

printf("\n - several minutes may elapse for each case - \n");

kwait(0); printf("Strings conforming to (3,0):"); kwait(0);
pass1a();
pass2i();
pass2o();
pass4();

kwait(0); printf("Strings conforming to (3,1):"); kwait(0);
pass1b();
pass2i();
pass2o();
pass4();

kwait(0); printf("Strings conforming to (3,2):"); kwait(0);
pass1c();
pass2i();
pass2o();
pass4();

kwait(0); printf("Strings conforming to (3,3):"); kwait(0);
pass1d();
pass2i();
pass2o();
pass4();

} /* end of main */

pass1a() {			/* mark sequences conforming to (3,0) */
int i0, i1, i2, i3, i4, i5, i6;
int j0, j1, j2, j3, j4;
int i, j, k;
printf(" pass1a\015");
for (i0=0; i0<4; i0++) {
for (i1=0; i1<4; i1++) {
for (i2=0; i2<4; i2++) {
for (i3=0; i3<4; i3++) {
for (i4=0; i4<4; i4++) {
for (i5=0; i5<4; i5++) {
for (i6=0; i6<4; i6++) {
j0=rule[i0+i1+i2];
j1=rule[i1+i2+i3];
j2=rule[i2+i3+i4];
j3=rule[i3+i4+i5];
j4=rule[i4+i5+i6];
i=rule[j0+j1+j2];
j=rule[j1+j2+j3];
k=rule[j2+j3+j4];
arry[i0][i1][i2][i3][i4][i5][i6]=rule[i+j+k]==i3?'Y':'N';
};};};};};};};
}

pass1b() {			/* mark sequences conforming to (3,1) */
int i0, i1, i2, i3, i4, i5, i6;
int j0, j1, j2, j3, j4;
int  i, j, k;
printf(" pass1b\015");
for (i0=0; i0<4; i0++) {
for (i1=0; i1<4; i1++) {
for (i2=0; i2<4; i2++) {
for (i3=0; i3<4; i3++) {
for (i4=0; i4<4; i4++) {
for (i5=0; i5<4; i5++) {
for (i6=0; i6<4; i6++) {
j0=rule[i0+i1+i2];
j1=rule[i1+i2+i3];
j2=rule[i2+i3+i4];
j3=rule[i3+i4+i5];
j4=rule[i4+i5+i6];
i=rule[j0+j1+j2];
j=rule[j1+j2+j3];
k=rule[j2+j3+j4];
arry[i0][i1][i2][i3][i4][i5][i6]=rule[i+j+k]==i2?'Y':'N';
};};};};};};};
}

pass1c() {			/* mark sequences conforming to (3,2) */
int i0, i1, i2, i3, i4, i5, i6;
int j0, j1, j2, j3, j4;
int i, j, k;
printf(" pass1c\015");
for (i0=0; i0<4; i0++) {
for (i1=0; i1<4; i1++) {
for (i2=0; i2<4; i2++) {
for (i3=0; i3<4; i3++) {
for (i4=0; i4<4; i4++) {
for (i5=0; i5<4; i5++) {
for (i6=0; i6<4; i6++) {
j0=rule[i0+i1+i2];
j1=rule[i1+i2+i3];
j2=rule[i2+i3+i4];
j3=rule[i3+i4+i5];
j4=rule[i4+i5+i6];
i=rule[j0+j1+j2];
j=rule[j1+j2+j3];
k=rule[j2+j3+j4];
arry[i0][i1][i2][i3][i4][i5][i6]=rule[i+j+k]==i1?'Y':'N';
};};};};};};};
}

pass1d() {			/* mark sequences conforming to (3,3) */
int i0, i1, i2, i3, i4, i5, i6;
int j0, j1, j2, j3, j4;
int i, j, k;
printf(" pass1d\015");
for (i0=0; i0<4; i0++) {
for (i1=0; i1<4; i1++) {
for (i2=0; i2<4; i2++) {
for (i3=0; i3<4; i3++) {
for (i4=0; i4<4; i4++) {
for (i5=0; i5<4; i5++) {
for (i6=0; i6<4; i6++) {
j0=rule[i0+i1+i2];
j1=rule[i1+i2+i3];
j2=rule[i2+i3+i4];
j3=rule[i3+i4+i5];
j4=rule[i4+i5+i6];
i=rule[j0+j1+j2];
j=rule[j1+j2+j3];
k=rule[j2+j3+j4];
arry[i0][i1][i2][i3][i4][i5][i6]=rule[i+j+k]==i0?'Y':'N';
};};};};};};};
}

/* Pass2i flags links which have an incoming arrow */
pass2i() {int i0, i1, i2, i3, i4, i5, i6, l, m;
do {
printf(" pass2i\015");
for (i0=0; i0<4; i0++) {
for (i1=0; i1<4; i1++) {
for (i2=0; i2<4; i2++) {
for (i3=0; i3<4; i3++) {
for (i4=0; i4<4; i4++) {
for (i5=0; i5<4; i5++) {
for (i6=0; i6<4; i6++) {
if ((arry[i0][i1][i2][i3][i4][i5][i6]&0x5F)=='Y')
 {for (m=0; m<4; m++) arry[i1][i2][i3][i4][i5][i6][m]|=0x20;};
};};};};};};};
l=pass3();		/* erase flage, mark survivors, count changes */
} while (l!=0); }

/* Pass2o flags links which have an outgoing arrow */
pass2o() {int i0, i1, i2, i3, i4, i5, i6, l, m;
do {
printf(" pass2o\015");
for (i0=0; i0<4; i0++) {
for (i1=0; i1<4; i1++) {
for (i2=0; i2<4; i2++) {
for (i3=0; i3<4; i3++) {
for (i4=0; i4<4; i4++) {
for (i5=0; i5<4; i5++) {
for (i6=0; i6<4; i6++) {
if ((arry[i0][i1][i2][i3][i4][i5][i6]&0x5F)=='Y')
 {for (m=0; m<4; m++) arry[m][i0][i1][i2][i3][i4][i5]|=0x20;};
};};};};};};};
l=pass3();		/* erase flags, mark survivors, count changes */
} while (l!=0); }

/* printf("pass3 - erase flags, mark survivors, count channges\n"); */
int pass3() {int i0, i1, i2, i3, i4, i5, i6, l;	/* mark states still linked */
l=0;
printf(" pass3 \015");
for (i0=0; i0<4; i0++) {
for (i1=0; i1<4; i1++) {
for (i2=0; i2<4; i2++) {
for (i3=0; i3<4; i3++) {
for (i4=0; i4<4; i4++) {
for (i5=0; i5<4; i5++) {
for (i6=0; i6<4; i6++) {
switch (arry[i0][i1][i2][i3][i4][i5][i6]) {
    case 'y': arry[i0][i1][i2][i3][i4][i5][i6]='Y'; break;
    case 'Y': arry[i0][i1][i2][i3][i4][i5][i6]='N'; l=1; break;
    case 'n': case 'N': arry[i0][i1][i2][i3][i4][i5][i6]='N'; break;
    default: break; };
};};};};};};};
return l; }

/* pass4 - print loops which remain */
pass4() {
int i0, i1, i2, i3, i4, i5, i6;
int j0, j1, j2, j3, j4, j5, j6, k, l, m;
printf(" pass4 \015");
for (i0=0; i0<4; i0++) {
for (i1=0; i1<4; i1++) {
for (i2=0; i2<4; i2++) {
for (i3=0; i3<4; i3++) {
for (i4=0; i4<4; i4++) {
for (i5=0; i5<4; i5++) {
for (i6=0; i6<4; i6++) {
j1=i1; j2=i2; j3=i3; j4=i4; j5=i5; j6=i6;
l=0;
m=0;
do {
        if (arry[0][j1][j2][j3][j4][j5][j6]=='Y')
    {arry[0][j1][j2][j3][j4][j5][j6]='y';
    k=j6; j5=j4; j4=j3; j3=j2; j2=j1; j1=0; m=1;}
  else {if (arry[1][j1][j2][j3][j4][j5][j6]=='Y')
    {arry[1][j1][j2][j3][j4][j5][j6]='y';
    k=j6; j5=j4; j4=j3; j3=j2; j2=j1; j1=1; m=1;}
  else {if (arry[2][j1][j2][j3][j4][j5][j6]=='Y')
    {arry[2][j1][j2][j3][j4][j5][j6]='y';
    k=j6; j5=j4; j4=j3; j3=j2; j2=j1; j1=2; m=1;}
  else {if (arry[3][j1][j2][j3][j4][j5][j6]=='Y')
    {arry[3][j1][j2][j3][j4][j5][j6]='y';
    k=j6; j5=j4; j4=j3; j3=j2; j2=j1; j1=3; m=1;}
  else {l=1;
    if (m==1) {j0=j1; j1=j2; j2=j3; j3=j4; j4=j5; j5=j6; j6=k;};
    };};};};
  } while (l==0);
l=0; 
m=0;
do {
        if (arry[j0][j1][j2][j3][j4][j5][0]=='y')
   {prf(j0,j1,j2,j3,j4,j5,0);
   arry[j0][j1][j2][j3][j4][j5][0]='N';
   j0=j1; j1=j2; j2=j3; j3=j4; j4=j5; j5=0; m=1;}
  else {if (arry[j0][j1][j2][j3][j4][j5][1]=='y')
   {prf(j0,j1,j2,j3,j4,j5,1);
   arry[j0][j1][j2][j3][j4][j5][1]='N';
   j0=j1; j1=j2; j2=j3; j3=j4; j4=j5; j5=1; m=1;}
  else {if (arry[j0][j1][j2][j3][j4][j5][2]=='y')
   {prf(j0,j1,j2,j3,j4,j5,2);
   arry[j0][j1][j2][j3][j4][j5][2]='N';
   j0=j1; j1=j2; j2=j3; j3=j4; j4=j5; j5=2; m=1;}
  else {if (arry[j0][j1][j2][j3][j4][j5][3]=='y')
   {prf(j0,j1,j2,j3,j4,j5,3);
   arry[j0][j1][j2][j3][j4][j5][3]='N';
   j0=j1; j1=j2; j2=j3; j3=j4; j4=j5; j5=3; m=1;}
  else {l=1;};};};};
  } while (l==0);
l=0;
do {
        if (arry[j0][j1][j2][j3][j4][j5][0]=='Y')
   {prf(j0,j1,j2,j3,j4,j5,0);
   arry[j0][j1][j2][j3][j4][j5][0]='N';
   j0=j1; j1=j2; j2=j3; j3=j4; j4=j5; j5=0; m=1;}
  else {if (arry[j0][j1][j2][j3][j4][j5][1]=='Y')
   {prf(j0,j1,j2,j3,j4,j5,1);
   arry[j0][j1][j2][j3][j4][j5][1]='N';
   j0=j1; j1=j2; j2=j3; j3=j4; j4=j5; j5=1; m=1;}
  else {if (arry[j0][j1][j2][j3][j4][j5][2]=='Y')
   {prf(j0,j1,j2,j3,j4,j5,2);
   arry[j0][j1][j2][j3][j4][j5][2]='N';
   j0=j1; j1=j2; j2=j3; j3=j4; j4=j5; j5=2; m=1;}
  else {if (arry[j0][j1][j2][j3][j4][j5][3]=='Y')
   {prf(j0,j1,j2,j3,j4,j5,3);
   arry[j0][j1][j2][j3][j4][j5][3]='N';
   j0=j1; j1=j2; j2=j3; j3=j4; j4=j5; j5=3; m=1;}
  else {l=1; if (m==1) kwait(0);};};};};
  } while (l==0);
};};};};};};};
}

/* print one of the individual links in a chain */
prf(i0,i1,i2,i3,i4,i5,i6)
int i0, i1, i2, i3, i4, i5, i6; {
kwait(1);
printf("%1d",i0);
printf("%1d",i1);
printf("%1d",i2);
printf("%1d",i3);
printf("%1d",i4);
printf("%1d",i5);
printf("-");
printf("%1d",i6);
printf(" ");}

/* print the whole list of links - impractical except for debugging  */
pall() {
int i0, i1, i2, i3, i4, i5, i6;
for (i0=0; i0<4; i0++) {
for (i1=0; i1<4; i1++) {
for (i2=0; i2<4; i2++) {
for (i3=0; i3<4; i3++) {
for (i4=0; i4<4; i4++) {
for (i5=0; i5<4; i5++) {
for (i6=0; i6<4; i6++) {
printf("%c",arry[i0][i1][i2][i3][i4][i5][i6]);
};};};};};};};
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
