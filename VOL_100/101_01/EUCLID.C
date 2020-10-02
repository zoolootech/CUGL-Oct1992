/* This is an implementation of the algorithm given
in Knuth Vol #1 describing a solution (by Euclid) to the problem
of finding the greatest common denominator for two given
integers.
*/

main ()
char *string[10];
unsigned r,m,n,x;
unsigned temp;
{
	     while (1)	 {
		printf("\nEnter first integer\n");
		m = atoi(gets(string));
		printf("Enter second integer\n");
		n = atoi(gets(string));
		if (m < n)  {
		   temp = m;
		   m = n;
		   n = temp;
		}
		r = m % n;
		x = 1;
		while (x > 0)	{
		     if (r == 0)  {
			printf("\nLargest common divisor = %u", n);
			x = 0;
		     }
		     else  {
			   m = n;
			   n = r;
			   r = m % n;
		     }
		}
	     }
}
