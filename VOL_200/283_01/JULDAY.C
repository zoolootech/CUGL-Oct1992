
/* julday.c  - convert a julian day number (double) to mm, dd, yyyy, & v.v.
               see also: Press, et al., Numerical Recipes, pp. 681, 682 

   This is the business, or non-astronomical, version of julian days.
   In other words, the julian day number assumes local noon; there is
   no fractional part to the julian day number indicating time of day.

   Does not recognize 400-year leapyear cycle; 
   i.e., incorrectly assumes that 2000 A.D. is a leap year.

*/

#include <stdio.h>
#include <stdlib.h>

/* after some nicely-done Pascal routines by Carley Phillips, 4/10/88, 
   converted to C, 9/8/88, by d.c.oshel
   */

#define inrange(J) ((julianmin <= J) && (J <= julianmax))

#define MDYtoJul(M,D,Y) mdy2jul((long)M,(long)D,(long)Y)

static long julianconstant =  1721119L, /* Julian Day for 02-28-0000  */
            julianmin      =  1721120L, /* Julian Day for 03-01-0000  */
            julianmax      = 25657575L, /* Julian Day for 12-31-65535 */
            julianDOS      =  2444240L, /* Julian Day for 01-01-1980  */ 
            juliannil      =        0L; /* error return */

            
/* sets month, day, year to Julian Day number */

void jul2mdy (  long jday,     /* valid Julian Day number */
                int * mm,      /* Gregorian month returned */
                int * dd,      /* Gregorian day   returned */
                int * yy       /* Gregorian year  returned */
                )
{
   long tmp1, tmp2, year, month, day;

   if ( inrange( jday ) )
   {
          tmp1  = jday - julianconstant; /* will be at least 1 */
          year  = ((tmp1-1) / 1460969L) * 4000L;
          tmp1  = ((tmp1-1) % 1460969L) + 1L;
          tmp1  = (4   *   tmp1) - 1L;
          tmp2  = (4   * ((tmp1 % 146097L) / 4)) + 3;
          year  = (100 *  (tmp1 / 146097L)) + (tmp2 / 1461L) + year;
          tmp1  = (5 * (((tmp2 % 1461L) + 4) / 4)) - 3;
          month =   tmp1 / 153;
          day   = ((tmp1 % 153) + 5) / 5;
          if (month < 10)
               month  += 3;
          else
          {
               month  -= 9;
               year += 1;
          }
   }
   else
   {
         month = 0L;
         day   = 0L;
         year  = 0L;
   }
   *mm = (int) month;
   *dd = (int) day;
   *yy = (int) year;
}



/*  returns Julian Day number  */

long mdy2jul (  int month,      /* Gregorian calendar month */
                int day,        /* Gregorian calendar day */
                int year        /* Gregorian calendar year */
                )
{
   long tmon, tyear, jday;

   if (month > 2)
   {
         tmon  = month - 3;
         tyear = year;
   }
   else
   {
         tmon  = month + 9;
         tyear = year - 1;
   }
   jday  = (tyear / 4000L) * 1460969L;
   tyear = (tyear % 4000L);
   jday  = jday +
            (((tyear / 100) * 146097L) / 4) +
            (((tyear % 100) *   1461L) / 4) +
            (((153   *  tmon) +      2) / 5) +
            day +
            julianconstant;
   if ( !inrange( jday ) )
      jday = juliannil;

   return ( jday );
}



/* returns 1 thru 7 for Sun thru Sat, 0=invalid */

int day_of_week( long jday )
{
   return ( inrange( jday ) ? (((jday + 1) % 7) + 1) : 0 );
}


