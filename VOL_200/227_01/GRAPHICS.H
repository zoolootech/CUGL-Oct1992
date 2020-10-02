/*
 * g r a p h i c s . h
 * -------------------
 * High-level header file. Contains all hardware-independant high-level
 * defintions for the compatible graphics library. Does includes the
 * generic low-level header-file "graf.h", if not forbidden (NOLLHINC set).
 *
 * update history
 * --------------
 * May, 28. 1987     Definitions for text-package added
 * Jun, 17. 1987     Section "Configurable parameters" added in this file.
 *                   This section allows it the user to configure some minor
 *                   features of the library. First entry in this section is
 *                   the preprocessor symbol COUTNRY. Allows text generation
 *                   for different countrys.
 * Jun, 20. 1987     Internal library implemtation mode added. This mode is
 *                   enabled by #defineing LIB_MODE to 1 and is restriced
 *                   to the graphics library only. User programs activating
 *                   this mode may get totally confused!
 * Aug, 30. 1987     Trigonemetric tables added.
 *		     Configuration tags USEENUM and LOADTRIG introduced.
 *		     Support for the write mode system added.
 *
 * Written by        Rainer Gerhards
 *                   Petronellastr. 6
 *                   D-5112 Baesweiler
 *                   West Germany
 *                   Phone (49) 2401 - 1601
 */

/*
 * Configurable parameters
 * -----------------------
 * The following parameters may be used to make minor configuration changes
 * to the graphics library. They may be modified by the user and do not impact
 * performance nor compatibility of the library. After modifications in this
 * part, the whole library (including low-level drivers) must be recompiled!
 *
 * Note: The user should take extreme care when modifying this parameters,
 *       even if this are only minor configuration parameters.
 *       Modifications may result in parameter conflicts, which can't be
 *       reported by all compilers.
 */

/*
 * The following defines are used to activate some language features,
 * which are currently not common to all compilers.
 */
#define USEPROTT 1			/* function prototyping allowed	*/
#define USEVOID  1			/* type void allowed?		*/
#define	USEENUM	 1			/* type enum allowed?		*/

/*
 * The following defines determine the optimization status of the library.
 * If tag OPTTIME is #defined to 1, the execution time is optimized (resulting
 * in larger memory allocation). If tag OPTSPACE is #defined to 1, the memory
 * allocation is optimized (resulting in slower execution). If neither tag is
 * #defined (or #defined to 0), the library uses a good compromise. Both tags
 * can't be true at the same time.
 * Note that this is only a minor request to the library coding. You should
 * not expect to much from modifying these tags. However, in an (IBM-) PC
 * environment I think it is the best guess to define OPTTIME.
 */
#define OPTTIME  1
#define OPTSPACE 0

/*
 * Select what to used: macros or function calls for the conversion and
 * other frequently used functions. Macro usage can't be selected if
 * OPTSPACE == true!
 */
#define USEMACS 1

/*
 * The following tag controls the loading of the trigonemetric lookup tables.
 * If it's defined to 0, they aren't loaded, otherwise they are loaded.
 * Loading this tables is of use when working with function fellipsis, the
 * turtle graphics system and the vector driven graphics functions (currently
 * not implemented).
 */
#define LOADTRIG 1

/*
 * The following define is used for country specific tasks, espicially
 * for message processing. The country number is as defined in your DOS
 * manual (international telephone country code).
 *
 * Currently supported countries are:
 * Country                        Code
 * United States of America          1
 * Switzerland                      41
 * Great Britain                    44
 * Federal Republic of Germany      49
 */
#define COUNTRY 1

/*
 * +-------------------------------------------------------------------------+
 * |      E N D   O F   C O N F I G U R A B L E   P A R A M E T E R S        |
 * +-------------------------------------------------------------------------+
 *
 * Please do not make any changes below this point!
 */
#ifndef __TURBOC__
  #define defined(x) x
#endif

#ifndef LIB_MODE
  #define LIB_MODE 0
#endif

/*
 * Cross check the configurable parameters.
 * If there are errors detected, and the compiler supports no text-output
 * function, an syntax error is generated.
 */
#ifndef COUNTRY
    "COUNTRY parameter must be defined!"
#else
  #if (COUNTRY != 1) && (COUNTRY != 41) && (COUNTRY != 44) && (COUNTRY != 49)
      "Unsupported country specified!"
  #endif
#endif  /* end of country check */

/*
 * cross-check optimization information
 */
#if OPTTIME && OPTSPACE
    "Error: Can't specifiy both OPTTIME and OPTSPACE!"
#endif

#if OPTSPACE && USEMACS
    "Can't specifiy both OPTSPACE and USEMACS!"
#endif

