/*
HEADER:         CUG999.02;
TITLE:          GED (nee QED) screen editor -- part 1;
DATE:           05/19/87;

DESCRIPTION:    "Disc directory and option selection for SE.";

SYSTEM:         MS-DOS;
FILENAME:       GED1.C;
AUTHORS:        G. Nigel Gilbert, James W. Haefner, Mel Tearle, G. Osborn;
COMPILERS:      DeSmet C;
*/

/*
     e/qed/ged  screen editor

    (C) G. Nigel Gilbert, MICROLOGY, 1981
           August-December 1981

    Modified:  Aug-Dec   1984:  BDS-C 'e'(vers 4.6a) to 'qed' (J.W.Haefner)
               March     1985:  BDS-C 'qed' to DeSmet-C 'qed' (J.W.Haefner)
               May       1986:  converted to ged - Mel Tearle

    FILE:      qed1.c

    FUNCTIONS: dirmsk, printdirectory, envir, files, putonoff,
               getnumb, getlow

    PURPOSE:   environment; change files.

*/

#include <stdio.h>
#include "ged.h"

#define EXTENT   12
#define RECORDS  15
#define EXTSIZ  128


/* in MSDOS 2.x just pass the string to showdir
 * maintain dirmsk call for other functions
 */
dirmsk()
{
    char *pattp;

    pattp = patt;
    if ( scans( patt, FILELEN ) == ESCKEY )
        return  YES;
    if ( !*pattp )
        pattp = "*.*";
    showdir( pattp );
}


/* show msdos directory from ambiguous mask
 * assumes vers.2.x, allows path names
 */
showdir(msk)
char *msk;
{
    unsigned char getkey();
    char  dtabuf[43], *dirsp, curdir[64], *curdirp;
    char  *bufp, first, driven, drivel, entriesonline, a, c;
    long  fsize, *sizep, used, free;
    int   x, y, i;

/* init display */
    cleareop(0);
/* has a drive been specified?
         */
    for ( i = 0; ( (c = msk[i++] ) && ( c != ':' ) ); );
    if ( c )  {
        drivel = toupper( msk[(i-=2)] );
        {
            if ( ( ( driven = drivel-'A' ) < 0 ) || ( driven > 15 ) )  {
                driven = curdsk;
                drivel = driven+'A';
            }
        }
    }
    else  {
        driven = curdsk;
        drivel = driven+'A';
    }

    curdirp  = curdir;
    *curdirp = 0;
    gotoxy( 0, 6 );
    if ( getpath( curdirp, ( driven+1 ) ) == FAIL )
        return  NO;
    putstr( "Directory of " );
    dispch( drivel );
    putstr( ":\\" );
    putstr( curdirp );

    if ( curdsk != ( driven ) && seldisk( driven ) != FAIL )  {
        putstr( "  (default drive is " );
        putch( curdsk+'A' );
        putstr( ":)" );
    }

/* get disk free space
           */
    rax = ( 0x36 << 8 );
    rds = -1;
    rdx = driven + 1;
    rcx = 0x00;
    syscall();
    if ( rax == 0xffff )  {
        error( " Bad drive " );
        return( rax );
    }

    free = (long) rax * rbx * rcx;
    cleareol( ( x = 2 ), ( y = 8 ) );
    dirsp = "=dir";
    entriesonline = 0;
    used = 0;

    for ( i = 0; i < 43; i++ )
        dtabuf[i] = 0;
/* set disk dma transfer address.  small data model */
    rdx = (unsigned int) dtabuf;
    rax = 0x1A << 8;
    syscall();

    first = TRUE;

    do  {
        rax = ( (first) ? ( 0x4e << 8 ) : ( 0x4f << 8 ) ); /* find first & next */
        rdx = (unsigned int) msk;             /* point to path */
        rds = -1;              /* use current ds */
        rcx = 0x30;            /* show normal, dir and archived files */
        syscall();
        if ( rax == 2 )  {     /* bad path */
            error(" Bad path ");
            return  NO;
        }
        if ( ( rax != 18 ) )  {   /* name is found */
            color1();
            first = FALSE;
            sizep = (long *) &dtabuf[0x1a];
            fsize = *sizep;
            used += fsize;
            bufp  = &dtabuf[0x1e];
            putstr( bufp );
            if ( dtabuf[0x15] == 0x10 )  putstr( dirsp );  /* entry is directory */
/* put code to show if archive bit set, here
                                 */
            if ( ( ++entriesonline < 5 ) )
                gotoxy( ( x += 15 ), y );
            else  {
                cleareol( ( x = 2 ), ++y );
                if ( y == SHEIGHT )  {
                    putstr( "[|<cr>| for MORE...]" );
                    a = getkey();
                    if ( ( a == ESCKEY ) || ( a == F9KEY ) )  return  ESCKEY;
                    color1();
                    cleareop( ( y = 8 ) );
                    gotoxy( ( x = 2 ), y );
                }
                entriesonline = 0;
            }
        }
    }
    while ( ( rax != 18 ) );

    gotoxy( 0, SHEIGHT );
    putstr( "Total Bytes Free = " );
    uspr( 0, free, ATTR0);
    putstr( "   Used = " );
    uspr( 0, used, ATTR0 );
    if ( curdsk != driven )  seldisk( curdsk );
    color1();
    gotoxy( 0, 0 );
    return ( rax );
}


