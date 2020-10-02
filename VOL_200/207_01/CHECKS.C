/*  checks.c -- main source file for check register program                  */

/*  copyright (c) 1986 by Jim Woolley and WoolleyWare, San Jose, CA          */

/*  vers. 1.0, 12/85 thru 5/86
 */

/*  this file contains:
 *      main()
 *      startup()
 *      getval( q)
 *      getentry()
 *      getinfo()
 *      getyesno( def)
 *      struct nlist *install( name, def)
 *      struct nlist *lookup( s)
 *      hash( s)
 *      char *strsave( s)
 *      compdate( e1, e2)
 *      datecomp( d1, d2)
 *      comppayee( e1, e2)
 *      compcateg( e1, e2)
 *      categcomp( e1, e2)
 *      compamount( e1, e2)
 *      compbbf( e1, e2)
 *      compabrev( p1, p2)
 *      isebbf( e)
 *      isibbf( i)
 *      char *index( s, c)
 *      char *skipspace( s)
 *      typcat( f, t)
 *      openerr()
 *      readerr()
 *      writerr( s)
 *      createrr( s)
 *      baddisk()
 */

#include    "a:checks.h"

main( argc, argv)                       /*  check register program           */
int argc;
char *argv[];
{
    char *p, *copyright, *allrights;
    int i, length;

    copyright = "Check Register Program, v.1.0 (c) 1986 by WoolleyWare";
                /*  copyright must be < (COLS - FNAMSIZE - 3) chars          */
    allrights = "All Rights Reserved";

    _Outdev = CONOUT;                   /*  direct putchar() to screen       */
    _Lastch = 0;                        /*  used by getchar()/ungetch()      */
    Speed = 5;                          /*  initial Speed for ^QW and ^QZ    */
    Today.yy = 0;                       /*  initialize other globals         */
    Savrecno = Oldfield = -1;
    Modified = Printing = Ctrlyundo = FALSE;
    for ( i = 0; i < HASHSIZE; ++i)
        Hashtab[ i] = 0;
    strcpy( Title, copyright);
    Ftoc[ MMFIELD] = 1;                 /*  Ftoc[ f] = c                     */
    Ftoc[ DDFIELD] = 4;                 /*  locates cursor for Field f       */
    Ftoc[ YYFIELD] = 7;                 /*  at column c, where c = 0         */
    Ftoc[ PAYFIELD] = 9;                /*  corresponds to left edge         */
    Ftoc[ CATFIELD] = 52;               /*  of screen                        */
    Ftoc[ AMTFIELD] = 61;
    Ftoc[ DEPFIELD] = 63;
    Ftoc[ CLRFIELD] = 67;

    if ( argc > 1)                      /*  get Filename root                */
        p = argv[ 1];
    else p = DEFNAM;
    length = strlen( p);
    if ( !index( p, ':'))               /*  if no drive designated           */
    {
        Filename[ 0] = 'A' + defdsk();
        Filename[ 1] = ':';
        i = 2;
    }
    else                                /*  drive was designated             */
    {
        if ( length == 2)               /*  if just d: without filename      */
        {
            strcpy( Filename, p);
            p = DEFNAM;                 /*  use default                      */
            i = 2;
        }
        else i = 0;
    }
    if ( length > ( FNAMSIZE - 5 - i))  /*  should be FALSE if p = DEFNAM    */
        *( p + FNAMSIZE - 5 - i) = '\0';
    strcpy(( Filename + i), p);
    if ( !( p = index( Filename, '.'))) /*  add dot if none                  */
        *( p = Filename + strlen( Filename)) = '.';
    *( p + 1) = '\0';                   /*  truncate after dot               */

    startup();
    getentry();                         /*  read entry data                  */
    disheading();                       /*  initialize display               */
    Recno = Maxentry + ( 1 - PAGE);     /*  initialize gobottom()            */
    First = Recno - PAGE;
    Last = First - 1;
    Field = 0;
    gobottom();
    getinfo();                          /*  read title, abrev, auto entries  */
    control();                          /*  never returns                    */
}

