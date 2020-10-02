/* ******************************************************* */
/*                                                         */
/* This program accepts two dates in time and calculates   */
/* the difference between dates.                           */
/*                                                         */
/* Program written by John Scarfone using Microsoft C 3.00 */
/*                                                         */
/* ******************************************************* */

#include <stdio.h>

main()
{
   long result;
   long result1;
   long result2;
   long date();


   printf("Datedif  ver 1.00\n");

   printf("Enter old date format yyyy mm dd ");
   result1 = date();

   printf("\nEnter new date format yyyy mm dd ");
   result2 = date();

   result = result2 - result1;
   printf("\nThe difference between dates is %ld days.\n", result);
}

long date()

{
   int year;
   int mon;
   int day;
   int n;

   n = 0l;

   scanf("%d %d %d", &year, &mon, &day);

   if (mon <= 2)
    {
      year = year - 1;
      mon = mon + 13;
    }
   else
      mon = mon + 1;

   n = (1461 * year) / 4 + (153 * mon) / 5 + day;

   return (n);
}
