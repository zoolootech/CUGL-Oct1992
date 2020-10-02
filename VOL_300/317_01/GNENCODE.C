/*	$Id: gnencode.c 1.2 90/06/09 18:24:51 marking Exp $
 *
 NAME
 *	gnencode.c -- routines for bit and byte encoding
 *
 TYPE
 *	C procedures
 *
 SYNOPSIS
 *	void initialize_encode ();
 *	void encode_word (unsigned short codeword, short length);
 *
 DESCRIPTION
 *	Support routines used in encoding group 3 and group 4 images.
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
 *	Reference: $Id: gnencode.c 1.2 90/06/09 18:24:51 marking Exp $
 *
 *	This program is offered without any warranty of any kind. It includes
 *	no warranty of merchantability or fitness for any purpose. Testing and
 *	suitability for any use are the sole responsibility of the user.
 * 
 HISTORY
 *	$Log:	gnencode.c $
 * Revision 1.2  90/06/09  18:24:51  marking
 * clean up comments for release
 * 
 * Revision 1.1  89/06/30  17:00:00  marking
 * Initial revision
 * 
 *
 NOTES
 *	1.	There are two versions of encode_word (). Refer to the remark
 *		below and comment-out one or the other.
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
 *	g3sencod.c, g4sencod.c
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

/*
 *	initialize the bit-encoding routines
 */

void initialize_encode ()
{
  static unsigned char msb_mask [8] =
    { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };
  static unsigned char lsb_mask [8] =
    { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
  static unsigned char msb_head_mask [8] =
    { 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF };
  static unsigned char lsb_head_mask [8] =
    { 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF };
  static unsigned char msb_tail_mask [8] =
    { 0xFF, 0x7F, 0x3F, 0x1F, 0x0F, 0x07, 0x03, 0x01 };
  static unsigned char lsb_tail_mask [8] =
    { 0xFF, 0xFE, 0xFC, 0xF8, 0xF0, 0xE0, 0xC0, 0x80 };
  output_fill_order = MSB_FIRST;	/* some implementations might
						want to change this */
  encode_buffer = 0;
  encode_position = 0;
  if (output_fill_order == MSB_FIRST)
  {
    encode_mask = msb_mask;
    encode_head_mask = msb_head_mask;
    encode_tail_mask = msb_tail_mask;
  }
  else /* output_fill_order == LSB_FIRST */
  {
    encode_mask = lsb_mask;
    encode_head_mask = lsb_head_mask;
    encode_tail_mask = lsb_tail_mask;
  }
  pending_black = 0;
  pending_white = 0;
}

/*
 *	append a codeword of the specified length in bits to the end
 *	of the current output file
 */

void encode_word (unsigned short codeword, short length)
{
/*********
	There are two versions of this procedure.  One or the other
	is to be commented out.  The first is somewhat faster,
	especially on a 386 because it makes use of the barrel
	shifter.  Its limitation is the assumption that the output
	file is filled most-significant-bit first.  The second is
	a little slower, but doesn't make the assumption.
********/

/* the faster routine, with the assumption . . . */
  short first_count, second_count, remaining, new_encode_position;
  static char limit_8 [] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8};
  if (length)
  {
    new_encode_position = limit_8 [encode_position + length];
    first_count = new_encode_position - encode_position;
    encode_buffer |= (codeword >> (encode_position + 8));
    encode_position = new_encode_position;
    remaining = length - first_count;
    if (encode_position > 7)
    {
      (*p_encode_next_byte) (encode_buffer);
      if (remaining)
      {
	encode_position = limit_8 [remaining];
	second_count = encode_position;
	remaining -= second_count;
	encode_buffer = ((unsigned char) (codeword >> (8 - first_count)))
		& encode_head_mask [second_count - 1];
	if (encode_position > 7)
	{
	  (*p_encode_next_byte) (encode_buffer);
	  if (remaining)
	  {
	    encode_buffer
	      = ((unsigned char) (codeword << first_count))
		& encode_head_mask [remaining - 1];
	    encode_position = remaining;
	  }
	  else { encode_buffer = 0; encode_position = 0; }
	}
      }
      else { encode_buffer = 0; encode_position = 0; }
    }
  }

/* the slower routine, with no assumption about output file bit order . . . */
/*
  static short read_mask [16] = {0x8000, 0x4000, 0x2000, 0x1000, 0x0800,
    0x0400, 0x0200, 0x0100, 0x0080, 0x0040, 0x0020, 0x0010, 0x0008, 0x0004,
    0x0002, 0x0001};
  short j1;
  for (j1 = 0; j1 < length; j1 ++)
  {
    if (read_mask [j1] & codeword)
      encode_buffer |= encode_mask [encode_position];
    encode_position++;
    if (encode_position > 7)
    {
      (*p_encode_next_byte) (encode_buffer);
      encode_position = 0;
      encode_buffer = 0;
    }
  }
*/
}
/*	end $RCSfile: gnencode.c $ */
