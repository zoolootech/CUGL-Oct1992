/*
	prime.c---random prime number generator.  This program
	uses Algorithm P(Probabilistic primality test).  From
	"Seminumerical Algorithms: The Art of Computer Programming",
	Vol. 2, by D. E. Knuth, page 379. This version for BDSc...

	by Hugh S. Myers

	build:
		n>cc prime
		n>clink prime ratc vli math

	3/17/84
	4/9/84
*/

#include <bdscio.h>
#define MAX 256

main()
{
	char n[MAX], limit[MAX];
	int i, p;

	puts("prime.c a random prime number generator\n");
	while(i<1 || i>100) {
		puts("number of digits to start with (for 0 < n < 100)? ");
		getline(n,MAX);
		i=atoi(n);
	}
	strcpy(n,randl(0));
	while(strlen(n)<i)
		strcat(n,randl(0));
	strcpy(limit,"1");
	pad(limit,i);
	strcpy(n,modl(n,limit));
	if(iseven(n))
		strcpy(n,incrl(n));
	forever {
		puts(n);
		for(i=0;i<10;i++) {
			p=primal(n);
			if(!p)
				break;
		}
		if(p) {
			puts(" is prime\n");
		}
		else
			puts(" is not prime\n");
		strcpy(n,saddl(n,2));
	}
}

int primal(n)
char *n;
{
	char k[MAX], q[MAX], y[MAX], x[MAX], j[MAX], n1[MAX];
	int i;	
/*
	do the easy ones first
*/
	i=qprime(n);
	if(!i)
		return FALSE;
	if(i==1)
		return TRUE;
/*
	now do it the hard way
*/
	strcpy(n1,decrl(n));
	strcpy(q,n1);
	strcpy(k,"0");
	while(iseven(q)) {
		strcpy(q,sdivl(q,2));
		strcpy(k,incrl(k));
	}
	strcpy(j,"0");
	do {
		strcpy(x,randl(0));
		strcpy(x,modl(x,n));
	} while (strcmp(x,"2") < 0);
	strcpy(y,modp(x,q,n));
	forever {
		if((iszero(j) && sisequal(y,1)) || isequal(y,n1))
			return TRUE;
		if(!iszero(j) && sisequal(y,1))
			return FALSE;
		strcpy(j,incrl(j));
		if(strcmp(j,k) < 0)
			strcpy(y,modp(y,"2",n));
		else
			return FALSE;
	}
}