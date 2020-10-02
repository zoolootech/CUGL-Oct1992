/*
        HEADER:         CUG000.06;
        TITLE:          GetTime, ElapsedTime;
        DATE:           Mar 89;
        DESCRIPTION:    Samples Clock Counter to Find Elapsed Time;
        VERSION:        2.0;
        FILENAME:       TIME.C;
        SEE-ALSO:       TSP.C, NN.C, POPT.C, 2OPT.C, HYBRID.C, 3OPT.C, FN.C,
                        BOOLEAN.H, NODELIST.H, TSP.H,
                        BLDMTX.C, PRTMTX.C;
        AUTHORS:        Kevin E. Knauss;
        WARNINGS:       Function GetTime is MIX C Compiler Dependent;
*/

#include <stdlib.h>

#define TicksPerHour 65536

/** ------------------------------------------------------------------
      This module is designed to establish a beginning time (GetTime)
      and total execution time (ElapsedTime) of a procedure or operation.
      Time is measured in ticks which are fractional portions of a second
      (there are 65536 "TicksPerHour" so one tick is about 55 miliseconds).
    ------------------------------------------------------------------ **/

long GetTime()
{
    REGS reg;

    reg.byte.ah = 0;
    bios(0x1A, &reg);
    return ((long) ((unsigned) reg.word.cx) * TicksPerHour
                                                  + (unsigned) reg.word.dx);
} /* GetTime */

long ElapsedTime (TicksBegin)
   long TicksBegin;
{
   long TicksEnd;

   TicksEnd = GetTime ();
   if (TicksEnd < TicksBegin) {  /* midnight passed during interval */
      TicksEnd += (24 * TicksPerHour);
   }
   return (TicksEnd - TicksBegin);
} /* ElapsedTime */
