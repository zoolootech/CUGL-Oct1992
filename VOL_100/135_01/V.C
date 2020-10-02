/*
	v.c---test bed for very long integer package
	written in assembler for BDSc.  To include:


	FUNCTIONS WRITTEN:

		s	string
		n	integer
		s1,s2	string

	incrl(s)	increment a string by one.
	decrl(s)	decrement a string by one.
	saddl(s,n)	short add, add an integer to a string.
	ssubl(s,n)	short subtract, subtract an integer from a string.
	addl(s1,s2)	add string one to string two.
	subl(s1,s2)	subtract string two from string one.
	smull(s,n)	short multiply, multiply a string by an integer.
	sdivl(s,n)	short divide, divide a string by an integer.
	sdivlr(s,n)	short divide rounded, as above with rounded result.
	smodl(s,n)	short modulus, return remainder of a string divided by
			an integer.
	mull(s1,s2)	multiply string one by string two.
	divl(s1,s2)	divide string one by string two.
	divrl(s1,s2)	divide string one by string two with rounded result.
	modl(s1,s2)	return remainder of string one divided by string two.
	sqrtl(s)	square root of a string.
	sqrtlr(s)	square root of a string rounded.
	facl(s)		string factorial.
	spowl(s,n)	return string to the power of an integer.
	powl(s1,s2)	return string one to the power of string two.
	gcdl(s1,s2)	greatest common divisor of string one and string two.
	sgcd(s,n)	greatest common divisor of a string and an integer.
	lcm(s1,s2)	least common multiple of string one and string two.
	slcm(s,n)	least common multiple of a string and an integer.
	randl(n)	random number generator.  if n == 0 return r(memory)
			else return r(1).
	maxl(s1,s2)	floor function, return smaller of s1 and s2.
	minl(s1,s2)	ceiling function, return larger of s1 and s2.
	absl(s)		return absolute value of a string.

	iszero(s)	boolean truth function, test s == 0?
	isneg(s)	boolean truth function, test s < 0?
	ispos(s)	boolean truth function, test s >= 0?
	iseven(s)	boolean	truth function, test mod(s,2)=0?
	isequal(s1,s2)	boolean truth function, test s1 == s2?
	sisequal(s,n)	boolean truth function, test s == n?
	isltl(s1,s2)	boolean truth function, test s1 < s2?
	sisltl(s,n)	boolean truth function, test s < n?
	isgtl(s1,s2)	boolean truth function, test s1 > s2?
	sisgtl(s,n)	boolean truth function, test s > n?


	Current version limits are 2^1024 bits of precision.
	Usage is pretty much the obvious, except that I would ovoid
	complex function calls.  They should work since vli.crl has its
	own storage for all calls but use your own discretion.  You 
	should be aware that the limit on the size of a string that
	printf() can handle is MAXLINE in bdscio.h.  I've changed my
	version to #define MAXLINE 600 and recompiled stdlib1.c and
	stdlib2.c, then used clib to build a new deff.crl.  This seems
	to take care of the problem.  This test program provides some
	examples of vli.crl usage, so hack away.

	Hugh S. Myers

	build:
		n>cc v
		n>clink v vli math

	3/12/84
	4/9/84

*/

#include <bdscio.h>
#define MAX 256

main()
{
	char strbuf1[MAX], strbuf2[MAX];
	char *s1, *s2;
	int n;

	s1=strbuf1;
	s2=strbuf2;
	n=1;

	puts("usage: a:v<cr>\n");
	puts("       ?s1 <some number><cr>\n");
	puts("       ?s2 <some number><cr>\n");

	forever {
		printf("?s1 ");
		getline(s1,MAX);
		printf("?s2 ");
		getline(s2,MAX);

		printf("the sum of s1 and s2 is %s\n",addl(s1,s2));
		printf("the difference of s1 and s2 is %s\n",subl(s1,s2));
		printf("the product s1 and s2 is %s\n",mull(s1,s2));
		printf("the quotient of s1\\s2 is %s\n",divl(s1,s2));
		printf("the modulus of s1\\s2 is %s\n",modl(s1,s2));
		printf("the s1 to the power of the s2 is %s\n",powl(s1,s2));
		printf("the GCD s1 and s2 is %s\n",gcdl(s1,s2));
		printf("the LCM s1 and s2 is %s\n",lcml(s1,s2));
		printf("a random number is %s\n",randl(n));
		if(n)
			n = 0; /* first call to randl n should = 1
				  there after n should = 0 */

		printf("s1 factorial is %s\n",facl(s1));
		printf("the smaller of s1 and s2 is %s\n",minl(s1,s2));
		printf("the larger of s1 and s2 is %s\n",maxl(s1,s2));
		printf("the absolute value of the difference of s1 and s2 is %s\n",absl(subl(s1,s2)));
		printf("s1 > s2 is %d\n",isgtl(s1,s2));
		printf("s1 < s2 is %d\n",isltl(s1,s2));
	}
}
