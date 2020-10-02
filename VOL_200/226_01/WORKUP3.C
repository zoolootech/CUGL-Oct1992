   /*  HEADER:CUG 226;
       TITLE:"ART-CEE";
       VERSION:1.4;
       DESCRIPTION:"Small artificial intelligence inference engine.

            ART-CEE is a rule-based general purpose driver capable of working
            with heirarchical and/or propositional information.";
       KEYWORDS:artificial intelligence, inference engines, database;
       SYSTEM:MS-DOS;
       FILENAME:WORKUP3.C;
       SEE-ALSO:WORKUP0.C, WORKUP2.C, WORKUP4.C, MANUAL.TXT, WORKUP0.H,
            WORKUP1.H, HELP1.AIH, HELP2.AIH, HELP3.AIH, HELP4.AIH, ART-CEE.COM;
       AUTHOR:Dan Cheatham;
       COMPILER:MIX-C;
   */


         /*  art-cee  artificial intelligence inference engine    */
         /*                                                       */
         /*  file:  workup3.c    --  thinking                     */
         /*                                                       */

#include "workup0.h"
#include "workup1.h"

    static int currsub[MAX], currpred[MAX], instant[MAX], s1, q4, ll4;

fullques(rr1,rr2)
   int rr1, rr2;
   { int wk3;
     if (ft[rr1][rr2] > 0)
       { ff1 = ft[rr1][rr2];
         statecon(rr1,rr2,0);}
     else
       { if (ft[rr1][rr2] < 0)
            stateimp(rr1,rr2,0);
         else
              queryloop(rr1,rr2); }
   marktime();
}

statecon(rw1,rw2,rw3)
    int rw1, rw2, rw3;
   { printf("\nIf %s\n    then %s",subjs[rw1],subjs[rw2]);
     printf("   is demonstrated with a probability of %f pct.\n",ff1);
     if (rw3 > 0)
         askreadd(rw1,rw2);
}

askreadd(de1,de2)
   int de1,de2;
   { char answer[2];
     printf("\nShall this fact be added to database (Y/N)? ");
     gets(answer);
     if (toupper(answer[0]) == 'Y')
        { ft[de1][de2] = ff1;
          printf("\nNew learning added.");}
}

stateimp(rz1,rz2,rz3)
   int rz1, rz2, rz3;
   { printf("\nIf %s\n   then %s",subjs[rz1],subjs[rz2]);
     printf("\n    cannot be, given present knowledge.");
     if (rz3 > 0)
        { ff1 = -100; askreadd(rz1,rz2,ff1); }
}

queryloop(q1,q2)
   int q1, q2;
   { int q3, q5, q6;     char dumment[2];
     q4 = MAX - 1; ll4 = q4;
     for (q3 = 0; q3 < MAX; q3++)
       { currsub[q3] = 0;
         currpred[q3] = 0;
         instant[q3] = 0; }
     currsub[0] = q1;
     s1 = 0;
     instant[0] = 1;
     q6 = cntinfer();
     while(currpred[0] < MAX)
        { if (ft[currsub[s1]][currpred[s1]]  >= 0 && q6 <= assumpno &&
           ft[currsub[0]][currpred[s1]] >= 0 &&
              subjs[currpred[s1]][0] != '@')
           { if (currpred[s1] == q2)
              {drawconc(q1,q2); moveback();}
             else
                movenext(); }
          else
             moveback();
          q6 = cntinfer();}
}

cntinfer()
   { int cr1, cr2;
     cr2 = 0;
     for (cr1 = 0; cr1 <= s1; cr1++)
       { if (ft[currsub[cr1]][currpred[cr1]] == 0)
            cr2++;}
     return(cr2);
}

movenext()
  { int mn1, mn2;
    if (s1 < q4)
      { mn1 = s1;  s1++;
        currsub[s1] = currpred[mn1];
        currpred[s1] = 0;
        mn2 = checkdups();
        if (mn2 != 0)
            { currpred[s1] = ll4; moveback(); }
        else
            {}}
    else
        moveback();
}

checkdups()
  { int cd1, cd2;
    cd2 = 0;
    for (cd1 = 0; cd1 < s1; cd1++)
      { if (currsub[cd1] == currsub[s1])
           cd2 = 1; }
    return(cd2);
}

moveback()
  { int mb1, mb2, mb3;
    if (s1 == 0)
       { if (currpred[0] == ll4)
           currpred[0] = MAX;
         else
           currpred[0]++; }
    else
       { if (s1 == q4 || currpred[s1] == ll4)
            { s1--; moveback(); }
         else
            currpred[s1]++; }
}