startup()                               /*  startup check register program   */
{
    char line, *p, *q, s[ MAXLINE], f[ FNAMSIZE], buf[ BUFSIZ], *fgets();
    int i;

    strcpy( f, DEFNAM);
    typcat( f, SCRTYP);
    if ( fopen( f, buf) == ERROR)
        abort( "Cannot open ", f);      /*  never returns                    */
    for ( line = 6; line; --line)       /*  skip 6 lines                     */
        if ( !fgets( s, buf))
            readerr( f);                /*  never returns                    */
    for ( line = 0; line < 11; ++line)  /*  get cursor/screen controls       */
    {
        if ( !fgets(( q = s), buf))
            readerr( f);                /*  never returns                    */
        switch ( line)
        {
        case 0:
            p = Clead1;
            break;
        case 1:
            p = Clead2;
            break;
        case 2:
            p = Ctrail;
            break;
        case 3:
            Cb4flg = getval( &q);
            Linoff = getval( &q);
            Coloff = getval( &q);
            if ( Ascur = getval( &q))   /*  Ascur must be 0, 2, or 3         */
                Ascur = min( 3, max( 2, Ascur));
            break;
        case 4:
            p = Eraeol;
            break;
        case 5:
            p = Lindel;
            break;
        case 6:
            p = Linins;
            break;
        case 7:
            p = Ivon;
            break;
        case 8:
            p = Ivoff;
            break;
        case 9:
            p = Trmini;
            break;
        case 10:
            Dloop = (( DLOOP/10)*max( 1, min( 1000, getval( &q))))/10;
            Inserton = getval( &q);
            break;
        default:
            break;
        }
        if ( line != 3)                 /*  note that i may be negative      */
            for ( *p++ = i = getval( &q); i > 0; --i)
                *p++ = getval( &q);
    }
    if ( fgets( s, buf))                /*  if more than 17 lines            */
    {
        clrscr();
        while (( i = getc( buf)) != CPMEOF && i != ERROR)
            putchar( i - 1);
        if ( getchar() == CTRLC)
            exit();                     /*  never returns                    */
    }
    fclose( buf);
    clrscr();
}

/*  getval( q) returns next int value from string of decimal numbers separated
 *  by white space; string must be pointed to by *q; each decimal number in
 *  string may be headed by white space with optional minus sign followed by
 *  consecutive decimal digits; first non-digit terminates the scan; zero is
 *  returned if no legal value is found; *q will be updated to point to first
 *  white space char following the current decimal number; *q will not point
 *  beyond end of string
 *
 *  sample calling program segment:
 *
 *      char *p;                        string pointer
 *      p = " 1   -32 123   9";         point to typical space separated string
 *      printf( "%d", getval( &p));     pass pointer to string pointer
 */

getval( q)                              /*  return int value and update *q   */
char **q;                               /*  pointer to string pointer        */
{
    int i;

    i = atoi( *q = skipspace( *q));
    while ( !isspace( **q) && **q)
        ++( *q);
    return ( i);
}

getentry()                              /*  get check register entries       */
{
    char *p, buf[ BUFSIZ];
    int g, imax;

    typcat( Filename, DATTYP);
    if ( fopen( Filename, buf) == ERROR)
    {
        openerr( Filename);
        Maxentry = -1;                  /*  initialize                       */
        Memory.dollar = Memory.cent = 0;
        return;
    }
    if (( g = getw( buf)) == ERROR)
        readerr( Filename);             /*  never returns                    */
    imax = RECSIZ*g;
    Maxentry = g - 1;
    Memory.dollar = getw( buf);         /*  assume no read error             */
    Memory.cent = getw( buf);
    p = Entry;
    while ( imax--)
    {
        if (( g = getc( buf)) == ERROR)
            readerr( Filename);         /*  never returns                    */
        *p++ = g;
    }
    fclose( buf);
    for ( g = 0; g <= Maxentry; ++g)
        newbalance( g);
}

