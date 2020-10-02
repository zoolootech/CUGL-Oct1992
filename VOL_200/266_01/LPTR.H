/*     Line Printer Control Routines - Epson FX-80 printer       FILE
                      Header File                               LPTR.H
                      01 May 1990
*/
void LP_LineSpace (int N72nds);
void LP_GraphMode (int LineBits);
void LP_Margin (int Column);
void LP_CrLf (int N);
void LP_Send (int Byte);
void LP_SetType (char *NewType);
void LP_Reset (void);
