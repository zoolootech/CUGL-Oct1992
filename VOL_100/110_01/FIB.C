/*
 * Fibbonacchi series (?) benchmark run at Usenix Conference.
 * For 16 bit integers, max argument without overflow is 25.
 */

main(argc,argv)
char **argv;
{
	printf("%u\n", fib(atoi(argv[1])));
}
fib(p)
int p;
{
	if(p==0)
		return 0;
	else if(p==1)
		return 1;
	else
		return fib(p-1)+fib(p-2);
}
