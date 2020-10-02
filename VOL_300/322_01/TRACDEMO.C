/*  tracdemo.c - Demonstrate "trace.h" macros. */

const static char RCsid[] = 
    "$Id: tracdemo.c 1.5 89/11/24 14:21:07 Bill_Rogers Exp $";

#include    "trace.h"

extern void func1(void);


void func1()
{
    T_FUNC(func1)
    int             i   = 23456;
/* begin */
    T_BEGIN()
    T_INT(i)
    T_END()
} /* func1 */
    

void main()
{
    T_FUNC(main)
    int             b   = 1;
    char            c   = 'c';
    double          d   = 12.3456789012345;
    unsigned int    h   = 0xabcd;
    int             i   = 12345;
    float           f   = 12.34567;
    unsigned long   lh  = 0x89abcdef;
    long            l   = 123456789;
    char *          p   = &c;
    static char     s[] = "here is a string";
/* begin */
    T_BEGIN()

    T_BOOL  (b)
    T_CHAR  (c)
    T_DBL   (d)
    T_HEX   (h)
    T_INT   (i)
    T_FLOAT (f)
    T_LHEX  (lh)
    T_LONG  (l)
    T_PTR   (p)
    T_STR   (s)

    func1();

    T_END()
} /* main */