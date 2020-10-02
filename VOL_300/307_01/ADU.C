

/*===========================================================

            ADAPTABLE DISK UTILITY.


Copyright (c) 1988.  This software is for non commercial
use only.

ADU is and disk utility programme designed to work with both
IBM PC standard and non IBM PC disk formats.  It has been
written to be user configurable such that the disk parameters
can be adapted to almost any conceivable disk format.  The
initial Alien disk parameters are derived be scanning the
disk and building up a disk_base table, which then may be
modified by the user.

To Compile and Link you under Turbo C V2.0 or V1.5 create a project
file ADU.PRJ containing;

        adu.c
        disk_io.c
        video_io.c


User Notes: 0. Function key F1 gives help, F3 repeats the command
               string without a CR, and F4 repeats the command.

            1. Numbers may be separated by either space or commas.

            2. When copying blocks you may specify a contiguous
               sequence using the shorthand notation 2/22 say,
               which will copy blocks 2 to 22.

            3. If you don't want to work in 'blocks' and would prefer
               sectors, then set the sectors/block variable using
               vb.  e.g vb = 1 and the command >>c a:fn 1/100 would
               copy sectors 1 to 100 to file fn on drive a:. Note
               however the Block Start refers to the track offset
               from track 0, thus if Block Start equals 2 then block
               1 will begin  at sector 1, track 2, side 0.

            4. If ADU returns a BIOS TIMEOUT during a diskcopying
               command, it may be either due to an incorrect sector
               or format skew or bad gap values. Adjusting these
               should cure the problem.

            5. Commands may consist of either a single or double
               letter sequence  Press F1 to get the help menu
               which will indicate, in capitals the number of
               significant letters.

            6. Commands may be separated by a semicolon,
               for example >>+s;r;e will increment the sector, read
               it in to memory and then place the user into the editor.
               For example, searching for data or strings on the disk
               may be facilitated by issuing the command +s;r;d. While
               the sector's data is being dumped, if the required data
               does not appear to be present; press F4.  Pressing F4
               will interrupt the 'dump' and also, because F4 has the
               effect of repeating the command then the sector number
               will be incremented, read, and then dumped to the screen
               - etc.


            7. Colour codes can be found in either the XT technical
               reference or various other publications.  I have chosen
               the colours I find suitable. Vary the codes to either
               remove the colour or adjust to suit your requirements.

            8. The inbuilt editor is not extensive and uses only some
               of the cusor control keys, however it is useful.  After
               completing an editing session, press the ESC key to exit
               the editor and then issue the >>w command to write out
               the sector.  The editor will highlight the bytes you
               have changed!

=============================================================*/

#include <dos.h>
#include <stdio.h>
#include <conio.h>
#include <ctype.h>      /* for toupper */
#include <alloc.h>  /*malloc*/

#define TITLE       putfa(2,1," Adaptable Disk Utility. Version 1.6",NULL,0x07);
#define AUTHOR      putfa(2,2," Copyright (c) 1988.  AME Systems",NULL,0x07);

#define BLANK       32
#define FALSE       0
#define TRUE        1
/* #define NULL     0 */    /* Must declare this if putfa is not decalared */
                            /* as a function prototype .  Anyone know why? */
#define YES         1
#define NO          0
#define UP          6       /* used in window scroll commands */
#define DOWN        7       /* used in window scroll commands */
#define ERROR       -1
#define LINLEN      75      /* max characters allowable in command */
#define BUFFSIZ     8196        /* the largest disk buffer likely  */
#define MEMLIMIT    65536       /* Allow 64k limit for malloc */
#define MAX_DRIVES  6
#define COLSIZ      79      /* display row and column sizes */
#define ROWSIZ      24
#define CURSOR_OFF  (gotoxy(0,25))  /* macro to turn cursor off */
#define YELLOW_RED  0x1c        /* screen attribute for yellow on red */

/* Disk utility commands */

#define READ        1
#define LOG         2
#define INCSECT     3
#define INCTRACK    4
#define DECSECT     5
#define DECTRACK    6
#define COMMA       7
#define END_LIN     8
#define HELP        9
#define DUMP        10
#define QUIT        11
#define COPY        12
#define WRITE       13
#define EDIT        14
#define SHEAD       15
#define SBLOCK      16
#define STRACK      17
#define SSECTOR     18
#define VOFFSET     19
#define VHEADS      20
#define VBLOCKS     21
#define VTRACKS     22
#define VEOT        23
#define VSIZE       24
#define DISKCOPY    25
#define SECTXLAT    26
#define FMTSKEW     27
#define VDRIVES     28
#define DISKBASE    29

FILE *fd;                       /* General file descriptor */
union REGS inregs,outregs;      /* DOS and BIOS registers */

int     drive,track,sector,head,dflag;  /* global disk parameters  */
int     spb,offset,sect_size,disk_blocks,last_drive;
int     evalerr;                /* getnum error flag     */
int     first_sector;           /* first sector on disk */
char    *linptr, linbuf[LINLEN]; /* input command line buffer etc */
char    sect_buf[BUFFSIZ];      /* sectors worth of data buffer  */
char    out_buf[BUFFSIZ];       /* scratch buffer for any output */
int     sect_xlat[40];          /* soft sector translate table   */
int     fmt_skew[40];           /* hard/format sector skew table */
char    *bufptr;                /* general buffer pointer    */
char    *trackbuf;              /* general pointer for diskcopy etc */

char    sbuf[20],*sptr;         /* required for itoa calls   */
int     block,saddr,laddr,snum;
int     lnum,eol,numflag;
int     col,lin;                /* column and line variables */

/*
 Define the disk_base structure.
*/


typedef struct {                /* Standard IBM PC Disk Base */
    unsigned char srt;          /* structure.  See IBM Tech  */
    unsigned char hld;          /* reference manual for more */
    unsigned char motor_wait;   /* detail.                   */
    unsigned char bytes_sect;
    unsigned char eot;
    unsigned char rw_gap;
    unsigned char dtl;
    unsigned char format_gap;
    unsigned char format_fill;
    unsigned char hs;
    unsigned char motor_start;
    unsigned char sides;
    unsigned char tracks;


} disk_base;


