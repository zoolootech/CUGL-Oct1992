/*	$Id: builddec.c 1.2 90/06/09 18:25:13 marking Exp $
 *
 NAME
 *	builddec.c -- build decoding tables for group 3 and group 4 images
 *
 TYPE
 *	C source for main program and subroutines
 *
 SYNOPSIS
 *	builddec
 *
 DESCRIPTION
 *	This program builds tables for decoding group 3 and group 4 encoded
 *	images. The tables are built as the arrays null_mode [] [],
 *	null_mode_next_state [] [], horiz_mode [] [], and
 *	horiz_mode_next_state [] []. The output is C source code which must
 *	be compiled and linked into a decoding program.
 *
 RETURNS
 *	zero if no errors detected, nonzero otherwise
 *
 LEGAL
 *	Copyright 1989, 1990 Michael P. Marking, Post Office Box 8039,
 *	Scottsdale, Arizona 85252-8039. All rights reserved.
 *
 *	License is granted by the copyright holder to distribute and use this
 *	code without payment of royalties or the necessity of notification as
 *	long as this notice (all the text under "LEGAL") is included.
 *
 *	Reference: $Id: builddec.c 1.2 90/06/09 18:25:13 marking Exp $
 *
 *	This program is offered without any warranty of any kind. It includes
 *	no warranty of merchantability or fitness for any purpose. Testing and
 *	suitability for any use are the sole responsibility of the user.
 * 
 HISTORY
 *	$Log:	builddec.c $
 * Revision 1.2  90/06/09  18:25:13  marking
 * clean up comments for release
 * 
 * Revision 1.1  89/06/30  17:00:00  marking
 * Initial revision
 * 
 * 
 NOTES
 *	1.	Since it is expected that this program will be run
 *		infrequently, there is no attempt at optimization.
 *	2.	The tables horiz_mode [] [] and horiz_mode_next_state [] []
 *		are used in both group 3 and group 4 decoding. The tables
 *		null_mode [] [] and null_mode_next_state [] [] are used only
 *		in decoding group 4.
 *	3.	On an XT, this can take around 15 minutes. The progress
 *		messages it displays let you know it's still alive, but
 *		otherwise can be ignored.
 *	4.	Most of the documentation for the tables themselves is in
 *		the decode routines g3tdecod.c and g4tdecod.c.
 *
 FILES
 *	Creates the file "tables.c", which is to be compiled and linked into
 *	the table-driven decoding routine.
 *
 PORTABILITY
 *	Tested under Microsoft C 5.1. Should be fairly portable.
 *
 SEE ALSO
 *	g3tdecod.c -- decode group 3 image using tables
 *	g4tdecod.c -- decode group 4 image using tables
 *	"Decoding Group 3 Images", C Users Journal, June 1990
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

/*
 *	standard headers
 */

#include	<stddef.h>		/* common types and macros */
#include	<stdio.h>		/* streams and files */
#include	<stdlib.h>		/* assorted functions, macros, types */

/*
 *	application
 */

#include "g3g4.h"			/* some #defines */

#define INVALID_CODE -1
#define INCOMPLETE_CODE -2
#define EOL_CODE -3

unsigned long append_0 (unsigned long);
unsigned long append_1 (unsigned long);
short black_run_length (unsigned long);
short search_run_length_table (unsigned long, long *);
short white_run_length (unsigned long);

unsigned long append_0 (unsigned long prefix)
{
  return (prefix + 0x10000);
}

unsigned long append_1 (unsigned long prefix)
{
  unsigned short prefix_length;
  static unsigned short prefix_mask [16] = {0x8000, 0x4000, 0x2000, 0x1000,
    0x0800, 0x0400, 0x0200, 0x0100, 0x0080, 0x0040, 0x0020, 0x0010, 0x0008,
    0x0004, 0x0002, 0x0001};
  prefix_length = 0xFF & (unsigned short) (prefix >> 16);
  return (prefix + 0x10000 + prefix_mask [prefix_length]);
}

short search_run_length_table (unsigned long prefix, long *p_table)
{
  short table_offset = 0;
  long prefix_length, prefix_value;
  prefix_length = 0xFF & (prefix >> 16);
  prefix_value = 0xFFFF & prefix;
  while (p_table [table_offset])
  {
    if (p_table [table_offset] == prefix_length
      && p_table [table_offset + 1] == prefix_value)
	return ((short) p_table [table_offset + 2]);
    table_offset += 3; /* move on to next entry */
  }
  return (INCOMPLETE_CODE); /* no entry found in table */
}

