   /*  HEADER:CUG 226;
       TITLE:"ART-CEE";
       VERSION:1.4;
       DESCRIPTION:"Small artificial intelligence inference engine.

            ART-CEE is a rule-based general purpose driver capable of working
            with heirarchical and/or propositional information.";
       KEYWORDS:artificial intelligence, inference engines, database;
       SYSTEM:MS-DOS;
       FILENAME:WORKUP4.C;
       SEE-ALSO:WORKUP0.C, WORKUP2.C, WORKUP3.C, MANUAL.TXT, WORKUP0.H,
            WORKUP1.H, HELP1.AIH, HELP2.AIH, HELP3.AIH, HELP4.AIH, ART-CEE.COM;
       AUTHOR:Dan Cheatham;
       COMPILER:MIX-C;
   */


         /*  art-cee  artificial intelligence inference engine    */
         /*                                                       */
         /*  file:  workup1.C    --  external data definitions    */
         /*                          main function                */
         /*                          initial input and parse      */
         /*                          help                         */
         /*                                                       */
#include "stdio"
#include "workup0.h"
    float    dfwd, drev, ff1, ft[MAX][MAX];
    char     indff[80], subjs[MAX][77], currfile[32], menu_sw,
             oride_sw, filebuf[81];
    int      x1, x2, filestat, srchdpth, assumpno;


main()
    {  wakeup(); menu_sw = 'Y'; oride_sw = 'Y'; assumpno = 0;
       x1 = 50; filestat = 0; dfwd = 100.0 ; drev = 0.0; srchdpth = 3;
       strcpy(currfile,"ART-CEE.DAT");
         while (toupper(indff[0]) != 'X')
             { mainin();
               if (x1 > 2 && x2 != 0)
                  exec_string();
               else
                  { if (x1 == 1)
                   commands();
                    else
                       { printf("\nInvalid entry.");
                         marktime(); } } }

       printf("\nSave database (Y/N)? ");
              gets(indff);
              if (toupper(indff[0]) == 'Y')
                  savedata(0,MAX);
}
  
marktime()
   { char xx; printf("\nPress return to proceed."); xx = getchar(); }


wakeup()
   { system("cls");
      printf("\n      AAAAAA   RRRRRRRR  TTTTTTTT      CCCCCCCC  EEEEEEEE  EEEEEEEE");
      printf("\n     AA    AA  RR    RR     TT        CC         EE        EE");
      printf("\n     AA    AA  RR    RR     TT        CC         EE        EE");
      printf("\n     AAAAAAAA  RRRRRRRR     TT  ++++  CC         EEEEEE    EEEEEE");
      printf("\n     AA    AA  RR  RR       TT  ++++  CC         EE        EE");
      printf("\n     AA    AA  RR   RR      TT        CC         EE        EE");
      printf("\n     AA    AA  RR    RR     TT         CCCCCCCC  EEEEEEEE  EEEEEEEE\n\n");
      printf("\n                            By Dan Cheatham\n\n\n\n\n\n\n");
      initialize();
}
  
showmenu()
      {  system("cls");
         printf("COMMANDS:                          SETTABLE DEFAULTS:\n");
         printf("   B batch group     L load          Forward        %f\n",dfwd);
         printf("   C change subject  P print         Reverse        %f\n",drev);
         printf("   D drop rule       S save          Assumptions    %d\n",assumpno);
         printf("   G manual group    T think         thinK depth    %d\n",srchdpth);
         printf("   H help            V view          allow Override %c\n",oride_sw);
         printf("   I initialize      X exit          show Menu      %c\n",menu_sw);
         printf("RULES:\n");
         printf("   IF <subject> THEN <predicate>\n");
         printf("QUERIES:\n");
         printf("   WHO <subject> <?>\n");
         printf("   WHAT <subject> <?>\n");
         printf("   DESCRIBE <subject>\n");
         printf("   IF <subject> THEN <predicate> ?\n\n");
}

commands()
     { switch(toupper(indff[0]))
        { case 'A':  setassump()                 ; break;
          case 'B':  filegroup()                 ; break;
          case 'C':  chgsubj()                   ; break;
          case 'D':  droprule()                  ; break;
          case 'F':  setfwd()                    ; break;
          case 'G':  manugroup()                 ; break;
          case 'H':  help()                      ; break;
          case 'K':  setdepth()                  ; break;
          case 'I':  initialize()                ; break;
          case 'L':  loaddata()                  ; break;
          case 'M':  menu_sw  = toggle(menu_sw)  ; break;
          case 'O':  oride_sw = toggle(oride_sw) ; break;
          case 'P':  hardcopy()                  ; break;
          case 'R':  setrev()                    ; break;
          case 'S':  presave()                   ; break;
          case 'T':  think()                     ; break;
          case 'V':  view()                      ;
          }
}

