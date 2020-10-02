###benchlng.c
/* benchlng - benchmark for  long  integers 
 * Thomas Plum, Plum Hall Inc, 609-927-3770
 * If machine traps overflow, use an  unsigned  type 
 * Let  T  be the execution time in milliseconds
 * Then  average time per operator  =  T/major  usec
 * (Because the inner loop has exactly 1000 operations)
 */
#define STOR_CL auto
#define TYPE long
#include <stdio.h>
main(ac, av)
	int ac;
	char *av[];
	{
	STOR_CL TYPE a, b, c;
	long d, major, atol();
	static TYPE m[10] = {0};

	major = atol(av[1]);
	printf("executing %ld iterations\n", major);
	a = b = 1;
	for (d = 1; d <= major; ++d)
		{
		/* inner loop executes 1000 selected operations */
		for (c = 1; c <= 40; ++c)
			{
			a = a + b + c;
			b = a >> 1;
			a = b % 10;
			m[a] = a;
			b = m[a] - b - c;
			a = b == c;
			b = a | c;
			a = !b;
			b = a + c;
			a = b > c;
			}
		}
	printf("a=%d\n", a);
	}
###benchfn.c
/* benchfn - benchmark for function calls
 * Thomas Plum, Plum Hall Inc, 609-927-3770
 * Let  T  be the execution time in milliseconds
 * Then  average time per operator  =  T/major  usec
 * (Because the inner loop has exactly 1000 operations)
 */
#include <stdio.h>
extern int dummy = 0;

/* f3 - lowest level function
 * Put this in separate source file if compiler detects and optimizes
 * useless code
 */
f3() { }

f2() { f3();f3();f3();f3();f3();f3();f3();f3();f3();f3();} /* 10 */
f1() { f2();f2();f2();f2();f2();f2();f2();f2();f2();f2();} /* 10 */
f0() { f1();f1();f1();f1();f1();f1();f1();f1();f1();} /* 10 */

main(ac, av)
	int ac;
	char *av[];
	{
	long d, major, atol();

	major = atol(av[1]);
	printf("executing %ld iterations\n", major);
	for (d = 1; d <= major; ++d)
		f0(); /* executes 1000 calls */
	printf("dummy=%d\n", dummy);
	}
###benchmul.c
/* benchmul - benchmark for  int multiply
 * Thomas Plum, Plum Hall Inc, 609-927-3770
 * If machine traps overflow, use an  unsigned  type 
 * Let  T  be the execution time in milliseconds
 * Then  average time per operator  =  T/major  usec
 * (Because the inner loop has exactly 1000 operations)
 */
#define STOR_CL auto
#define TYPE int
#include <stdio.h>
main(ac, av)
	int ac;
	char *av[];
	{
	STOR_CL TYPE a, b, c;
	long d, major, atol();
	static TYPE m[10] = {0};

	major = atol(av[1]);
	printf("executing %ld iterations\n", major);
	a = b = 1;
	for (d = 1; d <= major; ++d)
		{
		/* inner loop executes 1000 selected operations */
		for (c = 1; c <= 40; ++c)
			{
			a = 3 *a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a; /* 25 * */
			}
		}
	printf("a=%d\n", a);
	}
###time-cmd.bat
time <cr-lf
%1 0
time <cr-lf
%1 10000
time <cr-lf
###cr-lf.

###run-all.sh
c -o benchfn benchfn.c
time-cmd benchfn >benchfn.out
c -o benchmul benchmul.c
time-cmd benchmul >benchmul.out
c -o benchlng benchlng.c
time-cmd benchlng >benchlng.out
c -o benchsho benchsho.c
time-cmd benchsho >benchsho.out
c -o benchreg benchreg.c
time-cmd benchreg >benchreg.out
###benchsho.c
/* benchsho - benchmark for  short  integers 
 * Thomas Plum, Plum Hall Inc, 609-927-3770
 * If machine traps overflow, use an  unsigned  type 
 * Let  T  be the execution time in milliseconds
 * Then  average time per operator  =  T/major  usec
 * (Because the inner loop has exactly 1000 operations)
 */
#define STOR_CL auto
#define TYPE short
#include <stdio.h>
main(ac, av)
	int ac;
	char *av[];
	{
	STOR_CL TYPE a, b, c;
	long d, major, atol();
	static TYPE m[10] = {0};

	major = atol(av[1]);
	printf("executing %ld iterations\n", major);
	a = b = 1;
	for (d = 1; d <= major; ++d)
		{
		/* inner loop executes 1000 selected operations */
		for (c = 1; c <= 40; ++c)
			{
			a = a + b + c;
			b = a >> 1;
			a = b % 10;
			m[a] = a;
			b = m[a] - b - c;
			a = b == c;
			b = a | c;
			a = !b;
			b = a + c;
			a = b > c;
			}
		}
	printf("a=%d\n", a);
	}
###mkwsl.bat
c -dlistcs -dlo -o benchmul benchmul.c
command /c time-cmd benchmul >benchmul.out
c -dlistcs -dlo -o benchlng benchlng.c
command /c time-cmd benchlng >benchlng.out
c -dlistcs -dlo -o benchsho benchsho.c
command /c time-cmd benchsho >benchsho.out
c -dlistcs -dlo -o benchreg benchreg.c
command /c time-cmd benchreg >benchreg.out
c -dlistcs -dlo -o benchfn benchfn.c
command /c time-cmd benchfn >benchfn.out
###mklattic.bat
command /c lcc benchreg
command /c time-cmd benchreg >benchreg.out

command /c lcc benchsho
command /c time-cmd benchsho >benchsho.out

command /c lcc benchlng
command /c time-cmd benchlng >benchlng.out

command /c lcc benchfn
command /c time-cmd benchfn >benchfn.out

command /c lcc benchmul
command /c time-cmd benchmul >benchmul.out

###EOF
