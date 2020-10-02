/*	$Id: g4sencod.c 1.2 90/06/09 18:24:10 marking Exp $
 *
 NAME
 *	g4sencod.c -- encode group 4 data using nested if statements
 *
 TYPE
 *	C procedures
 *
 SYNOPSIS
 *	char	g4j_encode_black (short runlength);
 *	char	g4j_encode_white (short runlength);
 *	char	g4j_encode_EOFB (void);
 *	char	g4j_encode_new_row (void);
 *	char	g4j_initialize (short image_width, short image_length);
 *
 DESCRIPTION
 *	Routines to encode group 4 images. Consecutive calls to encode
 *	black or white runs will be combined.
 *
 RETURNS
 *
 LEGAL
 *	Copyright 1989, 1990 Michael P. Marking, Post Office Box 8039,
 *	Scottsdale, Arizona 85252-8039. All rights reserved.
 *
 *	License is granted by the copyright holder to distribute and use this
 *	code without payment of royalties or the necessity of notification as
 *	long as this notice (all the text under "LEGAL") is included.
 *
 *	Reference: $Id: g4sencod.c 1.2 90/06/09 18:24:10 marking Exp $
 *
 *	This program is offered without any warranty of any kind. It includes
 *	no warranty of merchantability or fitness for any purpose. Testing and
 *	suitability for any use are the sole responsibility of the user.
 * 
 HISTORY
 *	$Log:	g4sencod.c $
 * Revision 1.2  90/06/09  18:24:10  marking
 * clean up comments for release
 * 
 * Revision 1.1  90/05/01  02:00:00  marking
 * Initial revision
 * 
 *
 NOTES
 *
 PORTABILITY
 *	Tested using Microsoft C 5.1. Some memory models may not work due to
 *	the large encoding arrays.
 *
 *	There is a non-portable use of "global" variables in the file g3g4.h,
 *	about which a minority of compilers will justifiably complain. Certain
 *	variables are declared in g3g4.h without extern keywords. Strictly
 *	speaking, they should be declared extern in all but one module, but
 *	that would require complication of g3g4.h. If it gets past your
 *	compiler and linker, you can probably ignore it.
 *
 SEE ALSO
 *
 INFORMATION
 *	Although there is no support offered with this program, the author will
 *	endeavor to correct errors. Updates will also be made available from
 *	time to time.
 *
 *	Contact: Michael P. Marking, Post Office Box 8039, Scottsdale, Arizona
 *	85252-8039 USA. Replies are not guaranteed to be swift. Beginning
 *	July 1990, e-mail may be sent to uunet!ipel!marking.
 *
 *	Also beginning in July 1990, this code will be archived at the
 *	ipel!phoenix BBS in file g3g4.zoo. The 24-hour telephone number
 *	for 300/1200/2400 is (602)274-0462. When logging in, specify user
 *	"public", system "bbs", and password "public".
 *
 *	This code is also available from the C Users Group in volume 317.
 */

#include "g3g4.h"

/* #define TRACE 1 */

/* implementation limits: Due to the sizes of arrays and variables, and not
   due to any restrictions in the algorithm, the following limits exist:
     maximum number of pixels per row: 65533
     maximum number of rows per image: none
     maximum or minimum k-factor: none
     maximum number of runs per row: 16382 white, 16382 black
   To increase (or decrease) these limits, it will be necessary to play with
   array and variable sizes.  On segmented machines (such as the 8086), a
   different memory model may be necessary.  The algorithm itself has no
   limits on image size or complexity, and the stack requirements are in-
   sensitive to changes in these limits or to image complexity. */

#define EVEN 0
#define ODD 1