short white_run_length (unsigned long prefix)
{
  static long code_table [] =
  {
    8, 0x3500, 0, /* 0011 0101 */
    6, 0x1C00, 1, /* 0001 11 */
    4, 0x7000, 2, /* 0111 */
    4, 0x8000, 3, /* 1000 */
    4, 0xB000, 4, /* 1011 */
    4, 0xC000, 5, /* 1100 */
    4, 0xE000, 6, /* 1110 */
    4, 0xF000, 7, /* 1111 */
    5, 0x9800, 8, /* 1001 1 */
    5, 0xA000, 9, /* 1010 0 */
    5, 0x3800, 10, /* 0011 1 */
    5, 0x4000, 11, /* 0100 0 */
    6, 0x2000, 12, /* 0010 00 */
    6, 0x0C00, 13, /* 0000 11 */
    6, 0xD000, 14, /* 1101 00 */
    6, 0xD400, 15, /* 1101 01 */
    6, 0xA800, 16, /* 1010 10 */
    6, 0xAC00, 17, /* 1010 11 */
    7, 0x4E00, 18, /* 0100 111 */
    7, 0x1800, 19, /* 0001 100 */
    7, 0x1000, 20, /* 0001 000 */
    7, 0x2E00, 21, /* 0010 111 */
    7, 0x0600, 22, /* 0000 011 */
    7, 0x0800, 23, /* 0000 100 */
    7, 0x5000, 24, /* 0101 000 */
    7, 0x5600, 25, /* 0101 011 */
    7, 0x2600, 26, /* 0010 011 */
    7, 0x4800, 27, /* 0100 100 */
    7, 0x3000, 28, /* 0011 000 */
    8, 0x0200, 29, /* 0000 0010 */
    8, 0x0300, 30, /* 0000 0011 */
    8, 0x1A00, 31, /* 0001 1010 */
    8, 0x1B00, 32, /* 0001 1011 */
    8, 0x1200, 33, /* 0001 0010 */
    8, 0x1300, 34, /* 0001 0011 */
    8, 0x1400, 35, /* 0001 0100 */
    8, 0x1500, 36, /* 0001 0101 */
    8, 0x1600, 37, /* 0001 0110 */
    8, 0x1700, 38, /* 0001 0111 */
    8, 0x2800, 39, /* 0010 1000 */
    8, 0x2900, 40, /* 0010 1001 */
    8, 0x2A00, 41, /* 0010 1010 */
    8, 0x2B00, 42, /* 0010 1011 */
    8, 0x2C00, 43, /* 0010 1100 */
    8, 0x2D00, 44, /* 0010 1101 */
    8, 0x0400, 45, /* 0000 0100 */
    8, 0x0500, 46, /* 0000 0101 */
    8, 0x0A00, 47, /* 0000 1010 */
    8, 0x0B00, 48, /* 0000 1011 */
    8, 0x5200, 49, /* 0101 0010 */
    8, 0x5300, 50, /* 0101 0011 */
    8, 0x5400, 51, /* 0101 0100 */
    8, 0x5500, 52, /* 0101 0101 */
    8, 0x2400, 53, /* 0010 0100 */
    8, 0x2500, 54, /* 0010 0101 */
    8, 0x5800, 55, /* 0101 1000 */
    8, 0x5900, 56, /* 0101 1001 */
    8, 0x5A00, 57, /* 0101 1010 */
    8, 0x5B00, 58, /* 0101 1011 */
    8, 0x4A00, 59, /* 0100 1010 */
    8, 0x4B00, 60, /* 0100 1011 */
    8, 0x3200, 61, /* 0011 0010 */
    8, 0x3300, 62, /* 0011 0011 */
    8, 0x3400, 63, /* 0011 0100 */
    5, 0xD800, 64, /* 1101 1 */
    5, 0x9000, 128, /* 1001 0 */
    6, 0x5C00, 192, /* 0101 11 */
    7, 0x6E00, 256, /* 0110 111 */
    8, 0x3600, 320, /* 0011 0110 */
    8, 0x3700, 384, /* 0011 0111 */
    8, 0x6400, 448, /* 0110 0100 */
    8, 0x6500, 512, /* 0110 0101 */
    8, 0x6800, 576, /* 0110 1000 */
    8, 0x6700, 640, /* 0110 0111 */
    9, 0x6600, 704, /* 0110 0110 0 */
    9, 0x6680, 768, /* 0110 0110 1 */
    9, 0x6900, 832, /* 0110 1001 0 */
    9, 0x6980, 896, /* 0110 1001 1 */
    9, 0x6A00, 960, /* 0110 1010 0 */
    9, 0x6A80, 1024, /* 0110 1010 1 */
    9, 0x6B00, 1088, /* 0110 1011 0 */
    9, 0x6B80, 1152, /* 0110 1011 1 */
    9, 0x6C00, 1216, /* 0110 1100 0 */
    9, 0x6C80, 1280, /* 0110 1100 1 */
    9, 0x6D00, 1344, /* 0110 1101 0 */
    9, 0x6D80, 1408, /* 0110 1101 1 */
    9, 0x4C00, 1472, /* 0100 1100 0 */
    9, 0x4C80, 1536, /* 0100 1100 1 */
    9, 0x4D00, 1600, /* 0100 1101 0 */
    6, 0x6000, 1664, /* 0110 00 */
    9, 0x4D80, 1728, /* 0100 1101 1 */
    11, 0x0100, 1792, /* 0000 0001 000 */
    11, 0x0180, 1856, /* 0000 0001 100 */
    11, 0x01A0, 1920, /* 0000 0001 101 */
    12, 0x0120, 1984, /* 0000 0001 0010 */
    12, 0x0130, 2048, /* 0000 0001 0011 */
    12, 0x0140, 2112, /* 0000 0001 0100 */
    12, 0x0150, 2176, /* 0000 0001 0101 */
    12, 0x0160, 2240, /* 0000 0001 0110 */
    12, 0x0170, 2304, /* 0000 0001 0111 */
    12, 0x01C0, 2368, /* 0000 0001 1100 */
    12, 0x01D0, 2432, /* 0000 0001 1101 */
    12, 0x01E0, 2496, /* 0000 0001 1110 */
    12, 0x01F0, 2560, /* 0000 0001 1111 */
    12, 0x0010, EOL_CODE, /* 0000 0000 0001 */
    9, 0x0080, INVALID_CODE, /* 0000 0000 1 */
    10, 0x0040, INVALID_CODE, /* 0000 0000 01 */
    11, 0x0020, INVALID_CODE, /* 0000 0000 001 */
    12, 0x0000, INVALID_CODE, /* 0000 0000 0000 */
    0 /* end-of-table */
  };
  return (search_run_length_table (prefix, code_table));
}
   
