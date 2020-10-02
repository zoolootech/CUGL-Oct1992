/* xb.h--header file for cxb.c and rxb.c
   Copyright 7/85 N.T.Carnevale.
   Permission granted for nonprofit use only.
   See my article in the July/August 1986 issue of 
   Micro/Systems Journal pp.72-85.
*/

#define TRUE 1
#define FALSE 0
#define	boolean int

/* handy constants */
#define SIG "xbios installed$"
#define SIGLNTH 16	/* how many characters in the signature string */

/* useful structures */
struct xbhdr {				/* replica of xbios module header */
	char jump,			/* 0xc3 */
	     *oldbdos,			/* entry to bdos */
	     otbloffset,		/* how far from vstart to the 
					   start of the local copy of the
					   old jump table */
	     jmpnum,			/* how many jumps in the table */
	     reserved[11],		/* unused at present */
	     signature[SIGLNTH],	/* the signature string */
	     vstart;			/* 1st char of version string */
};

struct cpmhdr { 	/* replica of the first few bytes in cpm */
	char jump1,	/* 0xc3 */
	     *wboot,	/* address of the jmp wboot in bios table */
	     whatnot,	/* user etc */
	     drive,	/* logged drive */
	     jump2,	/* another 0xc3 */
	     *bdosloc;	/* entry to bdos */
};
