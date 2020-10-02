/* error - print error message and exit
 */
#include <stdio.h>
error(s1, s2)
	{
	fprintf(stderr, "%s %s\n", s1, s2);
	fflush(stderr);
	exit(1);
	}
