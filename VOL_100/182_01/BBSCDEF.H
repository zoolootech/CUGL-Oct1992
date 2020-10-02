/*
    bbscdef.h

    Global defines and storage definitions for use by
    BBSc system.

Mike Kelly

    07/23/83 v1.1   Added #define of CLOCK, NOCLOCK, LCDAV, LCEMPTY.
    07/05/83 v1.0   Added #define DEBUG to control compilation
            of "if(debug)" code.
    07/03/83 v1.0   Added stop_that (indicates a ctl-K or K received
            deleted).
            from the keyboard in portout routine).
    06/28/83 v1.0   Added msg_delete (indicates if message is
    06/25/83 v1.0   updated
    05/17/83 v1.0   written
*/
/*
#define DEBUG     1
*/
/* comment this out so the */
                    /*  "if(debug)" code does NOT */
                    /*  get compiled */
#define DEBUG 0

/*
#define CLOCK     1
*/
/* if you have a clock board then */
                    /*  this define wants to be  */
                    /*  uncommented and NOCLOCK should */
                    /*  be commented */

#define NOCLOCK   1
            /* uncomment this if you do NOT have */
                    /*  a clock board */

/* --------------------------------------------------------------------- */
#include <stdio.h>
#define MAXLINE 132
/* MSDOS: may need to replace CPMEOF with some other symbol throughout */
#define CPMEOF 0x1a
#define TRUE      1
#define FALSE     0
#define ERROR    -1
#define OK        1
#define READ      0
#define WRITE     1
#define UPDATE    2
#define STDIN     0
#define STDOUT    1
/* --------------------------------------------------------------------- */

#define CLEAR     'E'           /* clear screen on z-19 */
#define ESC   0x1b
#define CR    0x0d
#define LF    0x0a
#define BELL      0x07          /* Avon calling */
#define CRLF      "\r\n"        /* <cr><lf> */
#define CTL_D     0x04          /* <ctl-d> */
#define CTL_K     0x0b          /* <ctl-k> */
#define CTL_X     0x18          /* <ctl-x> */

#define BREAKCNT  5000          /* max count to allow for    */
                    /*   status routines to loop */
#define STATMAX   2000          /* used by stat debug routine */

#define WELCOME   "/z/welcome.bbs"  /* file name */
#define BULLETINS "/z/bulletin.bbs" /* file name */
#define NEWUSER   "/z/newuser.bbs"  /* file name */
#define SYSTEM    "/z/system.bbs"   /* file name */
#define CALLERS   "/z/callers.bbs"  /* file name */
#define LASTCALL  "/z/lastcall.bbs" /* file name */
#define USERS     "/z/users.bbs"    /* file name */
#define HEADER    "/z/header.bbs"   /* file name */
#define MESSAGES  "/z/messages.bbs" /* file name */
#define HELP      "/z/help.bbs"     /* file name */

/*  defines used by the bbscfile.c routine     */

#define MSG1MAX 104     /* max. piece of message to put in 128 */
                /*  byte chunk of msg file */
#define MSGSECT 128     /* size of message sector */

/*          end of defines used by the bbscfile.c routine */

/* global variables */
#ifdef MAINMOD
char    portin();       /* in routine returns a character */
char    *gb4sin();      /* returns pointer to character */

FILE    *inbuf ;        /* handy input buffer pointer */
FILE    *otbuf ;        /* another handy buffer pointer */
FILE    *itbuf ;        /* pointer used to read/write user file */

char    w_fname[25];        /* first name on signon */
char    w_lname[25];        /* last  name on signon */
char    w_password[10];     /* password   on signon */
char    *passptr;       /* ptr to password */

                /* define user file fields */
char    u_fname[25];
char    u_lname[25];
char    u_password[5];
char    u_time1[12];        /* first time on system */
char    u_date1[9];     /* first date on system */
char    u_time2[12];        /* last  time on system */
char    u_date2[9];     /* last  date on system */
char    u_city[31];     /* city, state */

                /* define message file fields   */
