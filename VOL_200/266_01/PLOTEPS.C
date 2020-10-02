/*                                               File: PLOTEPS.C
      MicroPLOX plot routine
      copyright 02 May 1990, Robert L. Patton, Jr.
   Produces the plot image on a dot-matrix printer that is compatible
   with the Epson FX-80 by reading the PLOX metafile PLOTCOM.DAT.
        WITH IMAGLIB, LPTR, PIXLIB
*/
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "PLOX.H"
#include "imaglib.h"
#include "lptr.h"
#include "pixlib.h"

#define PRINTYPE "FX80"
#define HSPACE   2       /* horizontal text spacing, bit mapped fonts */
#define DX(A)    A

#include "printdev.c"

