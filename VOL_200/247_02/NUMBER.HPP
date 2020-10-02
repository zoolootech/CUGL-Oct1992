/*
 *    Experimental MIRACL  C++ Header file
 *
 *    AUTHOR  :    M. Scott
 *
 *    NAME    :    number.hpp
 *             
 *    PURPOSE :    Definition of class GFn  (Arithmetic mod n)
 */

overload pow;
overload prime;
overload gcd;

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "miracl.h"

extern big w14;                        /* use this to store modulus */

void normalise(big b)
{ /* normalise a number into range */
    divide(b,w14,w14);
    if (size(b)<0) add(b,w14,b);
}

class miracl
{ /* dummy class to initialise MIRACL */
public:
    miracl(int nd,small nb=MAXBASE) {mirsys(nd,nb);STROUT=TRUE;POINT=TRUE;}
};


class GFn 
{ 
    big fn;
public:
    GFn()                                               { fn = mirvar(0); } 
    GFn(int i)                                          { fn = mirvar(i); }
    GFn(long lg)                         { fn = mirvar(0); lconv(lg,fn);  }
    GFn(GFn& b)                          { fn = mirvar(0); copy(b.fn,fn); }
    GFn(char* s)     { fn = mirvar(0); strcpy(IBUFF,s); cinnum(fn,stdin); }

    GFn& operator=(int i)                    {convert(i,fn); return *this;}
    GFn& operator=(const GFn& b)             {copy(b.fn,fn); return *this;}
    GFn& operator=(char* s){strcpy(IBUFF,s);cinnum(fn,stdin);return *this;}

    GFn& operator++()           {incr(fn,1,fn);normalise(fn);return *this;}
    GFn& operator--()           {decr(fn,1,fn);normalise(fn);return *this;}
    GFn& operator+=(int i)      {incr(fn,i,fn);normalise(fn);return *this;}
    GFn& operator+=(const GFn& b)
                              {add(fn,b.fn,fn);normalise(fn);return *this;}

    GFn& operator-=(int i)     {decr(fn,i,fn);normalise(fn); return *this;}
    GFn& operator-=(const GFn& b)
                        {subtract(fn,b.fn,fn);normalise(fn); return *this;}

    GFn& operator*=(int i)  {premult(fn,i,fn);normalise(fn); return *this;}
    GFn& operator*=(const GFn& b){mad(fn,b.fn,fn,w14,w14,fn);return *this;}

    GFn& operator/=(int);       
    GFn& operator/=(const GFn&);

    friend GFn operator-(const GFn&);

    friend GFn operator+(const GFn&,int);
    friend GFn operator+(int,const GFn&);
    friend GFn operator+(const GFn&, const GFn&);

    friend GFn operator-(const GFn&, int);
    friend GFn operator-(int,const GFn&);
    friend GFn operator-(const GFn&, const GFn&);

    friend GFn operator*(const GFn&, int);
    friend GFn operator*(int,const GFn&);
    friend GFn operator*(const GFn&, const GFn&);

    friend GFn operator/(const GFn&, int);
    friend GFn operator/(int,const GFn&);
    friend GFn operator/(const GFn&, const GFn&);

    friend bool operator==(const GFn& b1, const GFn& b2)
             {if (compare(b1.fn,b2.fn)==0) return TRUE; else return FALSE;}
    friend bool operator==(const GFn&,int);
    friend bool operator==(int,const GFn&);

    friend bool operator!=(const GFn& b1, const GFn& b2)
             {if (compare(b1.fn,b2.fn)!=0) return TRUE; else return FALSE;}
    friend bool operator!=(const GFn&, int);
    friend bool operator!=(int, const GFn&);

    friend bool prime(const GFn& b)                   {return prime(b.fn);}
    friend GFn  gcd(const GFn&,const GFn&);
    friend GFn  pow(const GFn&,int);
    friend GFn  pow(const GFn&,const GFn&);

