/*	$Id: g3g4.h 1.2 90/06/09 18:21:50 marking Exp $
 *
 NAME
 *	g3g4.h -- definitions for decoding group 3 and group 4 images
 *
 USED WITH
 *	builddec.c -- build decode tables
 *	group 3 and group 4 encoding and decoding routines
 *
 DESCRIPTION
 *	Common definitions for group 3 and group 4 utilities.
 *
 LEGAL
 *	Copyright 1989, 1990 Michael P. Marking, Post Office Box 8039,
 *	Scottsdale, Arizona 85252-8039. All rights reserved.
 *
 *	License is granted by the copyright holder to distribute and use this
 *	code without payment of royalties or the necessity of notification as
 *	long as this notice (all the text under "LEGAL") is included.
 *
 *	Reference: $Id: g3g4.h 1.2 90/06/09 18:21:50 marking Exp $
 *
 *	This program is offered without any warranty of any kind. It includes
 *	no warranty of merchantability or fitness for any purpose. Testing and
 *	suitability for any use are the sole responsibility of the user.
 *
 PORTABILITY
 *
 *	There is a non-portable use of "global" variables in this file,
 *	about which a minority of compilers will justifiably complain. Certain
 *	variables are declared here without extern keywords. Strictly
 *	speaking, they should be declared extern in all but one module, but
 *	that would require complication of this file. If it gets past your
 *	compiler and linker, you can probably ignore it.
 * 
 HISTORY
 *	$Log:	g3g4.h $
 * Revision 1.2  90/06/09  18:21:50  marking
 * clean up comments for release
 * 
 * Revision 1.1  89/06/30  17:00:00  marking
 * Initial revision
 * 
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

#define WHITE 0
#define BLACK 1

#define NULL_MODE 0
#define PASS_MODE 1
#define HORIZONTAL_MODE 2
#define VERTICAL_V0_MODE 3
#define VERTICAL_VR1_MODE 4
#define VERTICAL_VR2_MODE 5
#define VERTICAL_VR3_MODE 6
#define VERTICAL_VL1_MODE 7
#define VERTICAL_VL2_MODE 8
#define VERTICAL_VL3_MODE 9
#define EXT_MODE_UNCOMPRESSED 10
#define ERROR_MODE 11
#define ERROR_MODE_1 12

#define PASS_CODEWORD		0x1000
#define HORIZONTAL_CODEWORD	0x2000
#define VERTICAL_V0_CODEWORD	0x8000
#define VERTICAL_VR1_CODEWORD	0x6000
#define VERTICAL_VR2_CODEWORD	0x0C00
#define VERTICAL_VR3_CODEWORD	0x0600
#define VERTICAL_VL1_CODEWORD	0x4000
#define VERTICAL_VL2_CODEWORD	0x0800
#define VERTICAL_VL3_CODEWORD	0x0400

#define PASS_CODELEN		4
#define HORIZONTAL_CODELEN	3
#define VERTICAL_V0_CODELEN	1
#define VERTICAL_VR1_CODELEN	3
#define VERTICAL_VR2_CODELEN	6
#define VERTICAL_VR3_CODELEN	7
#define VERTICAL_VL1_CODELEN	3
#define VERTICAL_VL2_CODELEN	6
#define VERTICAL_VL3_CODELEN	7

#define RETURN_OK 0
#define ERROR_NEXT_BYTE 1 /* the routine next_byte () unexpectedly returned
  an error or end-of-file before the end of the image (EOFB) was reached */
#define ERROR_UNSUPPORTED_EXTENSION 2
#define ERROR_INVALID_CODEWORD 3
#define ERROR_PROGRAM_LOGIC 4

#define EOL_CODE -1
#define INVALID_CODE -2

#define MSB_FIRST 1
#define LSB_FIRST 2

#define PAD_NONE 1
#define PAD_BYTE 2
#define PAD_NIBBLE 3
#define PAD_ODD_NIBBLE 4

char  (*p_decode_black) (short);
char  (*p_decode_new_row) (void);
short (*p_decode_next_byte) (void);
char  (*p_decode_white) (short);

char g3i_decode_T (void);
char g3i_decode (void);
char g3i_initialize (short, short);
char g4i_decode (void);
char g4i_initialize (short, short);

char (*p_encode_next_byte) (unsigned char);

char g3j_encode_black (short);
char g3j_encode_white (short);
char g3j_encode_new_row (void);
char g3j_encode_pad (char);
char g3j_initialize (short, short);
char g4j_encode_black (short);
char g4j_encode_white (short);
char g4j_encode_EOFB (void);
char g4j_encode_new_row (void);
char g4j_initialize (short, short);

/* internal stuff */

void encode_word (unsigned short, short);
void initialize_encode (void);
short encode_position;

char output_fill_order, input_fill_order;
unsigned char encode_buffer;
unsigned char *decode_mask, *encode_mask, *encode_head_mask, *encode_tail_mask;
short pending_black, pending_white;

char trace_flag;			/* for debug */

/*	end $RCSfile: g3g4.h $ */
