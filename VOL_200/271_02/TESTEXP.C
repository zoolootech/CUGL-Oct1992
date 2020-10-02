/*
**          SM.LIB  function source code for Turbo-C
**  Copyright 1987, S.E. Margison
**
**  FUNCTION: expand_args function demonstration
**  This file will demonstrate the use of the expand_args() function.
**  It does nothing more than report all the expanded functions.
*/

#include <stdio.h>
#include <smdefs.h>
#include <dir.h>

int nargc;

main(argc, argv) int argc; char *argv[]; {

   int i;

   for(i = 0; i < argc; i++)
      printf("argc #%d is %s\n", i, argv[i]);  /* report actual args */

   nargc = expand_args(argc, argv);

   printf("\nexpand_args returned %d total arguments\n\n", nargc);

   for(i = 0; i < nargc; i++)
      printf("argc #%d is %s\n", i, nargv[i]); /* report new arguments */

   error("Finished");
   }
