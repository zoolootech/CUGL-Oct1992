/* Some useful bit manipulation functions inspired by the article
 * "Binary Magic Numbers" by Edwin E. Freed in DDJ #78, April 1983.
 * by Dale Wilson, 1983  --- placed in the Public Domain
 *
 * These functions were written so thay may be directly translated
 *  into assembly language for most computers.
 *
 * These functions were tested on a Zenith 100 computer using the
 * C86 compiler from Computer Innovations, Inc.
 *
 * --------------------------------------------------------------------
 * This code was keyboarded into an IBM PC from the March'84 (#89) issue
 * of Dr. Dobb's Journal.  The code was tested then on the IBM PC using
 * the C86 compiler from Computer Innovations, Inc.  The output test
 * results checked out with the sample output in the article.
 * --------------------------------------------------------------------
 */
#include <stdio.h>

#define TRUE 1	/* true */
#define FALSE 0 /* false */
#define CNTLZ 26 /* ms-dos eof */

#define N 16   /* bits per "word" */
#define V 4    /* log 2 of N	  */

/* Since C does not have binary constants, the binary magic numbers are
 * expressed below in hexadecimal.  B from Freed's article is devided
 * into b1 and b2 since ther was no good reason to have them in the
 * same array.
 */

unsigned int b1[V] = { 0x5555, 0x3333, 0x0f0f, 0x00ff };
unsigned int b2[V] = { 0xaaaa, 0xcccc, 0xf0f0, 0xff00 };

/* Converting binary numbers to Gray code is so simple that it may
 * be best defined as a macro rather than a function.
 */

#define binary_to_Gray(x) (x ^ x >> 1)	 /* X exclusive or X shifted right 1 */

/* MAIN rutine to test the functions.
 * Numbers ( entered in hexadecimal ) will be used as aguments in each
 * of the functions.  As an additional check, the binary number resulting
 * from the Gray_to_binary function will be converted back to Gray code --
 * which shold result in the original number.
 */

main(argc,argv)   int argc; char *argv[];
{     unsigned int r,i;
      int c;
      while (TRUE)
      {
	 printf("Enter number : ");
	 fscanf(stdin,"%x",&i);         /* read a hexadecimal */
	 while((c=getchar()) != '\n')   /* discard end of line */
	      if(c == EOF || c == CNTLZ)  /* except on end of file */
		   exit();		/*   quit */
	 printf("low clear bit: %d\n", low_clear_bit(i));
	 printf("high set bit : %d\n", hi_set_bit(i));
	 printf("side sum     : %d\n", side_sum(i));
	 printf("parity       : %d\n", parity(i));
	 r = Gray_to_binary(i);
	 printf("Gray code    : 0x%04x\n", r);
	 printf("GTB to Binary: 0x%04x\n", binary_to_Gray(r));
	 printf("Reversed bits: 0x%04x\n", reverse_bits(i));
	 putchar('\n');                 /* leave a blank line between */
      }
}

/* This function returns the bit number of the lowest bit in the word
 * which is clear (zero).  The least significant bit is numbered 0, the
 * bit to the left of that, 1, and so on...
 * A minus 1 is returned for words in which all bits are on.
 * The time to execute this function is proportional to V which is
 * log2 of the number of bits in a word.
 * Note that the function low_set_bit may be created by complementing the
 * argument and calling low_clear_bit.
 */
low_clear_bit(value)   unsigned int value;
{   unsigned int temp;
    int i, result;
    if((value = ~value) == 0)	       /* complement, test for zero */
	 result = -1;		       /*  zero means no clear bits */
    else
    {	 result = 0;
	 for(i = V-1; i >= 0; i--)
	 {    result <<= 1;	       /* make space for next bit */
	      temp = value & b1[i];    /* test using magic numbers */
	      if(temp == 0)
		   result |= 1;        /* next bit of result is 1 */
	      else
		   value = temp;       /* discard disqualified bits */
	 }
    }
    return(result);
}

/* This function returns the bit number of the highest bit in the word
 * which is set (one).	The least significant bit is numbered 0, the
 * bit to the left of that, 1, and so on ...
 * A minus 1 is returned for words in which all bits are off.
 * The time to execute this function is proportinal to V which is
 * log2 of the number of bits in a word.
 * Note that the function high_clear_bit may be created by complementing the
 * argument and calling high_set_bit.
 */
hi_set_bit(value)   unsigned int value;
{   unsigned int temp;
    int result, i;
    if(value == 0)		       /* if no bits are on  */
	 result = -1;		       /*  return that info  */
    else
    {	 result = 0;
	 for(i = V-1; i >= 0; i--)
	 {    result <<= 1;	       /* space for next bit */
	      temp = value & b2[i];
	      if(temp != 0)
	      {    result |= 1;        /* next bit is one    */
		   value = temp;       /* discarded unneeded bits */
	      }
	 }
    }
    return(result);
}

/* This function returns a count of the number of bits which are on in a
 * word.  It executes in a time porportional to V, the log base 2 of the
 * number of bits in a word.
 * Note that a count of the number of zero bits in the word may be found
 * by complementing the value and calling side_sum.
 */
side_sum(value)    unsigned int value;
{   int i;
    unsigned int s;
    s = 1;
    for(i=0; i<V; i++)		  /* use magic in reverse order */
    {	 value = (value & b1[i]) + ((value >> s) & b1[i]);
	 s <<= 1;		  /* generate the powers of two on the fly */
    }
    return(value);
}

/* This function converts a number expressed in Gray code to the
 * equivalent binary number.  It executes in time proportional to the
 * log base 2 of the number of bits in the word.
 */
Gray_to_binary(value)	unsigned int value;
{   unsigned int s;
    for(s = N >> 1; s != 0; s >>= 1)
    {	 value ^= value >> s;
    }
    return(value);
}

/* This function returns the parity of a word--that is, it returns a zero
 * if the number of one bits in the word is even, and a one if the number
 * of one bits in the word is odd.  The low order bit of the results of
 * Gray_to_binary and side_sum both have this property, so isolating this
 * bit gives the desired result.  Gray_to_binary was selected since it is
 * a faster function that side_sum.
 */
parity(value)	   unsigned int value;
{
    return(Gray_to_binary(value) & 1);	 /* buld on previous work */
}

/* This function reverses the bits in a word.  Strangly enough, this turns
 * out to be a very useful function to have available.	Note that this function
 * works only on functions with word lengths which are powers of 2.
 */
reverse_bits(value)  unsigned int value;
{   unsigned int s,i;
    s = 1;		     /* s provides the powers of 2 */
    for (i=0; i<V; i++)
    {	 value = ((value << s) & b2[i]) | ((value >> s) & b1[i]);
	 s <<= 1;
    }
    return(value);
}
           