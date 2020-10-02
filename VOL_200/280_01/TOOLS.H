/* [tools.H of JUGPDS Vol.46] */
/*
*****************************************************************
*								*
*	Written by  Hakuo Katayose (JUG-CP/M No.179)		*
*		    49-114 Kawauchi-Sanjuunin-machi		*
*		    Sendai, Miyagi 980                          *
*		    Phone: 0222-61-3219				*
*								*
*       Modifird by Toshiya Oota   (JUG-CPM No.10)              *
*                   Sakae ko-po 205 				*
*		    5-19-6 Hosoda				*
*		    Katusikaku Tokyo 124			*
*								*
*		for MS-DOS Lattice C V3.1J & 80186/V20/V30	*
*								*
*	Edited & tested by Y. Monma (JUG-C/M Disk Editor)       *
*								*
*****************************************************************
*/

/* stdio.h - Standard I/O Header for Software Tools */

/***	Definitions for MS-DOS ***/

#define LETTER	'a'
#define DIGIT	'0'
#define ALPHA	'a'

#define YES    1
#define NO     0
#define ON	YES
#define OFF	NO

#define EOS		'\0'
#define CPMBREAK	0x003
#define BELL		0x007
#define BACKSPACE	0x008
#define TAB		'\t'
#define CRETURN		'\r'
#define VT		0x00b
#define FORMFEED	0x00c
#define NEWLINE		'\n'
#define ESC		0x01b
#define SPACE		' '
#define BLANK		' '
#define CPMEOF		0x01a
#define NOT		0x021				/* Append 1986-11-24 */

#define	NPAT		0x07b				/* Append 1986-11-25 */
#define BPAT		0x07b				/* Append 1986-11-25 */
#define	NPATEND		0x07d				/* Append 1986-11-25 */
#define	EPAT		0x07d				/* Append 1986-11-25 */
#define	ANY		0x03f	/*'?'*/			/* Append 1986-11-25 */
#define	EOL		0x024	/*'$'*/			/* Append 1986-11-25 */
#define	BOL		0x025	/*'%'*/			/* Append 1986-11-25 */
#define AND		0x026	/*'&'*/			/* Append 1987-01-18 */
#define LPAREN		0x028	/*'('*/			/* Append 1987-02-08 */
#define RPAREN		0x029	/*')'*/			/* Append 1987-02-08 */
#define	CLOSURE		0x02a	/*'*'*/			/* Append 1986-11-25 */
#define	PLUS		0x02b	/*'+'*/			/* Append 1986-11-25 */
#define	CCL		0x05b	/*'['*/			/* Append 1986-11-25 */
#define LBRACKET	CCL				/* Append 1987-02-08 */
#define	CCLEND		0x05d	/*']'*/			/* Append 1986-11-25 */
#define RBRACKET	CCLEND				/* Append 1987-02-08 */
#define	CHAR		0x061	/*'a'*/			/* Append 1986-11-25 */
#define NCCL		0x06f	/*'n'*/			/* Append 1986-11-25 */
#define	SUBSTRG		0x07e	/*'~'*/			/* Append 1987-01-18 */
#define	OK		-2				/* Append 1986-11-25 */
#define	CLOSIZE		4				/* Append 1986-11-25 */
#define	ESCAPE		0x040	/*'@'*/			/* Append 1986-11-25 */
#define	DASH		0x02d	/*'-'*/			/* Append 1986-11-25 */
#define DITTO		-3				/* Append 1987-01-18 */
#define COUNT		1				/* Append 1987-01-18 */
#define START		3				/* Append 1987-01-18 */
#define PREVCL		2				/* Append 1987-01-18 */

#if LATTICE
#define STDIN		stdin
#define STDOUT		stdout
#define STDERR		stderr
#define READ		"r"
#define WRITE		"w"
#else
#define STDIN		0
#define STDOUT		1
#endif

#define	ERROR		-1

#define	TBUFSIZ		512
#define MAXSET		253
#define MAXLINE		253
#define MAXPAT          127

#define remark(str)	(printf("%s\n",(str)))
#define iswhite(c)	isspace(c)
#define FOREVER for(;;)

