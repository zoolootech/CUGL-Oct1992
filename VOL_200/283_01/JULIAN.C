
#include <math.h>

/*  Julian Day Number routine.  The julian day begins at noon.  Therefore,
**  our Epoch, 2444238.5, is Midnight, 31 December 1979, aka 0.0 January 1980.
**  If y = 0, the year is 1 B.C.  Note Gregorian reform on October 15, 1582;
**  adjust English calendar dates where required (e.g., George Washington).
*/

double julian( y,m,d )  /* the julian day begins at noon, midnight is +0.5 */
float y,m,d;
{
    double A, B, C, D;

    if ((m == 1.0) || (m == 2.0))
    {
        y -= 1.0; m += 12.0;
    }
    B = 0.0;
    if (y > 1582.0) goto starkle;
    else if (y == 1582.0)
    {
       if (m > 10.0) goto starkle;
       else if ((m == 10.0) && (d >= 15.0))
       {
        starkle: modf( y/100, &A );
                 modf( A/4, &B );
                 B = 2 - A + B;
    }  }
    modf( 365.25 * y, &C ); 
    modf( 30.6001 * (m + 1), &D );
    return (B + C + D + d + 1720994.5);
}

