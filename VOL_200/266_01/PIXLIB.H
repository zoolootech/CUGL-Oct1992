/*
   PIXLIB is a file of compiler-independent routines for use in
   device drivers for pixel oriented devices.
                                                 File: PIXLIB.H
*/
#ifndef FILE
#include <stdio.h>
#endif

int Bit (int N);
/*  ===             Returns 2 to the N power (i.e., Nth bit on)  */

int Dotter (int Type);
/*  ======                 Provides dot patterns for lines */

int GetWord (FILE *From);
/*  =======                 reads a binary integer */

void HatchHow (int Row, int *Hit, int *Gap);
/*   ======== */
                  /* Provides dot-on params for given row for  */
                  /*  the hatching pattern currently in effect */

int InBounds (int X,int Y);
/*  ======== */

void SetHatch (int Pattern, int Density);
/*   ========                       Sets the hatching pattern */

void SetLine (int Ltype);
/*   =======                         Sets the LineType */

int Sign (int X);
/*  ====                            Returns 1 with sign of X  */

