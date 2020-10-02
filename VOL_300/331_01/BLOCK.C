/*
HEADER:         CUG199.06;
TITLE:          GED (nee QED) screen editor -- part six;
DATE:           12/15/86;

DESCRIPTION:   "Block operation commands for the GED editor";
KEYWORDS:       block operations;
SYSTEM:         MS-DOS;
FILENAME:       GED6.C;
AUTHORS:        G. Nigel Gilbert, James W. Haefner,  Mel Tearle, G. Osborn;
COMPILERS:      Microsoft 4.0;
*/

/*
     e/qed/ged  screen editor

    (C) G. Nigel Gilbert, MICROLOGY, 1981
           August-December 1981

    Modified:  Aug-Dec   1984:   BDS-C 'e'(vers 4.6a) to 'qe' (J.W. Haefner)
               March     1985:   BDS-C 'qe' to DeSmet-C 'qed' (J.W. Haefner)
               May       1986:   converted to ged - Mel Tearle

    FILE:      ged6.c

    FUNCTIONS: blockpos, blockops, putpart, listfile

    PURPOSE:   performs block commands

*/

#include <stdio.h>
#include "ged.h"


blockpos(oldpos)
int  oldpos;
{
    unsigned char getkey();
    char  c;
    int to;

    do  {
        switch ( (c = getkey()) )  {
        case DOWNKEY :
            moveline(1);
            break;
        case UPKEY :
            moveline(-1);
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
        case BOLKEY :
            sync(0);
            break;
        case EOLKEY :
            sync(strlen(text));
            break;
        case UPPAGE :
            movepage(-1);
            break;
        case DOWNPAGE :
            movepage(0);
            break;
        case HOMEKEY :
            jumpline(lastl-cline);
            break;
        case BOFKEY :
            jumpline(1-cline);
            break;
        case JUMPKEY :
            dojump();
            break;
        case QWIKKEY :
            info();
            break;
        case REPKEY :
            repeat = YES;
            dofindrep(1);
            repeat = NO;
            break;
        case 'p' :
        case 'P' :
            if ( oldpos )  return  PREV;
            break;
        case ESCKEY :
            return  FAIL;
        default :
            ;
        }
    }
    while ( c != CR );
    return  cline;
}


/* 'K'- key block options
 * some mods to print routine - mt.
 */
