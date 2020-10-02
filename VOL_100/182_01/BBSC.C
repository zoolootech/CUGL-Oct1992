/*
    bbsc.c

    BBS (Bulletin Board System) written in UNIX SYSTEM-III "C".

        Support files needed:
            bbscdef.h
            bbscport.o or bbscport.c
            bbsclock.o or bbsclock.c
            bbscfile.o or bbscfile.c
            bbscmisc.o or bbscmisc.c

                Mike Kelly

    03/09/83 v1.0   written
    07/07/83 v1.0   updated
*/
#define MAINMOD 1
#include "bbscdef.h"

#define LASTDATE  " 02/07/84 "

#define PGMNAME "BBSc    "
#define VERSION " 1.1 "


main()
    {
    int i;
/*
*       init global variables
*/
    debug   = FALSE;
    statcnt = 0;
    xpert   = FALSE;
    active  = TRUE;

    w_fname[0] = '\0';
    w_lname[0] = '\0';
    strcpy(w_password,"MPK0");

    u_fname[0] = '\0';
    u_lname[0] = '\0';
    strcpy(u_password,"MPK0");
    u_time1[0] = '\0';
    u_date1[0] = '\0';
    u_time2[0] = '\0';
    u_date2[0] = '\0';
    u_city[0]  = '\0';

    mm[0]    = '\0';
    dd[0]    = '\0';
    yy[0]    = '\0';
    month[0] = '\0';
    day[0]   = '\0';
    year[0]  = '\0';
    date[0]  = '\0';
    week[0]  = '\0';
    ttime[0] = '\0';

    strcpy(msg_delete,"0");
    msg_no[0]   = '\0';
    msg_date[0] = '\0';
    msg_date[0] = '\0';
    msg_time[0] = '\0';
    msg_to[0]   = '\0';
    msg_from[0] = '\0';
    msg_pass[0] = '\0';
    msg_subject[0] = '\0';
    msg_text[0] = '\0';

    stop_that = FALSE;

/*   get date and time from the clock   */

    gettime(ttime);
    getdate(mm,dd,yy,month,day,year,date,week);

    portinit() ;    /* init terminal modes */
    signon()   ;
    driver()   ;
    portrst()  ;    /* undo any special modes */        
    }
/*  end of mainline routine     */

driver()
    {
char    linebuf[MAXLINE],
    buf[128],
    *bufptr,        /* ptr to buf */
    usercd[100],        /* user record */
    *userptr,       /* ptr to usercd */
    cmd[2];

int cnt,
    ok_sw,
    case_sw,
    i,
    num0,
    num1,
    num2;

    cnt = 0;
    ok_sw   = TRUE;
    bufptr  = buf;
    xpert   = FALSE;
    cmd[0]  = '\0';

    while (active)
        {
        stop_that = FALSE ;     /* reset switch */
        portsout(CRLF);
        if (!xpert)
            {
            portsout("Commands: B,C,E,G,K,N,Q,R,S,W,X or ? ");
            }
        portsout("===> ");
        portsin(cmd,1);
        portsout(CRLF);

        *cmd = toupper(*cmd) ;

        switch (cmd[0])
            {
            case ('C'):     /* Unix */
                cmd_c();
                break;
            case ('E'):     /* enter msg */
                cmd_e();
                break;
            case ('G'):     /* goodby */
                cmd_g();
                break;
            case ('K'):     /* kill msg */
                cmd_k();
                break;
            case ('Q'):     /* quick scan */
                cmd_q();
                break;
            case ('R'):     /* read msg */
                cmd_r();
                break;
            case ('S'):     /* scan msg */
                cmd_s();
                break;
            case ('X'):     /* expert toggle */
                cmd_x();
                break;
            case ('N'):     /* print new-user stuff */
                cmd_p(NEWUSER);
                break;
            case ('W'):     /* print welcome file */
                cmd_p(WELCOME);
                break;
            case ('B'):     /* print bulletins */
                cmd_p(BULLETINS);
                break;
            case ('?'):     /* help */
                cmd_p(HELP);
                break;
            default:
                break;
            }

        }
    }
/*  end of function     */

