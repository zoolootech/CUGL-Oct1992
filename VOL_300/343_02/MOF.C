

           /******************************************************
           *
           *       file d:\cips\mof.c
           *
           *       Functions: This file contains
           *          my_open
           *
           *       Purpose: This function opens a file. Borland's
           *          Turbo C opens files a little different from
           *          the standard UNIX C. Instead of using this
           *          different method in a number of various files,
           *          the method is placed in this one file. If the
           *          programs are moved to another system, all changes
           *          will be located in this one place.
           *
           *       External Calls:
           *          none
           *
           *       Modifications:
           *          18 June 1987 - created
           *
           ****************************************************/


#include "d:\cips\cips.h"

my_open(file_name)
   char file_name[];
{
   int file_descriptor;

   file_descriptor = open(file_name, O_RDWR | O_CREAT | O_BINARY, 
                          S_IWRITE);

   return(file_descriptor);


}  /* ends my_open  */
