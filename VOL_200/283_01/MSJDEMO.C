
/* demo.c -- after Microsoft Systems Journal, Nov. 1988, p. 8 */

#include "video.h"
#include <time.h>


int buffer[3][510];  /* screen region save areas */


void delay( double sec )
{
	time_t StartTime, EndTime;

    time (&StartTime);
    time (&EndTime);
    while ( difftime( EndTime,StartTime ) < sec )
        time (&EndTime);
}


main()
{
    char TextColor = 0x07;
    char BorderColor = 0x0F;
    char WinColor1, WinColor2, WinColor3;
    int i,j;

    /* -- init video struc -- */

    MSJ_GetVideoParms( &video );

    /* -- set attributes for monochrome or color -- */

    if ( video.ColorFlag )
    {
        WinColor1 = 0x1F;
        WinColor2 = 0x4F;
        WinColor3 = 0x6F;
    }
    else
    {
        WinColor1 = 0x0F;
        WinColor2 = 0x70;
        WinColor3 = 0x0F;
    }

    /* -- clear screen and fill it with text -- */

    MSJ_ClrScr( TextColor, &video );
    for ( i = 0; i < video.rows; i++ )
        for ( j = 0; j <= 52; j += 26 )
            MSJ_DispString( "Microsoft Systems Journal", i, j, TextColor, 
                        &video );

    delay( 2.0 );

    /* -- open a window -- */
    MSJ_SaveRegion( 5, 2, 14, 34, buffer[0], &video );
    MSJ_ClrRegion( 6, 3, 13, 33, WinColor1 );
    MSJ_TextBox( 5, 2, 14, 34, BorderColor, &video );
    for ( i = 6; i < 14; i++ )
        MSJ_DispString( "Open the first window here...", i, 4, WinColor1,
                    &video );

    delay( 2.0 );

    /* -- open a second window -- */

    MSJ_SaveRegion( 2, 48, 12, 74, buffer[1], &video );
    MSJ_ClrRegion( 3,49, 11, 73, WinColor2 );
    MSJ_TextBox( 2, 48, 12, 74, BorderColor, &video );
    for ( i = 3; i < 12; i++ )
        MSJ_DispString( "Then the second here...", i, 50, WinColor2,
                    &video );

    delay( 2.0 );

    /* -- open a third window overlapping the first two -- */

    MSJ_SaveRegion( 9, 25, 22, 60, buffer[2], &video );
    MSJ_ClrRegion( 10, 26, 21, 59, WinColor3 );
    MSJ_TextBox( 9, 25, 22, 60, BorderColor, &video );
    for ( i = 10; i < 22; i++ )
        MSJ_DispString( "And finally a third window here.", i, 27, WinColor3,
                    &video );

    delay( 4.0 );

    /* -- close all windows and exit -- */
    MSJ_RestRegion( 9, 25, 22, 60, buffer[2], &video );
    delay( 1.0 );
    MSJ_RestRegion( 2, 48, 12, 74, buffer[1], &video );
    delay( 1.0 );
    MSJ_RestRegion( 5,  2, 14, 34, buffer[0], &video );
    delay( 1.0 );
    MSJ_ClrScr( TextColor, &video );

    exit (0);
}