short black_run_length (unsigned long prefix)
{
  static long code_table [] =
  {
    10, 0x0DC0, 0, /* 0000 1101 11 */
    3, 0x4000, 1, /* 010 */
    2, 0xC000, 2, /* 11 */
    2, 0x8000, 3, /* 10 */
    3, 0x6000, 4, /* 011 */
    4, 0x3000, 5, /* 0011 */
    4, 0x2000, 6, /* 0010 */
    5, 0x1800, 7, /* 0001 1 */
    6, 0x1400, 8, /* 0001 01 */
    6, 0x1000, 9, /* 0001 00 */
    7, 0x0800, 10, /* 0000 100 */
    7, 0x0A00, 11, /* 0000 101 */
    7, 0x0E00, 12, /* 0000 111 */
    8, 0x0400, 13, /* 0000 0100 */
    8, 0x0700, 14, /* 0000 0111 */
    9, 0x0C00, 15, /* 0000 1100 0 */
    10, 0x05C0, 16, /* 0000 0101 11 */
    10, 0x0600, 17, /* 0000 0110 00 */
    10, 0x0200, 18, /* 0000 0010 00 */
    11, 0x0CE0, 19, /* 0000 1100 111 */
    11, 0x0D00, 20, /* 0000 1101 000 */
    11, 0x0D80, 21, /* 0000 1101 100 */
    11, 0x06E0, 22, /* 0000 0110 111 */
    11, 0x0500, 23, /* 0000 0101 000 */
    11, 0x02E0, 24, /* 0000 0010 111 */
    11, 0x0300, 25, /* 0000 0011 000 */
    12, 0x0CA0, 26, /* 0000 1100 1010 */
    12, 0x0CB0, 27, /* 0000 1100 1011 */
    12, 0x0CC0, 28, /* 0000 1100 1100 */
    12, 0x0CD0, 29, /* 0000 1100 1101 */
    12, 0x0680, 30, /* 0000 0110 1000 */
    12, 0x0690, 31, /* 0000 0110 1001 */
    12, 0x06A0, 32, /* 0000 0110 1010 */
    12, 0x06B0, 33, /* 0000 0110 1011 */
    12, 0x0D20, 34, /* 0000 1101 0010 */
    12, 0x0D30, 35, /* 0000 1101 0011 */
    12, 0x0D40, 36, /* 0000 1101 0100 */
    12, 0x0D50, 37, /* 0000 1101 0101 */
    12, 0x0D60, 38, /* 0000 1101 0110 */
    12, 0x0D70, 39, /* 0000 1101 0111 */
    12, 0x06C0, 40, /* 0000 0110 1100 */
    12, 0x06D0, 41, /* 0000 0110 1101 */
    12, 0x0DA0, 42, /* 0000 1101 1010 */
    12, 0x0DB0, 43, /* 0000 1101 1011 */
    12, 0x0540, 44, /* 0000 0101 0100 */
    12, 0x0550, 45, /* 0000 0101 0101 */
    12, 0x0560, 46, /* 0000 0101 0110 */
    12, 0x0570, 47, /* 0000 0101 0111 */
    12, 0x0640, 48, /* 0000 0110 0100 */
    12, 0x0650, 49, /* 0000 0110 0101 */
    12, 0x0520, 50, /* 0000 0101 0010 */
    12, 0x0530, 51, /* 0000 0101 0011 */
    12, 0x0240, 52, /* 0000 0010 0100 */
    12, 0x0370, 53, /* 0000 0011 0111 */
    12, 0x0380, 54, /* 0000 0011 1000 */
    12, 0x0270, 55, /* 0000 0010 0111 */
    12, 0x0280, 56, /* 0000 0010 1000 */
    12, 0x0580, 57, /* 0000 0101 1000 */
    12, 0x0590, 58, /* 0000 0101 1001 */
    12, 0x02B0, 59, /* 0000 0010 1011 */
    12, 0x02C0, 60, /* 0000 0010 1100 */
    12, 0x05A0, 61, /* 0000 0101 1010 */
    12, 0x0660, 62, /* 0000 0110 0110 */
    12, 0x0670, 63, /* 0000 0110 0111 */
    10, 0x03C0, 64, /* 0000 0011 11 */
    12, 0x0C80, 128, /* 0000 1100 1000 */
    12, 0x0C90, 192, /* 0000 1100 1001 */
    12, 0x05B0, 256, /* 0000 0101 1011 */
    12, 0x0330, 320, /* 0000 0011 0011 */
    12, 0x0340, 384, /* 0000 0011 0100 */
    12, 0x0350, 448, /* 0000 0011 0101 */
    13, 0x0360, 512, /* 0000 0011 0110 0 */
    13, 0x0368, 576, /* 0000 0011 0110 1 */
    13, 0x0250, 640, /* 0000 0010 0101 0 */
    13, 0x0258, 704, /* 0000 0010 0101 1 */
    13, 0x0260, 768, /* 0000 0010 0110 0 */
    13, 0x0268, 832, /* 0000 0010 0110 1 */
    13, 0x0390, 896, /* 0000 0011 1001 0 */
    13, 0x0398, 960, /* 0000 0011 1001 1 */
    13, 0x03A0, 1024, /* 0000 0011 1010 0 */
    13, 0x03A8, 1088, /* 0000 0011 1010 1 */
    13, 0x03B0, 1152, /* 0000 0011 1011 0 */
    13, 0x03B8, 1216, /* 0000 0011 1011 1 */
    13, 0x0290, 1280, /* 0000 0010 1001 0 */
    13, 0x0298, 1344, /* 0000 0010 1001 1 */
    13, 0x02A0, 1408, /* 0000 0010 1010 0 */
    13, 0x02A8, 1472, /* 0000 0010 1010 1 */
    13, 0x02D0, 1536, /* 0000 0010 1101 0 */
    13, 0x02D8, 1600, /* 0000 0010 1101 1 */
    13, 0x0320, 1664, /* 0000 0011 0010 0 */
    13, 0x0328, 1728, /* 0000 0011 0010 1 */
    11, 0x0100, 1792, /* 0000 0001 000 */
    11, 0x0180, 1856, /* 0000 0001 100 */
    11, 0x01A0, 1920, /* 0000 0001 101 */
    12, 0x0120, 1984, /* 0000 0001 0010 */
    12, 0x0130, 2048, /* 0000 0001 0011 */
    12, 0x0140, 2112, /* 0000 0001 0100 */
    12, 0x0150, 2176, /* 0000 0001 0101 */
    12, 0x0160, 2240, /* 0000 0001 0110 */
    12, 0x0170, 2304, /* 0000 0001 0111 */
    12, 0x01C0, 2368, /* 0000 0001 1100 */
    12, 0x01D0, 2432, /* 0000 0001 1101 */
    12, 0x01E0, 2496, /* 0000 0001 1110 */
    12, 0x01F0, 2560, /* 0000 0001 1111 */
    12, 0x0010, EOL_CODE, /* 0000 0000 0001 */
    9, 0x0080, INVALID_CODE, /* 0000 0000 1 */
    10, 0x0040, INVALID_CODE, /* 0000 0000 01 */
    11, 0x0020, INVALID_CODE, /* 0000 0000 001 */
    12, 0x0000, INVALID_CODE, /* 0000 0000 0000 */
    0 /* end-of-table */
  };
  return (search_run_length_table (prefix, code_table));
}

