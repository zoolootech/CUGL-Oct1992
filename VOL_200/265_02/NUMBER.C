#include <stdio.h>

#define MAX	1024*4

void main()
{
int i,j;

for (i = 0; i < MAX; i++)
	{
	printf ("%016x",i);
	}
}