static short a0, a1, a2, b0, b1, b2, length_of_current_run;
static unsigned char color, current_row, mode;
static short even_runs [32768], odd_runs [32768];
static unsigned short reference_index, coding_index;
static short *reference_line, *coding_line;
static short column_limit;
  /* Depending as current_row == EVEN or current_row == ODD, the runs of the
     current row are represented in even_runs [] or odd_runs [].  The white
     runs have even subscripts and the black runs have odd subscripts.  The
     values of the array elements are the offsets of the beginnings of the
     corresponding runs from the beginning of the row.  As defined by the
     specification,
	a0 is the reference or starting changing element on the coding line.
		It may be considered the "current position".
	a1 is the next changing element to the right of a0 on the coding line.
	a2 is the next changing element to the right of a1 on the coding line.
	b1 is the first changing element on the reference line to the right of
		a0 and of opposite color to a0.
	b2 is the next changing element to the right of b1 on the reference
		line.
     Furthermore,
	b0 is the "previous" value of b1. 
     */

char g4j_initialize (short image_width, short image_length)
{
  initialize_encode ();
  color = WHITE;
  current_row = ODD;
  coding_line = odd_runs;
  reference_line = even_runs;
  coding_index = 0;
  coding_line [0] = 0;
  length_of_current_run = 0;
  column_limit = image_width;
  reference_line [0] = 0;
  reference_line [1] = column_limit;
  reference_line [2] = column_limit;
  reference_line [3] = column_limit;
  return (0);
}

char g4j_encode_EOFB ()
{
  encode_word (EOL_code, EOL_length);
  encode_word (EOL_code, EOL_length);
  return (0);
}

