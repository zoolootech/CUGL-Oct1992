/*	system definitions
**
**	Version 1.2	07-Aug-80
*/

#define	ASCII_DATE	0x20BF			/* ASCII date */	
#define CODED_DATE	ASCII_DATE+9		/* coded date */
#define	TICCNT		0x201B			/* 2ms counter */
#define	UIVEC		0x201F			/* user vector */

/*	BDOS addresses  */

#define	CBOOT		0x1800
#define	BIOS		0x2280
#define	BASE		0x4200
#define	WBOOT		BASE
#define	IOBYT		BASE+3
#define BDOS		BASE+5
#define MEM_SIZE        BASE+6

#define	FCB		BASE+92
#define	FCBDN		FCB+0		/* disk name */
#define	FCBFN		FCB+1		/* file name */
#define	FCBFT		FCB+9		/* file type */
#define	FCBRL		FCB+12		/* reel number */
#define	FCBST		FCB+13		/* status (date,flags) */
#define	FCBRC		FCB+15		/* record count */
#define	FCBCR		FCB+32		/* next record number */
#define	FCBLN		FCB+33		/* FCB length */

#define	BUFF		BASE+128
#define	TPA		BASE+0x100

/*	BDOS system calls */

#define	SYS_RESET	0
#define	RD_CON		1
#define	WR_CON		2
#define	RD_RDR		3
#define	WR_PUN		4
#define	WR_LST		5
#define	INTER_IO	7
#define	ALTER_IO	8
#define	WR_BUFFER	9
#define	RD_BUFFER	10
#define	CHECK_STATUS	11
#define	RESET_DISK	13
#define	SELECT_DISK	14
#define	OPEN_FILE	15
#define	CLOSE_FILE	16
#define	SEARCH_FIRST	17
#define	SEARCH_NEXT	18
#define	DELETE_FILE	19
#define	RD_RECORD	20
#define	WR_RECORD	21
#define	CREATE_FILE	22
#define	RENAME_FILE	23
#define	INTER_LOGIN	24
#define	INTER_DISK	25
#define	SET_DMA		26
#define	INTER_ALLOC	27