union {                             /* This union gives access to structure */
                                    /* parameters*/
    disk_base      base;
    unsigned char  parm[13];

} dos;                              /* Dos disk base */


union {

    disk_base      base;
    unsigned char  parm[13];

} alien;                            /* alien disk base */

disk_base       far *pc_disk_vector;        /* declare pointers to both the  */
disk_base       far *alien_disk_vector;  /* standard and alien disk bases */



/*--------------------Function Prototypes----------------------------*/

char        *itoa(int, char *, int);
char        far *get_disk_vectors( void );
int         check_sectors(int);
int         check_head(int);
int         disk_format(int,int,int *,int,int,int,int);
int         putfa(int,int,char *,int,int);
disk_base   far *get_disk_vector( void );
void        put_string(char *, int);
void        prompt( void );
void        do_beep( void );
void        getname(char *);
void        dump_line(int, char *);
void        header( void );
void        status(char *,char *, int);
void        disk_data( void );
void        disk_data( void );
void        disk_data_c( void );
void        disk_data_v( void );
void        zero_buffer(char *, int);
void        block_to_tsh( void );
void        next_sector( void );
void        help( void );
void        log_disk( void );
void        init_disk_globals( void );
void        identify(int);
void        set_alien_parms( void );

/*-------------------------------------------------------------------*/