#define NULL_MODE_PREFIX_LIMIT 200 /* maximum number of null-mode prefixes */
#define HORIZ_MODE_PREFIX_LIMIT 250 /* maximum number of indigestible
  1-dimensional prefixes */

long null_mode_prefix [NULL_MODE_PREFIX_LIMIT];
  /* the bit string corresponding to this row of the decoding table */
unsigned char null_mode [NULL_MODE_PREFIX_LIMIT] [256];
  /* one of the entries PASS_MODE, HORIZONTAL_MODE, etc, or zero */
unsigned char null_mode_next_state [NULL_MODE_PREFIX_LIMIT] [256];
  /* next row of the decoding tables to be used */
short null_mode_prefix_count = 0;
  /* number of prefixes or rows in the G4 decoding tables */

/* decoding script values for horiz_mode [] []:
     0 - indigestible code
     1 - invalid code or error
     2..105 - white runs
     106..209 - black runs
     210 - EOL (valid only for Group 3)
     211..255 - (undefined) */
unsigned char horiz_mode [HORIZ_MODE_PREFIX_LIMIT] [256];
unsigned char horiz_mode_next_state [HORIZ_MODE_PREFIX_LIMIT] [256];
  /* if the corresponding horiz_mode [] [] entry is zero ("indigestible"),
     this entry is a row number for decoding the next byte; otherwise, it
     is the bit number to continue coding the next codeword */
