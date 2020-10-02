/*
HEADER:       CUG146.05;
TITLE:        Small C compiler for 6800;
DESCRIPTION:  "Erathosthenes Sieve Prime Number Program in C
              as excerpted from the Sept, 1981 issue of BYTE
              magazine. Modified for compatibility with Small-C
              derivatives, which do not have FOR loops. As
              published in '68' MICRO Journal, July 1982.";
KEYWORDS:     prime number program in C;
FILENAME:     prime.c
*/
/* Erathosthenes Sieve Prime Number Program in C
 *
 * As excerpted from the Sept, 1981 issue of BYTE
 * magazine.
 *
 * Modified for compatibility with Small-C deriva-
 * tives, which do not have FOR loops.
 *
 * As published in '68' MICRO Journal, July 1982.
 *
 */

#define TRUE    1
#define FALSE   0
#define SIZE    8190
#define SIZEP1  8191

char flags[SIZEP1];

main()
{
        int i, prime, k, count, iter;

        puts ("10 iterations"); nl();
        iter=1;
        while (iter <= 10)
                {
                count=0;
                i=0;
                while (i <= SIZE)
                        {
                        flags[i]=TRUE;
                        i++;
                        }
                i=0;
                while (i <= SIZE)
                        {
                        if (flags[i])
                                {
                                prime=i+i+3;
                                k=i+prime;
                                while (k <= SIZE)
                                        {
                                        flags[k]=FALSE;
                                        k=k+prime;
                                        }
                                count++;
                                }
                        i++;
                        }
                iter++;
                }
                outdec (count);
                puts(" primes"); nl();
        }

nl()
{
        putchar (13);
        }

outdec (n)
int n;
{
        if (n > 9)
                outdec (n/10);
        putchar ('0'+(n%10));
        }
SCRIPTION:  "Erathosthenes Sieve Prime Number Program 