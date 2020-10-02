/*
 * b a s n a m e s . h
 * -------------------
 * This header file converts compatible graphics library function names
 * to their basic counterparts. This file bases on an idea of Michael Yokoyama
 * of Aiea, HI.
 *
 * This file needs only to be included if the user wishes to use basic-like
 * funtion names.
 *
 * release history
 * --------------
 * Nov, 14. 1987     First version created.
 *
 * Written by        Rainer Gerhards
 *                   Petronellastr. 6
 *                   D-5112 Baesweiler
 *                   West Germany
 *                   Phone (49) 2401 - 1601
 */
#ifdef	H_BASNAMES
#define	H_BASNAMES

#define	pset(x, y, color)	wrtpixel(x, y, color)
#define	fpoint(x, y)		getpixel(x, y)
#define	point(x, y)		getpixel(x, y)
#define	initgco(x1, y1, x2, y2)	setgloco(x1, y1, x2, y2)
#define	gxtolx(x)		convxco(x)
#define	gytoly(y)		convyco(y)
#define	pmap(x, n)		((n) ? gytoly(x) : gxtolx(x))

#endif