long horiz_mode_prefix [HORIZ_MODE_PREFIX_LIMIT];
  /* the prefixes corresponding to the rows of the decoding table */
char horiz_mode_color [HORIZ_MODE_PREFIX_LIMIT];
  /* color of next run, BLACK or WHITE */
short horiz_mode_prefix_count = 0;

static unsigned char bit_mask [8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04,
  0x02, 0x01};

void build_null_mode_tables (void);
short find_horiz_mode_prefix (long, char);
short find_null_mode_prefix (long);
short null_mode_type (long);
void process_horiz_mode_prefixes (void);
short horiz_mode_code_black (short);
short horiz_mode_code_invalid (void);
short horiz_mode_code_indigestible (void);
short horiz_mode_code_EOL (void);
short horiz_mode_code_white (short);
void write_tables (void);

int main (int arg_count, char **p_args, char **p_env)
{
  printf ("g4build\n");
  /* build the null mode decoding tables */
    build_null_mode_tables ();
    printf ("  %hd null mode prefixes defined\n", null_mode_prefix_count);
  /* build the 1D decoding tables */
    printf ("  building 1D scripts...\n");
    process_horiz_mode_prefixes ();
    printf ("  %hd indigestible prefixes defined\n", horiz_mode_prefix_count);
  /* create tables.h */
    write_tables ();
  exit (0);
}

void build_null_mode_tables ()
{
  short prefix_number;
  /* note: the first eight entries correspond to a null prefix and starting
    bit numbers 0, 1, ... 7 */
  null_mode_prefix_count = 8;
  for (prefix_number = 0; prefix_number < null_mode_prefix_count;
   prefix_number++)
  {
    short byte_value;
    for (byte_value = 0; byte_value < 256; byte_value++)
    {
      short beginning_bit_number, bit_number, mode;
      long working_prefix;
      char found_code = 0;
      if (prefix_number < 8)
      {
        working_prefix = 0L;
        beginning_bit_number = prefix_number;
      }
      else
      {
        working_prefix = null_mode_prefix [prefix_number];
        beginning_bit_number = 0;
      }
      for (bit_number = beginning_bit_number; bit_number < 8 && !found_code;
       bit_number++)
      {
        if (bit_mask [bit_number] & byte_value)
	  working_prefix = append_1 (working_prefix);
	else working_prefix = append_0 (working_prefix);
        mode = null_mode_type (working_prefix);
        switch (mode)
        {
	  case PASS_MODE:
	  case HORIZONTAL_MODE:
	  case VERTICAL_V0_MODE:
	  case VERTICAL_VR1_MODE:
	  case VERTICAL_VR2_MODE:
	  case VERTICAL_VR3_MODE:
	  case VERTICAL_VL1_MODE:
	  case VERTICAL_VL2_MODE:
	  case VERTICAL_VL3_MODE:
	  case EXT_MODE_UNCOMPRESSED:
	  case ERROR_MODE:
	  case ERROR_MODE_1:
	    found_code = 1;
	    null_mode [prefix_number] [byte_value] = (unsigned char) mode;
	    null_mode_next_state [prefix_number] [byte_value]
	      = (unsigned char) ((bit_number + 1) & 0x7);
	      /* note: if the bit number is 8, then
	        the table entry will be zero, which indicates a new byte
	        is to be fetched during the decoding process */
	    break;
	  default:
	    break;
        }
      }
      if (!found_code)
      {
	null_mode_next_state [prefix_number] [byte_value]
	  = (unsigned char) find_null_mode_prefix (working_prefix);
	null_mode [prefix_number] [byte_value] = 0; /* indicating to the
	  decoder that no digestible state was found */
      }
    }
  }
}

