/*
    bbscfile.c

Mike Kelly

    06/12/83 v1.0   written
    07/07/83 v1.0   updated
*/

/* #define DEBUG 1 */

#include "bbscdef.h"

#define LASTDATE  " 07/07/83 "

#define PGMNAME "BBSCFILE "
#define VERSION " 1.0 "

hdrwrt()        /* write the header from memory variables */
    {       /* header is a 1 record file */
    int fd;
    char    buf128[MSGSECT] ;

    if ((fd = open(HEADER,WRITE,0666)) < 0) /* open i/o */
        {
        portsout("Can't open header-file, will create it!") ;
        portsout(CRLF) ;
        if ((fd = creat(HEADER,0666)) < 0)
            {
            portsout("Can't create header-file, aborting!") ;
            portsout(CRLF) ;
            return(ERROR) ;
            }
        }
    itoa(h_next_msg,h_next) ;   /* convert int to char */
    strfill(buf128,26,MSGSECT) ;    /* init buf128 to all hex 1a */
    sprintf(buf128,"%s~%s~",    /* build record */
        h_next_msg,
        h_date) ;
    write(fd,buf128,MSGSECT) ;  /* write it */
    close(fd) ;         /* no need to leave it open */
#ifdef DEBUG
    portsout(CRLF) ;
    portsout("<<< header file written ok >>>") ;
    portsout(CRLF) ;
#endif
    return(OK) ;
    }

hdrread()       /* read the header file into memory */
    {
    int fd,
        cnt;
    char    buf128[MSGSECT];

    if ((fd = open(HEADER,READ,0666)) < 0)  /* open input */
        {
        portsout("Can't open header-file, using inital values!") ;
        portsout(CRLF) ;
        h_next = 1 ;
        h_next_msg[0] = '1' ; h_next_msg[1] = 0 ;
        h_date[0] = '0' ; h_date[1] = 0 ;
        hdrwrt() ; return ;
        }
    if((cnt=read(fd,buf128,MSGSECT)) != MSGSECT)
        {
        portsout(CRLF) ;
            portsout("<<< header read error >>>") ;
        portsout(CRLF) ;
        return(ERROR) ;
        }
    cnt = sscanf(buf128,"%[^~]~%[^~]~",
            h_next_msg,
            h_date) ;
    close(fd) ;     /* no need to leave it open */

#ifdef DEBUG
    portsout(CRLF) ;
    portsout("<<< header read, next-message='") ;
    portsout(h_next_msg) ;
    portsout("', date='") ;
    portsout(h_date) ;
    portsout("'. >>>") ;
    portsout(CRLF) ;
#endif

    if (cnt != 2)
        {
#ifdef DEBUG
    portsout(CRLF) ;
    portsout("<<< Invalid header read! >>>") ;
    portsout(CRLF) ;
#endif
        return(ERROR) ;
        }
    h_next = atoi(h_next_msg) ;
    return(OK) ;
    }

msgopen(how)
int how ;       /* how to open 0=input, 1=output, 2=i/o */
    {
    int fd ;

    if ((fd = open(MESSAGES,how,0666)) < 0) /* open i/o */
        {
        portsout("can't open message-file, will create it!") ;
        portsout(CRLF) ;
        if ((fd = creat(MESSAGES,0666)) < 0)
            {
            portsout("can't create message-file, aborting!") ;
            portsout(CRLF) ;
            return(ERROR) ;
            }
        }
#ifdef DEBUG
    portsout(CRLF) ;
    portsout("<<< message file opened ok >>>") ;
    portsout(CRLF) ;
#endif
    return(fd) ;
    }

msgclose(fd)
int fd ;
    {
#ifdef DEBUG
    portsout(CRLF) ;
    portsout("<<< closing message file >>>") ;
    portsout(CRLF) ;
#endif
    return(close(fd)) ;
    }

msgwrt(fd)      /* write the message file from memory variables */
int fd;     /* writes a message starting with the h_next msg # */
    {
    int rc,         /* return code */
        cnt1,
        cnt2,
        len;
    char    bufmsg0[MSG1MAX+1],
        buf128[MSGSECT],
        this1[10],
        next1[10];

    rc = cnt1 = len = cnt2 = 0 ;
    itoa(this1,h_next) ;                /* convert int to char */
    rc = seek(fd,h_next - 1,0) ;        /* seek next available sector */
    h_next++ ;
    itoa(next1,h_next) ;
    strfill(buf128,0,MSGSECT) ;     /* init buf128 to all hex 00 */
/*
*           build first piece of msg record
*/
    sprintf(buf128,"%s~%s~%s~%s~%s~%s~%s~%s~%s~",   /* build record */
        this1,                  /* this rcd # */
        next1,                  /*  points next rcd # */
        msg_delete,             /* delete byte */
        msg_date,
        msg_time,
        msg_to,
        msg_from,
        msg_pass,
        msg_subject);
    rc = write(fd,buf128,MSGSECT);  /* write the first 128 byte record */
                    /*  for a message record */
/*
*           build the n+1 piece of msg record
*/

    len = (strlen(msg_text) / MSG1MAX) + 1; /* calc how many more 128 */
                        /*  byte records to write */
    cnt2 = 1 ;              /* init for substr */
    while (len--)
        {
        itoa(this1,h_next);     /* calc/convert record #'s */
        h_next++;
        if (len == 0)
            {
            strcpy(next1,"0");  /* marks last 128 byte piece */
            }           /*  of a msg */
        else
            {
            itoa(next1,h_next);
            }
        strfill(bufmsg0,0,MSG1MAX);
        substr(msg_text,bufmsg0,cnt2,MSG1MAX); /* mv MSG1MAX to buff */
        cnt2 += MSG1MAX;        /* up cnt2 by MSG1MAX */
        strfill(buf128,0,MSGSECT);  /* init buf128 to all hex 00 */
        sprintf(buf128,"%s~%s~%s~%s~",
            this1,          /* this rcd # */
            next1,          /* point to next rcd # */
            msg_delete,     /* delete byte */
            bufmsg0);       /* piece of msg */
        rc = write(fd,buf128,MSGSECT);  /* write n+1 128 byte record */
        }

    strfill(buf128,26,MSGSECT);     /* fill with all hex 1a */
    rc = write(fd,buf128,MSGSECT);  /* write all hex 1a 128 byte record */
    return(OK);
    }