/*
 * Convert some newer types to old types, if requested.
 */
#define	TRUE	-1
#define	FALSE	0

#if	!USEVOID
typedef int	void;
#endif


/* 
 * WARNING:
 * Turbo C V. 1.0 has an error in the constant folding algorithm.
 * So do not use constant expressions, if this C compiler is active.
 */
#ifdef	__TURBOC__
  #define	PI_D_180	0.017453293
#else
  #define	PI_D_180	PI / 180.0
#endif

/*
 * The following macro is used to declare data items as external usable
 * (in library mode) or external defined (in user mode).
 */

#if LIB_MODE == 2 /* master module? */
  #define EXTERN
  #define INITVAL(x) = (x)
#else
  #define EXTERN  extern
  #define INITVAL(x)
#endif

#if defined(DLC) || defined(__TURBOC__)
  #define PI 3.14159265358979323846
  #define iabs(x) abs(x)
#endif

/*
 *	support for the trigonometric functions
 *	---------------------------------------
 *	These functions are generally very slow, and so the graphics
 *	library uses a table of precomputed values, if not forbidden
 *	(OPTSPACE == TRUE).
 *	The trigonometric functions sin() and cos() are redifined by
 *	intsin() and intcos(). This new functions perform the requested
 *	action acording to the configuration switches. Their argument
 *	is always in degrees.
 */
#if	LOADTRIG
  #define	intsin(x)	sintab[x]
  #define	intcos(x)	costab[x]
  #if	LIB_MODE == 2
    #include "trigtab.h"		/* load precomputed values	*/
  #else
    extern float	sintab[361];
    extern float	costab[361];
  #endif
#else
  #define	intsin(x)	sin(torad(x))
  #define	intcos(x)	cos(torad(x))
#endif


/*
 * The following section contains macros to do some conversions, that are
 * often needed in a graphics environment. This macros are also available
 * as function calls and are only included if USEMACS is true. Your
 * program should always think it uses the macro and thus should be free
 * of side-effects!
 */
#if USEMACS
						/* arc conversion        */
  #define todeg(x) ((x)/(PI_D_180))		/* radiant -> degree     */
  #define torad(x) ((x)*(PI_D_180))		/* degree -> radiant     */
						/* coordinate conversion */
  /*
   * Note: when converting polar to cartesian coordinates, you have to
   * correct the x radius, because the current resuloution doesn't correspond
   * to the physical screen dimensions. If you wouldn't convert, you wouldn't
   * get a real circle.
   * The conversion can't be done by the macros below, because they don't
   * know the current screen resolution (you may use your onw world
   * coordinates). To aid you in the conversion task, an additional macro
   * is supplied to correct the x radius. You have to supply the maximum
   * x and y resolution and the radius to this macro. It then computes the
   * actual x radius to use.
   */
  #define tocarxc(rad, arc) ((rad)*cos((double)(arc)))	/*polar->cartesian x*/
  #define tocaryc(rad, arc) ((rad)*sin((double)(arc)))	/*polar->cartesian y*/
  #define itocarxc(rad, arc) ((rad)*intcos((int)(arc)))	/*polar->cartesian x*/
  #define itocaryc(rad, arc) ((rad)*intsin((int)(arc)))	/*polar->cartesian y*/
  #if !defined(__TURBOC__)
    #define convxrad(mx, my, rad) (((double)(mx)/(double)(my)/(3.2/2.275))\
  				  *(rad))
  #else
    #define convxrad(mx, my, rad) (((double)(mx)/(double)(my)/(1.40659341))\
				  *(rad))
  #endif
#endif /* USEMACS */

/*
 * Definiton of hardware-independent constants. These constants are common
 * to all graphics devices. If you add a new device, which uses some of
 * this definitions specific, please remove the definition here and put
 * the definition in all old header files. Your new definition should only
 * be in your header file for the new device.
 */
#define BLACK    0			/* color black			*/
#define WHITE    1			/* color white			*/
#define YES      -1			/* used for Yes/No-questiones	*/
#define NO       0			/* used for Yes/No-questiones	*/
#define	OK	0			/* good return value		*/
#define	WRONG	1			/* bad return value		*/

/*
 * Graphics text package definitions
 * ---------------------------------
 */
#define STDFONT		0		/* standard font		*/
#define OM_NORM		0		/* normal text output		*/
#define OM_ITAL		1		/* italic text output		*/
#define OM_RITAL	2		/* reverse italic text output	*/

/*
 * support for the error-system
 * ----------------------------
 */
#define GE_OK    0	/* everything ok					*/
#define GE_FATAL 1	/* fatal unexplainable error			*/
#define GE_NOCO  2	/* no or not enough coordinates given		*/
#define	GE_WMWRG 3	/* wrong write mode requested			*/

