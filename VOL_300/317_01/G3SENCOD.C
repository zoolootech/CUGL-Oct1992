/*	$Id: g3sencod.c 1.2 90/06/09 18:22:52 marking Exp $
 *
 NAME
 *	g3sencod.c -- encode group 3 data using nested if statements
 *
 TYPE
 *	C procedures
 *
 SYNOPSIS
 *	char	g3j_initialize (short image_width, short image_length);
 *	char	g3j_encode_new_row (void);
 *	char	g3j_encode_pad (char pad_type);
 *	char	g3j_encode_black (short);
 *	char	g3j_encode_white (short);
 *
 DESCRIPTION
 *	These routines are used to encode group 3 and group 4 images. (The
 *	encoding of group 4 also requires other routines in a separate file.)
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
 *	Reference: $Id: g3sencod.c 1.2 90/06/09 18:22:52 marking Exp $
 *
 *	This program is offered without any warranty of any kind. It includes
 *	no warranty of merchantability or fitness for any purpose. Testing and
 *	suitability for any use are the sole responsibility of the user.
 * 
 HISTORY
 *	$Log:	g3sencod.c $
 * Revision 1.2  90/06/09  18:22:52  marking
 * clean up comments for release
 * 
 * Revision 1.1  89/06/30  17:00:00  marking
 * Initial revision
 * 
 *
 NOTES
 *	1.	Encode a pad to the next byte to flush the output buffer.
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
 *	g4sencod.c -- encode group 4 image using nested if statements
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
 *
 */

#include "g3g4.h"

static short EOL_code = 0x0010, EOL_length = 12;

static short encode_black_code (short);
static short encode_white_code (short);

char g3j_initialize (short image_width, short image_length)
{
  initialize_encode ();
  return (0);
}

char g3j_encode_new_row (void)
{
  encode_word (EOL_code, EOL_length);
  return (0);
}

char g3j_encode_pad (char pad_type)
{
  short bits_to_pad;
  switch (pad_type)
  {
    case PAD_NONE:
      break;
    case PAD_BYTE:
      if (encode_position) /* if not already on a byte boundary */
	encode_word (0, 8 - encode_position);
      break;
    case PAD_NIBBLE:
      bits_to_pad = (8 - encode_position) & 3;
      if (bits_to_pad) encode_word (0, bits_to_pad);
      break;
    case PAD_ODD_NIBBLE:
      if (encode_position > 0 && encode_position < 4)
      {
	bits_to_pad = 4 - encode_position;
	encode_word (0, bits_to_pad);
      }
      else if (encode_position > 4)
      {
	bits_to_pad = 12 - encode_position;
	encode_word (0, bits_to_pad);
      }
      break;
    default:
      break;
  }
  return (0);
}

char g3j_encode_black (short runlength)
{
  runlength = encode_black_code (runlength);
  while (runlength) runlength = encode_black_code (runlength);
  return (0);
}