    friend void modulo(const GFn& b)      {copy(b.fn,w14); absol(w14,w14);}

    friend istream& operator>>(istream& s, GFn& x)
                           { s >> IBUFF; cinnum(x.fn,stdin) ;   return s; }
    friend ostream& operator<<(ostream& s, GFn& x)
                  { cotnum(x.fn,stdout); if (STROUT) s << OBUFF; return s;}

    ~GFn()                                             {free((char *)fn); }
};


GFn& GFn::operator/=(int k)        {GFn z=k; xgcd(z.fn,w14,z.fn,z.fn,z.fn);
                                  mad(z.fn,fn,fn,w14,w14,fn);return *this;}
GFn& GFn::operator/=(const GFn& b)   {GFn z; xgcd(b.fn,w14,z.fn,z.fn,z.fn);
                                  mad(z.fn,fn,fn,w14,w14,fn);return *this;}

GFn operator-(const GFn& b)
            {GFn nb; subtract(w14,b.fn,nb.fn);normalise(nb.fn); return nb;}
GFn operator+(const GFn& b,int i)
            {GFn abi; incr(b.fn, i, abi.fn);normalise(abi.fn); return abi;}
GFn operator+(int i,const GFn& b)
            {GFn aib; incr(b.fn, i, aib.fn);normalise(aib.fn); return aib;}
GFn operator+(const GFn& b1, const GFn& b2)
            {GFn abb;add(b1.fn,b2.fn,abb.fn);normalise(abb.fn);return abb;}

GFn operator-(const GFn& b, int i)
            {GFn mbi; decr(b.fn, i, mbi.fn);normalise(mbi.fn); return mbi;}
GFn operator-(int i,const GFn& b)   {GFn mib;decr(b.fn,i,mib.fn);
                       negate(mib.fn,mib.fn);normalise(mib.fn);return mib;}
GFn operator-(const GFn& b1, const GFn& b2)
      {GFn mbb;subtract(b1.fn,b2.fn,mbb.fn);normalise(mbb.fn); return mbb;}

GFn operator*(const GFn& b, int i)
         {GFn xbi; premult(b.fn, i, xbi.fn);normalise(xbi.fn); return xbi;}
GFn operator*(int i,const GFn& b)
         {GFn xib; premult(b.fn, i, xib.fn);normalise(xib.fn); return xib;}
GFn operator*(const GFn& b1, const GFn& b2)
             {GFn xbb; mad(b1.fn,b2.fn,xbb.fn,w14,w14,xbb.fn); return xbb;}

GFn operator/(const GFn& b, int k)  {GFn z=k;xgcd(z.fn,w14,z.fn,z.fn,z.fn);
                              mad(b.fn,z.fn,z.fn,w14,w14,z.fn);  return z;}
GFn operator/(int k,const GFn& b)    {GFn z; xgcd(b.fn,w14,z.fn,z.fn,z.fn);
                       premult(z.fn,k,z.fn);divide(z.fn,w14,w14);return z;}
GFn operator/(const GFn& b1, const GFn& b2)
                                    {GFn z; xgcd(b2.fn,w14,z.fn,z.fn,z.fn);
                             mad(b1.fn,z.fn,z.fn,w14,w14,z.fn);  return z;}

GFn gcd(const GFn& b1,const GFn& b2){GFn z;gcd(b1.fn,b2.fn,z.fn);return z;}
GFn pow(const GFn& b,int n)        {GFn z;power(b.fn,n,w14,z.fn);return z;}
GFn pow(const GFn& b1,const GFn& b2)
                             {GFn z;powmod(b1.fn,b2.fn,w14,z.fn);return z;}

bool operator==(const GFn& b, int i)              {GFn z=i; return (b==z);}
bool operator==(int i, const GFn& b)              {GFn z=i; return (z==b);}
bool operator!=(const GFn& b, int i)              {GFn z=i; return (b!=z);}
bool operator!=(int i, const GFn& b)              {GFn z=i; return (z!=b);}

