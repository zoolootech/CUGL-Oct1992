/*
        HEADER:         CUG000.04;
        TITLE:          Hybrid;
        DATE:           Mar 89;
        DESCRIPTION:    Point Proximity/2-Opt Hybrid Tour Improvement Algorithm;
        VERSION:        2.0;
        FILENAME:       HYBRID.C;
        SEE-ALSO:       TSP.C, NN.C, POPT.C, 2OPT.C, 3OPT.C, FN.C,
                        BOOLEAN.H, NODELIST.H, TSP.H,
                        BLDMTX.C, PRTMTX.C, TIME.C;
        AUTHORS:        Kevin E. Knauss;
*/

#include <nodelist.h>

long Hybrid (NumberOfVirteces, Path)
   unsigned NumberOfVirteces;
   NODE *Path;
{
   unsigned ArcCost();
   long CircuitCost;
   unsigned count, index, pindex, sindex, k;
   unsigned D1, D2, D3, D4;
   NODE *First, *Last, *Kth, *Save, *Reverse;

   count = 1;
   First = Path;
   do {
      Last = First->prior;
      Kth = First->next;
      do {
         D1 = ArcCost (Kth->position, Kth->next->next->position) /* Point-Opt */
            + ArcCost (First->position, Kth->next->position)
            + ArcCost (Kth->next->position, Last->position);
         D3 = ArcCost (First->position, Last->position)
            + ArcCost (Kth->position, Kth->next->position)
            + ArcCost (Kth->next->position, Kth->next->next->position);
         D2 = ArcCost (First->position, Kth->next->position)  /* 2-Opt */
            + ArcCost (Kth->position, Last->position);
         D4 = ArcCost (First->position, Last->position)
            + ArcCost (Kth->position, Kth->next->position);
         if ((D1 < D3) || (D2 < D4)) {
            if (D1 < D3) {
               Save = Kth->next;
               Kth->next = Kth->next->next;
               Kth->next->prior = Kth;
               Last->next = Save;
               First->prior = Save;
               Save->next = First;
               Save->prior = Last;
            } else {
               for ( Reverse = First; Reverse != Kth; Reverse = Save) {
                  Save = Reverse->next;
                  Reverse->next = Reverse->prior;
                  Reverse->prior = Save;
               }
               First->next = Kth->next;
               Kth->next->prior = First;
               Kth->next = Kth->prior;
               Kth->prior = Last;
               Last->next = Kth;
            }
            count = 0;
            First = Last->next;
            Kth = First->next;
         } else
            Kth = Kth->next;
      } while ((Kth != Last->prior->prior) && (count != 0));
      if (count != 0)
         First = First->next;
      count++;
   } while (count < NumberOfVirteces);
   Last = First->prior;
   CircuitCost = ArcCost (Last->position, First->position);
   for ( Kth = First; Kth != Last; Kth = Kth->next)
      CircuitCost += ArcCost (Kth->position, Kth->next->position);
   return (CircuitCost);
} /* Hybrid */
