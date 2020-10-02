/*
**                TEXT COMPARE UTILITY
**
**   Copyright 1987, S.E. Margison
**
**   This short utility compares two text files and shows
**   any line differences.
**
**   As distributed, this program requires (for compilation):
**     "Steve's Turbo-C Library" version 1.30 or later
**   which may be obtained without registration from many Bulletin
**   Board Systems including:
**      Compuserve IBMSW
**      Cul-De-Sac (Holliston, MA.)
**      GEnie
**   and software library houses including:
**      Public (Software) Library (Houston, TX.)
**
**   or by registration:
**      $10 for Docs, Small Model Library
**      $25 for Docs, C, S, M, L, H libraries, and complete library source
**              in C and Assembler
**     Steven E. Margison
**     124 Sixth Street
**     Downers Grove, IL, 60515
**
**
*/

#include <stdio.h>
#include <smdefs.h>

FILE *fp1, *fp2;
char buf1[MAXLINE], buf2[MAXLINE];

main(argc, argv)
int argc;
char *argv[];
{
   int lc, end1, end2;
   lc = 0;
   end1 = end2 = NO;

   if(argc isnot 3) error("usage: TC file1 file2");

   if((fp1 = fopen(argv[1], "r")) is NULL) cant(argv[1]);

   if((fp2 = fopen(argv[2], "r")) is NULL) {
      fclose(fp1);
      cant(argv[2]);
      }

   for ever {
      ++lc;
      if(fgets(buf1, MAXLINE, fp1) is NULL) end1 = YES;
      if(fgets(buf2, MAXLINE, fp2) is NULL) end2 = YES;
      if(end1 or end2) break;

      if(strcmp(buf1, buf2)) {
         printf("Line %d in %s\n", lc, argv[1]);
         printf("%s", buf1);
         printf("Line %d in %s\n", lc, argv[2]);
         printf("%s", buf2);
         }
      }
   if(end1 and !end2)
      printf("EOF on %s occured first\n", argv[1]);
   if(end2 and !end1)
      printf("EOF on %s occured first\n", argv[2]);
   fclose(fp1);
   fclose(fp2);
   }

