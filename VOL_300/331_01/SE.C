/*
HEADER:         CUG000.00;
TITLE:          SE (nee E) stack editor -- main();
DATE:           05/19/87;

DESCRIPTION:   "A full screen editor descended from Gilbert's 'e'(1981)
                through Haefner's qed and Tearle's ged.  Similar to
                Wordstar, with text stack, undo/redo, and other enhancements.
                Utilizes large RAM if available, virtual memory text storage
                in small systems.";

KEYWORDS:       Word processing, e, editor, qed, ged, full screen editor,
                text editor, program editor;

SYSTEM:         MS-DOS;
FILENAME:       SE.C;
SEE-ALSO:       CUG133, CUG133, CUG199;
AUTHORS:        G. Nigel Gilbert, James W. Haefner, Mel Tearle, G. Osborn;
COMPILERS:      Microsoft 4.00;
*/

/*   e/qed/ged screen editor

    (C) G. Nigel Gilbert, MICROLOGY, 1981
           licensed for private non-profitmaking use 1983
           August-December 1981

    Modified:   Aug-Dec   1984:  BDS-C 'e'(vers 4.6a) to 'qe' (J.W. Haefner)
                March     1985:  BDS-C 'qe' to DeSmet-C 'qed' (J.W. Haefner)
                May       1986:  converted to ged - Mel Tearle

    FUNCTIONS:  main, initialise, edit, finish, dispose, xit,
                askforfile, seldisk

    PURPOSE:    initialise; process commands;
*/

#include <stdio.h>
#include <ctype.h>
#include <setjmp.h>
#include "ged.h"

int cbreak;
int linel;
int pagingdisk;

main(argc,argv)
int argc;
char **argv;
{
    int argn;
    int i;
    char *dig;

/* --------------------- Default option settings ---------------------*/

    initjmp =    1;     /* jmp to line 1*/
    autoin  =  YES;     /* auto indent  [YES/NO]*/
    backup  =  YES;     /* make ".BAK" file [YES/NO]*/
    ctrl = NO;          /* Control characters stripped from input file */
    trail   =   NO;     /* leave trailing blanks [YES/NO]*/
    charmask =  0xFF;   /* allow input ASCII 0x80 to 0xFF */
    tabwidth    =   8;  /* tab stops every n cols [number]*/
    fullmem =  (long) RAMSLOTS*PAGESIZE/1024;
    displaypos  = YES;  /* display line:column at top of screen*/
    blockscroll = YES;  /* horizontal scroll whole page, not just current line*/
    eofchar = YES;      /* write ^Z end of file */
    pagingdisk  =   0;  /* create buffer file on this disk -
    set to either 0 (for currently logged-in disk)
    or to desired disk letter  (eg 'B') */

    defext[0] = '\0';   /* default extension (.c and .doc temporarilly hard coded*/

/* ------------------ End of default option settings -------------------- */

    inbufp = 0;
    rax = 0x30 << 8;
/* syscall is not a library function */
    syscall();

    if ( ( rax & 0x00ff ) < 2 )  {
        error1( "Must use MSDOS vers > 2.0" );
        exit(0);
    }

    filename[0] = name[0] = '\0';

    argn = 0;
    while ( ++argn < argc )
        if ( *argv[argn] == '-' )  {
            dig = argv[argn]+1;
            switch( toupper(*dig) )  {
            case 'A' :
                autoin=!autoin;
                break;
            case 'B' :
                backup=!backup;
                break;
            case 'C' :
                ctrl = !ctrl;
                break;
            case 'D' :
                pagingdisk = toupper(*(dig+1));
                if ( pagingdisk < 'A' || pagingdisk > 'Z' )
                    goto argerr;
                break;
            case 'H' :
                blockscroll=!blockscroll;
                break;

/* limit amount of RAM that will be allocated. number is in kb */
            case 'M' :
                i = 0;
                while (*++dig)
                    i = i*10 + *dig - '0';
                i = max(i, PAGESIZE * 3/1024);
                fullmem = min(fullmem, i);
                break;

            case 'P':
                charmask = 0x7F;   /* import Wordstar document mode */
                break;
            case 'T' :
                tabwidth=0;
                while (*++dig) tabwidth=tabwidth*10+*dig-'0';
                if (tabwidth < 1)
                    tabwidth = 1;
                break;
            case 'S' :
                trail=!trail;
                break;
            case 'Z' :
                eofchar = !eofchar;
                break;
            default  :
                if ( isdigit(*dig) )  {
                    initjmp = atoi((argv[argn]+1));
                    break;
                }
argerr:
                putstr("Illegal option: ");
                putstr(argv[argn]);
                exit(0);
            }
        }
        else {
            strcpy( filename[0] ? name : filename, argv[argn] );
        }
    ans[0] = patt[0] = opts[0] = '\0';

/* --------- start here ----------- */

/* remember state of ^C abort setting, then turn it off so that ^C can
 * be used for cursor control.
 */
    rax = 0x3300;
    syscall();
    cbreak = rdx & 0x00ff;
    rax = 0x3301;
    rdx = 0;
    syscall();

    keytranslate();
    do {
        initialise();
        edit();
    }
    while (YES);
}

