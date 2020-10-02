/*    Keyword-value Text Handling Routines       File: WORDLIB.H
        Robert L. Patton, Jr.
        1713 Parkcrest Terrace
        Arlington, TX 76012
           05 May 1989
*/
void Blank      (char  *Word, int Size);
void Capitalize (char  *Word);
void Clean      (char  *Sentence);
void Ftoa       (double Value, char *Digits);
int  Filler     (char  *Word);
void GetInt     (char  *Sentence, int *Number);
void GetVal     (char  *Sentence, double *Value);
void GetWord    (char  *Sentence, char *Word, int Size);
int  iKeyFind   (char **List, int Length, char *Word);
void PauseMsg   (char  *Text);
