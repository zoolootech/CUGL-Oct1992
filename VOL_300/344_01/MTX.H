/*-------------------------------------------------------------------------
   Main File : mtx.exe
   File Name : mtx.h

   Purpose - Global defines, includes, types
--------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <conio.h>
#include <limits.h>
#include <string.h>
#include <ctype.h>


  /* Allocation macro */
#define alloc(a,b)          calloc((a),(b))

  /* Indexing macro for main matrix */
#define A(i,j)              *(Aptr+((i)*n)+(j))

  /* Indexing macro for 'b' */
#define b(i)                *(bptr+(i))

  /* Numerical type of elements */
typedef double Mtype;

  /* Corresponding conversion and abs functions */
#define atoMtype(s)         atof((s))
#define absMtype(x)         fabs((x))

  /* Error Output define */
#define fs(s)               fprintf(stderr,"%s",s)

  /* Default precision define */
#define default_precision   2

  /* Boolean definition */
typedef enum {
  FALSE,
  TRUE
} Boolean;