/* ---------- end main ------------ */


initialise()
{
    int i, warn;

    lastl = cursorx = charn = offset = lastoff = 0;
    histptr = histcnt = ncommand = 0;

    cursory  = topline  = findir = jmpto = 1;
    blocking = isdim = repeat = NO;
    replace  = warn  = NO;
    blankedmess = YES;
    goteof  = YES;

    fbuf = &fbuf1;
    textbuf = &tbuf1;

    rax = 0x19 << 8;
    syscall();       /* get current disc */
    curdsk  = rax & 0xff;
    pagingfile[0] = (pagingdisk) ? pagingdisk : ( curdsk+'A' ) ;
    strcpy( pagingfile+1, ":e$$$.@@@" );

    initvm();

    text[0] = '\0';
    cline   = 1;
    inject(0,text);  /* create null line 1 */

    terminit();
    setstatusname();

    if ( filename[0] )  {
        curson(NO);
        cleareop(0);
        gotoxy(8,9);
        putstr("qed screen editor  version ");
        putstr(VERSION);
        putstr("  MICROLOGY 1983 and JWH 1985");
        gotoxy(17,10);
        putstr("ged 1.05 for Compaq, etc. - Mel Tearle 1986 ");
        gotoxy(17,11);
        putstr("Stack Editor version 1.00 G. Osborn 6-12-90");
        gotoxy(18,13);
        putstr("C Users' Group     Public Domain Software");
        gotoxy(0,21);
        putstr("F1 = help");

/* open a file, if it fails then try again to
 * open it using filename plus default extension
 * file handle is in textbuf structure.
 */
        while ( opentext(filename) == FAIL )  {
            askforfile();
            if ( !filename[0] )  goto  newfile;
        }
        lastl = 0;
        goteof = NO;

        if ( name[0] )  {
            strcpy(filename,name);
            name[0] = '\0';
        }
    }       /* end - if filename */

newfile:
    format( filename );

    for ( i = lastl; !(goteof); i += 1 )
        readtext(i);
    fclose(textbuf);
    setstatusname();
    gettext(1, charn);
    if ( initjmp> 2)  {              /* (2-1) is a bad jump from line 1 */
        jumpline(initjmp-cline);    /* not possible to do init jump to 2*/
        initjmp = 0;
    }
    linel = lastl;
}       /* end - initialise()  */