char    msg_no[11];     /* message number */
char    msg_date[9];        /* date of message */
char    msg_time[15];       /* time of message */
char    msg_to[21];     /* who message is to */
char    msg_from[21];       /* who message is from */
char    msg_pass[11];       /* password */
char    msg_subject[21];    /* what it's all about */
char    msg_text[842];      /* text of message 20 lines */
                /*  by 40 wide */
char    msg_delete[2];      /* indicates msg status */
                /*  0 = active */
                /*  9 = deleted */


int debug;          /* switch for debug mode */
int offline;        /* switch for offline mode */
int xpert;          /* switch */
int active;         /* indicates if logged on */
int statcnt;        /* count used for status routines */
int stop_that;      /* switch - true indicates ctl-K */
                /*  or K received in portout routine */
                /* date/time fields - used by bbsclock.c */
char    mm[3];
char    dd[3];
char    yy[3];
char    month[20];      /* ie. January */
char    day[3];         /* dd */
char    year[5];        /* 19yy */
char    date[30];       /* ie. January 31, 1983 */ 
char    week[20];       /* day of week Monday, Tuesday... */
char    ttime[12];      /* time of day  HH:MM:SS xM */
                /*          where x = A or P */

/*          variables used by the bbscfile.c routine */

char    h_next_msg[10];     /* next avail. message # */
char    h_date[10];     /* last update date of header file */
int h_next;         /* next avail. message # in integer */

/*          end of variables used by bbscfile.c */

            /* end of globals */
#else

extern char    portin();       /* in routine returns a extern character */
extern char    *gb4sin();      /* returns pointer to extern character */

extern FILE    *inbuf ;        /* handy input buffer pointer */
extern FILE    *otbuf ;        /* another handy buffer pointer */
extern FILE    *itbuf ;        /* poextern inter used to read/write user file */

extern char    w_fname[25];        /* first name on signon */
extern char    w_lname[25];        /* last  name on signon */
extern char    w_password[10];     /* password   on signon */
extern char    *passptr;       /* ptr to password */

                /* define user file fields */
extern char    u_fname[25];
extern char    u_lname[25];
extern char    u_password[5];
extern char    u_time1[12];        /* first time on system */
extern char    u_date1[9];     /* first date on system */
extern char    u_time2[12];        /* last  time on system */
extern char    u_date2[9];     /* last  date on system */
extern char    u_city[31];     /* city, state */

                /* define message file fields   */
extern char    msg_no[11];     /* message number */
extern char    msg_date[9];        /* date of message */
extern char    msg_time[15];       /* time of message */
extern char    msg_to[21];     /* who message is to */
extern char    msg_from[21];       /* who message is from */
extern char    msg_pass[11];       /* password */
extern char    msg_subject[21];    /* what it's all about */
extern char    msg_text[842];      /* text of message 20 lines */
                /*  by 40 wide */
extern char    msg_delete[2];      /* indicates msg status */
                /*  0 = active */
                /*  9 = deleted */


extern int debug;          /* switch for debug mode */
extern int offline;        /* switch for offline mode */
extern int xpert;          /* switch */
extern int active;         /* indicates if logged on */
extern int statcnt;        /* count used for status routines */
extern int stop_that;      /* switch - true indicates ctl-K */
                /*  or K received in portout routine */
                /* date/time fields - used by bbsclock.c */
extern char    mm[3];
extern char    dd[3];
extern char    yy[3];
extern char    month[20];      /* ie. January */
extern char    day[3];         /* dd */
extern char    year[5];        /* 19yy */
extern char    date[30];       /* ie. January 31, 1983 */
extern char    week[20];       /* day of week Monday, Tuesday... */
extern char    ttime[12];      /* time of day  HH:MM:SS xM */
                /*          where x = A or P */

/*          variables used by the bbscfile.c routine */

extern char    h_next_msg[10];     /* next avail. message # */
extern char    h_date[10];     /* last update date of header file */
extern int h_next;         /* next avail. message # in extern integer */

/*          end of variables used by bbscfile.c */
#endif
/* end of program */
