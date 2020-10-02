/* Array Handling Package
(C) Copyright 1985,1987,1988 James P. Cruse - All Rights Reserved

a_testall.c  

A Part of the User-Supported Array Handling package. Please see
the file ARRAY.h for discussion concerning the registration and 
usage of the package.

Tests all array functions 

*/	

#include <stdio.h>

#include "array.h"


/* which do you want to test? (some compilers can't handle them all at once) */
#define	TEST_FILL	1
#define	TEST_INFO	1
#define	TEST_UNOP	1
#define	TEST_BINOP	1
#define	TEST_MOD	1 		/* true if a legal % type */
#define	ARRAY_TYPE	int   	 		/* what type of array */
#define	PRV(X)		printf("\t%d",(X))	/* print Value x */

#define	NUM		10  		/* how many elements */
					/* NUM*NUM < maxint */


a_error(msg,i)
char *msg;
ARRAY_TYPE i;
{
printf("\n\nERROR: '%s'\n\tIndex:",msg);
PRV(i);
puts("\n\nAborting\n");
exit(-1);
}

ARRAY_TYPE cnt = 0;

reset_zero_param()
{
return(cnt = 0);
}
ARRAY_TYPE zero_param()
{
return(cnt++);
}
ARRAY_TYPE one_param(y)
ARRAY_TYPE y;
{
ARRAY_TYPE j,k;
j = y;
k = NUM;
j=k;
y=j;
j=NUM;
y=j;
return(y);
}
ARRAY_TYPE two_param(x,y)
ARRAY_TYPE x,y;
{
return(x*2-y+2);
}

/* index needed */
ARRAY_TYPE i,j,k;
/* arrays needed */
ARRAY_TYPE i1[NUM],i2[NUM],i3[NUM];

main()
{

ARR_IF1
{		/* if this curly brace causes an error, try defining */	
		/* ARR_IF_NEEDED to 1 */
i = NUM;		/* do something */
} ARR_IF2


#if TEST_FILL
	test_fill();
#endif
#if TEST_INFO
	test_info();
#endif
#if TEST_UNOP
	test_unop();
#endif
#if TEST_BINOP
	test_binop();
#endif

puts("\n\n\n\nAll Tests Passed\n\n\n");


exit(0);
}


#if TEST_FILL


