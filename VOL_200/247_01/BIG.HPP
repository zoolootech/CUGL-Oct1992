/*
 *    Experimental MIRACL  C++ Header file
 *
 *    AUTHOR  :    N.Coghlan
 *                 Modified by M.Scott
 *    NAME    :    big.hpp
 *             
 *    PURPOSE :    Definition of class Big
 */

overload pow;
overload sqrt;
overload abs;

overload prime;
overload gcd;
overload root;
overload inverse;
overload rand;

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "miracl.h"

class miracl
{ /* dummy class to initialise MIRACL */
public:
    miracl(int nd,small nb=MAXBASE) {mirsys(nd,nb);STROUT=TRUE;POINT=TRUE;}
};

class Big 
{ 
    big fn;
public:
    Big()                                               { fn = mirvar(0); } 
    Big(int i)                                          { fn = mirvar(i); }
    Big(long lg)                         { fn = mirvar(0); lconv(lg,fn);  }
    Big(Big& b)                          { fn = mirvar(0); copy(b.fn,fn); }
    Big(char* s)           {fn=mirvar(0);strcpy(IBUFF,s);cinnum(fn,stdin);}

    Big& operator=(int i)                    {convert(i,fn); return *this;}
    Big& operator=(const Big& b)             {copy(b.fn,fn); return *this;}
    Big& operator=(char* s){strcpy(IBUFF,s);cinnum(fn,stdin);return *this;}

    Big& operator++()                        {incr(fn,1,fn); return *this;}
    Big& operator--()                        {decr(fn,1,fn); return *this;}
    Big& operator+=(int i)                   {incr(fn,i,fn); return *this;}
    Big& operator+=(const Big& b)          {add(fn,b.fn,fn); return *this;}

    Big& operator-=(int i)                   {decr(fn,i,fn); return *this;}
    Big& operator-=(const Big& b)     {subtract(fn,b.fn,fn); return *this;}

    Big& operator*=(int i)                {premult(fn,i,fn); return *this;}
    Big& operator*=(const Big& b)     {multiply(fn,b.fn,fn); return *this;}

    Big& operator/=(int i)              {subdiv(fn,i,fn);    return *this;}
    Big& operator/=(const Big& b)       {divide(fn,b.fn,fn); return *this;}

    Big& operator%=(int i)     {convert(subdiv(fn,i,fn),fn); return *this;}
    Big& operator%=(const Big& b)     {divide(fn,b.fn,b.fn); return *this;}

    friend Big operator-(const Big&);

    friend Big operator+(const Big&,int);
    friend Big operator+(int,const Big&);
    friend Big operator+(const Big&, const Big&);

    friend Big operator-(const Big&, int);
    friend Big operator-(int,const Big&);
    friend Big operator-(const Big&, const Big&);

    friend Big operator*(const Big&, int);
    friend Big operator*(int,const Big&);
    friend Big operator*(const Big&, const Big&);

    friend Big operator/(const Big&, int);
    friend Big operator/(int,const Big&);
    friend Big operator/(const Big&, const Big&);

    friend int operator%(const Big&, int);
    friend Big operator%(const Big&, const Big&);

    friend bool operator<=(const Big& b1, const Big& b2)
             {if (compare(b1.fn,b2.fn)<=0) return TRUE; else return FALSE;}
    friend bool operator<=(const Big&,int);
    friend bool operator<=(int,const Big&);

    friend bool operator>=(const Big& b1, const Big& b2)
             {if (compare(b1.fn,b2.fn)>=0) return TRUE; else return FALSE;}
    friend bool operator>=(const Big&,int);
    friend bool operator>=(int,const Big&);
    
    friend bool operator==(const Big& b1, const Big& b2)
             {if (compare(b1.fn,b2.fn)==0) return TRUE; else return FALSE;}
    friend bool operator==(const Big&,int);
    friend bool operator==(int,const Big&);

    friend bool operator!=(const Big& b1, const Big& b2)
             {if (compare(b1.fn,b2.fn)!=0) return TRUE; else return FALSE;}
    friend bool operator!=(const Big&, int);
    friend bool operator!=(int, const Big&);

    friend bool operator<(const Big& b1, const Big& b2)
              {if (compare(b1.fn,b2.fn)<0) return TRUE; else return FALSE;}
    friend bool operator<(const Big&, int);
    friend bool operator<(int, const Big&);

    friend bool operator>(const Big& b1, const Big& b2)
              {if (compare(b1.fn,b2.fn)>0) return TRUE; else return FALSE;}
    friend bool operator>(const Big&, int);
    friend bool operator>(int, const Big&);

