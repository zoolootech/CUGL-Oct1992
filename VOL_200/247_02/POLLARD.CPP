/*
 *   Program to factor big numbers using Pollards (p-1) method.
 *   Works when for some prime divisor p of n, p-1 has only
 *   small factors.
 *   See "Speeding the Pollard and Elliptic Curve Methods"
 *   by Peter Montgomery, Math. Comp. Vol. 48 Jan. 1987 pp243-264
 */

#include <stream.hpp>
#include "number.hpp"

#define LIMIT1 1000        /* must be int, and > MULT/2 */
#define LIMIT2 30000L      /* may be long */
#define MULT   210          /* must be int, product of small primes 2.3.. */

miracl precision=50;

static GFn bu[1+MULT/2];
static bool cp[1+MULT/2];

main()
{  /*  factoring program using Pollards (p-1) method */
    int phase,m,iv,pos,btch;
    int *primes;
    long i,p,pa;
    GFn b=2;
    GFn q,n,t,bw,bvw,bd,bp;
    primes=gprime(LIMIT1);
    for (m=1;m<=MULT/2;m+=2)
        if (igcd(MULT,m)==1) cp[m]=TRUE;
        else                 cp[m]=FALSE;
    cout << "input number to be factored\n";
    cin >> n;
    modulo(n);                    /* do all arithmetic mod n */
    phase=1;
    p=0;
    btch=50;
    i=0;
    forever
    { /* main loop */
        if (phase==1)
        { /* looking for all factors of (p-1) < LIMIT1 */
            p=primes[i];
            if (primes[i+1]==0)
            { /* now change gear */
                phase=2;
                bw=pow(b,8);
                t=1;
                bp=bu[1]=b;
                for (m=3;m<=MULT/2;m+=2)
                { 
                    t*=bw;
                    bp*=t;
                    if (cp[m]) bu[m]=bp;
                }
                t=pow(b,MULT);
                t=pow(t,MULT);
                bd=t*t;
                iv=p/MULT;
                if (p%MULT>MULT/2) iv++,p=2*(long)iv*MULT-p;
                bw=pow(t,(2*iv-1));
                bvw=pow(t,iv);
                bvw=pow(bvw,iv);
                q=bvw-bu[p%MULT];
                btch*=10;
                i++;
                continue;
            }
            pa=p;
            while ((LIMIT1/p) > pa) pa*=p;
            b=pow(b,(int)pa);
            q=b-1;
        }
        else
        { /* phase 2 - looking for one last prime factor of (p-1) < LIMIT2 */
            p+=2;
            pos=p%MULT;
            if (pos>MULT/2)
            { 
                iv++;
                p=(long)iv*MULT+1;
                pos=1;
                bw*=bd;
                bvw*=bw;
            }
            if (!cp[pos]) continue;
            q*=(bvw-bu[pos]);
        }
        if (i++%btch==0)
        { /* try for a solution */
            t=gcd(q,n);
            if (t==1) continue;
            cout << "\nfactor is\n" << t << "\n";
            exit(0);
        }
    } 
}