short find_null_mode_prefix (long prefix)
{
  short j1;
  if (prefix == 0L) return (0);
  for (j1 = 8; j1 < null_mode_prefix_count; j1++)
    if (prefix == null_mode_prefix [j1]) return (j1);
  if (null_mode_prefix_count == NULL_MODE_PREFIX_LIMIT)
  {
    printf ("ERROR: null mode prefix table overflow\n");
    exit (1);
  }
  null_mode_prefix [null_mode_prefix_count] = prefix;
printf ("adding null mode prefix [%hd] 0x%lx\n", null_mode_prefix_count,
prefix);
  null_mode_prefix_count++;
  return (null_mode_prefix_count - 1);
}

short find_horiz_mode_prefix (long prefix, char color)
{
  short j1;
  for (j1 = 0; j1 < horiz_mode_prefix_count; j1++)
    if (prefix == horiz_mode_prefix [j1] && horiz_mode_color [j1] == color)
      return (j1);
  /* it wasn't found, so add it to the tables */
  /* but first, is there room? */
    if (horiz_mode_prefix_count == HORIZ_MODE_PREFIX_LIMIT) /* we're full */
    {
      printf ("ERROR: 1D prefix table overflow\n");
      exit (1);
    }
  /* OK, there's room... */
  horiz_mode_prefix [horiz_mode_prefix_count] = prefix;
  horiz_mode_color [horiz_mode_prefix_count] = color;
  horiz_mode_prefix_count++;
printf ("\n horiz mode prefix %hd, color %c = 0x%lx ",
(short) (horiz_mode_prefix_count - 1), "WB" [color], prefix);
  return (horiz_mode_prefix_count - 1);
}

short null_mode_type (long prefix)
{
  if (prefix == 0x18000L) return (VERTICAL_V0_MODE);      /* 1 */
  if (prefix == 0x36000L) return (VERTICAL_VR1_MODE);     /* 011 */
  if (prefix == 0x34000L) return (VERTICAL_VL1_MODE);     /* 010 */
  if (prefix == 0x32000L) return (HORIZONTAL_MODE);       /* 001 */
  if (prefix == 0x41000L) return (PASS_MODE);             /* 0001 */
  if (prefix == 0x60C00L) return (VERTICAL_VR2_MODE);     /* 0000 11 */
  if (prefix == 0x60800L) return (VERTICAL_VL2_MODE);     /* 0000 10 */
  if (prefix == 0x70600L) return (VERTICAL_VR3_MODE);     /* 0000 011 */
  if (prefix == 0x70400L) return (VERTICAL_VL3_MODE);     /* 0000 010 */
  if (prefix == 0x80200L) return (ERROR_MODE);            /* 0000 0010 */
  if (prefix == 0x90300L) return (ERROR_MODE);            /* 0000 0011 0 */
  if (prefix == 0xA0380L) return (ERROR_MODE);            /* 0000 0011 10 */
  if (prefix == 0xA03C0L) return (EXT_MODE_UNCOMPRESSED); /* 0000 0011 11 */
  if (prefix == 0x70000L) return (ERROR_MODE_1);          /* 0000 000 */
    /* under the assumption that there are no errors in the file, then this
       bit string can only be the beginning of an EOFB (end-of-facsimile-block)
       code */
  return (-1);
}