    friend Big sqrt(const Big&);
    friend Big abs(const Big&);
    friend bool prime(const Big& b)                   {return prime(b.fn);}
    friend Big gcd(const Big&, const Big &);
    friend Big pow(const Big&,int);
    friend Big pow(const Big&, int, const Big&);
    friend Big pow(const Big&, const Big&, const Big&);
    friend Big inverse(const Big&, int);
    friend Big inverse(const Big&, const Big&);
    friend Big root(const Big&, int);
    friend Big rand(const Big&);
    friend istream& operator>>(istream& s, Big& x)
                           { s >> IBUFF; cinnum(x.fn,stdin) ;   return s; }
    friend ostream& operator<<(ostream& s, Big& x)
                 { cotnum(x.fn,stdout); if (STROUT) s << OBUFF;  return s;}

    ~Big() {free((char *)fn); }
};

Big operator-(const Big& b)   {Big nb; negate(b.fn,nb.fn); return nb;}
Big operator+(const Big& b,int i)
                              {Big abi; incr(b.fn, i, abi.fn); return abi;}
Big operator+(int i,const Big& b)
                              {Big aib; incr(b.fn, i, aib.fn); return aib;}
Big operator+(const Big& b1, const Big& b2)
                              {Big abb;add(b1.fn,b2.fn,abb.fn);return abb;}

Big operator-(const Big& b, int i)
                              {Big mbi; decr(b.fn, i, mbi.fn); return mbi;}
Big operator-(int i,const Big& b)
          {Big mib;decr(b.fn, i, mib.fn);negate(mib.fn,mib.fn);return mib;}
Big operator-(const Big& b1, const Big& b2)
                       {Big mbb; subtract(b1.fn,b2.fn,mbb.fn); return mbb;}

Big operator*(const Big& b, int i)
                           {Big xbi; premult(b.fn, i, xbi.fn); return xbi;}
Big operator*(int i,const Big& b)
                           {Big xib; premult(b.fn, i, xib.fn); return xib;}
Big operator*(const Big& b1, const Big& b2)
                       {Big xbb; multiply(b1.fn,b2.fn,xbb.fn); return xbb;}

Big operator/(const Big& b, int i)
                            {Big dbi; subdiv(b.fn, i, dbi.fn); return dbi;}
Big operator/(int i,const Big& b)
         {Big dib;convert(i,dib.fn);divide(dib.fn,b.fn,dib.fn);return dib;}
Big operator/(const Big& b1, const Big& b2)
                         {Big dbb; divide(b1.fn,b2.fn,dbb.fn); return dbb;}

int operator%(const Big& b, int i)
                               {Big mdbi; return(subdiv(b.fn,i, mdbi.fn));}
Big operator%(const Big& b1, const Big& b2)
                     {Big mdbb=b1;divide(mdbb.fn,b2.fn,b2.fn);return mdbb;}
Big sqrt(const Big& b)              {Big z; root(b.fn, 2, z.fn); return z;}
Big abs(const Big& b)               {Big z; absol(b.fn,z.fn);    return z;}
Big gcd(const Big& b1,const Big& b2){Big z;gcd(b1.fn,b2.fn,z.fn);return z;}
Big pow(const Big& b,int n)     {Big z; power(b.fn,n,z.fn,z.fn); return z;}
Big pow(const Big& b1,int n,const Big& b3)
                              {Big z; power(b1.fn,n,b3.fn,z.fn); return z;}
Big pow(const Big& b1,const Big& b2,const Big& b3)
                         {Big z; powmod(b1.fn,b2.fn,b3.fn,z.fn); return z;}
Big inverse(const Big& b1,const Big& b2)
                        {Big z; xgcd(b1.fn,b2.fn,z.fn,z.fn,z.fn);return z;}
Big inverse(const Big& b,int n)
                        {Big z=n; xgcd(b.fn,z.fn,z.fn,z.fn,z.fn);return z;}
Big root(const Big& b,int n)          {Big z; root(b.fn,n,z.fn); return z;}
Big rand(const Big& b)               {Big z; bigrand(b.fn,z.fn); return z;}



bool operator<=(const Big& b, int i)              {Big z=i; return (b<=z);}
bool operator<=(int i, const Big& b)              {Big z=i; return (z<=b);}
bool operator>=(const Big& b, int i)              {Big z=i; return (b>=z);}
bool operator>=(int i, const Big& b)              {Big z=i; return (z>=b);}
bool operator==(const Big& b, int i)              {Big z=i; return (b==z);}
bool operator==(int i, const Big& b)              {Big z=i; return (z==b);}
bool operator!=(const Big& b, int i)              {Big z=i; return (b!=z);}
bool operator!=(int i, const Big& b)              {Big z=i; return (z!=b);}
bool operator<(const Big& b, int i)                {Big z=i; return (b<z);}
bool operator<(int i, const Big& b)                {Big z=i; return (z<b);}
bool operator>(const Big& b, int i)                {Big z=i; return (b>z);}
bool operator>(int i, const Big& b)                {Big z=i; return (z>b);}
                                             

