/*-----------------------------------------------------------------------
   Main File : mtx.exe
   File Name : mtxsolv.c

   Purpose - G-J computations on an A|b matrix
-------------------------------------------------------------------------*/
#include "mtx.h"
#include "mtxcle.h"


  /* Indexing macro for temporary array */
#define Pivot(i)            *(pivot+(i))

extern int MSize;

  /* Head pointers */
Mtype *Aptr=NULL;
Mtype *bptr=NULL;


void dswap(Mtype *p1, Mtype *p2)
{
  Mtype temp;

  temp=*p1;
  *p1=*p2;
  *p2=temp;
}


void solve_matrix()
{
  int i,j,k;
  const long int n = MSize;
  Mtype *pivot,mabs,mmax,prod,sum;

  if((pivot=(Mtype *)alloc(n,sizeof(Mtype)))==NULL)
    error(errHISIZE);

  for(k=0; k < n-1; ++k)
  {
    Pivot(k) = k;
    mmax = (Mtype)absMtype(A(k,k));
    for(i=k+1; i < n; ++i)
    {
      mabs = (Mtype)absMtype(A(i,k));
      if(mabs > mmax)
      {
        Pivot(k) = i;
        mmax = mabs;
      }
    }
    if(mmax == 0)
      error(errSINGULARITY);
    if(Pivot(k) != k)
    {
      i = Pivot(k);
      dswap(bptr+k, bptr+i);
      for(j=k; j < n ; ++j)
        dswap(Aptr+(k*n)+j, Aptr+(i*n)+j);
    }
    for(i=k+1; i < n; i++)
    {
      prod = A(i,k) / A(k,k);
      A(i,k) = prod;
      b(i) -= prod * b(k);
      for(j=k+1; j < n; ++j)
        A(i,j) -= prod * A(k,j);
    }
  }
  if(A(n-1,n-1) == 0)
    error(errSINGULARITY);
  for(i=n-1; i >= 0; --i)
  {
    for(sum=0.0, j=i+1; j < n; ++j)
      sum += A(i,j) * b(j);
    b(i) = (b(i)-sum) / A(i,i);
  }
}

