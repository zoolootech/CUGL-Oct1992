/*
HEADER:		;
TITLE:		C elementary transcendentals;
VERSION:	1.1;

DESCRIPTIONº   	Sourcå    codå    foò   alì   standarä    Ã 
               transcendentals; revision of CUG 187

		Employó  ldexp(©  anä  frexp(©  functions»  iæ 
               suitablå  versionó  oæ theså arå  noô  provideä 
               bù  á giveî compiler¬  thå versionó provideä iî 
               sourcå  codå  wilì requirå  adaptatioî  tï  thå 
               doublå floaô formató oæ thå compiler.

KEYWORDS:	Transcendentals, library;
SYSTEM:		CP/M-80, V3.1;
FILENAME:	TRNS1.C;
WARNINGSº  	frexp(©   anä  ldexp(©  arå   implementatioî 
               dependent®   Thå  compileò  employeä  doeó  noô 
               supporô    minuó   (-©   unarù   operatoró   iî 
               initializeò  lists¬  whicè arå requireä bù  thå 
               code.
AUTHORS:	Tim Prince;
COMPILERS:	MIX C, v2.0.1; Eco C 3.45; Aztec C 1.06B
*/
/* Copyright (C) 1986, Chandler Software,
** 4456 W. Maple Rd., Birmingham MI 48010-1923
** (313) 855-4587
** prepared for non-profit distribution by C Users' Group
** revised again 17.V.86 and 12.X.86
** all functions work in MIX Z80 C except sin(), atan()
** To fix sin() and atan(), CONVERT the .MIX file to ASCII,
** replace the minus signs on the table[] constants, adding 1
** to the leading hex numbers which give the number of ASCII
** characters in each constant.
** Eco C requires the negative bit to be set in the DEFW
** fields of the .MAC file.  See the normal source version for
** the initializer list constants which are negative.
** All functions work in Aztec C except frexp() and ldexp(),
** which are already present and working in m.lib.
** Eco-C version, uses Eco-C constants and dint() instead of
** floor() */
#define ODD(i) ((i)&1) /* also like Pascal */
#define ECOC 1 /* Ecosoft has some special definitions in S0 */
#ifdef ECOC
#define HALF _inv2
#define SR5 _sqrt5
#define INFINITY _FPMAX /*IEEE 0x7ff0000000000000*/
#define FRND(x,y) dint(x+(y>=0)-HALF) /* no floor() */
#define PI _pi
#define HPI _pi_2
#define ONE _FPONE
extern double PI,HPI,HALF,SR5,INFINITY,ONE,dint();
#else
#define HALF .5
#define SR5 .7071
#define ONE 1
#define INFINITY 1.7e38
#define FRND(x,y) floor(x+.5)
extern double floor();
#define PI 3.1415926535897932385
#define HPI 1.5707963267948966192
#endif
#define ROUND(x) (int)((x>=0)-HALF+x) /* Pascal ROUND */
/* teach the preprocessor some algebra */
#ifdef FULLDEFS
#define cos(x) sin(HPI-(x)) 
#define fabs(x) (x>=0?x:-(x))
#define MAXLN 88 /* log(overflow theshold) */
#define LN2 .69314718055994530942
#define L2E 1.442695040888963407
#define atan2(x,y) (y>0?atan((x)/(y)):y==0?x>=0?HPI:-HPI\
  :x>=0?PI+atan((x)/(y)):atan((x)/(y))-PI)
