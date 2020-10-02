/*----------------------------------------------------------------------
   Main File : mtx.c
   File Name : mtxcle.h

   Purpose - Public operations, defines for mtxcle.c
-----------------------------------------------------------------------*/

  /* Error flags */
#define errSINGULARITY      0
#define errLOWSIZE          1
#define errHISIZE           2
#define errPREC             3
#define errBADNUM           4



void get_args(int,char **);
void error(int);
void syntax(void);

