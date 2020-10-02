/*
      TITLE:   Token array demo;
       DATE:   4/09/1989;
DESCRIPTION:   "Tests all token array functions";
    VERSION:   2.04;
   FILENAME:   TTST_V2D.C;
   SEE-ALSO:   MAT_V2D.H;
*/

/*   (C) COPYRIGHT BY JOHN J. HUGHES III, 1989  */

#include "mat_v2D.h";
#include <stdlib.h>
#include <string.h>

int cmp (void *a, void *b);

main()

{
unsigned int   r,c,t,DEBUG,tsz,rw,cl;
struct fmat  *numbers;
struct tmat  *nums,*misc,*appnd;
FILE  *FN;

   printf("Enter DEBUG level => ");scanf("%d",&DEBUG);

   if (DEBUG>0) puts("mtcnt(nums.dta):1");
      mtcnt("nums.dta",&r,&c,&t);
   if (DEBUG>0) printf(
      "for nums.dta : rows %d,cols %d,and token size %d read.\n",r,c,t);
   if (DEBUG>0) puts("tdim(nums):2");
      tdim(nums,r,c,t);
   if (DEBUG>0) printf(
      "for nums array : rows %d,cols %d,and token size %d allocated.\n",
            no_rows(nums),no_cols(nums),token_sz(nums));
   if (DEBUG>0) puts("tdim(nums):3");
      mtget("nums.dta",nums);
   if (DEBUG>0) puts("mtcnt(misc.dta):1");
      mtcnt("misc.dta",&r,&c,&t);
   if (DEBUG>0) printf(
      "for misc.dta : rows %d,cols %d,and token size %d read.\n",r,c,t);
   if (DEBUG>0) puts("tdim(misc):4");
      tdim(misc,r,c,t);
   if (DEBUG>0) printf(
      "for misc array : rows %d,cols %d,and token size %d allocated.\n",
       no_rows(misc),no_cols(misc),token_sz(misc));
   if (DEBUG>0) puts("mtread(misc):5");
      mtread("misc.dta",misc);
   if (DEBUG>3) { puts("mtdump(nums):6");
      mtdump(nums); }
   if (DEBUG>3) { puts("mtdump(misc):7");
      mtdump(misc); }
   if (DEBUG>0)printf("Floating point value -- %f\n",tf(misc,5,1,4));
   if (DEBUG>0)printf("Floating point value -- %f\n",tf(misc,5,2,4));
   if (DEBUG>0) puts("ft used to place value in misc array:8");
      ft(misc,3,2,993.98,80,10,3);
   if (DEBUG>0)printf("Integer point value -- %d\n",ti(misc,5,3,4));
   if (DEBUG>0) puts("it used to place value in misc array:9");
      it(misc,3,1,666,80,10);
   if (DEBUG>0)printf("string value -- %s\n",ts(misc,3,0,3));
   if (DEBUG>0) puts("tt used to place value in misc array:10");
      tt(misc,4,2,"n_entry",80,10);
   if (DEBUG>3) { puts("mtdump(misc):11");
      mtdump(misc); }
   if (DEBUG>0)puts("tdim(apnd):12");
		tsz=token_sz(misc);
		rw=no_rows(misc)+1;
		cl=no_cols(misc);

	 { unsigned nnni;
		if ( (int) (((appnd)=(struct tmat *)malloc(sizeof(struct tmat))) == 0)
		) error (YES,9,rw,cl);
		if ( (int) ((((appnd)->t)=(char *) calloc((rw)*(cl)*(tsz+1),
												sizeof(char)))==0)
		) error (YES,9,rw,cl);
		if ( (int) ((((appnd)->n_toks) = (unsigned *)calloc((rw),
												sizeof(unsigned)))==0)
		) error (YES,9,rw,cl);
		((appnd)->n_rows)=(rw); ((appnd)->n_cols)=(cl);
		((appnd)->tok_sz)=(tsz)+1;  ((appnd)->n_recs)=0;
		for(nnni=0;nnni<(rw);nnni++) (appnd)->n_toks[nnni]=0;
		for(nnni=0;nnni<(rw)*((cl));nnni++) (appnd)->t[nnni*(tsz+1)]=NULL; }

/*		tdim(appnd,(no_rows(misc)+1),no_cols(misc),tmp);  */

	if (DEBUG>0)puts("mtcpy nums to misc:13");
      mtcpy(misc,appnd);
   if (DEBUG>0)puts("mtapnd third row of nums to appnd:14");
      mtapnd(appnd,nums,2);
   if (DEBUG>3) { puts("mtdump appnd:15");
      mtdump(appnd); }
   if (DEBUG>0)puts("flcreat output.dta file:16");
      flcreat("output.dta",FN);
   if (DEBUG>0)puts("Output a centered TITLE to output.dta:18");
      msc(FN,"TITLE");
   if (DEBUG>0)puts("Output a right justified RIGHT to output.dta:19");
      msr(FN,"RIGHT");
   if (DEBUG>0)puts("Output a left justified LEFT to output.dta:20");
      msl(FN,"LEFT");
   if (DEBUG>0)puts("Output phrase and float value to output.dta:21");
      msv(FN,"Value from appnd text matrix row 5, col 1",
         tf(appnd,5,1,4));
   if (DEBUG>0)puts("Output phrase and token to output.dta:21");
      mst(FN,"Token from appnd matrix row 5, col 0",
              t(appnd,5,0));
   if (DEBUG>3) puts("Output 4 cols of appnd to output.dta:22");
      mtput(FN,appnd,"c1 c2 c3 c4 c5","c1 c2 c3 c4 c5",70,7);
   if (DEBUG>3) puts("Sort 2nd column and re-output");
		qsort(t(appnd,0,0),no_recs(appnd),(no_cols(appnd)*
         token_sz(appnd)),cmp);
      mtput(FN,appnd,"c1 c2 c3 c4 c5","c1 c2 c3 c4 c5",70,7);
   if (DEBUG>0)puts("Close output.dta:23");
      fclose(FN);
   if (DEBUG>0)puts("Test tmtofm/read A.DTA into misc:24");
      trel(misc);tdim(misc,4,4,TOK);mtget("a.dta",misc);
   if (DEBUG>0)puts("Test tmtofm/fdim numbers array:25");
      fdim(numbers,4,4);
   if (DEBUG>0)puts("Test tmtofm/execute tmtofm:26");
      tmtofm(numbers,misc);
   if (DEBUG>9)  { puts("dump numbers");
      mfdump(numbers); }
}

int cmp (void *a, void *b)

{
unsigned int   test;
struct tmat *amat,*bmat;

   tdim(amat,1,5,10);
   tdim(bmat,1,5,10);

	memcpy(t(amat,0,0),a,55);
	memcpy(t(bmat,0,0),b,55);

   test = strcmp( t(amat,0,0) ,t(bmat,0,0) );
   trel(amat);
   trel(bmat);

   return(test);
}
