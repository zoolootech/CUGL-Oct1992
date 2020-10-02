/****************************************************************************/
/* random.h                                                                 */
/*  (c) by Ronald Michaels. This program may be freely copied, modified,    */
/*  transmitted, or used for any non-commercial purpose.                    */
/* this header file is required for random.c                                */
/****************************************************************************/

void s_limits(double,double);
void s_seed(long int);
double d_rand(void);

extern long int seed;
extern double u_limit,l_limit;