/* command processor */
edit()
{
    unsigned char inkey();
    unsigned char getkey();
    unsigned char  c;
    unsigned char  oldcrx, inc;
    int   i, j, k, to;
    jmp_buf env;   /* define data storage for setjmp (Microsoft) */

    setjmp(env);

    putstatusline(linel);  /* show the flie size until editing starts */
    displine = cline;
    blankedmess =  NO;
    calp();  /* initialization call */
    i = displaypos;
    displaypos = NO;
    putpage();
    displaypos = i;
    curson(YES);
    linem1 = 0;
    linem2 = 0;
    linem3 = 0;
    lastc = 0;

/* command processing loop */
nextchar:;
    goodline = cline;     /* restore environment */
    c = getkey();
    ncommand++;
    storehist = YES;

    switch(c)  {
    case UPPAGE :
    case DOWNPAGE :
        if (c == UPPAGE)
            movepage(-1);
        else
            movepage(0);
/* don't let input buffer overflow if auto repeat used */
        if (chkbuf() > 1)
            inbufp = 1;
        break;
    case DOWNKEY :  /* down one line */
        if (cline == lastl) {
            inject(cline,"");
            calp();  /* lastl has increased */
        }
/* a rewrite can be necessary if the cursor is moved into a marker.
 * Specify a preferred y cursor in that case */
        cursory += 1;
        moveline(1);
        break;
    case UPKEY :
        cursory -= 1;   /* preferred y cursor position */
        moveline(-1);
        break;
    case WINDDOWN :  /* window down one line */

        curson(NO);
        if (cursory == topline) {
            moveline(1);
        }
        if (pfirst != lastl) {
            cursory -= 1;
            scrollup(topline);
            i = plast;
            calp();
            if (i != lastl)
                putline(plast, SHEIGHT, NULL);
            resetpcursor();
        }
        curson(YES);
        break;

    case WINDUP : /* window up one line */

        curson(NO);
        if (cursory == SHEIGHT || pfirst == 1) {
            moveline(-1);
        }
        if (pfirst != 1) {
            cursory += 1;
            calp();
            scrolldown(topline);
            putline(pfirst, topline, NULL);
            resetpcursor();
        }
        curson(YES);
        break;

    case DELLNKEY :       /* delete from cursor to right margin */
        puttext();
/* add an undo step if the deleted line has been altered */
        ncommand++;
        text[charn] = '\0';
        if (charn == 0) {
            linedelete();
        }
        else {
            rewrite(charn);
            altered = YES;
        }
        break;

    case BLOCKKEY :
        switch ( to = blockops() )  {
        case YES :
            return;
            break;
        case 'x' :
        case 'q' :
            if ( finish(to) == YES )
                return;
            break;
        case 'r' :
            putmess(" |F|ile to read? ");
            if ( ( scans( name, FILELEN )) == ESCKEY )  break;
            if ( *(name) )  {
                readfile( name );
                putpage();
            }
            break;
        }
        break;
    case QWIKKEY :    /* all 'Q' functions */
        info();
        break;
    case BOLKEY :
        j = offset + (offset>0); /* leftmost visible character */
        if (j == charn)
            sync(j-SWIDTH+1);
        else
            sync(j);
        resetcursor();
        break;

    case EOLKEY :
        i = trim(text);          /* line end */
        k = offset + SWIDTH;     /* rightmost visible character */
        if (i > k+1)
            k--;         /* allow for field marker */

        j = k;
        if (j == charn || j-1 == charn) {
            j += SWIDTH-1;   /* overflow handled by sync() */
            if (i >= k && i < j && i != charn)
                j = i;
            sync(j);
        }
        else {
            if ( i < k && charn < i)
                sync(i);
            else
                sync(k);
        }
        resetcursor();
        break;
    case HOMEKEY :    /* last line of file */
/* jumpline is prefered to moveline here */
        cursory = SHEIGHT - (SHEIGHT - topline)/4;
        jumpline(lastl-cline);
        break;
    case BOFKEY :     /* first line of file */
/* no choice in cursory */
        jumpline(1-cline);
        break;
    case LEFTKEY :
        movechar(-1);
        break;
    case RIGHTKEY :
        movechar(1);
        break;
    case LEFTWKEY :
        moveword(-1);
        break;
    case RIGHTWKEY :
        moveword(1);
        break;
    case DELLEFT :
        deletechar(-1);
        break;
    case DELRIGHT :
        deletechar(0);
        break;
    case DELWDKEY :
        deleteword();
        break;
    case JUMPKEY :
        dojump();
        break;
    case TOPSCRKEY :
        moveline( (topline-cursory) ); /* no current key assignment */
        break;
    case BOTSCRKEY :
        moveline( (SHEIGHT-cursory) );
        break;
    case REPKEY :
        repeat = YES;
        if (replace)
            dofindrep(MAXINT,findir);
        else
            dofindrep(1,findir);
        repeat = NO;
        break;
    case HELPKEY :
        if ( topline > 1 )  {
            topline = 1;
            unmess();
        }
        else  {
            dohelp();
        }
        break;
    case CR :
        crinsert(0);
        break;
    case CRSTILL :        /* line break */
        crinsert(1);
        break;
    case OOPSKEY :
        undo();
        break;
    case POP :
        pop(0);
        break;
    case CPOP :
        pop(1);
        break;
    case TAB :
/* tab is a cursor positioning command in this version */
        i = tabwidth - (cursorx % tabwidth);   /* 1 <= i <= tabwidth */
        charn += i;
        sync(charn);
        resetcursor();
        break;

    case INSKEY :
        charep = 1 - charep;   /* toggle */
        putlineno(cline);   /* show the new header */
        break;

    case ESCKEY :
        if ( topline > 1 )
            unmess();
        blankedmess = YES;
        break;

    case F1KEY :                     /* help menu */
        if ( topline > 1 )
            unmess();
        else
            dohelp();
        break;

    case F6KEY:
/* center window on cursor.  will not move window right if right end of
   all lines already visible */
        cent();
        break;

    case F3KEY :
        if (replace)
            dofindrep(-1,-1);
        else
            dofindrep(1,-1);   /* backward ^L.  repeat string search or s & r*/
        break;

    case F4KEY :
        if (replace)
            dofindrep(-1,1);
        else
            dofindrep(1,1);    /* forward ^L */
        break;

    case F5KEY :
        break;

    case F2KEY :
        findorrep(0);   /* start string search.  */
        break;

    case F7KEY :   /* options */
        envir();
        break;


    case F8KEY :    /* disc directory */
        cleareol( 0, 0 );
        putmess( "Enter wildcard filespec, <ret> for all) ");
        c = dirmsk();
        if ( c != ESCKEY  &&  c != YES )  {
            gotoxy( 58, SHEIGHT );
            getkey();
        }
        cleareop(0);
        topline = 1;  /* help off if on */
        putpage();
        putstatusline( cline );
        break;

    case F9KEY :   /* exit without saving if no changes */
        puttext();
        finish('q');
        break;

    case F10KEY :       /* save and exit. change x to F10KEY for save & */
        puttext();
        finish('x');       /* stay.  that is ok for floppies but not fixed disc */
        break;

    default :
        if (charep)
            replchar(c);
        else
            insertchar(c);
        break;
    }

/* ensure that all modules maintain the correct vertical window boundary */
    i = pfirst;
    j = plast;
    calp();
    if (i != pfirst || j != plast )
        cerr(1);
/* insure that the status line is current */
    if ( displine != cline || dispcol != cursorx+1 )
        cerr(2);

/* End of command character processing.  Tidy up for next input.
 * Don't put up new status line if a command is waiting because it will
 * immediately be replaced.  ^K and ^Q cause excessive flashing in that case.
 */
    if ( blankedmess && (chkbuf() == 0) )
        putstatusline(cline);
    goto nextchar;
}


