/*
 *	e n v i r o n . h
 *	-----------------
 *	This module contains environment specific information. It is
 *	used to make the programs more portable.
 *
 *	@(#)Copyrigth (C) by Rainer Gerhards. All rights reserved.
 */
#ifndef	ENVIRON_H
#define	ENVIRON_H

/*
 *	configurable parameters.
 *	modify the following parameters according to the targe environment.
 */

/*
 *	define target operating system
 */
#define	MSDOS		1
#define	UNIX		0
#define	STARSYS		0

/*
 *	define target machine
 *
 *	This is auxiluary data only needed for some operating
 *	systems. Currently only needed if MS-DOS is active.
 */
#define	IBM_PC		1		/* IBM PC, XT, AT & compatibels	*/
#define	WANG_PC		0		/* Wang PC, APC ...		*/

/*
 *	define target compiler (if neccessary)
 */
#define	MSC		1		/* Microsoft C			*/

#define	AUTO_SEL	1
/*
 * The above #define allowes an automatic language set selection. It is
 * only functional if the used compiler identifies itself via a #define.
 *
 * Note: If AUTO_SEL is set, the parameters below are meaningless!
 */

#define	USE_FAR		0		/* use far keyword		*/
#define	USE_NEAR	0		/* use near keyword		*/
#define	USE_VOID	1		/* use void keyword		*/
#define	USE_VOLA	0		/* use volatile keyword		*/
#define	USE_CONST	0		/* use const keyword		*/
#define	USE_PROTT	0		/* use function prototypes	*/
#define	USE_INTR	0		/* use interrupt keyword	*/

/*	+----------------------------------------------------------------+
 *	|                  End Of Configurable Parameters                |
 *	+----------------------------------------------------------------+
 *	Please do not make any changes below this point!
 */

#ifndef	SYMDEB
# define	SYMDEB		0
#endif

/*
 *	Check target compiler. Note that the MSC switch is overriden if
 *	either __TURBOC__ or DLC are defined.
 */
#ifdef	__TURBOC__
# undef	MSC
#endif
#ifdef	DLC
# undef	MSC
#endif
#if	STARSYS
# undef MSC
#endif

#if	!MSDOS
# undef MSC
# undef	AUTO_SEL
# define	AUTO_SEL	0
#endif

#if	AUTO_SEL
#  undef	USE_FAR
#  undef	USE_NEAR
#  undef	USE_VOID
#  undef	USE_VOLA
#  undef	USE_CONST
#  undef	USE_PROTT
#  undef	USE_INTR
#  ifdef	__TURBOC__
#    define	USE_FAR		1
#    define	USE_NEAR	1
#    define	USE_VOID	1
#    define	USE_VOLA	1
#    define	USE_CONST	1
#    define	USE_PROTT	1
#    define	USE_INTR	1
#  endif
#  ifdef	DLC
#    define	USE_FAR		1
#    define	USE_NEAR	1
#    define	USE_VOID	1
#    define	USE_VOLA	1
#    define	USE_CONST	1
#    define	USE_PROTT	1
#    define	USE_INTR	0
#  endif
#  ifdef	MSC
#    define	USE_FAR		1
#    define	USE_NEAR	1
#    define	USE_VOID	1
#    define	USE_VOLA	1
#    define	USE_CONST	1
#    define	USE_PROTT	1
#    define	USE_INTR	1
#  endif
#endif


#if	!USE_FAR
#define	far
#endif

#if	!USE_NEAR
#define	near
#endif

#if	!USE_VOID
#define	void
#endif

#if	!USE_VOLA
#define	volatile
#endif

#if	!USE_CONST
#define	const
#endif

#if	USE_INTR
# ifdef	MSC
#   define	INTERRUPT interrupt far
# else
#   define	INTERRUPT interrupt
# endif
#else
# define	INTERRUPT
#endif

#if	USE_PROTT
#  define	PROTT(x)	x
#  ifdef MSC
#    define	STATICPT(func, prott) static func prott
#  else
#    define	STATICPT(func, prott) extern func prott
#   endif
#else
#  define	PROTT(x)	()
#  ifdef MSC
#    define	STATICPT(func, prott) static func ()
#  else
#    define	STATICPT(func, prott) extern func ()
#   endif
#endif

#ifdef	MSC
# define	inportb(port) inp(port)
# define	outportb(port, val) outp(port, val)
#endif

#ifdef	__TURBOC__
#  define	REGPKT	struct REGS
#else
#  define	REGPKT	union REGS
#endif

#ifdef	DLC
#  define	defined(x)
#  define	inportb		inp
#  define	outportb	outp
#endif

#if	!SYMDEB				/* symbolic debugging support	*/
#  define	STATIC		static
#endif

#if	STARSYS
#  define	exit(x)		dx_exit(x)
#endif

/*
 * Define open modes according to selected operating system/compiler.
 */
#if	MSDOS
#  define	OPM_WB		"wb"
#  define	OPM_WT		"wt"
#  define	OPM_RB		"rb"
#  define	OPM_RT		"rt"
#endif

#if	UNIX
#  define	OPM_WB		"w"
#  define	OPM_WT		"w"
#  define	OPM_RB		"r"
#  define	OPM_RT		"r"
#endif

#if	STARSYS
#  define	OPM_WB		"wb"
#  define	OPM_WT		"wt"
#  define	OPM_RB		"rb"
#  define	OPM_RT		"rt"
#endif

#define	TRUE	(1)
#define	FALSE	(0)

typedef	unsigned char	uchar;

#define tonumber(x)	((x) - '0')
#define	FOREVER()	for(;;)

#endif
