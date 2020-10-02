/*
	e.c---demonstration of VLI math package.  Compute
	'e' with variable precision.  There is an arbitrary
	limit on the size of e simply because 50 digits of
	precision is all there is in my copy of "CRC Standard
	Math Tables", 25th edition.  I would also point out
	that at level 50, t1 and r are over 110 characters long,
	so if you wish to expand further, adjust #define MAX.

	build:

		n>cc e
		n>clink e ratc vli math
 
	by Hugh S. Myers

	4/1/84
	4/3/84
*/

#include <bdscio.h>
#define MAX 256
#define LIMIT 3

main()
{
	char one[MAX], s[LIMIT], r[MAX], t1[MAX], t2[MAX];
	int i, n;

	puts("e.c a program to compute 'e'\n");
	forever {
		puts("accurate to how many digits(0>n>50)? ");
		getline(s,LIMIT);
		n=atoi(s);
		if(n<1 || n>50)
			continue;	
		strcpy(one,"1.0");
		pad(one,n+10);
		strcpy(r,"2");
		strcpy(t2,"2");
		for(i=0;i<n+5;i++) {
			strcpy(t1,facl(t2));
			strcpy(t1,rdiv(one,t1));
			strcpy(r,radd(r,t1));
			strcpy(t2,incrl(t2));
			putch('.');
		}
		putch('\n');
		r[n+2]='\0';
		printf("e= %s\n",r);
	}
}