cmd_g()         /* good-by */
    {
    active = FALSE;
    portsout("\r\nThanks for calling ");
    portsout(w_fname) ;
    portsout(CRLF)    ;
    sleep(5)          ;     /* for the Smartmodem */
    portsout("+")     ;     /* get the modems attention */
    portsout("+")     ;     /* get the modems attention */
    portsout("+")     ;     /* get the modems attention */
    sleep(15)         ;         /* for the Smartmodem */
    gobble()          ;
    portsout("ATH")   ;     /* hang up the phone */
    portsout(CRLF)    ;
    sleep(15)         ;         /* for the Smartmodem */
    }
/*  end of function     */

cmd_c()         /* go to the operating system level */
    {
    if ((inbuf=fopen(SYSTEM,"r")) == NULL)
        {
        portsout(CRLF) ;
        portsout("Exiting to the operating system level!") ;
        portsout(CRLF) ;
        }
    else
        {
        porttype(inbuf);    /* type a file to port */
        close(inbuf);
        }
    active = FALSE;
    }
/*  end of function     */

cmd_e()         /* enter a message */
    {
    int entering,
        editing,
        cnt1;
    char    msg[50],
        l_cnt[3],
        ans[2];

    entering = editing = TRUE;
    cnt1 = 0;
    l_cnt[0] = '0';         /* init line count */
    msg_text[0] = '\0';     /* init message text area */

    portsout(CRLF);
    portsout("This will be message # ");
    portsout(h_next_msg);
    portsout(CRLF);
    portsout("                             ____.____1____.____2\r\n");
    portsout("Who is the message for  ===> ");
    portsin(msg_to,20);
    portsout(CRLF);
    portsout("What is the subject     ===> ");
    portsin(msg_subject,20);
    portsout(CRLF);

    portsout(CRLF) ;
    portsout("A line can contain 40 characters, up to 20 lines.") ;
    portsout(CRLF) ;
    portsout("To end, enter a carriage return on an empty line.") ;
    portsout(CRLF) ;
    portsout(CRLF) ;
    portsout("    .___.____1____.____2____.____3____.____4") ;
    portsout(CRLF) ;

    while (entering)        /* get the text of the message */
        {
        linecnt(l_cnt);     /* calc line count */

        portsout(l_cnt);    /* show line count */
        portsout("> ");     /*  and prompt */

        portsin(msg,40);
        portsout(CRLF);

        strcat(msg,"\n");   /* put a <lf> on each line */

        if (strlen(msg) < 2)        /* count cr too */
            {
            entering = FALSE;   /* all done */
            }
        else
            {
            strcat(msg_text,msg);   /* append each line to text */
            }
        if ((++cnt1) > 19)      /* only allow 20 lines */
            {
            entering = FALSE;   /* all done */
            }
        }

    while (editing)     /* save it, fix it, list it, or get out */
        {
        portsout(CRLF);
        portsout("(S)ave,   (L)ist,   or (Q)uit") ;
        portsout(CRLF);
        portsout("(D)elete, (I)nsert, or (E)dit line ==> ") ;
        portsin(ans,1);         /* get a byte */ 
        portsout(CRLF);

        *ans = toupper(*ans) ;

        switch (ans[0])         /* get 1 byte */
            {
            case ('S'):     /* save message */
                savemsg();
                editing = FALSE; /* get out of while */
                break;
            case ('E'):     /* edit message */
            case ('I'):     /* edit message */
            case ('D'):     /* edit message */
                editmsg(*ans);
                break;
            case ('L'):     /* list message */
                listmsg();
                break;
            case ('Q'):     /* get out w/out saving */
                editing = FALSE; /* get out of while */
                break;
            default:
                portsout(CRLF);
                portsout("Shall we try that one again?\r\n");
                break;
            }   
        }

    }
/*  end of function     */

linecnt(lcnt)           /* 2 byte string of line numbers */
char    *lcnt;
    {
    int icnt;
    char    xcnt[4];

    icnt = atoi(lcnt);
    if (++icnt < 10)
        {
        strcpy(lcnt,"0");
        itoa(xcnt,icnt);
        strcat(lcnt,xcnt);
        }
    else
        {
        itoa(xcnt,icnt);
        strcpy(lcnt,xcnt);
        }
    return;
    }