EXTERN int gr_error INITVAL(GE_OK);  /* contains last graphics error code */

/*
 * external variables supported by the global/local coordinate system
 */
EXTERN long glo_maxx;			/* maximum x resolution		*/
EXTERN long glo_maxy;			/* maximum y resolution		*/

/*
 * support for the turtle graphics system
 * --------------------------------------
 */
EXTERN double turt_x	INITVAL(0);	/* turtle x coordinate		*/
EXTERN double turt_y	INITVAL(0);	/* turtle y coordinate		*/
EXTERN double turt_sz	INITVAL(10.0);	/* turtle size (if visible)	*/
EXTERN int turt_vis	INITVAL(0);	/* turtle visible?		*/
EXTERN int turt_del	INITVAL(0);	/* turtle delay in miliseconds	*/
EXTERN int turt_pdn	INITVAL(-1);	/* turtle pen down?		*/
EXTERN int turt_clr	INITVAL(1);	/* turtle color			*/

#define	GTP_UP		0		/* turtle pen up		*/
#define	GTP_DOWN	-1		/* turtle pen down		*/

#define	GTCO_HX		0		/* turtle x home coordinate	*/
#define	GTCO_HY		0		/* turtle Y home coordinate	*/

#define	GTDHOME		0		/* home direction  (up)		*/
#define	GTDNORTH	0		/* nothern direction (up)	*/
#define	GTDSOUTH	180		/* southern direction (down)	*/
#define	GTDEAST		90		/* eastern direction (rigth)	*/
#define	GTDWEST		270		/* western direction (left)	*/

/*
 *	support for the write mode system
 *	---------------------------------
 *	Starting with version 2.0 of the library, you may specify different
 *	write modes (i. e. xor old and new color).
 *	Here are all external visible names (except function names) defined.
 */
#if	USEENUM
  typedef enum	{
		WM_NORM,		/* no manipulation		*/
		WM_XOR,			/* xor old and new color	*/
		WM_AND,			/* and old and new color	*/
		WM_OR,			/* or old and new color		*/
		WM_INV			/* invert old color		*/
		}	WRITEMOD;
#else
  #define		WM_NORM		0
  #define		WM_XOR		1
  #define		WM_AND		2
  #define		WM_OR		3
  #define		WM_INV		4
  typedef	int	WRITEMOD;
#endif

#define	WM_MIN	WM_NORM			/* minimal write mode value	*/
#define	WM_MAX	WM_INV			/* maximal write mode value	*/

EXTERN WRITEMOD	writemod INITVAL(WM_NORM);
EXTERN int	curcolor INITVAL(0);	/* current drawing color	*/
/*
 * function prototyping (if supported by compiler)
 * -----------------------------------------------
 */
#if USEPROTT
  extern int	setwm(WRITEMOD);
  extern void	box(int, int, int, int, int);
  extern void	circle(int, int, int, int, double);
  extern int	convxco(double);
  extern int	convxdis(double);
  extern int	convyco(double);
  extern int	convydis(double);
  extern void	ellipsis(int, int, int, int, int, int, int);
  extern void	fellipsis(int, int, int, int, int, int, int);
  extern void	fillbox(int, int, int, int, int);
  extern int	getpixel(int, int);
  extern void	line(int, int, int, int, int);
  extern void	paint(int, int, int, int);
  extern int	polyline(int, int *);
  extern int	prtgrf(void);
  extern void	putmsg(int, int, int, int, char *, int, int);
  extern void	setgloco(double, double, double, double);
  extern void	setpixel(int, int, int);
  extern void	pgrerror(FILE *, char *);
  extern void	wrtpixel(int, int, int);
  extern void	gt_show(void);
  extern void	gt_hide(void);
  extern void	gt_forwd(int);
  extern void	gt_backwd(int);
  extern void	gt_dir(int);
  extern void	gt_tleft(int);
  extern void	gt_trght(int);
  extern void	gt_setco(int, int);
  extern void	gt_pendn(void);
  extern void	gt_penup(void);
  extern void	gt_home(void);
  extern void	gt_usrmv(int *);
  extern void	gt_init(void);
#endif

/*
 * Now do a #include of the low-level definitions. This include can be
 * avoided by defining the preprocessor-symbol NOLLHINC to a value other
 * then zero.
 * This practice is useful if you want to include a specific low-level
 * header-file. Otherwise the generic file 'GRAF.H' is included.
 */
#ifndef NOLLHINC
  #include "graf.h"
#endif

#undef	EXTERN
#undef	INITVAL
/*
 * Vi mode lines. Do not remove them.
 *
 * vi: set sw=2 :
 */
