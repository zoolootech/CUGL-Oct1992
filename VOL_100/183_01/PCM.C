/*
  PCM.C  - a communications program for fast access to mainframes
               with asyn communications and to provide a file
               transfer utility

  John Hurst,  July 1984
  adapted from Commterm by Tom Poindexter, March 1984

*/

#include "comsup.h"

#define ALT_2  0x7900  /* userid */
#define ALT_5  0x7c00  /* userid */

#define F1     0x3B00  /* break */
#define ALT_B  0x3000  /* break */
#define ALT_D  0x2000  /* DPS8 download */
#define ALT_E  0x1200  /* echo  */
#define ALT_H  0x2300  /* help  */
#define ALT_Q  0x1000  /* resume*/
#define ALT_R  0x1300  /* receive */
#define ALT_S  0x1f00  /* pause */
#define ALT_T  0x1400  /* transmit */
#define ALT_U  0x1600  /* DPS8 upload */
#define ALT_X  0x2d00  /* exit  */
#define PGUP   0x4900  /* spare */
#define PGDN   0x5100  /* spare */
#define C_END  0x7500  /* break */
#define C_PGDN 0x7600  /* transmit oct2, ascii 1 */

#define NUL    0
#define BS     8
#define LF     10
#define CR     13
#define XON    17
#define XOFF   19
#define ENDFILE 26
#define ESC     27
#define RUBOUT  127
#define HDEL   64
#define DELAY  5000

#define IPORT     2
#define IBAUD     2400
#define IPARITY   2
#define IDATA     7
#define ISTOP     1
#define IBUFFER   1024
#define IFLOW     FALSE
#define IECHO     1


int max;
unsigned total,min_sp;

#include "stdio.h"

main (argc, argv)
int argc;
char *argv[];

{
  int port,baud,parity,data,stop,buff_len;
  int a,c,e,x,i,l,n,x_state,counter;
  int bytecount();
  FILE f;
  char strn1[100],temp[100];

  puts("\nPCM ... Personal Communications Manager / File transfer utility ");
  puts("\n        John Hurst, 1984  (adapted from Commterm by Tom Poindexter)\n");

  port = IPORT;
  baud = IBAUD;

  for (i = 1; i < argc; i++ )
    switch ( l = atoi(*++argv) )
     {
      case 1:
       port = 1;
       break;
      case 2:
       port = 2;
       break;
      case 300:
       baud = 300;
       break;
      case 1200:
       baud = 1200;
       break;
      case 2400:
       baud = 2400;
       break;
      default:
       break;
     }

  puts("\n        operating from COM");
  itoa(port, strn1);
  puts(strn1);
  puts(" at ");
  itoa(baud, strn1);
  puts(strn1);
  puts(" baud");

  parity = IPARITY;
  data = IDATA;
  stop = ISTOP;
  buff_len = IBUFFER;
  x_state = IFLOW;
  e = IECHO;

  if (init_com(port,baud,parity,data,stop,buff_len) == ERROR)
   {
    putchar(7);
    puts("\n\nSorry, cannot open COM port as defined... \n\n");
    exit();
   }
  else
   {
    set_xoff(port,x_state);
    on_com(port,bytecount);
   }

  puts("\n\nPress ALT-H for help on special keys.\n\n");
  total = max = c = n = 0;
  min_sp = 0xffff;

  while (c != ALT_X)
   {

    termin(port,1);

    if (com_stat(port) & BREAK_IND)
      puts("\n ** Break ** \n");

    c = key_in();
    if (c > 0)
     {
      switch (c)
       {
        case ALT_H:
         puts("\n\nPCM special keys\n\n");
         puts("press ....... for\n");
         puts("Alt-E.........echo key strokes\n");
         puts("Alt-H.........help on keys\n");
         puts("Alt-X.........exit program\n");
         puts("Alt-R.........receive a file\n");
         puts("Alt-D.........download (recv) DPS8 file\n");
         puts("Alt-T.........transmit a file\n");
         puts("Alt-U.........upload (transmit) DPS8 file\n");
         puts("Alt-S.........pause (XOFF)\n");
         puts("Alt-Q.........resume (XON)\n");
         puts("F1............DOS command\n");
         puts("Esc...........IBM PC backspace\n");
         puts("Backspace.....DPS8 delete\n");
         puts("Ctrl-End......send break signal\n");
         puts("Ctrl-PgDn.....send 2400 baud code\n");
         puts("\n\nproceed...\n\n");
         break;

        case ALT_B:
         send_brk(port);
         break;

        case C_END:
         send_brk(port);
         break;

        case ALT_U:
         up_load(port,e,x_state,buff_len,TRUE);
         break;

        case ALT_T:
         up_load(port,e,x_state,buff_len,FALSE);
         break;

        case ALT_D:
         down_load(port,e,x_state,buff_len,TRUE);
         break;

        case ALT_R:
         down_load(port,e,x_state,buff_len,FALSE);
         break;

        case ALT_E:
         e = !e;
         break;

        case F1:
         puts("\nEnter DOS command string ? ");
         gets(strn1);
         strcpy(temp,"/c:");
         strcat(temp,strn1);
         n = exec("command.com",temp);
		 if (n != 0)
		 	puts("\nExec failed...\n");
		 else
		 	puts("\n\nExec successful, proceed...\n");
         break;

        default:
         conout(c,e,port);
         break;
       }
     }
   }

  puts("\n\nExiting..\n");
  puts("\nLeave port initialized (y or n)? ");
  getchar(a);
  if ( isupper(a) ) a = tolower(a);
  x = (a == 'y') ? TRUE : FALSE;

  uninit_com(port,x);
  puts("\n");

/*  printf("\nThe buffer high water mark reached %d of a buffer length of %d",
         max,buff_len);

  printf("\nThe total number of characters received was %u",
         total);

  printf("\nThe smallest stack space available was %u \n",
         min_sp);  */

}  /* end of commterm.c */

bytecount () /* on comm routine */
{
 int m;

 m = _showsp();
 if (m < min_sp)
   min_sp = m;
 total++;
}