main() {

char        fname[32];
char        c,ch;                   /* general character declarations */
int         nibble;
int         i,j,jm,k,t,item,nsect;      /* general loop/buffer variables */
int         x,y,ym,xoff,yoff,xl;    /* screen variables     */
unsigned    int msize;              /* used to allocate memory buffer sizes */

/*
 Main loop
*/

pc_disk_vector = get_disk_vector(); /* catch the original PC disk vector */
clrscrn();                      /* clear the screen */
prompt();                       /* prompt user to insert disk */
clrscrn();
header();                       /* place the header info on screen */
init_disk_globals();            /* initialise all global variables */
drive = 0;                      /* default to the a: drive  */
last_drive = 2;
first_sector = 0;
set_alien_parms();              /* set up the alien disk base */
zero_buffer(sect_buf,BUFFSIZ);  /* zero the disk buffer     */
xoff = 6; yoff = 5;             /* set the screen edit variables */
numflag = FALSE;                /* initialise getnum() and getitem() stuff */
eol = TRUE;
clr_window(0,3,COLSIZ,3,7);     /* clear status line */
while(TRUE) {
    set_disk_vector(pc_disk_vector);/* force this for safety reasons */
                                /* use alien vector only when required */
    evalerr = FALSE;
    disk_data_c();              /* update display areas */
    disk_data_v();
    if (eol == TRUE) {
       gotoxy(0,23);
       scroll_window(0,yoff-1,COLSIZ,ROWSIZ,UP);   /* main display area */
       printf(">>");
       get_string(linbuf);
       linptr = linbuf;
       eol = FALSE;             /* reset true end of line flag */
       clr_window(0,3,COLSIZ,3,7);  /* clear status line */
    }
    item = getitem();
    switch(item) {

        case DUMP:
            clr_window(0,4,COLSIZ,ROWSIZ,7);    /* clear display area */
            putfa(0,4," Press 'P' to pause the display. Any other key to exit ",NULL,0x4e);
            jm = sect_size/16 - 1;              /* number of paragraphs in buffer */
            gotoxy(0,yoff);
            for (i = 0; i <= jm; i++) {
                gotoxy(0,yoff+i);
                if (kbhit()) {
                    c = getch();
                    if ('P' == toupper(c)) {
                        while(!kbhit());        /* wait for next keystroke */
                        getch();                /* before continuing dump  */
                    }
                    else {
                        if (c == 0) c = getch();    /* If function key hit - clear it */
                        break;
                    }
                }
                if (i > yoff + 14) {            /* bottom of dump page  */
                    scroll_window(0,yoff,COLSIZ,ROWSIZ,UP);
                    gotoxy(0,23);
                }
                dump_line(i*16,sect_buf);
            }
            break;

        case EDIT:
            /* i,j are the buffer variables */
            /* x,y are the screen variables */
            /* xl points to the left nibble */
            /* jm,ym are the maxima values of j,y */
            /* xoff,yoff are the screen reference points */
            /* c is the entered character   */
            /* ch is the buffer character   */

            clr_window(0,4,COLSIZ,ROWSIZ,7);    /* clear display area */
            putfa(0,4," ESC to exit.  F6 (Read sector). F5 (Write sector) ",NULL,0x4e);
            gotoxy(0,yoff);
            ym = 15 + yoff;                     /* allow 16 lines to be displayed */
            jm = sect_size/16 - 1;              /* number of paragraphs in buffer */
            for (i = 0; i <= jm && i < 16; i++) {
                gotoxy(0,yoff+i);
                dump_line(i*16,sect_buf);
            }
            i = 0; j = 0;
            x = xoff; y = yoff;
            xl = x;                             /* used as left char marker */
            gotoxy(x,y);
            while (TRUE) {
                if (x <= xoff) {
                    x = xoff;
                    xl = x;
                    i = 0;
                }
                if (x > 53) {                   /* position of last char */
                    x = xoff; y++;
                    xl = x;
                    i = 0; j++;
                }
                if ( y > ym ) {
                    y = ym;
                    if ( j <= jm ) {
                        scroll_window(0,yoff,COLSIZ,ROWSIZ,UP);
                        gotoxy(0,ym);
                        dump_line(j*16,sect_buf);
                    }
                    else j = jm;
                }
                if ( y < yoff ) {
                    y = yoff;
                    if (j >= 0) {
                        scroll_window(0,yoff,COLSIZ,ym,DOWN);
                        gotoxy(0,yoff);
                        dump_line(j*16,sect_buf);
                    }
                    else j = 0;
                }
                if (x == xl) nibble = 0;
                else        nibble = 1;
                gotoxy(x,y);
                c = getch();            /* get character from keyboard */
                if (c == 0x1b) break;   /* esc */
                if (c == 0) {
                    c = getch();        /* get extended char */
                                        /* if extended perform limited process */
                    switch (c) {        /* now look for cursor keys etc.   */


                        case 63: /** F5 - Write **/
                                set_disk_vector(alien_disk_vector);
                                bufptr = sect_buf;
                                status("Writing sector buffer to disk. ","",NO);
                                dflag = disk_write(drive,track,sector,head,bufptr,1);
                                if (dflag) {
                                    *((sptr = itoa(dflag,out_buf,2)) + 8) = '\0';   /* NULL terminate */
                                    status("Write error to Disk. BIOS code = ",sptr,YES);
                                }
                                clr_window(0,3,COLSIZ,3,7);     /* clear status line */
                                continue;

                        case 64: /** F6 - Read **/
                                set_disk_vector(alien_disk_vector);     /* put the alien disk_base in place */
                                bufptr = sect_buf;
                                status("Reading sector into buffer. ","",NO);
                                dflag = disk_read(drive,track,sector,head,bufptr,1);
                                if (dflag) {
                                    *((sptr = itoa(dflag,out_buf,2)) + 8) = '\0';   /* NULL terminate */
                                    status("Read error on Disk. BIOS code = ",sptr,YES);
                                }
                                clr_window(0,3,COLSIZ,3,7);     /* clear status line */

                                /* fall through to home function */

                        case 71: /** Home **/
                                clr_window(0,5,COLSIZ,ROWSIZ,7);    /* clear display area */
                                gotoxy(0,yoff);
                                for (i = 0; i <= jm && i < 16; i++){
                                    gotoxy(0,yoff+i);
                                    dump_line(i*16,sect_buf);
                                }
                                y = yoff; x = xoff;
                                i = 0; j = 0;
                                xl = x;
                                continue;

                        case 72: /** Up arrow **/
                                y--; j--;
                                continue;

                        case 75: /** Left arrow **/
                                x--;
                                if (x == xl - 1) {
                                    x--; xl = x-1;
                                    i--;
                                }
                                continue;

                        case 77: /** Right arrow **/
                                x++;
                                if (x == xl + 2) {
                                    x++; xl = x;
                                    i++;
                                }
                                continue;

                        case 79:  /** End **/
                                clr_window(0,5,COLSIZ,ROWSIZ,7);    /* clear display area */
                                gotoxy(0,yoff);
                                if (sect_size == 128) continue;
                                j = jm - 15;
                                for (i = j,k = 0; i <= jm; i++,k++){
                                    gotoxy(0,yoff+k);
                                    dump_line(i*16,sect_buf);
                                }
                                y = ym; x = xoff;
                                i = 0; j = jm;
                                xl = x;
                                continue;

                        case 80:  /** Down arrow **/
                                j++;y++;
                                continue;
                    }

                }
                gotoxy(x,y);                /* position for put_char */
                k = i + (j * 16);           /* buffer position */
                ch = sect_buf[k];           /* ch is the buffer character */
                c = toupper(c);             /* convert charcter to upper */
                if (isxdigit(c)) {          /* check for hex range */
                    put_char(c,0x12);
                    gotoxy(51+i+xoff,y);    /* point to ascii dump */
                    if (isdigit(c))
                        c -= '0';
                    else
                        c += -'@' + 9;
                    if (!nibble) {
                        ch &= 0x0f;
                        c = (c << 4) | ch;
                        x++;
                        }
                    else {
                        ch &= 0xf0;         /* mask high nibble of buffer char */
                        c |= ch;
                        x += 2;             /* realign to next char */
                        xl = x;
                        i++;
                    }
                    sect_buf[k] = c;        /* place char in buffer */
                    if ((c > ' ') && (c < 0x7f))
                        put_char(c,0x12);   /* highlight the char */
                    else
                        put_char('.',0x12);
                }
                else status("Invalid key ..","",YES);
                clr_window(0,3,COLSIZ,3,7); /* clear status line */
            }
            break;

        case READ:
                switch(getitem()){

                    case BLANK:
                            t = getnum();
                            if (check_track(t))  continue;
                            track = t;
                            t = getnum();
                            if (check_sector(t)) continue;
                            sector = t;
                            t = getnum();
                            if (check_head(t))  continue;
                            head = t;
                            break;

                    case END_LIN:
                            break;

                    default:    continue;

                }
                set_disk_vector(alien_disk_vector);     /* put the alien disk_base in place */
                bufptr = sect_buf;
                dflag = disk_read(drive,track,sector,head,bufptr,1);
                if (dflag) {
                    *((sptr = itoa(dflag,out_buf,2)) + 8) = '\0';   /* NULL terminate */
                    status("Read error on Alien Disk. BIOS code = ",sptr,YES);
                }
                break;

        case WRITE:
                switch(getitem()){

                    case BLANK:
                            t = getnum();
                            if (check_track(t)) continue;
                            track = t;
                            t = getnum();
                            if (check_sector(t)) continue;
                            sector = t;
                            t = getnum();
                            if (check_head(t))   continue;
                            head = t;
                            break;

                case END_LIN:   break;

                default:        continue;

                }
                set_disk_vector(alien_disk_vector);
                bufptr = sect_buf;
                dflag = disk_write(drive,track,sector,head,bufptr,1);
                if (dflag) {
                    *((sptr = itoa(dflag,out_buf,2)) + 8) = '\0';   /* NULL terminate */
                    status("Write error on Alien Disk. BIOS code = ",sptr,YES);
                }
                break;

        case SECTXLAT:

                switch(getitem()){

                    case BLANK:

                            i = first_sector; t = getnum();
                            while (evalerr != TRUE) {
                                if (check_sector(t)) {
                                    eol = TRUE;         /* terminate processing */
                                    return;
                                }
                                sect_xlat[i++] = t;     /* note - we don't use offset 0 */
                                t = getnum();
                            }

                   case END_LIN:

                            putfa(0,3,"Sector Translate Table: ",NULL,0x4e);
                            for (i = first_sector; i <= alien.base.eot; i++) {
                                getxy(&x,&y);
                                putfa(x,y,"%d ",sect_xlat[i],0x4e);
                            }
                }
                break;

        case FMTSKEW:

                switch(getitem()){

                    case BLANK:

                            i = first_sector; t = getnum();
                            while (evalerr != TRUE) {
                                if (check_sector(t)) {
                                    eol = TRUE;
                                    return;
                                }
                                fmt_skew[i++] = t;
                                t = getnum();
                            }

                    case END_LIN:

                        putfa(0,3,"Format Skew Table: ",NULL,0x4e);
                        for (i = first_sector; i <= alien.base.eot; i++) {
                            getxy(&x,&y);
                            putfa(x,y,"%d ",fmt_skew[i],0x4e);
                        }
                }
                break;

        case DISKBASE:

                switch(getitem()){

                    case BLANK:

                            i = 0;
                            t = getnum();
                            while (evalerr != TRUE && i < 11) {
                                alien.parm[i++] = t;
                                t = getnum();
                            }

                    case END_LIN:

                            putfa(0,3,"PC's Disk Base Table is (in HEX): ",NULL,0x4e);
                            for (i = 0; i <= 10; i++) {
                                getxy(&x,&y);
                                putfa(x,y,"%02X ",alien.parm[i],0x4e);
                            }
                    }
                    break;

        case COPY:

                msize = sect_size*spb;      /* determine memory required first */
                if ((trackbuf = malloc(msize)) == NULL){
                    status(" Insufficient Memory: ","",YES);
                    goto label_0;
                }
                getname(fname);         /* get destination file name and open */
                set_disk_vector(pc_disk_vector);
                if ((fd = fopen(fname,"w")) == NULL) {
                    status(" Can't open ",fname,YES);
                    fclose(fd);
                    break;
                }
                evalerr = FALSE;            /* evalerr signals end of processing */
                block = getnum();
                    while (evalerr != TRUE) {
                        if (check_block(block)) break;
                        sptr = itoa(block,sbuf,10);
                        status(" Reading block: ",sptr,NO);
                        block_to_tsh();         /* get track sector head */
                        bufptr = trackbuf;
                        set_disk_vector(alien_disk_vector);
                        for (i = 0; i < spb; i++) {     /* copy a block at a time */
                            disk_data_v();
                            t = sect_xlat[sector];
                            dflag =  disk_read(drive,track,t,head,bufptr,1);
                            if (dflag) {
                                *((sptr = itoa(dflag,sbuf,2)) + 8) = '\0';
                                status("Read error on Alien Disk. BIOS code = ",sptr,YES);
                                goto label_0;
                            }
                            next_sector();      /* find next track and logical sector */
                            bufptr += sect_size;
                        }
                        bufptr = trackbuf;
                        set_disk_vector(pc_disk_vector);     /* restore dos vector */
                        for (j = 0; j < msize; j++) {
                            fputc(*(bufptr+j),fd);
                            if (ferror(fd)) {
                                clearerr(fd);
                                status ("Write error to DOS file: ",fname,YES);
                                goto label_0;
                            }
                        }
                        block = getnum();
                }

        label_0:
                if (trackbuf != NULL) free(trackbuf);   /* give back the memory */
                set_disk_vector(pc_disk_vector);
                fclose(fd);
                break;

        case DISKCOPY:

                if (drive != 0) {
                    status (" Must copy from Drive A to B. ","",YES);
                    break;
                }
                status(" Disk in B: will be reformatted... Continue (Y/N)?","",YES);
                while (!kbhit());
                if (toupper(c = getch()) != 'Y') break;
                set_disk_vector(alien_disk_vector);
                disk_reset();
                nsect = alien.base.eot + 1 - first_sector;  /* number of sectors */
                msize = alien.base.sides*nsect*sect_size; /* bytes on cylinder */
                if ((trackbuf = malloc(msize)) == NULL){
                    status(" Insufficient Memory: ","",YES);
                    goto label_1;
                }
                for (track = 0; track < alien.base.tracks; track++) {
                    sptr = itoa(track,sbuf,10);
                    status(" Copying Track: ",sptr,NO);
                    bufptr = trackbuf;
                    for ( head = 0; head < alien.base.sides; head++) {
                        disk_data_v();
                        dflag = disk_read(drive,track,first_sector,head,bufptr,nsect);
                        if (dflag) {
                            *((sptr = itoa(dflag,sbuf,2)) + 8) = '\0';
                            status("Read error. BIOS code = ",sbuf,YES);
                            goto label_1;
                        }
                        bufptr += msize/2;  /* next side if necessary */
                    }
                    bufptr = trackbuf;      /* re-initalise pointer */
                    for ( head = 0; head < alien.base.sides; head++) {
                        sptr = itoa(track,sbuf,10);
                        status(" Formating Track: ",sptr,NO);
                        if (first_sector == 0) alien.base.eot++;        /* format requires this */
                        dflag = disk_format(drive+1,track,fmt_skew,head,
                        alien.base.bytes_sect,first_sector,nsect);
                        if (dflag) {
                            *((sptr = itoa(dflag,sbuf,2)) + 8) = '\0';
                            status("Format error. BIOS code = ",sptr,YES);
                            goto label_1;
                        }
                        if (first_sector == 0) alien.base.eot--;
                        disk_data_v();      /* keep track of position */
                        sptr = itoa(track,sbuf,10);
                        status(" Writing Track: ",sptr,NO);
                        dflag = disk_write(drive+1,track,first_sector,head,bufptr,nsect);
                        if (dflag) {
                            *((sptr = itoa(dflag,sbuf,2)) + 8) = '\0';
                            status ("Write error on B:.  BIOS code = ",sptr,YES);
                            goto label_1;
                        }
                        bufptr += msize/2;      /* next side if necessary */
                    }
                }

        label_1:

                if (trackbuf != NULL) free(trackbuf);   /* give back the memory */
                disk_reset();
                disk_recal(drive);
                disk_data_v();
                set_disk_vector(pc_disk_vector);
                break;

        case LOG:

                switch(getitem()){
                    case BLANK:
                            drive = getnum();
                            if (drive >= last_drive) {
                                status(" Select error","",YES);
                                break;
                            }

                    case END_LIN:
                            clrscrn();
                            zero_buffer(sect_buf,BUFFSIZ);
                            header();
                            disk_blocks = 0;
                            alien.base.tracks = 0;
                            alien.base.eot = 0;
                            init_disk_globals();                    /* initialise all global variables */
                            set_alien_parms();
                            alien_disk_vector = &alien.base;
                            set_disk_vector(alien_disk_vector);     /* set t    he alien disk base */
                            log_disk();
                }
                break;

        case VBLOCKS:

                t = getnum();
                if (evalerr || (t > MEMLIMIT/sect_size)) {  /* keep 64k limit */
                    status("Invalid Block Number","",YES);
                    break;
                }
                spb = t;
                disk_blocks = blocks_on_disk();
                break;

        case VOFFSET:

                t = getnum();
                if (evalerr) {
                    status("Invalid Offset Number","",YES);
                    break;
                }
                offset = t;
                disk_blocks = blocks_on_disk();
                break;

        case VHEADS:

                t = getnum();
                if (evalerr || t > 2) {
                    status("Invalid number ","",YES);
                    break;
                }
                alien.base.sides = t;
                disk_blocks = blocks_on_disk();
                break;

        case VDRIVES:

                t = getnum();
                if (evalerr || t > MAX_DRIVES) {
                    status ("Maximum number of drives exceeded ","",YES);
                    break;
                }
                last_drive = t;
                break;

        case VTRACKS:

                t = getnum();
                if (evalerr || t > 80) {
                    status("Invalid number ","",YES);
                    break;
                }
                alien.base.tracks = t;
                disk_blocks = blocks_on_disk();
                break;

        case VEOT:

                t = getnum();
                if (evalerr || t > 40) {
                    status("Invalid number ","",YES);
                    break;
                }
                alien.base.eot = t;
                disk_blocks = blocks_on_disk();
                break;

        case VSIZE:

                t = getnum();
                if (evalerr || t > BUFFSIZ)  {
                    status("Invalid number ","",YES);
                    break;
                }
                sect_size = t;
                t = t >> 7;                     /* shift for 128 base */
                i = 0;
                while ((t = t >> 1) != 0) i++;  /* i represents sector size */
                alien.base.bytes_sect =  i;
                break;

        case SSECTOR:

                t = getnum();                   /* else go to specific head number */
                if (evalerr) {
                    status("Invalid number ","",YES);
                    break;
                }
                if (check_sector(t)) break;
                sector = t;
                break;

        case STRACK:

                t = getnum();
                if (evalerr) {
                    status("Invalid number ","",YES);
                    break;
                }
                if (check_track(t)) break;
                track = t;
                break;

        case SBLOCK:

                t = getnum();
                if (evalerr) {
                    status("Invalid number ","",YES);
                    break;
                }
                if (check_block(t)) break;
                block = t;
                block_to_tsh();
                break;

        case SHEAD:

                t = getnum();
                if (evalerr) {
                    status("Invalid number ","",YES);
                    break;
                }
                if (!check_head(t)) head = t;
                break;

        case INCTRACK:

                if (check_track(++track)) --track;
                break;

        case INCSECT:

                if (check_sector(++sector)) --sector;
                break;

        case DECTRACK:

                if (check_track(--track)) ++track;
                break;

        case DECSECT:

                if (check_sector(--sector)) ++sector;
                break;

        case HELP:

                help();
                break;

        case QUIT:

                set_disk_vector(pc_disk_vector);
                disk_reset();
                clrscrn();                                      /* clear the screen */
                exit();         /* good bye */

        case END_LIN:

                break;

        default:

                status(" Invalid Command..","",YES);
                break;
        }
    }
}

