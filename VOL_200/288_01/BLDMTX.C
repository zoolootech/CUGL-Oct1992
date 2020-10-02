/*
        HEADER:         CUG000.12;
        TITLE:          BuildMatrix (main), InputMatrix, OutputMatrix;
        DATE:           Mar 89;
        DESCRIPTION:    Prompts User for Data to Build Distance Matrix;
        VERSION:        2.0;
        FILENAME:       BLDMTX.C;
        SEE-ALSO:       TSP.C, NN.C, POPT.C, 2OPT.C, HYBRID.C, 3OPT.C, FN.C,
                        BOOLEAN.H, NODELIST.H, TSP.H,
                        PRTMTX.C, TIME.C;
        AUTHORS:        Kevin E. Knauss;
*/

#include <stdio.h>
#include <tsp.h>

main ()
{
   char *Names[];
   unsigned Network[MAXROWS][MAXCOLS];
   unsigned NumberOfVirteces;
   char *filename;
   FILE *fp;

   printf("\n\n");
   NumberOfVirteces = InputMatrix (Network, Names);
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
      if (filename == "stdout")
         fp = stdout;
      else
         if ((fp = fopen (filename, "w")) == NULL) {
            printf("\n*******************************************");
            printf("\n   Execution Terminated - Invalid Open!");
            printf("\n*******************************************\n\n");
            exit(-1);
         }
   OutputMatrix (fp, Network, Names, NumberOfVirteces);
   /* Clean up and go */
      fclose (fp);
      printf("\n*******************************************");
      printf("\n        Execution Terminated Normally!");
      printf("\n*******************************************\n\n");
} /* BuildMatrix - main */
   

unsigned InputMatrix (Network, Names)
   unsigned *Network[];
   char *Names[];
{
   unsigned NumberOfVirteces;
   unsigned index, FirstIndex, SecondIndex, dist;
   char *Answer;
   char ch;
   long atol ();
   printf("Input number of virteces to be entered: ");
   scanf ("%u", NumberOfVirteces);
   for ( index = 1; index <=  NumberOfVirteces; index++) {
      printf("Input name for virtex #%2u: ", index);
      scanf("%s", Names [index]);
   }
   printf("Are the distances between virteces symetric? (Y|N): ");
   scanf ("%s", Answer);
   if ((Answer [0] == 'n') || (Answer [0] == 'N')) {
      FirstIndex = 1;
      do {
         SecondIndex = 1;
         do {
            if (FirstIndex != SecondIndex) {
               printf("Input distance from %s", Names [FirstIndex]);
               printf(" to %s:", Names [SecondIndex]);
               scanf ("%s", Answer);
               ch = Answer [0];
               if ((ch == 'b') || (ch == 'B') ||
                  ((ch != NULL) && (ch < ' '))) {
                  FirstIndex = NumberOfVirteces;
                  SecondIndex = FirstIndex + 1;
               }
               if (ch == NULL) {
                  printf("\nWARNING: Matrix will not be complete!\n");
                  Network [FirstIndex][SecondIndex] = MAXCARD;
                  SecondIndex++;
               } else {
                  if ((ch != 'd') && (ch != 'D'))
                     dist = (unsigned) atol(Answer);
                  if (dist > 0) {
                     Network [FirstIndex][SecondIndex] = dist;
                     SecondIndex = SecondIndex + 1;
                  } else {
                     if (ch == '-') {
                        do {
                           if (--SecondIndex < 1) {
                              if (FirstIndex > 1) {
                                 FirstIndex--;
                                 SecondIndex = NumberOfVirteces;
                              } else {
                                 FirstIndex = 1;
                                 SecondIndex = 2;
                              }
                           }
                        } while (FirstIndex == SecondIndex);
                     }
                  }
               }
            } else {
               Network [FirstIndex][SecondIndex] = MAXCARD;
               SecondIndex++;
            }
         } while (SecondIndex <= NumberOfVirteces);
         FirstIndex++;
      } while (FirstIndex <= NumberOfVirteces);
   } else {
      FirstIndex = 1;
      do {
         SecondIndex = FirstIndex;
         do {
            if (FirstIndex != SecondIndex) {
               printf("Input distance from %s", Names [FirstIndex]);
               printf(" to %s: ", Names [SecondIndex]);
               gets (Answer);
               ch = Answer [0];
               if ((ch == 'b') || (ch == 'B') ||
                  ((ch != NULL) && (ch < ' '))) {
                  FirstIndex = NumberOfVirteces;
                  SecondIndex = FirstIndex + 1;
               }
               if (ch == NULL) {
                  printf("\nWARNING: Matrix will not be complete!");
                  Network [FirstIndex][SecondIndex] = MAXCARD;
                  Network [SecondIndex][FirstIndex] = MAXCARD;
                  SecondIndex++;
               } else {
                  if ((ch != 'd') && (ch != 'D'))
                     dist = (unsigned) atol (Answer);
                  if (dist > 0) {
                     Network [FirstIndex][SecondIndex] = dist;
                     Network [SecondIndex][FirstIndex] = dist;
                     SecondIndex++;
                  } else { 
                     if (ch == '-') {
                        if (--SecondIndex <= FirstIndex) {
                           if (FirstIndex > 1) {
                              FirstIndex--;
                              SecondIndex = NumberOfVirteces;
                           } else {
                              FirstIndex = 1;
                              SecondIndex = 2;
                           }
                        }
                     }
                  }
               }
            } else {
               Network [FirstIndex][SecondIndex] = MAXCARD;
               SecondIndex++;
            }
         } while (SecondIndex <= NumberOfVirteces);
         FirstIndex++;
      } while (FirstIndex <= NumberOfVirteces);
   }
   return (NumberOfVirteces);
} /* InputMatrix */


unsigned OutputMatrix (fp, Network, Names, NumberOfVirteces)
   FILE *fp;
   unsigned *Network[];
   char *Names[];
   unsigned NumberOfVirteces;
{   
   unsigned index, FirstIndex, SecondIndex;
   char *Answer;

   printf("%2u\n", NumberOfVirteces);
   for ( index = 1; index <=  NumberOfVirteces; index++)
      printf("%s\n", Names [index]);
   for ( FirstIndex = 1; FirstIndex <=  NumberOfVirteces; FirstIndex++)
      for ( SecondIndex = 1; SecondIndex <=  NumberOfVirteces; SecondIndex++)
         printf("%5u\n", Network [FirstIndex][SecondIndex]);
} /* OutputMatrix */