/*  end of function     */

savemsg()               /* save a message */
    {
    int fd;

    portsout("Saving...\r\n");

    strcpy(msg_delete,"0");     /* delete byte */
    strcpy(msg_date, mm);
    strcat(msg_date,"/");
    strcat(msg_date, dd);
    strcat(msg_date,"/");
    strcat(msg_date, yy);

    strcpy(msg_time,ttime);     /* and the time */

    strcpy(msg_from,w_fname);   /* and the from name */
    strcat(msg_from," ");
    strcat(msg_from,w_lname);

    portsout("Updating the message file...\r\n");
    if ((fd = msgopen(2)) == ERROR)     /* open i/o */
        {
        return(ERROR);
        }
    msgwrt(fd);     /* write the message */
    msgclose(fd);

    strcpy(h_date,msg_date);
    portsout("Updating the header  file...\r\n");
    hdrwrt();       /* update the header file */
    }
/*  end of function     */


listmsg()
    {
    char    *ptr,
        byte,
        lcnt[3];
    int i;

    ptr = msg_text;     /* ptr points to start of text */
    lcnt[0] = '0';      /* start linecount at zero     */
    
    portsout(CRLF) ;
    while (*ptr != 0)
        {
        linecnt(lcnt);
        portsout(lcnt);         /* put out line number */
        portsout(">");
        while (byte = *ptr++)
            {
            if (byte == '\n')   /* we dont save <cr>, so add */
                {       /*  it here when find a <cr> */
                portsout(CRLF) ;
                break ;
                }
            else portout(byte) ;
            }
        }
    }
/*  end of function     */

editmsg(tflag) char tflag ;
    {
    char    ans[3] ;
    int lnum ;      /* line number to edit */

    while(1)
        {
        portsout(CRLF);
        portsout("Enter line number to ") ;
        if(tflag == 'E') portsout("edit,") ;
        else if(tflag == 'D') portsout("delete,") ;
        else if(tflag == 'I') portsout("insert after,") ;
        else return ;
        portsout(CRLF);
        portsout("   or RETURN to exit ==> ");
        portsin(ans,2);     /* only 20 lines possible */
        portsout(CRLF);
        lnum = atoi(ans);
        if (lnum <= 0)
            {
            break;      /* out of while and return */
            }
        if ((lnum < 20) || ((lnum == 20) && (tflag != 'I')))
            {
            editline(lnum,tflag) ; break ;
            }
        else            /* else slap on hand and loop around */
            {
            portsout("Aw come on, there can only be 20 lines!");
            portsout(CRLF);
            portsout("Shall we try that again?");
            portsout(CRLF);
            }
        }       
    }
/*  end of function     */

editline(line_no,tflag)
int line_no ; char tflag ;
    {
    char    work_msg[(20*40)+40],   /* work message area */
        *work_ptr,      /* pointers */
        *msg_ptr,
        *new_ptr,
        new_line[40+1],     /* new line to replace in text */
        byte ;
    int i ;         /* a counter */

    work_msg[0] = '\0';     /* init strings */
    new_line[0] = '\0';
    i = 1 ;             /* assume at line one */

    work_ptr = work_msg;    /* put the addr of work_msg in the pointer */
    msg_ptr  = msg_text;    /* put the addr of the global message in ptr */
    new_ptr  = new_line;    /* the new line too */

    while(byte = *msg_ptr)  /* if at the line then found it's start */
        {
        if(line_no == i) break ;    /* found the line        */
        while(byte = *msg_ptr++)    /* move msg to work area */
            {           /*  until end of msg */
            *work_ptr++ = byte;
            if (byte == '\n')   /* end of line? */
                {
                i++ ; break ;   /* then stop moving */
                }
            }
        }
    if (byte == 0)  /* if true then there ain't no such beast */
        {
        portsout("Hmmm, can't seem to find that line");
        portsout(CRLF);
        portsout("   give me another chance, ok?");
        portsout(CRLF);
        return;     /* here we go round the mulberry bush */
        }

    portsout(CRLF);
    portsout("Line to ") ;
    if(tflag == 'E')
        { portsout("edit") ; }
    else if(tflag == 'I')
        { portsout("insert after"); }
    else if(tflag == 'D')
        {
        portsout("delete");
        }
    else return ;

    portsout(" is:") ;
    portsout(CRLF) ;
    portsout(">") ;

    while(byte = *msg_ptr++)        /* show the line to edit */
        {
        if(tflag == 'I')
            {
            *work_ptr++ = byte ;    /* if adding new line copy */
            }
        if(byte == '\n') break ;
        portout(byte);
        }
    portsout(CRLF) ;

    if(tflag == 'D')
        {
        portsout("Are you sure? (Y/N) ==> ");
        byte = portin() ;   /* get answer */
        portout(byte) ;     /* send it back */
        if ((byte != 'Y') && (byte != 'y')) return ;
        }
    else
        {
        portsout("Enter new line:") ;
        portsout(CRLF) ;
        portsout(">") ;
        portsin(new_line,40) ;
        portsout(CRLF) ;
        strcat(new_line,"\n\0") ;       /* lf at end new line */
        while (*work_ptr++ = *new_ptr++)    /* tag on new line to */
            { ; }               /*  work area */
        *work_ptr--;                /* back up one */
        }

    while (*work_ptr++ = *msg_ptr++)    /* now move in rest of */
        { ; }               /*  message to work area */
    *work_ptr++ = '\0';     /* for luck */

    strcpy(msg_text,work_msg);  /* replace the message with the */
                    /*  new line inserted */
    }
