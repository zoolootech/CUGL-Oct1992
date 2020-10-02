/*  checkc.c -- 4th source file for check register program                   */

/*  copyright (c) 1986 by Jim Woolley and WoolleyWare, San Jose, CA          */

/*  vers. 1.0, 12/85 thru 5/86
 */

/*  this file contains:
 *      putcursor( i, f)
 *      cursorput( i, c)
 *      cursorto( r, c)
 *      putpos( i)
 *      puttitle()
 *      disheading()
 *      recheading( s1, s2)
 *      putquery()
 *      prompt( s)
 *      char putcommand( c)
 *      char putcntrl( c)
 *      char putnext()
 *      addmoney( m3, m1, m2, add)
 *      movmoney( m2, m1, credit)
 *      putmoney( m)
 *      putdate( d)
 *      formatd( s, n, d)
 *      printd( d)
 *      sign( i)
 *      char witch( c)
 *      iscntrl( c)
 *      waitesc()
 *      waitkey()
 *      done()
 *      saveclr()
 *      savedat()
 *      calcbbf()
 *      char help()
 *      abort( s1, s2)
 *      aexit()
 */

#include    "a:checks.h"

putcursor( i, f)                        /*  put cursor in entry i, field f   */
int i, f;
{
    if ( f == PAYFIELD)
        Character = min( Character, strlen( Entry[ i].payee));
    else Character = 0;
    cursorto(( i - First + HEAD), ( Ftoc[ f] + Character));
}

cursorput( i, c)                        /*  put cursor in entry i, column c  */
int i, c;
{
    cursorto(( i - First + HEAD), c);
}

cursorto( r, c)                         /*  put cursor at row r, column c    */
int r, c;                               /*  0, 0 is upper-left corner        */
{
    int i;

    if ( Printing)
        putchar( '\n');
    else
    {
        r += Linoff;                    /*  add offset                       */
        c += Coloff;
        if ( Cb4flg)
        {
            i = r;                      /*  swap                             */
            r = c;
            c = i;
        }
        putscr( Clead1);
        putpos( r);
        putscr( Clead2);
        putpos( c);
        putscr( Ctrail);
    }
}

putpos( i)                              /*  send cursor position i           */
int i;                                  /*  ( 0 + offset) is home            */
{
    char *p, s[ 4];

    if ( Ascur)                         /*  if ASCII wanted                  */
    {
        strcpy(( p = s), "000");        /*  template                         */
        if ( Ascur < 3)
            ++p;
        formatd( p, Ascur, i);
        puts( p);
    }
    else putchar( i);
}

puttitle()
{
    char f[ FNAMSIZE];
    int i, length;

    strcpy( f, Filename);
    *( index( f, '.')) = '\0';          /*  truncate at dot                  */

/*  display 5 spaces on left, right justify f on column (COLS - 1), precede
 *  f with 2 spaces, then center Title in the remaining area; i.e.,
 *  _____<<<<<<<<<<<<<<<<<<<<<<< center Title >>>>>>>>>>>>>>>>>>>>>__A:FILENAME
 */
    length = strlen( Title);
    i = 5 + (( COLS - FNAMSIZE - 3) - length)/2;
    length += i;
    while ( i--)                        /*  center title                     */
        putchar( ' ');
    puts( Title);
    i = ( COLS - 1) - length - strlen( f);
    while ( i--)                        /*  fill title line (with Ivon)      */
        putchar( ' ');
    puts( f);
    i = HEAD - 2;
    while ( i--)
        putchar( '\n');
}

disheading()                            /*  display heading for display()    */
{
    if ( !Printing)
    {
        cursorto( 0, 0);
        putscr( Ivon);
    }
    puttitle();
    puts( 
"  Date   Payee                                      C  Amount  DEP CLR  Balance"
         );
    putchar( '\n');
    puts( 
"-------- ------------------------------------------ - -------- --- --- --------"
         );
    if ( !Printing)
        putscr( Ivoff);
}

