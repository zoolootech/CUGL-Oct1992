/*                 MICRO PLOX                         FILE: PLOX.C
                   Version 5.0
     Copyright Robert L. Patton, Jr. 02 July 1990
               1713 Parkcrest Terrace
               Arlington, TX 76012
  Reads a file of PLOX chart specification statements and executes
  them as encountered to produce a binary file of plot commands.
     WITH  CONLIB,DATLIB,DRAWLIB,MATHLIB,PXLIB,WORDLIB
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "PLOX.H"
#include "DATLIB.H"
#include "CONLIB.H"
#include "WORDLIB.H"
#include "PXLIB.H"
#include "MATHLIB.H"
#include "DRAWLIB.H"
main (int ArgC, char **ArgV)
{
  #define LINE 81
  #define LEN  12
  char Sentence[LINE], Word[LEN+1];
  FILE  *Control;

  printf("\n micro PLOX Version 5.0  07 Jul 1990\n\n");
  if ((Control = fopen(ArgV[1],"r"))==NULL){
    Capitalize (ArgV[1]);
    printf("PLOX statements file, %s, not found.\n\n",ArgV[1]);
  }
  else {
    if (ArgC > 2) AdHoc (ArgV[2]);
    SavePlox (Control);
    while (fgets(Sentence,LINE,Control) != NULL) {
      Sentence[strlen(Sentence)-1] = '\0';
      puts(Sentence);
      Clean(Sentence);
      GetWord (Sentence,Word,LEN);
      Capitalize(Word);
      if (Word[0]=='*' || Word[0]==' ');
      else if (EQUAL (Word,"PIC"))   PCcon (Sentence);
      else if (EQUAL (Word,"TITLE")) TLcon (Sentence);
      else if (EQUAL (Word,"AREA"))  ARcon (Sentence);
      else if (EQUAL (Word,"AXIS"))  AXcon (Sentence);
      else if (EQUAL (Word,"LINE"))  LIcon (Sentence);
      else if (EQUAL (Word,"BARS"))  BRcon (Sentence);
      else if (EQUAL (Word,"LABEL")) LAcon (Sentence);
      else if (EQUAL (Word,"ISO"))   IScon (Sentence);
      else if (EQUAL (Word,"HUE"))   HUcon (Sentence);
      else Unknown (Word);
    }
    PCcon ("CLOSE");
  }
}
