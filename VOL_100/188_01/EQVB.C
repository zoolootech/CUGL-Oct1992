/*
HEADER:		;
TITLE:		Unsymmetric variable band matrix factorization
VERSION:	1.0;

DESCRIPTIONº   	Factorization and linear equation solution with
               variable banded unsymmetric matrix

KEYWORDS:	Unymmetric variable band matrix solution
SYSTEM:		CP/M-80, V3.1;
FILENAME:	EQVB.C;
WARNINGSº  	requires floating point

AUTHORS:	Tim Prince;
COMPILERS:	MIX C, v2.0.1; Eco C 3.45; Aztec C 1.06B
*/
typedef double OPTREAL; /* can work on float or double arrays*/
main(){/* sample test of simultaneous linear solution */
#define ndef 12 /* 12 for double, 6 for single precision test*/
#define abs(d) (d>0?d:-d)
#define max(d,a) d>a?d:a
OPTREAL au[ndef*(ndef-1)],al[ndef*(ndef+1)],b[ndef],factr;
double sum,d;
int maxdvr,i,j,iu[ndef],il[ndef],icu,icl;
factr=3;
maxdvr=ndef*2-1;
for(i=2;i<=maxdvr;i++)
  factr*=i;/* maxdvr! */
/* scale factor 1 for true Hilbert matrix but this allows
** exact integral data and is a more severe test */
icu=icl=-1;
for(i=0;i<ndef;i++)
  {/* set up to test accuracy of solution for all 1's */
  for(sum=j=0;j<ndef;j++){
    /* set up Hilbert matrix, symmetric variable bandwidth */
    d=factr/(i+j+1);
    /* rows & columns are full length in this example
** upper triangle stored by columns, lower triangle
** by rows including diagonal element */
    if(j<i){
      icu++;
      au[icu]=d;} /* a[i][j] */
    if(j<=i){
      icl++;
      al[icl]=d;} /* a[j][i] */
    sum+=d;}
  iu[i]=icu; /* index to element above diagonal (may not exist) */
  il[i]=icl; /* index to diagonal element */
  b[i]=sum;}; /* [i++] fails on several compilers */
vbfac(au,iu,al,il,ndef);/* replace a by factors*/
vbfwb(au,iu,al,il,ndef,b);/* solve b system */
for(sum=i=0;i<ndef;i++)
  sum=max(abs(b[i]-1),sum);
printf("\n solution error:%g",sum);
}
vbfac(au,iu,al,il,n)
OPTREAL au[],al[];
int n,iu[],il[];
{
register double sum; /* long double preferred */
register int i,k,ic,iuc,ilr;
int j,iu1;
/* n: order of matrix
a[]: matrix to be overwritten by its triangular factors
optionally split into au (upper triangle)
and al (lower triangle including main diagonal) for clarity
al[il]: diagonal elements: end of row vector
au[iu]: element of column just above al[il]: end of column
matrix storage: variable length row & column (unsymmetric) */
for(j=1;j<n;j++){ /* factors L U 
** start by calculating U from 1st row of column j */
  for(i=(ic=iu1=iu[j-1]+1)-iu[j]-1+j;i<j;ic++,i++){
   /* set pointers to start dot product L row i, U col j
   ** first assuming row i equal or longer than col j
   ** then correcting if "reentrant" */
    ilr=(iuc=iu1)-ic+il[i];
    sum=0;
    if(ic>iu1){
      if((k=ilr-il[i-1])<=0){ 
	iuc+=k=1-k; /* skip over implicit zeros in row i */
	ilr+=k;}
      for(;iuc<ic;iuc++,ilr++)
	sum += au[iuc] * al[ilr];}
    au[ic]=(au[ic]-sum)/al[ilr];};/* replace by factor U */
/* now do L from 2nd column of row j */
  for(i=(ic=(iu1=il[j-1]+1)+1)-il[j]+j;i<=j;ic++,i++){
    if((k=(iuc=(ilr=iu1)-ic+iu[i]+1)-iu[i-1])<=0){
      iuc+=k=1-k; /* skip reentrancy in column i */
      ilr+=k;}
    for(sum=0;ilr<ic;iuc++,ilr++)
      sum += au[iuc] * al[ilr];
    al[ic] -= sum;} /* replace with L */
}
}
vbfwb(au,iu,al,il,n,b)
int n,iu[],il[];
OPTREAL au[],al[],b[];
{
register double sum;
register int i,j,ic,ilr;
/* b[n]: right side vector to be overwritten by
  solution 
solve L U b" = b
writing b' and b" over b 
U[j][j] =1 not stored */
for(ilr=-1,j=0;j<n;j++){ /* b' = Linv b */
/* multiply by L inverse */
  for(sum=0,i=++ilr+j-il[j];i<j;ilr++,i++)
    sum += al[ilr] * b[i];
  b[j] = (b[j]-sum)/al[ilr];};/* L inv b */
/* multiply by Lt inverse */
for(ilr=iu[n-1];--j;)
  for(i=j-1;ilr>iu[j-1];ilr--,i--)
    b[i] -= b[j] * au[ilr];
}
