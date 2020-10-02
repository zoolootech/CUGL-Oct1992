/*
MSC/ MS Quick C
ufc013.c
*/

/* variable fields */
    #include "_UFC01.h"
    #include "order.h"

main() {
/* save screen */
    FKZ = 0;
    UNIF (&FKZ, FMT, FMT, &RET, &SM, Daten);
/* clear screen and display new format */
    FKZ = 1;
    ORDER:
    memcpy(FMT, "order   ", 8);
    #include "order.i"
    UNIF (&FKZ, FMT, order.product, &RET, &SM, Daten);
/* same format when ESC */
    if(RET == 99) 
     { FKZ = 3; goto ORDER; }
/* program end when F10 */
    if(RET == 110) goto ENDE;
/* display empty format */
    FKZ = 4;
    memcpy(FMT, "message ", 8);
    #include "message.i"
    UNIF (&FKZ, FMT, FMT, &RET, &SM, Daten);
/* display new format without clear screen */
    FKZ = 2;
    goto ORDER;
/* display saved screen */
    ENDE:
    FKZ = 5;
    UNIF (&FKZ, FMT, FMT, &RET, &SM, Daten);
}
