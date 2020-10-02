/*
 *   MIRACL euclidean mediant rounding routine
 *   bnround.c
 */

#include <stdio.h>
#include "miracl.h"
#define abs(x)  ((x)<0? (-(x)) : (x))

/* Access global variables */

extern int  depth;    /* error tracing .. */
extern int  trace[];  /* .. mechanism     */
extern int  nib;      /* length of bigs   */
extern small base;    /* number base      */
extern bool check;    /* overflow check   */

extern big w0;        /* workspace variables */
extern big w5;
extern big w6;
extern big w7;

int euclid(x,num)
big x;
int num;
{ /* outputs next c.f. quotient from gcd(w5,w6) */
    static long u,v;
    static small a,b,c,d;
    static int   n,oldn,q;
    static bool last,carryon;
    small r,m;
    long lr,lq;
    big t;
    if (num==0)
    {
        oldn=(-1);
        carryon=FALSE;
        last=FALSE;
        if (compare(w6,w5)>0)
        { /* ensure w5>w6 */
            t=w5,w5=w6,w6=t;
            return (q=0);
        }
    }
    else if (num==oldn || q<0) return q;
    oldn=num;
    if (carryon) goto middle;
start:
    if (size(w6)==0) return (q=(-1));
    n=w5[0];
    carryon=TRUE;
    a=1;
    b=0;
    c=0;
    d=1;
    if (n==1)
    {
        last=TRUE;
        u=w5[1];
        v=w6[1];
    }
    else
    {
        m=w5[n]+1;
        if (sizeof(long)==2*sizeof(small))
        { /* use longs if they are double length */
            if (n>2)
            { /* squeeze out as much significance as possible */
                u=muldiv(w5[n],base,w5[n-1],m,&r);
                u=u*base+muldiv(r,base,w5[n-2],m,&r);
                v=muldiv(w6[n],base,w6[n-1],m,&r);
                v=v*base+muldiv(r,base,w6[n-2],m,&r);
            }
            else
            {
                u=(long)base*w5[n]+w5[n-1];
                v=(long)base*w6[n]+w6[n-1];
                last=TRUE;
            }
        }
        else
        {
            u=muldiv(w5[n],base,w5[n-1],m,&r);
            v=muldiv(w6[n],base,w6[n-1],m,&r);
        }
    }
middle:
    forever
    { /* work only with most significant piece */
        if (last)
        {
            if (v==0) return (q=(-1));
            lq=u/v;
        }
        else
        {
            if (((v+c)==0) || ((v+d)==0)) break;
            lq=(u+a)/(v+c);
            if (lq!=(u+b)/(v+d)) break;
        }
        if (lq>=TOOBIG || lq>=MAXBASE/abs(d)) break;
        q=lq;
        r=a-q*c;
        a=c;
        c=r;
        r=b-q*d;
        b=d;
        d=r;
        lr=u-lq*v;
        u=v;
        v=lr;
        return q;
    }
    carryon=FALSE;
    if (b==0)
    { /* update w5 and w6 */
        check=OFF;
        divide(w5,w6,w7);
        check=ON;
        if (lent(w7)>nib) return (q=(-2));
        t=w5,w5=w6,w6=t;   /* swap(w5,w6) */
        copy(w7,x);
        return (q=size(x));
    }
    else
    {
        check=OFF;
        premult(w5,c,w7);
        premult(w5,a,w5);
        premult(w6,b,w0);
        premult(w6,d,w6);
        add(w5,w0,w5);
        add(w6,w7,w6);
        check=ON;
    }
    goto start;
}


void round(num,den,z)
big num,den;
flash z;
{ /* reduces and rounds the fraction num/den into z */
    int s;
    if (ERNUM) return;
    if (size(num)==0) 
    {
        zero(z);
        return;
    }
    depth++;
    trace[depth]=34;
    if (TRACER) track();
    if (size(den)==0)
    {
        berror(13);
        depth--;
        return;
    }
    copy(num,w5);
    copy(den,w6);
    s=exsign(w5)*exsign(w6);
    insign(PLUS,w5);
    insign(PLUS,w6);
    if (compare(w5,w6)==0)
    {
        convert((small)s,z);
        depth--;
        return;
    }
    if (size(w6)==1)
    {
        if (w5[0]>nib)
        {
            berror(13);
            depth--;
            return;
        }
        copy(w5,z);
        insign(s,z);
        depth--;
        return;
    }
    build(z,euclid);
    insign(s,z);
    depth--;
}
