/* machine.h      Define machine-dependencies for bison,
   copyright (C) 1984 Bob Corbett and Richard Stallman      */


#define  MAXSHORT   32767       /* note: -32768 is a long on DOS compilers */
#define  MINSHORT  -32767

#define  BITS_PER_WORD  16
#define  WORDSIZE(n)  (((n) + 15) / 16)
#define  SETBIT(x, i) ((x)[(i)>>4] |= (1<<((i) & 15)))