/*
 Function to display user help menu.
*/
void help() {

    clr_window(0,4,COLSIZ,ROWSIZ,7);            /* clear display area */
    gotoxy(0,5);
    puts("ADAPTABLE DISK UTILITY: The following commands are allowed:\n");
    puts("Dump:                   Hex dump the buffer to the screen.");
    puts("Edit:                   Edit the current sector.");
    puts("Read  [T  S  H]:        Read in sectors worth of data.");
    puts("Write [T  S  H]:        Write out sectors worth of data.");
    puts("F1/F3/F4:               Help, Reissue/Repeat Command.");
    puts("+/-Sector:              Increment/decrement sector");
    puts("+/-Track:               Increment/decrement track");
    puts("Sb/St/Ss/Sh N:          Set Block,Track, Sector or Head.");
    puts("Vb/Vt/Vz/Ve/Vh/Vo/Vd N: Vary configuration parameters");
    puts("FS/SX [n1,n2..]:        Vary both sector xlat and fmt skew");
    puts("DB [p1,p2...]           Change PC Disk Base parameters");
    puts("Log [Drive]:            Log disk in drive (0,1).");
    puts("Copy N:fn Blocks:       Copy Blocks to DOS Disk in Drive N:");
    puts("DC                      Disk copy - Make a copy of the Alien Disk onto A:");
    puts("Quit:                   Quit - restoring MS DOS environment.");
    puts("?:                      Re-display main menu.");
    puts(" (Note: hex numbers are entered as 0x31 etc)");

}
/*
**  Log the current drive.
*/