getinfo()                               /*  get title, abrev, and auto trans */
{
    char *p, *q, *amsg, c, new, next, def, savmodified;
    char *key[ 3], buf[ BUFSIZ], s[ MAXLINE], *fgets();
    char adate, adeposit, acategory;
    int i, delta, count, adollar, acent, compdate();
    struct record *e;
    struct calendar maxdate;

    key[ 0] = "TITLE";                  /*  initialize                       */
    key[ 1] = "ABREV";
    key[ 2] = "AUTOM";
    amsg = "Installing abreviations ... ";
    typcat( Filename, INFTYP);
    if ( fopen( Filename, buf) == ERROR)
    {
        openerr( Filename);
        return;
    }
    next = 0;
    while ( fgets( s, buf))             /*  read one line at a time          */
    {
        s[ strlen( s) - 1] = '\0';      /*  truncate Abc\n\0 at \n           */
        if ( p = index( s, '|'))        /*  skip comment starting with |     */
            *p = '\0';
        if ( s[ 0] == '\0')
            continue;
        for ( new = 3; new; --new)      /*  look for keyword                 */
        {
            p = key[ new - 1];
            for ( i = 0; i < 5;  ++i)
                if ( toupper( s[ i]) != *p++)
                    break;
            if ( i == 5)
            {
                if (( next = new) == 2)
                    prompt( amsg);
                break;                  /*  break for loop on new            */
            }
        }
        if ( new)                       /*  if keyword found                 */
            continue;                   /*      get next line from file      */
        switch ( next)
        {
        case 2:                         /*  keyword was ABREV                */
            if ( strlen( p = s) < 3)
            {
                prompt( "Invalid abreviation ");
                goto error;
            }
            while ( p < ( s + 3))       /*  make abreviation upper case      */
            {
                *p = toupper( *p);
                ++p;
            }
            *p++ = '\0';                /*  end abreviation                  */
            p = skipspace( p);          /*  locate full text                 */
            if ( strlen( p) > ( PAYEESIZE - 1))
                *( p + PAYEESIZE - 1) = '\0';
            if ( !install( s, p))
            {
                prompt( "Could not install abreviation ");
                goto error;
            }
            break;                      /*  break switch on next             */
error:      puts( s);                   /*  complete error message           */
            waitesc();
            prompt( amsg);
            break;                      /*  break switch on next             */
        case 3:                         /*  keyword was AUTOM                */
            if ( Maxentry == ( ENTRYSIZE - 1))
            {
                prompt( "Number of entries is maximum allowed");
                waitesc();
                next = 0;
                break;                  /*  break switch on next             */
            }
            if ( !Today.yy)             /*  initialize maxdate once          */
                maxdate.mm = maxdate.dd = maxdate.yy = 0;
            ++Maxentry;                 /*  create temporary entry           */
            ++Last;
            for ( i = 0; i < Maxentry; ++i)
                datemax( &maxdate, &( Entry[ i].date));
            cursorput( Recno, 0);       /*  Recno and Maxentry are same      */
            putdate( &maxdate);
            e = &Entry[ Recno];         /*  setup to use eddate              */
            e->date.mm = maxdate.mm;
            e->date.dd = maxdate.dd;
            e->date.yy = maxdate.yy;
            count = Ftoc[ YYFIELD] + 4;
            savmodified = Modified;
            FOREVER
            {
                if ( Today.yy)
                    goto query;
                cursorput( Recno, count);
                puts( p = "<<<  Enter today's date");
                Field = MMFIELD;
                while ( Field < ( YYFIELD + 1))
                {
                    putquery();
                    putcursor( Recno, Field);
                    c = eddate( getchar());
                    if ( c == ESC)
                        goto skipit;
                    switch ( c)
                    {
                        case 0:
                            break;
                        case CTRLD:  case '\r':  case CTRLF:  case '\t':
                            goright( c);
                            break;
                        case CTRLS:  case '\b':  case CTRLA:
                            if ( Field > MMFIELD)
                            {
                                goleft( c);
                                break;
                            }           /*  else fall thru                   */
                        default:
                            putchar( BEL);
                            break;
                    }                   /*  end of switch on c               */
                }                       /*  end of while loop on Field       */
query:          cursorput( Recno, count);
                clreol( count);
                prompt( "Do you wish to revise today's date (Y/N)? ");
                if ( getyesno( NO))
                    Today.yy = 0;
                else break;             /*  break FOREVER loop               */
            }                           /*  end of FOREVER loop              */
skipit:     Today.mm = e->date.mm;
            Today.dd = e->date.dd;
            Today.yy = e->date.yy;
            if ( Recno > 0)             /*  delta = last date to cur month   */
            {                           /*  limit 1 year; use 32 days/month  */
                delta = ( min( 1, ( Today.yy - maxdate.yy))*12
                        + ( Today.mm - maxdate.mm - 1))*32 + ( 32 - maxdate.dd);
            }
            else delta = 0;

/*  graphical representation of various automatic entry parameters:
 *
 *  when month of most recent entry (last) < current month (today):
 *
 *      | delta |        Today.dd        |
 *      |       |  adate |               |
 *  ----|-------|--------|---------------|-----------|---
 *     last   32|0      auto           today       32|
 *  last month  |           current month            |
 *
 *  when month of most recent entry (last) = current month (today):
 *
 *              |        Today.dd        | 
 *              | -delta | delta+Today.dd|
 *  ------------|--------|-----|---------|-----------|---
 *            32|0      last  auto     today       32|
 *  last month  |           current month            |
 */
            cursorput( Recno, 0);       /*  clear today's date               */
            clreol( 0);
            --Maxentry;                 /*  delete temporary entry           */
            --Last;
            Field = 0;
            Modified = savmodified;
            if (( delta + Today.dd) <= 0)
            {
                next = 0;
                break;                  /*  break switch on next             */
            }
            next = 4;                   /*  prepare for next line from file  */
        case 4:                         /*  interpret automatic transaction  */
            adate = atoi( p = skipspace( s));
            acategory = DEFCAT;
            if ( q = index( p, ' '))
                acategory = *( p = skipspace( q));
            if ( acategory == '-')
                acategory = DEFCAT;
            else acategory = toupper( acategory);
            adollar = acent = 0;
            if ( q = index( p, ' '))
            {
                q = skipspace( q);      /*  start of amount                  */
                while ( *q == '-')
                    ++q;                /*  ignore minus signs               */
                if ( p = index( q, '.'))
                    acent = atoi( p + 1);
                else p = index( q, ' ');
                if (( p - q) > 2)
                {
                    p -= 2;
                    acent += 100*atoi( p);
                    *p = '|';           /*  mark end of adollar              */
                    adollar = atoi( q);
                }
                else acent += 100*atoi( q);
                p = q;
            }
            adeposit = FALSE;
            if ( q = index( p, ' '))
                adeposit = ( toupper( *( p = skipspace( q))) == 'D');
            if ( q = index( p, ' '))
                p = skipspace( q);      /*  start of payee                   */
            if ( strlen( p) > ( PAYEESIZE - 1))
                *( p + PAYEESIZE - 1) = '\0';
            count = max( 0, ( delta + adate - 1)/32);
            if ( Today.dd >= adate && -delta < adate)
                ++count;                /*  count is number of entries       */
            while ( count--)            /*  skip if count is zero            */
            {
                savmodified = Modified;
                if ( !insert())         /*  if cannot insert Recno           */
                {
                    next = 0;
                    break;              /*  break while loop on count        */
                }
                e = &Entry[ Recno];
                i = Today.mm - count;
                if ( Today.dd < adate)
                    --i;
                e->date.yy = Today.yy;
                while ( i <= 0)
                {
                    i += 12;
                    --( e->date.yy);
                }
                e->date.mm = i;
                e->date.dd = adate;
                strcpy( e->payee, p);
                e->category = acategory;
                e->amount.dollar = adollar;
                e->amount.cent = acent;
                e->deposit = adeposit;
                newbalance( Recno);
                putrecord( Recno);
                def = NO;               /*  default                          */
                c = ESC;
                while ( c == ESC)
                {
                    prompt( "Do you accept this automatic entry (Y/N)? ");
                    def = !def;         /*  reverse                          */
                    putchar( def ? 'Y' : 'N');
                    putcursor( Recno, ( Field = 0));
                    c = getchar();
                }
                c = toupper( c);
                if ( c == 'N' || ( !def && c != 'Y'))
                {
                    delete();           /*  delete Recno                     */
                    Modified = savmodified;
                    Ctrlyundo = FALSE;
                }
                else godown( CTRLX);
            }                           /*  end of while loop on count       */
            break;                      /*  break switch on next             */
        case 1:                         /*  keyword was TITLE                */
            s[ COLS - FNAMSIZE - 3] = '\0';
            strcpy( Title, s);          /*  s truncated at max Title length  */
            cursorto( 0, 0);
            putscr( Ivon);
            puttitle();
            putscr( Ivoff);
            break;                      /*  break switch on next             */
        default:
            break;                      /*  break switch on next             */
        }                               /*  end of switch on next            */
    }                                   /*  end of while loop on fgets       */
    fclose( buf);
}

