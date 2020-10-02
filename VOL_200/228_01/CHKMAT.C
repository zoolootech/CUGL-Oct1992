/*
HEADER:         CUGXXX;
TITLE:          Matrix mathematics checker;
DATE:           3-20-86;
DESCRIPTION:    Checks matrix functions;
KEYWORDS:       Matrix, mathematics;
FILENAME:       CHKMAT.C;
WARNINGS:       Undocumented;
AUTHORS:        Unknown;
COMPILER:       Lattice C;
REFERENCES:     US-DISK 1308;
ENDREF
*/
#include "stdio.h"
#define CDIMA 5
#define CDIMB 5
main()
{
    float a[CDIMA][CDIMA],b[CDIMA][CDIMB];
    int row,col,n,m;
    int intr1[CDIMA];
    double det;
    int i;

	printf("input n and m ");
	scanf("%d %d",&n,&m);
	for (row = 1 ; row <= n ; row++)
	{
	    for (col = 1 ; col <= n ; col++)
	    {
		printf("read in row %2d and column %2d of A ",row,col);
		scanf("%f",&a[row-1][col-1]);
	    }
	}
	for (row = 1 ; row <= n ; row++)
	{
	    for (col = 1 ; col <= m ; col++)
	    {
		printf("read in row %2d and column %2d of B ",row,col);
		scanf("%f",&b[row-1][col-1]);
	    }
	}
	printf("a is:\n");
	for (row = 1 ; row <= n ; row++)
	{
	    for (col = 1 ; col <= n ; col++)
		printf("%10.4f",a[row-1][col-1]);
	    printf("\n");
	}
	printf("b is:\n");
	for (row = 1 ; row <= n ; row++)
	{
	    for (col = 1 ; col <= m ; col++)
		printf("%10.4f",b[row-1][col-1]);
	    printf("\n");
	}

	i = mdecomp(a,n,intr1,CDIMA,&det);
	printf("det is %e, mdecomp is %d\n",det,i);
	printf("a is:\n");
	for (row = 1 ; row <= n ; row++)
	{
	    for (col = 1 ; col <= n ; col++)
		printf("%10.4f",a[row-1][col-1]);
	    printf("\n");
	}
	i = mback(b,m,CDIMB,intr1,a,n,CDIMA);
	printf("mback is %d\n",i);
	printf("b is:\n");
	for (row = 1 ; row <= n ; row++)
	{
	    for (col = 1 ; col <= m ; col++)
		printf("%10.4f",b[row-1][col-1]);
	    printf("\n");
	}
}
