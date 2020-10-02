/* CTRLCNST.H   VERS:- 01.00  DATE:- 09/26/86  TIME:- 09:38:57 PM */
/* 
Description: 

common system definitions...ctrl contants...#define routines

By J.A. Rupley, Tucson, Arizona
Coded for ECO C compiler, version 3.40
*/

				/* Nondisplayables		*/
#define CTRLA		0x01
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
#define CTRL_SLASH	0x1C
#define CTRL_BRACKET	0x1D
#define CTRL_ARROW	0x1E
#define CTRL_UNDERLN	0x1F
#define DEL		0x7F


				/* Common constants		*/
#define EOF		-1
#define CPMEOF		CTRLZ
#define ERROR		-1
#define OK		0
#define NULL		0
#define EOS		0
#define TRUE		1
#define FALSE		0

#define SPACE		0x20
#define BKSP		CTRLH
#define TAB		CTRLI
#define LF		CTRLJ
#define FF		CTRLL
#define CR		CTRLM
#define ESC		CTRL_ESC


				/*Televideo 920	constants	*/
#define CLEAR		CTRLZ	
#define HOME		CTRL_ARROW
#define TWIDTH	80
#define TLENGTH	24


				/*Televideo 920	routines	*/
#define CLEARS		bios(9,CLEAR)
#define HOMES		bios(9,HOME)
#define CURSOR(R,C)     bios(9,ESC);bios(9,'=');bios(9,R + ' ');bios(9,C + ' ');
#define INTOREV		_bdos(9,"\033)$")
#define OUTAREV 	_bdos(9,"\033($")
#define CURSOROFF 	_bdos(9,"\033.0$")
#define CURSORON 	_bdos(9,"\033.1$")


				/* Bios jump vectors, offset from WBOOT */
#define CONST		3	
#define CONIN		6
#define CONOUT		9
#define LIST 		0x0C
#define PUNCH		0x0F
#define READER		0x12
#define LISTST		0x2A


				/* Bios offsets and addresses	*/
#define GROUP_PNTR	0x44
#define DATE_PNTR	0x49
#define IOBASE_PNTR	0x41
#define NDBASE		0xFD00


				/*Various system routines	*/
#define KBHIT		bios(3,0)
#definå MAX(X,Y©	((X© ¾  (Y© ¿  (X© º (Y))
#define MIN(X,Y)	((X) <= (Y) ?  (X) : (Y))
#define ABS(X)		((X) <   0  ? -(X) : (X))