void log_disk() {

    status (" Analysing disk . . .","",NO);
    disk_reset();
    disk_recal(drive);
    identify(drive);
    set_alien_parms();              /* Up date alien disk table */
    disk_reset();
    track = 0; sector = first_sector; head = 0;
    disk_recal(drive);
    disk_data_c();
    disk_data_v();
    clr_window(0,3,COLSIZ,3,7);     /* clear status line */
}

/*
**  Check for valid sector range.
*/
int check_sector(s) int s; {
    if (s > alien.base.eot || s < first_sector){
       status("Invalid sector..","",YES);
       return TRUE;
    }
    return FALSE;
}
/*
**  Check for a valid track.
*/

int check_track(trk) int trk; {

    if (trk >= alien.base.tracks || trk < 0){
       status("Invalid track","",YES);
       return TRUE;
    }
    return FALSE;
}

/*
** Check the requested block number against the system
** number.
*/
int check_block(blk) int blk; {

    if (blk >= disk_blocks){
       status("Invalid block","",YES);
       return TRUE;
    }
    return FALSE;
}
/*
** Check the requested head against the system
** number.
*/
int check_head(head_no) int head_no; {

    if (head_no + 1 > alien.base.sides){
       status ("Invalid head","",YES);
       return TRUE;
    }
    return FALSE;
}

