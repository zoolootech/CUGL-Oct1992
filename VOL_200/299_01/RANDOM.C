/****************************************************************************/
/* random.c                                                                 */
/*  (c) by Ronald Michaels. This program may be freely copied, modified,    */
/*  transmitted, or used for any non-commercial purpose.                    */
/* this is an implementation of the pseudorandom number generator as        */
/* described in:  Park, Stephen K., and Miller, Keith W. Random Number      */
/* Generators: Good Ones Are Hard to Find. Communications of the ACM,       */
/* Vol.31,No.10,(October 1988),1192-1201.                                   */
/****************************************************************************/
#include<stdio.h>  /* may be omitted if warning statements are omitted */

void s_limits(double,double);
void s_seed(long int);
double d_rand(void);

long int seed = 1;
double u_limit = 1.0;
double l_limit = 0.0;

/****************************************************************************/
/* s_seed                                                                   */
/* this function sets the initial seed for the random number generator      */
/* default value is 1                                                       */
/****************************************************************************/
void s_seed(long int seed_value)
{
	if(seed_value<1||seed_value>2147483646){
		/* warning statements may be omitted for smaller code */
		printf("\n         ********** WARNING ***********\n");      
		printf("seed for random number must be in the range\n"); 
		printf("1 - 2147483646\n");            
		printf("default value of seed = 1 has been chosen\n");
		printf("press any key to proceed:\n");
		seed = 1;                                   /* leave this statement */
		getch();
		return;
	}
	else{
	   seed = seed_value;
		return;
	}
}
/****************************************************************************/
/* s_limits                                                                 */
/* this function sets the upper and lower limits for                        */
/* the random number generator                                              */
/* default limits are 0.0 to 1.0                                            */
/****************************************************************************/
void s_limits(double upper,double lower)
{
	u_limit = upper;
	l_limit = lower;
}
/***************************************************************************/
/* d_rand                                                                  */
/* this function is the pseudorandom number generator                      */
/***************************************************************************/
double d_rand()
{
   long int a = 16807;          /* multiplier */
   long int m = 2147483647;     /* modulus */
   long int q = 127773;         /* m/a */
   long int r = 2836;           /* m%a */

   long int lo;
   long int hi;
   long int test;

   hi = seed/q;
   lo = seed%q;
   test = a*lo - r*hi;
   if(test>0) seed = test;
   else seed = test + m;
   return(((double)seed/(double)m)*(u_limit-l_limit) + l_limit);
}
