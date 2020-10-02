/*----------------------------------------------------------------
  Main File : randmtx.exe
  Author    : Bill Forseth, Fargo ND
  Purpose   : generates an n(n+1) random matrix of integers
  Syntax    : randmtx n [S] <where n is the size of one column,
                S is the seed (optional)>
  Randomization : Standard Turbo C random functions - initialization
    via system clock if no seed is specified.
  Compiler  : Turbo C, v.2.0
  Switches  : small model, 8086 instruction set, fp emulation.
-----------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

/*
  Arbitrary multiplier for random generation
  (ie: random(n * default_range))
*/
#define default_range         3


static unsigned SEED;

void write_matrix(int size)
{
  int i,j;

  SEED ? srand(SEED) : randomize();

  for(i=0; i < size; ++i)
  {
    for(j=0; j < size; ++j)
      printf("%-4d",random(size*default_range));
    printf("%d\n",random(size*default_range));
  }
}




int main(int argc, char **argv)
{
  int size;

  if(argc > 1)
  {
    size = atoi(argv[1]);
    if(argc > 2)
      SEED = (unsigned)atoi(argv[2]);
    write_matrix(size);
    return 0;
  }
  else
  {
    fprintf(stderr,"\nSyntax: randmtx n [S]\n");
    fprintf(stderr,"    Where n is the size of an n(n+1) A|b matrix,\n");
    fprintf(stderr,"    S is the seed for the random function (optional)\n");
    return 1;
  }
}