/* No to return to current file or don't return,
 * but exit if finished altogther
 * return Yes to edit another file
 */
finish(o)
int o;
{
    int abandon;
    char c, tempname[FILELEN], namebak[FILELEN], *strcpy();

    if (altered)
        cerr(3);    /* disc file not updated from RAM */
    c = '\0';

    if ( o == F10KEY && !filename[0] )  return  NO;

    if ( o != 'x' && o != F10KEY )  {
        if ( o != 'q' || lastc) {
            putmess( "|W|rite edited text to file, |A|bandon all edits, or |<esc>|? " );
            while ( (c = getlow() ) != 'w' && c != 'a' &&  c != ESCKEY);
            if ( c != ESCKEY)
                putch(c);
            else
                return  NO;
        }
        abandon = c == 'a';
    }

    if ( (c == 'w') || (o == 'x') || (o == F10KEY) )  {
        if ( (!filename[0]) && (o != F10KEY) )  {
            putmess("|F|ile to write to? ");
            if ( ( scans( filename, FILELEN )) == ESCKEY )  return NO;
            format(filename);
            if ( filename[0] <= ' '  ||  (!backup && !exists(filename)) )  {
                filename[0] == '\0';
                return  NO;
            }
        }
        if ( backup )  {   /* delete old bak file */
            retag(strcpy(namebak,filename),"BAK");
            if ( checkexists(namebak) )
                if ( funlink(namebak) == FAIL )  return  NO;
        }
/* keep old name in 'filename' */
        strcpy( tempname, filename );
/* new file called 'filename.@@@ ' - or whatever you like  */
        retag( tempname, "@@@" );
        if ( writefile(1,lastl,tempname,filename ) == FAIL )
            return  NO;
/* check original file still exists -
 * may have been deleted or renamed by user
 */
        if ( checkexists( filename ) )  {
            if ( backup )  {
/* orig. file becomes '.bak' */
                if ( frename( filename, namebak ) == FAIL )  goto failed;
            }
            else {
/* delete orig file */
                if ( funlink(filename) == FAIL )  {
failed:             /* if can't delete/rename old file, change
                    * new name to '.@@@'
                    */
                    strcpy( filename, tempname );
                    goto nowrite;
                }
            }
        }
/* new file goes from '@@@' to orig name */
        frename(tempname,filename);
        lastc = 0;     /* file now not changed */
    }
    if ( o == 'x' || o == F10KEY)  {
        if ( pagefd != NOFILE )  {
            close(pagefd);
            funlink(pagingfile);
        }
        xit();
    };

nowrite:
    if ( o == F10KEY )  goto qksave;
    if ( o != 'q' || lastc) {
        putmess("|E|xit to DOS, edit |A|nother file, or |<esc>|? ");
        while ( (c=getlow() ) != 'e' && c != ESCKEY && c != 'a' )
            ;
        if ( c != ESCKEY)
            putch(c);
    }
    else
        c = 'e';
    switch(c) {
    case 'e' :
        if ( lastc )
            if ( !dispose() )  return  NO;
/*      fclose(textbuf);  */
        if ( pagefd != NOFILE ) {
            close(pagefd);
            funlink(pagingfile);
        }
        xit();
    case 'a' :
        if ( lastc )
            if ( !dispose() )  return  NO;
/*      fclose(textbuf);  */
        if ( pagefd != NOFILE )  {
            close(pagefd);
            funlink(pagingfile);
        }
        askforfile();
        return  YES;
qksave:
    case ESCKEY :
        if ( !abandon )  {
            gotoxy( WAITPOS, 0 );
            opentext( filename );
            lastl    = 1;
            goteof   = NO;
            initvm();
            cleareol( 0, 0 );
            gettext( cline, charn);
            putstatusline( cline );
        }
        return  NO;
    }
}


