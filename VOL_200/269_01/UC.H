/***************************************************************************
 *                                                                         *
 *                             uc.h                                        *
 *                                                                         *
 *      The unicorn library header for version 4.0                         *
 *                                                                         *
 ***************************************************************************/

 /*  alias data types  */

#define META    short
#define UCHAR   unsigned char
#define UINT    unsigned int
#define ULONG   unsigned long
#define USHORT  unsigned short

/*  Boolean data type  */

typedef enum
{
   FALSE, TRUE
} BOOLEAN;

/* Return values and exit codes */

#define OK       0
#define BAD      1
#define SUCCESS  0
#define FAILURE  1

/*  infinite loop declaration */

#define FOREVER   1

/* masks */

#define HIBYTE    0xff00
#define LOBYTE    0x00ff
#define HI_NIBBLE 0x0f
#define LO_NIBBLE 0xf0
#define ASCII     0x7f
#define HIBIT     0x80

/* data lengths */

#define MAXNAME  8
#define MAXEXT   3
#define MAXLINE  256
#define MAXPATH  64

/* special values */

#define BIGGEST  65535




