/* TIMING FUNCTIONS: BEGINTIM, ENDTIM, REPORT
 */
#include <stdio.h>
#include "cputim.h"
#include "config.h"

#define MINTYPE 1			/* smallest value of t_type except SKIP */
#define MAXTYPE 3			/* largest value of t_type  */
#define SKIP 0				/* skip this excerpt */
#define SKIPTIME -999.99	/* time value for skipped sample */

extern int *returnp();

char *t_s = "";				/* descriptive string for this trial */
long t_reps = 0;			/* number of repetitions this iteration */
double t_minsam = MINSAM;	/* desired minimum sample */
double t_sample[NTRY] = {0};/* array of samples */
short t_try = 0;			/* which trial mumber is being done next */
short t_type = 0;			/* selection of timing method */
char *t_title[NTRY] = {0};	/* array of descriptions for printout */
short t_size[NTRY] = {0};	/* array of code sizes in bytes */
double t_ctime[NTRY] = {0};	/* array of actual times */

int calibrun = 0;			/* is this a calibration run? */

static double t1 = 0;		/* time needed to run one full sample (usec) */
static short thistry = -1;	/* try last done by begintim */
static int *p1 = NULL;		/* return address for the call to begintim */
static int *p2 = NULL;		/* return address for the call to endtim */

#define PARMS struct parms
PARMS
	{
	double time;
	unsigned space;
	};
PARMS testparms[3] = {0};	/* harness parms for stmt, reg-asst, dbl-asst */																				                                                  /*
.PE
.PS 45
/* BEGINTIM - START TIMING FOR ONE TRY */
begintim(arg1)
	int arg1;
	{
	int var1;			/* first auto variable */
	int *pa = &FR_ANCHOR;		/* pointer to "frame anchor" */
	static short first = 1;	/* first time */
	static long i = 0;		/* counter for timing loop */
	FILE *fpin;				/* file for reading testparms */
	if (t_try >= NTRY)
		{	
		printf("reached maximum # of tries; ignored '%s'\n", t_s);
		exit(1);
		}	
	t_title[t_try] = t_s;
	if (t_type == SKIP)
		{
		t_size[t_try] = 0;
		t_ctime[t_try] = SKIPTIME;
		return;
		}
	p1 = returnp(pa);
	thistry = t_try;
	if (first && !calibrun)
		{
		first = 0;
		fpin = fopen("parms.dat", "r");
		if (fpin == NULL)
			error("can't open parms.dat;",
				"Run  calib >parms.dat  to create parms.dat");
		while (getc(fpin) != '\n')
			;
		for (i = 0; i <= 2; ++i)
			{
			if (2 > fscanf(fpin, "%*22c %d %lf",
				&testparms[i].space, &testparms[i].time))
				error("bad data in", "parms.dat");
			while (getc(fpin) != '\n')
				;
			}
		fclose(fpin);
		}
	cputim();
		}																				                                                  /*
.PE
.PS 25
/* ENDTIM - COMPUTE TIMES FOR ONE TRIAL */
endtim(arg1)
	int arg1;
	{
	int var1;				/* first auto variable */
	int *pa = &FR_ANCHOR;	/* pointer to "frame anchor" */

	t1 = cputim() / ((double)CLOCK_TICKS_PER_SECOND/1e6);
	if (thistry != t_try)
		return;
	p2 = returnp(pa);
	if (t_type < MINTYPE || MAXTYPE < t_type)
		{
		fprintf(stderr, "wrong t_type: %d; t_try=%d\n", t_type, t_try);
		t_size[t_try] = 0;
		t_ctime[t_try] = SKIPTIME;
		}
	else
		{
		t_size[t_try] = (char *)p2 - (char *)p1 - testparms[t_type-1].space;
		t_ctime[t_try] = t1 / (double)t_reps - testparms[t_type-1].time;
		}
	t_sample[t_try] = t_ctime[t_try] * t_reps;
	}																				                                                  /*
.PE
.PS 22
/* REPORT - PRINT TIMES */
report()
	{
	char *fround();	/* function to round output values */
	short i;		/* counter for printout loop */

	printf("%-20s  %6s  %12s\n",
		"Code sample", "Size", "Time (usec)");
	for (i = 0; i < t_try; i++) 
		{
		if (t_ctime[i] == SKIPTIME)
			printf("%-20.20s  %6s  %12s\n", t_title[i], "-", "-");
		else
			{
			if (t_ctime[i] < 0.)
				t_ctime[i] = 0.;
			printf("%-20.20s  %6d  %12s\n",
				t_title[i], t_size[i], 
				fround(t_ctime[i], 3, 3));
			}
		}
	}
