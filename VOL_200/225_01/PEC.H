/*-----------------------------------------------------------------*/
/*  FILE  ==>  pec.h                                               */
/*  Contains special purpose function numbers for use with the     */
/*  "BDOS(c, de)" function of BDS C.                               */
/*  Also some general purpose character translations.              */
/*                                                                 */
/*  Written:       15/11/86
    -------                                                        */
/*  Copyright 1986 - Cogar Computer Services Pty. Ltd.             */
/*-----------------------------------------------------------------*/
/*            BDOS FUNCTIONS                                       */
/*-----------------------------------------------------------------*/

#define READ            1	/* Read console byte */
#define WRITE           2	/* Write console byte */
#define READER          3       /* Read logical "reader" byte      */
#define PUNCH           4       /* Write logical "punch" byte      */
#define LIST            5	/* Write list device (LPT) byte */
#define DCIO            6	/* Direct console in - out */
#define IOBYTE          7	/* Get IOBYTE setting */
#define SET_IOBYTE      8	/* Set IOBYTE */
#define PRT_STRING      9	/* Display "$" terminated string */
#define READ_STRING    10	/* Read a string from the console */
#define CON_STATUS     11	/* Read console status */
#define CPM_NUM        12	/* Get CP/M Version No. */
#define RESET          13	/* Reset the disk system */
#define LOGICAL        14	/* Select logical disk */
#define OPEN_FILE      15	/* Open file for read/write */
#define CLOSE_FILE     16	/* Close the file */
#define SEARCH_FIRST   17	/* Search for first name match */
#define SEARCH_NEXT    18	/* Search for next name match */
#define ERASE          19	/* Erase the named file */
#define READ_SECT      20	/* Read a sequential sector */
#define WRITE_SECT     21	/* Write a sequential sector */
#define CREATE         22	/* Create (make) the named file */
#define RENAME         23	/* Rename a file */
#define ACTIVE         24       /* Get Log-In vector               */
#define DEFAULT        25	/* Get current default disk */
#define MAKEDMA        26	/* Set the DMA address */
#define ALLOCATION     27       /* Get (block) Allocation vector   */
#define READ_ONLY      28	/* Sets logical disk to R/O status */
#define R_O            29       /* Gets R/O disks bit map          */
#define SET_ATTRIBUTES 30	/* Set file attributes */
#define DPB            31	/* Get the disk paramater block address */
#define USER_ID        32	/* Set/get User No. */
/*-----------------------------------------------------------------*/
/*            CHARACTER TRANSLATIONS                               */
/*-----------------------------------------------------------------*/

#define CR      13	/* ASCII carriage return                   */
#define LF      10	/* ASCII line feed                         */
#define SPACE   32	/* ASCII single space character            */
#define HT       9	/* ASCII horizontal tab character          */
#define NULL     0
#define FF      12	/* ASCII form-feed character               */
/*-----------------------------------------------------------------*/
/*            SPECIAL PURPOSE TRANSLATIONS                         */
/*-----------------------------------------------------------------*/

#define short   int	/* Creates a "short" definition            */
#define TRUE      1
#define FALSE     0
/*-----------------------------------------------------------------*/

/*            TERMINAL FUNCTIONS                                   */
/*            ------------------                                   */
/*  For use with Hazeltine Esprit terminal                         */
/*  Use with LEAD-IN as set by switches.   Change this setting if  */
/*  different in your case.                                        */
/*-----------------------------------------------------------------*/
#define LEAD_IN 0x07e	/* Tilde selected by switches              */
#define BRIGHT  0x01f	/* Needs to have foreground switch set     */
#define DIM     0x019   /* Low intensity screen display            */
#define CLEAR   0x01c
/*******************************************************************/
/*            DISK PARAMETER BLOCK                                 */
/*******************************************************************/
struct dpb {
	int SPT;	/* Sectors per track                       */
	char BSH;	/* Block shift factor                      */
	char BLM;	/* Block mask                              */
	char EXM;	/* Extent mask                             */
	int DSM;	/* Drive capacity                          */
	int DRM;	/* Directory size                          */
	char AL0;	/* Allocation vector                       */
	char AL1;	/* do                                      */
	int CKS;	/* Check area size                         */
	int OFF;	/* Count of reserved tracks                */
};
/*******************************************************************/
/*            FILE CONTROL BLOCK                                   */
/*******************************************************************/

struct fcb {

	char  DRIVE;	/* The drive code OR the user No.          */
	char  NAME[8];	/* The file name                           */
	char  TYPE[3];	/* The file extension                      */
	char  EXTENT1;	/* The extent No. for range 0 - 31         */
	char  FLAGS;	/* The BDOS flags                          */
	char  EXTENT2;	/* The extent No. for range > 31           */
	char  RECORDS;	/* Records in current extent               */
	char  BLOCKS[16];  /* The list of allocation blocks        */
/*-----------------------------------------------------------------*/
/*            EXTENDED FCB                                         */
/*-----------------------------------------------------------------*/

	char  CUR_REC;	/* The current record of the extent.   This
	                   is numbered from 0 to (RECORDS - 1)     */
	unsigned  RAN_ADR; /* Direct address from 0x000000 to
	                   0x00ffff (0 to 65535)                   */
	char  RAN_OFLOW;   /* Overflow byte for Random Record      */
	} ;
/*******************************************************************/
RAMETER BLOCK                                 */
/**********************************