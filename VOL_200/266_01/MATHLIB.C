/*     General Math Library        File: MATHLIB.C
           18 Nov 1986       */

#include "MATHLIB.H"
/*     =====    */
double Exp10 (N)
/*     =====     Finds 10 ** N  */
int  N;
{
double D = 1;
  for (;N>0; N--) D = D * 10;
  for (;N<0; N++) D = D / 10;
  return D;
}
/*  ==== */
int Isin (Dir)
/*  ====       Integer sine = 0 or 1 */
char Dir;
{
  return (Dir == 'V');
}
/*  ==== */
int Icos (Dir)
/*  ====       Integer cosine = 0 or 1 */
char Dir;
{
  return (Dir == 'H');
}
/*  ====     */
int Sign (X)
/*  ====     Returns 1 with sign of X  */
int X;
{
  if (X < 0) return -1; else return 1;
}