msgrewrt(fd,r_msg)  /* re-write the message file from memory variables */
int fd,     /* re-writes only the 1st part of a message */
    r_msg;      /* used to update the delete byte */
    {
    int rc,         /* return code */
        cnt1,
        file_size;
    char    buf128[MSGSECT],
        this1[10],
        next1[10];

    rc = cnt1 = 0;
    if (r_msg > h_next) /* don't try to seek past end of file */
        {
        return(ERROR);
        }
    if ((rc = seek(fd,r_msg-1,0)) == ERROR) /* seek to requested sector */
        {
        return(ERROR);
        }
    itoa(this1,r_msg);      /* convert int to char */
    r_msg++;
    itoa(next1,r_msg);
    strfill(buf128,0,MSGSECT);      /* init buf128 to all hex 00 */
/*
*           build first piece of msg record
*/
    sprintf(buf128,"%s~%s~%s~%s~%s~%s~%s~%s~%s~",   /* build record */
        this1,                  /* this rcd # */
        next1,                  /* points next rcd # */
        msg_delete,             /* delete byte */
        msg_date,
        msg_time,
        msg_to,
        msg_from,
        msg_pass,
        msg_subject);
    rc = write(fd,buf128,MSGSECT);  /* write the first 128 byte record */
                    /*  for a message record */
    return(OK);
    }


msgread(fd,msgno)       /* read message number requested */
int fd,         /* returns ERROR if msg past eof */
    msgno;          /* returns 0 if msg is not 1st piece */
                /*   of a message */
                /* returns 0 if msg is deleted */
                /* returns msg # if successful */
    {
    int rc,         /* return code */
        cnt1,
        cnt2,
        len,
        next,
        ret_this,
        file_size;
    char    bufmsg0[MSG1MAX+1],
        buf128[MSGSECT+256],
        buftmp[MSGSECT+256],
        this1[10],
        next1[10];

    if (msgno > h_next)     /* don't try to seek past end of file */
        {
        return(ERROR);
        }
    if ((rc = seek(fd,msgno-1,0)) == ERROR)
        {
        portsout(CRLF) ;
        portsout("Can't seek on message-file!") ;
        portsout(CRLF) ;
        return(ERROR);      /* when cant find it */
        }
    if (read(fd,buf128,MSGSECT) != MSGSECT) /* read 128 byte sector */
        {
        portsout(CRLF) ;
        portsout("Can't read in message-file!") ;
        portsout(CRLF) ;
        return(ERROR);
        }
/*
*           get first piece of msg record
*/
/* do trial read, since if not first record, fields might overflow */
rc = sscanf(buf128,"%[^~]~%[^~]~%[^~]~%[^~]~%[^~]~%[^~]~%[^~]~%[^~]~%[^~]~",
    buftmp,buftmp,buftmp,buftmp,buftmp,buftmp,buftmp,buftmp,buftmp) ;
    if (rc != 9)        /* makes sure we read the 1st piece */
        {       /*  of a message and not in the middle */
        return(0);  /* 0 when is not the msg header */
        }
/* now do the real read since looks like is a good record */
rc = sscanf(buf128,"%[^~]~%[^~]~%[^~]~%[^~]~%[^~]~%[^~]~%[^~]~%[^~]~%[^~]~",
        this1,              /* this rcd # */
        next1,              /*  points to next rcd # */
        msg_delete,         /* delete byte */
        msg_date,
        msg_time,
        msg_to,
        msg_from,
        msg_pass,
        msg_subject);
    if (rc != 9)        /* makes sure we read the 1st piece */
        {       /*  of a message and not in the middle */
        return(0);  /* 0 when is not the msg header */
        }

    if (msg_delete[0] == '9')   /* check for deleted messages */
        {           /*  if so, return as if not found */
        return(0);
        }

    ret_this = atoi(this1); /* return this msg no. */
    next = atoi(next1);
    strcpy(msg_no,this1);
    msg_text[0] = '\0';
    while (next)            /* read until no more pieces for */
        {           /*  this message */
        if (read(fd,buf128,MSGSECT) != MSGSECT) /* read next sector */
            {
            portsout(CRLF) ;
            portsout("Can't read in message-file(2)!") ;
            portsout(CRLF) ;
            return(ERROR);
            }
        strfill(bufmsg0,0,MSG1MAX); /* init bufmsg0 to all hex 00 */
        rc = sscanf(buf128,"%[^~]~%[^~]~%[^~]~%[^~]~",
            this1,          /* this rcd # */
            next1,          /* point to next rcd # */
            msg_delete,     /* delete byte */
            bufmsg0);       /* piece of msg */
        next = atoi(next1);
        strcat(msg_text,bufmsg0);   /* tag piece of msg to */
                        /*  whole msg array */
        }
    return(ret_this);   /* if all ok, return the msg no. found */
    }

/*  end of program      */