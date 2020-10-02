/*         Data Handling Package    06 May 1989        File:DATLIB.C
                         revised    03 Jul 1990
                Robert L. Patton, Jr.
                1713 Parkcrest Terrace
                Arlington, TX 76012

    The first value in each array is the number of values following.
    Data types are: 'N' - numeric
                    'D' - date as yymmdd
                    'W' - word (later)
*/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "datlib.h"
#include "wordlib.h"
#define  POINTS  200
#define  FNLEN    30
#define  ADHOC   '@'
#define  EMPTY   '*'
#define  IBID    '*'
#define  FINIS   '$'
#define  SPLOX   '@'
#define  EQUAL(A,B) !strcmp(A,B)

FILE   *Data,*Plox;
static char  AdHocFile[FNLEN+1] = {EMPTY,'\0'};
static float Val [2] [POINTS+1];

void AdHoc (char *FileName) {
/*   =====                                Accept ad hoc file name */
    strncpy (AdHocFile, FileName, FNLEN);
}
int CumDays (Month, Year)
/*  =======              No. of days in year up to start of month */
int Month, Year;
{
  static int Cum[] = {0,31,59,90,120,151,181,212,243,273,304,334};

  return (Cum[Month-1] + (Month>2? Leap(Year) : 0) );
}

int Evaluate (Word, Type, Value)
/*  ======== */
char  *Word,Type;   /* Find the float value of a     */
float *Value;       /* string with a given data type */
{
  int D,M,Y;

  if      (Word[0]==EMPTY)
    return 1;
  else if (Type=='N')            /* Numeric */
    *Value = (float) atof(Word);

  else if (Type=='D') {        /* Date as YYMMDD */
    Split (Word, &Y, &M, &D);  /* gives YY.frac  */
    if (Y < 50) Y += 100;
    *Value = (float) Y + ((float) (CumDays(M,Y) + D - 1))
                       / ((float) (365 + Leap(Y)));
  }
  else
    *Value = 0.0;

  return 0;
}

int Leap (Year)
/*  ====       = 1 for leap year, 0 if not */
int Year;
{
  return !(Year %(Year%100? 4: 400));
}
/*@@*/
void GetTwo (FileName, Xptr, Xtype, Xitem, Yptr, Ytype, Yitem)
/*   ====== */
char    *FileName;      /* Reads a set of X,Y points as     */
char    Xtype, Ytype;   /* designated items on a data line  */
float   **Xptr, **Yptr;   /* and returns pointers to the data */
int     Xitem, Yitem;
{
  char Sentence[81], Word[21];
  int  n, Count, Xvoid, Yvoid;
/* -----------------------------------------
printf("GetTwo(%s,%u,%c,%d,%u,%c,%d)\n",
FileName,*Xptr,Xtype,Xitem,*Yptr,Ytype,Yitem);
------------------------------------------ */
  Count = 0;
  if (FileName[0] == IBID) Data = Plox;
  else {
      if (FileName[0] == ADHOC) {
          if (AdHocFile[0] == EMPTY) {
              printf ("   Enter ad hoc dat file name: ");
              scanf ("%30s",AdHocFile);
          }
          Data = fopen (AdHocFile,"r");
      }
      else Data=fopen (FileName,"r");
  }
  if (Data != NULL) {
    Count = 1;
    while (fgets (Sentence, 80, Data) != NULL) {
      if (Sentence[0] != '/' &&        /* bypass comment */
          Sentence[0] !='\032') {      /* and CTRL Z */

        if (Sentence[0] == FINIS) break;
        if (Sentence[0] == SPLOX) {     /* Handle PLOX statements in data */
            Clean (Sentence);
            Sentence[0] = ' ';
            Sentence[strlen(Sentence)-1] = '\0';
            if ((Sentence[1]-'0') == Yitem) {
                Sentence[1] = ' ';
                GetWord (Sentence, Word, 12);
                Capitalize (Word);
                if      (EQUAL (Word,"TITLE")) TLcon (Sentence);
                else if (EQUAL (Word,"LABEL")) LAcon (Sentence);
                else if (EQUAL (Word,"ISO"))   IScon (Sentence);
                else if (EQUAL (Word,"HUE"))   HUcon (Sentence);
                else if (EQUAL (Word,"AXIS"))  AXcon (Sentence);
                else PauseMsg("Unknown control word after @ in data file");
            }
        }
        else {
            for (n=1; n<=(Xitem>Yitem? Xitem: Yitem); n++) {
              GetWord (Sentence, Word, 20);
              if (n == Xitem)
                Xvoid = Evaluate (Word, Xtype, &Val [0] [Count]);
              else if (n == Yitem)
                Yvoid = Evaluate (Word, Ytype, &Val [1] [Count]);
            }
            if (Xvoid || Yvoid) Count--;
            if (++Count > POINTS) break;
        }
      }
    }
    Count--;
    if (Data != Plox) fclose (Data);
  }
  Val [0] [0] = (float) Count;
  Val [1] [0] = (float) Count;
  *Xptr = &Val [0] [0];
  *Yptr = &Val [1] [0];
}

void SavePlox (PloxPtr)
/*   ========           Accept the PLOX file pointer */
FILE *PloxPtr;
{
  Plox = PloxPtr;
}

void Split (YYMMDD,Y,M,D)
/*   =====               string date to int year, month, day */
char *YYMMDD;
int  *Y,*M,*D;
{
  char Date[3][3];
  register int  n;

  for (n=0; n<3; n++) {
    Date[n][0] = YYMMDD[2*n];
    Date[n][1] = YYMMDD[2*n+1];
    Date[n][2] = '\0';
  }
  *Y = atoi (Date[0]);
  *M = atoi (Date[1]);
  *D = atoi (Date[2]);
}
