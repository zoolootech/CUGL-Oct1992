/*                                               File: PLOTEPS2.C
      MicroPLOX plot routine
      copyright 02 May 1990, Robert L. Patton, Jr.
   Produces the plot image on a dot-matrix printer that is compatible
   with most any Epson by reading the PLOX metafile PLOTCOM.DAT.
        WITH IMAGLIB, LPTR, PIXLIB
*/
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "PLOX.H"
#include "imaglib.h"
#include "lptr.h"
#include "pixlib.h"

#define PRINTYPE "GENERIC"
#define HSPACE   1       /* horizontal text spacing, bit mapped fonts */
#define DX(A) (A*5)/6

#include "printdev.c"

