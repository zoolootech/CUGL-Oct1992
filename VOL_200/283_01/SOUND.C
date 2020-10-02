
#include "ciao.h"

/*
**  sound generation primitives
*/

void setfrq( frequency ) unsigned frequency;
{
     static int C;
     static long Hz = 1193180;

     C = Hz / frequency;
     outp( 67, 182 );
     outp( 66, C % 256 );
     outp( 66, C / 256 );
}


void spkron()
{
     static int bits;

     bits = inp( 97 );
     outp( 97, (char) (bits | 0x03) );
}


void spkroff()
{
     static int bits;

     bits = inp( 97 );
     outp( 97, (char) (bits & 0xFC) );
}



/*
**  generate tone f for duration d
**  f = 0 is silence for duration d
*/

void note( f,d ) unsigned f,d;
{
     if ( f > 0)
     {
        setfrq( f );
        spkron();
        while ( d-- > 0)
        ;
        spkroff();
     }
     else 
     {
        d = d = d;
        while ( d-- > 0)
        ;
     }
}




/* 
**  musical gadgets:
**  all except thurb(), an error grumble, respond to the tone flag
*/


int tone = 1;  /* controls whether the music gadgets make noise or not */


void HIclack()
{
     if (tone)
     {
     note (  466, 38 );
     note ( 1865, 27 );
     note (  466, 60 );
     note ( 1865, 30 );
     note (  466, 28 );
     note ( 1865, 17 );
     }
}



void LOclack()
{
     if (tone)
     {
     note (  392, 38 );
     note ( 1568, 27 );
     note (  392, 60 );
     note ( 1568, 30 );
     note (  392, 28 );
     note ( 1568, 17 );
     }
}


void bopbleet()
{
     if (tone)
     {
     note ( 587, 2000 );
     note ( 784, 4000 );
     note ( 880, 2000 );
     }
}


void blopbloop()
{
     if (tone)
     {
     note ( 392, 4000 );
     note ( 440, 2000 );
     note ( 294, 2000 );
     }
}


void thurb()
{
     note (  46, 250 );
     note ( 185, 600 );
     note ( 740, 350 );
     note (  46, 150 );
     note ( 185, 800 );
     note ( 740, 250 );
     note (  46, 250 );
     note ( 185, 600 );
     note ( 740, 350 );   /* 3200 */

     note (  46, 250 );
     note ( 185, 600 );
     note ( 740, 350 );
     note (  46, 400 );
     note ( 185, 400 );
     note ( 740, 400 );
     note (  46, 250 );
     note ( 185, 600 );
     note ( 740, 350 );  /* 6400 */

     note (  98, 1600 ); /* 8000 */
}


