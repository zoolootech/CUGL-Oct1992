/*
HEADER:		;
TITLE:		C elementary transcendentals;
VERSION:	1.0;

DESCRIPTION:   	Source    code    for   all   standard    C 
               transcendentals

		Employs  ldexp()  and  frexp()  functions;  if 
               suitable  versions  of these are  not  provided 
               by  a given compiler,  the versions provided in 
               source  code  will require  adaptation  to  the 
               double float formats of the compiler.

KEYWORDS:	Transcendentals, library;
SYSTEM:		CP/M-80, V3.1;
FILENAME:	TRANS.C;
WARNINGS:  	frexp()   and  ldexp()  are   implementation 
               dependent.   The  compiler  employed  does  not 
               support    minus   (-)   unary   operators   in 
               initializer  lists,  which are required by  the 
               code.
AUTHORS:	Tim Prince;
COMPILERS:	MIX C, v2.0.1;
*/
/* Copyright (C) 1986, Chandler Software,
** 4456 W. Maple Rd., Birmingham MI 48010-1923
** (313) 855-4587
** C adaptation of 1984 FORTRAN code
** prepared for non-profit distribution by C Users' Group */
/*$NESTCMNT*/
# include "a:stdio"
/* dblfmt describes floating point format
** machine dependencies in case frexp() and ldexp() are not
** supplied */
union dblfmt{
  double flt;
  struct{
    char mant[7]; /* full reverse byte order
** IEEE int sign:1;unsigned expn:11;unsigned mant:4
** with DEC reverse byte pairing, can't cross 16-bit
** boundaries */
    char expn;
  }fld;	/* char same as unsigned :8 in this C */
}
#define ROUND(x) (int)((x>=0)-.5+x) /* Pascal ROUND */
#define ODD(i) ((i)&1) /* also like Pascal */
/* teach the preprocessor some algebra */
#define PI 3.1415926535897932385
#define HPI 1.5707963267948966192
#define LN2 .69314718055994530942
#define L2E 1.442695040888963407
#define cos(x) sin(HPI-(x)) /* double sin() */
#define tan(x) sin(x)/cos(x) /* double sin() */
/* for future use
#define fabs(x) (x>=0?x:-(x))
/* double sin() */
#define atan2(x,y) (y>0?atan((x)/(y)):y==0?(x>=0?HPI:-HPI):(x>=0?PI+atan((x)/(y)):atan((x)/(y))-PI))
#define asin(x) atan2(x,sqrt((1-(x))*(1+x))) /* double sin(),sqrt() */
#define acos(x) atan2(sqrt((1-(x))*(1+x)),x) /* double sin(),sqrt() */
#define log(x) log2(x)*LN2 /* double log2() */
#define exp(x) exp2((x)*L2E) /* double exp2() */
#define cosh(x) sqrt(sinh(x)*sinh(x)+1) /* double sinh(),sqrt() */
#define tanh(x) (x<-20?-1.:(x>=20?1.:sinh(x)/cosh(x)))
*/
#define pow(x,y) exp2(log2(x)*(y))
main(){ /* test accuracy of tan,sin,cos,atan,exp2,log2 */
  double sin(),atan(),log2(),exp2();
  double x;
  for(x=1e-36;x<1e33;x*=1e4)
    printf("x:%23.16e tan(atan):%23.16e \n\tpow:%23.16e\n",
      x,tan(atan(x)),pow(x,1.));
}
double frexp(x,scale)
  int *scale;
  double x;
{
  union dblfmt x1;
  #define BIAS 128 /* exponent field of .9 (IEEE 1023) */
  x1.flt=x;
  *scale=x1.fld.expn-BIAS; /* scale by .5^(*scale) */
  x1.fld.expn=BIAS;  /* .5 <= result < 1 */
  return(x1.flt);
}
double ldexp(x,scale)
  double x;
  int scale;
{
  union dblfmt x1;
  x1.flt=x;
/* scale by 2^scale */
  x1.fld.expn+=scale;
  return(x1.flt);
}
double sin(x)
  double x;
{
  #define NTS 8
/* negative signs are ignored by some compilers! */
  static double table[NTS]={
    -.7370812e-12,.160478576e-9,-.2505187133e-7,
    .275573164289e-5,-.00019841269823293,
    .008333333333276252,-.1666666666666597,.9999999999999999};
  double poly(),pm,floor();
/* try to take care of x>maxint unless much too slow
** employ periodicity sin(x+n*PI)=(-1)^n*sin(x) */
  x-=PI*(pm=floor(x/PI+.5));
/* now |x| < HPI; use series and fix sign */
  return(poly(x*x,NTS,table)*(ODD((int)pm)?-x:x));
}
/* use fast polynomial evaluation (possibly non-portable) */
double poly(x,n,table)
  double x;
  int n;
  double table[];
{
  register double sum;
  register int i;
  sum=table[0];
/* unroll loop by pairing terms to save overhead */
  for(i=2;i<n;i+=2)
    sum=(sum*x+table[i-1])*x+table[i];
  return(i==n?sum*x+table[i-1]:sum);
}
double atan(x)
  double x;
{
  #define TNPI6 .57735026918962576451
  #define NTA 9
  static double table[NTA]={
    .0443911883527,-.06483241134718,.07679374240257,
    -.090903714847573,.111110979102203,-.1428571410307564,
    .1999999999873222,-.33333333333329944,
    .99999999999999999};
  char invert,reduce,neg;
  double poly();
/* atan(-x)=-atan(x) */
  if(neg=(x<0))x=-x;
/* tan(HPI-x)=1/tan(x) */
  if(invert=(x>=1))x=1/x;
/* tan(a-b)={tan(a)-tan(b)}/{tan(a)*tan(b)+1}; b=PI/6 */
  if(reduce=(x>=.269))x=(x-TNPI6)/(x*TNPI6+1);
  x*=poly(x*x,NTA,table);
  if(reduce)x+=.52359877559829887308;
  if(invert)x=HPI-x;
  return(neg?-x:x);
}
double log2(x)
  double x;
{
  #define NTL 7
  static double table[NTL]={
    .24291838162,.2614440423316,.3206163946133,.41219840197457,
    .57707801724629,.961796693924339,2.885390081777927};
  int scale,incsc;
  double poly(),ldexp(),frexp();
/* split x -> 2^scale*(reduced x near 1) */
  incsc=frexp(x,&scale)<.7071;
  x=ldexp(x,-(scale-=incsc));
  x=(x-1)/(x+1);
  return(poly(x*x,NTL,table)*x+scale);
}
double exp2(x)
  double x;
{
  #define INFINITY 1.7e38/*IEEE 0x7ff0000000000000*/
  #define MAXEXP 127 /* IEEE 1023 */
  #define MINLG2 -128
  double evenp,oddp,xsq,ldexp();
  int scale;
  if(x>=MAXEXP)return(INFINITY);
  if(x<MINLG2)return(0);
/* 2^x=2^ROUND(x)*2^(x-ROUND(x)) */
  x-=scale=ROUND(x);
/* approximation is ratio of polynomials differing only in
** sign of odd terms */
  xsq=x*x;
/* continued fraction approx for e^x
  (x*(15120+xsq*(420+xsq))+30240+xsq*(3360+xsq*30))/... */
/* minimax fit for 2^x, 18 significant digits */
  oddp=x*(65556.325877407443+xsq*(874.804294426022+xsq));
  evenp=189155.572484473087+xsq*(10097.515376265486+
    xsq*43.302654542155);
  return(ldexp((evenp+oddp)/(evenp-oddp),scale));
}
/* for future use
double sinh(x)
  double x;
{
  #define NTSH 7
  static double table[NTSH]={
    1.632721e-10,2.50484370e-8,2.7557344083e-6,
    1.984126975507e-4,.0083333333334753,.1666666666666579,
    1.0000000000000001};
  double poly(),exp2();
  return(fabs(x)<1?poly(x*x,NTSH,table)*x:(exp(x)-1/exp(x))/2);
}
double sqrt(x) /* as if division hardware but no sqrt */
  double x;
{ static float table[]={.59,.417,.295}; /* minimax 2 digits */
  register double x1;
  double frexp(),ldexp();
  register int i;
  register char iters=3; /* number of Newton iterations */
  register float x0; /* early stages need 2.5 digits precision */
  int scale;
  if(x<=0)return(x);
  x0=frexp(x,&scale); /* sqrt(2^x*y)=2^(x/2)sqrt(y) */
  i=ODD(scale); /* separate fits for y<.5 & y>.5 */
  x1=ldexp(x0*table[i]+table[i+1],(scale+1)>>1); /*crude sqrt*/
  do /* enough Newton iterations for full accuracy */
    x1=ldexp(x/x1+x1,-1); /* ldexp might be faster than *.5 */
  while(--iters);
  return(x1);
}
*/
