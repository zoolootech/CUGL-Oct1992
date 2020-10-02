/*	$Id: g4tdecod.c 1.2 90/06/09 18:24:25 marking Exp $
 *
 NAME
 *	g4tdecod.c -- decode group 4 data using tables
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
 *	To acquire a new data byte, g4i_decode () calls (*p_g41_next_byte) ().
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
 *	Reference: $Id: g4tdecod.c 1.2 90/06/09 18:24:25 marking Exp $
 *
 *	This program is offered without any warranty of any kind. It includes
 *	no warranty of merchantability or fitness for any purpose. Testing and
 *	suitability for any use are the sole responsibility of the user.
 * 
 HISTORY
 *	$Log:	g4tdecod.c $
 * Revision 1.2  90/06/09  18:24:25  marking
 * clean up comments for release
 * 
 * Revision 1.1  89/06/30  17:00:00  marking
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
#define TRACE_BEGIN 0
#define TRACE_END 30000

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
static unsigned char color, current_row, mode, next_state;
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

extern unsigned char huge null_mode [] [256];
  /* One of the entries PASS_MODE, HORIZONTAL_MODE, etc, or zero.  If the entry
     is zero, then the bit string is indigestible and null_mode_next_state [][]
     is consulted to decode the subsequent byte.  The row number (first sub-
     script) corresponds to the assumption about the state of the machine after
     decoding the previous codeword.  If < 8, then it refers to the starting
     bit number for this codeword.  If > 7, it implies a bit string to be pre-
     fixed to the first bit of the current byte.  The column number (second
     subscript) refers to the value of the current byte. */

extern unsigned char huge null_mode_next_state [] [256];
extern unsigned char huge horiz_mode [] [256];
extern unsigned char huge horiz_mode_next_state [] [256];

