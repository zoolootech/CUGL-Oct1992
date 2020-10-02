/*
 * TESTS FOR VARIOUS FORMS OF OPTIMIZATION 
 *
 * The following test helps determine what kinds of optimizations are
 * being done.  
 *
 * Unless specified otherwise, if the size of the first statement
 * is the same as the second statement,
 * even when the optimization flag is not set, then the partiucular
 * optimization is always done.  If the sizes are different 
 * (i.e., the first is larger) when run
 * without optimization but are the same when the optimization flag
 * is included then it is an optimization which is done during the
 * optimization pass.  If the sizes are different in both cases then
 * the optimization is not done.
 */

#include "timer1.h"

int i, j, k, l, temp;
unsigned int u1;
int arr[15];
int arr2[15][15];
register int *p;

/* Constant folding */
DO_STMT("Const folding #1" )
j = 42 * 6 / 3
OD
DO_STMT("Const folding #2" )
j = 84
OD

/* Constant folding plus propogation */
DO_STMT("Const fold + prop #1")
i = 3 + 14 / 7 + 6;
j = i + 7
OD
DO_STMT("Const fold + prop #2")
i = 11;
j = 18;
OD

/* Algebaric identities */

/* Adding 0 */
DO_STMT("Adding 0 #1")
i = j + 0
OD
DO_STMT("Adding 0 #2")
i = j
OD

/* Adding 1 to a value*/
/* If code is the same then no optimization for adding 1 */
DO_STMT("Adding 1 #1")
i = i + 1
OD
DO_STMT("Adding 1 #2")
i = i + 49
OD
/* Multiplying by 0 */
DO_STMT("Mult by 0 #1")
j = i * 0
OD
DO_STMT("Mult by 0 #2")
j = 0
OD

/* Multiplying by 1 */
DO_STMT("Mult by 1 #1")
j = i * 1
OD
DO_STMT("Mult by 1 #2")
j = i
OD

/* Multiplying by a power of 2 */
DO_STMT("Mult by power 2 #1")
i = j * 8
OD
DO_STMT("Mult by power 2 #2")
i = j << 3
OD

/* Dividing by a power of 2 */
DO_STMT("Div by power 2 #1")
u1 = u1 / 2
OD
DO_STMT("Div by power 2 #2")
u1 = u1 >> 1
OD

/* Cummutativity and rearrangement */
DO_STMT("Commute & rearr #1")
i = i + 1 + i + 2 + i + 3
OD
DO_STMT("Commute & rearr #2")
i = i + i + i + 6
OD

/* Special handling assignment of 0 */
/* If both statements are the same then no optimization */
DO_STMT("Assigning 0 #1")
i = 0;
OD
DO_STMT("Assigning 0 #2")
i = 49
OD

/* Testing against 0 in a conditional */
/* If both statements are the same then no optimization */
DO_STMT("Test against 0 #1")
if (i < 0)
	j = k;
OD
DO_STMT("Test against 0 #2")
if (i < 49)
	j = k;
OD

/* Post-incrementing by 1 */
/* If code is the same then no optimization for post-incrementing by 1 */
DO_STMT("Post-inc by 1 #1")
i++
OD
DO_STMT("Post-inc by 1 #2")
i = i + 49
OD

/* Pre-incrementing by 1 */ 
/* If code is the same then no optimization for pre-incrementing by 1 */
DO_STMT("Pre-inc by 1 #1")
++i
OD
DO_STMT("Pre-inc by 1 #2")
i = i + 49
OD

/* Post-incrementing by more than 1 */
/* If code is the same then no optimization for post-incrementing by > 1 */
DO_STMT("Post-inc by >1 #1")
p++
OD
DO_STMT("Post-inc by >1 #2")
j = j + 49 /*don't use p because I don't want  a scaled add */
OD

