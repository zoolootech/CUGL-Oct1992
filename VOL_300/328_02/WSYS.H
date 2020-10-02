/*! WSYS.H
 *       header file for wtwg source code programs
 */
#ifndef WSYS_H
	#define WSYS_H

#include "wtwg.h"
#include <string.h>
#include <dos.h>
#include <ctype.h>

#ifdef __TURBOC__
	#include <alloc.h>	/* TurboC */
		#ifdef  __OVERLAY__
			/* not allowed to use near calls in overlayed programs */
			#define W_NEAR
		#else
			#define W_NEAR near
		#endif
#else
	#include <malloc.h> /* Microsoft C */
	
	#ifndef W_NEAR
		#define W_NEAR near
	#endif	/* ifndef W_NEAR */

	#include "msc.h"
#endif



/* farmemcpy()
 * in large models this is just memcpy
 * but in small models it's movedata()
 */
#undef LARGE_DATA_MODEL
#ifdef __LARGE__
	#define LARGE_DATA_MODEL
#endif
#ifdef __COMPACT__
	#define LARGE_DATA_MODEL
#endif
#ifdef __HUGE__
	#define LARGE_DATA_MODEL
#endif 




#ifdef LARGE_DATA_MODEL
	#define farmemcpy(aa,bb,cc)  memcpy (aa,bb,cc)
#else

	/* model is small or medium - need a far * equivalent of
	 * memcpy, write, read, memcmp
	 */
	#define farmemcpy(dest,src,num)  \
	movedata (FP_SEG(src), FP_OFF(src), FP_SEG(dest), FP_OFF(dest), num )

#endif	/* memcpy */



/* NORMALIZE macros.
*	Far pointer normalization routine
 *		Arithmetic on far pointers only affects the offset,
 *		does not carry into the segment portion.
 * 	to avoid overflow errors,
 *		must convert seg:off pair to place as much
 * 		of the address as possible into the segment part.
 *
 *	 NORMALIZE () - unconditionally normalize a far pointer.
 *      _NORMALIZE () - normalize a data pointer IF memory model requires it
 *
 *  use _NORMALIZE for pointers not explicitly declared far.
 */
#ifndef __cplusplus
	/* traditional C */
	#define NORMALIZE(fp)		\
	   fp = MK_FP ( ( FP_SEG(fp) + (FP_OFF(fp)>>4) ),  \
			((unsigned)(FP_OFF(fp))& 0x000f) )
#else
	// C++ requires type casts for assignment
	#define NORMALIZE(fp,type) \
	   fp = (type) MK_FP ( ( FP_SEG(fp) + (FP_OFF(fp)>>4) ),  \
			((unsigned)(FP_OFF(fp))& 0x000f) )
#endif	/* NORMALIZE */




#ifdef LARGE_DATA_MODEL
	/* pointers are far *, need to normalize all ptrs
	 * except in __HUGE__ model, where they are normalized for you.
	 */
	#ifndef __HUGE__
		#define _NORMALIZE(fp)  NORMALIZE(fp)
	#endif  /* HUGE */
#else
	/* model is SMALL or MEDIUM - data ptrs only are far if explicit far
 	 */
	#define _NORMALIZE(fp)
	
#endif  /* LARGE_DATA - normalize  */



#ifdef __HUGE__
	/* data pointers are all far, but TurboC normalizes them for you */
	#undef  _NORMALIZE()
	#define _NORMALIZE(fp)
#endif











	#ifdef  __TURBOC__
		/*  Defining PSEUDOREGS allows using in-line interrupts 
		 *  in the TurboC version, but generates calls to funcs 
		 *  in the Microsoft version. see MSC.H
		 */
		#define	PSEUDOREGS
		#define INTERRUPT(intno)  geninterrupt ( (intno) )
		#define INTERRUPTX(intno) geninterrupt ( (intno) )

	#endif	/* end of redefinning pseudoregs  */


/*end of wsys.h*/
#ifdef __BORLANDC__
	#pragma hdrstop
#endif /* BORLANDC */

#endif /* WSYS.H */