setassump()
   { int qa1, qa2; char dumment[2];
     qa1 = MAX - 3;
     printf("\nWhat is the maximum number of assumptions to allow?");
     printf("\nAbsolute maximum is %d, absolute minimum is zero.",qa1);
     printf("\nYour choice: ");
     scanf("%d",&qa2); gets(dumment);

     if (qa2 < 0)
        assumpno = 0;
     else
        { if (qa2 > qa1)
            assumpno = qa1;
          else
            assumpno = qa2; }
}

setdepth()
   { int sd1, sd2; char dumment[2];
     sd1 = MAX - 1;
     printf("\nHow many levels shall be examined in thinking?");
     printf("\nAbsolute maximum is %d, absolute minimum is 3.",sd1);
     printf("\nYour choice: ");
     scanf("%d",&sd2); gets(dumment);
     if (sd2 < 3)
        srchdpth = 3;
     else
        { if (sd2 > sd1)
            srchdpth = sd1;
          else
            srchdpth = sd2;}
}

initialize()
       { int sx1; char empstr[] = "@@@@@"; 
         printf("\n\nInitializing storage.");
         for (sx1 = 0; sx1 < MAX; sx1++)
          { blank1pt(sx1);
            strcpy(subjs[sx1],empstr); }
}

setfwd()
  { ff1 = dfwd;
    dflt("forward");
    dfwd = ff1;
}

setrev()
  { ff1 = drev;
    dflt("reverse");
    drev = ff1;
}

toggle(cv)
   char *cv;
 { if (cv == 'Y')
      return('N');
   else
      return('Y');
}

dflt(st1)
      char *st1[ ];
   {  float ff2; char tx;
      printf("\n\nEnter new default %s percentage",st1);
      printf("\n   100 = certainty, 0 = never, must be at least zero.  ");
      scanf("%f",&ff2); tx = getchar();
      if (ff2 < 0 || ff2 > 100)
       { printf("\nInvalid entry."); marktime(); }
      else
          ff1 = ff2;
}

disphelp(tt)
     char *tt[ ];
   { int      wk2; char tc; FILE *infile;
     infile = NULL;
     tc = "C";
     while (infile == NULL && tc != 'A' && tc != 'a')
       { if (filestat == 0)
          { printf("\n\n Insert disk containing ART-CEE help files.");
            printf("\n Press 'A' to abort, any other key to proceed.");
            tc = getchar(); }
          if (toupper(tc)  != 'A')
            { infile = fopen(tt,"r");
              filestat  = infile; } }
     if (toupper(tc) != 'A' && infile != NULL);
       { wk2 = 0; system("cls");
         while (fgets(filebuf ,81,infile) != NULL)
           { if (wk2 > 20)
               { marktime(); system("cls"); wk2 = 0; }
             printf("%s",filebuf );
             wk2++; }
             fclose(infile); }
}

help()
   { char tcc[2];
     disphelp("help1.aih");
     gets(tcc);
     while(tcc [0] == 'R' || tcc [0] == 'C' || tcc [0] == 'r' ||
           tcc [0] == 'c' || tcc [0] == 'Q' || tcc [0] == 'q')
       { if (tcc [0] == 'R' || tcc [0] == 'r')
              disphelp("help2.aih");
         else
           { if (tcc[0] == 'C' || tcc [0] == 'c')
                disphelp("help4.aih");
             else
                disphelp("help3.aih"); }
         printf("\n\n Press 'R' for rules, 'Q' for queries, 'C' for commands,");
         printf("\n anything else to return to main input. ");
         gets(tcc);}
}

failfile()
   { printf("\n\nFile procedure has failed."); marktime();
}

exec_string()
    {  char pred[77]; int rs1, rs2, rs3, rs4, findsub();
       rs4 = 1;
       if ( x2 != 1 && x2 != -1 && x2 != 9)
          { rs2 = findsub(indff);
            if (rs2 > MAX)
               { printf("\nSubject not found."); marktime(); }
            else
                 halfques(rs2,rs4); }
       else
          { rs1 = parsepred(indff," THEN ",pred,strlen(indff),6);
            if (rs1 != 1)
              { printf("\nInvalid predicate."); marktime(); }
            else
             { rs2 = findsub(indff);
               rs3 = findsub(pred);
                   if (x2 == -1 && rs2 < MAX && rs3 < MAX)
                     fullques(rs2,rs3);
                   else
                     { if (x2 == 9 && rs2 < MAX && rs3 < MAX)
                         dropem(rs2,rs3);
                       else
                         { if (x2 == 1)
                              newrule(pred,rs2,rs3);
                           else
                              { printf("\nSubject or predicate not found.");
                                marktime(); }}}}}
}
 
