struct _fcb {
	char f_entry;	/* drive code: 0=>default 1=>A */
	char f_name[11];	/* filename */
	char f_extent;		/* extent number */
	char f_magic[23];	/* more klugery */
};


#define	fcb	struct _fcb
#define _MBYTE	9	/* offset of permission byte */
#define _SBYTE	10	/* offset of system byte */
                                                                             