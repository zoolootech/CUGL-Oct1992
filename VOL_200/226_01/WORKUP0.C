/*     HEADER:CUG 226;
       TITLE:"ART-CEE";
       VERSION:1.4;
       DESCRIPTION:"Small artificial intelligence inference engine.

            ART-CEE is a rule-based general purpose driver capable of working
            with heirarchical and/or propositional information.";
       KEYWORDS:artificial intelligence, inference engines, database;
       SYSTEM:MS-DOS;
       FILENAME:WORKUP0.C;
       SEE-ALSO:WORKUP2.C, WORKUP3.C, WORKUP4.C, MANUAL.TXT, WORKUP0.H,
            WORKUP1.H, HELP1.AIH, HELP2.AIH, HELP3.AIH, HELP4.AIH, ART-CEE.COM;
       AUTHOR:Dan Cheatham;
       COMPILER:MIX-C;
   */

         /*  art-cee artificial intelligence inference engine     */
         /*                                                       */
         /*  file:  workup0.c    --  parsing subroutines          */
         /*                          predicate parse              */
#include "workup0.h"
#include "workup1.h"

mainin()
    {    char extr1[81], extr2[81], extr3[81]; int x3;
         if (menu_sw == 'Y')
             showmenu();
         printf("\nEnter rule, query or command.\n");
         gets(indff);

         x1 = strlen(indff); x2 = 0;

         if (x1 > 2)
         { x2 = x1 - 1;
           strsplit(indff,extr1,extr2,x1,x1-1);
            if (x1 > 7)
                x2 = parsesub(indff,"IF THE ",extr1,x1,7);
            if (x2 == 0 && x1 > 6)
                x2 = parsesub(indff,"IF AN ",extr1,x1,6);
            if (x2 == 0 && x1 > 5)
                x2 = parsesub(indff,"IF A ",extr1,x1,5);
            if (x2 == 0 && x1 > 3)
                x2 = parsesub(indff,"IF ",extr1,x1,3);
            if (x2 == 0 && x1 > 11)
                x2 = 2 * parsesub(indff,"WHO IS THE ",extr1,x1,11);
            if (x2 == 0 && x1 > 10)
                x2 = 2 * parsesub(indff,"WHO IS AN ",extr1,x1,10);
            if (x2 == 0 && x1 > 9)
                x2 = 2 * parsesub(indff,"WHO IS A ",extr1,x1,9);
            if (x2 == 0 && x1 > 7)
                x2 = 2 * parsesub(indff,"WHO IS ",extr1,x1,7);
            if (x2 == 0 && x1 > 12)
                x2 = 3 * parsesub(indff,"WHAT IS THE ",extr1,x1,12);
            if (x2 == 0 && x1 > 11)
                x2 = 3 * parsesub(indff,"WHAT IS AN ",extr1,x1,11);
            if (x2 == 0 && x1 > 10)
                x2 = 3 * parsesub(indff,"WHAT IS A ",extr1,x1,10);
            if (x2 == 0 && x1 > 7)
                x2 = 3 * parsesub(indff,"WHO IS ",extr1,x1,7);
            if (x2 == 0 && x1 > 8)
                x2 = 3 * parsesub(indff,"WHAT IS ",extr1,x1,8);
            if (x2 == 0 && x1 > 4)
                x2 = 2 * parsesub(indff,"WHO ",extr1,x1,4);
            if (x2 == 0 && x1 > 5)
                x2 = 3 * parsesub(indff,"WHAT ",extr1,x1,5);
            if (x2 == 0 && x1 > 13)
                x2 = 5 * parsesub(indff,"DESCRIBE THE ",extr1,x1,13);
            if (x2 == 0 && x2 > 12)
                x2 = 5 * parsesub(indff,"DESCRIBE AN ",extr1,x1,12);
            if (x2 == 0 && x2 > 11)
                x2 = 5 * parsesub(indff,"DESCRIBE A ",extr1,x1,11);
            if (x2 == 0 && x1 > 9)
                x2 = 5 * parsesub(indff,"DESCRIBE ",extr1,x1,9);

            if (strcmp(extr2,"?") == 0)
               { x2 = x2 * -1;
               x3 = strlen(extr1) - 1;
               strsplit(extr1,extr2,extr3,strlen(extr1),x3);
               strcpy(extr1,extr2);}
            if (x2 != 0)
               strcpy(indff,extr1);}
}

parsesub(dd,dd2,dd3,dn1,dn2)
    char *dd[ ], *dd2[ ], *dd3[ ]; int dn1, dn2;
     {  int sd6; char sd7[81], sd8[81];
        strsplit(dd,sd7,sd8,dn1,dn2);
        strcpy(dd3,sd8);
        if (strcmp(sd7,dd2) == 0)
            return(1);
        else
            return(0);
}

strsplit(sl1,sl2,sl3,sl5,sl4)
     short int *sl1, *sl2, *sl3; int sl4, sl5;
     { int sn1, sn2;
       sn2 = sl5;
       sn1 = 0;
       while(sn1++ < sl4)
         { if (*sl1 > 96 && *sl1 < 123)
             *sl1 = *sl1 - 32;
          *sl2++ = *sl1++;}
       *sl2++ = 0;
       sn1 = sl4 - 2;
       while(sn1++ < sn2)
          { if (*sl1 > 96 && *sl1 < 123)
              *sl1 = *sl1 - 32;
            *sl3++ = *sl1++;}
       return strsplit;
}
  
parsepred(dd1,dd2,dd3,dd4,dd5)
    char *dd1[ ], *dd2[ ], *dd3[ ]; int dd4, dd5;
   { short int pp1, pp2, pp3, pp4;
     char pp6[81], pp7[81], pp8[81], pp9[81];
     pp4 = strlen(dd1) - 6;
     pp1 = 0; pp2 = 1;
     while(pp1 == 0 && pp2 < pp4)
        { strsplit(dd1,pp6,pp7,dd4,pp2);
          pp3 = parsesub(pp7," THEN ",pp8,strlen(pp7),6);
          if (pp3 != 0)
             { strcpy(dd1,pp6);
               strcpy(dd3,pp8);
               pp1 = 1;} 
          pp2++; }
        return(pp1);
}

