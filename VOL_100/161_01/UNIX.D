###alltyp.sh
set -x

cc -o groups groups.c libtim -lm

Makex dadd; groups <dadd.out >dadd.grp
Makex dand; groups <dand.out >dand.grp
Makex dban; groups <dban.out >dban.grp
Makex dbor; groups <dbor.out >dbor.grp
Makex ddiv; groups <ddiv.out >ddiv.grp
Makex deor; groups <deor.out >deor.grp
Makex deq; groups <deq.out >deq.grp
Makex dlsh; groups <dlsh.out >dlsh.grp
Makex dlt; groups <dlt.out >dlt.grp
Makex dmul; groups <dmul.out >dmul.grp
Makex drem; groups <drem.out >drem.grp
Makex drsh; groups <drsh.out >drsh.grp
Makex dsub; groups <dsub.out >dsub.grp
Makex uinc; groups <uinc.out >uinc.grp
Makex uneg; groups <uneg.out >uneg.grp
Makex upin; groups <upin.out >upin.grp
###do_all.sh
Install.sh
Mkavg.sh
Alltyp.sh
Runmost.sh
Runmost2.sh
###install.sh
set -x

cc -c calib.c \
	begint.c \
	cputim.c \
	returnp.c \
	error.c \
	fround.c \
	intreg.c \
	sstat.c \
	sauto.c \
	lauto.c \
	dauto.c \

ar qc libtim begint.o cputim.o returnp.o fround.o error.o
cc -o calib.x calib.c libtim -lm
calib.x > parms.dat

Makex intreg
Makex sstat
Makex sauto
Makex lauto
Makex dauto

cc -o fmttbl fmttbl.c libtim
fmttbl intreg.out sstat.out sauto.out lauto.out dauto.out
###makex.
cc -o $1.x $1.c libtim -lm
echo $1:
$1.x >$1.out
mv $1.x Previous.x
###mk.qso
echo "Jim - this file also transfers local.h which i forgot to tell you about"
cat >local.h <<!EOF
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
!EOF
cc -o qsortrcm.x qsortrcm.c libtim
qsortrcm.x 100 >qsortrcm.out
qsortrcm.x 1000 >>qsortrcm.out
qsortrcm.x 10000 >>qsortrcm.out
cc -o qsortitm.x qsortitm.c libtim
qsortitm.x 100 >qsortitm.out
qsortitm.x 1000 >>qsortitm.out
qsortitm.x 10000 >>qsortitm.out

cc -p -o qsortpro.x qsortpro.c 
qsortpro.x  >>qsortpro.out
prof qsortpro.x >>qsortpro.out
###runmost.sh
set -x

Makex fncall
Makex loops1
Makex loops2
Makex 102
Makex 104
Makex 105
Makex 106
Makex 401
Makex 402
Makex 403
Makex 405
Makex 411
Makex 413
Makex 501
Makex 503
Makex 504
Makex 505
Makex 506
Makex 507
Makex 508
Makex 601
###runmost2.sh
set -x

Makex switch

Makex ctypelib
Makex fdlib
Makex mathlib
Makex strlib
Makex stdlib
Makex stdiolib

cc -o harness1.x harness1.c; echo harness1; time harness1.x
cc -o harness2.x harness2.c; echo harness2; time harness2.x
cc -o harness3.x harness3.c; echo harness3; time harness3.x
cc -o harness4.x harness4.c libtim; echo harness4; time harness4.x

Makex optimtst
###config.h
/* config.h - configuration data for each environment
 *		This file should either be customized to the intended
 *		compiler/machine environment, or parameterized via -D flags
 *		on the compile step:
 *		-D USHORT	if compiler supports unsigned short
 *		-D UTINY	if compiler supports unsigned char
 *		-D VOID		if compiler supports void
 *		-D ULONG	if compiler supports unsigned LONG
 */
#define UTINY	/* UNIX System 3 ff */
#define USHORT	/* UNIX System 3 ff */
#define ULONG	/* UNIX System 3 ff */
#define VOID	/* UNIX System 3 ff */
#ifdef USHORT
typedef unsigned short ushort;
#else
typedef unsigned ushort;	/* assumes short == int */
#endif
#ifdef UTINY
typedef unsigned char utiny;
#endif
#ifdef ULONG
typedef unsigned long ulong;
#endif
#ifndef VOID
#define void int
#endif
#define MINSAM 2e6		/* minimum timing sample (usec) */
#define NTRY 100		/* maximum no of code excerpts per file */
#define FR_ANCHOR arg1	/* what is the "frame anchor": arg1 or var1 */
#define RETOFFSET (-1)	/* offset of return pointer
						   relative to frame anchor */
#define JUNKFILE "/dev/null"
###cputim.c
/* cputim - UNIX version
 */


#define UTIME (tbufb.ut - tbufa.ut + tbufb.cut - tbufa.cut)
#define STIME (tbufb.st - tbufa.st + tbufb.cst - tbufa.cst)

typedef struct tbuf {	/* UNIX V7 ff   */
	long ut, st;
	long cut, cst;
} TBUF;

long times();

/*
 * cputim - return (in ticks) CPU time since last call
 */
long cputim()
{
	long ret;
	long ut1, st1, i;
	static TBUF tbufa = {0, 0, 0, 0};
	static TBUF tbufb = {0, 0, 0, 0};

	times(&tbufb);
	ut1 = UTIME;
	st1 = STIME;
	ret = (ut1 + st1);

	tbufa = tbufb;
	return (ret);
}
###cputim.h
/* cputim.h  */
#ifndef TIME_H
#define TIME_H
#define CLOCK_TICKS_PER_SECOND 60
typedef long cputim_t;
extern cputim_t cputim();
#endif
###readme.
to run the timing suite on unix

the source disk requires these steps to run:

the procedure involves dozens of compiles.
the stdiolib test needs lots of free disk space, 200 K would suffice.


run  install.sh

	this produces the tables for basic operations, and creates the
	object files needed for the other steps.

run  mkavg.sh

	this does some simple "cluster analysis" to produce an
	"average time" estimate for each data category.  any time
	that is within a factor of 3 of the average is included in
	the average; the out-of-range times are listed individually.

run  runmost.sh

	this times various control structures

run  runmost2.sh

	more control structures, library tests, simple timing harnesses

run  alltyp.sh

	this times each C operator on all the basic types, and
	produces a "grouping" of significantly different categories.
	(in other words, what combintions of type and storage class
	affect the timing of each operator.)

call with any questions.

see the appendix of Efficient C for further instructions.


tom plum  609-927-3770
###EOF
