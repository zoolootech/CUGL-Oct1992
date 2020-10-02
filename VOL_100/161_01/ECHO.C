#include <stdio.h>
#define SUCCEED 0
/* echo - echo args
 */
main(ac, av)
	int ac;
	char *av[];
	{
	int i;

	for (i = 1; i < ac; ++i)
		printf(i < ac-1 ? "%s " : "%s\n", av[i]);
	exit(SUCCEED);
	}
