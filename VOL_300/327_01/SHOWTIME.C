#include <stdio.h>
#include <conio.h>
#include <time.h>
#include "panel.h"
#include "keys.h"

float random();

char buf[255], *window =
"size %d %d %d %d\n"
"background 1\n"
"tag $ p\n"
"tag # ph7 time\n"
"$\n"
"  #                         $\n";

main()
    {
    time_t ltime, prev = 0;
    char *stime;
    int row = 9, col = 23;

    pan_init();

    while (!kbhit())
        {
        time(&ltime);

        if (ltime >= prev+10)    /* every 10 seconds relocate time window */
            {
            if (prev != 0)
                pan_destroy();
            row = random() * 22;
            col = random() * 50;
            sprintf(buf, window, row, col, row+2, col+29);
            pan_activate(buf);
            prev = ltime;
            }

        stime = ctime(&ltime);
        stime[24] = 0;
        pan_put_field("time", 1, stime);
        }

    pan_get_key();

    pan_destroy();
    }


/******************************************************************************
*
* Function Name : random()
* Written By    : JJB
* Created       : 03/16/90
* Description   : Produces a pseudo random number.
* Arguments     : none
* Returns       : floating point number between zero and one.
* Modifications :
*
*****************************************************************************/

float random()
   {
   static unsigned long a;
   static int first = 1;

   if (first)
       {
       time(&a);
       first = 0;
       }

   a = (a * 125) % 2796203;
   return ((float) a / 2796203);

   }