recheading( s1, s2)                     /*  display heading for reconcile()  */
char *s1, *s2;
{
    if ( !Printing)
    {
        clrscr();
        putscr( Ivon);
    }
    puttitle();
    puts( 
"  Last      Balance  _____ Cleared Entries ______  _______ All Entries ________"
         );
    putchar( '\n');
    puts(
"  Date   C  Forward   Checks   Deposits   Balance   Checks   Deposits   Balance"
         );
    putchar( '\n');
    puts( s1);
    puts( s2);
    if ( !Printing)
        putscr( Ivoff);
}

putquery()                              /*  select and display query         */
{
    char test, cwitch, dep;

    if ( Recno > Maxentry)
    {
        prompt( "Press RETURN to start a new entry, or ^J for help");
        return;
    }
    if ( isibbf( Recno))                /*  if a BBF entry                   */
    {
        prompt( "Do you wish to void this entry's status as ");
        puts( BBF);
        puts( " (Y/N)? N");
        return;
    }
    test = ( Field == Oldfield);
    cwitch = Field;
    if ( test && cwitch != DEPFIELD && cwitch != CLRFIELD)
        return;
    switch ( cwitch)
    {
    case MMFIELD:
        prompt( "Enter month");
        break;
    case DDFIELD:
        prompt( "Enter date");
        break;
    case YYFIELD:
        prompt( "Enter year");
        break;
    case PAYFIELD:
        prompt( "Enter/edit payee    ( INSERT ");
        puts( Inserton ? "ON )" : "OFF )");
        break;
    case CATFIELD:
        prompt( "Enter category");
        break;
    case AMTFIELD:
        prompt( "Enter/calculate amount");
        cursorto(( HEAD - 4), ( AMTCOL - 9));
        puts( "MEMORY = ");
        putmoney( &Memory);
        break;
    case DEPFIELD:
        prompt( "Is entry a deposit (Y/N)? ");
        dep = Entry[ Recno].deposit;
        goto yesno;
    case CLRFIELD:
        prompt( "Has entry cleared the bank (Y/N)? ");
        dep = Entry[ Recno].clear;
yesno:  putchar( dep ? 'Y' : 'N');
        break;
    }
    return;
}

prompt( s)                              /*  prompt for entry                 */
char *s;
{
    int c;

    cursorto(( HEAD - 4), 0);
    puts( s);
    clreol( c = strlen( s));
    if ( !( *Eraeol))                   /*  if no Eraeol string              */
        cursorto(( HEAD - 4), c);       /*  return cursor                    */
    Oldfield = -1;
}

char putcommand( c)                     /*  display command                  */
char c;                                 /*  return TRUE if cursor moved      */
{
    switch ( c)
    {
    case CTRLK:  case CTRLQ:  case CTRLC:  case CTRLR:
    case CTRLB:  case CTRLO:  case CTRLP:
        cursorto( 0, 0);
        putscr( Ivon);
        puts( CLRCOM);
        putchar( '\r');
        putcntrl( c);
        putscr( Ivoff);
        return ( TRUE);
    default:
        return ( FALSE);
    }
}

char putcntrl( c)                       /*  display control char             */
char c;                                 /*  return ( c + CTRLTOA) or DEL     */
{
    if ( c != DEL)
    {
        putchar( '^');
        putchar( c += CTRLTOA);
    }
    else puts( "DEL");
    return ( c);
}

char putnext()                          /*  display next command character   */
{                                       /*  return upper case character      */
    char c;

    putscr( Ivon);
    if ( isprint( c = getchar()))
    {
        putchar( c);
        c = toupper( c);
    }
    else c = putcntrl( c);
    putscr( Ivoff);
    return ( c);
}

addmoney( m3, m1, m2, add)              /*  get m3 = m1 + m2 if add = TRUE   */
struct money *m3, *m1, *m2;             /*  get m3 = m1 - m2 if add = FALSE  */
char add;
{
    int d, c, sd;

    if ( add)
    {
        d = m1->dollar + m2->dollar;
        c = m1->cent + m2->cent;
    }
    else
    {
        d = m1->dollar - m2->dollar;
        c = m1->cent - m2->cent;
    }
    if ( d && ( sd = sign( d)) != sign( c))
    {
        d -= sd;
        if ( sd < 0)
            c -= 10000;
        else c += 10000;
    }
    m3->dollar = d + c/10000;
    m3->cent = c%10000;
}