drawconc(cd1,cd2)
   { int dc1, dc2, dc3, dc4;  char resp[2], dummystr[2];
     dc1 = 1; dc2 = 0; strcpy(resp,"Y"); ff1 = 1.00;
     if (s1 > 0)
      {  while (dc1 == 1 && dc2 < s1)
         { printf("\nIt has been given that %s , and...",subjs[currsub[dc2]]);
           dc2++;
           dc1 = instant[currsub[dc2]] ; }

      while (dc2 <= s1 && resp[0] == 'Y')
          { instant[dc2] = 1;
            printf("\n  then is it also true that...\n   %s (Y/N)? ",subjs[currsub[dc2]]);
            gets(resp);
            if (toupper(resp[0]) == 'Y')
              { instant[currsub[dc2]] = 1; dc3 = dc2 - 1;
                if (ft[currsub[dc3]][currsub[dc2]] == 0)
                  { printf("\nEnter percent of time that the following is true.");
                    printf("\n  If %s",subjs[currsub[dc3]]);
                    printf("\n     then %s\nPercent:  ",subjs[currsub[dc2]]);
                    dc4 = scanf("%f",&ft[currsub[dc3]][currsub[dc2]]); gets(dummystr);
                    if (dc4 != 1)
                       ft[currsub[dc3]][currsub[dc2]] = -100;
                    if (ft[currsub[dc3]][currsub[dc2]] > 100)
                       ft[currsub[dc3]][currsub[dc2]] = 100;
                    else
                       { if (ft[currsub[dc3]][currsub[dc2]] < 0)
                          { printf("\nImpossibility rejected.");
                            ft[currsub[dc3]][currsub[dc2]] = 0;
                            instant[currsub[dc2]] = 0;
                            strcpy(resp,"N"); }}}}
            else
                instant[currsub[dc2]] = 0;
            dc2++;}

         if (toupper(resp[0]) == 'Y')
           { for (dc1 = 0; dc1 <= s1; dc1++)
                ff1 = ff1 * (ft[currsub[dc1]][currpred[dc1]] / 100);
             ff1 = 100 * ff1;

         if (ff1 > 0)
           { printf("\nthen, finally, %s",subjs[currpred[s1]]);
             statecon(cd1,cd2,1);}}}
}

think()
   { int t1, t2, t3, t4, t5;
     t4 = 0;
     for (t1 = 0; t1 < MAX; t1++)
      { if (subjs[t1][0] != '@')
        { for (t2 = 0; t2 < MAX; t2++)
          { if (subjs[t2][0] != '@' && t1 != t2)
            { if (ft[t1][t2] == -100)
              { for (t3 = 0; t3 < MAX; t3++)
                { if (subjs[t3][0] != '@' && t1 != t3)
                  { if (ft[t3][t2] == 100 && ft[t1][t3] == 0)
                     { ft[t1][t3] = -100;
                       t4++; }}}}}}}}
      printf("\n %d logical impossibilities identified and marked.",t4);
 
      ll4 = MAX - 1; t3 = 0; q4 = srchdpth;
  
      for (t1 = 0; t1 < MAX; t1++)
       { for (t2 = 0; t2 < MAX; t2++)
         { currsub[t2] = 0;
           currpred[t2] = 0; }
         if (subjs[t1][0] != '@')
           { s1 = 0; printf("\nNow analyzing references for %s",subjs[t1]);
             currsub[s1] = t1;
             while(currpred[0] < MAX)
              { if (subjs[currpred[s1]][0] != '@'
                   && ft[currsub[s1]][currpred[s1]] > 0
                   && ft[currsub[0]][currpred[s1]] >= 0)
                      { t3 = t3 + figpct(s1); movenext(); }
                else
                   moveback();}}}
      printf("\n\n %d new learnings added or updated.",t3);
      marktime();
}
 
figpct(fp1)
    int fp1;
    { int fp2;
      if (ft[currsub[0]][currpred[fp1]] >= 0)
         { ff1 = 1;
           for (fp2 = 0; fp2 <= fp1; fp2++)
            ff1 = ff1 * (ft[currsub[fp2]][currpred[fp2]] / 100);
          ff1 = 100 * ff1;
          if (ff1 > ft[currsub[0]][currpred[fp1]])
             { ft[currsub[0]][currpred[fp1]] = ff1; return(1);}
          else
             return(0);}
}
