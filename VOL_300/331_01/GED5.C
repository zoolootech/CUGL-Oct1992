/*
HEADER:         CUG999.05;
TITLE:          GED (nee QED) screen editor -- part 5;
DATE:           10/10/86;

DESCRIPTION:    "File manipulation commands for the GED editor.";
FILENAME:       GED5.C;
AUTHORS:        G. Nigel Gilbert, James W. Haefner, Mel Tearle, G. Osborn;
COMPILERS:      Microsoft 4.0;
*/

/*
     e/qed/ged  screen editor

    (C) G. Nigel Gilbert, MICROLOGY, 1981
           August-December 1981

    Modified:  Aug-Dec   1984:   BDS-C 'e'(vers 4.6a) to 'qe' (J.W. Haefner)
               March     1985:   BDS-C 'qe' to DeSmet-C 'qed' (J.W. Haefner)
               May       1986:   converted to ged - Mel Tearle

    FILE:      ged5.c

    FUNCTIONS: readfile, writefile, exists, checkexists, scans, retag,
               format

    PURPOSE:   read and write files

*/

#include <stdio.h>
#include "ged.h"

#define  GS   0x1d
#define  ESC  0x1b


/* read text into current file from 'filename' with ^KR
 * does not inject into middle of line.  first inserted line
 * is above current line, the same as the line pop operation.
 */
readfile(name)
char *name;
{
    int  c, i, j, k, line, res, cnt;
    char textb[LLIM];
    struct iobuffer *textbuf;
    int fopen1();

    if ( fopen1( name, fbuf) == FAIL )  {
        if ( fopen1( strcat( name, defext ), fbuf) == FAIL )  {
            error( " Can't find file " );
            name[0] = '\0';
            return( FAIL );
        }
    }

    line  = cline-1;
    do  {
        i=0;
        while ( i < LLIM  &&  ( (c = egetc(fbuf)) != DFAIL )  &&  (c != '\n')
            &&  ( c != ENDFILE && c != ENDCHAR) )
            textb[i++] = c;

        if ( textb[i-1] == '\r' )
            i--;
        if ( textb[i-1] == '\n' )
            i--;
        if ( textb[i-1] == '\r' )
            i--;
        if ( textb[i-1] == '\n' )
            i--;
        textb[i] = '\0';
        res = inject( line++, textb );
    }
    while ( c != DFAIL  &&  res  != FAIL  &&  c != ENDFILE );

    fclose( fbuf );

    blocking = YES;
    vbord1 = cline;
    vbord2 = line;
    if (vbord2 >= plast)
        cursory += plast - vbord2 -1;  /* limit checked in calp() */
    plast = -1;
    moveline(0);
    while (chkbuf() == 0)
        ;
    blocking = NO;
    moveline(0);
    return;
}


writefile(from,to,name,nametoprint)
int  from, to;
char *name, *nametoprint;
{
    char buf[90];
    int  l;
    char c, *t, *getline();

    puttext();
    if ( name[0] <= ' ' )  {
        error(" Bad name ");
        return  FAIL;
    }
/* fcreat is not a library function */
    if ( fcreat( name, fbuf ) == FAIL )  {
        strcpy(buf,"Can't create file ");
        strcat(buf,name);
        error(buf);
        return(FAIL);
    }
    strcpy(buf,"|S|aving: ");
    strcat(buf, nametoprint );
    putmess(buf);

    for ( l = from; l <= to; )  {
        t = getline( l++ );
        while ( *t )
            if ( eputc( *t++, fbuf ) == FAIL )
                goto diskfull;
        if ( eputc( '\r', fbuf ) == FAIL )
            goto diskfull;
        if ( eputc( '\n', fbuf ) == FAIL )
            goto diskfull;
    }
    if (eofchar)
        if ( eputc( ENDCHAR, fbuf ) == FAIL )
            goto  diskfull;
    if ( dflush( fbuf ) == FAIL )
        goto  diskfull;
    if ( fclose( fbuf ) == FAIL )  {
        error( " Can't close file " );
        return FAIL;
    }
    return  YES;

diskfull:
    error(" Disk full ");
    return FAIL;
}


exists(name)
char *name;
{
    char c;
    int buf[81];

    c = 'y';
    if ( checkexists( name ) )  {
        strcpy(buf, " OK to replace ");
        strcat(buf, name);
        strcat(buf, " ? ");
        putmess(buf);
        putch( (c = getlow()) );
        putret();
    }
    return c == 'y';
}


/* return YES if file 'name' exists, else NO
 */
checkexists(name)
char *name;
{
    int  fd;

/* if (dskcheck(setjmp(dskerr)) != 0 || (fd=open(name,0)) == FAIL) return NO; */
/* no setjmp just yet */
    if ( ( fd = open( name, 0 ) ) == FAIL )
        return  NO;
    close( fd );
    return  YES;
}


/* used to input filename, etc in message routine
 */
scans(answer,maxlen)
char *answer;
int maxlen;
{
    unsigned char c;
    int  n, i;

    maxlen--;
    n = 0;
    while ( n < maxlen )  {
        c = getscankey();
        switch(c) {
        case F3KEY :
        case F4KEY :
        case F2KEY :
            n = maxlen;
            break;  /* allows use of F3, F4, & F2 as cr for string search terminator*/
        case F1KEY :
        case F6KEY :
        case F5KEY :
        case F7KEY :
        case F8KEY :
        case F9KEY :
        case F10KEY :
            return  ESCKEY;  /* purpose unknown.  g.o. */
        case BACKSP :
        case DELLEFT_P :
            if ( n )  {
                putch(BACKSP);
                putch(' ');
                putch(BACKSP);
                n--;
                answer--;
            }
            break;
        case CR_P :
        case ESCKEY_P :
            n = maxlen;
            break;
        case LEFTKEY_P :   /* pass this key code thru */
            c = tran[LEFTKEY];
            goto passthru;
        case RETRIEVE_P :
/* if (n == 0) { */
            if ( *answer )  {
                while ( *answer )  {
                    dispch( *answer++ );
                    n++;
                }
                break;
            }
            else  {
                c = tran[RETRIEVE]; /*if no answer, display the key code*/
                goto passthru;
            }
        case GS :           /* kludge to permit ESC in find/alter */
            c = ESC;
        default :
passthru:
            dispch(c);
            *answer++ = c;
            n++;
            break;
        }
    }
    *answer = '\0';
/* this is a work-around until type integer characters are used */
    if (c != F3KEY && c != F4KEY && c != F2KEY)
        c = c & ~PARBIT;
    putret();                   /* set PARBIT of c back to 0 to */
    return (c);
}


/* puts a new suffix on a file name.  the filespec  ..\name.ext is acceptable
 * in this version.
 */
retag(name,tag)
char *name, *tag;
{
    char *namep, *name0;

    name0 = name;
    namep = name;
    while (*name) {
        if (*name == '.')
            namep = name;
         name++;
    }
    if (namep == name0) {
        namep = name;
        *namep = '.';
    }
    strcpy(namep+1,tag);
}


/* format filename for display */
format(name)
char *name;
{
    char tempname[FILELEN], *n, *tn;

    if ( !*name )
       return;

    if ( *(name+1) == ':' ) {
        strcpy( tempname, name );
    }
    else  {
        tempname[0] = curdsk + 'A';
        tempname[1] = ':';
        name[FILELEN-1] = '\0';
        strcpy( &tempname[2], name );
    }
    for ( n = name, tn = tempname; ( *n = toupper( *tn++ ) ); n++ )
        if ( *n == '.' )
            break;
    while ( *n++ )
        *n = tolower( *tn++ );
    return;
}






