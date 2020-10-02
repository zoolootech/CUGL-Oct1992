#include <stdio.h>
#include <math.h>

main()
{
  int ang;

  for (ang = 0; ang < 90; ang += 5) {
    printf("ang = %d  ixmax = %d, %f\n", ang, (255*ang)/90,
     127.5*(1.0 + sqrt(2.0)*sin(((double) ang - 45.0)*3.14/180.)));
   }
}