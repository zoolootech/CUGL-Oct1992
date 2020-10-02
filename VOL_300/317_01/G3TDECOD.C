/*	$Id: g3tdecod.c 1.2 90/06/09 18:23:04 marking Exp $
 *
 NAME
 *	g3tdecod.c -- decode group 3 data using tables
 *
 TYPE
 *	C procedures
 *
 SYNOPSIS
 *	char	g3i_initialize (short image_width, short image_length);
 *	char	g3i_decode (void);	-- for standard group 3
 *	char	g3i_decode_T (void);	-- for TIFF g3 with no EOL chars
 *
 DESCRIPTION
 *	In order to acquire data from the image and to return run lengths and
 *	new line information, these routines invoke procedures provided by the
 *	caller. These caller-provided procedures are invoked throught pointers
 *	which have been stuffed by the caller with the procedure addresses.
 *	To acquire a new data byte, g3i_decode () and g3i_decode_T () call
 *	(*p_decode_next_byte) (). To report the decoding of a black or white
 *	run, the routines (*p_decode_black) () or (*p_decode_white) () are
 *	called.
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
 *	Reference: $Id: g3tdecod.c 1.2 90/06/09 18:23:04 marking Exp $
 *
 *	This program is offered without any warranty of any kind. It includes
 *	no warranty of merchantability or fitness for any purpose. Testing and
 *	suitability for any use are the sole responsibility of the user.
 * 
 HISTORY
 *	$Log:	g3tdecod.c $
 * Revision 1.2  90/06/09  18:23:04  marking
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
 *	g4tdecod.c -- decode group 4 image using tables
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

static short bit_number, code_byte;
static unsigned char color, current_row, mode, next_state;
static short column_limit, row_limit;
static short row_number = 0, column_number;

extern unsigned char huge horiz_mode [] [256];
extern unsigned char huge horiz_mode_next_state [] [256];

static short new_row (void);
static short decode_white_run (void);
static short decode_black_run (void);

static char decode_return;

/* g3i_decode () successively invokes (*p_decode_next_byte) () for each byte of the
   encoded image, and calls (*p_decode_white) () or (*p_decode_black) () as
   required to return the image contents on a run-by-run basis. */

char g3i_decode ()
{
  short runlength;
  while (!decode_return)
  {
    if (color == WHITE)
    {
      runlength = decode_white_run ();
      if (runlength == -2) return (-1);
      else if (runlength == -1)
      {
	if (new_row ()) return (0);
      }
      else
      {
	column_number += runlength;
	(*p_decode_white) ((short) runlength);
	color = BLACK;
      }
    }
    else
    {
      runlength = decode_black_run ();
      if (runlength == -2) return (-1);
      else if (runlength == -1)
      {
	if (new_row ()) return (0);
      }
      else
      {
	column_number += runlength;
	(*p_decode_black) ((short) runlength);
	color = WHITE;
      }
    }
  }
  return (decode_return);
}

/* special version for TIFF files with no EOL characters */
char g3i_decode_T ()
{
  short runlength;
  while (!decode_return)
  {
    if (color == WHITE)
    {
      runlength = decode_white_run ();
      if (runlength == -2) return (-1);
      else if (runlength == -1)
      {
	if (new_row ()) return (0);
      }
      else
      {
	column_number += runlength;
	(*p_decode_white) ((short) runlength);
	color = BLACK;
	if (column_number >= column_limit)
	{
	  if (new_row ()) return (0);
	  bit_number = 0;
	}
      }
    }
    else
    {
      runlength = decode_black_run ();
      if (runlength == -2) return (-1);
      else if (runlength == -1)
      {
	if (new_row ()) return (0);
      }
      else
      {
	column_number += runlength;
	(*p_decode_black) ((short) runlength);
	color = WHITE;
	if (column_number >= column_limit)
	{
	  if (new_row ()) return (0);
	  bit_number = 0;
	}
      }
    }
  }
  return (decode_return);
}

/* g3i_initialize () is called to set up to decode a new image.  All of the
   static data (flags, etc) for g3i_decode () are initialized, allowing the
   decoding of multiple images as long as g3i_initialize () is
   called before each. */
char g3i_initialize (short image_width, short image_length)
{
  color = WHITE;
  bit_number= 0;
  column_limit = image_width;
  row_limit = image_length;
  row_number = 0;
  column_number = 0;
  decode_return = 0;
  return (0);
}

static short new_row ()
{
  if (column_number)
  {
    (*p_decode_new_row) ();
    color = WHITE;
    if (++row_number >= row_limit) return (-1);
    column_number = 0;
    return (0);
  }
  else return (0);
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
    if (mode == 1) return (-2); /* invalid code */
    else if (mode == 210)
    {
      bit_number = next_state;
      return (-1);
    }
    else if (mode) /* if digestible */
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
    if (mode == 1) return (-2); /* invalid code */
    else if (mode == 210)
    {
      bit_number = next_state;
      return (-1);
    }
    else if (mode) /* if digestible */
    {
      bit_number = next_state;
      runlength = run_length_table [mode - 2];
      accum_runlength += runlength;
      if (runlength < 64) return (accum_runlength);
    }
    else bit_number = 0;
  }
}

/*	end $RCSfile: g3tdecod.c $ */