/*
** Calculate number of blocks on disk
*/
int blocks_on_disk() {
unsigned int bm;

    if (alien.base.tracks < offset)
    return (0);
    bm = (alien.base.tracks-offset)*alien.base.eot/spb; /* maximum blocks per disk */
    return ( bm*alien.base.sides );                     /* Blocks per disk */
}

/*
** Place the basic header data on the system screen area.
*/
void header() {

    clr_window(0,0,COLSIZ,2,0x10);          /* blue background */
    putfa(0,0,"Tracks:     ",NULL,0x1e);    /* blue background green forground */
    putfa(12,0,"Last Sector on Track:    ",NULL,0x1e);
    putfa(38,0,"Sector size:    ",NULL,0x1e);
    putfa(58,0,"Sides:  ",NULL,0x1e);
    putfa(0,1,"Block size:    ",NULL,0x1e);
    putfa(16,1,"Block start:   ",NULL,0x1e);
    putfa(35,1,"Blocks on disk:  ",NULL,0x1e);
    putfa(58,1,"Number of Drives: ",NULL,0x1e);
    putfa(0,2,"Current Drive:  ",NULL,0x1e);
    putfa(18,2,"Current Disk Position: ",NULL,0x1e);
    putfa(41,2,"Track   ",NULL,0x1e);
    putfa(51,2,"Sector   ",NULL,0x1e);
    putfa(63,2,"Head   ",NULL,0x1e);


}

/*
** data_c refers to data which is generally constant.
*/

void disk_data_c() {

    putfa(8,0, "%3d",alien.base.tracks,YELLOW_RED);
    putfa(33,0,"%3d",alien.base.eot,YELLOW_RED);
    putfa(51,0,"%4d",sect_size,YELLOW_RED);         /* sector size */
    putfa(65,0,"%1d",alien.base.sides,YELLOW_RED);  /* number of sides */
    putfa(12,1,"%3d",spb,YELLOW_RED);               /* sectors per block */
    putfa(31,1,"%3d",offset,YELLOW_RED);            /* start block offset */
    putfa(52,1,"%4d",disk_blocks,YELLOW_RED);
    putfa(76,1,"%1d",last_drive,YELLOW_RED);        /* last allowed drive */
}

/*
** data_v refers to variable data to reduce screen update time
*/

void disk_data_v() {

    putfa(16,2,"%1d",drive,YELLOW_RED);     /* Current drive  */
    putfa(58,2,"%4d",sector,YELLOW_RED);    /* Current sector */
    putfa(46,2,"%4d",track,YELLOW_RED);     /* Current track  */
    putfa(69,2,"%1d",head,YELLOW_RED);      /* Current head   */
}

/*
** Get next item off the command line.
*/
int getitem() {
char token[32];

    if(*linptr == BLANK){
        while(*linptr == BLANK) linptr++;
        return BLANK;
    }
    if (*linptr == ';') {       /* allow more than one command per line */
        eol = FALSE;
        linptr++;
        while (*linptr == BLANK) linptr++;  /* skip the blanks */
        return END_LIN;
    }
    if ((*linptr == '\n') || (*linptr == '\0')){
        eol = TRUE;
        return END_LIN;
    }
    getname(token);
    if(amatch("VH",token,2)) return VHEADS;
    if(amatch("VB",token,2)) return VBLOCKS;
    if(amatch("VO",token,2)) return VOFFSET;
    if(amatch("VZ",token,2)) return VSIZE;
    if(amatch("VE",token,2)) return VEOT;
    if(amatch("VT",token,2)) return VTRACKS;
    if(amatch("VD",token,2)) return VDRIVES;
    if(amatch("ST",token,2)) return STRACK;
    if(amatch("SS",token,2)) return SSECTOR;
    if(amatch("SB",token,2)) return SBLOCK;
    if(amatch("SH",token,2)) return SHEAD;
    if(amatch("DC",token,2)) return DISKCOPY;
    if(amatch("DB",token,2)) return DISKBASE;
    if(amatch("DUMP",token,1)) return DUMP;
    if(amatch("EDIT",token,1)) return EDIT;
    if(amatch("READ",token,1)) return READ;
    if(amatch("WRITE",token,1)) return WRITE;
    if(amatch("COPY",token,1)) return COPY;
    if(amatch("LOG",token,1))  return LOG;
    if(amatch("+TRACK",token,2)) return INCTRACK;
    if(amatch("+SECTOR",token,2)) return INCSECT;
    if(amatch("-TRACK",token,2)) return DECTRACK;
    if(amatch("-SECTOR",token,2)) return DECSECT;
    if(amatch("SX",token,2)) return SECTXLAT;
    if(amatch("FS",token,2)) return FMTSKEW;
    if(amatch("QUIT",token,1)) return QUIT;
    if(amatch("?",token,1)) return HELP;
    return -1;
}

/*
** Get a name symbol etc form the command line
*/
void getname(buffer) char *buffer; {
char c,count;

    while( *linptr == BLANK) linptr++;
    c = *linptr++;
    count = 1;
    while ( isalpha(c) || isdigit(c) || (c == '?') || (c == ':')
        || (c == '-') || (c == '.') || (c == '+') || (c == '\\')) {

        if(count < 32) *buffer++ = c;
        count++;
        c = *linptr++;
    }
    --linptr;
    *buffer = '\0';     /* null terminate */
}

