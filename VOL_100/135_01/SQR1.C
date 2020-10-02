/*
	sqr1.c---yet another example program for vli.crl and
	math.crl.  This one is from the April l984 Scientific
	American's Computer Recreations column by Fred Gruenberger
	titled, "How to handle numbers with thousands of digits,
	and why one might want to"...nice huh!  This program
	is the case of 1.0000001^134217728 to 50 digits of precision.

	by Hugh S. Myers

	build:
		n>cc sqr1
		n>clink	sqr1 vli math ratc

	4/9/84
	4/9/84

*/

#include <bdscio.h>
#define MAX 256
#define LIMIT 50

main()
{
	char n[MAX];
	int i;

	strcpy(n,"1.0000001");
	for(i=0;i<28;i++) {
		printf("%d %s\n",i+1,n);
		strcpy(n,rmul(n,n));
		if(strlen(n)>LIMIT)
			n[LIMIT]='\0'; 
/*
	 some sort of precision limit is
	 required else overflow is guarenteed
	 in the seventh iteration
 */
	}
}