/*  end of function     */

cmd_k()
    {
    char    ans[11];
    int kill_msg,
        fd,
        rc;

    portsout(CRLF) ;
    portsout("Enter message number to delete ==> ") ;
    portsin(ans,10) ;
    portsout(CRLF) ;
    kill_msg = atoi(ans) ;
    fd = msgopen(2) ;       /* open i/o */
    if ((rc = msgread(fd,kill_msg)) == ERROR || rc == 0)
        {
        portsout("Can't delete that message!") ;
        portsout(CRLF) ;
        return ;
        }
    portsout("Deleting message...") ;
    portsout(CRLF) ;
    strcpy(msg_delete,"9") ;        /* mark for deletion */
    if (msgrewrt(fd,kill_msg) == ERROR) /* re-write the record just read */
        {
        portsout("Sorry, can't delete that message");
        portsout(CRLF);
        return;
        }
    msgclose(fd);
    portsout("Message ");
    portsout(ans);
    portsout(" has been deleted!") ;
    portsout(CRLF);
    return;
    }
/*  end of function     */

cmd_q()
    {
    char    ans[11];
    int q_msg,
        fd,
        rc;

    if ((fd = msgopen(0)) == ERROR)     /* open input */
        {
        return(ERROR);
        }
    portsout(CRLF);
    portsout("Enter starting message number -- RETURN to exit  ==> ");
    portsin(ans,10);
    q_msg = atoi(ans);
    if (q_msg == 0)         /* = 0 all done */
        {
        msgclose(fd);
        return;
        }
    portsout(CRLF);
    portsout(CRLF);
    portsout("No.      Date            Subject") ;
    portsout(CRLF);
    portsout("---------------------------------------");
    portsout(CRLF);

    while ((rc = msgread(fd,q_msg++)) != ERROR)
        {           /* read until eof or error */
        if (rc == 0)
            {
            continue;   /* not a valid msg */
            }
        portlsout(msg_no  , 6) ;
        portlsout(msg_date,12) ;
        portsout(msg_subject ) ;
        portsout(CRLF);
        if (stop_that)      /* ctl-K ? */
            {
            stop_that = FALSE;
            break;
            }
        }
    msgclose(fd);
    }
/*  end of function     */

