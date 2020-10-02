/* timer1.h - this file is to be included at the front of each
 * timing sample file.
 * Note: variables visible to the sample file all start with "t_";
 * macro names all start with "T_", to avoid name conflicts.
 */
#include <stdio.h>

#define void int	/* remove if compiler supports  void  */
typedef int bool;
#define T_NO  0
#define T_YES 1
#define T_MAXITER  10000000

#define T_SKIP 0
#define T_NOASST 1
#define T_REGASST 2
#define T_DBLASST 3
#define T_LOOPDOWN(a, b) for (a = (b)+1; --a > 0;  )
#define DO_SKIP(S) \
	t_s=S; \
	t_type = T_SKIP; \
	begintim(0); \
	if (0) { 
#define DO_STMT(S) \
	t_s=S; \
	t_type = T_NOASST; \
	if (t_sample[t_try] < t_minsam) { \
	begintim(0); \
	T_LOOPDOWN(t_major, t_majrlim) T_LOOPDOWN(t_minor, t_minrlim)
#define DO_FEXPR(S) \
	t_s=S; \
	t_type = T_DBLASST; \
	if (t_sample[t_try] < t_minsam) { \
	begintim(0); \
	T_LOOPDOWN(t_major, t_majrlim) T_LOOPDOWN(t_minor, t_minrlim)  t_dbl = 
#define DO_IEXPR(S) \
	t_s=S; \
	t_type = T_REGASST; \
	if (t_sample[t_try] < t_minsam) { \
	begintim(0); \
	T_LOOPDOWN(t_major, t_majrlim) T_LOOPDOWN(t_minor, t_minrlim)  t_reg = 
#define OD ;} endtim(0);  ++t_try;
																							/*
.PE
.PS 50
/* external references */
extern char *t_s;			/* description of this try */
extern long t_reps;			/* number of samples this iteration */
extern short t_try;			/* which trial number is being done next */
extern short t_type;		/* selection of timing method */
extern double t_minsam;		/* desired minimum samples */
extern double t_sample[];	/* actual samples for each try */
extern char *t_title[];		/* array of descriptions for printout */
extern short t_size[];		/* array of code sizes in bytes */
extern double t_ctime[];	/* array of actual times */

/* static (internal) definitions */
static double t_dbl = 0;		/* target for forced double assigns */
static unsigned t_majrlim = 0;	/* major loop limit */
static unsigned t_major = 0;	/* major loop variable */
static unsigned t_minrlim = 0;	/* minor loop limit */
static unsigned t_minor = 0;	/* minor loop variable */
static void t_timeall();
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
			t_minrlim = t_reps;
			t_majrlim = 1;
			}
		else
			{
			t_minrlim = 10000;
			t_majrlim = t_reps / 10000;
			}
		t_timeall();
		t_reps *= 10;
		more = T_NO;
		for (i = 0; i < t_try; ++i)
			if (t_sample[i] < t_minsam)
				more = T_YES;
		} while (more && t_reps <= T_MAXITER);
	report();
	}
static void t_timeall()
	{
	register int t_reg;		/* target for forced assigns */
	/* ... USER'S FUNCTION CONTINUES HERE ... */
