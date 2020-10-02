/*	$Id: g4sdecod.c 1.2 90/06/09 18:23:27 marking Exp $
 *
 NAME
 *	g4sdecod.c -- decode group 4 data using nested if statements
 *
 TYPE
 *	C procedures
 *
 SYNOPSIS
 *	char	g4i_initialize (short image_width, short image_length);
 *	char	g4i_decode (void);
 *
 DESCRIPTION
 *	In order to acquire data from the image and to return run lengths and
 *	new line information, these routines invoke procedures provided by the
 *	caller. These caller-provided procedures are invoked throught pointers
 *	which have been stuffed by the caller with the procedure addresses.
 *	To acquire a new data byte, g4i_decode () calls (*p_g4i_next_byte) ().
 *	To report the decoding of a black or white run, the routines
 *	(*p_decode_black) () or (*p_decode_white) () are called.
 *
 RETURNS
 *	Initialization always returns zero.
 *
 *	For decoding,
 *		0	end of image reached
 *		-1	on error (bad data)
 *	The decode loop will be prematurely terminated if decode_return is
 *	set to not zero, and the value of decode_return will be returned.
 *	No code here does this, but it might be useful under certain
 *	circumstances.
 *
 LEGAL
 *	Copyright 1989, 1990 Michael P. Marking, Post Office Box 8039,
 *	Scottsdale, Arizona 85252-8039. All rights reserved.
 *
 *	License is granted by the copyright holder to distribute and use this
 *	code without payment of royalties or the necessity of notification as
 *	long as this notice (all the text under "LEGAL") is included.
 *
 *	Reference: $Id: g4sdecod.c 1.2 90/06/09 18:23:27 marking Exp $
 *
 *	This program is offered without any warranty of any kind. It includes
 *	no warranty of merchantability or fitness for any purpose. Testing and
 *	suitability for any use are the sole responsibility of the user.
 * 
 HISTORY
 *	$Log:	g4sdecod.c $
 * Revision 1.2  90/06/09  18:23:27  marking
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
 *	the large decoding arrays.
 *
 *	There is a non-portable use of "global" variables in the file g3g4.h,
 *	about which a minority of compilers will justifiably complain. Certain
 *	variables are declared in g3g4.h without extern keywords. Strictly
 *	speaking, they should be declared extern in all but one module, but
 *	that would require complication of g3g4.h. If it gets past your
 *	compiler and linker, you can probably ignore it.
 *
 SEE ALSO
 *	g3tdecod.c -- decode group 3 image using tables
 *	builddec.c -- build image decoding tables
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

static short a0, a1, a2, b0, b1, b2, bit_number, code_byte;
static unsigned char color, current_row, mode;
static unsigned short even_runs [32768], odd_runs [32768];
static unsigned short even_index, odd_index;
static short column_limit;
static short row_number = 0;
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
     Depending as current_row == EVEN or == ODD, even_index or odd_index is
     the subscript of the entry in even_runs [] or odd_runs [] corresponding
     to the run containing the current value of a0, and its counterpart cor-
     responds to the run containing b1. */

static void new_row (void);
static short decode_white_run (void);
static short decode_black_run (void);
static short decode_white_word (void);
static short decode_black_word (void);
static char next_bit (void);
static short null_mode (void);

static char decode_return;

/* g4i_decode () successively invokes (*p_decode_next_byte) () for each byte of the
   encoded image, and calls (*p_decode_white) () or (*p_decode_black) () as
   required to return the image contents on a run-by-run basis. */
