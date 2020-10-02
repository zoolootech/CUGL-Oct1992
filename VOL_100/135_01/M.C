/*
	m.c---an implementation of Fermat's little theorem
	as a practical test of primality for microcomputers.
	In a nutshell, if N is a prime number and B is any
	whole number, then B^N - B is a multiple of N, or
	stated another way, mod(B^N-B,N) is congruent to 0 if N
	is prime.  This does not guarentee that N is prime (the
	existance of pseudoprimes and carmichael numbers precludes
	that) but if carried out on a random selection of numbers
	in the range of 2...N-1 passage of each test should allow
	reasonable confidence of primality.  If we test say
	100 numbers in this range then the chance that N is not
	prime is 1/2^100, an exceedingly small number indeed!

	by Hugh S. Myers

	build:
		n>cc m
		n>clink m vli math

	3/13/84
	4/3/84
*/

#include <bdscio.h>
#define MAX 256

main()
{
	char N[MAX], B[MAX], r[MAX];

        printf("test for mod(B^N-B,N) === 0\n");
	forever {
		printf("?B ");
		getline(B,MAX);
		printf("?N ");
		getline(N,MAX);
	
		strcpy(r,modp(B,N,N));
		printf("mod(B^N,N) = %s\n",r);
		if(isequal(r,B))
			printf("N is possibly prime\n");
		else {
			strcpy(r,subl(r,B));
			strcpy(r,modl(r,N));
			if(iszero(r))
				printf("N is possibly prime\n");
			else
				printf("N is not a prime\n");
		}
	}
}

/*
	char *modp(s1,s2,s3) return mod(s1^s2,s3) where s1^s2 is
	based on Algorithm A of "Seminumerical Algorithms: The
	Art of Computer Programming", Vol. 2, second edition, by
	D.E. Knuth.
*/
char *modp(s1,s2,s3)
char *s1, *s2, *s3;
{
	char y[MAX], z[MAX], n[MAX], M[MAX];
	int odd;

	strcpy(y,"1");
	strcpy(z,s1);
	strcpy(n,s2);
	strcpy(M,s3);

	forever {
		odd = (!iseven(n));
		strcpy(n,sdivl(n,2));
		if (odd) {
			strcpy(y,mull(y,z));
			strcpy(y,modl(y,M));
			if (iszero(n)) 
				return y;
		}
		strcpy(z,mull(z,z));
		strcpy(z,modl(z,M));
	}
}