void process_horiz_mode_prefixes ()
{
  unsigned short code_byte_value;
  short prefix_number;
  horiz_mode_prefix_count = 16; /* the first 8 are for white, the second 8 are
    for black, beginning with bits 0, 1, ... 7 */
  for (prefix_number = 0; prefix_number < horiz_mode_prefix_count;
    prefix_number++)
    for (code_byte_value = 0; code_byte_value < 256; code_byte_value++)
    {
      short bits_digested = 0;
      short bit_number, beginning_bit_number;
      char working_color;
      long working_prefix;
      if (prefix_number < 8)
      {
	working_color = WHITE;
	working_prefix = 0L;
	beginning_bit_number = prefix_number;
      }
      else if (prefix_number < 16)
      {
	working_color = BLACK;
	working_prefix = 0L;
	beginning_bit_number = prefix_number - 8;
      }
      else
      {
        working_color = horiz_mode_color [prefix_number];
        working_prefix = horiz_mode_prefix [prefix_number];
	beginning_bit_number = 0;
      }
      for (bit_number = beginning_bit_number; bit_number < 8 && !bits_digested;
	bit_number++)
      {
	if (bit_mask [bit_number] & code_byte_value)
	  working_prefix = append_1 (working_prefix);
	else working_prefix = append_0 (working_prefix);
	if (working_prefix == 0xC0000L) working_prefix = 0xB0000L;
	  /* This conversion allows for arbitrary strings of zeroes to precede
	     the end-of-line code 0000 0000 0001.  It assumes no errors in the
	     data, and is based on the assumption that the code replaced (12
	     consecutive zeroes) can only be "legally" encountered before the
	     end-of-line code.  This assumption is valid only for a Group 3
	     image; the combination will never occur in horizontal mode in a
	     proper Group 4 image. */
	if (working_color == WHITE)
	{
	  short runlength;
	  runlength = white_run_length (working_prefix);
	  if (runlength == INVALID_CODE)
	  {
	    horiz_mode [prefix_number] [code_byte_value]
	      = (unsigned char) horiz_mode_code_invalid ();
	    horiz_mode_next_state [prefix_number] [code_byte_value]
	      = (unsigned char) bit_number;
	    bits_digested = bit_number + 1;
	  }
	  else if (runlength == EOL_CODE) /* Group 3 only */
	  {
	    horiz_mode [prefix_number] [code_byte_value]
	      = (unsigned char) horiz_mode_code_EOL ();
	    horiz_mode_next_state [prefix_number] [code_byte_value]
	      = (unsigned char) ((bit_number + 1) & 0x7);
	    bits_digested = bit_number + 1;
	  }
	  else if (runlength != INCOMPLETE_CODE)
	  {
	    horiz_mode [prefix_number] [code_byte_value]
	      = (unsigned char) horiz_mode_code_white (runlength);
	    horiz_mode_next_state [prefix_number] [code_byte_value]
	      = (unsigned char) ((bit_number + 1) & 0x7);
	    bits_digested = bit_number + 1;
	  }
	  /* else incomplete code */
	}
	else /* working_color == BLACK */
	{
	  short runlength;
	  runlength = black_run_length (working_prefix);
	  if (runlength == INVALID_CODE)
	  {
	    horiz_mode [prefix_number] [code_byte_value]
	      = (unsigned char) horiz_mode_code_invalid ();
	    horiz_mode_next_state [prefix_number] [code_byte_value]
	      = (unsigned char) (bit_number + 8);
	    bits_digested = bit_number + 1;
	  }
	  else if (runlength == EOL_CODE) /* Group 3 only */
	  {
	    horiz_mode [prefix_number] [code_byte_value]
	      = (unsigned char) horiz_mode_code_EOL ();
	    horiz_mode_next_state [prefix_number] [code_byte_value]
	      = (unsigned char) ((bit_number + 1) & 0x7);
	    bits_digested = bit_number + 1;
	  }
	  else if (runlength != INCOMPLETE_CODE)
	  {
	    horiz_mode [prefix_number] [code_byte_value]
	      = (unsigned char) horiz_mode_code_black (runlength);
	    horiz_mode_next_state [prefix_number] [code_byte_value]
	      = (unsigned char) ((bit_number + 1) & 0x7);
	    bits_digested = bit_number + 1;
	  }
	  /* else incomplete code */
	}
      }
      if (!bits_digested) /* if no codewords after examining byte */
      {
	horiz_mode [prefix_number] [code_byte_value]
	  = (unsigned char) horiz_mode_code_indigestible ();
	horiz_mode_next_state [prefix_number] [code_byte_value]
	  = (unsigned char) find_horiz_mode_prefix (working_prefix,
	    working_color);
      }
    }
}

short horiz_mode_code_black (short runlength)
{
  /*
	0	106
	1	107
	...	...
	63	169
	64	170
	128	171
	...	...
	2560	209
  */
  if (runlength < 64) return (runlength + 106);
  else return ((runlength / 64) + 169);
}

short horiz_mode_code_invalid ()
{
  return (1);
}

