/*
HEADER:		;
TITLE:		Symmetric matrix factorization
VERSION:	1.0;

DESCRIPTIONº   	Factorization and linear equation solution with
               full or variable banded symmetric matrix

KEYWORDS:	Symmetric matrix linear equation solution
SYSTEM:		CP/M-80, V3.1;
FILENAME:	EQSYM.C;
WARNINGSº  	requires floating point

AUTHORS:	Tim Prince;
COMPILERS:	MIX C, v2.0.1; Eco C 3.45; Aztec C 1.06B
*/
typedef double OPTREAL; /* can work on float or double arrays*/
main(){/* sample test of simultaneous linear solution */
#define ndef 12 /* 12 for double, 6 for single precision test*/
#define abs(d) (d>0?d:-d)
#define max(d,a) d>a?d:a
OPTREAL a[ndef*(ndef+1)],b[ndef],factr;
double sum,d;
int maxdvr,i,j,id[ndef],ic;
factr=3;
maxdvr=ndef*2-1;
for(i=2;i<=maxdvr;i++)
  factr*=i;/* maxdvr! */
/* scale factor 1 for true Hilbert matrix but this allows
** exact integral data and is a more severe test */
ic=-1;
for(i=0;i<ndef;i++)
  {/* set up to test accuracy of solution for all 1's */
/* multiple assignment in for doesn't work in Manx Aztec C */
  for(sum=j=0;j<ndef;j++){
    /* set up Hilbert matrix, symmetric variable bandwidth */
    d=factr/(i+j+1);
    if(j<=i){ /* columns are full length */
      ic++;
      a[ic]=d;}
    sum+=d;}
  id[i]=ic;
  b[i]=sum;}; /* [i++] fails on several compilers */
symfac(a,id,ndef);/* replace a by factors*/
symfwb(a,id,ndef,b);/* solve b system */
for(sum=i=0;i<ndef;i++)
  sum=max(abs(b[i]-1),sum);
printf("\n solution error:%g",sum);
}
symfac(a,id,n)
OPTREAL a[];
int n,id[];
{
register double u,sum; /* long double preferred */
register int i,k,ic,iu,il;
int j,k1,iu1;
/* n: order of matrix
a[]: matrix to be overwritten by its triangular factors
a[id[]]: diagonal elements: end of column vector
matrix storage: variable length columns (symmetric)
for(j=1;j<n;j++){ /* symmetric factors L D Lt */
/* start by calculating U=D Lt from 2nd row of column j */
  for(i=k1=(ic=(iu1=id[j-1]+1)+1)-id[j]+j;i<j;ic++,i++){
   /* set pointers to start dot product L row i, U col j
   ** first assuming row i equal or longer than col j
   ** then correcting if "reentrant" */
    if((k=(il=(iu=iu1)-ic+id[i])-id[i-1])<=0){
      iu+=k=1-k; /* skip over implicit zeros in col i */
      il+=k;}
    for(sum=0;iu<ic;iu++,il++)
      sum += a[iu] * a[il];
    a[ic]-=sum;};/* replace by factor U */
  for(k=k1-1,iu=iu1,sum=0;k<j;iu++,k++){
    a[iu]=(u=a[iu])/(a[id[k]]); /* replace with L = Dinv Ut */
    sum += a[iu] * u ;} /* Lt U */
  a[ic] -= sum; /* replace with D */
}
}
symfwb(a,id,n,b)
int n,id[];
OPTREAL a[],b[];
{
register double sum;
register int i,j,ic,il;
/* b[n]: right side vector to be overwritten by
  solution 
solve L D Lt b" = b
writing b' and b" over b 
L[j][j] =1 not stored */
for(j=1;j<n;j++){ /* b' = Linv b */
/* multiply by L inverse */
  for(sum=0,i=j+(il=id[j-1]+1)-id[j];i<j;il++,i++)
      sum += a[il] * b[i];
    b[j] -= sum;};/* L inv b */
for(j=0;j<n;j++) /* b" = Dinv b' */
  b[j] /= a[id[j]];
/* multiply by Lt inverse */
for(j=n-1;j;j--)
  for(i=j-1,il=id[j]-1;il>id[j-1];il--,i--)
    b[i] -= b[j] * a[il];
}
