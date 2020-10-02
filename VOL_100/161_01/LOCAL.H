/* local.h - definitions for use with
 *      Learning to Program in C
 */
#ifndef LOCAL_H
#define LOCAL_H
#include <stdio.h>
#define FAIL		1
#define FOREVER		for (;;)
#define NO			0
#define STDERR		2
#define STDIN		0
#define STDOUT		1
#define SUCCEED		0
#define YES			1
#define bits		ushort
#define bool		int
#define metachar	short
#define tbool		char
#define ushort      unsigned  /* use unsigned short, if you can */
#define void		int		/* delete, if compiler supports void */
#define getln(s, n) ((fgets(s, n, stdin)==NULL) ? EOF : strlen(s))
#define ABS(x)		(((x) < 0) ? -(x) : (x))
#define MAX(x, y)	(((x) < (y)) ? (y) : (x))
#define MIN(x, y)	(((x) < (y)) ? (x) : (y))

/* new material - include standard fn declarations */
#include <ctype.h>
#include <math.h>

#define TRUE 1
#define FALSE 0

#define DIM(a) (sizeof(a) / sizeof(a[0]))
#define SWAP(a, b, t) (t = a, a = b, b = t)
#define LOOPDN(r, n) for (r = n+1; --r > 0; )
#ifndef NDEBUG
#define asserts(cond, str) \
	{ if (!(cond)) fprintf(stderr, "Assertion '%s' failed\n", str); }
#else
#define asserts(e, s)
#endif
typedef char *data_ptr;
typedef unsigned size_t;
typedef unsigned index_t;
#define STREQ(s, t) (strcmp(s, t) == 0)
#define STRLT(s, t) (strcmp(s, t) < 0)
#define STRGT(s, t) (strcmp(s, t) > 0)
#define CHAR_BIT 8
#endif
