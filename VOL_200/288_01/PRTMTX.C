/*
        HEADER:         CUG000.13;
        TITLE:          PrintMatrix (main), ArcCost, TSPOutput;
        DATE:           Mar 89;
        DESCRIPTION:    Prints Distance Matrix in Readable Format;
        VERSION:        2.0;
        FILENAME:       PRTMTX.C;
        SEE-ALSO:       TSP.C, NN.C, POPT.C, 2OPT.C, HYBRID.C, 3OPT.C, FN.C,
                        BOOLEAN.H, NODELIST.H, TSP.H,
                        BLDMTX.C, TIME.C;
        AUTHORS:        Kevin E. Knauss;
*/


#include <stdio.h>
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
   unsigned NumberOfVirteces;
   unsigned CircuitCost, OrigCost;
   unsigned count, index;
   unsigned FirstIndex, SecondIndex;
   char filename[MAXLINE], ValueString[MAXLINE];
   long atol ();
   FILE *fp;

   /* Input network to be printed */
      printf("\n\n\n****************************************");
      printf("\n\nInput filename for input: ");
      if (gets(filename) == NULL) {
         printf("\n*******************************************");
         printf("\n   Execution Terminated At User Request!");
         printf("\n*******************************************\n\n");
         exit(-1);
      }
      printf("\n\nInput will be read from: %s\n\n", filename);
      if ((fp = fopen (filename, "r")) == NULL) {
         printf("\n*******************************************");
         printf("\n   Execution Terminated - Invalid Open!");
         printf("\n*******************************************\n\n");
         exit(-1);
      }
      fscanf (fp, "%u", &NumberOfVirteces);
      printf("\nVirteces = %u\n", NumberOfVirteces);
      for ( index = 1; index <= NumberOfVirteces; index++) {
         fscanf (fp, "%s", Names [index]);
         printf("%2u -- %s\n", index, Names [index]);
      }
      for ( FirstIndex = 1; FirstIndex <= NumberOfVirteces; FirstIndex++) {
         for ( SecondIndex = 1; SecondIndex <= NumberOfVirteces;
                                                            SecondIndex++) {
            fscanf (fp, "%s", ValueString);
            Network [FirstIndex][SecondIndex] = (unsigned) atol (ValueString);
            printf("%5u", Network[FirstIndex][SecondIndex]);
         }
         printf("\n");
      }
      fclose (fp);
   /* Open output and print headers */
      printf("\n\n\n****************************************");
      printf("\n\nInput filename for output: ");
      if (gets(filename) == NULL) {
         printf("\n*******************************************");
         printf("\n   Execution Terminated At User Request!");
         printf("\n*******************************************\n\n");
         exit(-1);
      }
      printf("\n\nOutput will be written to: %s\n\n", filename);
      if (!strcmp(filename, "stdout"))
         fp = stdout;
      else
         if ((fp = fopen (filename, "w")) == NULL) {
            printf("\n*******************************************");
            printf("\n   Execution Terminated - Invalid Open!");
            printf("\n*******************************************\n\n");
            exit(-1);
         }
      printf("\n   File opened.\n");
      TSPOutput (fp, NumberOfVirteces);
   /* Clean up and go */
      fclose (fp);
      printf("\n*******************************************");
      printf("\n        Execution Terminated Normally!");
      printf("\n*******************************************\n\n");
} /* PrintMatrix - main */


TSPOutput (fp, NumberOfVirteces)
   FILE *fp;
   unsigned NumberOfVirteces;
{
   unsigned ArcCost ();
   unsigned Cost, index, FirstIndex, SecondIndex, Step, Last;

   printf("\n     Printing ...\n");
   fprintf(fp, "\n          ");
   fprintf(fp, "T R A V E L I N G   S A L E S M A N   P R O B L E M");
   fprintf(fp, "\n\n\nCost data for the");
   fprintf(fp, " %u city problem\n\n", NumberOfVirteces);
/* Write MATRIX with labels */
   for ( Step = 1; Step <= NumberOfVirteces; Step += 14) {
      Last = Step + 13;
      if (Last > NumberOfVirteces)
         Last = NumberOfVirteces;
      fprintf(fp, "    ");
      for ( index = Step; index <= Last; index++)
         fprintf(fp, "%5u", index);
      fprintf(fp, "\n     ");
      for ( index = Step; index <= Last; index++)
         fprintf(fp, " ====");
      fprintf(fp, "\n");
      for ( FirstIndex = 1; FirstIndex <=  NumberOfVirteces; FirstIndex++) {
         fprintf(fp, "%3u |", FirstIndex);
         for ( SecondIndex = Step; SecondIndex <=  Last; SecondIndex++) {
            Cost = ArcCost (FirstIndex, SecondIndex);
            if ((FirstIndex != SecondIndex) && (Cost != MAXCARD))
               fprintf(fp, "%5u", Cost);
            else
               fprintf(fp, "    X");
         }
         fprintf(fp, "\n");
      }
      fprintf(fp, "     ");
      for ( index = Step; index <= Last; index++)
         fprintf(fp, " ====");
      fprintf(fp, "\n    ");
      for ( index = Step; index <= Last; index++)
         fprintf(fp, "%5u", index);
      fprintf(fp, "\n\n\n\n\n");
   }
} /* TSPOutput */
