/*
        HEADER:         CUG000.01;
        TITLE:          NearNeighbor, CheapArc;
        DATE:           Mar 89;
        DESCRIPTION:    Nearest Neighbor Tour Building Algorithm;
        VERSION:        2.0;
        FILENAME:       NN.C;
        SEE-ALSO:       TSP.C, POPT.C, 2OPT.C, HYBRID.C, 3OPT.C, FN.C,
                        BOOLEAN.H, NODELIST.H, TSP.H,
                        BLDMTX.C, PRTMTX.C, TIME.C;
        AUTHORS:        Kevin E. Knauss;
*/

#include <stdio.h>
#include <boolean.h>
#include <nodelist.h>
#include <tsp.h>

long NearNeighbor (NumberOfVirteces, SavePath)
   unsigned NumberOfVirteces;
   NODE *SavePath;
{
   unsigned ArcCost(), CheapArc();
   long CircuitCost;
   unsigned NewVirtex, ToIndex, CurrentVirtex, FromIndex;
   BOOLEAN Available[MAXROWS][MAXCOLS];
   NODE *Path;

   CircuitCost = 0;
   for ( FromIndex = 1; FromIndex <= NumberOfVirteces; FromIndex++) {
      for ( ToIndex = 1; ToIndex <= NumberOfVirteces; ToIndex++)
         Available [FromIndex][ToIndex] = TRUE;
      Available [FromIndex][FromIndex] = FALSE;
   }
   /* STEP 1: Find virtex from which cheapest arc eminates */
      CurrentVirtex = 1;
      NewVirtex = CheapArc (CurrentVirtex, NumberOfVirteces, Available);
      for ( FromIndex = 2; FromIndex <= NumberOfVirteces; FromIndex++) {
         ToIndex = CheapArc (FromIndex, NumberOfVirteces, Available);
         if (ArcCost (FromIndex, ToIndex) <
           ArcCost (CurrentVirtex, NewVirtex)) {
            CurrentVirtex = FromIndex;
            NewVirtex = ToIndex;
         }
      }
   /* STEP 2: Establish current virtex as base of path */
      Path = SavePath;
      Path->position = CurrentVirtex;
   /* STEP 7 Init */
   do {
      /* STEP 3: Set all paths unavailable to the current virtex */
         for ( FromIndex = 1; FromIndex <= NumberOfVirteces; FromIndex++)
            Available [FromIndex][CurrentVirtex] = FALSE;
      /* STEP 4: Add cheapest arc from current virtex to cost of path */
         CircuitCost += ArcCost (CurrentVirtex, NewVirtex);
      /* STEP 5: Add new virtex to path */
      if ((Path->next = calloc (1, sizeof(NODE))) == NULL) {
         printf("\n*******************************************");
         printf("\n Execution Terminated - Insuficient Memory!");
         printf("\n*******************************************\n\n");
         exit(-1);
      } else {
         Path->next->prior = Path;
         Path = Path->next;
         Path->position = NewVirtex;
      }
      /* STEP 6: Establish the new virtex as the current virtex */
         CurrentVirtex = NewVirtex;
      /* STEP 7: Find the next virtex at the opposite end of the cheapest
                 arc from current virtex; if found, continue with step 3 */
         NewVirtex = CheapArc (CurrentVirtex, NumberOfVirteces, Available);
   } while (Available [CurrentVirtex][NewVirtex]);
   /* STEP 8: Find the cost of the arc between the new current virtex and
              the initial virtex in the path and add to cost of path */
      SavePath->prior = Path;
      Path->next = SavePath;
      CircuitCost += ArcCost (CurrentVirtex, SavePath->position);
      return (CircuitCost);
} /* NearNeighbor */

unsigned CheapArc (FromIndex, NumberOfVirteces, Available)
   unsigned FromIndex, NumberOfVirteces;
   BOOLEAN Available[MAXROWS][MAXCOLS];
{
   unsigned ArcCost();
   unsigned ToIndex, NewIndex;

   for ( ToIndex = 1; ((ToIndex < NumberOfVirteces) &&
      (!Available [FromIndex][ToIndex])); ToIndex++);
   for ( NewIndex = ToIndex + 1; NewIndex <= NumberOfVirteces; NewIndex++)
      if (Available [FromIndex][NewIndex])
         if (ArcCost (FromIndex, NewIndex) < ArcCost (FromIndex, ToIndex))
            ToIndex = NewIndex;
   return (ToIndex);
} /* CheapArc */