static void new_row (void);
static short decode_white_run (void);
static short decode_black_run (void);

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
    /* No codewords have been digested, so no mode is currently understood.
       It is possible, however, that bits have been left over from the last
       code byte, and that these must be conceptually prefixed to the next
       bits at the beginning of the next code byte to establish which code
       word is next.  The decision is made based on bit_number, which is
       zero if the codeword begins on a byte boundary; 1, 2, ... 7 if the
       codeword begins with bit 1, 2, ... 7; or > 7 if the codeword begins
       with bit zero but is assumed to have a non-null bits string prefix.
       In any case, the bit_number corresponds to a row in the decoding
       tables. */
    if (bit_number == 0 || bit_number > 7) /* the code word will begin on
      the first bit of the next code byte, possibly with a bit string
      implicitly prefixed */
    {
      code_byte = (*p_decode_next_byte) (); /* invoke a user-supplied routine
        to provide the next byte of the encoded image */
      if (code_byte < 0) /* implying end-of-file or some error in
        reading the next code byte */
        return (ERROR_NEXT_BYTE);
    }
    /* else 0 < bit_number < 8, implying that the next code word begins within
      the current code byte; that is, a new byte need not be fetched */
    mode = null_mode [bit_number] [code_byte];
    next_state = null_mode_next_state [bit_number] [code_byte];
      /* The above two index operations involve muliplication (by the compiler)
         of the first subscript by 256, a procedure obviously a candidate for
	 optimization in many architectures.  It may be worthwhile to hand-code
	 this calculation, since this is a fairly tight loop.  Check also that
	 the compiler does the calculation but once and saves the result in a
	 temporary, since the two arrays are of identical size. */
    if (mode == NULL_MODE) /* the codeword (or partial codeword) is
      indigestible */
    {
      bit_number = next_state; /* to be continued... */
    }
    else
    {
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
	if (row_number >= TRACE_BEGIN && row_number <= TRACE_END)
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
            if (row_number >= TRACE_BEGIN && row_number <= TRACE_END)
              printf (" P ");
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
          bit_number = next_state;
          break;
        case HORIZONTAL_MODE: /* revert to 1-dimensional modified Huffman
          encoding for a pair of runs */
          bit_number = next_state;
	  #if defined (TRACE)
            if (row_number >= TRACE_BEGIN && row_number <= TRACE_END)
              printf (" H ");
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
            if (row_number >= TRACE_BEGIN && row_number <= TRACE_END)
              printf (" V0 ");
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
          bit_number = next_state;
          break;
        case VERTICAL_VR1_MODE: /* the next color change begins one pixel to the
          right of its location on the previous row */
	  #if defined (TRACE)
            if (row_number >= TRACE_BEGIN && row_number <= TRACE_END)
              printf (" VR1 ");
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
          bit_number = next_state;
          break;
        case VERTICAL_VR2_MODE: /* the next color change begins two pixels to
          the right of its location on the previous row */
	  #if defined (TRACE)
            if (row_number >= TRACE_BEGIN && row_number <= TRACE_END)
              printf (" VR2 ");
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
          bit_number = next_state;
          break;
        case VERTICAL_VR3_MODE: /* the next color change begins three pixels to
          the right of its location on the previous row */
	  #if defined (TRACE)
            if (row_number >= TRACE_BEGIN && row_number <= TRACE_END)
              printf (" VR3 ");
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
          bit_number = next_state;
          break;
        case VERTICAL_VL1_MODE: /* the next color change begins one pixel to the
          left of its location on the previous row */
	  #if defined (TRACE)
            if (row_number >= TRACE_BEGIN && row_number <= TRACE_END)
              printf (" VL1 ");
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
          bit_number = next_state;
          break;
        case VERTICAL_VL2_MODE: /* the next color change begins two pixels to
          the left of its location on the previous row */
	  #if defined (TRACE)
            if (row_number >= TRACE_BEGIN && row_number <= TRACE_END)
              printf (" VL2 ");
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
          bit_number = next_state;
          break;
        case VERTICAL_VL3_MODE: /* the next color change begins three pixels to
          the left of its location on the previous row */
	  #if defined (TRACE)
            if (row_number >= TRACE_BEGIN && row_number <= TRACE_END)
              printf (" VL3 ");
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
          bit_number = next_state;
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

static short run_length_table [] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
  13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
  32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
  51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 128, 192, 256, 320,
  384, 448, 512, 576, 640, 704, 768, 832, 896, 960, 1024, 1088, 1152, 1216,
  1280, 1344, 1408, 1472, 1536, 1600, 1664, 1728, 1792, 1856, 1920, 1984,
  2048, 2112, 2176, 2240, 2304, 2368, 2432, 2496, 2560};

static short decode_black_run ()
{
  short runlength, accum_runlength = 0;
  next_state = (unsigned char) (bit_number + 8);
  for (;;) /* exit with "return" */
  {
    if (!bit_number) code_byte = (*p_decode_next_byte) ();
	 /* this will fetch a new byte
	    if the previous codeword ended on a byte boundary */
    mode = horiz_mode [next_state] [code_byte];
    next_state = horiz_mode_next_state [next_state] [code_byte];
    if (mode) /* if digestible */
    {
      bit_number = next_state;
      runlength = run_length_table [mode - 106];
      accum_runlength += runlength;
      if (runlength < 64) return (accum_runlength);
      next_state += 8;
    }
    else bit_number = 0;
  }
}

static short decode_white_run ()
{
  short runlength, accum_runlength = 0;
  next_state = (unsigned char) bit_number;
  for (;;) /* exit with "return" */
  {
    if (!bit_number) code_byte = (*p_decode_next_byte) ();
	 /* this will fetch a new byte
	    if the previous codeword ended on a byte boundary */
    mode = horiz_mode [next_state] [code_byte];
    next_state = horiz_mode_next_state [next_state] [code_byte];
    if (mode) /* if digestible */
    {
      bit_number = next_state;
      runlength = run_length_table [mode - 2];
      accum_runlength += runlength;
      if (runlength < 64) return (accum_runlength);
    }
    else bit_number = 0;
  }
}

/*	end $RCSfile: g4tdecod.c $ */