getyesno( def)                          /*  return YES or NO response        */
char def;                               /*  default response                 */
{
    char c;

    def = !def;                         /*  reverse default                  */
    c = ESC;
    while ( c == ESC)
    {
        def = !def;                     /*  reverse default                  */
        putchar( def ? 'Y' : 'N');      /*  display default                  */
        putchar( '\b');
        c = getchar();
    }
    c = toupper( c);
    if (( def && c != 'N') || ( !def && c != 'Y'))
        return ( def);
    def = !def;                         /*  reverse default                  */
    putchar( def ? 'Y' : 'N');          /*  display response                 */
    return ( def);
}

struct nlist *install( name, def)       /*  install in Hashtab               */
char *name;                             /*  abrev                            */
char *def;                              /*  fullname                         */
{                                       /*  ref. K & R, p. 136               */
    int hashval;
    struct nlist *np;

    if ( !( np = lookup( name)))        /*  if not found                     */
    {
        if ( !( np = alloc( sizeof( *np))))
            return ( 0);
        if ( !( np->abrev = strsave( name)))
            return ( 0);
        hashval = hash( np->abrev);
        np->next = Hashtab[ hashval];   /*  initialized to zero              */
        Hashtab[ hashval] = np;
    }
    else if ( np->fullname)
        free( np->fullname);
    if ( !( np->fullname = strsave( def)))
        return ( 0);
    return ( np);
}

