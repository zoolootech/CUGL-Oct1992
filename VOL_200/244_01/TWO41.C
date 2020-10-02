
/* program to analyze the de Bruijn diagram of a cellular */
/* automaton and report all the periodic states. */
/* version for totalistic (4,1), second generation */
/* [Harold V. McIntosh, 4 May 1987] */
/* [10 May 1987 - wait after typing 24 lines] */

# include <stdio.h>

# define MC	 9				/* maximum number of columns */
# define NS     10				/* number of distinct sums */
# define NW	24				/* pause after so many lines */

char arry[4][4][4][4][4];			/* all states in the diagram */
int  rule[NS] = 0,1,0,0,1,2,1,2,3,3;		/* transitions by sum value */	
int  nc, nl;					/* #columns, # lines for print */

main() {
int i;

printf("Rule number:\n\n");
printf("0..1..2..3\n");
for (i=0; i<NS; i++) rule[i]=getchar()-'0';
nc=0;
nl=0;

kwait(0); printf("Strings conforming to (2,0):"); kwait(0);
pass1a();
pass2i();
pass2o();
pass4();

kwait(0); printf("Strings conforming to (2,1):"); kwait(0);
pass1b();
pass2i();
pass2o();
pass4();

kwait(0); printf("Strings conforming to (2,2):"); kwait(0);
pass1c();
pass2i();
pass2o();
pass4();
	    
} /* end of main */

pass1a() {int i0, i1, i2, i3, i4, i, j, k;	/* mark sequences conforming to (2,0) */
printf(" Pass1a\015");
for (i0=0; i0<4; i0++) {
for (i1=0; i1<4; i1++) {
for (i2=0; i2<4; i2++) {
for (i3=0; i3<4; i3++) {
for (i4=0; i4<4; i4++) {
i=rule[i0+i1+i2];
j=rule[i1+i2+i3];
k=rule[i2+i3+i4];
arry[i0][i1][i2][i3][i4]=rule[i+j+k]==i2?'Y':'N';
};};};};};
}

pass1b() {int i0, i1, i2, i3, i4, i, j, k;	/* mark sequences conforming to (2,1) */
printf(" Pass1b\015");
for (i0=0; i0<4; i0++) {
for (i1=0; i1<4; i1++) {
for (i2=0; i2<4; i2++) {
for (i3=0; i3<4; i3++) {
for (i4=0; i4<4; i4++) {
i=rule[i0+i1+i2];
j=rule[i1+i2+i3];
k=rule[i2+i3+i4];
arry[i0][i1][i2][i3][i4]=rule[i+j+k]==i1?'Y':'N';
};};};};};
}

pass1c() {int i0, i1, i2, i3, i4, i, j, k;	/* mark sequences conforming to (2,2) */
printf(" Pass1c\015");
for (i0=0; i0<4; i0++) {
for (i1=0; i1<4; i1++) {
for (i2=0; i2<4; i2++) {
for (i3=0; i3<4; i3++) {
for (i4=0; i4<4; i4++) {
i=rule[i0+i1+i2];
j=rule[i1+i2+i3];
k=rule[i2+i3+i4];
arry[i0][i1][i2][i3][i4]=rule[i+j+k]==i0?'Y':'N';
};};};};};
}

/* Pass2i flags links which have an incoming arrow */
pass2i() {int i0, i1, i2, i3, i4, m;
printf(" Pass2i\015");
do {
for (i0=0; i0<4; i0++) {
for (i1=0; i1<4; i1++) {
for (i2=0; i2<4; i2++) {
for (i3=0; i3<4; i3++) {
for (i4=0; i4<4; i4++) {
if ((arry[i0][i1][i2][i3][i4]&0x5F)=='Y')
 {for (m=0; m<4; m++) arry[i1][i2][i3][i4][m]|=0x20;};
};};};};};
} while (pass3()!=0); }

/* Pass2o flags links which have an outgoing arrow */
pass2o() {int i0, i1, i2, i3, i4, m;
do {
printf(" Pass2o\015");
for (i0=0; i0<4; i0++) {
for (i1=0; i1<4; i1++) {
for (i2=0; i2<4; i2++) {
for (i3=0; i3<4; i3++) {
for (i4=0; i4<4; i4++) {
if ((arry[i0][i1][i2][i3][i4]&0x5F)=='Y')
 {for (m=0; m<4; m++) arry[m][i0][i1][i2][i3]|=0x20;};
};};};};};
} while (pass3()!=0); }