test_fill()
{


/* first test fill operations, needed for other tests */
puts("\nTesting Fill Operations:");

a_index(NUM,i1);	/* fill with index */
for ( i=0; i<NUM; ++i )
	if ( !( i1[i] == (ARRAY_TYPE)  i ) )
		a_error("index Not Work:",i);

putchar('1');

a_indoff(NUM,i2,1);	/* fill with index + 1 */
for ( i=0; i < NUM; ++i )
	if ( ! ( i2[i] == (ARRAY_TYPE)  (i+1) ) )
		a_error("indoff Not Work:",i);

putchar('2');

a_assign(NUM,i3,5);	/* fill with 5 */
for ( i=0; i<NUM; ++i )
	if ( ! ( i3[i] == (ARRAY_TYPE)  5 ) )
		a_error("indassign Not Work:",i);

putchar('3');

a_i_scale(NUM,i1,5,4);	/* fill with i*5+4 */
for ( i=0; i<NUM; ++i )
	if ( ! ( i1[i] == (ARRAY_TYPE)  (i*5+4) ) )
		a_error("a_i_scale Not Work:",i);

putchar('4');

a_fill(NUM,i2,-5,5);
for ( i=0; i<NUM; ++i )
	if ( ! ( i2[i] == (ARRAY_TYPE)  (-5+(i* 10)/NUM) ) )
		a_error("a_fill Not Work:",i);
if ( ! ( i2[0] == (ARRAY_TYPE)  -5 ) )
	a_error("a_fill Not Work:",0);


putchar('5');

a_t_fill(NUM,i3,-6,-1,ARRAY_TYPE);
for ( i=0; i<NUM; ++i )
	if ( ! ( i3[i] == (k= (ARRAY_TYPE)  (-6+((ARRAY_TYPE)i* 5 )/NUM)) ) ) {
		PRV(i);
		PRV(i3[i]);
		PRV(((ARRAY_TYPE)  (-6+((ARRAY_TYPE)i* 5 )/NUM)) );
		a_error("a_t_fill Not Work:",i);
	}
if ( ! ( i3[0] == (ARRAY_TYPE)  -6 ) )
	a_error("a_t_fill Not Work:",0);

putchar('6');

a_ifill(NUM,i1,-3,15);
for ( i=0; i<NUM; ++i )
	if ( ! ( i1[i] == (ARRAY_TYPE)  (-3+(i* 18)/(NUM-1)) ) )
		a_error("a_ifill Not Work:",i);
if ( ! ( i1[0] == (ARRAY_TYPE)  -3 ) )
	a_error("a_ifill Not Work:",0);
if ( ! ( i1[NUM-1] == (ARRAY_TYPE)  15 ) )
	a_error("a_ifill Not Work:",1);


putchar('7');

a_t_ifill(NUM,i2,5,-13,ARRAY_TYPE);
for ( i=0; i<NUM; ++i )
	if ( ! ( i2[i] == (ARRAY_TYPE)  (5 +((ARRAY_TYPE)i* -18)/(NUM-1)) ) )
		a_error("a_t_ifill Not Work:",i);
if ( ! ( i2[0] == (ARRAY_TYPE)  5  ) )
	a_error("a_t_ifill Not Work:",0);
if ( ! ( i2[NUM-1] == (ARRAY_TYPE)  -13 ) )
	a_error("a_t_ifill Not Work:",0);

putchar('8');

reset_zero_param();
a_f_fun(NUM,i3,zero_param);
reset_zero_param();
for ( i=NUM; i--; )
	if ( ! ( i3[i] == (ARRAY_TYPE)  zero_param() ) )
		a_error("a_f_fun Not Work:",i);

putchar('9');


a_i_fun(NUM,i1,one_param);
for ( i=0; i<NUM; ++i )
	if ( ! ( i1[i] == one_param(i) ) )
		a_error("a_i_fun Not Work:",i);


putchar('A');

a_t_i_fun(NUM,i2,one_param,ARRAY_TYPE);
for ( i=0; i<NUM; ++i )
	if ( ! ( i2[i] == (ARRAY_TYPE)  one_param((ARRAY_TYPE)i) ) )
		a_error("a_t_i_fun Not Work:",i);


/* successful */
puts(".\tSuccessful");

return(0);
}


#endif
#if TEST_INFO


test_info()
{

/* now set up arrays for info testing */
a_fill(NUM,i1,-10,10);
a_indoff(NUM,i2,- NUM);
a_i_scale(NUM,i3,2,-25);

/* start info */
puts("\nTesting Info");




a_sum(NUM,j,i1);
k = i1[0]; for ( i=1; i<NUM; ++i ) k += i1[i];
if ( ! ( j == (ARRAY_TYPE)  k ) )
	a_error("Sum Not Work:",j-k);

putchar('1');

a_prod(NUM,j,i2);
k = i2[0]; for ( i=1; i<NUM; ++i ) k *= i2[i];
if ( ! ( j == (ARRAY_TYPE)  k ) )
	a_error("Prod Not Work:",j-k);

putchar('2');

a_max(NUM,j,i3);
if ( ! ( j == (ARRAY_TYPE)  (2*(NUM-1) + -25) ) )
	a_error("Max Not Work:",j-k);

putchar('3');


a_min(NUM,j,i1);
if ( ! ( j == (ARRAY_TYPE)  -10 ) )
	a_error("Min Not Work:",j-k);

putchar('4');

a_minmax(NUM,j,k,i2)
if ( ! ( ( j == (ARRAY_TYPE)  (-NUM) ) && ( k == (ARRAY_TYPE)  -1) ) )
	a_error("MinMax Not Work:",i2[NUM-1]);

/* successful */
puts(".\tSuccessful");
return(0);
}