/*
** Get a numeric value off the line.  Sequences may be as follows:
**   num1 num2 ....
**   num1,num2.....
**   num1/numN.....   this represents are short hand sequence to
**                      specify the range num1 to numN.
*/
int getnum() {
int val;
char c;

    evalerr = FALSE;
    if (numflag == TRUE) {          /* if true look for short hand sequence */
       if (snum <= lnum) {
        val = snum++;
        return val;
       }
       numflag = FALSE;         /* indicate we're back to normal */
    }
    val = 0;
    while (( c = *linptr) == '\t' || c == ' ' || c == ',')
         ++linptr;
    if (!isdigit(c = *linptr)) {
        evalerr = TRUE;
        return (NULL);
    }
    while (isdigit(c = *linptr++)) val = val * 10 + c - '0';
    if ( toupper(*(linptr-1)) == 'X' && val == 0) {     /* i.e.  true if 0x found */
        while ((isxdigit(c = toupper(*linptr++)))) {
            if (isalpha(c)) c += -'@'+ 9;
            else            c -= '0';
            val = val * 16 + c;
        }
    }
    --linptr;
/*
** Check to see if there is a short hand sequence
*/
    if (*linptr == '/') {
        snum = val+1;           /* saddr must start after val */
        lnum = 0;
        linptr++;
        while ( isdigit(c = *linptr++)) lnum = lnum * 10 + c - '0';
        if ( toupper(*(linptr-1)) == 'X' && lnum == 0) {        /* i.e.  true if 0x found */
            while ((isxdigit(c = toupper(*linptr++)))) {
                if ( isalpha(c)) c += -'@' + 9;
                else             c -= '0';
                lnum = lnum * 16 + c;
            }
        }
        --linptr;
        if (snum > lnum ) evalerr = TRUE;
        numflag = TRUE;                 /* flag this for next time getnum() is called */
    }
    return val;
}

/*
** match a null terminated string
**  s     -> source string;
**  t     -> string to match;
**  count -> characters to match
*/
int amatch(s,t,count) char s[],t[]; int  count; {
int i;

    i=0;
    while ( t[i] ) {
        if( s[i] != toupper(t[i])) return 0;
        i++;
    }
    return (count == i) ? count : 0;        /* return number of matches */
}

/*
 This function displays, on the status line, user prompts
 or errors.  If the beep parameter is YES then status will
 output an audible prompt.
*/
void status(s,s2,beep) char *s,*s2; int beep; {
int attr;

    attr = 0x4e;                    /* red background yellow foreground */
    clr_window(0,3,COLSIZ,3,attr);
    gotoxy(4,3);
    put_string(s,attr);
    put_string(s2,attr);
    if (beep) {
        do_beep();
        eol = TRUE;                 /* stop all processing */
    }
}
/*
** Dump a line of buffer at the current cursor position.
*/

void dump_line(saddr,buff) int saddr; char *buff; {
int  i;
char c;
char *bptr;

    bptr = out_buf;
    bptr += sprintf(bptr,"%04x: ",saddr);           /* address offset */
    for(i = saddr; i <= saddr + 15; i++)
        bptr += sprintf(bptr,"%02x ",0xff & buff[i]);   /* hex data */
    bptr += sprintf(bptr,"  |");
    for(i = saddr; i <= saddr + 15; i++){
        c = buff[i];
        if ((c > ' ') && (c < 0x7f))              /* show in ascii format */
            *bptr++ = c;
        else
            *bptr++ = '.';
    }
    *bptr++ =  '|';
    *bptr = '\0';
    put_string(out_buf,0x07);

}

/*
**  Flush to zero the buffer contents.
*/

void zero_buffer(buff,size) char *buff; int size; {
int  i;

    for(i = 0; i < size; i++)
        buff[i] = '\0';
}

/*
** See IBM XT Technical reference manual page 5-98.
*/

void do_beep() {
int  i,timer,port_b,mode,count_l,count_h,mask;

    timer = 0x40;
    mode = 0xb6;
    port_b = 0x61;
    outp(timer+3,mode);
    count_l =  0x33;            /* lsb of count */
    count_h = 0x05;             /* msb of count */
    outp(timer+2,count_l);
    outp(timer+2,count_h);
    mask = inp(port_b);         /* save current settings */
    outp(port_b,mask | 0x03);       /* turn speaker on  */
    for (i = 0; i < 15000; i++);        /* delay loop       */
    outp(port_b,mask);          /* restore original settings */
}

void prompt() {

    TITLE;
    AUTHOR;
    putfa(25,15," RETURN WHEN READY ",NULL,0xf0);
    CURSOR_OFF;
    do_beep();
    while (!kbhit());
    getch();                    /* flush character */
}
/*
 Translate the current block number 'block' to
 track, sector and head values.
*/
void block_to_tsh() {

    track = block*spb/alien.base.eot;
    if (alien.base.sides == 2)
        head = track%2;
    track /= alien.base.sides;
    track += offset;
    sector = (block*spb%alien.base.eot) + 1;
}

/*
  Get next sector.  Also bump track if necessary.
*/

void next_sector()  {

    sector++;
    if (sector > alien.base.eot) {
        if (alien.base.sides == 2){
            if (head == 1) track++;
            head ^= 1;
        }
        else track++;
        sector = 1;
   }
}

/*
 Initialise all the global disk parameters
*/

void init_disk_globals() {
int i;

    disk_blocks = 0;
    sect_size = 0;
    spb = 8; offset = 2;
    for (i = 0; i < 40; i++){
        sect_xlat[i] = i;       /* initialise with non interlaced sectors */
        fmt_skew[i] = i;        /* initialise to zero format skew  */
    }
}

/*
 Get the current disk vector by asking DOS
 to fetch the interrupt vector from 0x1E.  The
 pointer is returned, as is saved by the calling
 module.
*/

disk_base far *get_disk_vector() {
struct SREGS segregs;
disk_base far *longptr;
unsigned long ls,lo;

    inregs.h.ah = 0x35;                     /* DOS function number */
    inregs.h.al = 0x1E;                     /* Disk vector number */
    intdosx (&inregs,&outregs,&segregs);
    ls = (unsigned long)(segregs.es << 16);
    lo = (unsigned long)outregs.x.bx;
    longptr = (disk_base far *)(ls | lo);   /* convert to the correst pointer type */
    return (longptr);
}
/*

 Replace current disk vector with the value
 pointed to by vectptr.  This routine is normally
 used to swap between the Alien and PC disk vectors.

*/

int set_disk_vector(vectptr) disk_base far *vectptr; {
struct SREGS segregs;

    inregs.h.ah = 0x25;
    inregs.h.al = 0x1E;
    inregs.x.dx = FP_OFF(vectptr);
    segregs.ds  = FP_SEG(vectptr);
    intdosx (&inregs,&outregs,&segregs);
    return outregs.x.ax;
}