struct nlist *lookup( s)                /*  look for s in Hashtab            */
char *s;
{                                       /*  ref. K & R, p. 135               */
    struct nlist *np;

    for ( np = Hashtab[ hash( s)]; np; np = np->next)
        if ( !strcmp( s, np->abrev))
            return ( np);               /*  found                            */
    return ( 0);                        /*  not found                        */
}

hash( s)                                /*  determine hash value for s       */
char *s;
{                                       /*  ref. K & R, p. 135               */
    int hashval;

    hashval = 0;
    while ( *s)
        hashval += *s++;
    return ( hashval%HASHSIZE);
}

char *strsave( s)                       /*  save s somewhere                 */
char *s;
{                                       /*  ref. K & R, p. 103               */
    char *p;

    if ( p = alloc( strlen( s) + 1))
        strcpy( p, s);
    return ( p);
}

compdate( e1, e2)                       /*  return  1 if date of e1 > e2     */
struct record *e1, *e2;                 /*  return -1 if date of e1 < e2     */
{                                       /*  else, return strcmp on payee     */
    int test;

    if ( test = compbbf( e1, e2))       /*  sort BBF entries to top          */
        return ( test);
    if ( test = datecomp( &( e1->date), &( e2->date)))
        return ( test);
    return ( strcmp( e1->payee, e2->payee));
}

datecomp( d1, d2)                       /*  return  1 if calendar d1 > d2    */
struct calendar *d1, *d2;               /*  return -1 if calendar d1 < d2    */
{                                       /*  else, return 0                   */
    if ( d1->yy > d2->yy)
        return ( 1);
    if ( d1->yy < d2->yy)
        return ( -1);
    if ( d1->mm > d2->mm)
        return ( 1);
    if ( d1->mm < d2->mm)
        return ( -1);
    if ( d1->dd > d2->dd)
        return ( 1);
    if ( d1->dd < d2->dd)
        return ( -1);
    return ( 0);
}

comppayee( e1, e2)                      /*  return  1 if payee of e1 > e2    */
struct record *e1, *e2;                 /*  return -1 if payee of e1 < e2    */
{                                       /*  else, return compdate( e1, e2)   */
    int test;

    if ( test = compbbf( e1, e2))       /*  sort BBF entries to top          */
        return ( test);
    if ( test = strcmp( e1->payee, e2->payee))
        return ( test);
    return ( compdate( e1, e2));
}

