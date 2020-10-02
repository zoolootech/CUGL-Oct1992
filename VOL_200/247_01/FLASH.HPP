/*
 *    Experimental MIRACL  C++ Header file, for use with Zortech C++
 *
 *    AUTHOR  :    N.Coghlan
 *                 Modified by M.Scott
 *    NAME    :    flash.hpp
 *             
 *    PURPOSE :    Definition of class Flash
 */

overload cos;
overload sin;
overload tan;
overload acos;
overload asin;
overload atan;

overload cosh;
overload sinh;
overload tanh;
overload acosh;
overload asinh;
overload atanh;

overload log;
overload exp;
overload pow;
overload sqrt;
overload fabs;
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

class Flash
{ /* Flash Class Definitions */
    flash fn;      /* pointer to actual data */
public:
    Flash()                                                 {fn=mirvar(0);}
    Flash(int i)                                            {fn=mirvar(i);}
    Flash(long lg)                          {fn=mirvar(0);   lconv(lg,fn);}
    Flash(double d)                         {fn=mirvar(0);    dconv(d,fn);}
    Flash(Flash& f)                         {fn=mirvar(0); copy(f.fn, fn);}
    Flash(char* s)        {fn=mirvar(0);strcpy(IBUFF,s);cinnum(fn, stdin);}

    Flash& operator=(int i)              {convert(i,fn);     return *this;}
    Flash& operator=(const double& d)    {dconv(d,fn);       return *this;}
    Flash& operator=(const Flash& f)     {copy(f.fn, fn);    return *this;}
    Flash& operator=(char* s)            {strcpy(IBUFF,s);cinnum(fn,stdin);
                                                             return *this;}
    Flash& operator++()                  {fincr(fn,1,1,fn);  return *this;}
    Flash& operator--()                  {fincr(fn,-1,1,fn); return *this;}
    Flash& operator+=(int i)             {fincr(fn,i,1,fn);  return *this;}
    Flash& operator+=(const Flash& f)    {fadd(fn,f.fn,fn);  return *this;}
    Flash& operator-=(int i)             {fincr(fn,-i,1,fn); return *this;}
    Flash& operator-=(const Flash& f)    {fsub(fn,f.fn,fn);  return *this;}
    Flash& operator*=(int i)             {fpmul(fn,i,1,fn);  return *this;}
    Flash& operator*=(const Flash& f)    {fmul(fn,f.fn,fn);  return *this;}
    Flash& operator/=(int i)             {fpmul(fn,1,i,fn);  return *this;}
    Flash& operator/=(const Flash& f)    {fdiv(fn,f.fn,fn);  return *this;}

    friend Flash operator-(const Flash&);   /* unary - */

    /* binary ops */

    friend Flash operator+(const Flash&, const Flash&);
    friend Flash operator+(const Flash&, int);
    friend Flash operator+(int, const Flash&);

    friend Flash operator-(const Flash&, const Flash&);
    friend Flash operator-(const Flash&, int);
    friend Flash operator-(int, const Flash&);

    friend Flash operator*(const Flash&, const Flash&);
    friend Flash operator*(const Flash&, int);
    friend Flash operator*(int, const Flash&);

    friend Flash operator/(const Flash&, const Flash&);
    friend Flash operator/(const Flash&, int);
    friend Flash operator/(int, const Flash&);

    /* relational ops */

    friend bool operator<=(const Flash& f1, const Flash& f2)
             {if (fcomp(f1.fn,f2.fn) <= 0) return TRUE; else return FALSE;}
    friend bool operator<=(const Flash& f, int i);
    friend bool operator<=(int i, const Flash& f);

    friend bool operator>=(const Flash& f1, const Flash& f2) 
             {if (fcomp(f1.fn,f2.fn) >= 0) return TRUE; else return FALSE;}
    friend bool operator>=(const Flash& f, int i);
    friend bool operator>=(int i, const Flash& f);

    friend bool operator==(const Flash& f1, const Flash& f2)
             {if (fcomp(f1.fn,f2.fn) == 0) return TRUE; else return FALSE;}
    friend bool operator==(const Flash& f, int i);
    friend bool operator==(int i, const Flash& f);

    friend bool operator!=(const Flash& f1, const Flash& f2)
             {if (fcomp(f1.fn,f2.fn) != 0) return TRUE; else return FALSE;}
    friend bool operator!=(const Flash& f, int i);
    friend bool operator!=(int i, const Flash& f);

    friend bool operator<(const Flash& f1, const Flash& f2)
             {if (fcomp(f1.fn,f2.fn) < 0)  return TRUE; else return FALSE;}
    friend bool operator<(const Flash& f, int i);
    friend bool operator<(int i, const Flash& f);

    friend bool operator>(const Flash& f1, const Flash& f2) 
             {if (fcomp(f1.fn,f2.fn) > 0)  return TRUE; else return FALSE;}
    friend bool operator>(const Flash& f, int i);
    friend bool operator>(int i, const Flash& f);

    friend Flash pi(); 
    friend Flash cos(const Flash&);
    friend Flash sin(const Flash&);
    friend Flash tan(const Flash&);

    friend Flash acos(const Flash&);
    friend Flash asin(const Flash&);
    friend Flash atan(const Flash&);

    friend Flash cosh(const Flash&);
    friend Flash sinh(const Flash&);
    friend Flash tanh(const Flash&);

    friend Flash acosh(const Flash&);
    friend Flash asinh(const Flash&);
    friend Flash atanh(const Flash&);

