/* JARIO.H        VERS:- 01.00  DATE:- 09/26/86  TIME:- 09:36:40 PM */
/* 
Description:

Header file, with various constants, including input-output.

By J.A. Rupley, Tucson, Arizona
Coded for BDS C compiler, version 1.50a
*/

#define EOF		-1	/* Common constants	*/
#define ERROR		-1
#define NULL		'\0'
#define EOS		NULL
#define TRUE		1
#define FALSE		NULL


#define CTRLA		0x01	/* nondisplayables	*/
#define CTRLB		0x02
#define CTRLC		0x03
#define CTRLD		0x04
#define CTRLE		0x05
#define CTRLF		0x06
#define CTRLG		0x07
#define CTRLH		0x08
#define CTRLI		0x09
#define CTRLJ		0x0A
#define CTRLK		0x0B
#define CTRLL		0x0C
#define CTRLM		0x0D
#define CTRLN		0x0E
#define CTRLO		0x0F
#define CTRLP		0x10
#define CTRLQ		0x11
#define CTRLR		0x12
#define CTRLS		0x13
#define CTRLT		0x14
#define CTRLU		0x15
#define CTRLV		0x16
#define CTRLW		0x17
#define CTRLX		0x18
#define CTRLY		0x19
#define CTRLZ		0x1A
#define CTRL_ESC	0x1B
#define CTRL_FS		0x1C
#define CTRL_GS		0x1D
#define CTRL_RS		0x1E
#define CTRL_US		0x1F
#define DEL		0x7F


#define CLS		putchar(26)	/* Terminal IO		*/
#define ESC		putchar(27)
#define HOME		putchar(30)
#define BUF_SIZE	80
#define SCREEN_HEIGHT	23
#define TAB		5

#define STD_IN	0			/* Fd-Iobuf values for device io */
#define STD_OUT 1
#define STD_ERR 4
#define DEV_LST 2
#define DEV_RDR 3
#define DEV_PUN 3

#define	READ	0			/* Modes for file IO	*/
#define WRITE	1
#define RW	2
#define APPEND	3