#endif
#if TEST_UNOP

test_unop()
{

/* now set up arrays for unop testing */
a_ifill(NUM,i1,-NUM , NUM ); 
a_index(NUM,i2);
a_indoff(NUM,i3,-1);


puts("\nStarting Unop");

a_cpy(NUM,i2,i1);
for ( i=0; i<NUM; ++i )
	if ( ! ( i1[i] == (ARRAY_TYPE)  i2[i] ) )
		a_error("a_cpy Not Work:",i);

putchar('1');

a_neg(NUM,i2,i3);
for ( i=0; i<NUM; ++i )
	if ( ! ( i2[i] == (ARRAY_TYPE)  - i3[i] ) )
		a_error("a_neg Not Work:",i);

putchar('2');

a_abs(NUM,i3,i1);
for ( i=0; i<NUM; ++i )
	if ( ! (i3[i] == (ARRAY_TYPE)  ( (i1[i] < 0) ? (- i1[i]) : i1[i] ) ) )
		a_error("a_abs Not Work:",i);

putchar('3');

a_scale(NUM,i2,i1,10,5);
for ( i=0; i<NUM; ++i )
	if ( ! (i2[i] == (ARRAY_TYPE)  (i1[i]*10 + 5) ) )
		a_error("a_scale Not Work:",i);

putchar('4');

a_rsum(NUM,i3,i2);
for ( k=i=0; i<NUM; ++i ) {
	k += i2[i];
	if ( ! (i3[i] == (ARRAY_TYPE)  k) )
		a_error("a_rsum Not Work:",i);
}
putchar('5');

a_rprod(NUM,i1,i3);
k=1;
for ( i=0; i<NUM; ++i ) {
	k *= i3[i];
	if ( ! (i1[i] == (ARRAY_TYPE)  k) )
		a_error("a_rprod Not Work:",i);
}
putchar('6');

a_rmin(NUM,i1,i2);
k=i2[0];
for ( i=1; i<NUM; ++i ) {
	k = ((k<i2[i])?k:i2[i]);
	if ( ! (i1[i] == (ARRAY_TYPE)  k) )
		a_error("a_rmin Not Work:",i);
}
putchar('7');

a_rmax(NUM,i3,i2);
k=i2[0];
for ( i=1; i<NUM; ++i ) {
	k = ((k>i2[i])?k:i2[i]);
	if ( ! (i3[i] == (ARRAY_TYPE)  k) )
		a_error("a_rmax Not Work:",i);
}
putchar('8');


a_fun(NUM,i3,i2,one_param);
for ( i=0; i<NUM; ++i ) {
	if ( ! (i3[i] == (ARRAY_TYPE)  one_param(i2[i])) )
		a_error("a_fun Not Work:",i);
}
putchar('9');

a_t_fun(NUM,i2,i1,one_param,ARRAY_TYPE);
for ( i=0; i<NUM; ++i ) {
	if ( ! (i2[i] == (ARRAY_TYPE)  one_param((ARRAY_TYPE)i1[i])) )
		a_error("a_t_fun Not Work:",i);
}
putchar('A');

/* successful */
puts(".\tSuccessful");

return(0);
}

#endif
#if TEST_BINOP


test_binop()
{
/* now set up arrays for unop testing */
a_ifill(NUM,i1, NUM ,-NUM ); 
a_indoff(NUM,i2,-NUM/2);
a_i_scale(NUM,i3,-1,-5);


puts("\nStarting Binop");

test_a_binop();
test_b_binop();
test_c_binop();
test_d_binop();

/* successful */
puts(".\tSuccessful");

return(0);
}


