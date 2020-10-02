/*
** args.c	Example of obtaining runstring arguments
**		via argc and argv in Small C - 2.7
**
**		by F. A. Scacchitti 7/17/84
**
**  Just like K & R  !
**
*/

#include <stdio.h>

main(argc,argv) int argc; char *argv[]; {

int count;
count = 1;

/* One line does it all*/

while(count != argc)
   printf("\n argument = %2d, argv = %d, *argv = %d, char = %c, string = %s",          count++,argv++,*argv,**argv,*argv);

printf("\n");     /* makes it look neat */
}

