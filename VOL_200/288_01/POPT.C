/*
        HEADER:         CUG000.02;
        TITLE:          PointOpt;
        DATE:           Mar 89;
        DESCRIPTION:    Point Proximity Tour Improvement Algorithm;
        VERSION:        1.0;
        FILENAME:       POPT.C;
        SEE-ALSO:       TSP.C, NN.C, 2OPT.C, HYBRID.C, 3OPT.C, FN.C,
                        BOOLEAN.H, NODELIST.H, TSP.H,
                        BLDMTX.C, PRTMTX.C, TIME.C;
        AUTHORS:        Kevin E. Knauss;
*/

#include <nodelist.h>

long PointOpt (NumberOfVirteces, Path)
   unsigned NumberOfVirteces;
   NODE *Path;
{
   unsigned ArcCost();
   long CircuitCost;
   int count, BestImprove, EXIST, TEST;
   NODE *First, *Last, *Kth, *Best;

   count = 1;
   First = Path;
   do {
      BestImprove = 0;
      Last = First->prior;
      for ( Kth = First; Kth != Last->prior->prior; Kth = Kth->next) {
         EXIST = ArcCost (Last->prior->position, Last->position)
               + ArcCost (Last->position, First->position)
               + ArcCost (Kth->position, Kth->next->position);
         TEST  = ArcCost (Last->prior->position, First->position)
               + ArcCost (Kth->position, Last->position)
               + ArcCost (Last->position, Kth->next->position);
         if ((EXIST - TEST) > BestImprove) {
            BestImprove = EXIST - TEST;
            Best = Kth;
         }
      }
      if (BestImprove == 0) {
         First = First->next;
         count++;
      } else {
         First->prior = Last->prior;
         Last->prior->next = First;
         Last->prior = Best;
         Last->next = Best->next;
         Best->next = Last;
         Last->next->prior = Last;
         count = 1;
      }
   } while (count < NumberOfVirteces);
   Last = First->prior;
   CircuitCost = ArcCost (Last->position, First->position);
   for ( Kth = First; Kth != Last; Kth = Kth->next)
      CircuitCost += ArcCost (Kth->position, Kth->next->position);
   return (CircuitCost);
} /* PointOpt */