movmoney( m2, m1, credit)               /*  set m2 = m1 if credit = TRUE     */
struct money *m2, *m1;                  /*  else, set m2 = abs( m1)          */
char credit;
{
    if ( credit)
    {
        m2->dollar = m1->dollar;
        m2->cent = m1->cent;
    }
    else
    {
        m2->dollar = abs( m1->dollar);
        m2->cent = abs( m1->cent);
    }
}

putmoney( m)                            /*  format and display money         */
struct money *m;
{
    char s[ 10];
    int is, id, ic;                     /*  int is assumed to be 16 bits     */
    struct money w;

    if (( id = m->dollar) < -99 || 999 < id)
    {
        w.dollar = 0;
        w.cent = 50;                    /*  round-off to nearest dollar      */
        addmoney( &w, m, &w, ( id > 0));
        id = w.dollar;
        ic = w.cent;
        is = 6;
        strcpy( s, "       0 ");        /*  template                         */
    }
    else
    {
        ic = m->cent;
        is = 3;
        strcpy( s, "    0.00 ");        /*  template                         */
    }
    if ( id < 0 || ic < 0)
    {
        id = abs( id);
        ic = abs( ic);
        s[ 0] = '(';
        s[ 8] = ')';
    }
    if ( id)
    {
        formatd( s, is, id);
        s[ is] = '0';                   /*  fill                             */
    }
    formatd(( s + is), 2, ic/100);
    if ( is == 3)
        formatd(( s + 6), 2, ic%100);
    puts( s);
}

putdate( d)                             /*  format and display date          */
struct calendar *d;
{
    char s[ 10];

    strcpy( s, " 0/00/00 ");            /*  template                         */
    formatd( s, 2, d->mm);
    formatd(( s + 3), 2, d->dd);
    formatd(( s + 6), 2, d->yy);
    puts( s);
}

formatd¨ s¬ n¬ d©                       /ª  formaô î digitó oæ ä intï ó      */
char s[];                               /*  like sprintf( s, "%nd", d)       */
int n, d;                               /*  d is assumed to be positive      */
{                                       /*  and n decimal digits or less     */
    while ( d && n--)                   /*  s is assumed to be initialized   */
    {                                   /*  with an appropriate template     */
        s[ n] = '0' + d%10;
        d /= 10;
    }
}

printd( d)                              /*  print d in decimal               */
int d;                                  /*  like printf( "%d", d)            */
{                                       /*  d is assumed to be positive      */
    int n;                              /*  see K & R, p. 85                 */

    if ( n = d/10)
        printd( n);                     /*  recursion for leading digits     */
    putchar( '0' + d%10);               /*  trailing digit                   */
}

sign( i)                                /*  return 1 with sign of i          */
{
    return ( i < 0 ? -1 : 1);
}

char witch( c)                          /*  return control character         */
char c;                                 /*  else return CR                   */
{
    if ( iscntrl( c))
        return ( c);
    return ( '\r');
}

iscntrl( c)                             /*  is c a control char?             */
char c;
{
    return ( c < ' ' || c == DEL);
}

waitesc()                               /*  wait for ESCAPE key              */
{
    puts( "  --  Press ESCAPE to continue");
    putchar( BEL);
    while ( getchar() != ESC)
        continue;
}

waitkey()                               /*  wait for CTRL-C or other key     */
{
    puts( "  --  ^C to quit, or any other key to continue");
    putchar( BEL);
    if ( getchar() == CTRLC)
        aexit();                        /*  never return                     */
}

done()                                  /*  save and archive check register  */
{
    if ( saveclr())
        return;
    calcbbf();
    if ( savedat())
    {
        Recno = Maxentry + 1 - PAGE;    /*  initialize gobottom()            */
        First = Recno - PAGE;
        Last = First - 1;
        Field = 0;
        gobottom();                     /*  redisplay                        */
        return;
    }
    aexit();                            /*  no return                        */
}

