/* HEADER:      PRINT.C
 * TITLE:       Functions to access resident portion of PRINT.COM.
 * VERSION:     1.00
 *
 * DESCRIPTION: The following functions are implemented:
 *
 *               unsigned prn_installed(void);
 *                Checks for installation of PRINT.COM
 *                Input:  nothing
 *                Output: 0x0000     PRINT NOT installed
 *                0xFFFF             PRINT is installed
 *
 *               unsigned prn_submit(char *file_name);
 *                Submits file to print queue for printing
 *                Input:  char *file pointer to file to be printed
 *                Output: 0x0000     file successfully submitted
 *                0xFFFF             PRINT not resident
 *                else               error code of PRINT
 *
 *               unsigned prn_cancel_file(char *file_name);
 *                Cancels file in print queue
 *                Input:  char *file pointer to file to be cancelled
 *                Output: 0x0000     file successfully cancelled
 *                0xFFFF             PRINT not resident
 *                else               error code of PRINT
 *
 *               unsigned prn_cancel_all(void);
 *                Cancels all files currently in print queue
 *                Input:  nothing
 *                Output: 0x0000     print queue successfully cancelled
 *                0xFFFF             PRINT not resident
 *                else               error code of PRINT
 *
 *               unsigned prn_stop(void);
 *                Stops print queue from printing
 *                Input:  nothing
 *                Output: 0x0000     print queue successfully stopped
 *                0xFFFF             PRINT not resident
 *                else               error code of PRINT
 *
 *               unsigned prn_resume(void);
 *                Resumes printing
 *                Input:  nothing
 *                Output: 0x0000     printing successfully resumed
 *                0xFFFF             PRINT not resident
 *                else               error code of PRINT
 *
 *               unsigned prn_active(void);
 *                Checks for print queue status
 *                Input:  nothing
 *                Output: 0x0000     print queue is empty
 *                0xFFFF             PRINT is active
 *
 *              Error codes:
 *                0x0001  invalid function
 *                0x0002  file not found
 *                0x0003  invalid path
 *                0x0004  too many open files
 *                0x0005  access denied
 *                0x0008  print queue full
 *                0x0009  print queue busy
 *                0x000C  filename too long
 *                0x000F  invalid drive
 *
 * KEYWORDS:    Printing
 * SYSTEM:      MS- /PC-DOS Version 3.xx
 * FILENAME:    PRINT.C
 * AUTHOR:      Michael Wiedmann, December 1990
 * COMPILERS:   Microsoft QuickC 1.0 + 2.0, MS C 5.1 + 6.0
 */

#include <stdio.h>
#include <dos.h>


/* Macros                       */

#define INT_2F()        int86(0x2F, &r, &r)
#define INT_2FX()       int86x(0x2F, &r, &r, &sr)


/* typedef for request packet   */

typedef struct _REQ_PACKET
{
 unsigned char  prio;
 unsigned       off;
 unsigned       seg;
} REQ_PACKET;



unsigned prn_installed(void)
{
 union REGS     r;

 r.x.ax = 0x0100;       /* check for installation state of PRINT        */
 INT_2F();              /* call int 2fh                                 */
 return ((r.h.al == 0xFF) ? 0xFFFF : 0x0000);
}



unsigned prn_submit(char *file)
{
 union REGS     r;
 struct SREGS   sr;
 REQ_PACKET     req_packet;

 /* check for installation of PRINT first                       */
 if (!prn_installed())
    return 0xFFFF;

 /* fill request packet with offset and segment of file         */
 req_packet.prio = 0x00;
 req_packet.off  = FP_OFF(file);
 req_packet.seg  = FP_SEG(file);

 /* fill register variables with appropriate values             */
 sr.ds  = FP_SEG(req_packet);
 r.x.dx = FP_OFF(req_packet);
 r.x.ax = 0x0101;       /* submit file function of int 2fh      */
 INT_2FX();             /* call int 2fh                         */
 if (r.x.cflag)         /* error ?                              */
    return (r.x.ax);    /* yes, return error code               */
 else
    return 0x0000;      /* no, return success                   */
}



unsigned prn_cancel_file(char *file)
{
 union REGS     r;
 struct SREGS   sr;

 /* check for installation fo PRINT first                       */
 if (!prn_installed())
    return 0xFFFF;

 /* fill in register variables                                  */
 sr.ds    = FP_SEG(file);
 r.x.dx   = FP_OFF(file);
 r.x.ax   = 0x0102;     /* cancel file function of int 2fh      */
 INT_2FX();             /* call int 2fh                         */
 if (r.x.cflag)         /* error ?                              */
    return (r.x.ax);    /* return error code                    */
 else
    return 0x0000;      /* no, return success                   */
}



unsigned prn_cancel_all(void)
{
 union REGS r;

 /* check for installation of PRINT first                       */
 if (!prn_installed())
    return 0xFFFF;

 r.x.ax = 0x0103;       /* cancel all function of PRINT         */
 INT_2F();              /* call int 2fh                         */
 if (r.x.cflag)         /* error ?                              */
    return (r.x.ax);    /* return error code                    */
 else
    return 0x0000;      /* no, return success                   */
}



unsigned prn_stop(void)
{
  union REGS r;

  /* check for installation of PRINT first                      */
  if (!prn_installed())
     return 0xFFFF;

  r.x.ax = 0x0104;      /* get status function of PRINT         */
  INT_2F();             /* call int 2fh                         */
  if (r.x.cflag)        /* error ?                              */
     return (r.x.ax);   /* yes, return error code               */
  else
     return 0x0000;     /* no, return success                   */
}



unsigned prn_resume(void)
{
 union REGS r;

 /* check for installation of PRINT first                      */
 if (!prn_installed())
    return 0xFFFF;

 r.x.ax = 0x0105;       /* end of status function of PRINT      */
 INT_2F();              /* call int 2fh                         */
 if (r.x.cflag)         /* error ?                              */
    return (r.x.ax);    /* yes, return error code               */
 else
    return 0x0000;      /* no, return success                   */
}



unsigned prn_active(void)
{
 union REGS     r;
 struct SREGS   sr;
 unsigned char far *fptr;

 /* check for installation of PRINT first                      */
 if (!prn_installed())
    return 0xFFFF;

 r.x.ax = 0x0104;       /* status function of PRINT             */
 INT_2FX();             /* call int 2fh                         */

 FP_SEG(fptr) = sr.ds;
 FP_OFF(fptr) = r.x.si;

 r.x.ax = 0x0105;       /* end of status function of PRINT      */
 INT_2F();

 if (*fptr)             /* print queue empty ?                  */
    return 0xFFFF;      /* no                                   */
 else
    return 0x0000;      /* yes                                  */
}



