/*
 *   Program to factor big numbers using Brent-Pollard method.
 *   See "An Improved Monte Carlo Factorization Algorithm"
 *   by Richard Brent in BIT Vol. 20 1980 pp 176-184
 */

#include <stream.hpp>
#include "big.hpp"

#define min(a,b) ((a) < (b)? (a) : (b))

miracl precision=50;

main()
{  /*  factoring program using Brents method */
    long k,r,i,m,iter;
    Big x,y,z,n,q,ys;
    cout << "input number to be factored\n";
    cin >> n;
    if (prime(n))
    {
        printf("this number is prime!\n");
        exit(0);
    }
    m=10;
    r=1;
    iter=0;
    do
    {
        cout << "iterations=" << dec(iter,5);
        q=1;
        do
        {
            x=y;
            for (i=1;i<=r;i++) y=(y*y+3)%n;
            k=0;
            do
            {
                flushall();
                iter++;
                if (iter%10==0) cout << "\b\b\b\b\b" << dec(iter,5);
                ys=y;
                for (i=1;i<=min(m,r-k);i++)
                {   
                    y=(y*y+3)%n;
                    q=((y-x)*q)%n;
                }
                z=gcd(q,n);
                k+=m;
            } while (k<r && z==1);
            r*=2;
        } while (z==1);
        if (z==n) do 
        { /* back-track */
            ys=(ys*ys+3)%n;
            z=gcd(ys-x,n);
        } while (z==1);
        if (!prime(z))
             cout << "\ncomposite factor ";
        else cout << "\nprime factor     ";
        cout << z << "\n";
        if (z==n) exit(0);
        n/=z;
        y%=n;
    } while (!prime(n));      
    cout << "prime factor     ";
    cout << n << "\n";
}