saveclr()                               /*  save cleared entries             */
{                                       /*  return TRUE if bad disk          */
    char *p;
    char tempname[ FNAMSIZE], bufin[ BUFSIZ], bufout[ BUFSIZ];
    int i, j, count, oldcount, imax, total, compdate();

    typcat( Filename, CLRTYP);
    count = 0;                          /*  count number of cleared entries  */
    for ( i = 0; i <= Maxentry; ++i)
        if ( Entry[ i].clear)
            ++count;
    if ( !count)
    {
        prompt( "No change necessary to ");
        puts( Filename);
        puts( " - ");
        return ( FALSE);
    }
    prompt( "");
    reorder( 0, compdate);
    strcpy( tempname, Filename);
    typcat( tempname, TMPTYP);
    resetdsk();
    if ( fcreat( tempname, bufout) == ERROR)
    {
        createrr( tempname);
        return ( TRUE);
    }
    if ( fopen( Filename, bufin) == ERROR)
        oldcount = 0;
    else
    {
        oldcount = getw( bufin);     	/*  records in CLR file              */
        getw( bufin);                   /*  filler                           */
        getw( bufin);                   /*  filler                           */
    }
    if ( imax = max( 0, ( RECSIZ*( oldcount + count - CLRSIZE))))
    {
        prompt( Filename);
        puts( " is full.  OK to discard oldest cleared entries (Y/N)? ");
        if ( !getyesno( YES))
            return ( FALSE);
        for ( i = imax; i; --i)
            getc( bufin);               /*  discard old records              */
    }
    imax = RECSIZ*oldcount - imax;
    total = min( CLRSIZE, ( oldcount + count));
    putw( total, bufout);               /*  record new total record count    */
    putw( 0, bufout);                   /*  filler                           */
    putw( 0, bufout);                   /*  filler                           */
    for ( i = imax; i; --i)             /*  transfer old records             */
        if ( putc( getc( bufin), bufout) == ERROR)
            goto error;
    if ( oldcount)
        fclose( bufin);
    for ( i = 0; i <= Maxentry; ++i)
    {
        if ( Entry[ i].clear)
        {
            p = &Entry[ i];
            for ( j = RECSIZ; j; --j)
                if ( putc( *p++, bufout) == ERROR)
                    goto error;
        }
    }
    if ( fflush( bufout) == ERROR)      /*  make sure it is all saved        */
        goto error;
    fclose( bufout);
    prompt( "Do you wish to print an audit trail of cleared entries (Y/N)? ");
    if ( getyesno( NO) && setlst())
    {
        disheading();
        for ( i = 0; i <= Maxentry; ++i)
        {
            if ( Entry[ i].clear)
            {
                Balance[ i].dollar = Balance[ i].cent = 0;
                putrecord( i);          /*  print cleared entry              */
            }
        }
        resetlst();
    }
    unlink( Filename);
    if ( rename( tempname, Filename) == ERROR)
    {
        prompt( "Could not rename ");
        puts( tempname);
        puts( " to ");
        puts( Filename);
        waitesc();
        strcpy( Filename, tempname);
    }
    prompt( Filename);
    puts( " has ");
    printd( count);
    puts( " new entries (");
    printd( total);
    puts( " total) - ");
    return ( FALSE);                    /*  normal return                    */

error:
    fclose( bufout);
    writerr( tempname);
    return ( TRUE);
}

savedat()                               /*  save all entries                 */
{                                       /*  return TRUE if bad disk          */
    char *p, tempname[ FNAMSIZE], backname[ FNAMSIZE], bufout[ BUFSIZ];
    int i, j;

    typcat( Filename, DATTYP);
    strcpy( tempname, Filename);
    typcat( tempname, TMPTYP);
    strcpy( backname, Filename);
    typcat( backname, BAKTYP);
    resetdsk();
    if ( fcreat( tempname, bufout) == ERROR)
    {
        createrr( tempname);
        return ( TRUE);
    }
    putw(( Maxentry + 1), bufout);      /*  record number of entries         */
    putw( Memory.dollar, bufout);       /*  record Memory                    */
    putw( Memory.cent, bufout);
    for ( i = 0; i <= Maxentry; ++i)
    {
        p = &Entry[ i];
        for ( j = RECSIZ; j; --j)
            if ( putc( *p++, bufout) == ERROR)
                goto error;
    }
    if ( fflush( bufout) == ERROR)      /*  make sure it is all saved        */
        goto error;
    fclose( bufout);
    unlink( backname);
    rename( Filename, backname);
    rename( tempname, Filename);
    puts( Filename);
    puts( " has ");
    printd( Maxentry + 1);
    puts( " entries");
    return ( FALSE);                    /*  normal return                    */

error:
    fclose( bufout);
    writerr( tempname);
    return ( TRUE);
}