cmd_r()
    {
    char    msgno[10],
        *text,
        byte0;
    int msg,
        fd,
        rc;

    if ((fd = msgopen(0)) == ERROR)     /* open input */
        {
        return(ERROR);
        }
    portsout("\r\n\n");
    portsout("Enter the message number to read");
    portsout(" - RETURN to exit   ==> ");

    while (!stop_that)
        {
        portsin(msgno,9);
        portsout(CRLF);
        msg = atoi(msgno);  /* make the no. integer */
        if (msg == 0)
            {
            break;
            }
        if ((rc = msgread(fd,msg)) == 0 || rc == ERROR)
            {
            portsout("Message ");
            portsout(msgno);
            portsout(" not found\r\n");
            }
        else
            {
            portsout(CRLF);
            portsout("No. ");
            portsout(msg_no);
            portsout("  ");
            portsout(msg_date);
            portsout("  ");
            portsout(msg_time);
            portsout(CRLF);

            portsout("From: ");
            portsout(msg_from);
            portsout("   To: ");
            portsout(msg_to);
            portsout(CRLF);

            portsout("Subject: ");
            portsout(msg_subject);
            portsout(CRLF);
            portsout(CRLF);

            text = msg_text;
            while (byte0 = (*text++))
                {
                if (byte0 == '\n')
                    {
                    portout('\r');
                    }
                portout(byte0);
                if (stop_that)
                    {
                    stop_that = FALSE;
                    msgclose(fd);
                    return;
                    }
                }
            }
        portsout(CRLF);
        portsout("Number or RETURN to exit ==> ") ;
        }
    msgclose(fd);
    }
/*  end of function     */

cmd_s()
    {
    char    ans[11];
    int q_msg,
        fd,
        rc;

    if ((fd = msgopen(0)) == ERROR)     /* open input */
        {
        return(ERROR);
        }
    portsout(CRLF);
    portsout("Enter starting message number -- RETURN to exit ) ==> ") ;
    portsin(ans,10);
    q_msg = atoi(ans);
    if (q_msg == 0)
        {
        return;
        }
    portsout(CRLF);
    portsout(CRLF);
    portsout("No.     Date          From         ") ;
    portsout("      To                    Subject") ;
    portsout(CRLF);
    portsout("---------------------------------------") ;
    portsout("---------------------------------------") ;
    portsout(CRLF);

    while ((rc = msgread(fd,q_msg++)) != ERROR)
        {           /* read until eof or error */
        if (rc == 0)
            {
            continue;   /* not a valid msg */
            }
        portlsout(msg_no  , 6) ;
        portlsout(msg_date,12) ;
        portlsout(msg_from,21) ;
        portlsout(msg_to  ,21) ;
        portsout(msg_subject)  ;
        portsout(CRLF);
        if (stop_that)
            {
            stop_that = FALSE;
            break;
            }
        }
    msgclose(fd);
    }
/*  end of function     */

cmd_x()
    {
    xpert = !xpert;     /* flip the expert mode */
    }
/*  end of function     */

cmd_p(name) char *name ;    /* type a file */
    {
    if ((inbuf=fopen(name,"r")) == NULL)
        {
        portsout("\r\nSorry, needed file not available\r\n");
        }
    else
        {
        porttype(inbuf);    /* type a file to port */
        close(inbuf);       /* close it to free up fcb */
        }
    }
/*  end of function     */

signon()    
    {
    char    byte0 ;

    int i ;
    int tfd ;

    portsout(PGMNAME)   ;
    portsout("Version") ;
    portsout(VERSION)   ;
    portsout(LASTDATE)  ;
    portsout(CRLF)      ;
/*
*           type welcome file
*/
    if ((inbuf=fopen(WELCOME,"r")) == NULL)
        {
        portsout(CRLF) ;
        portsout("Welcome file not present, welcome anyhow!") ;
        portsout(CRLF) ;
        }
    else
        {
        porttype(inbuf);    /* type a file to port */
        close(inbuf);
        }
    portsout("On at "); /* give date and time of signon */
    portsout(ttime);
    portsout("  ");
    portsout(week);
    portsout("  ");
    portsout(date);
/*
*           get name
*/
    while (1)
        {
        portsout("\r\n\n");
        portsout("Enter your first name   ==> ");
        portsin(w_fname,20);
        fix_name(w_fname) ;
        portsout(CRLF);
        portsout("Enter your last name    ==> ");
        portsin(w_lname,20);
        fix_name(w_lname) ;
        portsout("\r\r\n");
        portsout("Hello ");
        portsout(w_fname);
        portsout(" ");
        portsout(w_lname);
        portsout(CRLF);
        portsout("Did I get your name right?  (Y/N) ==> ");
        byte0 = portin();   /* get answer */
        portout(byte0);     /* send it back */
        if ((byte0 == 'Y') || (byte0 == 'y'))
            {
            portsout(CRLF) ;
            wcaller() ;
            if (checkuser())
                {
                break ; /* got a valid user */
                }
            else
                {
                return ; /* do not have a valid user */
                }
            }
        }
/*
*           type bulletins file
*/
    if ((inbuf=fopen(BULLETINS,"r")) == NULL)
        {
        portsout("\r\nNo Bulletins today\r\n");
        }
    else
        {
        porttype(inbuf);    /* type a file to port */
        close(inbuf);
        }

    hdrread();      /* read msgs header file into memory */
/*
*           end of signon
*/

    }
