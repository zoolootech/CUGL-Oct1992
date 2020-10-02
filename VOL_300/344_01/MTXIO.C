/*-----------------------------------------------------------------------
   Main File : mtx.exe
   File Name : mtxio.c

   Purpose - Allocation, initialization and output routines
-----------------------------------------------------------------------*/
#include "mtx.h"
#include "mtxcle.h"
#include "mtxio.h"


  /* Remove trailing '_' for operations timing */
#define TIME_

#ifdef TIME
#include <time.h>
#endif

extern int MSize;
extern Mtype *Aptr;
extern Mtype *bptr;
extern int P;
extern int display_all;


void read_matrix()
/*
  Assumes all input is correct - does no checking!
*/
{
  int i,j;
  char s[30];
  const long int n = MSize;

  if((Aptr=(Mtype *)alloc(n*n,sizeof(Mtype)))==NULL)
    error(errHISIZE);
  if((bptr=(Mtype *)alloc(n,sizeof(Mtype)))==NULL)
    error(errHISIZE);

  for(i=0; i < n; ++i)
  {
    for(j=0 ; j < n; ++j)       /*get A*/
    {
      fscanf(stdin,"%s",s);
      A(i,j) = atoMtype(s);
    }
    fscanf(stdin,"%s",s);       /*get b*/
    b(i) = atoMtype(s);
  }
}


void print_matrix(int d_flag)
{
  int i,j;
  const long int n = MSize;
  static int mat_flag;

  if(d_flag==NO_SHOW)
    return;
  if(d_flag != SOLUTION_ONLY)
    printf("%s\n",mat_flag++ ? "\nOutput Matrix--\n" : "\nInput Matrix--\n");
  for(i=0; i < n; ++i)
  {
    if(d_flag==SHOW_MATRIX)
      for(j=0 ; j < n; ++j)
        printf("  %*.*G",4+P,P,A(i,j));
    printf("  %*.*G\n",4+P,P,b(i));
  }
  printf("\n");

#ifdef TIME
  {
    static int flag;
    if(flag)
      print_time;
    ++flag;
  }
#endif

}


