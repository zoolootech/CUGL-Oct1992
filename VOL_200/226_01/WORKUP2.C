   /*  HEADER:CUG 226;
       TITLE:"ART-CEE";
       VERSION:1.4;
       DESCRIPTION:"Small artificial intelligence inference engine.

            ART-CEE is a rule-based general purpose driver capable of working
            with heirarchical and/or propositional information.";
       KEYWORDS:artificial intelligence, inference engines, database;
       SYSTEM:MS-DOS;
       FILENAME:WORKUP2.C;
       SEE-ALSO:WORKUP0.C, WORKUP3.C, WORKUP4.C, MANUAL.TXT, WORKUP0.H,
            WORKUP1.H, HELP1.AIH, HELP2.AIH, HELP3.AIH, HELP4.AIH, ART-CEE.COM;
       AUTHOR:Dan Cheatham;
       COMPILER:MIX-C;
   */


         /*  art-cee  artificial intelligence inference engine    */
         /*                                                       */
         /*  file:  workup2.c    --  data file i-o                */
         /*                          exclusive groupings routines */
         /*                                                       */

#include "workup0.h"
#include "workup1.h"

loaddata()
   { FILE *infile;
     int wk1, wk2, wk3, wk5, wk6, wk7;
     printf("\nEnter the name of the file to be read: ");
     gets(currfile);
     infile  = fopen(currfile,"r");
     if (infile  == NULL)
         failfile();
     else
       { fscanf(infile,"%d%d",&wk5,&wk6);
         if (wk5 < wk6 && wk6 < MAX)
          { wk6++; for (wk1 = wk5; wk1 < wk6; wk1++)
            { fscanf(infile,"%d",&wk7);
              wk7++;
              wk3 = fgets(subjs[wk1],wk7,infile);
              wk2 = 0;
              while (wk2 < wk6)
                 {  wk3 = fscanf(infile,"%f",&ft[wk1][wk2]);
                    wk2++;} }
             wk3 = fclose(infile); }
          else
            { printf("\nStart and/or stop positions invalid in file.");
              wk3 = fclose(infile);
              marktime();}}
}

presave()
   { int wk5, wk6, wk7; char prompt[2];
     printf("\n\nSave entire database (Y/N)? ");
     gets(prompt);
     if (prompt[0] == 'Y')
         savedata(0,MAX);
     else
      { printf("\nEnter the following:");
        printf("\nPosition of first subject to be saved (0 = start of data),");
        printf("\n the position: ");
        scanf("%d",wk5);
        if (wk5 < MAX)
          {}
        else
          { printf("\nInput is greater than maximum position in data.");
            printf("\nMaximum position used.");
            wk5 = MAX; }
       wk7 = MAX - 1;
       printf("\nPosition of last subject to be saved (%d = end of data)",wk7);
       printf("\n the position: ");
       scanf("%d",wk6);
       if (wk6 < MAX && wk5 < wk6)
            { wk6++; savedata(wk5,wk6); }
       else
         {  printf("\nInvalid limits entered.  Cannot proceed.");
            marktime(); }}
}

savedata(wk5,wk6)
   int wk5, wk6;
   { FILE *outfile;  char fname[32];          int wk7, wk8;
     int wk1, wk2, wk3;           char tempfile[32];  float fn1, fn2, fn3, fn4;
     printf("\n%s will be overwritten if you just press",currfile);
     printf("\nenter.  Enter a new file name to avoid this.");
     printf("\nThe name:  ");
     gets(tempfile);
     if (tempfile[0] != '\0' && tempfile[0] != '\n')
            strcpy(fname,tempfile);
     else
            strcpy(fname,currfile);
     outfile = fopen(fname,"w");
     if (outfile == NULL)
         failfile();
     else
       { filestat = 0; wk7 = wk6 - 1;
         printf("\n\nSaving database from positon %d to position %d.",wk5,wk7);
            filestat = fprintf(outfile,"%d %d ",wk5,wk7);
         for (wk1 = wk5; wk1 < wk6; wk1++)
           { wk8 = strlen(subjs[wk1]);
             wk3 =  fprintf(outfile,"%d", wk8);
             if  (wk3 == -1)
                filestat = 1;
             wk3 = fputs(subjs[wk1],outfile);
             if (wk3 == -1)
                filestat = 1;
             wk2 = wk5;
             while (wk2 < wk6)
               { wk3   = fprintf(outfile,"%f ",ft[wk1][wk2]);
                 wk2++;
                 if (wk3 != 0)
                   filestat = wk3;}}
          wk3 = fflush(outfile);
          if (wk3 != 0)
             filestat = wk3;
          wk3 = fclose(outfile);
          if (filestat != 0 || wk3 != 0)
              failfile(); }
}