char g4i_decode ()
{
  /* At the beginning of this routine, we are in the NULL mode, which is to
     to say that no codewords are currently understood or digested.  The
     variable bit_number has been initialized to zero, to indicate that the
     next (first) codeword is to begin with bit zero of the next code byte.
     This betrays an assumption that all images begin on byte boundaries, a
     condition that can be changed by arranging for bit_number to be set
     otherwise by g4i_initialize () or elsewhere. */
  while (!decode_return)
  {
    mode = (unsigned char) null_mode ();
    if (current_row == EVEN)
    {
      b0 = b1;
      b1 = odd_runs [odd_index];
      if ((b1 <= a0) && a0)
      {
	odd_index += 2;
	b0 = odd_runs [odd_index - 1];
	b1 = odd_runs [odd_index];
      }
      b2 = odd_runs [odd_index + 1];
    }
    else /* current_row == ODD */
    {
      b0 = b1;
      b1 = even_runs [even_index];
      if ((b1 <= a0) && a0)
      {
	even_index += 2;
	b0 = even_runs [even_index - 1];
	b1 = even_runs [even_index];
      }
      b2 = even_runs [even_index + 1];
    }
    #if defined (TRACE)
      if (trace_flag)
      {
	if (current_row == EVEN)
	{
	  printf ("\n ref_index=%hd b1=%hd b2=%hd a0=%hd curr[%hd]=%hd "
	    "color=%hd ",
	    odd_index, b1, b2, a0, even_index, even_runs [even_index],
	    (short) color);
	}
	else /* current_row == ODD */
	{
	  printf ("\n ref_index=%hd b1=%hd b2=%hd a0=%hd curr[%hd]=%hd "
	    "color=%hd ",
	    even_index, b1, b2, a0, odd_index, odd_runs [odd_index],
	    (short) color);
	}
      }
    #endif
    switch (mode)
    {
      case PASS_MODE: /* skip (pass) two color changes on the previous row */
	#if defined (TRACE)
	  if (trace_flag) printf (" P ");
	#endif
	if (color == WHITE) (*p_decode_white) ((short) (b2 - a0));
	else /* color == BLACK */ (*p_decode_black) ((short) (b2 - a0));
	a0 = b2;
	if (current_row == EVEN)
	{
	  odd_index += 2;
	  b1 = odd_runs [odd_index];
	}
	else /* current_row == ODD */
	{
	  even_index += 2;
	  b1 = even_runs [even_index];
	}
	break;
      case HORIZONTAL_MODE: /* revert to 1-dimensional modified Huffman
	encoding for a pair of runs */
	#if defined (TRACE)
	  if (trace_flag) printf (" H ");
	#endif
	if (color == WHITE)
	{
	  short black_runlength, white_runlength;
	  white_runlength = decode_white_run ();
	  (*p_decode_white) ((short) white_runlength);
	  a1 = (a0 += white_runlength);
	  black_runlength = decode_black_run ();
	  (*p_decode_black) ((short) black_runlength);
	  a2 = (a0 += black_runlength);
	}
	else /* color == BLACK */
	{
	  short black_runlength, white_runlength;
	  black_runlength = decode_black_run ();
	  (*p_decode_black) ((short) black_runlength);
	  a1 = (a0 += black_runlength);
	  white_runlength = decode_white_run ();
	  (*p_decode_white) ((short) white_runlength);
	  a2 = (a0 += white_runlength);
	}
	if (current_row == EVEN)
	{
	  even_runs [++even_index] = a1;
	  even_runs [++even_index] = a2;
	  while (a0 > odd_runs [odd_index]) odd_index += 2;
	  b1 = odd_runs [odd_index];
	}
	else /* current_row == ODD */
	{
	  odd_runs [++odd_index] = a1;
	  odd_runs [++odd_index] = a2;
	  while (a0 > even_runs [even_index]) even_index += 2;
	  b1 = even_runs [even_index];
	}
	break;
      case VERTICAL_V0_MODE: /* the next color change begins at the same
	location as in the previous row */
	#if defined (TRACE)
	  if (trace_flag) printf (" V0 ");
	#endif
	if (color == WHITE)
	{
	  (*p_decode_white) ((short) (b1 - a0));
	  color = BLACK;
	}
	else /* color == BLACK */
	{
	  (*p_decode_black) ((short) (b1 - a0));
	  color = WHITE;
	}
	a0 = b1; 
	if (current_row == EVEN)
	{
	  even_runs [++even_index] = a0;
	  odd_index++;
	}
	else /* current_row == ODD */
	{
	  odd_runs [++odd_index] = a0;
	  even_index++;
	}
	break;
      case VERTICAL_VR1_MODE: /* the next color change begins one pixel to the
	right of its location on the previous row */
	#if defined (TRACE)
	  if (trace_flag) printf (" VR1 ");
	#endif
	if (color == WHITE)
	{
	  (*p_decode_white) ((short) (b1 - a0 + 1));
	  color = BLACK;
	}
	else /* color == BLACK */
	{
	  (*p_decode_black) ((short) (b1 - a0 + 1));
	  color = WHITE;
	}
	a0 = b1 + 1; 
	if (current_row == EVEN)
	{
	  even_runs [++even_index] = a0;
	  odd_index++;
	}
	else /* current_row == ODD */
	{
	  odd_runs [++odd_index] = a0;
	  even_index++;
	}
	break;
      case VERTICAL_VR2_MODE: /* the next color change begins two pixels to
	the right of its location on the previous row */
	#if defined (TRACE)
	  if (trace_flag) printf (" VR2 ");
	#endif
	if (color == WHITE)
	{
	  (*p_decode_white) ((short) (b1 - a0 + 2));
	  color = BLACK;
	}
	else /* color == BLACK */
	{
	  (*p_decode_black) ((short) (b1 - a0 + 2));
	  color = WHITE;
	}
	a0 = b1 + 2; 
	if (current_row == EVEN)
	{
	  even_runs [++even_index] = a0;
	  odd_index++;
	}
	else /* current_row == ODD */
	{
	  odd_runs [++odd_index] = a0;
	  even_index++;
	}
	break;
      case VERTICAL_VR3_MODE: /* the next color change begins three pixels to
	the right of its location on the previous row */
	#if defined (TRACE)
	  if (trace_flag) printf (" VR3 ");
	#endif
	if (color == WHITE)
	{
	  (*p_decode_white) ((short) (b1 - a0 + 3));
	  color = BLACK;
	}
	else /* color == BLACK */
	{
	  (*p_decode_black) ((short) (b1 - a0 + 3));
	  color = WHITE;
	}
	a0 = b1 + 3; 
	if (current_row == EVEN)
	{
	  even_runs [++even_index] = a0;
	  odd_index++;
	}
	else /* current_row == ODD */
	{
	  odd_runs [++odd_index] = a0;
	  even_index++;
	}
	break;
      case VERTICAL_VL1_MODE: /* the next color change begins one pixel to the
	left of its location on the previous row */
	#if defined (TRACE)
	  if (trace_flag) printf (" VL1 ");
	#endif
	if (color == WHITE)
	{
	  (*p_decode_white) ((short) (b1 - a0 - 1));
	  color = BLACK;
	}
	else /* color == BLACK */
	{
	  (*p_decode_black) ((short) (b1 - a0 - 1));
	  color = WHITE;
	}
	a0 = b1 - 1; 
	if (current_row == EVEN)
	{
	  even_runs [++even_index] = a0;
	  odd_index++;
	}
	else /* current_row == ODD */
	{
	  odd_runs [++odd_index] = a0;
	  even_index++;
	}
	break;
      case VERTICAL_VL2_MODE: /* the next color change begins two pixels to
	the left of its location on the previous row */
	#if defined (TRACE)
	  if (trace_flag) printf (" VL2 ");
	#endif
	if (color == WHITE)
	{
	  (*p_decode_white) ((short) (b1 - a0 - 2));
	  color = BLACK;
	}
	else /* color == BLACK */
	{
	  (*p_decode_black) ((short) (b1 - a0 - 2));
	  color = WHITE;
	}
	a0 = b1 - 2; 
	if (current_row == EVEN)
	{
	  even_runs [++even_index] = a0;
	  if (a0 < b0) odd_index--;
	  else odd_index++;
	}
	else /* current_row == ODD */
	{
	  odd_runs [++odd_index] = a0;
	  if (a0 < b0) even_index--;
	  else even_index++;
	}
	break;
      case VERTICAL_VL3_MODE: /* the next color change begins three pixels to
	the left of its location on the previous row */
	#if defined (TRACE)
	  if (trace_flag) printf (" VL3 ");
	#endif
	if (color == WHITE)
	{
	  (*p_decode_white) ((short) (b1 - a0 - 3));
	  color = BLACK;
	}
	else /* color == BLACK */
	{
	  (*p_decode_black) ((short) (b1 - a0 - 3));
	  color = WHITE;
	}
	a0 = b1 - 3; 
	if (current_row == EVEN)
	{
	  even_runs [++even_index] = a0;
	  if (a0 < b0) odd_index--;
	  else odd_index++;
	}
	else /* current_row == ODD */
	{
	  odd_runs [++odd_index] = a0;
	  if (a0 < b0) even_index--;
	  else even_index++;
	}
	break;
      case EXT_MODE_UNCOMPRESSED: /* enter extension type 7 ("111"), an
	uncompressed encoding scheme */
	return (ERROR_UNSUPPORTED_EXTENSION);
	break;
      case ERROR_MODE: /* The bit pattern found corresponds to an unknown or
	invalid codeword.  This MAY be one of the seven possible extensions
	not defined by the specification. */
	return (ERROR_INVALID_CODEWORD);
	break;
      case ERROR_MODE_1: /* assumed in this implementation to be equivalent
	to EOFB (end-of-facsimile-block) */
	return (RETURN_OK);
	break;
      default: /* we should never get here; if we do, the tables are bad */
	return (ERROR_PROGRAM_LOGIC);
	break;
    }
    if (a0 >= column_limit) new_row ();
  }
  return (decode_return);
}