/* Pre-incrementing by  more than 1 */ 
/* If code is the same then no optimization for pre-incrementing by > 1 */
DO_STMT("Pre-inc by >1 #1")
++p
OD
DO_STMT("Pre-inc by >1 #2")
j = j + 49
OD


/* Recognizing a constant subscripts */
DO_STMT("Const subscript #1")
i = arr[10]
OD
j = 10;
DO_STMT("Const subscript #2")
i = arr[j]
OD

/* Jump to jump elimination */
/* The size difference on this test is unimportant. */
/* The first should be faster than */
/* the second one if the jump to jumps have been eliminated */
	
DO_STMT("Jmp to jmp elim #1")
if (j < 1)
     {
     if (k < 1)
	     l = 1;
     else
	     l = 2;
     }
else
     {
     if (k < 1)
	     l = 3;
     else
	     l = 4;
     }
k = 2;
i = 2
OD
DO_STMT("Jmp to jmp elim #2")
i = 7;
if (j < 1)
     {
     if (k < 1)
	     l = 1;
     else
	     l = 2;
     k = 2;
     }
else
     {
     if (k < 1)
	     l = 3;
     else
	     l = 4;
     i = 2;
     }
OD


/* Most of the following are primarily space optimizations */

/* Constant test elimination */
DO_STMT("Const test elim #1")
if (1)
	i = j * k
OD
DO_STMT("Const test elim #2")
	i = j * k
OD

/* Dead Code Elimination */ 
DO_STMT("Dead code elim #1")
	i = j * 7 / k;
	goto lab1;
	i = j * 3 / l;
lab1:
OD
DO_STMT("Dead code elim #2")
	i = j * 7 / k;
	goto lab2;
lab2:
OD

/* Common Subexpression Elimination */
DO_STMT("CSE elim #1")
i = j * k + 9;
k = l + j * k + 9
OD
DO_STMT("CSE elim #1")
i = j * k + 9; 
l = l + i
OD

/* Common Tail Elimination */
DO_STMT("Common tail elim #1")
if (i)
	{
	i = j * 5;
	k = 7 * l;
	}
else
	{
	i = j * 7;
	k = 7 * l;
	}
OD
DO_STMT("Common tail elim #2")
if (i)
	i = j * 5;
else
	i = j * 7;
k = 7 * l
OD


/* Code Hoisting */
DO_STMT("Code Hoisting #1")
if (i)
	{
	k = 7 * l;
	i = j * 5;
	}
else
	{
	k = 7 * l;
	i = j * 7;
	}
OD
DO_STMT("Code Hoisting #2")
k = 7 * l;
if (i)
	i = j * 5;
else
	i = j * 7;
OD

/* NOTES FOR OPTIMIZATION CHAPTER */
/* There are machine-dependent optimizations which depend on the */
/* the particular hardware and the instruction set */
/* For example, special addressing modes for combining operations */
/* such as the multiply and add of array indexing or */
/* indirection and increment are supported as a single */
/* addressing mode.  Are there others? */

/* Combining multiply and add */
/* I'm not to sure about this test */
DO_STMT("Combine mult-plus #1")
i = j * 2 + 7
OD
DO_STMT("Combine mult-plus #2")
i = j * 2
OD

/* Combining indirection and incrementing */
DO_STMT("Combine indr-plus #1")
for (p = arr, j = 0; j < 14; ++j)
	i = *p++;
OD
DO_STMT("Combine indr-plus #2")
for (p = arr, j = 0; j < 14; ++j)
	i = *p, p++;
OD

/* MORE TESTS ADDED BY PLUM 85/02 */

/* various  if (...) optimizations */
DO_STMT("if (i)")
if (i) ++j;
i = !i;
OD
DO_STMT("if (!i)")
if (!i) ++j;
i = !i;
OD
DO_STMT("if (i != 0)")
if (i != 0) ++j;
i = !i;
OD
DO_STMT("if (i == 0)")
if (i == 0) ++j;
i = !i;
OD

}
