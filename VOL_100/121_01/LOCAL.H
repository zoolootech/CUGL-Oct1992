/*
 *	This file contains all of the local definitions
 *	It is used by assorted utilities and LOCAL.C
 *	Written by Stephen M. Kenton concurrent with same
 *
 * version 1.0:	September 1985
 *	added version number
 */

/*
 *	Misc stuff that I use lots
 */
#define	DRIVES	"ABCDEFGHIJKLMNOP"
#define CR	0x0D
#define NL	0x0A
#define CHARLEN	8	/* # of bits in a byte... */
/*
 *	Random device defines for use with file I/O
 */
#define	STDIN	0	/* Input to terminal, redirected by CIO */
#define STDOUT	1	/* Output from terminal, redirected by CIO */
#define STDLST	2	/* Output to the list device */
#define STDPUN	3	/* I/O to the punch device */
#define STDERR	4	/* Output to terminal, not redirected */

/*
 *	The BIOS definitions
 */
#define	BOOT	0
#define	WBOOT	1
#define	CONST	2
#define	CONIN	3
#define	CONOUT	4
#define	LIST	5
#define	PUNCH	6
#define	READER	7
#define	HOME	8
#define	SELDSK	9
#define	SETTRK	10
#define	SETSEC	11
#define	SETDMA	12
#define	READ	13
#define	WRITE	14
#define	LISTST	15
#define	SECTRAN	16

/*
 *	The BDOS definitions
 */
#define	SYSRESET	0
#define	CONSOLEIN	1
#define	CONSOLEOUT	2
#define	READERIN	3
#define	PUNCHOUT	4
#define	LISTOUT		5
#define	DIRECTIO	6
#define	GETIOBYTE	7
#define	SETIOBYTE	8
#define	PRINTBUFF	9
#define	READBUFF	10
#define	CONSOLESTAT	11
#define	GETVERSION	12
#define	DISKRESET	13
#define	SELECTDISK	14
#define	OPENFILE	15
#define	CLOSEFILE	16
#define	SEARCHFIRST	17
#define	SEARCHNEXT	18
#define	DELETEFILE	19
#define	ERASEFILE	19
#define	READSEQ		20
#define	WRITESEQ	21
#define	MAKEFILE	22
#define	CREATEFILE	22
#define	RENAMEFILE	23
#define	GETLOGIN	24
#define	GETCURRENT	25
#define	SETADDRESS	26
#define	GETALLOC	27
#define	WRITEPROT	28
#define	GETREADONLY	29
#define	SETFILEATTR	30
#define	GETPARMS	31
#define	USERCODE	32
#define	READRAND	33
#define	WRITERAND	34
#define	FILESIZE	35
#define	SETRANDOM	36
#define	DRIVERESET	37
#define	ZWRITERAND	40

/*
 *	The FCB with optional random record number
 */
#define	FCBSIZ	36
#define	fcb	struct FCB
fcb	{
	char	fcb_dr;
	char	fcb_name[8];
	char	fcb_type[3];
	char	fcb_ex;
	char	fcb_s1;
	char	fcb_s2;
	char	fcb_rc;
	char	fcb_alv[16];
	char	fcb_cr;
	int	fcb_random;
	char	fcb_r2;
	};

/*
 *	The Disk Directory Format
 *	The added two byte NEXT field is for linked lists
 */
#define	DIRSIZ	32
#define	ENTRYSIZE 34
#define	dir	struct	DIR
dir	{
	char	dir_user;
	char	dir_name[8];
	char	dir_type[3];
	char	dir_ex;
	char	dir_s1;
	char	dir_s2;
	char	dir_rc;
	char	dir_alv[16];
	dir	*dir_next;
	};

/*
 *	The Disk Parameter Block
 */
#define	DPBSIZ	16
#define	dpb	struct DPB
dpb	{
	int	dpb_spt;
	char	dpb_bsh;
	char	dpb_blm;
	char	dpb_exm;
	int	dpb_dsm;
	int	dpb_drm;
	char	dpb_al0;
	char	dpb_al1;
	int	dpb_cks;
	int	dpb_off;
	char	dpb_fmt;
	};

/*
 *	The Disk Parameter Header
 */
#define	DPHSIZ	16
#define	dph	struct DPH
dph	{
	char	*dph_xlt;
	char	dph_junk[6];
	char	*dph_dirbuf;
	dpb	*dph_dpb;
	char	*dph_csv;
	char	*dph_alv;
	};
                                                                                                                      