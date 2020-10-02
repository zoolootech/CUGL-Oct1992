/*
        HEADER:         CUG000.08;
        TITLE:          TSP (main), ArcCost, Terminate, PrintCircuit,
                        CalculateImprovements, PrintImprovement;
        DATE:           Mar 89;
        DESCRIPTION:    Traveling Salesman Problem Driver;
        VERSION:        2.0;
        FILENAME:       TSP.C;
        SEE-ALSO:       NN.C, POPT.C, 2OPT.C, HYBRID.C, 3OPT.C, FN.C,
                        BOOLEAN.H, NODELIST.H, TSP.H,
                        BLDMTX.C, PRTMTX.C, TIME.C;
        AUTHORS:        Kevin E. Knauss;
*/

#include <stdio.h>
#include <nodelist.h>
#include <tsp.h>

unsigned Network[MAXROWS][MAXCOLS];

/* This version of ArcCost to be used with complete matricies */
unsigned ArcCost (FromIndex, ToIndex)
   unsigned FromIndex, ToIndex;
{
   extern unsigned Network[MAXROWS][MAXCOLS];

   return (Network [FromIndex][ToIndex]);
} /* ArcCost */


main ()
{
   extern unsigned Network[MAXROWS][MAXCOLS];
   char Names[MAXROWS][MAXLINE];
   NODE *Path, *Path2, *DuplicatePath;
   long CircuitCost, OrigCost;
   unsigned NumberOfVirteces;
   unsigned index;
   unsigned FirstIndex, SecondIndex;
   long NearNeighbor(), PointOpt(), TwoOpt(), Hybrid(), ThreeOpt();
   long FarNeighbor();
   char filename[MAXLINE], ValueString[MAXLINE];
   long atol();
   long StartTime, TotalTime, GetTime() , ElapsedTime();
   FILE *fp;

   /* Input network to be traversed */
      printf("\n\n\n****************************************");
      printf("\n\nInput filename for input: ");
      if (gets(filename) == NULL)
         Terminate (-1, "   Execution Terminated At User Request!");
      printf("\n   Reading distance matrix ... ");
      StartTime = GetTime ();
      if ((fp = fopen (filename, "r")) == NULL)
         Terminate (-1, "   Execution Terminated - Invalid Open!");
      fscanf (fp, "%u", &NumberOfVirteces);
      for ( index = 1; index <= NumberOfVirteces; index++)
         fscanf (fp, "%s", Names [index]);
      for ( FirstIndex = 1; FirstIndex <= NumberOfVirteces; FirstIndex++)
         for ( SecondIndex = 1; SecondIndex <= NumberOfVirteces;
                                                           SecondIndex++) {
            fscanf (fp, "%s", ValueString);
            Network [FirstIndex][SecondIndex] = (unsigned) atol(ValueString);
         }
      fclose (fp);
      TotalTime = ElapsedTime (StartTime);
      printf("%ld ticks.", TotalTime);
   /* Open output and print headers */
      printf("\n\n\n****************************************");
      printf("\n\nInput filename for output: ");
      if (gets(filename) == NULL)
         Terminate (-1, "   Execution Terminated At User Request!");
      if (filename == "stdout")
         fp = stdout;
      else
         if ((fp = fopen (filename, "w")) == NULL)
            Terminate (-1, "   Execution Terminated - Invalid Open!");
      fprintf(fp, "\n          ");
      fprintf(fp, "T R A V E L I N G   S A L E S M A N   P R O B L E M");
      fprintf(fp, "\n\n\nThe following results were produced for the");
      fprintf(fp, " %u city problem:\n\n\n", NumberOfVirteces);
      fprintf(fp, "Distance matrix was input in %ld ticks.\n\n\n", TotalTime);
   /* Calculate and print initial solution - good starting circuit */
      if ((Path = calloc (1, sizeof(NODE))) == NULL)
         Terminate (-1, "Execution Terminated - Insuficient Memory!");
      printf("\n   Nearhest Neighbor calculation ... ");
      StartTime = GetTime ();
      CircuitCost = NearNeighbor (NumberOfVirteces, Path);
      TotalTime = ElapsedTime (StartTime);
      printf("%ld ticks.", TotalTime);
      fprintf(fp, "Nearest Neighbor solution: \n\n");
      fprintf(fp, "   A circuit with cost %ld", CircuitCost);
      fprintf(fp, " was found in %ld", TotalTime);
      fprintf(fp, " ticks with the following path:\n   ");
      PrintCircuit (fp, NumberOfVirteces, Path);
      fprintf(fp, "\n\n\n");
      OrigCost = CircuitCost;
      if ((DuplicatePath = calloc (1, sizeof(NODE))) == NULL)
         Terminate (-1, "Execution Terminated - Insuficient Memory!");
      Path2 = DuplicatePath;
      Path2->position = Path->position;
      Path = Path->next;
      while (Path->position != DuplicatePath->position) {
         if ((Path2->next = calloc (1, sizeof(NODE))) == NULL)
            Terminate (-1, "Execution Terminated - Insuficient Memory!");
         Path2->next->prior = Path2;
         Path2 = Path2->next;
         Path2->position = Path->position;
         Path = Path->next;
      }
      DuplicatePath->prior = Path2;
      Path2->next = DuplicatePath;
      CalculateImprovements (fp, NumberOfVirteces, OrigCost, Path,
         DuplicatePath);
   /* Calculate and print initial solution - reversed starting circuit */
      fprintf(fp, "\014\n          ");
      fprintf(fp, "T R A V E L I N G   S A L E S M A N   P R O B L E M");
      fprintf(fp, "\n\n\n%u city problem continued:", NumberOfVirteces);
      fprintf(fp, " (reversed nearest neighbor circuit)\n\n\n");
      Path2 = DuplicatePath->prior;
      for ( index = 1; index <= NumberOfVirteces; index++) {
         Path->position = Path2->position;
         Path = Path->next;
         Path2 = Path2->prior;
      }
      Path2 = DuplicatePath;
      for ( index = 1; index <= NumberOfVirteces; index++) {
         Path2->position = Path->position;
         Path = Path->next;
         Path2 = Path2->next;
      }
      fprintf(fp, "Reverse Nearest Neighbor solution: \n\n");
      fprintf(fp, "   A circuit with cost %ld", CircuitCost);
      fprintf(fp, " was found in %ld", TotalTime);
      fprintf(fp, " ticks with the following path:\n   ");
      PrintCircuit (fp, NumberOfVirteces, Path);
      fprintf(fp, "\n\n\n");
      CalculateImprovements (fp, NumberOfVirteces, OrigCost, Path,
         DuplicatePath);
   /* Calculate and print initial solution - poor starting circuit */
      fprintf(fp, "\014\n          ");
      fprintf(fp, "T R A V E L I N G   S A L E S M A N   P R O B L E M");
      fprintf(fp, "\n\n\n%u city problem continued:", NumberOfVirteces);
      fprintf(fp, " (poor starting circuit)\n\n\n");
      printf("\n   Farthest Neighbor calculation ... ");
      StartTime = GetTime ();
      CircuitCost = FarNeighbor (NumberOfVirteces, Path);
      TotalTime = ElapsedTime (StartTime);
      printf("%ld ticks.", TotalTime);
      fprintf(fp, "Farthest Neighbor solution: \n\n");
      fprintf(fp, "   A circuit with cost %ld", CircuitCost);
      fprintf(fp, " was found in %ld", TotalTime);
      fprintf(fp, " ticks with the following path:\n   ");
      PrintCircuit (fp, NumberOfVirteces, Path);
      fprintf(fp, "\n\n\n");
      OrigCost = CircuitCost;
      Path2 = DuplicatePath;
      for ( index = 1; index <= NumberOfVirteces; index++) {
         Path2->position = Path->position;
         Path = Path->next;
         Path2 = Path2->next;
      }
      CalculateImprovements (fp, NumberOfVirteces, OrigCost, Path,
         DuplicatePath);
   /* Clean up and go */
      fclose (fp);
      Terminate (0, "      Execution Terminated Normally!");
} /* TravelingSalesman - main */


