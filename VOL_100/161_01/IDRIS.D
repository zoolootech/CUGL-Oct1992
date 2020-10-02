###alltyp.sh
goto next
: next
cc -o groups groups.c libtim

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
goto next
: next
sh -e Install.sh
sh -e Mkavg.sh
sh -e Runmost.sh
sh -e Alltyp.sh
sh -e Runmost2.sh
###install.sh
goto next
: next

cc +o  \
	begint.c \
	cputim.c \
	returnp.c \
	fround.c \

lib libtim -c begint.o cputim.o returnp.o fround.o
cc -o calib.x calib.c libtim
calib.x > parms.dat

Makex intreg
Makex sstat
Makex lauto
Makex sauto
Makex dauto
Makex cauto
Makex fauto

cc -o fmttbl fmttbl.c
fmttbl intreg.out sstat.out sauto.out lauto.out dauto.out
###makex.
cc -o $1.x $1.c libtim
echo $1:
$1.x >$1.out
mv $1.x Previous.x
mv $1.o Previous.o
###mkavg.sh
goto next
: next

: after running install.bat you can run mkavg.bat to compute averages
cc -o avg avg.c libtim

avg intreg.out 8 >intreg.avg
avg sstat.out 10 >sstat.avg
avg sauto.out 10 >sauto.avg
avg lauto.out 50 >lauto.avg
avg dauto.out 400 >dauto.avg
avg cauto.out 10 >cauto.avg
avg fauto.out 400 >fauto.avg
###runmost.sh
goto next
: next
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
###testloop.out
104
100
100
100
100
100
100
101
101
100
101
102
101
101
100
100
100
100
101
101
101
101
100
100
100
101
101
101
100
100
100
100
101
101
100
100
101
101
100
100
###testloop.sh
testloop 39
testloop 39
testloop 39
testloop 39
testloop 39
testloop 39
testloop 39
testloop 39
testloop 39
testloop 39
testloop 39
testloop 39
testloop 39
testloop 39
testloop 39
testloop 39
testloop 39
testloop 39
testloop 39
testloop 39
testloop 39
testloop 39
testloop 39
testloop 39
testloop 39
testloop 39
testloop 39
testloop 39
testloop 39
testloop 39
testloop 39
testloop 39
testloop 39
testloop 39
testloop 39
testloop 39
testloop 39
testloop 39
testloop 39
testloop 39
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
#define UTINY	/* Idris 11 2.3 WSL */
#define USHORT	/* Idris 11 2.3 WSL */
#define ULONG	/* Idris 11 2.3 WSL */
/* no VOID Idris 11 2.3 WSL */
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

/* circumvent c 2.3 bug - no ULONG on right-shift */
#ifdef DRSH
#undef ULONG
#endif
###cputim.c
/* cputim - Idris version
 */
#include <stdio.h>

#define STRUCTASST(a, b) cpybuf(&a, &b, sizeof(a))
typedef unsigned short  ushort;

#define UTIME (tbufb.ut - tbufa.ut + tbufb.cut - tbufa.cut)
#define STIME (tbufb.st - tbufa.st + tbufb.cst - tbufa.cst)

typedef struct tbuf {	/* UNIX V6  Idris */
	ushort ut, st;
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

	STRUCTASST(tbufa, tbufb);
	return (ret);
}
###cputim.h
/* cputim.h */
#ifndef TIME_H
#define TIME_H
#define CLOCK_TICKS_PER_SECOND 60
typedef long cputim_t;
extern cputim_t cputim();
#endif
###testloop.c
#include <stdio.h>
main(ac, av)
	int ac;
	char *av[];
	{
	long atol();
	long cputim();
	long limit = atol(av[1]);
	long i;
	long j;

	cputim();
	for (i = 0; i < limit; ++i)
		for (j = 0; j < 1000; ++j)
			;
	printf("%ld\n", cputim());
	}
###do-all.sh
goto next
: next
sh -e Install.sh
sh -e Mkavg.sh
sh -e Runmost.sh
sh -e Alltyp.sh
sh -e Runmost2.sh
###EOF