/*  end of function     */
        
fix_name(adr) char *adr ;
    {
    char *adrs ;
    adrs = adr ;
    while(*adrs)
        {
        *adrs = tolower(*adrs) ;
        adrs++ ;
        }
    *adr = toupper(*adr) ;
    }

wcaller()   /* added this caller to the caller file */
    {
    int code ;
    char    l_date[80] ;

    strcpy(l_date,  mm) ;
    strcat(l_date, "/") ;
    strcat(l_date,  dd) ;
    strcat(l_date, "/") ;
    strcat(l_date,  yy) ;

    if ((inbuf=fopen(CALLERS,"a")) == NULL) /* create or open for append */
        {
        portsout(CRLF) ;
        portsout("Can't open/create callers file!");
        portsout(CRLF) ;
        return ;
        }
    code = fprintf(inbuf,"%s %s %s %s",l_date,ttime,w_fname,w_lname) ;
    if (code < 0)
        {
        portsout(CRLF) ;
        portsout("Caller file has problem writing") ;
        portsout(CRLF) ;
        }
    else
        {
        fputs("\n",inbuf) ; /* put lf on end of each record */
        }
    fclose(inbuf) ;
    /* now also save caller as last-caller */
    if ((inbuf=fopen(LASTCALL,"w")) == NULL) /* create or open for write */
        {
        portsout(CRLF) ;
        portsout("Can't open/create last-caller file!");
        portsout(CRLF) ;
        return ;
        }
    code = fprintf(inbuf,"%s %s %s %s",l_date,ttime,w_fname,w_lname) ;
    if (code < 0)
        {
        portsout(CRLF) ;
        portsout("Last-caller file has problem writing") ;
        portsout(CRLF) ;
        }
    else
        {
        fputs("\n",inbuf) ; /* put lf on end of record */
        }
    fclose(inbuf) ;
    }
/*  end of function     */

checkuser()         /* returns 1 when find a match */
    {
    char    name[50] ;

    if ((inbuf=fopen(USERS,"r")) == NULL)
        {
        portsout(CRLF) ;
        portsout("User file not present, will log you on as");
        portsout(" a GUEST!");
        portsout(CRLF) ;
        strcpy(u_fname,"GUEST");
        strcpy(u_lname," ");
        strcpy(u_password,"MPK0");
        return(TRUE);
        }

    portsout(CRLF) ;
    portsout("Checking user file...");
    portsout(CRLF) ;

    while (readuser(inbuf) != 0)    /* look until eof on users file */
        {
        if ((strcmp(u_fname,w_fname) == 0) &&
            (strcmp(u_lname,w_lname) == 0))
            {
            if (checkpass() == OK)
                {
                fclose(inbuf) ;
                return(TRUE)  ; /* passwords match */
                }
            else
                {
                fclose(inbuf) ;
                return(FALSE) ; /* passwords dont match */
                }
            }
        }
    fclose(inbuf) ;
    newuser() ;     /* not on file, so add 'em */
/*
*           type new-user file
*/
    if ((inbuf=fopen(NEWUSER,"r")) != NULL)
        {
        porttype(inbuf) ;   /* type a file to port */
        close(inbuf) ;
        }
    return(TRUE) ;
    }
/*  end of function     */