envir()
{
    char   c;
    long jj;
    char buf[81];

    puttext();
    do {
        cleareop(0);
        putstatusline(cline);
        gotoxy(0,2);
        putstr("Program options:\n\n");
        putstr("  enter |F| for files\n\n");

        putstr("|A|uto indent:                  ");
        putonoff(autoin);

        putstr("|B|ack up original              ");
        putonoff(backup);

        putstr("|C|trl codes OK                 ");
        putonoff(ctrl);

        sprintf(buf,"temp. |D|isc drive               %c:\n",pagingfile[0]);
        putstr(buf);

        putstr("|H|orizontal scroll all lines   ");
        putonoff(blockscroll);

        putstr("|P|arity=0. (import wordstar).  ");
        putonoff(charmask == 0x7F);

        sprintf(buf,"|M|em use allowed               %d kb\n",fullmem);
        putstr(buf);

        sprintf(buf,"|T|ab setting                     %d\n",tabwidth);
        putstr(buf);

        putstr("|S|trip trailing blanks,tabs    ");
        putonoff(!trail);

        putstr("^|Z| at end of output file      ");
        putonoff(eofchar);

        jj = (long) (slotsinmem-2)*(PAGESIZE/(sizeof(*tp)));
        if (jj > 16383L)
            jj = 16383L;   /* really a little more */
        sprintf(buf,"\nMemory allows %ld lines of text\n",jj);
        putstr(buf);

        putstr("Use -A -M99 etc. to set options at load time\n");
agn:
        gotoxy(0,19);
        putstr("Enter capital letter to change, |<esc>| to return: ");

        switch( (c = getlow()) )  {
        case 'a':
            autoin = !autoin;
            break;
        case 'b':
            backup = !backup;
            break;
        case 'c':
            ctrl = !ctrl;
            break;
        case 'h':
            blockscroll = !blockscroll;
            break;
        case 't':
            putstr( "\n\nEnter new |tab width|: " );
            if ( !( tabwidth = getnumb()) )
                tabwidth = 1;
            break;
        case 's':
            trail = !trail;
            break;
        case 'f':
            if ( files() )
                goto escape;
            break;
        case 'z':
            eofchar = !eofchar;
            break;
        case 'p':
        case 'm':
        case 'd':
            putstr("Must be specified at load time.");
            goto agn;
            break;
        case ESCKEY:
            goto escape;
        }
    }
    while (YES);
escape:
    cleareop(0);
    topline = 1;  /* help off if on */
    calp();
    putpage();
    putstatusline( cline );
}


