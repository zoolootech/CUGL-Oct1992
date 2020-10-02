
#include <time.h>
#include "ciao.h"

/*
**  noop definition for #defines in debug test files
*/

void noop()
{
     ;
}

void bell() { putch('\a'); }


void sleep( tenths ) int tenths;
{
     time_t start, finish;
     float value;

     value = (float) tenths / 10.0;    /* convert 20 tenths to 0.2 seconds */
     time(&start);
     do
     {
          time(&finish);
     }
     while (difftime(finish,start) < value);

}

