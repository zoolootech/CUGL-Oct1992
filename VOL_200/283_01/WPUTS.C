/* wputs.c -- 9/5/88, d.c.oshel
   */

#include "vidinit.h"

void wputs( char *p )  /* write string into window, uses wink */
{
     static int test;
     static int temp;

     if (!Initialized) vid_init(0);
 
     temp = synchronized;
     synchronized = 0;
     while (*p) 
     {
         if (*p == '^')  /* escape character? switch modes */
         {
             p++;              /* read escape command char */
             test = *p & 0xFF; /* kill sign extension */
             switch ( test ) 
             {
                 case  '0': { vid_attr = vid[ 0]; break; }
                 case  '1': { vid_attr = vid[ 1]; break; }
                 case  '2': { vid_attr = vid[ 2]; break; }
                 case  '3': { vid_attr = vid[ 3]; break; }

                 case  '4': { vid_attr = vid[ 4]; break; }
                 case  '5': { vid_attr = vid[ 5]; break; }
                 case  '6': { vid_attr = vid[ 6]; break; }
                 case  '7': { vid_attr = vid[ 7]; break; }
                 case  '8': { vid_attr = vid[ 8]; break; }
                 case  '9': { vid_attr = vid[ 9]; break; }

                 case  'ê': { vid_attr = vid[10]; break; } /* ea */
                 case  'ë': { vid_attr = vid[11]; break; } /* eb */
                 case  'ì': { vid_attr = vid[12]; break; } /* ec */
                 case  'í': { vid_attr = vid[13]; break; } /* ed */
                 case  'î': { vid_attr = vid[14]; break; } /* ee */
                 case  'ï': { vid_attr = vid[15]; break; } /* ef */

                 case  '^': { 
                            wink( *p );  /* verbatim ^ char */
                            break; 
                            }
                 default:   { 
                            --p; 
                            vid_attr = vid[0];  /* just ^, no command arg */
                            break; 
                            }
             }
             p++;  /* next after escape command */
         }
         else 
         {
             wink( *p++ );  /* obeys newline, tab, if present */
         }
     }
     /* update machine cursor only at last, if at all */
     if ((synchronized = temp))
        setmchnloc();

} /* wputs */