checkpass()         /* returns TRUE (1) when equal passwords */
    {
    char    *passptr;
    int j,
        i;

    passptr = w_password;   /* give passptr the addr of w_password */

    for (i = 0; i < 3; i++)     /* give 'em 3 tries to get it right */
        {
        portsout(CRLF) ;
        portsout("Enter your password  ==> ") ;
        for (j=0; j < 4; j++)   /* password is 4 long */
            {
/*          *passptr++ = portin() ;
*/
            w_password[j] = portin() ;
            portout('_');
            }
/*      *passptr++ = '\0';
*/
        w_password[j] = '\0';

        if ((strcmp(w_password,u_password)) == 0)
            {
            return(OK); /* passwords match */
            }
        portsout(" Incorrect!\007") ;
        }
    if (i >= 3)
        {
        portsout(CRLF) ;
        portsout("Sorry, but three tries is all you get!") ;
        portsout(CRLF) ;
        portsout("  Goodby....") ;
        cmd_g()        ;            /* hang up the phone   */
        portsout(CRLF) ;
        return(ERROR);          /* bad try on password */
        }
    }
/*  end of function     */

newuser()
    {
    char    pword[5];
    int j;

    if ((inbuf=fopen(USERS,"a")) == NULL)
        {
        portsout(CRLF) ;
        portsout("Can't open/create the user file for writing!") ;
        portsout(CRLF) ;
        return ;
        }

    strcpy(u_fname,w_fname);
    strcpy(u_lname,w_lname);

    strcpy(u_time1,ttime);
    strcpy(u_time2,ttime);

    strcpy(u_date1,mm);
    strcat(u_date1,"/");
    strcat(u_date1,dd);
    strcat(u_date1,"/");
    strcat(u_date1,yy);

    strcpy(u_date2,mm);
    strcat(u_date2,"/");
    strcat(u_date2,dd);
    strcat(u_date2,"/");
    strcat(u_date2,yy);

    portsout(CRLF) ;
    portsout("Welcome, as a new user I need a few pieces") ;
    portsout(" of information.") ;
    portsout(CRLF) ;
    portsout(CRLF) ;

    portsout("Please enter the City, State you are from ===> ");
    portsin(u_city,30);
    portsout(CRLF);

    while (1)
        {
        portsout(CRLF) ;
        portsout("Ok, now I need a 4 character password ===> ");
        for (j=0; j < 4; j++)   /* password is 4 long */
            {
            u_password[j] = portin();
            portout('_');
            }
        u_password[j] = '\0';
        portsout(CRLF) ;

        portsout("Just to make sure, enter it again ===> ");
        for (j=0; j < 4; j++)   /* password is 4 long */
            {
            w_password[j] = portin();
            portout('_');
            }
        w_password[j] = '\0';
        portsout(CRLF) ;

        if (strcmp(u_password,w_password) == 0)
            {
            break ; /* get it right twice, then get out */
            }
        portsout(CRLF) ;
        portsout("hmmmm, one of us forgot it already") ;
        portsout(CRLF) ;
        portsout("  let's try it again!!") ;
        portsout(CRLF) ;
        portsout(CRLF) ;
        }

    wrtuser(inbuf) ;    /* write a user record */
    fflush(inbuf)  ;    /* ok leor, just for you */
    fclose(inbuf)  ;    /* close the file after new user added */
    }
/*  end of function     */

readuser(buf)   /* read a record from the user file */
        /*  returns 0 on eof, 1 on good read */
FILE    *buf;
    {
    int code;

    code = fscanf(buf,"%[^~]~%[^~]~%[^~]~%[^~]~%[^~]~%[^~]~%[^~]~%[^~]~\n",
            u_fname,
            u_lname,
            u_password,
            u_time1,
            u_date1,
            u_time2,
            u_date2,
            u_city);

    if (code < 8)
        {
        return(0);  /* all done, hit eof */
        }
    else
        {
        return(1);  /* good read */
        }
    }
/*  end of function     */

wrtuser(buf)
FILE    *buf;
    {
    int code;
    code = fprintf(buf,"%s~%s~%s~%s~%s~%s~%s~%s~",
            u_fname,
            u_lname,
            u_password,
            u_time1,
            u_date1,
            u_time2,
            u_date2,
            u_city);
    if (code == -1)
        {
        portsout("User file has problem writing\r\n");
        }
    else
        {
        fputs("\n",buf);    /* put lf on end of each record */
        }
    }
/*  end of function     */

/*  end of program      */