/* g4i_initialize () is called to set up to decode a new image.  All of the
   static data (flags, etc) for g4i_decode () are initialized, allowing the
   decoding of multiple images in a run as long as g4i_initialize () is
   called before each one. */
char g4i_initialize (short image_width, short image_length)
{
  color = WHITE;
  bit_number= 0;
  current_row = ODD;
  even_runs [0] = 0;
  even_runs [1] = image_width; /* initial b1 */
  even_runs [2] = image_width; /* initial b2 */
  odd_runs [0] = 0;
  a0 = 0;
  even_index = 1; odd_index = 0;
  column_limit = image_width;
  row_number = 0;
  b1 = -1;
  decode_return = 0;
  return (0);
}

static short null_mode ()
{
  if (next_bit ()) /* 1 */ return (VERTICAL_V0_MODE);
  else /* 0... */
  {
    if (next_bit ()) /* 01... */
    {
      if (next_bit ()) /* 011 */ return (VERTICAL_VR1_MODE);
      else /* 010 */ return (VERTICAL_VL1_MODE);
    }
    else /* 00... */
    {
      if (next_bit ()) /* 001 */ return (HORIZONTAL_MODE);
      else /* 000... */
      {
	if (next_bit ()) /* 0001 */ return (PASS_MODE);
	else /* 0000... */
	{
	  if (next_bit ()) /* 0000 1... */
	  {
	    if (next_bit ()) /* 0000 11 */ return (VERTICAL_VR2_MODE);
	    else /* 0000 10 */ return (VERTICAL_VL2_MODE);
	  }
	  else /* 0000 0... */
	  {
	    if (next_bit ()) /* 0000 01... */
	    {
	      if (next_bit ()) /* 0000 011 */ return (VERTICAL_VR3_MODE);
	      else /* 0000 010 */ return (VERTICAL_VL3_MODE);
	    }
	    else /* 0000 00... */
	    {
	      if (next_bit ()) /* 0000 001... */
	      {
		if (next_bit ()) /* 0000 0011... */
		{
		  if (next_bit ()) /* 0000 0011 1... */
		  {
		    if (next_bit ()) /* 0000 0011 11 */
					return (EXT_MODE_UNCOMPRESSED);
		    else /* 0000 0011 10 */ return (ERROR_MODE);
		  }
		  else /* 0000 0011 0 */ return (ERROR_MODE);
		}
		else /* 0000 0010 */ return (ERROR_MODE);
	      }
	      else /* 0000 000 */ return (ERROR_MODE_1);
		/* under the assumption that there are no errors in the file,
			then this bit string can only be the beginning of an
			EOFB (end-of-facsimile-block) code */
	    }
	  }
	}
      }
    }
  }
  return (-1);
}

