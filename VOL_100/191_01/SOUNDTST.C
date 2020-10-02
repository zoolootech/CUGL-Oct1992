
#include "sound.h"

/* ------------------------------------------------------------ */
/*      This is a portion of the SOUND EFFECTS LIBRARY.         */
/*                                                              */
/*      Copyright (C) 1986 by Paul Canniff.                     */
/*      All rights reserved.                                    */
/*                                                              */
/*      This library has been placed into the public domain     */
/*      by the author.  Use is granted for non-commercial       */
/*      pusposes, or as an IMBEDDED PORTION of a commercial     */
/*      product.                                                */
/*                                                              */
/*      Paul Canniff                                            */
/*      PO Box 1056                                             */
/*      Marlton, NJ 08053                                       */
/*                                                              */
/*      CompuServe ID: 73047,3715                               */
/*                                                              */
/* ------------------------------------------------------------ */


main()
{
    static char template[] = "t%d c d e f a b";
    char temp[256];
    long f;
    int i,u,r;

    printf("SOUND_INIT returns %d\n",sound_init(4,1));


    play("L1");
    for (i=8; i<=64; i+=8)
    {
        sprintf(temp,template,i);
        play(temp);
    }

    for (f=1; f<=120; f++) 
    {
        sound_bchk(&u,&r);
        printf("Waiting u %-3d r %-3d%c",u,r,((f & 0x3) == 0) ? '\n' : ' ');
    }
 
    sound_done();

    exit(0);
}