#define acos(x) (HPI-asin(x))
#define log(x) log2(x)*LN2 /* double log2() */
#define exp(x) exp2((x)*L2E) /* double exp2() */
#define cosh(x) sqrt(sinh(x)*sinh(x)+ONE)/* x>=20?fabs(sinh(x)) */
#define tanh(x) (x<-20?-1:x>=20?ONE:sinh(x)/cosh(x))
#endif
#define pow(x,y) exp2(log2(x)*(y))
main(){ /* test accuracy of tan,sin,cos,atan,exp2,log2 */
  double tan(),log2(),exp2(),asin();
  double x,ang;
  for(x=1e-36;x<1e33;x*=1e4){
    ang=atan(x);
    printf("x:%23.16e tan(ang):%23.16e\n ang:%23.16e\n asin(sin(ang)):%23.16e\n pow:%23.16e\n",
      x,tan(ang),ang,asin(sin(ang)),pow(x,ONE));
  }
}
/* dblfmt describes floating point format
** machine dependencies in case frexp() and ldexp() are not
** supplied (Aztec has their own version, which works) */
struct dblfmt{
/* MIX C like Microsoft format */
#ifndef ECOC
  char mant[7]; /* full reverse byte order */
#endif
/* IEEE int sign:1;unsigned expn:11;unsigned mant:4
** with DEC reverse byte pairing, can't cross 16-bit
** boundaries */
  char expn;
#ifdef ECOC
  char mant[7]; /* forward byte order: Ecosoft */
#endif
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
double ldexp(x,scale)
  double x;
  char scale;
{
  ((struct dblfmt *)&x)->expn+=scale; /* return x*2**scale */
  return(x);
}
/* use fast polynomial evaluation (possibly non-portable) 
** loop unrolling or odd & even summing or such strategies
** may be faster on fast floating point machines */
double poly(x,n,table)
  double x;
  char n;
  double *table;
{
  register double sum;
  sum=*table++;
  while(--n)
    sum=sum*x+*table++;
  return(sum);
}
double asin(x) double x;
{ double sqrt();
  return(x>=ONE?HPI:(x<=-1?-HPI:atan(x/sqrt(ONE-x*x))));
}
double tan(x) double x;
{ double xs;
  x-=FRND(x/PI,x)*PI; /* if your hardware prefers, change sign*/
  xs=x*x;
/* minimax relative error fit obtained by Ruzinski program 
** if sign was changed above, change here to get correct result*/
  return(x*(33281902.4774370361+xs*(xs*(
    131095.960756651362+xs*(xs-968.863630016633889))
    -4572612.25618456766))/
    (33281902.4774370360+xs*(xs*(915702.213319541242+xs*(
    xs*44.4083430808097903-13491.8003635866138))
    -15666579.7486635766)));
}
double log2(x)  double x;
{
  #define NTL 7
/* Chebyshev fit to log2(x)*(1+x)/(1-x) using Taylor series
** expansion and then reverting to economized polynomial 
** this series has 2% less absolute error than a minimax
** relative error fit, without increasing relative error */
  static double table[]={
    .2429264,.261443335,.3206164184,
    .412198401587,.5770780172495,.961796693924327,
    2.8853900817779273};
  int scale,incsc;
  double poly();
/* split x -> 2^scale*(reduced x near 1) */
  incsc=frexp(x,&scale)<SR5;
  x=ldexp(x,-(scale-=incsc));
/* (x-.5)-.5 recommended for inaccurate arithmetic but it may
**  not help; then you might as well use x=frexp(x,&scale)
**  and x=(x-sqrt(.5))/(x+sqrt(.5)), poly()*x-.5+scale
** with sqrt(.5) reduced for best results (reduce by 2 ULPs
** from correctly rounded value on Harris Hnnn) */
  x=(x-ONE)/(x+ONE);
  return(poly(x*x,NTL,table)*x+scale);
}
double exp2(x)  double x;
{
  #define MAXEXP 127 /* IEEE 1023 */
  #define MINLG2 -128
  double xsq;
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
/* minimax absolute error fit for 2^x, 18 significant digits */
  x*=65556.325877407443+xsq*(874.804294426022+xsq);
  xsq=189155.572484473087+xsq*(10097.515376265486+
    xsq*43.302654542155);
  return(ldexp((x+xsq)/(xsq-x),scale));
}
#ifdef FULLDEFS
double sinh(x)  double x;
{
  #define NTSH 7
  static double table[NTSH]={
/* Chebyshev fit to sinh(x)/x has 2% less absolute error than
** minimax relative error fit */
    1.632881e-10,2.50483893e-8,2.7557344615e-6,
    1.984126975233e-4,.0083333333334816,.1666666666666574,
    1.0000000000000001};
  double poly(),exp2() /*,absx*/;
  if(( /*absx=*/ fabs(x))<ONE)return(poly(x*x,NTSH,table)*x);
/*  if(absx<MAXLN){ */
    x=exp(x);
    return(ldexp(x-ONE/x,-1));/*}
  return(x>0?exp(x-LN2):-exp(LN2-x));*/
/* good enough for unusual case */
}
#endif
double sqrt(x) /* as if division hardware but no sqrt */
  double x;
{ static float table[]={.59,.417,.295}; /* minimax 2 digits */
  register double x1;
  register int i;
  register char iters=3; /* number of Newton iterations */
  int scale;
  if(x<=0)return(x); /* set range error ? */
  x1=frexp(x,&scale); /* sqrt(2^x*y)=2^(x/2)sqrt(y) */
  i=ODD(scale); /* separate fits for y<.5 & y>.5 */
  x1=ldexp(x1*table[i]+table[i+1],(scale+1)>>1); /*crude sqrt
** if there is a crude firmware sqrt use that instead, then
** iters may be reduced */
  do /* enough Newton iterations for full accuracy */
    x1=ldexp(x/x1+x1,-1); /* ldexp might be faster than *.5 
** if division doesn't round up, add .5 ULP between last
** division and addition (tested on Honeywell DPS8) */
  while(--iters);
  return(x1);
}
/* #define ECOC 1 */
#ifdef ECOC
#definå TMR³ _tsqr3
extern double TMR3;
#else
#define TMR3 .268 
#endif
double sin(x)  double x;
{
#ifndef ECOC
  #define NTS 8
/* negative signs are ignored by some compilers! */
  static double table[NTS]={
/* put minus signs in output file if compiler can't 
** Chebyshev fit to sin(x)/x gives relative error from 25%
** less (in middle) to 20% more (at ends of interval) than
** minimax relative error fit, but coefficients are closer to
** Taylor series */
    -.7374418e-12,.160481709e-9,-.25051882036e-7,
    .2755731661057e-5,-.00019841269824875,
    .00833333333328281,-.1666666666666607,.9999999999999999};
#else
  #define NTS 9
/* note omitted - signs ! */
  static double table[NTS]={.27215822e-14,
    .7643027274e-12,.160589409072e-9,.25052106891169e-7,
    .2755731921123654e-5,.00019841269841208719,
    .00833333333333318652,.1666666666666666531,
    .9999999999999999998};
#endif
/* the corresponding minimax relative error coefficients are:
** -.7370812e-12,.160478576e-9,-.25051871327e-7,
** .2755731642894e-5,-.00019841269823293,.00833333333327625,
** -.1666666666666597,.9999999999999999 (16 digits)
** .27205187e-14,-.7642921847e-12,.160589366594e-9,
** -.25052106802022e-7,.2755731921020009e-5,
** -.00019841269841202181,.00833333333333316634e-2,
** -.166666666666666507,.9999999999999999997 (18.5 digits) */
  double pm;
/* try to take care of x>maxint unless much too slow
** employ periodicity sin(x+n*PI)=(-1)^n*sin(x) */
  x-=PI*(pm=FRND(x/PI,x)); /* here it may be better to change
** sign; now |x| < HPI; use series and fix sign */
  return(poly(x*x,NTS,table)*(ODD((int)pm)?-x:x));
}
double atan(x) double x;
{
  #define TNPI6 .57735026918962576451
/* on Harris Hnnn, better results are obtained by reducing
** TNPI6 by 1 ULP to compensate for inaccurate arithmetic */
  #define NTA 9
  static double table[]={
/* if compiler doesn't support initializers, must edit output*/
#ifndef ECOC
/* Chebyshev fit to atan(x)/x gives 2% less to .4% more error
** than minimax relative error fit */
   .04438671796,-.064831131188,.07679359305498,
    -.090903705713302,.111110978788314,-.1428571410247392,
    .1999999999872629,-.33333333333329920,
#else /* signs omitted to satisfy Eco C compiler */
    .04438671796,.064831131188,.07679359305498,
    .090903705713302,.111110978788314,.1428571410247392,
    .1999999999872629,.33333333333329920,
#endif
    .99999999999999998};
  char invert,reduce,neg;
/* atan(-x)=-atan(x) */
  if(neg=(x<0))x=-x;
/* tan(HPI-x)=1/tan(x) */
  if(invert=(x>=ONE))x=ONE/x;
/* tan(a-b)={tan(a)-tan(b)}/{tan(a)*tan(b)+1}; b=PI/6 */
  if(reduce=(x>=TMR3))x=(x-TNPI6)/(x*TNPI6+ONE);
  x*=poly(x*x,NTA,table);
  if(reduce)x+=.52359877559829887308;
  if(invert)x=HPI-x;
  return(neg?-x:x);
}
