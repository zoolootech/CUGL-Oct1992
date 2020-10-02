#include "a:stdio.h"
#include "a:math.h"
main()
{ int i ; float sq(),cu() ;
 for (i= 0; i < 15 ; i++)
 printf("%d %f %f\n",i,sq( (float) i),cu( (float) i) ) ;
}