/*
** Routine to step through all combinations of
** head, sector and track building up the disk
** parameters as it goes.
*/

void identify(drive) int drive; {
int ssize;

/* first find bytes per sector */

    outregs.x.cflag = 1;
    head = 0; track = 0; sector = 0; ssize = 0;

    while (outregs.x.cflag && sector < 40) {        /* some formats start at high sector numbers */
        while (outregs.x.cflag && ssize < 8) {
            alien.base.bytes_sect = ssize++;
            sect_size = (128 << alien.base.bytes_sect); /* a global variable */
            disk_test(drive,head,track,sector); /* returns status in outregs.x.cflag */
        }
        sector++;
        ssize = 0;
    }
    if (outregs.x.cflag) {
        status("Can't figure this disk format out. Vary parameters ","",YES);
        return;
    }
    sect_size = (128 << alien.base.bytes_sect); /* a global variable */
    first_sector = --sector;            /* this is the first sector on disk */


/* up to here we know sector size  */
/* now try to find out how many sectors */

    head = 0; sector = first_sector;        /* begin at sector we found previously  */
    outregs.x.cflag = 0;
    alien.base.eot = 1;             /* assume at least 2 sector per track */
    while (!outregs.x.cflag && sector < 40) {
        disk_test(drive,head ,track,++sector); /* returns status in outregs.x.cflag */
        alien.base.eot = sector - 1;
        disk_data_c();                      /* keep track of the parameters */
    }
    alien.base.eot = --sector;
    disk_data_c();
    disk_data_v();

/* search for second side */

    alien.base.sides = 1;               /* assume single side */
    head = 1; ssize = alien.base.bytes_sect;
    sector = first_sector;
    disk_test(drive,head,track,sector);  /* returns status in outregs.x.cflag */
    if (!outregs.x.cflag ) {
        sector = alien.base.eot;
        disk_test(drive,head,track,sector);  /* returns status in outregs.x.cflag */
        if (!outregs.x.cflag)
            alien.base.sides = 2;
    }
    sector = alien.base.eot + 1;                /* sectors may continue on other side */
    disk_test(drive,head,track,sector);  /* returns status in outregs.x.cflag */
    if (!outregs.x.cflag ) {
        sector = 2*alien.base.eot + 1 + first_sector;
        disk_test(drive,head,track,sector);  /* returns status in outregs.x.cflag */
        if (!outregs.x.cflag)
            alien.base.sides = 2;
    }
    sector = first_sector;
    disk_data_c();                      /* do a partial summary here */


/* Now determine how many tracks are on the disk */

    outregs.x.cflag = 0; track = 0;
    while (!outregs.x.cflag && track < 80) {
        for ( head = 0; head < alien.base.sides; head++)        /* check both sides */
            disk_test(drive,head ,track,sector); /* returns status in outregs.x.cflag */
        track++;
        alien.base.tracks = track-1;
        disk_blocks = blocks_on_disk();     /* keep track of blocks */
        disk_data_c();                      /* display data on screen */
    }
    disk_reset();
}

/*
 Initialise all default variable settings.  Used
 during initial programme activation and during logs.
*/
void set_alien_parms() {
int standard,gap;

/* standard dos disk */

    dos.base.srt =         0xcf;
    dos.base.hld =         2;
    dos.base.motor_wait =  25;
    dos.base.bytes_sect =  2;
    dos.base.eot =         8;
    dos.base.rw_gap =      0x2a;
    dos.base.dtl =         0xff;
    dos.base.format_gap =  0x50;
    dos.base.format_fill = 0xf6;
    dos.base.hs =          25;
    dos.base.motor_start = 4;
    dos.base.sides =       2;
    dos.base.tracks =      40;

/*
 Basic ALIEN disk data.  Most parametes are
 modified on the fly during the disk
 identification process.
*/

    alien.base.srt =         0xcf;
    alien.base.hld =         2;
    alien.base.motor_wait =  0x25;
    alien.base.dtl =         0xff;
    alien.base.rw_gap =      0x05;
    alien.base.format_gap =  0x08;
    alien.base.format_fill = 0xf6;
    alien.base.hs =          25;
    alien.base.motor_start = 4;

/*
   Fill in the format stuff here ( See 8272 data sheet
   for the standard/recommended values).
   The 'read/write' gaps, defined by alien.base.gap
   may have to be ajusted to suit some special formats.
   These gaps are related to the VCO 'idle' period.  Hence
   if too large the VCO will idle straight across the ID/data
   fields.  As a consequence BIOS will time out while looking
   for particular sectors.
*/

    switch (sect_size) {



        case 256:   if ( alien.base.eot <= 0x10) {
                        alien.base.rw_gap = 0x20;
                        alien.base.format_gap = 0x32;
                    }
                    else {
                        alien.base.rw_gap = 0x0a;
                        alien.base.format_gap = 0x10;
                    }
                    break;

        case 512:
                    if (alien.base.eot <= 9) {              /* standard format */
                        alien.base.rw_gap =  0x2a;
                        alien.base.format_gap = 0x50;
                    }
                    else {
                        alien.base.rw_gap = 0x09;
                        alien.base.format_gap = 0x1e;
                    }
                    break;
        case 1024:
                    if (alien.base.eot <= 4){               /* standard values */
                        alien.base.rw_gap = 0x80;
                        alien.base.format_gap = 0xf0;
                    }
                    else {
                        alien.base.rw_gap = 0x30;           /* non standard values */
                        alien.base.format_gap = 0x40;
                    }
                    break;

        case 2048:
        case 4096:
                    if (alien.base.eot <= 2){               /* standard values */
                        alien.base.rw_gap = 0xc8;
                        alien.base.format_gap = 0xff;
                    }
                    else {
                        alien.base.rw_gap = 0xd0;           /* non standard values */
                        alien.base.format_gap = 0xff;
                    }
                    break;

        default:
                alien.base.rw_gap = 0x05;
                alien.base.format_gap = 0x08;
                break;

    }
}
