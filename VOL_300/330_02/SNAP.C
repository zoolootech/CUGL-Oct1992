
/*
   This program installs the tasker, does a snapshot dump, and exits.
   It is useful if you want to peek into a resident copy of CTask.

   Program call is
      SNAPxx      - Dump to STDOUT
      SNAPxx M    - Dump to Mono screen
      SNAPxx C    - Dump to Colour screen
*/

#include <stdlib.h>
#include <ctype.h>

#include "tsk.h"
#include "tsksup.h"
#include "tskprf.h"

int main (int argc, char **argv)
{
   int c;

   install_tasker (0, 0, IFL_STD, "Snap");

   c = (argc < 2) ? 'x' : tolower (argv [1][0]);

   switch (c)
      {
      case 'm': tsk_set_mono (25, 80);
                screensnap (25); 
                break;
      case 'c': tsk_set_colour (25, 80);
                screensnap (25); 
                break;
      default:  snapshot (stdout); 
                break;
      }

   remove_tasker ();
   return 0;
}

