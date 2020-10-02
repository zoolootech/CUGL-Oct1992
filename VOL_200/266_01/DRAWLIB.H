/* micro PLOX Front End Plot Utility Routines        File: DRAWLIB.H
        Robert L. Patton, Jr.
        1713 Parkcrest Terrace
        Arlington, TX 76012
           04-26-90
*/
void Annote (int X, int Y, int Offset, char Dir,   int  Mode,
                           int Side,   char *Word, int  Font);
void AxNum (int Xa, int Ya, char Dir, int Side, int  Span,
            SCALING *Scale, int Boxes);
void Bar  (int X1, int Y1, int X2, int Y2, char Dir,
                      int Width, int Hatch, int Gap);
void Box  (int X1, int Y1, int X2, int Y2);
void BoxTry (int Span, int *MinBox, int *MaxBox);
void CopyScale (SCALING *From, SCALING *To);
void Fit (double Min, double Max, int Boxes, double *Start, double *Delta);
int  IsHue (char *Word);
void Liner (float *Xptr, SCALING *Xfit, float *Yptr, SCALING *Yfit,
            int LineType, int PointType);
int  Pct_Fit (double Min, double Max, int Boxes);
void Scale (double Min, double Max, int Span, int Exact,
            int *Boxes, SCALING *Adjust);
int  Scaled (float *Vptr, SCALING *Vfit);
void ShowScale (char Dir, int Boxes, SCALING *Fit);
void TicMark (int Xt, int Yt, int TicLen, char Dir, int Side);
int  TxHigh (int Font);
int  TxWide (int Nchars, int Font);
