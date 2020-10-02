/*                                                FILE: IMAGLIB.H
*/

void Dot (int X, int Y);   /* Turns on the pixel at X,Y  */

void Draw (int Xmax, int Ymax, int Bold);
/*   ====  Draws the memory image on the printer.  */

void Line (int X1, int Y1, int X2, int Y2);
/*   ====               Draws a straight line */

void NewImage (void);
/*   ========   Clears the plot image (to binary zeros) */

void NoDot (int X, int Y);
/*   =====      Turns off the pixel at X,Y  */
