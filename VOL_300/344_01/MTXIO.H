/*-----------------------------------------------------------------------
   Main File : mtx.exe
   File Name : mtxio.h

   Purpose - Headers, defines for mtxio.c
-------------------------------------------------------------------------*/
  /* Output flags */
#define NO_SHOW             0
#define SHOW_MATRIX         1
#define SOLUTION_ONLY       2

  /* Public i/o operations */
void print_matrix(int);
void read_matrix(void);

  /* Timing macro */
#define print_time  {\
                     int m; float s;\
                     s=clock()/CLK_TCK;\
                     m= (int)(s / 60.0);\
                     s = s -(m*60);\
                     printf("\nTime: %d:%.2f\n",m,s);\
                     }


