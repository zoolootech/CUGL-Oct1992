
/* GetLongDate --> Taken from CUG-273 */

#include <stdek.h>
#include <gadgets.h>
#include <dos.h>
#include <stdio.h>
#include <stdlib.h>

long int GetLongDate(void)
/* Return: The current date in the format YYMMDD. */
{
    struct date today;
    char Text[40];

    getdate(&today);
    sprintf(Text, "%2.2d%2.2d%2.2d", today.da_year - 1900,
       today.da_mon, today.da_day);
    return atol(Text);
}