char g4j_encode_new_row ()
{
  short last_point;
  last_point  = coding_line [coding_index] + length_of_current_run;
  coding_index++;
  coding_line [coding_index] = last_point;
  coding_line [coding_index + 1] = last_point;
  coding_line [coding_index + 2] = last_point;
  coding_line [coding_index + 3] = last_point;
  /* encode the current row */
  #if defined (TRACE)
    if (trace_flag)
    {
      short j = 0, a;
      printf (" coding_line = < ");
      while ((a = coding_line [j]) < column_limit)
      {
        printf ("%hd@%hd ", (short) (coding_line [j + 1] - a), a);
        j++;
      }
      printf ("> ");
      j = 0;
      printf ("\nreference_line = < ");
      while ((a = reference_line [j]) < column_limit)
      {
        printf ("%hd@%hd ", (short) (reference_line [j + 1] - a), a);
        j++;
      }
      printf ("> ");
    };
  #endif
  reference_index = 0;
  coding_index = 0;
  a1 = coding_line [0];
  a2 = coding_line [1];
  if (a2 == 0 && reference_line [1] == 0)
    /* if the first (white) run is null as was in previous row */
  {
    encode_word (VERTICAL_V0_CODEWORD, VERTICAL_V0_CODELEN);
    coding_index++;
    a1 = coding_line [1];
    a2 = coding_line [2];
    #if defined (TRACE)
      if (trace_flag) printf ("V0 ");
    #endif
  }
  while (a1 < column_limit)
  {
    short effective_a0;
    a0 = a1;
    a1 = a2;
    a2 = coding_line [coding_index + 2];
    if (reference_index > 3) reference_index -= 3;
    else reference_index = 0;
    if (coding_index) effective_a0 = a0;
    else effective_a0 = -1;
    while (effective_a0 >= (b1 = reference_line [reference_index]))
      reference_index++;
      /* now the reference index points to the first changing point beyond a0,
	 but it may or may not be the same color as a0 */
    if (! ((coding_index ^ reference_index) & 1)) /* if same colors */
    {
      reference_index++; /* now different color */
      b1 = reference_line [reference_index];
    }
    b2 = reference_line [reference_index + 1];
    #if defined (TRACE)
      if (trace_flag)
        printf ("\na0=%hd a1=%hd a2=%hd b1=%hd b2=%hd rx=%hd cx=%hd ",
	  a0, a1, a2, b1, b2, reference_index, coding_index);
    #endif
    while (b2 < a1) /* PASS */
    {
      #if defined (TRACE)
	short old_a0 = a0;
      #endif
      encode_word (PASS_CODEWORD, PASS_CODELEN);
      a0 = b2;
      reference_index += 2;
      b1 = reference_line [reference_index];
      b2 = reference_line [reference_index + 1];
      #if defined (TRACE)
        if (trace_flag) printf ("PASS k%hd ", (short) (a0 - old_a0));
      #endif
    }
    if (a1 == b1) /* V0 */
    {
      encode_word (VERTICAL_V0_CODEWORD, VERTICAL_V0_CODELEN);
      coding_index++;
      #if defined (TRACE)
        if (trace_flag) printf ("V0 ");
      #endif
    }
    else if (a1 == b1 + 1) /* VR1 */
    {
      encode_word (VERTICAL_VR1_CODEWORD, VERTICAL_VR1_CODELEN);
      coding_index++;
      #if defined (TRACE)
        if (trace_flag) printf ("VR1 ");
      #endif
    }
    else if (a1 == b1 + 2) /* VR2 */
    {
      encode_word (VERTICAL_VR2_CODEWORD, VERTICAL_VR2_CODELEN);
      coding_index++;
      #if defined (TRACE)
        if (trace_flag) printf ("VR2 ");
      #endif
    }
    else if (a1 == b1 + 3) /* VR3 */
    {
      encode_word (VERTICAL_VR3_CODEWORD, VERTICAL_VR3_CODELEN);
      coding_index++;
      #if defined (TRACE)
        if (trace_flag) printf ("VR3 ");
      #endif
    }
    else if (a1 == b1 - 1) /* VL1 */
    {
      encode_word (VERTICAL_VL1_CODEWORD, VERTICAL_VL1_CODELEN);
      coding_index++;
      #if defined (TRACE)
        if (trace_flag) printf ("VL1 ");
      #endif
    }
    else if (a1 == b1 - 2) /* VL2 */
    {
      encode_word (VERTICAL_VL2_CODEWORD, VERTICAL_VL2_CODELEN);
      coding_index++;
      #if defined (TRACE)
        if (trace_flag) printf ("VL2 ");
      #endif
    }
    else if (a1 == b1 - 3) /* VL3 */
    {
      encode_word (VERTICAL_VL3_CODEWORD, VERTICAL_VL3_CODELEN);
      coding_index++;
      #if defined (TRACE)
        if (trace_flag) printf ("VL3 ");
      #endif
    }
    else /* HORIZONTAL */
    {
      short first_run, second_run;
      encode_word (HORIZONTAL_CODEWORD, HORIZONTAL_CODELEN);
      first_run = a1 - a0;
      second_run = a2 - a1;
      if (coding_index & 1) /* if BLACK, WHITE */
      {
	g3j_encode_black (first_run);
	g3j_encode_white (second_run);
	#if defined (TRACE)
	  if (trace_flag) printf ("B%hd W%hd ", first_run, second_run);
	#endif
      }
      else /* WHITE, BLACK */
      {
	g3j_encode_white (first_run);
	g3j_encode_black (second_run);
	#if defined (TRACE)
	  if (trace_flag) printf ("W%hd B%hd ", first_run, second_run);
	#endif
      }
      coding_index++;
      a0 = a1;
      a1 = a2;
      a2 = coding_line [coding_index + 2];
      coding_index++;
      #if defined (TRACE)
        if (trace_flag) printf ("HORIZ ");
      #endif
    }
  }
  /* now swap rows */
  if (current_row == EVEN)
  {
    current_row = ODD;
    coding_line = odd_runs;
    reference_line = even_runs;
  }
  else /* current_row == ODD */
  {
    current_row = EVEN;
    coding_line = even_runs;
    reference_line = odd_runs;
  }
  coding_index = 0;
  coding_line [coding_index] = 0;
  length_of_current_run = 0;
  return (0);
}

char g4j_encode_black (short runlength)
{
  if (!(coding_index & 1)) /* if even number, is white */
  {
    coding_index++; /* move on to black */
    coding_line [coding_index]
      = coding_line [coding_index - 1] + length_of_current_run;
    length_of_current_run = 0;
  }
  length_of_current_run += runlength;
  return (0);
}

char g4j_encode_white (short runlength)
{
  if (coding_index & 1) /* if odd number, is black */
  {
    coding_index++; /* move on to white */
    coding_line [coding_index]
      = coding_line [coding_index - 1] + length_of_current_run;
    length_of_current_run = 0;
  }
  length_of_current_run += runlength;
  return (0);
}

/*	end $RCSfile: g4sencod.c $ */
