/*
HEADER:		;
TITLE:		general matrix factorization;
VERSION:	1.0;

DESCRIPTIONº   	Sourcå   codå    foò   general full matrix 
               matrix factorization and linear system solution

		Employó Crouô scaleä partiaì pivotinç foò 
		fulì matrix®  Solutioî oæ lineaò equationó 
		anä accuracù testó arå demonstrated.

KEYWORDS:	Matrix linear equation solution;
SYSTEM:		CP/M-80, V3.1;
FILENAME:	EQD.C;
WARNINGSº	requireó compileò whicè supportó doublå 
		subscriptó [][Ý anä floatinç point.

AUTHORS:	Tim Prince;
COMPILERS:	MIX C, v2.0.1; Aztec C 1.06B
*/
typedef double OPTREAL; /* can work on float or double arrays*/
main(){/* sample test of simultaneous linear solution */
#define ndef 12 /* 12 for double, 6 for single precision test*/
#define abs(d) (d>0?d:-d)
#define max(d,a) d>a?d:a
float scale[ndef],accest;
OPTREAL a[ndef][ndef],b[ndef],aa[ndef][ndef],factr;
double sum,det();
char iperm[ndef];
int maxdvr,i,j;
factr=3;
maxdvr=ndef*2-1;
for(i=2;i<=maxdvr;i++)
  factr*=i;/* maxdvr! */
/* scale factor 1 for true Hilbert matrix but this allows
** exact integral data and is a more severe test */
for(i=0;i<ndef;i++)
  {/* set up to test accuracy of solution for all 1's */
  for(sum=j=0;j<ndef;j++)
    /* set up Hilbert matrix */
    sum+=aa[j][i]=a[j][i]=factr/(i+j+1);
  b[i]=sum;}; /* [i++] fails on several compilers */
factor(a,iperm,scale,&accest);/* replace a by factors*/
printf("\n accest =%e",accest); /* %g doesn't work in Mix C */
#define DETRMNT 1 
#ifdef DETRMNT
sum=det(a,iperm,&i);
printf("\n determinant = ldexp(%g,%d)",sum,i);
#endif
fwdbak(a,iperm,b,1);/* solve b system */
for(sum=i=0;i<ndef;i++)
  sum=max(abs(b[i]-1),sum);
printf("\n solution error:%g",sum);
}
factor(a,iperm,scale,accest)
OPTREAL a[ndef][ndef];
char iperm[ndef];
float scale[ndef],*accest;/* need not be accurate */
{register double d;
register float x,xmax,scali;/* need not be accurate */
register double sum; /* long double preferred */
register int i,j,k,ipiv,l;
/* n: order of matrix
a[ndef][ndef]: matrix to be overwritten by its triangular factors
iperm: record of row exchanges
accest: estimated relative accuracy of factorization
accest =0. if singular */
#define min(i,j) (i<j?i:j)
/* matrix storage: (row,column) stored by columns in FORTRAN
determine scale of each row */
for(i=0;i<ndef;i++)
  {
  for(scali=j=0;j<ndef;j++)
    scali=max(abs(a[j][i]),scali);
  scale[i]=1/scali;};/* avoid more slow divides
now factor by method in Jennings "Matrix Computation for
#Engineers and Scientists" p. 114 */
*accest=1;/* assume exact input*/
for(j=1;j<ndef;j++)
  {
  xmax=0;
/*  look for pivot */
  for(i=l=j-1;i<ndef;i++)
    {if((x=abs(a[l][i])*scale[i])>xmax){
/* find max scaled pivot */
      xmax=x;
      ipiv=i;};};
  /* move row ipiv to row j-1 */
  if(xmax<*accest)*accest=xmax;
/* indicates relative accuracy i.e. .1 for loss of 1
** digit due to cancellations */
  if(l!=(iperm[l]=ipiv)){/* swap rows; test to save time */
    x=scale[l];
    scale[l]=scale[ipiv];
    scale[ipiv]=x;
    for(k=0;k<ndef;k++)
      {d=a[k][l];
      a[k][l]=a[k][ipiv];
      a[k][ipiv]=d;};};
  d=a[l][l]; /* d=1/a[l][l] OK for float OPTREAL or long
** double d */
  for(i=1;i<ndef;i++)
    { /* can split in 2 loops and eliminate explicit if else 
** the i<j part doesn't depend on the swapping code and may be
** executed in parallel.  In the i>=j part the i iterations are
** not "vector dependent" and may be executed in parallel. */
    if(i>=j){
      a[j-1][i]/=d;/* complete lower factor */
      l=j;}
    else l=i;
    for(sum=k=0;k<l;k++)
      sum+=a[k][i]*a[j][k];
    a[j][i]-=sum;};/* replace a by factor */
};
*accest=min(abs(a[ndef-1][ndef-1]*scale[ndef-1]),*accest);
}
#ifdef DETRMNT
double det(a,iperm,exp)
OPTREAL a[ndef][ndef];
char iperm[ndef];
int *exp;
{ double frexp(),fract;
  int i,pwr;
/* calculate determinant as fraction * 2^exp to keep in range*/
  fract=a[0][0];
  *exp=0;
  for(i=1;i<ndef;i++){
    fract=frexp(fract*a[i][i],&pwr);
    *exp+=pwr;
/* each swap changes sign of determinant */
    if(iperm[i-1]!=i-1)fract=-fract;}
  return(fract);
}
  #ifndef AZTEC
/* dblfmt describes floating point format
** machine dependencies in case frexp() and ldexp() are not
** supplied */
struct dblfmt{
/* MIX C like Microsoft format */
  char mant[7]; /* full reverse byte order */
  char expn;
};
double frexp(x,scale)
  int *scale;
  double x;
{
  #define BIAS 128 /* IEEE 1023 */
  *scale=((struct dblfmt *)&x)->expn-BIAS;
  ((struct dblfmt *)&x)->expn=BIAS;
  return(x);
}
  #endif
#endif
fwdbak(a,iperm,b,m)
int m;
OPTREAL a[ndef][ndef],b[][ndef];
char iperm[ndef];
{register double x;
register double sum;
register int i,j,k,ipiv;
/* b[m][ndef]: matrix of right side vectors to be overwritten by
  solution 
solve L U b" = b
writing b' and b" over b 
L stored in a[j][i]: i>j; L[j][j] =1 not stored */
iperm[ndef-1]=ndef-1;
for(k=0;k<m;k++)/* operate by columns, for efficiency if m=1 */
  {
  for(j=0;j<ndef;j++)
    {
    /* exchange as was done in factor */
    x=b[k][ipiv=iperm[j]];
/* eliminating x by setting sum here OK for float OPTREAL */
    b[k][ipiv]=b[k][j];
/* multiply by L inverse */
    for(sum=i=0;i<j;i++)
      sum+=a[i][j]*b[k][i];
    b[k][j]=x-sum;};/* L inv b */
/* multiply by U inverse */
  for(j=ndef-1;j>=0;j--){
    sum=0;
    for(i=j+1;i<ndef;i++)
	sum+=a[i][j]*b[k][i];
    b[k][j]=(b[k][j]-sum)/a[j][j];};};
}
