/* fround - round double x to precision p, n significant digits
 * uses static string for result - not re-entrant
 * fround is an accomodation for K+R-level printf which lacks %.*e or %g
 * slow, fat version - uses sprintf
 */
#include <stdio.h>
char *fround(x, p, n)
	double x;
	short p;
	short n;
	{
	double y;
	double log10();
	short digs;
	short nlog;
	static char s[40] = {0};
	char fmt[20];

	sprintf(fmt, "%%.%de", n-1);
	sprintf(s, fmt, x);
	sscanf(s, "%lf", &y);
	if (y == 0)
		nlog = 0;
	else
		nlog = log10(y);
	if (nlog < 0)
		--nlog;
	digs = n - nlog - 1;
	if (digs < 0)
		digs = 0;
	else if (digs > p)
		digs = p;
	sprintf(fmt, "%%.%df", digs);
	sprintf(s, fmt, y);
	if (digs == 0)
		strcat(s, ".");
	while (digs++ < p)
		strcat(s, " ");
	return (s);
	}
#ifdef TRYMAIN
main()
	{
	short m;

	for (m = 1; m <= 5; ++m)
		printf("fround(123.57, 2, %d) = %s;\n", m, fround(123.57, 2, m));
	for (m = 1; m <= 5; ++m)
		printf("fround(.013579, 5, %d) = %s;\n", m, fround(.013579, 5, m));
	}
#endif
