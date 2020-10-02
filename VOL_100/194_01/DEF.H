/* [DEF.H of JUGPDS Vol.17]
*****************************************************************
*								*
*	Written by  Hakuo Katayose (JUG-CP/M No.179)		*
*		    49-114 Kawauchi-Sanjuunin-machi		*
*		    Sendai, Miyagi 980                          *
*		    Phone: 0222-61-3219				*
*								*
*	Edited & tested by Y. Monma (JUG-C/M Disk Editor)       * 
*								*
*****************************************************************
*/

/* Table of Definitions for Software Tools */

#define MAXTOK		32
#define MAXDEF         128
#define MAXPAT	       256

#define NPAT		'{'
#define NPATEND		'}'
#define AND		'&'
#define ANY		'?'
#define BOL		'%'
#define EOL		'$'
#define CCL		'['
#define NCCL		'n'
#define CCLEND		']'
#define CLOSURE		'*'
#define CHAR		'a'
#define DASH		'-'
#define PLUS		'+'
#define LETTER		'a'
#define DIGIT		'0'
#define NOT		'!'
#define MINUS		'-'
#define UPALLOW		'^'
#define ESCAPE		'@'

#define	EOS		'\0'
#define	BACKSPACE	0x08
#define	TAB		0x09
#define	NEWLINE		0x0A
#define	FORMFEED	0x0C
#define	CRETURN		0x0D
#define	BLANK		' '

#define DITTO		4
#define BPAT		7
#define EPAT		8
#define SUBSTRG		9

#define ON		1
#define OFF		0

#define COUNT		1
#define PREVCL		2
#define START		3
#define CLOSIZE		4

