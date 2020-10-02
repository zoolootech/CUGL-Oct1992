/*  >  DPRINTF-F.C
 *
 *  dprintf -- PrintFloat using floor()
 *  (C)  April 4  1990  Asaf Arkin
 *  All rights reserved
 *
 *  Altered PrintFloat does no double to int casts; the math library floor
 *  function is used instead.
 *
 *  PrintFloat and ToFloat perform floating point arithmetic, requiring the
 *  presence of a FP processor or emulator, and the floor() function (rounds
 *  down a double to its integer value.)
 */


/*  void  PrintFloat(long double, int, int, char, char, int *)
 *
 *  Print a floating point number in standard (%f) or engineering (%e) form;
 *  the %g format requires that the shortest of the two be selected. The
 *  number divides into integer, fraction and exponent parts; the exponent
 *  is stringized with ToInteger, the integer and fraction with ToFloat.
 */

static void  PrintFloat(long double Float, int Width, int Precis, char Flags,
                        char Format, int *OutCnt)
{
  :
  long double  Float2;

  :
  :
  /*  The mantissa divides into integer and fraction parts, stringized by
   *  ToFloat: the last digit always rounds up; a period is printed only
   *  before a fraction or in the variant format; the %g format allows
   *  trailing zeros in the fraction to be lost.
   *  N.B.: floor rounds only doubles -- long doubles are not catered for.
   */
  Float2=floor(Float);
  Float-=Float2;
  if (Precis<=0 && Float>=.5)
    ++Float2;
  LengthI=ToFloat(&BufferI,(double) Float2,-1);
  if (Precis>0)
  {
    for (LengthF=0; LengthF<Precis; ++LengthF)
      Float*=10;
    Float2=floor(Float);
    Float-=Float2;
    if (Float>=.5)
      ++Float2;
    LengthF=ToFloat(&BufferF,(double) Float2,Precis);
  }
  else
    LengthF=0;
  :
  :
}


/*  int  ToFloat(char **, double, int)
 *
 *  Convert a double to a NULL-terminated string of digits. If the string has
 *  less digits than the precision, additional zeros are inserted at the
 *  start of it. ToFloat allocates a memory block in which it stores the
 *  string -- Buffer returns its address.
 */

static int  ToFloat(char **Buffer, double Float, int Precis)
{
  int  Cnt,  Length,  Char;
  double  TempFloat = Float;

  if (Precis<0)
    Precis=1;
  for (Cnt=0; TempFloat>=1; TempFloat/=10, ++Cnt)  ;
  *Buffer=malloc(Maximum(Precis,Cnt)+1);
  if (*Buffer==NULL)
    return  0;
  for (Length=0; Length+Cnt<Precis; )
    (*Buffer)[Length++]='0';
  Cnt= Length=Maximum(Length+Cnt,Precis);
  (*Buffer)[Length]='\0';
  for (; Float>=1; Float/=10)
  {
    Char=floor(Float-floor(Float/10)*10);
    (*Buffer)[--Cnt]=ToDigit(Char);
  }
  return  Length;
}


