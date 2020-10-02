/* avg - compute averages of cpu time
 */
#include <stdio.h>
#include <math.h>
#define NOPRINT 0
#define PRINT 1
#define NAME_MAX 64
main(ac, av)
	int ac;
	char *av[];
	{
	char fname[NAME_MAX];
	double atof();
	double avg;
	double favg();
	double trial;

	if (ac != 3)
		error("usage: avg filename trial", "");
	strcpy(fname, av[1]);
	trial = atof(av[2]);
	avg = favg(fname, trial, 5., NOPRINT);
	avg = favg(fname, avg, 4., NOPRINT);
	avg = favg(fname, avg, 3., NOPRINT);
	avg = favg(fname, avg, 3., PRINT);
	}
/* favg - compute avg of all entries within factor of prev avg
 */
double favg(fname, prev, factor, doprint)
	char fname[];	/* file name */
	double prev;	/* previous average */
	double factor;	/* acceptance factor */
	int doprint;	/* print resulting table? */
	{
	char buf[100];
	char timestr[20];
	double avg;	/* average of selected samples */
	double fit;	/* how close is sample */
	double sum;	/* sum of samples */
	double x;
	short n;	/* number of samples */
	FILE *fp;
	FILE *fopen();

	sum = 0.;
	n = 0;
	fp = fopen(fname, "r");
	if (fp == NULL)
		error("can't open", fname);
	fgets(buf, sizeof(buf), fp);	/* skip heading */
	while (fgets(buf, sizeof(buf), fp))
		{
		sscanf(&buf[29], "%s", timestr);
		if (strcmp(timestr, "-") == 0 || sscanf(&buf[29], "%lf", &x) < 1)
			continue;
		fit = x / prev;
		if (fit < 1. && fit != 0.)
			fit = 1/fit;
		if (fit < factor && fit != 0.)
			{
			sum = sum + x;
			++n;
			}
		else if (doprint)
			{
			printf("%29.29s %12s\n", buf, fround(x, 3, 3));
			}
		}
	if (n > 0)
		avg = sum / n;
	if (doprint)
		printf("%28.28s %12s\n",
			"Average of other samples", fround(avg, 3, 3));
	fclose(fp);
	return (avg);
	}