short horiz_mode_code_indigestible ()
{
  return (0);
}

short horiz_mode_code_EOL ()
{
  return (210);
}

short horiz_mode_code_white (short runlength)
{
  /*
	0	2
	1	3
	...	...
	63	65
	64	66
	128	67
	...	...
	2560	105
  */
  if (runlength < 64) return (runlength + 2);
  else return ((runlength / 64) + 65);
}

void write_tables ()
{
  FILE *p_table_file;
  short j1, j2, j3;
  p_table_file = fopen ("tables.c", "w+");
  if (p_table_file == NULL)
  {
    printf ("can't open \"tables.c\"\n");
    exit (1);
  }
  /* ---------------------------------------- null_mode [] [] */
  fprintf (p_table_file,
    "unsigned char null_mode [%hd] [256] = {\n",
    null_mode_prefix_count);
  for (j1 = 0; j1 < null_mode_prefix_count; j1++)
  {
    fprintf (p_table_file, "/* %hd */", j1);
    j3 = 1;
    fprintf (p_table_file, "  { ");
    for (j2 = 0; j2 < 256; j2++)
    {
      fprintf (p_table_file, "%hd", (short) null_mode [j1] [j2]);
      if (j3++ == 14)
      {
	j3 = 0; fprintf (p_table_file, ",\n    ");
      }
      else if (j2 != 255) fprintf (p_table_file, ", ");
    }
    if (j1 == horiz_mode_prefix_count - 1) fprintf (p_table_file, "  }\n");
    else fprintf (p_table_file, "  },\n");
  }
  fprintf (p_table_file, "};\n");
  /* ---------------------------------------- null_mode_next_state [] [] */
  fprintf (p_table_file,
    "unsigned char null_mode_next_state [%hd] [256] = {\n",
    null_mode_prefix_count);
  for (j1 = 0; j1 < null_mode_prefix_count; j1++)
  {
    fprintf (p_table_file, "/* %hd */", j1);
    j3 = 1;
    fprintf (p_table_file, "  { ");
    for (j2 = 0; j2 < 256; j2++)
    {
      fprintf (p_table_file, "%hd", (short) null_mode_next_state [j1] [j2]);
      if (j3++ == 14)
      {
	j3 = 0; fprintf (p_table_file, ",\n    ");
      }
      else if (j2 != 255) fprintf (p_table_file, ", ");
    }
    if (j1 == horiz_mode_prefix_count - 1) fprintf (p_table_file, "  }\n");
    else fprintf (p_table_file, "  },\n");
  }
  fprintf (p_table_file, "};\n");
  /* ---------------------------------------- horiz_mode [] [] */
  fprintf (p_table_file,
    "unsigned char horiz_mode [%hd] [256] = {\n",
    horiz_mode_prefix_count);
  for (j1 = 0; j1 < horiz_mode_prefix_count; j1++)
  {
    j3 = 1;
    fprintf (p_table_file, "/* %hd */", j1);
    fprintf (p_table_file, "  { ");
    for (j2 = 0; j2 < 256; j2++)
    {
      fprintf (p_table_file, "%hd", (short) horiz_mode [j1] [j2]);
      if (j3++ == 11)
      {
	j3 = 0; fprintf (p_table_file, ",\n    ");
      }
      else if (j2 != 255) fprintf (p_table_file, ", ");
    }
    if (j1 == horiz_mode_prefix_count - 1) fprintf (p_table_file, "  }\n");
    else fprintf (p_table_file, "  },\n");
  }
  fprintf (p_table_file, "};\n");
  /* ---------------------------------------- horiz_mode_next_state [] [] */
  fprintf (p_table_file,
    "unsigned char horiz_mode_next_state [%hd] [256] = {\n",
    horiz_mode_prefix_count);
  for (j1 = 0; j1 < horiz_mode_prefix_count; j1++)
  {
    fprintf (p_table_file, "/* %hd */", j1);
    j3 = 1;
    fprintf (p_table_file, "  { ");
    for (j2 = 0; j2 < 256; j2++)
    {
      fprintf (p_table_file, "%hd", (short) horiz_mode_next_state [j1] [j2]);
      if (j3++ == 14)
      {
	j3 = 0; fprintf (p_table_file, ",\n    ");
      }
      else if (j2 != 255) fprintf (p_table_file, ", ");
    }
    if (j1 == horiz_mode_prefix_count - 1) fprintf (p_table_file, "  }\n");
    else fprintf (p_table_file, "  },\n");
  }
  fprintf (p_table_file, "};\n");
  fclose (p_table_file);
}

/*	end $RCSfile: builddec.c $ */
