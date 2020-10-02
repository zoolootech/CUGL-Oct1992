/****************************************************************************/
/* test.c                                                                   */
/* a driver for s_rand and d_rand                                           */
/* in order to determine whether or not the function d_rand() in file       */
/* random.c works on your hardware as compiled by your compiler do this:    */
/*  compile test.c and random.c                                             */
/*      link test.obj and random.obj to form test.exe                       */
/*      execute test.exe                                                    */
/* d_rand should work on 80X86 machines and other machines with             */
/* long int of at least 4 bytes                                             */
/* see the article referenced in rancom.c for details of implementations    */
/* for smaller machines                                                     */
/****************************************************************************/

#include<stdio.h>
#include <stdlib.h>
#include"random.h"


void get_seed(void);
void limits(double *,double *);

/****************************************************************************/
/* main                                                                     */
/****************************************************************************/
int main()
{
   double dummy;
   int i;

	printf("\nfor test of random number generator select a seed of 1\n");

   get_seed();

   limits(&u_limit,&l_limit);

	printf("now calculating 10,000 pseudorandom numbers\n");
	
   for(i=0;i<10000;i++){
      dummy = d_rand();
   }
   printf("\nvalue of seed after 10,000 iterations is: %ld",seed);
   printf("\nwith initial seed of 1, value should be:  1043618065\n");

	if(seed==1043618065)
		printf("\npseudorandom number generator working properly");
	else{
		printf("\npseudorandom number generator not working properly");
		printf("\nreview article in Communications of ACM for help");
	}
}

/****************************************************************************/
/* get_seed                                                                 */
/* this function asks for a number and uses it to seed rand()               */
/****************************************************************************/
void get_seed()
{
   char buff[10];
   long int s;

   printf("Enter seed for pseudorandom number generator.\nDefault = 1\nseed: ");
   s = atol(gets(buff));
   if(s<1) s = 1;
   s_seed(s);
   printf("seed = %ld",s);
}

/****************************************************************************/
/* limits                                                                   */
/* this function gets limits for d_random                                   */
/****************************************************************************/
void limits(
   double *upper,
   double *lower
)
{
   char buff[10];

   printf("\nEnter range for pseudorandom numbers:\n");
   printf("defaults:\n   upper limit = 1.0\n   lower limit = 0.0\n");
   printf("enter upper limit: ");
   *upper = atof(gets(buff));
   printf("enter lower limit: ");
   *lower = atof(gets(buff));
   if(*lower>*upper){
      printf("upper limit must be greater than lower limit; ");
      printf("default selected\n");
      *upper = 1.0;
      *lower = 0.0;
   }
   if(*lower==*upper){
      printf("default selected\n");
      *upper = 1.0;
      *lower = 0.0;
   }
   printf("upper limit = %4.2f\nlower limit = %4.2f\n",*upper,*lower);
}