/* check to dispose of modified file */
dispose()
{
    putmess( "|A|bandon modified file?| (y/n) ");
    if ( getlow() == 'y' )
        return  YES;
    else
        return  NO;
}


/* quit editor */
xit()
{
    cleareol( 0, 24 );
    gotoxy( 0, 23 );
    curson(YES);
    rax = 0x3301;
    rdx = cbreak;  /* restore ^C to its original state */
    syscall();
    exit( 0 );
}


/* get another file to edit into 'filename' */
askforfile()
{
    int drive;

    cleareop(1);
dirdone:;
    gotoxy(0,1);  /* line 0 may contain an error message here */
    putstr("|<return>| to create a new file\n" );   /* line 2 */
    putstr("|>|        to view directory|\n");      /* line 3 */
    putstr("|d:|       to change default disk\n");  /* line 4 */
    putstr("|<esc>|    to exit\n");                 /* line 5 */

    cleareol(0, 6);                                 /* line 6 */
    curson(YES);
    putstr("|File to edit:  " );

    if ( scans(filename,FILELEN) == ESCKEY )
        xit();

    cleareol(EMPOS,0);
    if ( filename[0] == '>' )  {
        filename[0] = '\0';
        cleareol(0,6);
        putstr( "Enter wildcard filespec, |<ret>| for all " );
        dirmsk();
        goto  dirdone;
    }

    if ( strlen(filename) == 2 &&  filename[1] == ':' )  {
        if ( ( drive = toupper( filename[0]) - 'A' ) >= 0 && drive < 26 )  {
            seldisk ( drive );
            curdsk = drive;
            goto dirdone;
        }
    }
    else  {
        name[0] = '\0';
        return  YES;
    }
}



seldisk(drive)
int drive;
{
    rax = 0x0E << 8;
    rdx = drive;
    syscall();  /* no error status returned */
    return 0;
}