    friend Flash log(const Flash&);
    friend Flash exp(const Flash&);
    friend Flash pow(const Flash&,const Flash&);
    friend Flash pow(const Flash&,int);
    friend Flash sqrt(const Flash&);
    friend Flash fabs(const Flash&);

    friend istream& operator>>(istream& s, Flash& x)
                           { s >> IBUFF; cinnum(x.fn,stdin) ;   return s; }
    friend ostream& operator<<(ostream& s, Flash& x)
                  { cotnum(x.fn,stdout); if (STROUT) s << OBUFF; return s;}

    ~Flash()                                            {free((char *)fn);}
};

Flash operator-(const Flash& f)  {Flash nf; negate(f.fn,nf.fn); return nf;}

Flash operator+(const Flash& f1, const Flash& f2)
                         {Flash aff; fadd(f1.fn,f2.fn,aff.fn); return aff;}
Flash operator+(const Flash& f,int i)
                         {Flash afi;    fincr(f.fn,i,1,afi.fn);return afi;}
Flash operator+(int i,const Flash& f)
                         {Flash aif;    fincr(f.fn,i,1,aif.fn);return aif;}

Flash operator-(const Flash& f1, const Flash& f2)
                         {Flash mff; fsub(f1.fn,f2.fn,mff.fn); return mff;}
Flash operator-(const Flash& f, int i)
                         {Flash mfi; fincr(f.fn,(-i),1,mfi.fn);return mfi;}
Flash operator-(int i, const Flash& f)
    {Flash mif;fincr(f.fn,(-i),1,mif.fn);negate(mif.fn,mif.fn);return mif;}

Flash operator*(const Flash& f1, const Flash& f2)
                         {Flash xff; fmul(f1.fn,f2.fn,xff.fn); return xff;}
Flash operator*(const Flash& f, int i)
                         {Flash xfi;  fpmul(f.fn,i,1,xfi.fn);  return xfi;}
Flash operator*(int i, const Flash& f)
                         {Flash xif;  fpmul(f.fn,i,1,xif.fn);  return xif;}

Flash operator/(const Flash& f1, const Flash& f2)
                         {Flash dff; fdiv(f1.fn,f2.fn,dff.fn); return dff;}
Flash operator/(const Flash& f, int i)
                         {Flash dfi;  fpmul(f.fn,1,i,dfi.fn);  return dfi;}
Flash operator/(int i, const Flash& f)
    {Flash dif;  fpmul(f.fn,1,i,dif.fn);frecip(dif.fn,dif.fn); return dif;}

Flash pi()                        {Flash z; fpi(z.fn);           return z;}

Flash cos(const Flash& f)         {Flash z; fcos(f.fn, z.fn);    return z;}
Flash sin(const Flash& f)         {Flash z; fsin(f.fn, z.fn);    return z;}
Flash tan(const Flash& f)         {Flash z; ftan(f.fn, z.fn);    return z;}

Flash acos(const Flash& f)        {Flash z; facos(f.fn, z.fn);   return z;}
Flash asin(const Flash& f)        {Flash z; fasin(f.fn, z.fn);   return z;}
Flash atan(const Flash& f)        {Flash z; fatan(f.fn, z.fn);   return z;}

Flash cosh(const Flash& f)        {Flash z; fcosh(f.fn, z.fn);   return z;}
Flash sinh(const Flash& f)        {Flash z; fsinh(f.fn, z.fn);   return z;}
Flash tanh(const Flash& f)        {Flash z; ftanh(f.fn, z.fn);   return z;}

Flash acosh(const Flash& f)       {Flash z; facosh(f.fn, z.fn);  return z;}
Flash asinh(const Flash& f)       {Flash z; fasinh(f.fn, z.fn);  return z;}
Flash atanh(const Flash& f)       {Flash z; fatanh(f.fn, z.fn);  return z;}

Flash log(const Flash& f)         {Flash z; flog(f.fn, z.fn);    return z;}
Flash exp(const Flash& f)         {Flash z; fexp(f.fn, z.fn);    return z;}
Flash pow(const Flash& f1,const Flash& f2) 
                               {Flash z;fpowf(f1.fn,f2.fn,z.fn); return z;}
Flash pow(const Flash& f,int i)
                               {Flash z; fpower(f.fn,i,z.fn);    return z;}

Flash sqrt(const Flash& f)     {Flash z; froot(f.fn, 2, z.fn);   return z;}
Flash fabs(const Flash& f)     {Flash z; absol(f.fn,z.fn);       return z;}

bool operator<=(const Flash& b, int i)          {Flash z=i; return (b<=z);}
bool operator<=(int i, const Flash& b)          {Flash z=i; return (z<=b);}
bool operator>=(const Flash& b, int i)          {Flash z=i; return (b>=z);}
bool operator>=(int i, const Flash& b)          {Flash z=i; return (z>=b);}
bool operator==(const Flash& b, int i)          {Flash z=i; return (b==z);}
bool operator==(int i, const Flash& b)          {Flash z=i; return (z==b);}
bool operator!=(const Flash& b, int i)          {Flash z=i; return (b!=z);}
bool operator!=(int i, const Flash& b)          {Flash z=i; return (z!=b);}
bool operator<(const Flash& b, int i)            {Flash z=i; return (b<z);}
bool operator<(int i, const Flash& b)            {Flash z=i; return (z<b);}
bool operator>(const Flash& b, int i)            {Flash z=i; return (b>z);}
bool operator>(int i, const Flash& b)            {Flash z=i; return (z>b);}

