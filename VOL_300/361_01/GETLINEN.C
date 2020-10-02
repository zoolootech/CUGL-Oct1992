
/* GetLineN.c ---> Ramdom Access a Text File by Line Number.
 *
 * Author: J.Ekwall                                     13 September 91
 *
 * Copyrighted to the Public Domain.  Unlimited Distribution Authorized.
 *
 * User Assumes All Risks/Liabilities.
 *
 * Last Update: 13 September 91/EK
 */

#include <gadgets.h>
#include <stdek.h>
#include <stdio.h>
#include <string.h>

int GetLineN(char *Line, int Which1)
{
    static int LastKnownLine    = 0;
    static int LineInc          = 1;
    static int NextLine         = 0;
    static int EOFatLine        = 32500;
    static long Lines[501];     /* List of File Jump Pointers */
    static FILE *fp = NULL;
    int c, i, j;
    char *tp1;

 /* IF Initialize/Close */
    if (Which1-- < 1) {
       if (fp != NULL) fclose(fp); EOFatLine = 32500;
       NextLine = LastKnownLine = 0; LineInc = 1; Lines[0] = 0L; 
       if (Line != NULL && *Line && (fp = fopen(Line, "r")) IS NULL) 
          return FALSE;
       return TRUE;
     }
     
 /* If Within Known Range */
    if (Which1 <  0 || Which1 >= EOFatLine) { 
       strcpy(Line,"~"); return FALSE; }
    if (Which1 < LastKnownLine) {
       if (Which1 != NextLine) {
          i = Which1 / LineInc; fseek(fp, Lines[i], SEEK_SET);
          i = Which1 % LineInc;
       } else i = 0;
       for ( ; i >= 0; i--) fgets(Line, 255, fp);
       Line[strlen(Line)-1] = NULL; NextLine = ++Which1;
       return TRUE;
    }

 /* Not in Known Range */
    i = LastKnownLine / LineInc;
    if (Which1 != NextLine) {
       fseek(fp,Lines[i],SEEK_SET); LastKnownLine = i * LineInc; }
    while (LastKnownLine <= Which1) {
       if (fgets(Line, 255, fp) IS NULL) {
          EOFatLine=LastKnownLine; strcpy(Line, "~"); return FALSE; }
       if (++LastKnownLine % LineInc IS 0) {
          Lines[++i] = ftell(fp);
          if (i IS 500) {               /* OverFlow --> Crunch List */
             LineInc *= 2; for (i = 1; i <= 250; i++) Lines[i] = Lines[2*i];
             i = 250;
          }
       }
    }
    Line[strlen(Line)-1] = NULL; NextLine = LastKnownLine; return TRUE;
}

