                                        /* Chapter 3 - Program 9 */
main()
{
int x1,x2,x3;

   printf("Centigrade to Farenheit temperature table\n\n");

   for(x1 = -2;x1 <= 12;x1 = x1 + 1){
      x3 = 10 * x1;
      x2 = 32 + (x3 * 9)/5;
      printf("  C =%4d   F =%4d  ",x3,x2);
      if (x3 == 0)
         printf(" Freezing point of water");
      if (x3 == 100)
         printf(" Boiling point of water");
      printf("\n");
   }
}



/* Result of execution

Centigrade to Farenheit temperature table

  C = -20   F =  -4
  C = -10   F =  14
  C =   0   F =  32   Freezing point of water
  C =  10   F =  50
  C =  20   F =  68
  C =  30   F =  86
  C =  40   F = 104
  C =  50   F = 122
  C =  60   F = 140
  C =  70   F = 158
  C =  80   F = 176
  C =  90   F = 194
  C = 100   F = 212   Boiling point of water
  C = 110   F = 230
  C = 120   F = 248

*/
