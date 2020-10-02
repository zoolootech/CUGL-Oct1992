/*
	p.c---Algorithm P(Probabilistic primality test).  From
	"Seminumerical Algorithms: The Art of Computer Programming",
	Vol. 2, by D. E. Knuth, page 379. This version for BDSc...

	by Hugh S. Myers

	build:
		n>cc p
		n>clink p vli math
 
	3/17/84
	4/3/84
*/

#include <bdscio.h>
#define MAX 256

main()
{
	char n[MAX], n1[MAX], k[MAX], q[MAX], y[MAX], x[MAX], j[MAX];

	printf("Algorithm P prime test\n");
	forever {
		printf("?n ");
		getline(n,MAX);
		if(iseven(n) && (!isequal(n,"2"))) {
			printf("except for 2 all primes are odd!\n");
			continue;
		}
		if(isequal(n,"2")) {
			printf("n is definately prime!\n");
			continue;
		}
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
		} while (sisltl(x,2));
		strcpy(y,modp(x,q,n));
		forever {
			if((iszero(j) && sisequal(y,1)) || isequal(y,n1)) {
				printf("%s is probably prime\n",n);
				break;
			}
			if(!iszero(j) && sisequal(y,1)) {
				printf("%s is not prime\n",n);
				break;
			}
			strcpy(j,incrl(j));
			if(isltl(j,k))
				strcpy(y,modp(y,"2",n));
			else {
				printf("%s is not prime\n",n);
				break;
			}
		}
	}
}