blockops()
{
    unsigned char getkey();
    int   oldcline, oldcharn, oldto, oldfrom, op;
    int   l, ll, line, *t, shifts, shiftx, cp;
    char  *txt, c, shift[LLIM];

    puttext();
    blankedmess = YES;  /* disable time of day display */
    oldcline = cline;
    oldcharn = charn;
    oldfrom  = vbord1;
    oldto    = vbord2;

    putmess("|Q|uit, e|X|it with save, |R|ead file, |B|lock/output operations ");
    while ( (op = getlow()) != 'q'  &&  op != 'x'  &&  op != 'r' &&
        op != 'b'  &&  op != 'j' &&
        op != ESCKEY );
    if ( op == ESCKEY  )
        return;

    putch(op);
    switch (op)  {
    case 'q':
        return('q');
        break;
    case 'x':
        return('x');
        break;
    case 'r':
        putmess("Wait");
        return('r');
        break;
    case F7KEY:
        return  (NO);
        break;
    default:
/* block commands */
        vbord1 = cline;
        vbord2 = 0;      /* use updated cline for second limit */
        blocking = YES;

        putline(cline, cursory, NULL);  /* reverse field current line */
        putmess( "Move cursor then press <return>");

        if (blockpos(oldto) == FAIL) {
            goto abort;
        }
        if ( vbord2 == PREV )  {
            moveline( oldfrom-cline );
            vbord1 = cline;
            moveline( oldto-cline );
            vbord2 = cline;
        }
        if (vbord2 == 0)
            vbord2 = cline;

        if ( vbord2 < vbord1 )  {
            l = vbord2;
            vbord2 = vbord1;
            vbord1 = l;
        }


        putmess("|W|rite to file, |P|rint, |S|hift, |M|ove, |C|opy, or |D|elete block ? ");
        while ((op=getlow()) != 'w' && op != 'p' && op != 's' && op != 'm'
            && op != 'c' && op != 'd' && op != ESCKEY);

        switch (op)  {
        case ESCKEY:
           goto abort;
           break;
        case 'w':
            putmess( " File to write to? ");
            if ( scans( name, FILELEN ) != ESCKEY )
                if ( exists( name ) )
                    writefile( vbord1, vbord2, name, name );
            break;

        case 'p':
            if ( !prnstat() )
                goto abort;      /* add 12/13/86 */
            cleareol( 0, 0 );
            putstatusline( vbord1 );
            listfile( vbord1, vbord2 );
            putmess( "Any |Function Key| to form feed ");
            c = getkey();
            if ( c >= F1KEY  &&  c <= F10KEY )
                listout(  0x0c );
            break;

        case 's':
            charn=0;
            moveline( vbord1 - cline );
            putmess( "Insert or delete spaces/tabs to shift block, press <return>");
            shifts = 0;
            while ( (c = getkey()) != CR )  {
                switch (c)  {
                case DELRIGHT:
                    if ( text[0] == ' ' || text[0] == '\t' )
                        deletechar(0);
                    break;
                case ' ':
                    insertchar(' ');
                    break;
                case TAB:
                    insertchar('\t');
                    break;
                case ESCKEY:
                    goto abort;
                }
                shift[shifts++] = c;
                sync(0);
                rewrite(0);
            }
            puttext();
            for ( l = vbord1 + 1; l <= vbord2; l++ )  {
                gettext(l, charn);
                for ( shiftx = 0; shiftx < shifts; shiftx++ )  {
                    switch( ( c = shift[shiftx] ) )  {
                    case DELRIGHT:
                        if ( *(txt=&text[0]) == ' ' || *txt == '\t' )
                            while ( (*txt=*(txt+1)) ) txt++;
                        break;
                    case ' ':
                    case TAB:
                        if ( (cp = strlen( text ) ) < (LLIM-1) )  {
                            for ( ; cp >= 0; cp-- )
                                text[cp+1]=text[cp];
                            text[0] = ( c == ' ' ? ' ': '\t' );
                        }
                        break;
                    }
                }
                altered = YES;
                puttext();
            }
            break;

/* block delete */
        case 'd':
            deltp( vbord1, vbord2-vbord1+1 );
            blocking = NO;
            plast = -1;
            moveline(vbord1-cline);
            break;

/* block copy or move */
        case 'm':
        case 'c':
            if (op == 'm')
                putmess("Put cursor under |move| destination and press <return>");
            else
                putmess("Put cursor under |copy| destination and press <return>");
            if ( (line = blockpos(0)) == FAIL )  {
                break;
            }
            putmess("Wait");
            line--;
            for ( l = vbord1; l <= vbord2; l++ )  {
                if ( (line = inject( line, getline(l)) ) == FAIL )
                    break;
                if ( op == 'm' )  {
                    deltp( ( l < line ? l : l + 1 ), 1 );
                    if ( vbord2 < line )  {
                        vbord2--;
                        l--;
                        line--;
                        cline--;
                    }
                }
                else {
                    if (vbord2 >= line)
                        vbord2++;
                    if (l >= line)
                        l++;
                    if (l == cline)
                        l = line;
                }
            }

            vbord1 = cline;
            vbord2   = line;
            blocking=TRUE;
            calp();
            if (vbord2 >= plast)
                cursory += plast - vbord2 -1;  /* limit checked in calp() */

            plast = -1;
            moveline(0);    /* show the new block */
            while (chkbuf() == 0)
                ;
            blocking=FALSE;  /* clear the reverse field on the first keystroke */
/* the keystroke is then used normally */
            break;
        }
abort:
        blocking = NO;
        switch (op)  {
        case 'w':
        case 'p':
        case 's':
            break;
        case 'd':
            break;
        case 'm':
        case 'c':
        break;
        }

        plast = -1;
        moveline(0);
        return NO;
    }   /* outer case */

}       /* func blockops */


/* get status of parallel printer port - is the printer turned on?
 * report if error
 */
prnstat()
{
    rax = ( 0x02 << 8 );
    rdx = 0;                  /* 0 = prn */
    syscall();

    if ( ( ( rax >> 8 ) & 0x80 ) ||
        ( ( rax >> 8 ) & 0x40 ) ||
        ( ( rax >> 8 ) & 0x10 ) )
        return( 1 );
    else  {
        putch( '\07' );
        error( " error on printer " );
        return( 0 );
    }
}


/* printer routine
 */
listfile(from,to)
int  from, to;
{
    unsigned char testkey();
    int  cp, i, k, l;
    char *t;

    k = 0;

    puttext();
    for ( l = from; l <= to; l++, k++ )  {
        curson(NO);
        if ( l%10 == 0  && l > 1 )  putlineno(l);
        if ( k >= PAGELEN - NLINES )  {
            for ( i = k; i < PAGELEN; i++ )  {
                listout('\r');
                listout('\n');
            }
            k = 0;
        }
        for ( cp = 0, t = getline(l); *t; t++ )
            if (*t == '\t') for (i=tabwidth-cp%tabwidth; i>0 ; cp++, i--)
                listout(' ');
        else {
            if (*t > 0x19) listout(*t);      /* send some ctrl char */
            else {                              /* for OKI & CITOH */
                switch (*t)  {
                    case (0x08):
                    case (0x0a):
                    case (0x0c):
                    case (0x0d):
                    case (0x18):
                    listout(*t);
                    break;
                default:
                    {
                        listout('^');
                        listout(*t+64);
                    }
                }
            }
            cp++;
        }
        listout('\r');
        listout('\n');

        if ( testkey() == ESCKEY )  {
            error(" Listing aborted ");
            goto ret1;
        }
    }
ret1:;
    curson(YES);
    calp();
    return;
}
listout(c)
int c;
{
    rdx = c;
    rax = 05 << 8;
    syscall();
    return;
}









