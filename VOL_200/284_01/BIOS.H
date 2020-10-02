/* bios configuration ( must match with CP/M bios )			*/

/* CP/M secter length							*/
#define SECLEN		128		/* This constant is fixed	*/

/* pseudo disk related constants					*/
#define NDISKS		8		/* # of disk drives in a system */

/* phisical disk related constants					*/
#define PHISDISK	1		/* 1 to support phisical disk	*/

/* constants in Disk Parameter Block					*/
#define DPB_SPT		26		/* sector / track of phis. disk */
#define DPB_OFF		2		/* # of reserved track		*/

/* disk types								*/
#define EMPTY		(-1)		/* empty drive			*/
#define PHIS		(-2)		/* phisical disk drive		*/
