/* commsupp.h	 - commsupp declares and defines */

/*
  init_com   - initialize comm port, interrupts, baud, data, parity, stop, buf
  uninit_com - disable interrupts, flushes buffer
  send_brk   - send break signal
  set_xoff   - enable XON, XOFF protocol
  recd_xoff  - return true if XOFF received, reset recd_xoff flag
  sent_xoff  - return true if XOFF sent, reset sent_xoff flag
  inp_cnt    - returns the number of unread bytes in receive buffer
  inp_char   - return one byte from buffer
  inp_strn   - transfer n bytes from buffer to string
  outp_char  - send one byte
  outp_strn  - send n bytes of a string, up to end of string
  com_stat   - return line status (high byte) and modem status (low byte)
  on_com     - specify a routine to execute when a byte is received
*/

/* declare entry points to commsupp routines */

int  init_com();   /* init_com(port,baud,parity,data,stop,buff_len); */
int  uninit_com(); /* uninit_com(port,lv_open);  */
int  send_brk();   /* send_brk(port);		 */
int  set_xoff();   /* set_xoff(port,xoff_state); */
int  recd_xoff();  /* recd_xoff(port);		 */
int  sent_xoff();  /* sent_xoff(port);		 */
int  inp_cnt();    /* inp_cnt(port);		 */
int  inp_char();   /* inp_char(port);		 */
int  inp_strn();   /* inp_strn(port,&str,cnt);	 */
int  outp_char();  /* outp_char(port,c);	 */
int  outp_strn();  /* outp_strn(port,&str,cnt);  */
int  com_stat();   /* com_stat(port);		 */
int  on_com();	   /* on_com(port,routine);	 */

/* define for values returned by routines */
#define ERROR	  -1
#define TRUE	  1
#define FALSE	  0

/* defines for parity (init_com) */
#define PARITY_NONE  0
#define PARITY_ODD   1
#define PARITY_EVEN  2
#define PARITY_MARK  3
#define PARITY_SPACE 4

/* defines for comm errors - mask using com_stat value */
#define OVER_RUN    0x0200
#define PARITY_ERR  0x0400
#define FRAME_ERR   0x0800
#define BREAK_IND   0x1000

/* defines for modem conditions - mask using com_stat value */
#define CLEAR_SEND  0x0010
#define DATASET_RDY 0x0020
#define RING_IND    0x0040
#define CARRIER     0x0080

/* end of commsupp.h */
                                   