Terminate (code, message)
   int code;
   char *message;
{
   printf("\n*******************************************");
   printf("\n %s", message);
   printf("\n*******************************************\n\n\007");
   exit(code);
} /* Terminate */


PrintCircuit (fp, NumberOfVirteces, Path)
   FILE *fp;
   unsigned NumberOfVirteces;
   NODE *Path;
{
   unsigned count, index;

      count = 0;
      for ( index = 1; index <= NumberOfVirteces; index++) {
         if (count >= 18) {
            fprintf(fp, "\n    ");
            count = 1;
         } else {
            count++;
         }
         fprintf(fp, "-%u-", Path->position);
         Path = Path->next;
      }
} /* PrintCircuit */


CalculateImprovements (fp, NumberOfVirteces, OrigCost, Path, DuplicatePath)
   FILE *fp;
   unsigned NumberOfVirteces;
   long OrigCost;
   NODE *Path, *DuplicatePath;
{
   NODE *Path2;
   unsigned index;
   long CircuitCost, StartTime, TotalTime;

      Path2 = DuplicatePath;
      for ( index = 1; index <= NumberOfVirteces; index++) {
         Path->position = Path2->position;
         Path = Path->next;
         Path2 = Path2->next;
      }
   /* Calculate and print improved solution */
      printf("\n   PointOpt calculation ... ");
      StartTime = GetTime ();
      CircuitCost = PointOpt (NumberOfVirteces, Path);
      TotalTime = ElapsedTime (StartTime);
      printf("%ld ticks.", TotalTime);
      fprintf(fp, "PointOpt improvement: \n\n");
      if (OrigCost > CircuitCost) {
         PrintImprovement (fp, NumberOfVirteces, CircuitCost, OrigCost,
            TotalTime, Path);
      } else {
         fprintf(fp, "   No improvement.    %ld ticks\n\n\n", TotalTime);
      }
      Path2 = DuplicatePath;
      for ( index = 1; index <= NumberOfVirteces; index++) {
         Path->position = Path2->position;
         Path = Path->next;
         Path2 = Path2->next;
      }
   /* Calculate and print improved solution */
      printf("\n   TwoOpt calculation ... ");
      StartTime = GetTime ();
      CircuitCost = TwoOpt (NumberOfVirteces, Path);
      TotalTime = ElapsedTime (StartTime);
      printf("%ld ticks.", TotalTime);
      fprintf(fp, "TwoOpt improvement: \n\n");
      if (OrigCost > CircuitCost) {
         PrintImprovement (fp, NumberOfVirteces, CircuitCost, OrigCost,
            TotalTime, Path);
      } else {
         fprintf(fp, "   No improvement.    %ld ticks\n\n\n", TotalTime);
      }
      Path2 = DuplicatePath;
      for ( index = 1; index <= NumberOfVirteces; index++) {
         Path->position = Path2->position;
         Path = Path->next;
         Path2 = Path2->next;
      }
   /* Calculate and print improved solution */
      printf("\n   Hybrid calculation ... ");
      StartTime = GetTime ();
      CircuitCost = Hybrid (NumberOfVirteces, Path);
      TotalTime = ElapsedTime (StartTime);
      printf("%ld ticks.", TotalTime);
      fprintf(fp, "Hybrid improvement: \n\n");
      if (OrigCost > CircuitCost) {
         PrintImprovement (fp, NumberOfVirteces, CircuitCost, OrigCost,
            TotalTime, Path);
      } else {
         fprintf(fp, "   No improvement.    %ld ticks\n\n\n", TotalTime);
      }
      Path2 = DuplicatePath;
      for ( index = 1; index <= NumberOfVirteces; index++) {
         Path->position = Path2->position;
         Path = Path->next;
         Path2 = Path2->next;
      }
   /* Calculate and print improved solution */
      printf("\n   3-Opt calculation ... ");
      StartTime = GetTime ();
      CircuitCost = ThreeOpt (NumberOfVirteces, Path);
      TotalTime = ElapsedTime (StartTime);
      printf("%ld ticks.\n\n", TotalTime);
      fprintf(fp, "3-Opt improvement: \n\n");
      if (OrigCost > CircuitCost) {
         PrintImprovement (fp, NumberOfVirteces, CircuitCost, OrigCost,
            TotalTime, Path);
      } else {
         fprintf(fp, "   No improvement.   %ld ticks\n\n\n", TotalTime);
      }
} /* CalculateImprovements */


PrintImprovement (fp, NumberOfVirteces, CircuitCost, OrigCost,
                                                   TotalTime, Path)
   FILE *fp;
   unsigned NumberOfVirteces;
   long CircuitCost, OrigCost;
   long TotalTime;
   NODE *Path;
{

   fprintf(fp, "   A circuit with cost %ld", CircuitCost);
   fprintf(fp, " was found in %ld", TotalTime);
   fprintf(fp, " ticks with the following path:\n   ");
   PrintCircuit (fp, NumberOfVirteces, Path);
   fprintf(fp, "\n   This solution represents an improvement of ");
   fprintf(fp, "%ld over the initial solution.\n\n\n", (OrigCost - CircuitCost));
} /* PrintImprovement */
