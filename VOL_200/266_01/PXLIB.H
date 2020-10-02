/*       Front End Plot Drawing Routines      File: PXLIB.H
            04-21-90
         WITH MATHLIB
*/
void PX_Close  (int X,  int Y);  /* Close the plot file */
void PX_Dot    (int X,  int Y);  /* Turn on X,Y pixel. */
void PX_Draw   (int X,  int Y);  /* Draw vector to X,Y pixel loc. */
void PX_Hatch  (int X1, int Y1,
                int X2, int Y2); /* Hatch an area */
void PX_Hue    (int Color);      /* Set color */
void PX_Margin (int Column);     /* Set plot left margin */
void PX_Move   (int X, int Y);   /* Move to X,Y pixel loc. */
void PX_Open   (void);           /* Open the plot file */
void PX_Origin (int X, int Y);   /* Set new plot origin */
void PX_Style  (int Type,        /* Set hatching pattern */
                int Spacing);
void PX_Symbol (int Shape);      /* Draw symmetrical, centered symbol */
void PX_Text   (int X, int Y,    /* Draws a string of text */
                int Size,
                char Dir,
                char *Text_Ptr);
void PX_Trail  (int  Mark);      /* Sets the line type */