static short encode_black_code (short runlength)
{
  static short term_codes [64] =
  {
    0x0DC0, 0x4000, 0xC000, 0x8000, 0x6000, 0x3000,
    0x2000, 0x1800, 0x1400, 0x1000, 0x0800, 0x0A00,
    0x0E00, 0x0400, 0x0700, 0x0C00, 0x05C0, 0x0600,
    0x0200, 0x0CE0, 0x0D00, 0x0D80, 0x06E0, 0x0500,
    0x02E0, 0x0300, 0x0CA0, 0x0CB0, 0x0CC0, 0x0CD0,
    0x0680, 0x0690, 0x06A0, 0x06B0, 0x0D20, 0x0D30,
    0x0D40, 0x0D50, 0x0D60, 0x0D70, 0x06C0, 0x06D0,
    0x0DA0, 0x0DB0, 0x0540, 0x0550, 0x0560, 0x0570,
    0x0640, 0x0650, 0x0520, 0x0530, 0x0240, 0x0370,
    0x0380, 0x0270, 0x0280, 0x0580, 0x0590, 0x02B0,
    0x02C0, 0x05A0, 0x0660, 0x0670
  };
  static short makeup_codes [40] =
  {
    0x03C0, 0x0C80, 0x0C90, 0x05B0, 0x0330, 0x0340,
    0x0350, 0x0360, 0x0368, 0x0250, 0x0258, 0x0260,
    0x0268, 0x0390, 0x0398, 0x03A0, 0x03A8, 0x03B0,
    0x03B8, 0x0290, 0x0298, 0x02A0, 0x02A8, 0x02D0,
    0x02D8, 0x0320, 0x0328, 0x0100, 0x0180, 0x01A0,
    0x0120, 0x0130, 0x0140, 0x0150, 0x0160, 0x0170,
    0x01C0, 0x01D0, 0x01E0, 0x01F0
  };
  static short term_lengths [64] =
  {
    10, 3, 2, 2, 3, 4, 4, 5, 6, 6, 7, 7, 7, 8, 8, 9, 10, 10,
    10, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 12, 12
  };
  static short makeup_lengths [40] =
  {
    10, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
    13, 13, 13, 13, 13, 13, 13, 13, 13, 11, 11, 11, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12
  };
  short remaining;
  if (runlength < 64)
  {
    encode_word (term_codes [runlength], term_lengths [runlength]);
    return (0);
  }
  else if (runlength < 2561)
  {
    short x;
    x = (runlength - 64) / 64;
    encode_word (makeup_codes [x], makeup_lengths [x]);
    remaining = runlength - (64 * (1 + x));
    if (!remaining) encode_word (term_codes [0], term_lengths [0]);
    return (remaining);
  }
  else
  {
    encode_word (makeup_codes [39], makeup_lengths [39]);
    return (runlength - 2560);
  }
}

char g3j_encode_white (short runlength)
{
  runlength = encode_white_code (runlength);
  while (runlength) runlength = encode_white_code (runlength);
  return (0);
}

static short encode_white_code (short runlength)
{
  static short term_codes [64] =
  {
    0x3500, 0x1c00, 0x7000, 0x8000, 0xb000, 0xc000,
    0xe000, 0xf000, 0x9800, 0xa000, 0x3800, 0x4000,
    0x2000, 0x0c00, 0xd000, 0xd400, 0xa800, 0xac00,
    0x4e00, 0x1800, 0x1000, 0x2e00, 0x0600, 0x0800,
    0x5000, 0x5600, 0x2600, 0x4800, 0x3000, 0x0200,
    0x0300, 0x1a00, 0x1b00, 0x1200, 0x1300, 0x1400,
    0x1500, 0x1600, 0x1700, 0x2800, 0x2900, 0x2a00,
    0x2b00, 0x2c00, 0x2d00, 0x0400, 0x0500, 0x0a00,
    0x0b00, 0x5200, 0x5300, 0x5400, 0x5500, 0x2400,
    0x2500, 0x5800, 0x5900, 0x5a00, 0x5b00, 0x4a00,
    0x4b00, 0x3200, 0x3300, 0x3400
  };
  static short makeup_codes [40] =
  {
    0xd800, 0x9000, 0x5c00, 0x6e00, 0x3600, 0x3700,
    0x6400, 0x6500, 0x6800, 0x6700, 0x6600, 0x6680,
    0x6900, 0x6980, 0x6a00, 0x6a80, 0x6b00, 0x6b80,
    0x6c00, 0x6c80, 0x6d00, 0x6d80, 0x4c00, 0x4c80,
    0x4d00, 0x6000, 0x4d80, 0x0100, 0x0180, 0x01a0,
    0x0120, 0x0130, 0x0140, 0x0150, 0x0160, 0x0170,
    0x01c0, 0x01d0, 0x01e0, 0x01f0
  };
  static short term_lengths [64] =
  {
    8, 6, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8
  };
  static short makeup_lengths [40] =
  {
    5, 5, 6, 7, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
    9, 6, 9, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12
  };
  short remaining;
  if (runlength < 64)
  {
    encode_word (term_codes [runlength], term_lengths [runlength]);
    return (0);
  }
  else if (runlength < 2561)
  {
    short x;
    x = (runlength - 64) / 64;
    encode_word (makeup_codes [x], makeup_lengths [x]);
    remaining = runlength - (64 * (1 + x));
    if (!remaining) encode_word (term_codes [0], term_lengths [0]);
    return (remaining);
  }
  else
  {
    encode_word (makeup_codes [39], makeup_lengths [39]);
    return (runlength - 2560);
  }
}

/*	end $RCSfile: g3sencod.c $ */