findsub(dd)
   char *dd[ ];
   { int fs1, fs2, fs3;
     fs1 = MAX + 1; fs2 = 0;
     while (fs2 < MAX && fs1 > MAX)
        { fs3 = strcmp(dd,subjs[fs2]);
          if (fs3 == 0)
             fs1 = fs2;
          fs2++; }
    return(fs1);
}

newrule(dd,r1,r2)
    char *dd; int r1,r2;
  { char pred[77]; int r3, r4, r5, r6, r7, r8;

    if (r1 == r2 && r1 < MAX)
       r5 = 1;
    else
      { r3 = MAX; r4 = MAX;
        for (r6 = 0; r6 < MAX; r6++)
          { if (r1 > MAX && subjs[r6][0] == '@')
               r1 = r6;
            if (r2 > MAX && subjs[r6][0] == '@' && r1 != r6)
               r2 = r6;}
        if (r1 < MAX && r2 < MAX)
          { r5 = 2;
            strcpy(subjs[r1],indff);
            strcpy(subjs[r2],dd); }
        else
            r5 = 3; }
     if (r5 == 1)
        { printf("\nTautology entered.  Cannot continue."); marktime(); }
     else
        { if (r5 == 3)
           { printf("\nDatabase full.  Cannot add rule."); marktime(); }
          else
             percts(r1,r2);  }
}

percts(d1,d2)
   int d1, d2;
   { char newpctg[3], tmpchr; float ff2;
     if (ft[d1][d2] != 0)
      { printf("\n\nRule exists--probability %5.2f pct.  Change percentage? ",
           ft[d1][d2]);
        gets(newpctg);
        if (toupper(newpctg[0]) == 'Y')
          { printf("\nEnter new percent of time true: ");
            scanf("%f",&ff2); tmpchr = getchar();
              if (ff2 < 0 || ff2 > 100)
                { printf("\nInvalid value.  Old value kept."); marktime(); }
              else
                ft[d1][d2] = ff2; }
        else
           {} }
     else
        { if (oride_sw != 'Y')
            { ft[d1][d2] = dfwd; ft[d2][d1] = drev; }
          else
            { printf("\nEnter percent of time that the following are true.");
              printf("\nIf %s \n   then %s: ",subjs[d1],subjs[d2]);
              scanf("%f",&ff2); tmpchr = getchar();
              {if (ff2 < 0 || ff2 > 100)
                { printf("\nInvalid value.  Default used.");
                  ft[d1][d2] = dfwd; }
               else
                  ft[d1][d2] = ff2;  }
              printf("\nIf %s \n   then %s: ",subjs[d2],subjs[d1]);
              scanf("%f",&ff2); tmpchr = getchar();
              if (ff2 < 0 || ff2 > 100)
                { printf("\nInvalid value.  Default used.");
                  ft[d2][d1] = drev;
                  marktime(); }
              else
                  ft[d2][d1] = ff2; } }
}

halfques(d1,d2)
   int d1, d2;
   { int wh1, wh2;
     if (d2 == 1)
        system("cls");
     wh2 = 1; printf("\nForward references for %s",subjs[d1]);
     for (wh1 = 0; wh1 < MAX; wh1++)
        { if (wh2 > 18 && d2 == 1)
            { marktime(); system("cls"); wh2 = 1;
              printf("\nForward references for %s",subjs[d1]); }
           if (ft[d1][wh1] > 0)
             { printf("\n   then %s  %5.2f pct.",subjs[wh1],ft[d1][wh1]); wh2++;
               }}
     printf("\n\nReverse references for %s",subjs[d1]); wh2++; wh2++;
     for (wh1 = 0; wh1 < MAX; wh1++)
        { if (wh2 > 18 && d2 == 1)
            { marktime(); system("cls"); wh2 = 1;
              printf("\nReverse references for %s",subjs[d1]); }
           if (ft[wh1][d1] > 0)
             { printf("\n   if   %s  %5.2f pct.",subjs[wh1],ft[wh1][d1]); wh2++;
              }}
     if (d2 == 1)
         marktime();
}