getpath(curdirp,drvnum)
char *curdirp;
int drvnum;
{
    rax = ( 0x47 << 8 );       /* get directory */
    rsi = (unsigned int) curdirp;
    rdx = drvnum;
    syscall();
    if ( ( carryf ) && ( rax == 0x15 ) )  {
        error( "Bad drive" );
        return  FAIL;
    }
    return  YES;
}


files()
{
    char oldname[15], newname[15], dc, c;
    char curdir[64], *curdirp, *tp1;
    int  dir;

    do  {
        cleareop(0);
        putstatusline( cline );
newcomm:
        dir = curdsk;
        gotoxy( 0, 1 );
        putstr( "Enter |Q| to return to Edit context     |D| to delete a file\n" );
        putstr( "      |R| to rename a file  |P| to change/show path  |>| to view directory  \n" );
        cleareol( 0, 3 );
        putstr( "      |C| to change the name of the edited file " );
        putstr( filename );
        cleareol( 0, 4 );
        cleareol( 0, 5 );
        cleareol( 6, 4 );
        c = getlow();
        putstatusline( cline );
        switch (c)  {
        case ESCKEY:
            return  YES;
        case 'q':
            return  NO;
        case '>':
            cleareol( 6, 4 );
            putstr( " |Enter [drv:]mask| (RET for all) " );
            dirmsk();
            goto  newcomm;
        case 'p':
            tp1 = curdirp = curdir;
            *curdirp = 0;
            cleareol( 6, 4 );
            putstr( "Drive: " );
            if ( ( c = getscankey() ) == CR || c == ESCKEY )  dir++;
            else dir = ( toupper(c)-'@' );        /*A=1,B=2...*/

            if ( getpath( curdirp, (dir) ) == FAIL )  goto newcomm;
            cleareol( 6, 4 );
            putstr( "Current directory: " );
            dispch( ( dir+'@' ) );
            putstr( ":\\" );
            putstr( curdirp );
            putstr( "  Enter new directory: " );
            scans( curdir, FILELEN );
            if ( !( *curdirp ) )  goto  newcomm;
/* drive specified? */
            while( ( dc = *(tp1++)) && dc != ':' );
            if ( dc )  {
                if ( ( dir = toupper( *(tp1-2))-'A') < 0  ||  dir > 16 )  {
/*if (seldisk(dir)) continue;
                                                                 curdsk=dir; */
                    *curdirp = curdsk+'A';
                }
            }
            rax = (0x3B << 8);
            rds = -1;
            rdx = (unsigned int) curdirp;
            syscall();
            cleareol( 6, 4 );
            if ( carryf )
                error( "Bad path" );
            goto  newcomm;
        default:
            if ( c == 'r' || c == 'd' || c == 'c' )  {
                cleareol( 6, 4 );
                putstr( "Name of file: " );
                scans( oldname, FILELEN );
                if ( !oldname[0] )  goto  newcomm;
                format( oldname );
            }
            switch (c)  {
            case 'd':
                if ( funlink( oldname ) == FAIL )  {
                    error( "Can't delete" );
                    goto  newcomm;
                }
                break;
            case 'r' :
                cleareol( 6, 5 );
                putstr( "New name:     " );
                scans( newname, 15 );
                format( newname );
                if ( !newname[0] || frename( oldname, newname ) == FAIL )  {
                    error( "Can't rename" );
                    goto newcomm;
                }
                break;
            case 'c'  :
                strcpy( filename, oldname );
                setstatusname();
                putstatusline( cline );
            default:
                goto  newcomm;
            }
        }

    }
    while (YES);
}


putonoff(flag)
int  flag;
{
    putstr( (flag) ? " ON\n" : "OFF\n" );
}


getnumb()
{
    int i, n;
    char c, numb[5];

    scans( numb, 5 );
    n = 0;
    for ( i = 0; ( c = numb[i] ); i++ )
        if ( c >= '0' && c <= '9' )  n = n*10 + c - '0';
    return n;
}