compcateg( e1, e2)                      /*  return  1 if category of e1 > e2 */
struct record *e1, *e2;                 /*  return -1 if category of e1 < e2 */
{                                       /*  else, return compdate( e1, e2)   */
    int test;

    if ( test = compbbf( e1, e2))       /*  sort BBF entries to top          */
        return ( test);
    return ( categcomp( e1, e2));
}

categcomp( e1, e2)                      /*  return  1 if category of e1 > e2 */
struct record *e1, *e2;                 /*  return -1 if category of e1 < e2 */
{                                       /*  else, return compdate( e1, e2)   */
    if ( e1->category > e2->category)
        return ( 1);
    if ( e1->category < e2->category)
        return ( -1);
    return ( compdate( e1, e2));
}

compamount( e1, e2)                     /*  return  1 if amount of e1 > e2   */
struct record *e1, *e2;                 /*  return -1 if amount of e1 < e2   */
{                                       /*  else, return compdate( e1, e2)   */
    int test;
    struct money *m1, *m2;

    if ( test = compbbf( e1, e2))       /*  sort BBF entries to top          */
        return ( test);
    m1 = &( e1->amount);
    m2 = &( e2->amount);
    if ( m1->dollar > m2->dollar)
        return ( 1);
    if ( m1->dollar < m2->dollar)
        return ( -1);
    if ( m1->cent > m2->cent)
        return ( 1);
    if ( m1->cent < m2->cent)
        return ( -1);
    return ( compdate( e1, e2));
}

compbbf( e1, e2)                        /*  return  1 if e1 not BBF, e2 is   */
struct record *e1, *e2;                 /*  return -1 if e1 is BBF, e2 not   */
{                                       /*  return  1 if both BBF, e1 > e2   */
                                        /*  return -1 if both BBF, e1 < e2   */
                                        /*  else, return 0                   */
    if ( isebbf( e1))                    
    {
        if ( isebbf( e2))               /*  note two BBF entries cannot      */
            return ( categcomp( e1, e2));   /*  have the same category       */
        return ( -1);
    }
    if ( isebbf( e2))
        return ( 1);
    return ( 0);
}

compabrev( p1, p2)                      /*  return  1 if abrev at p1 > p2    */
struct nlist **p1, **p2;                /*  return -1 if abrev at p1 < p2    */
{                                       /*  else, return 0 (not possible)    */
    return ( strcmp(( *p1)->abrev, ( *p2)->abrev));
}

isebbf( e)                              /*  return TRUE if e is a BBF entry  */
struct record *e;                       /*  else, return FALSE               */
{
    return ( e->category & 0x80);
}

isibbf( i)                              /*  return TRUE if Entry[ i] is BBF  */
{                                       /*  else, return FALSE               */
    return ( isebbf( &Entry[ i]));
}

char *index( s, c)                      /*  point to char c in string s      */
char *s, c;
{
    while ( *s && ( *s != c))
        ++s;
    return ( *s == c ? s : NULL);
}

char *skipspace( s)                     /*  point to next non-space in s     */
char *s;
{
    while ( *s && isspace( *s))
        ++s;
    return ( s);
}

typcat( f, t)                           /*  add filetype t to filename f     */
char *f, *t;
{
    strcpy(( index( f, '.') + 1), t);   /*  filename f MUST have a dot       */
}

openerr( f)                             /*  display file open error          */
char *f;
{
    prompt( "Could not open ");
    puts( f);
    waitkey();                          /*  may never return                 */
}

readerr( f)                             /*  display file read error          */
char *f;
{
    abort( "Error reading ", f);        /*  never returns                    */
}

writerr( f)                             /*  display file write error         */
char *f;
{
    prompt( "Error writing ");
    puts( f);
    waitesc();
    unlink( f);
    baddisk();
}

createrr( f)                            /*  display file write error         */
char *f;
{
    prompt( "Could not create ");
    puts( f);
    waitesc();
    baddisk();
}

baddisk()                               /*  prompt for another disk          */
{
    prompt( "Try another disk in ");
    putchar( Filename[ 0]);
    putchar( Filename[ 1]);
    waitesc();
}