view()
   { int lines,  wh2, wh3, wh4, wh5;
     system("cls"); lines = 0;
     for (wh3 = 0; wh3 < MAX; wh3++)
        { if (subjs[wh3][0] != '@')
            { if (lines > 20)
                { marktime(); system("cls"); lines = 0; }
             printf("\nSubject #%d, %s",wh3,subjs[wh3]);
             wh4 = 0; wh5 = 0;
             for (wh2 = 0; wh2 < MAX; wh2++)
               { if (ft[wh3][wh2] > 0)
                   wh4++;
                 if (ft[wh2][wh3] > 0)
                   wh5++; }
             printf("\n     Forward references:  %d",wh4);
             printf("\n     Reverse references:  %d",wh5);
             lines++; lines++; lines++;}}
     marktime();
}

chgsubj()
   { char newsub[77];  int rs1, rs2, rs3, rs4, findsub();
     printf("\n\nEnter subject you wish to change:  ");
     gets(newsub);
     rs2 = findsub(newsub);
     if (rs2 > MAX)
        { printf("\n\nSubject not found."); marktime(); }
     else
        { printf("\n\nEnter new subject.  It will NOT be edited.");
          printf("\nThe subject: ");
          gets(newsub);
          strcpy(subjs[rs2],newsub);}
}

hardcopy()
   { FILE *outfile;
     int wk1, wk2, wk3;
     outfile = fopen("prn:","w");
     if (outfile == NULL)
         printf("\nPrinter not found.  Cannot proceed.");
     else
       { for (wk1 = 0; wk1 < MAX; wk1++)
         { if (subjs[wk1][0] != '@')
             { wk3 = fprintf(outfile,"IF %s\n",subjs[wk1]);
               for (wk2 = 0; wk2 < MAX; wk2++)
                 { if (ft[wk1][wk2]  > 0)
                     wk3 = fprintf(outfile,"   then %s, %8.5f\n",subjs[wk2],ft[wk1][wk2]);
                  }
            wk3 = fprintf(outfile,"Then %s\n",subjs[wk1]);
               for (wk2 = 0; wk2 < MAX; wk2++)
                 { if (ft[wk2][wk1] > 0)
                     wk3 = fprintf(outfile,"   if %s, %8.5f\n",subjs[wk2],ft[wk2][wk1]);
         }}}
         wk3 = fflush(outfile);
         wk3 = fclose(outfile); }
}

droprule()
   { char dr4[81];
     printf("\nEnter rule to be dropped.\n");
     gets(indff);
     x1 = strlen(indff); x2 = 0;
     if (x1 > 7)
        x2 = parsesub(indff,"IF THE ",dr4,x1,7);
     if (x2 == 0 && x1 > 6)
        x2 = parsesub(indff,"IF AN ",dr4,x1,6);
     if (x2 == 0 && x1 > 5)
        x2 = parsesub(indff,"IF A ",dr4,x1,5);
     if (x2 == 0 && x1 > 3)
        x2 = parsesub(indff,"IF ",dr4,x1,3);
     if (x2 != 0)
        { x2 = 9;
          strcpy(indff,dr4);
          exec_string(); }
     else
        { printf("\nInvalid syntax.  Cannot proceed."); marktime(); }
}

dropem(ww1,ww2)
        int ww1, ww2;
      { int ws2, ws3, ws4; char empstr[] = "@";
        if (ft[ww1][ww2] < 0 || ft[ww1][ww2] == 0)
            printf("Rule not found.");
        else
          { ft[ww1][ww2] = 0; printf("\n\n  Rule dropped.");
            ws2 = 0; ws3 = 0;
            for (ws4 = 0; ws4 < MAX; ws4++)
             { if (ft[ww1][ws4] > 0 || ft[ws4][ww1] > 0)
                  ws2 = 1;
               if (ft[ww2][ws4] > 0 || ft[ws4][ww2] > 0)
                  ws3 = 1; }
            if (ws2 == 0)
             { strcpy(subjs[ww1],empstr); blank1pt(ww1);
               printf("\n  Subject removed."); }
            if (ws3 == 0)
             { strcpy(subjs[ww2],empstr); blank1pt(ww2);
               printf("\n  Predicate removed."); }}
            marktime();
}

blank1pt(wx)
   int wx;
       { int sz2;
         for (sz2 = 0; sz2 < MAX; sz2++ )
           ft[wx][sz2] = 0;
         ft[wx][wx] = -100;
}
