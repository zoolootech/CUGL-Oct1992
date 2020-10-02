/*  PLOX keyword handling modules  04-21-90       FILE: CONLIB.C
         Robert L. Patton, Jr.
         1713 Parkcrest Terrace
         Arlington, TX 76012
*/
void Unknown (char *Word);
void ARcon (char *Sentence);
void AXcon (char *Sentence);
void BRcon (char *Sentence);
void HUcon (char *Sentence);
void IScon (char *Sentence);
void LAcon (char *Sentence);
void LIcon (char *Sentence);
int  ItemOut (int Item);
int  LineType (char *Word, int *Trace);
void SetDtype (char *Sentence, char *Dtype);
void SetItems (char *Sentence, int *Ix, int *Iy);
void PCcon  (char *Sentence);
void TLcon  (char *Sentence);
