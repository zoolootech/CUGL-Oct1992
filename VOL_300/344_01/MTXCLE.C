/*------------------------------------------------------------------------
   Main File : mtx.exe
   File Name : mtxcle.c

   Purpose - command line, error handling
-------------------------------------------------------------------------*/
#include "mtx.h"
#include "mtxcle.h"


int P = default_precision;
int display_all = TRUE;
int MSize;


void get_args(int argc, char **argv)
{
  int i;

  if(argc < 2)
    syntax();

  if((MSize=atoi(argv[1])) <= 0)
    error(errLOWSIZE);

  for(i=2; i < argc; ++i)
  {
    switch(argv[i][1])
    {
      case 's' : display_all=FALSE;
                 break;

      case 'p' : P=atoi(argv[i]+2);
                 if(P < 1 || P > 18)
                   error(errPREC);
                 break;

      default  : fprintf(stderr,"MTX :: unknown flag \"");
                 fprintf(stderr,"%s\"\n",argv[i]);
                 exit(1);
    }
  }
}




void syntax()
{
  fs("\nSyntax : MTX < filename n [opts]\n");
  fs("     filename : redirected ascii text file with A|b matrix\n");
  fs("     n        : n for n(n+1) matrix\n");
  fs("  [opts]\n");\
  fs("    -s        : display solution only (suitable for input data)\n");
  fs("    -pn       : precision (0..18) - defaults to 2\n");

  exit(0);
}






void error(int eflag)
{
   char *errs[] = {
 /* errSINGULARITY */ "\nError - Singularity\n",
 /* errLOWSIZE     */ "\nError in matrix size (too small)\n",
 /* errHISIZE      */ "\nError in matrix size (too big)\n",
 /* errPREC	       */ "\nInvalid precision - defaulting to 2\n",
 /* errBADNUM      */ "\nError - invalid integer\n",
                  };

 fs(errs[eflag]);

 if(eflag == errPREC)
   P = default_precision;
 else
   exit(eflag);
}

