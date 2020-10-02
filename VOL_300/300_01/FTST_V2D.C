/*   HEADER:   CUG300;
      TITLE:   Float array test;
       DATE:   3/12/1989;
DESCRIPTION:   "Test all float arrays functions";
    VERSION:   2.03;
   FILENAME:   FTST_V2D.C;
   SEE-ALSO:   MAT_V2D.H;
*/

/*   (C) COPYRIGHT BY JOHN J. HUGHES III, 1989  */

#include <stdio.h>
#include "mat_v2D.h"

main()

{
register int i,j;
int r, c, rows, cols,DEBUG;
struct fmat *A, *B, *C, *D, *E, *F;
FILE *results;
float test;

   printf("Enter DEBUG level => "); scanf("%d",&DEBUG);

   if(DEBUG>0) puts("mfcnt(A.DTA)");
      mfcnt("A.DTA",&rows,&cols);
   if(DEBUG>0) puts("fdim(A)");
      fdim(A,rows,cols);
   if(DEBUG>0) puts("mfget(A)");
      mfget ("A.DTA",A);
   if(DEBUG>0) puts("mfcnt(B)");
      mfcnt("B.DTA",&rows,&cols);
   if(DEBUG>0) puts("fdim(B)");
      fdim(B,rows,cols);
   if(DEBUG>0) puts("mfread(B)");
      mfread (B,"B.DTA");
   if(DEBUG>0) puts("mfcnt(C)");
      mfcnt("C.DTA",&rows,&cols);
   if(DEBUG>0) puts("fdim(C)");
      fdim(C,rows,cols);
   if(DEBUG>0) puts("mfread(C)");
      mfread (C,"C.DTA");
   if(DEBUG>9) { puts("mfdump(A)");
      mfdump(A);}
   if(DEBUG>9) { puts("mfdump(B)");
      mfdump(B); }
   if(DEBUG>9) { puts("mfdump(C)");
      mfdump(C);}
   if(DEBUG>0)
      printf ("cofactor A 1,1 -> %f\n",mfcof(A,0,0) );
   if(DEBUG>0)
      printf ("cofactor A 2,1 -> %f\n",mfcof(A,1,0) );
   if(DEBUG>0)
      printf ("cofactor A 3,1 -> %f\n",mfcof(A,2,0) );
   if(DEBUG>0)
      printf ("determinant A -> %f\n",mfdet(A) );
   if(DEBUG>0) puts("fdim(D)");
      fdim(D,3,3);
   if(DEBUG>0) puts("mftrnsp(A into D)");
      mftrnsp(D,A);
   if(DEBUG>9) { puts("mfdump(D)");
      mfdump(D); }
   if(DEBUG>0) puts("mfinv(B into D)");
      mfinv (D,B);
   if(DEBUG>0) puts("mfstore(D into INVB.DTA)");
      mfstore ("INVB.DTA",D);
   if(DEBUG>0) puts("mfstore(D into INVB.DTA)");
      fdim(E,3,3);
   if(DEBUG>0) puts("mult D by 42");
      all(D,r,c) f(E,r,c) = 42 * f(D,r,c);
   if(DEBUG>9) { puts("mfdump(E)");
      mfdump(E); }
   if(DEBUG>0) puts("add A to B & put into E");
      all(A,r,c) f(E,r,c) = f(A,r,c) + f(B,r,c);
   if(DEBUG>9) { puts("mfdump(E)");
      mfdump( E ); }
   if(DEBUG>0) puts("frel(E)");
      frel(E);
   if(DEBUG>0) puts("fdim(E)");
      fdim(E,no_rows(C),1);
   if(DEBUG>0) puts("mfmlt D x C => E");
      mfmlt(E,D,C);
   if(DEBUG>9) { puts("mfdump(E)");
      mfdump(E); }
   if(DEBUG>0) puts("solve B & C => E");
      mfslv (E,B,C);
   if(DEBUG>9) { puts("mfdump(E)");
      mfdump (E); }
   if(DEBUG>0) puts("create file RESULT.OUT");
      flcreat("Result.out",results);
   if(DEBUG>0) puts("place 3 cols of A into RESULT.OUT");
      mfput (results,A, "c1 c2 c3", "c1 c3",72, 2, 1 );
   if(DEBUG>0) puts("frel(E)");
      frel(E);
   if(DEBUG>0) puts("fdim(E) to new size");
      fdim(E,3,3);
   if(DEBUG>0) puts("copy A into E");
      mfcpy(E,A);
   if(DEBUG>9) { puts("mfdump(E)");
      mfdump(E); }
   if(DEBUG>0) puts("frel(E)");
      frel(E);
   if(DEBUG>0) puts("frel(E)");
      mfcnt("big.dta",&rows,&cols);
   if(DEBUG>0) puts("fdim(E)");
      fdim(E,rows,cols);
   if(DEBUG>0) puts("mfread(BIG.DTA");
      mfread(E,"big.dta");
   if(DEBUG>0) { puts("sum columns");
      test = mfsumc( E, 1, 2, 5);
      printf("column sum = %f\n",test);  }
   if(DEBUG>9) { puts("mfdump(E)");
      mfdump (E); }
   if(DEBUG>0) puts("mfread(BIG.DTA into E");
      mfread(E,"big.dta");
   if(DEBUG>0) { puts("sum rows");
      test = mfsumr( E, 1, 2, 5);
      printf("row sum = %f\n",test);  }
   if(DEBUG>9) { puts("mfdump(E)");
      mfdump ( E ); }
   if(DEBUG>0) puts("mfread(BIG.DTA into E)");
      mfread(E,"big.dta");
   if(DEBUG>0) puts("calculate cumulative column totals => E");
      mfcumc ( E, 1, 2, 5, 7);
   if(DEBUG>9) { puts("mfdump(E)");
      mfdump ( E ); }
   if(DEBUG>0) puts("mfread(BIG.DTA into E)");
      mfread(E,"big.dta");
   if(DEBUG>0) puts("calculate cumulative row totals => E");
      mfcumr ( E, 1, 2, 5, 7);
   if(DEBUG>9) { puts("mfdump(E)");
      mfdump ( E ); }
   if(DEBUG>0) puts("mfread(BIG.DTA into E)");
      mfread(E,"big.dta");
   if(DEBUG>0) puts("calculate move average row totals => E");
      mfmvar ( E, 2, 2, 2, 5, 3 );
   if(DEBUG>9) { puts("mfdump(E)");
      mfdump ( E ); }
   if(DEBUG>0) puts("mfread(BIG.DTA into E)");
      mfread(E,"big.dta");
   if(DEBUG>0) puts("calculate move average column totals => E");
      mfmvac ( E, 2, 2, 2, 5, 3 );
   if(DEBUG>9) { puts("mfdump(E)");
      mfdump (E); }
   if(DEBUG>0) puts("frel(E)");
      frel(E);
   if(DEBUG>0) puts("mfcnt(WIDE.DTA)");
      mfcnt("wide.dta",&rows,&cols);
   if(DEBUG>0) puts("fdim(E)");
      fdim(E,rows,cols);
   if(DEBUG>0) puts("mfread(WIDE.DTA into E)");
      mfread (E,"WIDE.DTA");
   if(DEBUG>0) puts("page output of E to WD-RESLT.OUT");
      mfpgput (E,"WD-RESLT.OUT",72,12,1);
   if(DEBUG>0) puts("mfread(WIDE.DTA into E)");
      mfread(E,"wide.dta");
   if(DEBUG>9) { puts("mfdump(E)");
      mfdump(E); }
   if(DEBUG>0) { puts("\n test rows macro ... ");
      rows(E,i) printf("%5.2f ",f(E,i,2)); }
   if(DEBUG>0) { puts("\n test cols macro  ... ");
      cols(A,j) printf("%5.2f ",f(E,2,j)); }
   if(DEBUG>0) puts("\n fdim(F) for a 120 x 120 matrix speed test");
      fdim (F,120,120);
   if(DEBUG>0) puts("test by placing 500 in all elements .... GO");
      all(F,i,j) f(F,i,j)=500;
   if(DEBUG>0) printf(" %d END\n",no_cols(F) );
}