/* printf("pass3 - erase flags, mark survivors, count channges\n"); */
int pass3() {int i0, i1, i2, i3, i4, l;	/* mark states still linked */
printf(" Pass3\015");
l=0;
for (i0=0; i0<4; i0++) {
for (i1=0; i1<4; i1++) {
for (i2=0; i2<4; i2++) {
for (i3=0; i3<4; i3++) {
for (i4=0; i4<4; i4++) {
switch (arry[i0][i1][i2][i3][i4]) {
    case 'y': arry[i0][i1][i2][i3][i4]='Y'; break;
    case 'Y': arry[i0][i1][i2][i3][i4]='N'; l=1; break;
    case 'n': case 'N': arry[i0][i1][i2][i3][i4]='N'; break;
    default: break; };
};};};};};
return l;
}

/* pass4 - print loops which remain */
pass4() {
int i0, i1, i2, i3, i4;
int j0, j1, j2, j3, j4, k, l, m;
printf(" pass4 \015");
for (i0=0; i0<4; i0++) {
for (i1=0; i1<4; i1++) {
for (i2=0; i2<4; i2++) {
for (i3=0; i3<4; i3++) {
for (i4=0; i4<4; i4++) {
j1=i1; j2=i2; j3=i3; j4=i4; l=0; m=0;
do {
        if (arry[0][j1][j2][j3][j4]=='Y')
    {arry[0][j1][j2][j3][j4]='y';
    k=j4; j4=j3; j3=j2; j2=j1; j1=0; m=1;}
  else {if (arry[1][j1][j2][j3][j4]=='Y')
    {arry[1][j1][j2][j3][j4]='y';
    k=j4; j4=j3; j3=j2; j2=j1; j1=1; m=1;}
  else {if (arry[2][j1][j2][j3][j4]=='Y')
    {arry[2][j1][j2][j3][j4]='y';
    k=j4; j4=j3; j3=j2; j2=j1; j1=2; m=1;}
  else {if (arry[3][j1][j2][j3][j4]=='Y')
    {arry[3][j1][j2][j3][j4]='y';
    k=j4; j4=j3; j3=j2; j2=j1; j1=3; m=1;}
  else {l=1; if (m==1) {j0=j1; j1=j2; j2=j3; j3=j4; j4=k;}; };};};};
  } while (l==0);
l=0; 
m=0;
do {
        if (arry[j0][j1][j2][j3][0]=='y')
   {prf(j0,j1,j2,j3,0);
   arry[j0][j1][j2][j3][0]='N';
   j0=j1; j1=j2; j2=j3; j3=0; m=1;}
  else {if (arry[j0][j1][j2][j3][1]=='y')
   {prf(j0,j1,j2,j3,1);
   arry[j0][j1][j2][j3][1]='N';
   j0=j1; j1=j2; j2=j3; j3=1; m=1;}
  else {if (arry[j0][j1][j2][j3][2]=='y')
   {prf(j0,j1,j2,j3,2);
   arry[j0][j1][j2][j3][2]='N';
   j0=j1; j1=j2; j2=j3; j3=2; m=1;}
  else {if (arry[j0][j1][j2][j3][3]=='y')
   {prf(j0,j1,j2,j3,3);
   arry[j0][j1][j2][j3][3]='N';
   j0=j1; j1=j2; j2=j3; j3=3; m=1;}
  else {l=1;};};};};
  } while (l==0);
l=0;
do {
        if (arry[j0][j1][j2][j3][0]=='Y')
   {prf(j0,j1,j2,j3,0);
   arry[j0][j1][j2][j3][0]='N';
   j0=j1; j1=j2; j2=j3; j3=0; m=1;}
  else {if (arry[j0][j1][j2][j3][1]=='Y')
   {prf(j0,j1,j2,j3,1);
   arry[j0][j1][j2][j3][1]='N';
   j0=j1; j1=j2; j2=j3; j3=1; m=1;}
  else {if (arry[j0][j1][j2][j3][2]=='Y')
   {prf(j0,j1,j2,j3,2);
   arry[j0][j1][j2][j3][2]='N';
   j0=j1; j1=j2; j2=j3; j3=2; m=1;}
  else {if (arry[j0][j1][j2][j3][3]=='Y')
   {prf(j0,j1,j2,j3,3);
   arry[j0][j1][j2][j3][3]='N';
   j0=j1; j1=j2; j2=j3; j3=3; m=1;}
  else {l=1; if (m==1) {kwait(0);} ;} ;};};};
  } while (l==0);
};};};};};
}

/* print one of the links in a chain */
prf(i0,i1,i2,i3,i4) int i0, i1,i2, i3, i4; {
kwait(1);
printf("%1d",i0);
printf("%1d",i1);
printf("%1d",i2);
printf("%1d",i3);
printf("-");
printf("%1d",i4);
printf("  ");}

/* print the whole list of links */
pall() {
int i0, i1, i2, i3, i4;
for (i0=0; i0<4; i0++) {
for (i1=0; i1<4; i1++) {
for (i2=0; i2<4; i2++) {
for (i3=0; i3<4; i3++) {
for (i4=0; i4<4; i4++) {
printf("%c",arry[i0][i1][i2][i3][i4]);
};};};};};
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
