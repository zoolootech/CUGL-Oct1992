/* compile.h - define some compiler and/or CPU dependent conditions	*/

/* Edit the following definitons to match your environment.  If your	*/
/* machine is in a list, you can simply select one.  Otherwise, you	*/
/* must add your machine into the list and supply proper definitions.	*/
/* Note that some entries in the list are NOT TESTED, sorry.		*/

#define MSC_86	    0	/* for Microsoft C ver 3 or later for 8086's	*/
#define TURBO_C_86  1	/* for Borland's Turbo C 1.5 for 8086's		*/
#define LATTICE_86  0	/* for Lattice C ver 2 for 8086's		*/
#define DRI_68K	    0	/* for Degital Research C for 68000's		*/
#define SUN3	    0	/* for Sun Microsystems' SUN 3 work station	*/
#define u3B2	    0	/* for AT&T's 3B2 with UNIX system V		*/

/* specific definitions							*/

#if MSC_86 | TURBO_C_86
/* basic data types */
#define BYTE		unsigned char	/* unsigned  8 bit type		*/
#define WORD		unsigned short	/* unsigned 16 bit type		*/
#define BOOL		unsigned short	/* fast unsigned type		*/
#define EXTRA		unsigned long	/* any type w/ 18 bits or more	*/
/* byte order */
#define LITTLE_INDIAN	1		/* Least Significant BYTE first */
#define BIG_INDIAN	0		/* Most Significant BYTE first	*/
#endif

#if LATTICE_86
/* basic data types */
#define BYTE		char		/* unsigned  8 bit type		*/
#define WORD		unsigned short	/* unsigned 16 bit type		*/
#define BOOL		char		/* fast unsigned type		*/
#define EXTRA		unsigned long	/* any type w/ 18 bits or more	*/
/* byte order */
#define LITTLE_INDIAN	1		/* Least Significant BYTE first */
#define BIG_INDIAN	0		/* Most Significant BYTE first	*/
#endif

#if u3B2
/* basic data types */
#define BYTE		unsigned char	/* unsigned  8 bit type		*/
#define WORD		unsigned short	/* unsigned 16 bit type		*/
#define BOOL		unsigned long	/* fast unsigned type		*/
#define EXTRA		unsigned long	/* any type w/ 18 bits or more	*/
/* byte order */
#define LITTLE_INDIAN	1		/* Least Significant BYTE first */
#define BIG_INDIAN	0		/* Most Significant BYTE first	*/
#endif

#if DRI_68K
/* basic data types */
#define BYTE		unsigned char	/* unsigned  8 bit type		*/
#define WORD		unsigned short	/* unsigned 16 bit type		*/
#define BOOL		unsigned short	/* fast unsigned type		*/
#define EXTRA		unsigned long	/* any type w/ 18 bits or more	*/
/* byte order */
#define LITTLE_INDIAN	0		/* Least Significant BYTE first */
#define BIG_INDIAN	1		/* Most Significant BYTE first	*/
#endif

#if SUN3
/* basic data types */
#define BYTE		unsigned char	/* unsigned  8 bit type		*/
#define WORD		unsigned short	/* unsigned 16 bit type		*/
#define BOOL		unsigned long	/* fast unsigned type		*/
#define EXTRA		unsigned long	/* any type w/ 18 bits or more	*/
/* byte order */
#define LITTLE_INDIAN	0		/* Least Significant BYTE first */
#define BIG_INDIAN	1		/* Most Significant BYTE first	*/
#endif
