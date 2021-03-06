/*  FILE  ==>  pec.h                                               */
/*  Contains special purpose function numbers for use with the     */
/*  "BDOS(c, de)" function of BDS C.                               */
/*-----------------------------------------------------------------*/

#define READ	1	/* Read console byte */
#define	WRITE	2	/* Write console byte */
#define	LIST	5	/* Write list device (LPT) byte */
#define	DCIO	6	/* Direct console in - out */
#define IOBYTE	7	/* Get IOBYTE setting */
#define	SET_IOBYTE	8	/* Set IOBYTE */
#define PRT_STRING	9	/* Display "$" terminated string */
#define READ_STRING	10	/* Read a string from the console */
#define CON_STATUS	11	/* Read console status */
#define	CPM_NUM	12	/* Get CP/M Version No. */
#define	RESET	13	/* Reset the disk system */
#define	LOGICAL	14	/* Select logical disk */
#define	OPEN_FILE	15	/* Open file for read/write */
#define	CLOSE_FILE	16	/* Close the file */
#define	SEARCH_FIRST	17	/* Search for first name match */
#define SEARCH_NEXT	18	/* Search for next name match */
#define ERASE	19	/* Erase the named file */
#define READ_SECT	20	/* Read a sequential sector */
#define	WRITE_SECT	21	/* Write a sequential sector */
#define	CREATE	22	/* Create (make) the named file */
#define RENAME	23	/* Rename a file */
#define DEFAULT	25	/* Get current default disk */
#define	SETDMA	26	/* Set the DMA address */
#define	READ_ONLY	28	/* Sets logical disk to R/O status */
#define	SET_ATTRIBUTES	30	/* Set file attributes */
#define USER_ID	32	/* Set/get User No. */
/*-----------------------------------------------------------------*/

                         