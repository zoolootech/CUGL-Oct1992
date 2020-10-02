/* groups - make groups of similar timings
 */
#include <stdio.h>
#define ABS(n) ((n) < 0 ? (-n) : (n))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) < (b) ? (b) : (a))
#define NGROUPS 25
#define GI groups[i]
struct
	{
	short n;
	double tmin, tmax;
	double total;
	short members[50];
	} groups[NGROUPS] = {0};
short igroup[NGROUPS] = {0};
short itemp = 0;
double gtemp = 0;
char s[100][24] = {0};
short ns = 0;
double t = 0;
short i = 0;
short j = 0;
short itmax = 0;
double avg = 0;
int ret = 0;
char *fround();
main()
	{

	for (i = 0; i < NGROUPS; ++i)
		GI.tmin = 9e19;
	while (getchar() != '\n')
		;	/* skip heading line */
	for (ns = 0; (ret = scanf("%22c%*d%lf", s[ns], &t)) != EOF; ++ns)
		{
		while (getchar() != '\n')
			;
#if 0
		printf("%22.22s %8.2f\n", s[ns], t);
#endif
		if (ret < 2)
			printf("bad data!!\n");
		for (i = 0; ; ++i)
			{
			if (GI.n != 0)
				avg = GI.total / GI.n;
			if (GI.n == 0 ||
					(GI.tmin <= t && t-GI.tmin < GI.tmin / 3) ||
					(t <= GI.tmax && GI.tmax-t < t / 3))
				{
				GI.total += t;
				GI.tmin = MIN(GI.tmin, t);
				GI.tmax = MAX(GI.tmax, t);
				GI.members[GI.n] = ns;
				++GI.n;
				break;
				}
			}
		}
	for (i = 0; GI.n != 0; ++i)
		{
		igroup[i] = i;
		itmax = i;
		}
	for (i = 0; i <= itmax; ++i)
		{
		itemp = igroup[i];
		gtemp = groups[itemp].tmax;
		j = i;
		while (j > 0 && groups[igroup[j-1]].tmax > gtemp)
			{
			igroup[j] = igroup[j-1];
			--j;
			}
		igroup[j] = itemp;
		}
	for (j = 0; j <= itmax; ++j)
		{
		i = igroup[j];
		printf("\nGroup %d: ", j+1);
		printf(" avg=%s", fround(GI.total/GI.n, 4, 4));
		printf(" tmin=%s", fround(GI.tmin, 3, 3));
		printf(" tmax=%s\n", fround(GI.tmax, 3, 3));
		for (ns = 0; ns < GI.n; ++ns)
			printf("\t%s\n", s[GI.members[ns]]);
		}
	}