calcbbf()                               /*  calculate BBF for each category  */
{                                       /*  BBF = BALANCE BROUGHT FORWARD    */
    char c, nbr[ DEL];
    int i, j, count, new;
    struct record *e;
    struct money sum;
    struct calendar maxdate;

    setmem( nbr, DEL, 0);               /*  initialize                       */
    for ( j = 0; j <= Maxentry; ++j)    /*  identify categories              */
    {
        e = &Entry[ j];
        c = e->category;
        if ( c & 0x80)                  /*  if BBF category                  */
            e->clear = TRUE;            /*      mark for summation           */
        if ( e->clear)                  /*  count cleared entries            */
            ++nbr[ c & 0x7f];           /*      for each category            */
    }
    new = 0;
    for ( i = ' '; i < DEL; ++i)        /*  sum cleared categories           */
    {                                   /*      in ASCII order               */
        if ( !nbr[ i])                  /*  if category does not exist       */
            continue;                   /*      next category                */
        count = 0;
        maxdate.mm = maxdate.dd = maxdate.yy = sum.dollar = sum.cent = 0;
        for ( j = 0; j <= Maxentry; ++j)
        {
            e = &Entry[ j];
            if ( !( e->clear) || ( e->category & 0x7f) != i)
                continue;               /*  next entry                       */
            addmoney( &sum, &sum, &( e->amount), e->deposit);
            datemax( &maxdate, &( e->date));
            if ( j < Maxentry)          /*  delete cleared category          */
                movmem( &Entry[ j + 1], e, ( Maxentry - j)*RECSIZ);
            --Maxentry;
            --j;                        /*  Entry[ j + 1] moved up           */
            if ( ++count == nbr[ i])    /*  if all entries for this category */
                break;                  /*      break j loop over entries    */
        }                               /*  end j loop over entries          */
        e = &Entry[ new];               /*  insert new BBF category          */
        if ( new <= Maxentry)
            movmem( e, &Entry[ new + 1], ( Maxentry - new + 1)*RECSIZ);
        ++Maxentry;
        ++new;
        if ( sum.dollar < 0 || sum.cent < 0)
            e->deposit = FALSE;
        else e->deposit = TRUE;
        movmoney( &( e->amount), &sum, e->deposit);
        e->date.mm = maxdate.mm;
        e->date.dd = maxdate.dd;
        e->date.yy = maxdate.yy;
        e->clear = FALSE;
        e->category = i | 0x80;
        strcpy( e->payee, BBF);
    }                                   /*  end i loop over categories       */
}

char help()                             /*  display help screen              */
{
    char c, s[ MAXLINE], f[ FNAMSIZE], buf[ BUFSIZ], *fgets();
    int i;

    strcpy( f, DEFNAM);
    typcat( f, SCRTYP);
    if ( fopen( f, buf) == ERROR)
    {
        openerr( f);
        return( 0);
    }
    cursorto( 0, 0);
    putscr( Ivon);
    for ( i = 6; i-- && fgets( s, buf); )
        puts( s);
    putscr( Ivoff);
    fclose( buf);
    putcursor( Recno, Field);
    c = getchar();                      /*  wait for another command         */
    for ( i = 0; i < HEAD; ++i)         /*  clear top of screen              */
    {
        cursorto( i, 0);
        clreol( 0);
    }
    disheading();
    putcursor( Recno, Field);
    return ( c);
}

abort( s1, s2)                          /*  abort with two part message      */
char *s1, *s2;
{
    cursorto(( ROWS - 2), 0);
    clreol( 0);
    putchar( '\r');
    puts( s1);
    puts( s2);
    putchar( BEL);
    aexit();                            /*  no return                        */
}

aexit()                                 /*  clear last line and exit         */
{
    cursorto(( ROWS - 1), 0);
    clreol( 0);
    cursorto(( ROWS - 2), 0);
    exit();
}