test_a_binop()
{

aa_add(NUM,i1,i2,i3);
for ( i=0; i<NUM; ++i ) {
	if ( ! (i1[i] == (ARRAY_TYPE)  (i2[i]+i3[i])) )
		a_error("aa_add Not Work:",i);
}
ac_add(NUM,i1,i2,5 );
for ( i=0; i<NUM; ++i ) {
	if ( ! (i1[i] == (ARRAY_TYPE)  (i2[i] + 5 )) )
		a_error("ac_add Not Work:",i);
}
ca_add(NUM,i1,7 ,i3);
for ( i=0; i<NUM; ++i ) {
	if ( ! (i1[i] == (ARRAY_TYPE)  (7 + i3[i])) )
		a_error("ca_add Not Work:",i);
}
putchar('0');

aa_mul(NUM,i1,i2,i3);
for ( i=0; i<NUM; ++i ) {
	if ( ! (i1[i] == (ARRAY_TYPE)  (i2[i] * i3[i])) )
		a_error("aa_mul Not Work:",i);
}
ac_mul(NUM,i1,i2,5 );
for ( i=0; i<NUM; ++i ) {
	if ( ! (i1[i] == (ARRAY_TYPE)  (i2[i] * 5 )) )
		a_error("ac_mul Not Work:",i);
}
ca_mul(NUM,i1,7 ,i3);
for ( i=0; i<NUM; ++i ) {
	if ( ! (i1[i] == (ARRAY_TYPE)  (7 * i3[i])) )
		a_error("ca_mul Not Work:",i);
}
putchar('1');


aa_sub(NUM,i1,i2,i3);
for ( i=0; i<NUM; ++i ) {
	if ( ! (i1[i] == (ARRAY_TYPE)  (i2[i] - i3[i])) )
		a_error("aa_sub Not Work:",i);
}
ac_sub(NUM,i1,i2,5 );
for ( i=0; i<NUM; ++i ) {
	if ( ! (i1[i] == (ARRAY_TYPE)  (i2[i] - 5 )) )
		a_error("ac_sub Not Work:",i);
}
ca_sub(NUM,i1,7 ,i3);
for ( i=0; i<NUM; ++i ) {
	if ( ! (i1[i] == (ARRAY_TYPE)  (7 - i3[i])) )
		a_error("ca_sub Not Work:",i);
}
putchar('2');

a_assign(NUM,i3,2);
aa_div(NUM,i1,i2,i3);
for ( i=0; i<NUM; ++i ) {
	if ( ! (i1[i] == ((ARRAY_TYPE)  (i2[i] / i3[i]))) )
		a_error("aa_div Not Work:",i);
}
ac_div(NUM,i1,i2,4 );
for ( i=0; i<NUM; ++i ) {
	if ( ! (i1[i] == (ARRAY_TYPE)  (i2[i] / 4 )) )
		a_error("ac_div Not Work:",i);
}
ca_div(NUM,i1,-2,i3);
for ( i=0; i<NUM; ++i ) {
	if ( ! (i1[i] == (ARRAY_TYPE)  (-2/ i3[i])) )
		a_error("ca_div Not Work:",i);
}
putchar('3');

#if	TEST_MOD

aa_mod(NUM,i1,i2,i3);
for ( i=0; i<NUM; ++i ) {
	if ( ! (i1[i] == (ARRAY_TYPE)  (i2[i] % i3[i])) )
		a_error("aa_mod Not Work:",i);
}
ac_mod(NUM,i1,i2,5 );
for ( i=0; i<NUM; ++i ) {
	if ( ! (i1[i] == (ARRAY_TYPE)  (i2[i] % 5 )) )
		a_error("ac_mod Not Work:",i);
}
ca_mod(NUM,i1,7 ,i3);
for ( i=0; i<NUM; ++i ) {
	if ( ! (i1[i] == (ARRAY_TYPE)  (7 % i3[i])) )
		a_error("ca_mod Not Work:",i);
}
putchar('4');

#endif


aa_equ(NUM,i1,i2,i3);
for ( i=0; i<NUM; ++i ) {
	if ( ! (i1[i] == (ARRAY_TYPE)  (i2[i] == (ARRAY_TYPE)  i3[i])) )
		a_error("aa_equ Not Work:",i);
}
ac_equ(NUM,i1,i2,5 );
for ( i=0; i<NUM; ++i ) {
	if ( ! (i1[i] == (ARRAY_TYPE)  (i2[i] == (ARRAY_TYPE)  5 )) )
		a_error("ac_equ Not Work:",i);
}
ca_equ(NUM,i1,7 ,i3);
for ( i=0; i<NUM; ++i ) {
	if ( ! (i1[i] == (ARRAY_TYPE)  (7 == (ARRAY_TYPE)  i3[i])) )
		a_error("ca_equ Not Work:",i);
}
putchar('5');

aa_geq(NUM,i1,i2,i3);
for ( i=0; i<NUM; ++i ) {
	if ( ! (i1[i] == (ARRAY_TYPE)  (i2[i] >= i3[i])) )
		a_error("aa_geq Not Work:",i);
}
ac_geq(NUM,i1,i2,5 );
for ( i=0; i<NUM; ++i ) {
	if ( ! (i1[i] == (ARRAY_TYPE)  (i2[i] >= 5 )) )
		a_error("ac_geq Not Work:",i);
}
ca_geq(NUM,i1,7 ,i3);
for ( i=0; i<NUM; ++i ) {
	if ( ! (i1[i] == (ARRAY_TYPE)  (7 >= i3[i])) )
		a_error("ca_geq Not Work:",i);
}
putchar('6');

return(0);
}

