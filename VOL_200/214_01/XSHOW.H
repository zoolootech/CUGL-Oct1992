/* XSHOW.H   VERS:- 01.00  DATE:- 09/26/86  TIME:- 09:37:45 PM */
/*
Description:

Header file for XSHOWR.C and XSHOWFNS.C

By J.A. Rupley, Tucson, Arizona
Coded for BDS C compiler, version 1.50a
*/

#define NSECS 16	/* Number of SECSIZ sectors transferred per read */
#define BUFSIZ (NSECS * SECSIZ * 2 + 3) 
#define PAUSE_COUNT 2	/* seconds to wait for input */
#define TAB_SPACES 8	/* number of spaces in expansion of tab character */
#define TLENGTH 23	/* (number of lines on screen) - 1 */
#define TWIDTH 79	/* (number of characters on screen line) - 1 */

#define BS 8
#define TAB 9
#define LF 10
#define FF 12
#define CR 13
#define ESC 27
#define SPACE 32
#define CNTRL_C 'C' - 0x40
#define CNTRL_S 'S' - 0x40


#define	CONST 2
#define CONIN 3
#define CONOUT 4
#define	CHAR_AVAILABLE 0xff

#define BIOS_OUTPUT(x)	bios(CONOUT, x)
#define BIOS_INPUT	bios(CONIN, 0)
#define KBHIT		(bios(CONST, 0) == CHAR_AVAILABLE)
#define CLEAR		{bios(CONOUT, ESC); bios(CONOUT, '*');}
#define HOME		{bios(CONOUT, '^' - 0x40);}
#define INSERT_LINE	{bios(CONOUT, ESC); bios(CONOUT, 'E');}
#define DELETE_LINE	{bios(CONOUT, ESC); bios(CONOUT, 'R');}
#define ERA_EOP		{bios(CONOUT, ESC); bios(CONOUT, 'Y');}
#define ERA_EOL		{bios(CONOUT, ESC); bios(CONOUT, 'T');}
#define CUR_TOP		{bios(CONOUT, ESC); bios(CONOUT, '='); bios(CONOUT, SPACE); bios(CONOUT, SPACE);}
#define CUR_BOTTOM	{bios(CONOUT, ESC); bios(CONOUT, '='); bios(CONOUT, '7'); bios(CONOUT, SPACE);}
#define CUR_UP		{bios(CONOUT, 'K' - 0x40);}
#define REV_OFF		{bios(CONOUT, ESC); bios(CONOUT, '(');}
#define REV_ON		{bios(CONOUT, ESC); bios(CONOUT, ')');}

#define FOUND 0x80
#define NOTFOUND 0x81
#define MATCH 0x82
#define NOTMATCH 0x83
#define SKIPANY 0x86
#define SKIPONE 0x87
#define IGNOREONE 0x1c
#define LINEEND 0x1d
#define LINESTART 0x7f
#define ANYALPHA 0x1e
#define ANYNUMBER 0x1f
#define SRCH_CHAR 1
#define FIRST 2
#define NEXT 3


int x, t, i, j;
int tabcount;
int start_flag, end_flag;
int top_flag, last_flag, old_flag;
int disp_sec, last_sec, old_sec;
int end_sec;
int fd;
int c1, c2, n1;
int old_c1, old_c2, old_n1;
int use_last;
int sleep_base;
int upper_case;
int f_sec, n_sec;
int srch_out;

char *p_srch1, *p_srch2;
char *f_pntr, *n_pntr;
char *start_addr, *mid_addr, *end_addr;
char *disp_pntr, *last_pntr, *old_pntr;
char buffer[BUFSIZ];
char file[30];
char pattern[80];
char input_pattern[80];
char old_pattern[80];
char string1[10];
char jbuf[JBUFSIZE];
char jbufsrch[BUFSIZ];



