/* Recursive version of Bose-Nelson sort */
/* by Mark D. Lougheed */
/* 2-SEP-85 */

#include "stdio.h"

/* #define DEBUG */


int count=0;
FILE *fp;



main(argc, argv)
int argc;
char **argv;
{
int n;


if(argc < 3)
	{
	printf("USAGE: BOSE n outfile\n");

	exit();
	}

else
	{
	n=atoi(*(++argv));

	if( ( fp=fopen(*(++argv), "w") )==0 )
		{
		printf("Error: could not open %s for writing.\n", *argv);

		exit();
		}

	p2(1,n);

	printf("There were %d swaps\n",count);

	fclose(fp);
	}
}


p1(x, y)
int x, y;
{
fprintf(fp, "swap(%d,%d);\n", x, y);

++count;
}


p2(i, x)
int i, x;
{
int a;


if(x!=1)
	{
#ifdef DEBUG
	printf("p2:  i=%d, x=%d\n", i, x);
#endif

	a=x/2;

	p2(i, a);

	p2(i+a, x-a);

	p3(i, a, i+a, x-a);
	}
}


p3(i, x, j, y)
int i, x, j, y;
{
int a, b;


#ifdef DEBUG
printf("p3:  i=%d, x=%d, j=%d, y=%d\n", i, x, j, y);
#endif

a=x/2;

if(even(x))
	{
#ifdef DEBUG
	puts("p3: x is even");
#endif
	
	b=(y+1)/2;
	}

else
	{
#ifdef DEBUG
	puts("p3: x is odd");
#endif
	b=y/2;
	}

if( (x==1) && (y==1) )
	p1(i, j);

else if( (x==1) && (y==2) )
	{
	p1(i, j+1);

	p1(i, j);
	}

else if( (x==2) && (y==1) )
	{
	p1(i, j);

	p1(i+1, j);
	}

else
	{
	p3(i, a, j, b);

	p3(i+a, x-a, j+b, y-b);

	p3(i+a, x-a, j, b);
	}
}


even(x)
int x;
{
return(1-(1&x));
}