test_b_binop()
{


aa_gtr(NUM,i1,i2,i3);
for ( i=0; i<NUM; ++i ) {
	if ( ! (i1[i] == (ARRAY_TYPE)  (i2[i] > i3[i])) )
		a_error("aa_gtr Not Work:",i);
}
ac_gtr(NUM,i1,i2,5 );
for ( i=0; i<NUM; ++i ) {
	if ( ! (i1[i] == (ARRAY_TYPE)  (i2[i] > 5 )) )
		a_error("ac_gtr Not Work:",i);
}
ca_gtr(NUM,i1,7 ,i3);
for ( i=0; i<NUM; ++i ) {
	if ( ! (i1[i] == (ARRAY_TYPE)  (7 > i3[i])) )
		a_error("ca_gtr Not Work:",i);
}
putchar('7');


aa_leq(NUM,i1,i2,i3);
for ( i=0; i<NUM; ++i ) {
	if ( ! (i1[i] == (ARRAY_TYPE)  (i2[i] <= i3[i])) )
		a_error("aa_leq Not Work:",i);
}
ac_leq(NUM,i1,i2,5 );
for ( i=0; i<NUM; ++i ) {
	if ( ! (i1[i] == (ARRAY_TYPE)  (i2[i] <= 5 )) )
		a_error("ac_leq Not Work:",i);
}
ca_leq(NUM,i1,7 ,i3);
for ( i=0; i<NUM; ++i ) {
	if ( ! (i1[i] == (ARRAY_TYPE)  (7 <= i3[i])) )
		a_error("ca_leq Not Work:",i);
}
putchar('8');

aa_les(NUM,i1,i2,i3);
for ( i=0; i<NUM; ++i ) {
	if ( ! (i1[i] == (ARRAY_TYPE)  (i2[i] < i3[i])) )
		a_error("aa_les Not Work:",i);
}
ac_les(NUM,i1,i2,5 );
for ( i=0; i<NUM; ++i ) {
	if ( ! (i1[i] == (ARRAY_TYPE)  (i2[i] < 5 )) )
		a_error("ac_les Not Work:",i);
}
ca_les(NUM,i1,7 ,i3);
for ( i=0; i<NUM; ++i ) {
	if ( ! (i1[i] == (ARRAY_TYPE)  (7 < i3[i])) )
		a_error("ca_les Not Work:",i);
}
putchar('9');


return(0);
}

