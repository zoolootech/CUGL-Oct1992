###mkavg.sh
set -x

cc -o avg avg.c libtim -lm

avg intreg.out 1 >intreg.avg
avg sstat.out 2 >sstat.avg
avg sauto.out 2 >sauto.avg
avg lauto.out 2 >lauto.avg
avg dauto.out 7 >dauto.avg
avg cauto.out 2 >cauto.avg
avg fauto.out 7 >fauto.avg
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
#define FR_ANCHOR var1	/* what is the "frame anchor": arg1 or var1 */
						/* VAX UNIX !!! */
#define RETOFFSET (5)	/* offset of return pointer
						   relative to frame anchor */
#define JUNKFILE "/dev/null"
###EOF
