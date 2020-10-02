/* alutyp - run timing for all variable types (unary ops)
 */
#include "config.h"

double ed0 = 0, ed1 = 63, ed2 = 15;
float ef0 = 0, ef1 = 63, ef2 = 15;
char ec0 = 0, ec1 = 63, ec2 = 15;
short es0 = 0, es1 = 63, es2 = 15;
long el0 = 0, el1 = 63, el2 = 15;
ushort eus0 = 0, eus1 = 63, eus2 = 15; 
static double sd0 = 0, sd1 = 63, sd2 = 15;
static float sf0 = 0, sf1 = 63, sf2 = 15;
static char sc0 = 0, sc1 = 63, sc2 = 15;
static short ss0 = 0, ss1 = 63, ss2 = 15;
static long sl0 = 0, sl1 = 63, sl2 = 15;
#ifdef UTINY
 utiny euc0 = 0, euc1 = 63, euc2 = 15; 
 static utiny suc0 = 0, suc1 = 63, suc2 = 15; 
#endif
static ushort sus0 = 0, sus1 = 63, sus2 = 15; 
#ifdef ULONG
 ulong eul0 = 0, eul1 = 63, eul2 = 15; 
 static ulong sul0 = 0, sul1 = 63, sul2 = 15; 
#endif

/* #include "timer1.h" */
#include <stdio.h>

typedef int bool;
#define NO  0
#define YES 1
#define MAXITER  10000000

#define SKIP 0
#define NOASST 1
#define REGASST 2
#define DBLASST 3
#define CDOWN(a, b) for (a = (b)+1; --a > 0;  )
#define DO_SKIP(S) \
	t_s=S; \
	t_type = SKIP; \
	begintim(0); \
	if (0) { 
#define DO_STMT(S) \
	t_s=S; \
	t_type = NOASST; \
	if (t_sample[t_try] < t_minsam) { \
	begintim(0); \
	CDOWN(t_major, t_majrtry) CDOWN(t_minor, t_minrtry)
#define DO_FEXPR(S) \
	t_s=S; \
	t_type = DBLASST; \
	if (t_sample[t_try] < t_minsam) { \
	begintim(0); \
	CDOWN(t_major, t_majrtry) CDOWN(t_minor, t_minrtry)  t_dbl = 
#define DO_IEXPR(S) \
	t_s=S; \
	t_type = REGASST; \
	if (t_sample[t_try] < t_minsam) { \
	begintim(0); \
	CDOWN(t_major, t_majrtry) CDOWN(t_minor, t_minrtry)  t_reg = 
#define OD ;} endtim(0);  ++t_try;

extern char *t_s;			/* description of this try */
extern long t_reps;			/* number of samples this iteration */
extern short t_try;			/* which trial number is being done next */
extern short t_type;			/* selection of timing method */
extern double t_mintry;		/* min samples this iteration */
extern double t_minsam;		/* desired minimum samples */
extern double t_sample[];	/* actual samples for each try */

static double t_dbl = 0;			/* target for forced double assigns */
static unsigned t_majrtry = 0;	/* major loop limit */
static unsigned t_major = 0;		/* major loop variable */
static unsigned t_minrtry = 0;	/* minor loop limit */
static unsigned t_minor = 0;		/* minor loop variable */
static void timeall();
main()
	{
	bool more;
	short i;

	t_reps = 1;
	do
		{
		t_try = 0;
		if (t_reps <= 10000)
			{
			t_minrtry = t_reps;
			t_majrtry = 1;
			}
		else
			{
			t_minrtry = 10000;
			t_majrtry = t_reps / 10000;
			}
		timeall();
		t_reps *= 10;
		more = NO;
		for (i = 0; i < t_try; ++i)
			if (t_sample[i] < t_minsam)
				more = YES;
		} while (more && t_reps <= MAXITER);
	report();
	}
static void timeall()
	{
	register int t_reg;		/* target for forced assigns */
/* end of "timer1.h" */
double ad0 = 0, ad1 = 63, ad2 = 15;
float af0 = 0, af1 = 63, af2 = 15;
char ac0 = 0, ac1 = 63, ac2 = 15;
short as0 = 0, as1 = 63, as2 = 15;
long al0 = 0, al1 = 63, al2 = 15;
#ifdef UTINY
 utiny auc0 = 0, auc1 = 63, auc2 = 15; 
#endif
ushort aus0 = 0, aus1 = 63, aus2 = 15; 
#ifdef ULONG
 ulong aul0 = 0, aul1 = 63, aul2 = 15; 
#endif
register int ri1 = 63, ri2 = 15;

#ifndef NODOUBLE
DO_FEXPR("a   f")	OP(af2);	OD;
DO_FEXPR("a   d")	OP(ad2);	OD;
DO_FEXPR("s   f")	OP(sf2);	OD;
DO_FEXPR("s   d")	OP(sd2);	OD;
DO_FEXPR("e   f")	OP(ef2);	OD;
DO_FEXPR("e   d")	OP(ed2);	OD;
#endif
DO_IEXPR("e   c")	OP(ec2);	OD;
DO_IEXPR("e   s")	OP(es2);	OD;
DO_IEXPR("e   l")	OP(el2);	OD;
#ifdef UTINY
 DO_IEXPR("e u c")	OP(euc2);	OD; 
#endif
DO_IEXPR("e u s")	OP(eus2);	OD;
#ifdef ULONG
 DO_IEXPR("e u l")	OP(eul2);	OD; 
#endif
DO_IEXPR("s   c")	OP(sc2);	OD;
DO_IEXPR("s   s")	OP(ss2);	OD;
DO_IEXPR("s   l")	OP(sl2);	OD;
#ifdef UTINY
 DO_IEXPR("s u c")	OP(suc2);	OD; 
#endif
DO_IEXPR("s u s")	OP(sus2);	OD;
#ifdef ULONG
 DO_IEXPR("s u l")	OP(sul2);	OD; 
#endif
DO_IEXPR("a   c")	OP(ac2);	OD;
DO_IEXPR("a   s")	OP(as2);	OD;
DO_IEXPR("a   l")	OP(al2);	OD;
#ifdef UTINY
 DO_IEXPR("a u c")	OP(auc2);	OD; 
#endif
DO_IEXPR("a u s")	OP(aus2);	OD;
#ifdef ULONG
 DO_IEXPR("a u l")	OP(aul2);	OD; 
#endif
DO_IEXPR("r   i")	OP(ri2);	OD;
}
