#include <dos.h>

#define TimerResolution    1193181.667

void cardinal(long l, double *result)
{

  *result = ((l < 0) ? (double)4294967296.0 + (long) l : (double)((long) l));

}

void elapsedtime(long start, long stop, double *result)
{
  double r;

  cardinal(stop - start,&r);
  *result = (1000.0 * r) / TimerResolution;

}

