#include <stdio.h>
#include <time.h>

main()
{
	long ltime;

	time(&ltime);
	printf("The current date and time are %s\n", ctime(&ltime));
}
