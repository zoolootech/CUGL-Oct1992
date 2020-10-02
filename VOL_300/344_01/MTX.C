/*-----------------------------------------------------------------------
   Program : mtx.exe

   Author  : Bill Forseth
             1405 8th Ave N
             Fargo, ND  58102
             (701) 239-0150

   Purpose - 1. Solves A|b matricies using Gauss-Jordan elimination.
             2. Output adjustable, suitable for indirection

   Syntax  - mtx < input_data_file N [options]

             where : input_data_file contains an N(N+1)
                       matrix in A|b form of integers or reals
                     N is the size of one column
                     [options]
                       -s   (display the solution only)
                       -pn  (n is the precision - defaults to 2)

             When used in conjunction with 'randmtx' the following syntax
             is suggested:

                randmtx n | mtx n [opts] > out_file

   Files : mtx.c,.h     : main calling module and global defines
           mtxcle.c,.h, : command line, error handling
           mtxio.c,.h,  : allocation, initialization,output display
           mtxsolv.c,.h : G-J computations

           randmtx.c    : creates a random, variable n(n+1) matrix
                          (seperate executable program)

   Compiler : Torbo C, v.2.0 (IDE)
   Switches : small model,
              8086 instruction set,
              floating point emulation,
              word alingment,
              all debug info off,
              register optimization

   Updates - 1/4/91  - created
             2/25/91 - optimized, modularized
             5/29/91 - cleaned up, expanded
-------------------------------------------------------------------------*/
#include "mtxcle.h"
#include "mtxio.h"
#include "mtxsolv.h"

extern int display_all;

main(int argc, char **argv)
{
  get_args(argc,argv);
  read_matrix();
  print_matrix(display_all ? SHOW_MATRIX : NO_SHOW);
  solve_matrix();
  print_matrix(display_all ? SHOW_MATRIX : SOLUTION_ONLY);

  return 0;
}
