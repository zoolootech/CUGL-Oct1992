/* MS C, MS Quick C
  example for dynamic attribute modification
*/
#include "_ufc01.h"
#include "testfmt.h"
main()
{
#include "testfmt.i"
/* normal output */
memcpy(FMT,"testfmt ",8);
FKZ = 2;
SM = 0;
UNIF(&FKZ, FMT, testfmt.fld0, &RET, &SM, Daten);

/* change attribute of field fld1 */
FKZ = 20;
SM = 1;
RET = 15;
UNIF(&FKZ, FMT, testfmt.fld0, &RET, &SM, Daten);
FKZ = 3;
UNIF(&FKZ, FMT, testfmt.fld0, &RET, &SM, Daten);

/* change textattribut and frame attribut */
FKZ = 21;
RET = 15;
UNIF(&FKZ, FMT, testfmt.fld0, &RET, &SM, Daten);
FKZ = 22;
RET = 15;
UNIF(&FKZ, FMT, testfmt.fld0, &RET, &SM, Daten);
FKZ = 2;
UNIF(&FKZ, FMT, testfmt.fld0, &RET, &SM, Daten);

}
