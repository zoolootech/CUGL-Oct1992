/*
 * MIRACL small number theoretic routines 
 * bnsmall.c
 */

#include <stdio.h>
#include <math.h>
#include "miracl.h"

/* access global variables */

extern int depth;     /* error tracing... */
extern int trace[];   /* ....mechanism    */
extern char *calloc();

int *gprime(maxp)
int maxp;
{ /* generate all primes less than maxp */
    int *primes;
    double dn;
    char *sv;
    int pix,i,k,prime;
    if (ERNUM) return NULL;
    if (maxp<0)
    { /* generate (-maxp) primes */
        dn=(-maxp);
        if (dn<20.0) dn=20.0;
        maxp=(int)(dn*(log(dn*log(dn))-0.5)); /* Rossers upper bound */
    }
    depth++;
    trace[depth]=70;
    if (TRACER) track();
    if (maxp>=TOOBIG)
    {
         berror(8);
         depth--;
         return NULL;
    }
    maxp=(maxp+1)/2;
    sv=(char *)calloc(maxp,1);
    if (sv==NULL)
    {
        berror(8);
        depth--;
        return NULL;
    }
    pix=1;
    for (i=0;i<maxp;i++)
        sv[i]=TRUE;
    for (i=0;i<maxp;i++)
    if (sv[i])
    { /* using sieve of Eratosthenes */
        prime=i+i+3;
        for (k=i+prime;k<maxp;k+=prime)
            sv[k]=FALSE;
        pix++;
    }
    primes=(int *)calloc(pix+2,sizeof(int));
    if (primes==NULL)
    {
        berror(8);
        depth--;
        return NULL;
    }
    primes[0]=2;
    pix=1;
    for (i=0;i<maxp;i++)
        if (sv[i]) primes[pix++]=i+i+3;
    primes[pix]=0;
    free(sv);
    depth--;
    return primes;
}

small smul(x,y,n)
small x,y,n;
{ /* returns x*y mod n */
    small r;
    x%=n;
    y%=n;
    if (x<0) x+=n;
    if (y<0) y+=n;
    muldiv(x,y,(small)0,n,&r);
    return r;
}

small spmd(x,n,m)
small x,n,m;
{ /*  returns x^n mod m  */
    small r;
    x%=m;
    if (x<0) x+=m;
    r=0;
    if (x==0) return r;
    r=1;
    if (n==0) return r;
    forever
    {
        if (n%2!=0) muldiv(r,x,(small)0,m,&r);
        n/=2;
        if (n==0) return r;
        muldiv(x,x,(small)0,m,&x);
    }
}

small inverse(x,y)
small x,y;
{ /* returns inverse of x mod y */
    small r,s,q,t,p;
    x%=y;
    if (x<0) x+=y;
    r=1;
    s=0;
    p=y;
    while (p!=0)
    { /* main euclidean loop */
        q=x/p;
        t=r-s*q;
        r=s;
        s=t;
        t=x-p*q;
        x=p;
        p=t;
    }
    if (r<0) r+=y;
    return r;
}

small sqrmp(x,m)
small x,m;
{ /* square root mod a small prime by Shanks method  *
   * returns 0 if root does not exist or m not prime */
    small q,z,y,v,w,t;
    int i,r,e,n;
    bool pp;
    x%=m;
    if (x==0) return 0;
    if (x<0) x+=m;
    if (x==1) return 1;
    if (spmd(x,(m-1)/2,m)!=1) return 0;    /* Legendre symbol not 1   */
    if (m%4==3) return spmd(x,(m+1)/4,m);  /* easy case for m=4.k+3   */
    q=m-1;
    e=0;
    while (q%2==0) 
    {
        q/=2;
        e++;
    }
    if (e==0) return 0;      /* even m */
    for (r=2;;r++)
    { /* find suitable z */
        z=spmd((small)r,q,m);
        if (z==1) continue;
        t=z;
        pp=FALSE;
        for (i=1;i<e;i++)
        { /* check for composite m */
            if (t==(m-1)) pp=TRUE;
            muldiv(t,t,(small)0,m,&t);
            if (t==1 && !pp) return 0;
        }
        if (t==(m-1)) break;
        if (!pp) return 0;   /* m is not prime */
    }
    y=z;
    r=e;
    v=spmd(x,(q+1)/2,m);
    w=spmd(x,q,m);
    while (w!=1)
    {
        t=w;
        for (n=0;t!=1;n++) muldiv(t,t,(small)0,m,&t);
        if (n>=r) return 0;
        y=spmd(y,(small)(1<<(r-n-1)),m);
        muldiv(v,y,(small)0,m,&v);
        muldiv(y,y,(small)0,m,&y);
        muldiv(w,y,(small)0,m,&w);
        r=n;
    }
    return v;
}