filegroup()
   { FILE *infile;  char grpname[32], grpitem[77];
     int wk1, wk2, wk3, wk5, wk6, wk8, wk9, grpmems[MAX];

     printf("\nEnter the name of the groups file to be read: ");
     gets(grpname);
     infile  = fopen(grpname,"r");
     if (infile  == NULL)
         wk6 = 1;
     else
       { wk6 = 0; wk9 = 0;
         wk5 = fscanf(infile,"%d",&wk1);
         for (wk2 = 0; wk2 < wk1; wk2++)
            { wk5 = fscanf(infile,"%d",&wk3);
              wk3++;
              wk5 = fgets(grpitem,wk3,infile);
              if (wk5 == NULL)
                   wk6 = 1;
              else
                { wk8 = findmem(grpitem);
                  if (wk8 < MAX)
                   { grpmems[wk9] = wk8; wk9++; }}}
          wk5 = fclose(infile); }

     if (wk6 != 0)
        printf("\nProcedure failed.");
     else
        markgrup(grpmems,wk9);
}

findmem(dd1)
   char *dd1;
   { int kw1, kw2, kw3;
     kw3 = MAX + 1; kw1 = 0;
     while (kw1 < MAX && kw3 > MAX)
      { kw2 = strcmp(dd1,subjs[kw1]);
        if (kw2 == 0)
          kw3 = kw1;
        kw1++; }
     return(kw3);
}

markgrup(dd1,dd2)
   int *dd1[], dd2;
   { int wd1, wd2, wd3, wd4;
     for (wd1 = 0; wd1 < dd2; wd1++)
       { wd2 = dd1[wd1];
         for (wd3 = 0; wd3 < dd2; wd3++)
           { wd4 = dd1[wd3];
             if (wd2 != wd4)
               { ft[wd2][wd4] = -100;
                 printf("\n%s cannot be %s.",subjs[wd2],subjs[wd4]); }}}
}

manugroup()
   { int wk1, wk2, wk3, wk4, wk5, wk6, wk7, wk8, wk9, grpmems[MAX];
      FILE *outfile;  char grpname[32], grpitem[77], answer[2];

     strcpy(grpitem,indff); /* make sure while loop does not abend */
     wk9 = 0;
     while (grpitem[0] != 'E' && wk9 < MAX)
       { printf("\n\n\nEnter the contents for group member # %d.",wk9);
         printf("\nIt will NOT be edited for proper syntax.");
         printf("\nEnter 'E' to end making entries.\nThe entry: ");
         gets(grpitem);
         wk8 = findmem(grpitem);
         if (wk8 < MAX)
           { grpmems[wk9] = wk8; wk9++;
             printf("\n  Subject found."); }}

     if (wk9 > 0)
       { markgrup(grpmems,wk9);
         printf("\nSave group (Y/N)? ");
         gets(answer);
         if (answer[0] == 'Y')
          { printf("\nEnter the name under which the group will be stored.");
            printf("\nFile name:  ");
            gets(grpname);
            outfile = fopen(grpname,"w");
            if (outfile == NULL)
               failfile();
            else
              { filestat = 0;
                printf("\n\nSaving group.");
                filestat = fprintf(outfile," %d ",wk9);
                for (wk1 = 0; wk1 < wk9; wk1++)
                  { wk4 = grpmems[wk1];
                    wk7 = strlen(subjs[wk4]);
                    wk3 = fprintf(outfile," %d",wk7);
                    if  (wk3 == -1)
                       filestat = 1;
                    wk3 = fputs(subjs[wk4],outfile);
                    if (wk3 == -1)
                       filestat = 1; }
                    wk3 = fflush(outfile);
                    if (wk3 != 0)
                       filestat = wk3;
                    wk3 = fclose(outfile);
                    if (filestat != 0 || wk3 != 0)
                      failfile(); }}}
}