test_c_binop()
{
aa_min(NUM,i1,i2,i3);
for ( i=0; i<NUM; ++i ) {
	if ( ! (i1[i] == (ARRAY_TYPE)  ((i2[i] < i3[i])?i2[i]:i3[i])) )
		a_error("aa_min Not Work:",i);
}
ac_min(NUM,i1,i2,5 );
for ( i=0; i<NUM; ++i ) {
	if ( ! (i1[i] == (ARRAY_TYPE)  ((i2[i] < 5 )?i2[i]:5)) )
		a_error("ac_min Not Work:",i);
}
ca_min(NUM,i1,7 ,i3);
for ( i=0; i<NUM; ++i ) {
	if ( ! (i1[i] == (ARRAY_TYPE)  ((7 < i3[i])?7:i3[i])) )
		a_error("ca_min Not Work:",i);
}
putchar('A');


aa_max(NUM,i1,i2,i3);
for ( i=0; i<NUM; ++i ) {
	if ( ! (i1[i] == (ARRAY_TYPE)  ((i2[i] > i3[i])?i2[i]:i3[i])) )
		a_error("aa_max Not Work:",i);
}
ac_max(NUM,i1,i2,5 );
for ( i=0; i<NUM; ++i ) {
	if ( ! (i1[i] == (ARRAY_TYPE)  ((i2[i] > 5 )?i2[i]:5)) )
		a_error("ac_max Not Work:",i);
}
ca_max(NUM,i1,7 ,i3);
for ( i=0; i<NUM; ++i ) {
	if ( ! (i1[i] == (ARRAY_TYPE)  ((7 > i3[i])?7:i3[i])) )
		a_error("ca_max Not Work:",i);
}
putchar('B');


return(0);
}

test_d_binop()
{


aa_fun(NUM,i1,i2,i3,two_param);
for ( i=0; i<NUM; ++i ) {
	if ( ! (i1[i] == (ARRAY_TYPE)  two_param(i2[i] , i3[i])) )
		a_error("aa_fun Not Work:",i);
}
ac_fun(NUM,i1,i2,5,two_param );
for ( i=0; i<NUM; ++i ) {
	if ( ! (i1[i] == (ARRAY_TYPE)  two_param(i2[i],5)) )
		a_error("ac_fun Not Work:",i);
}
ca_fun(NUM,i1,7 ,i3,two_param);
for ( i=0; i<NUM; ++i ) {
	if ( ! (i1[i] == (ARRAY_TYPE)  two_param(7 , i3[i])) )
		a_error("ca_fun Not Work:",i);
}
putchar('C');

aa_t_fun(NUM,i1,i2,i3,two_param,ARRAY_TYPE);
for ( i=0; i<NUM; ++i ) {
	if ( ! (i1[i] == (ARRAY_TYPE)  two_param((ARRAY_TYPE)i2[i] , (ARRAY_TYPE)i3[i])) )
		a_error("aa_t_fun Not Work:",i);
}
ac_t_fun(NUM,i1,i2,5,two_param,ARRAY_TYPE );
for ( i=0; i<NUM; ++i ) {
	if ( ! (i1[i] == (ARRAY_TYPE)  two_param((ARRAY_TYPE)i2[i],(ARRAY_TYPE)5)) )
		a_error("ac_t_fun Not Work:",i);
}
ca_t_fun(NUM,i1,7 ,i3,two_param,ARRAY_TYPE);
for ( i=0; i<NUM; ++i ) {
	if ( ! (i1[i] == (ARRAY_TYPE)  two_param((ARRAY_TYPE)7 , (ARRAY_TYPE)i3[i])) )
		a_error("ca_t_fun Not Work:",i);
}
putchar('D');
return(0);
}

#endif

/* end */