static void new_row ()
{
  (*p_decode_new_row) ();
  color = WHITE;
  if (current_row == ODD)
  {
    current_row = EVEN;
    odd_runs [++odd_index] = a0;
    odd_runs [++odd_index] = a0;
    odd_index = 1;
    even_index = 0;
  }
  else /* current_row == EVEN */
  {
    current_row = ODD;
    even_runs [++even_index] = a0;
    even_runs [++even_index] = a0;
    even_index = 1;
    odd_index = 0;
  }
  a0 = 0;
  b1 = -1;
  #if defined (TRACE)
    row_number++;
  #endif
}

static char next_bit (void)
{
  char value;
  static unsigned char decode_mask [8] =
    { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };
  if (!bit_number) code_byte = (*p_decode_next_byte) ();
  if ((unsigned char) code_byte & decode_mask [bit_number]) value = 1;
  else value = 0;
  if (++bit_number > 7) bit_number = 0;
  return (value);
}

/*
 *	return the length of the next run, assumed to be black
 */

static short decode_black_run ()
{
  short cumulative = 0, code_value;
  for (;;)
  {
    code_value = decode_black_word ();
    cumulative += code_value;
    if (code_value < 0) return (code_value);
    if (code_value < 64) return (cumulative);
  }
}

static short decode_black_word ()
{
  if (next_bit ()) /* 1... */
  {
    if (next_bit ()) /* 11 */ return (2);
    else /* 10 */ return (3);
  }
  else /* 0... */
  {
    if (next_bit ()) /* 01... */
    {
      if (next_bit ()) /* 011 */ return (4);
      else /* 010 */ return (1);
    }
    else /* 00... */
    {
      if (next_bit ()) /* 001... */
      {
	if (next_bit ()) /* 0011 */ return (5);
	else /* 0010 */ return (6);
      }
      else /* 000... */
      {
	if (next_bit ()) /* 0001... */
	{
	  if (next_bit ()) /* 0001 1 */ return (7);
	  else /* 0001 0... */
	  {
	    if (next_bit ()) /* 0001 01 */ return (8);
	    else /* 0001 00 */ return (9);
	  }
	}
	else /* 0000... */
	{
	  if (next_bit ()) /* 0000 1... */
	  {
	    if (next_bit ()) /* 0000 11... */
	    {
	      if (next_bit ()) /* 0000 111 */ return (12);
	      else /* 0000 110... */
	      {
		if (next_bit ()) /* 0000 1101... */
		{
		  if (next_bit ()) /* 0000 1101 1... */
		  {
		    if (next_bit ()) /* 0000 1101 11 */ return (0);
		    else /* 0000 1101 10... */
		    {
		      if (next_bit ()) /* 0000 1101 101... */
		      {
			if (next_bit ()) /* 0000 1101 1011 */ return (43);
			else /* 0000 1101 1010 */ return (42);
		      }
		      else /* 0000 1101 100 */ return (21);
		    }
		  }
		  else /* 0000 1101 0... */
		  {
		    if (next_bit ()) /* 0000 1101 01... */
		    {
		      if (next_bit ()) /* 0000 1101 011... */
		      {
			if (next_bit ()) /* 0000 1101 0111 */ return (39);
			else /* 0000 1101 0110 */ return (38);
		      }
		      else /* 0000 1101 010... */
		      {
			if (next_bit ()) /* 0000 1101 0101 */ return (37);
			else /* 0000 1101 0100 */ return (36);
		      }
		    }
		    else /* 0000 1101 00... */
		    {
		      if (next_bit ()) /* 0000 1101 001... */
		      {
			if (next_bit ()) /* 0000 1101 0011 */ return (35);
			else /* 0000 1101 0010 */ return (34);
		      }
		      else /* 0000 1101 000 */ return (20);
		    }
		  }
		}
		else /* 0000 1100... */
		{
		  if (next_bit ()) /* 0000 1100 1... */
		  {
		    if (next_bit ()) /* 0000 1100 11... */
		    {
		      if (next_bit ()) /* 0000 1100 111 */ return (19);
		      else /* 0000 1100 110... */
		      {
			if (next_bit ()) /* 0000 1100 1101 */ return (29);
			else /* 0000 1100 1100 */ return (28);
		      }
		    }
		    else /* 0000 1100 10... */
		    {
		      if (next_bit ()) /* 0000 1100 101.. */
		      {
			if (next_bit ()) /* 0000 1100 1011 */ return (27);
			else /* 0000 1100 1010 */ return (26);
		      }
		      else /* 0000 1100 100... */
		      {
			if (next_bit ()) /* 0000 1100 1001 */ return (192);
			else /* 0000 1100 1000 */ return (128);
		      }
		    }
		  }
		  else /* 0000 1100 0 */ return (15);
		}
	      }
	    }
	    else /* 0000 10... */
	    {
	      if (next_bit ()) /* 0000 101 */ return (11);
	      else /* 0000 100 */ return (10);
	    }
	  }
	  else /* 0000 0... */
	  {
	    if (next_bit ()) /* 0000 01... */
	    {
	      if (next_bit ()) /* 0000 011... */
	      {
		if (next_bit ()) /* 0000 0111 */ return (14);
		else /* 0000 0110... */
		{
		  if (next_bit ()) /* 0000 0110 1... */
		  {
		    if (next_bit ()) /* 0000 0110 11... */
		    {
		      if (next_bit ()) /* 0000 0110 111 */ return (22);
		      else /* 0000 0110 110... */
		      {
			if (next_bit ()) /* 0000 0110 1101 */ return (41);
			else /* 0000 0110 1100 */ return (40);
		      }
		    }
		    else /* 0000 0110 10... */
		    {
		      if (next_bit ()) /* 0000 0110 101... */
		      {
			if (next_bit ()) /* 0000 0110 1011 */ return (33);
			else /* 0000 0110 1010 */ return (32);
		      }
		      else /* 0000 0110 100... */
		      {
			if (next_bit ()) /* 0000 0110 1001 */ return (31);
			else /* 0000 0110 1000 */ return (30);
		      }
		    }
		  }
		  else /* 0000 0110 0... */
		  {
		    if (next_bit ()) /* 0000 0110 01... */
		    {
		      if (next_bit ()) /* 0000 0110 011... */
		      {
			if (next_bit ()) /* 0000 0110 0111 */ return (63);
			else /* 0000 0110 0110 */ return (62);
		      }
		      else /* 0000 0110 010... */
		      {
			if (next_bit ()) /* 0000 0110 0101 */ return (49);
			else /* 0000 0110 0100 */ return (48);
		      }
		    }
		    else /* 0000 0110 00 */ return (17);
		  }
		}
	      }
	      else /* 0000 010... */
	      {
		if (next_bit ()) /* 0000 0101... */
		{
		  if (next_bit ()) /* 0000 0101 1... */
		  {
		    if (next_bit ()) /* 0000 0101 11 */ return (16);
		    else /* 0000 0101 10... */
		    {
		      if (next_bit ()) /* 0000 0101 101... */
		      {
			if (next_bit ()) /* 0000 0101 1011 */ return (256);
			else /* 0000 0101 1010 */ return (61);
		      }
		      else /* 0000 0101 100... */
		      {
			if (next_bit ()) /* 0000 0101 1001 */ return (58);
			else /* 0000 0101 1000 */ return (57);
		      }
		    }
		  }
		  else /* 0000 0101 0... */
		  {
		    if (next_bit ()) /* 0000 0101 01... */
		    {
		      if (next_bit ()) /* 0000 0101 011... */
		      {
			if (next_bit ()) /* 0000 0101 0111 */ return (47);
			else /* 0000 0101 0110 */ return (46);
		      }
		      else /* 0000 0101 010... */
		      {
			if (next_bit ()) /* 0000 0101 0101 */ return (45);
			else /* 0000 0101 0100 */ return (44);
		      }
		    }
		    else /* 0000 0101 00... */
		    {
		      if (next_bit ()) /* 0000 0101 001... */
		      {
			if (next_bit ()) /* 0000 0101 0011 */ return (51);
			else /* 0000 0101 0010 */ return (50);
		      }
		      else /* 0000 0101 000 */ return (23);
		    }
		  }
		}
		else /* 0000 0100 */ return (13);
	      }
	    }
	    else /* 0000 00... */
	    {
	      if (next_bit ()) /* 0000 001... */
	      {
		if (next_bit ()) /* 0000 0011... */
		{
		  if (next_bit ()) /* 0000 0011 1... */
		  {
		    if (next_bit ()) /* 0000 0011 11 */ return (64);
		    else /* 0000 0011 10... */
		    {
		      if (next_bit ()) /* 0000 0011 101... */
		      {
			if (next_bit ()) /* 0000 0011 1011... */
			{
			  if (next_bit ()) /* 0000 0011 1011 1 */ return (1216);
			  else /* 0000 0011 1011 0 */ return (1152);
			}
			else /* 0000 0011 1010... */
			{
			  if (next_bit ()) /* 0000 0011 1010 1 */ return (1088);
			  else /* 0000 0011 1010 0 */ return (1024);
			}
		      }
		      else /* 0000 0011 100... */
		      {
			if (next_bit ()) /* 0000 0011 1001... */
			{
			  if (next_bit ()) /* 0000 0011 1001 1 */ return (960);
			  else /* 0000 0011 1001 0 */ return (896);
			}
			else /* 0000 0011 1000 */ return (54);
		      }
		    }
		  }
		  else /* 0000 0011 0... */
		  {
		    if (next_bit ()) /* 0000 0011 01... */
		    {
		      if (next_bit ()) /* 0000 0011 011... */
		      {
			if (next_bit ()) /* 0000 0011 0111 */ return (53);
			else /* 0000 0011 0110... */
			{
			  if (next_bit ()) /* 0000 0011 0110 1 */ return (576);
			  else /* 0000 0011 0110 0 */ return (512);
			}
		      }
		      else /* 0000 0011 010... */
		      {
			if (next_bit ()) /* 0000 0011 0101 */ return (448);
			else /* 0000 0011 0100 */ return (384);
		      }
		    }
		    else /* 0000 0011 00... */
		    {
		      if (next_bit ()) /* 0000 0011 001... */
		      {
			if (next_bit ()) /* 0000 0011 0011 */ return (320);
			else /* 0000 0011 0010... */
			{
			  if (next_bit ()) /* 0000 0011 0010 1 */ return (1728);
			  else /* 0000 0011 0010 0 */ return (1664);
			}
		      }
		      else /* 0000 0011 000 */ return (25);
		    }
		  }
		}
		else /* 0000 0010... */
		{
		  if (next_bit ()) /* 0000 0010 1... */
		  {
		    if (next_bit ()) /* 0000 0010 11... */
		    {
		      if (next_bit ()) /* 0000 0010 111 */ return (24);
		      else /* 0000 0010 110... */
		      {
			if (next_bit ()) /* 0000 0010 1101... */
			{
			  if (next_bit ()) /* 0000 0010 1101 1 */ return (1600);
			  else /* 0000 0010 1101 0 */ return (1536);
			}
			else /* 0000 0010 1100 */ return (60);
		      }
		    }
		    else /* 0000 0010 10... */
		    {
		      if (next_bit ()) /* 0000 0010 101... */
		      {
			if (next_bit ()) /* 0000 0010 1011 */ return (59);
			else /* 0000 0010 1010... */
			{
			  if (next_bit ()) /* 0000 0010 1010 1 */ return (1472);
			  else /* 0000 0010 1010 0 */ return (1408);
			}
		      }
		      else /* 0000 0010 100... */
		      {
			if (next_bit ()) /* 0000 0010 1001... */
			{
			  if (next_bit ()) /* 0000 0010 1001 1 */ return (1344);
			  else /* 0000 0010 1001 0 */ return (1280);
			}
			else /* 0000 0010 1000 */ return (56);
		      }
		    }
		  }
		  else /* 0000 0010 0... */
		  {
		    if (next_bit ()) /* 0000 0010 01... */
		    {
		      if (next_bit ()) /* 0000 0010 011... */
		      {
			if (next_bit ()) /* 0000 0010 0111 */ return (55);
			else /* 0000 0010 0110... */
			{
			  if (next_bit ()) /* 0000 0010 0110 1 */ return (832);
			  else /* 0000 0010 0110 0 */ return (768);
			}
		      }
		      else /* 0000 0010 010... */
		      {
			if (next_bit ()) /* 0000 0010 0101... */
			{
			  if (next_bit ()) /* 0000 0010 0101 1 */ return (704);
			  else /* 0000 0010 0101 0 */ return (640);
			}
			else /* 0000 0010 0100 */ return (52);
		      }
		    }
		    else /* 0000 0010 00 */ return (18);
		  }
		}
	      }
	      else /* 0000 000... */
	      {
		if (next_bit ()) /* 0000 0001... */
		{
		  if (next_bit ()) /* 0000 0001 1... */
		  {
		    if (next_bit ()) /* 0000 0001 11... */
		    {
		      if (next_bit ()) /* 0000 0001 111... */
		      {
			if (next_bit ()) /* 0000 0001 1111 */ return (2560);
			else /* 0000 0001 1110 */ return (2496);
		      }
		      else /* 0000 0001 110... */
		      {
			if (next_bit ()) /* 0000 0001 1101 */ return (2432);
			else /* 0000 0001 1100 */ return (2368);
		      }
		    }
		    else /* 0000 0001 10... */
		    {
		      if (next_bit ()) /* 0000 0001 101 */ return (1920);
		      else /* 0000 0001 100 */ return (1856);
		    }
		  }
		  else /* 0000 0001 0... */
		  {
		    if (next_bit ()) /* 0000 0001 01... */
		    {
		      if (next_bit ()) /* 0000 0001 011... */
		      {
			if (next_bit ()) /* 0000 0001 0111 */ return (2304);
			else /* 0000 0001 0110 */ return (2240);
		      }
		      else /* 0000 0001 010... */
		      {
			if (next_bit ()) /* 0000 0001 0101 */ return (2176);
			else /* 0000 0001 0100 */ return (2112);
		      }
		    }
		    else /* 0000 0001 00... */
		    {
		      if (next_bit ()) /* 0000 0001 001... */
		      {
			if (next_bit ()) /* 0000 0001 0011 */ return (2048);
			else /* 0000 0001 0010 */ return (1984);
		      }
		      else /* 0000 0001 000 */ return (1792);
		    }
		  }
		}
		else /* 0000 0000... */
		{
		  if (next_bit ()) /* 0000 0000 1 */ return (INVALID_CODE);
		  else /* 0000 0000 0... */
		  {
		    if (next_bit ()) /* 0000 0000 01 */ return (INVALID_CODE);
		    else /* 0000 0000 00... */
		    {
		      if (next_bit ()) /* 0000 0000 001 */
							return (INVALID_CODE);
		      else /* 0000 0000 000... */
		      {
			if (next_bit ()) /* 0000 0000 0001 */ return (EOL_CODE);
			else /* 0000 0000 0000 */ return (INVALID_CODE);
		      }
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }
}

/*
 *	return the length of the next run, assumed to be white 
 */

static short decode_white_run ()
{
  short cumulative = 0, code_value;
  for (;;)
  {
    code_value = decode_white_word ();
    cumulative += code_value;
    if (code_value < 0) return (code_value);
    if (code_value < 64) return (cumulative);
  }
}

static short decode_white_word ()
{
  if (next_bit ()) /* 1... */
  {
    if (next_bit ()) /* 11... */
    {
      if (next_bit ()) /* 111... */
      {
	if (next_bit ()) /* 1111 */ return (7);
	else /* 1110 */ return (6);
      }
      else /* 110... */
      {
	if (next_bit ()) /* 1101... */
	{
	  if (next_bit ()) /* 1101 1 */ return (64);
	  else /* 1101 0... */
	  {
	    if (next_bit ()) /* 1101 01 */ return (15);
	    else /* 1101 00 */ return (14);
	  }
	}
	else /* 1100 */ return (5);
      }
    }
    else /* 10... */
    {
      if (next_bit ()) /* 101... */
      {
	if (next_bit ()) /* 1011 */ return (4);
	else /* 1010... */
	{
	  if (next_bit ()) /* 10101... */
	  {
	    if (next_bit ()) /* 101011 */ return (17);
	    else /* 101010 */ return (16);
	  }
	  else /* 10100 */ return (9);
	}
      }
      else /* 100... */
      {
	if (next_bit ()) /* 1001... */
	{
	  if (next_bit ()) /* 10011 */ return (8);
	  else /* 10010 */ return (128);
	}
	else /* 1000 */ return (3);
      }
    }
  }
  else /* 0... */
  {
    if (next_bit ()) /* 01... */
    {
      if (next_bit ()) /* 011... */
      {
	if (next_bit ()) /* 0111 */ return (2);
	else /* 0110... */
	{
	  if (next_bit ()) /* 01101... */
	  {
	    if (next_bit ()) /* 011011... */
	    {
	      if (next_bit ()) /* 0110111 */ return (256);
	      else /* 0110110... */
	      {
		if (next_bit ()) /* 01101101... */
		{
		  if (next_bit ()) /* 011011011 */ return (1408);
		  else /* 011011010 */ return (1344);
		}
	        else /* 01101100... */
		{
		  if (next_bit ()) /* 011011001 */ return (1280);
		  else /* 011011000 */ return (1216);
		}
	      }
	    }
	    else /* 011010... */
	    {
	      if (next_bit ()) /* 0110101... */
	      {
		if (next_bit ()) /* 01101011... */
		{
		  if (next_bit ()) /* 011010111 */ return (1152);
		  else /* 011010110 */ return (1088);
		}
		else /* 01101010... */
		{
		  if (next_bit ()) /* 011010101 */ return (1024);
		  else /* 011010100 */ return (960);
		}
	      }
	      else /* 0110100... */
	      {
		if (next_bit ()) /* 01101001... */
		{
		  if (next_bit ()) /* 011010011 */ return (896);
		  else /* 011010010 */ return (832);
		}
		else /* 01101000 */ return (576);
	      }
	    }
	  }
	  else /* 01100... */
	  {
	    if (next_bit ()) /* 011001... */
	    {
	      if (next_bit ()) /* 0110011... */
	      {
		if (next_bit ()) /* 01100111 */ return (640);
		else /* 01100110 */
		{
		  if (next_bit ()) /* 011001101 */ return (768);
		  else /* 011001100 */ return (704);
		}
	      }
	      else /* 0110010 */
	      {
		if (next_bit ()) /* 01100101 */ return (512);
		else /* 01100100 */ return (448);
	      }
	    }
	    else /* 011000 */ return (1664);
	  }
	}
      }
      else /* 010... */
      {
	if (next_bit ()) /* 0101... */
	{
	  if (next_bit ()) /* 01011... */
	  {
	    if (next_bit ()) /* 010111 */ return (192);
	    else /* 010110... */
	    {
	      if (next_bit ()) /* 0101101... */
	      {
		if (next_bit ()) /* 01011011 */ return (58);
		else /* 01011010 */ return (57);
	      }
	      else /* 0101100... */
	      {
		if (next_bit ()) /* 01011001 */ return (56);
		else /* 01011000 */ return (55);
	      }
	    }
	  }
	  else /* 01010... */
	  {
	    if (next_bit ()) /* 010101... */
	    {
	      if (next_bit ()) /* 0101011 */ return (25);
	      else /* 0101010... */
	      {
		if (next_bit ()) /* 01010101 */ return (52);
		else /* 01010100 */ return (51);
	      }
	    }
	    else /* 010100... */
	    {
	      if (next_bit ()) /* 0101001... */
	      {
		if (next_bit ()) /* 01010011 */ return (50);
		else /* 01010010 */ return (49);
	      }
	      else /* 0101000 */ return (24);
	    }
	  }
	}
	else /* 0100... */
	{
	  if (next_bit ()) /* 01001... */
	  {
	    if (next_bit ()) /* 010011... */
	    {
	      if (next_bit ()) /* 0100111 */ return (18);
	      else /* 0100110... */
	      {
		if (next_bit ()) /* 01001101... */
		{
		  if (next_bit ()) /* 010011011 */ return (1728);
		  else /* 010011010 */ return (1600);
		}
		else /* 01001100... */
		{
		  if (next_bit ()) /* 010011001 */ return (1536);
		  else /* 010011000 */ return (1472);
		}
	      }
	    }
	    else /* 010010... */
	    {
	      if (next_bit ()) /* 0100101... */
	      {
		if (next_bit ()) /* 01001011 */ return (60);
		else /* 01001010 */ return (59);
	      }
	      else /* 0100100 */ return (27);
	    }
	  }
	  else /* 01000 */ return (11);
	}
      }
    }
    else /* 00... */
    {
      if (next_bit ()) /* 001... */
      {
	if (next_bit ()) /* 0011... */
	{
	  if (next_bit ()) /* 00111 */ return (10);
	  else /* 00110... */
	  {
	    if (next_bit ()) /* 001101... */
	    {
	      if (next_bit ()) /* 0011011... */
	      {
		if (next_bit ()) /* 00110111 */ return (384);
		else /* 00110110 */ return (320);
	      }
	      else /* 0011010... */
	      {
		if (next_bit ()) /* 00110101 */ return (0);
		else /* 00110100 */ return (63);
	      }
	    }
	    else /* 001100... */
	    {
	      if (next_bit ()) /* 0011001... */
	      {
		if (next_bit ()) /* 00110011 */ return (62);
		else /* 00110010 */ return (61);
	      }
	      else /* 0011000 */ return (28);
	    }
	  }
	}
	else /* 0010... */
	{
	  if (next_bit ()) /* 00101... */
	  {
	    if (next_bit ()) /* 001011... */
	    {
	      if (next_bit ()) /* 0010111 */ return (21);
	      else /* 0010110... */
	      {
		if (next_bit ()) /* 00101101 */ return (44);
		else /* 00101100 */ return (43);
	      }
	    }
	    else /* 001010... */
	    {
	      if (next_bit ()) /* 0010101... */
	      {
		if (next_bit ()) /* 00101011 */ return (42);
		else /* 00101010 */ return (41);
	      }
	      else /* 0010100... */
	      {
		if (next_bit ()) /* 00101001 */ return (40);
		else /* 00101000 */ return (39);
	      }
	    }
	  }
	  else /* 00100... */
	  {
	    if (next_bit ()) /* 001001... */
	    {
	      if (next_bit ()) /* 0010011 */ return (26);
	      else /* 0010010... */
	      {
		if (next_bit ()) /* 00100101 */ return (54);
		else /* 00100100 */ return (53);
	      }
	    }
	    else /* 001000 */ return (12);
	  }
	}
      }
      else /* 000... */
      {
	if (next_bit ()) /* 0001... */
	{
	  if (next_bit ()) /* 00011... */
	  {
	    if (next_bit ()) /* 000111 */ return (1);
	    else /* 000110... */
	    {
	      if (next_bit ()) /* 0001101... */
	      {
		if (next_bit ()) /* 00011011 */ return (32);
		else /* 00011010 */ return (31);
	      }
	      else /* 0001100 */ return (19);
	    }
	  }
	  else /* 00010... */
	  {
	    if (next_bit ()) /* 000101... */
	    {
	      if (next_bit ()) /* 0001011... */
	      {
		if (next_bit ()) /* 00010111 */ return (38);
		else /* 00010110 */ return (37);
	      }
	      else /* 0001010... */
	      {
		if (next_bit ()) /* 00010101 */ return (36);
		else /* 00010100 */ return (35);
	      }
	    }
	    else /* 000100... */
	    {
	      if (next_bit ()) /* 0001001... */
	      {
		if (next_bit ()) /* 00010011 */ return (34);
		else /* 00010010 */ return (33);
	      }
	      else /* 0001000 */ return (20);
	    }
	  }
	}
	else /* 0000... */
	{
	  if (next_bit ()) /* 00001... */
	  {
	    if (next_bit ()) /* 000011 */ return (13);
	    else /* 000010... */
	    {
	      if (next_bit ()) /* 0000101... */
	      {
		if (next_bit ()) /* 00001011 */ return (48);
		else /* 00001010 */ return (47);
	      }
	      else /* 0000100 */ return (23);
	    }
	  }
	  else /* 00000... */
	  {
	    if (next_bit ()) /* 000001... */
	    {
	      if (next_bit ()) /* 0000011 */ return (22);
	      else /* 0000010... */
	      {
		if (next_bit ()) /* 00000101 */ return (46);
		else /* 00000100 */ return (45);
	      }
	    }
	    else /* 000000... */
	    {
	      if (next_bit ()) /* 0000001... */
	      {
		if (next_bit ()) /* 00000011 */ return (30);
		else /* 00000010 */ return (29);
	      }
	      else /* 0000 000... */
	      {
		if (next_bit ()) /* 0000 0001... */
		{
		  if (next_bit ()) /* 0000 0001 1... */
		  {
		    if (next_bit ()) /* 0000 0001 11... */
		    {
		      if (next_bit ()) /* 0000 0001 111... */
		      {
			if (next_bit ()) /* 0000 0001 1111 */ return (2560);
			else /* 0000 0001 1110 */ return (2496);
		      }
		      else /* 0000 0001 110... */
		      {
			if (next_bit ()) /* 0000 0001 1101 */ return (2432);
			else /* 0000 0001 1100 */ return (2368);
		      }
		    }
		    else /* 0000 0001 10... */
		    {
		      if (next_bit ()) /* 0000 0001 101 */ return (1920);
		      else /* 0000 0001 100 */ return (1856);
		    }
		  }
		  else /* 0000 0001 0... */
		  {
		    if (next_bit ()) /* 0000 0001 01... */
		    {
		      if (next_bit ()) /* 0000 0001 011... */
		      {
			if (next_bit ()) /* 0000 0001 0111 */ return (2304);
			else /* 0000 0001 0110 */ return (2240);
		      }
		      else /* 0000 0001 010... */
		      {
			if (next_bit ()) /* 0000 0001 0101 */ return (2176);
			else /* 0000 0001 0100 */ return (2112);
		      }
		    }
		    else /* 0000 0001 00... */
		    {
		      if (next_bit ()) /* 0000 0001 001... */
		      {
			if (next_bit ()) /* 0000 0001 0011 */ return (2048);
			else /* 0000 0001 0010 */ return (1984);
		      }
		      else /* 0000 0001 000 */ return (1792);
		    }
		  }
		}
		else /* 0000 0000... */
		{
		  if (next_bit ()) /* 0000 0000 1 */ return (INVALID_CODE);
		  else /* 0000 0000 0... */
		  {
		    if (next_bit ()) /* 0000 0000 01 */ return (INVALID_CODE);
		    else /* 0000 0000 00... */
		    {
		      if (next_bit ()) /* 0000 0000 001 */
							return (INVALID_CODE);
		      else /* 0000 0000 000... */
		      {
			if (next_bit ()) /* 0000 0000 0001 */ return (EOL_CODE);
			else /* 0000 0000 0000 */ return (INVALID_CODE);
		      }
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }
}

/*	end $RCSfile: g4sdecod.c $ */
