
        /******************************************************
        *
        *        file d:\cips\gpcips.c
        *
        *       Functions: This file contains
        *           get_parameters
        *           show_parameters
        *
        *       Purpose - These functions get image parameters.
        *
        *       External Calls:
        *           rstring.c - read_string
        *           intcvrt.c - get_integer
        *
        *       Modifications:
        *           19 February 1987 - These functions were taken out
        *                of the file ip.c.
        *           28 June 1990 - changed to gpcips and the channel
        *                parameter was removed.
        *
        *******************************************************/


#include "d:\cips\cips.h"




get_parameters(il, ie, ll, le)
        int *il, *ie, *le, *ll;
{
        int choice, not_finished;

        not_finished = 1;
        while(not_finished){
           show_parameters(il, ie, ll, le);
           printf("\n\nEnter choice to change (enter 0 for no changes) __\b\b");
           get_integer(&choice);

           switch (choice){
              case 0:
                 not_finished = 0;
                 break;

              case 1:
                 break;

              case 2:
                 printf("\nEnter initial line\n___\b\b\b");
                 get_integer(il);
                 break;

              case 3:
                 printf("\nEnter initial element\n___\b\b\b");
                 get_integer(ie);
                 break;

              case 4:
                 printf("\nEnter last line\n___\b\b\b");
                 get_integer(ll);
                 break;

              case 5:
                 printf("\nEnter last element\n___\b\b\b");
                 get_integer(le);
                 break;

           }  /* ends switch choice       */
        }     /* ends while not_finished  */
}             /* ends get_parameters      */


show_parameters(il, ie, ll, le)
        int *il, *ie, *le, *ll;
{
   printf("\n\nThe image parameters are:");
   printf("\n\t2.      il = %4d", *il);
   printf("\n\t3.      ie = %4d", *ie);
   printf("\n\t4.      ll = %4d", *ll);
   printf("\n\t5.      le = %4d", *le);
}  /* ends show_parameters  */
