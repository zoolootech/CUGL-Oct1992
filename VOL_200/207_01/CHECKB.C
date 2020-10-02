/*  checkb.c -- 3rd source file for check register program                   */

/*  copyright (c) 1986 by Jim Woolley and WoolleyWare, San Jose, CA          */

/*  vers. 1.0, 12/85 thru 5/86
 */

/*  this file contains:
 *      display( i)
 *      putrecord( i)
 *      update( i)
 *      newbalance( i)
 *      newentry( i)
 *      clrscr()
 *      clreol( c)
 *      dellin( r)
 *      inslin( r)
 *      putscr( z)
 *      godown( c)
 *      shodown()
 *      goup( c)
 *      goright( c)
 *      goleft( c)
 *      gonext()
 *      goprior()
 *      gobottom()
 *      gotop()
 *      goupdown( c)
 */

#include    "a:checks.h"

display( i)                             /*  display entries i thru Last      */
int i;
{
    char c;
    int firstrow, lastrow;

    c = 0;
    i = max( i, First);
    while ( i <= Last)
    {
        if ( !c && kbhit())             /*  look ahead for vertical motion   */
        {
            if ((( c = getchar()) == CTRLR && First) ||
                ( c == CTRLC && ( First + LAST) <= Maxentry))
                break;
            if ( !( *Lindel) && (       /*  if no line delete control and    */
                ( c == CTRLX && Recno >= ( Last - 1) &&
                    ( Last - First + HEAD) == ( ROWS - 1)) ||
                ( c == CTRLZ && First < Last)
                ))
                break;
            if ( !( *Linins) &&         /*  if no line insert control and    */
                First && (( c == CTRLE && Recno == First) || c == CTRLW))
                break;
        }
        putrecord( i++);
    }
    i = Last + 1;
    firstrow = First - HEAD;
    lastrow = firstrow + ROWS;
    while ( i < lastrow)
    {
        cursorto(( i - firstrow), 0);
        clreol( 0);                     /*  clear bottom of screen           */
        ++i;
    }
    if ( c)                             /*  restore keyboard char            */
        ungetch( c);
}

putrecord( i)                           /*  display Entry[ i]                */
int i;
{
    char c, *p, s[ 10];
    int count;
    struct record *e;

    e = &Entry[ i];
    cursorto(( i - First + HEAD), 0);
    putdate( &( e->date));
    count = PAYEESIZE - 1 - strlen( p = e->payee);
    puts( p);
    while ( count--)
        putchar( PAYEEFILL);
    putchar( ' ');
    putchar( e->category & 0x7f);
    putchar( ' ');
    putmoney( &( e->amount));
    puts( e->deposit ? "DEP " : DEPCLRFIL);
    puts( e->clear ? "CLR " : DEPCLRFIL);
    putmoney( &Balance[ i]);
}

update( i)                              /*  update balance for entry >= i    */
int i;
{
    int firstrow;

    firstrow = First - HEAD;
    while ( i <= Maxentry)
    {
        newbalance( i);
        if ( First <= i && i <= Last)
        {
            cursorto(( i - firstrow), BALCOL);
            putmoney( &Balance[ i]);
        }
        ++i;
    }
}

newbalance( i)                          /*  update balance for entry i       */
int i;
{
    struct record *e;
    struct money m, *p;

    if ( i)
        p = &Balance[ i - 1];
    else
    {
        m.dollar = 0;
        m.cent = 0;
        p = &m;
    }
    e = &Entry[ i];
    addmoney( &Balance[ i], p, &( e->amount), e->deposit);
}

newentry( i)                            /*  create and initialize entry i    */
int i;
{
    struct record *en, *ep;

    en = &Entry[ i];
    if ( Today.yy)
    {
        en->date.mm = Today.mm;
        en->date.dd = Today.dd;
        en->date.yy = Today.yy;
    }
    else if ( i)
    {
        ep = &Entry[ i - 1];
        en->date.mm = ep->date.mm;
        en->date.dd = ep->date.dd;
        en->date.yy = ep->date.yy;
    }
    en->payee[ 0] = '\0';
    en->category = DEFCAT;
    en->amount.dollar = 0;
    en->amount.cent = 0;
    en->deposit = FALSE;
    en->clear = FALSE;
    newbalance( i);
    Modified = TRUE;
}

clrscr()                                /*  clear screen and home cursor     */
{
    int i;

    if ( *Trmini)
        putscr( Trmini);
    else
    {
        cursorto(( ROWS - 1), 0);
        for ( i = ROWS; i; --i)
            putchar( '\n');
    }
    cursorto( 0, 0);
}

clreol( c)                              /*  clear column c to end-of-line    */
int c;                                  /*  0 is left edge                   */
{                                       /*  cursor must be set to column c   */
    if ( *Eraeol)
        putscr( Eraeol);
    else for ( ; c < COLS; ++c)
        putchar( ' ');
}

dellin( r)                              /*  delete line at row r             */
int r;                                  /*  move remaining lines up          */
{                                       /*  display new last line            */
    if ( lindel( r))
        display( First + LAST);
    else display( First + r - HEAD);
}

lindel( r)                              /*  if *Lindel != 0,                 */
int r;                                  /*  delete line at row r             */
{                                       /*  move remaining lines up          */
                                        /*  return TRUE; else, return FALSE  */
                                        /*  final cursor pos. indeterminate  */
    if ( *Lindel)
    {
        if ( *Lindel == 255)
        {
            puts( "\033[");             /*  special for VT100                */
            printd( r + 1);
            putchar( 'r');
            cursorto(( ROWS - 1), 0);
            puts( "\033D\033[1r");
        }
        else
        {
            cursorto( r, 0);
            putscr( Lindel);
        }
        return ( TRUE);
    }
    return ( FALSE);
}

inslin( r)                              /*  insert blank line at row r       */
int r;                                  /*  move existing lines down         */
{                                       /*  putrecord at row r               */
    if ( linins( r))
        putrecord( First + r - HEAD);
    else display( First + r - HEAD);
}

linins( r)                              /*  if *Linins != 0,                 */
int r;                                  /*  insert blank line at row r       */
{                                       /*  move existing lines down         */
                                        /*  position cursor at ( r, 0)       */
                                        /*  return TRUE; else, return FALSE  */
    if ( *Linins)
    {
        if ( *Linins == 255)
        {
            puts( "\033[");             /*  special for VT100                */
            printd( r + 1);
            putchar( 'r');
            cursorto( r, 0);
            puts( "\033M\033[1r");
            cursorto( r, 0);
        }
        else
        {
            cursorto( r, 0);
            putscr( Linins);
        }
        return ( TRUE);
    }
    return ( FALSE);
}

putscr( z)                              /*  send screen control chars        */
char *z;                                /*  first char is count of chars     */
{
    char c;

    for ( c = *z++; c; --c)
        putchar( *z++);
}

godown( c)                              /*  go down one entry                */
char c;
{
    if ( c == CTRLZ)
    {
        if ( Last <= First)
            return;
        ++First;                        /*  scroll up                        */
        Recno = max( Recno, First);
        Last = min(( Last + 1), Maxentry);
        shodown();
        return;
    }
    if ( ++Recno > ( Last - 1))         /*  at next to last on screen?       */
    {
        if ( Recno > Maxentry)          /*  at Maxentry?                     */
        {
            Recno = Maxentry + 1;
            Field = 0;
            if ( c == '\r')             /*  assume new entry desired         */
            {
                if ( Maxentry < ( ENTRYSIZE - 1))
                {
                    newentry( ++Maxentry);
                    putrecord( Last = Maxentry);
                    Today.yy = 0;       /*  use previous entry's date next   */
                }
                else
                {
                    prompt( "Number of entries is maximum allowed");
                    waitesc();
                }
            }
        }
        if (( Last - First + HEAD) == ( ROWS - 1))
        {
            ++First;                    /*  scroll up                        */
            Last = min(( Last + 1), Maxentry);
            shodown();
        }
    }
}

shodown()                               /*  scroll display up to show down   */
{
    if ( !( *Lindel))                   /*  if no delete line control        */
    {                                   /*      get a head start             */
        if ( Last < ( First + LAST))
        {
            cursorput(( Last + 1), 0);
            clreol( 0);
        }
        putrecord( Last);
    }
    dellin( HEAD);
}

goup( c)                                /*  go up one entry                  */
char c;
{
    if ( Recno != First && c != CTRLW)
    {
        --Recno;
        return;
    }
    if ( !First)
        return;
    --First;
    if ( !( *Linins))                   /*  if no insert line control        */
        putrecord( First);              /*      get a head start             */
    Last = min(( First + LAST), Maxentry);
    if ( c == CTRLW)
        Recno = min( Recno, ( First + ( LAST - 1)));
    else --Recno;
    inslin( HEAD);
}

goright( c)                             /*  move right within current entry  */
char c;
{
    char *p;
    int edge;
    struct record *e;

    if ( Recno > Maxentry)
    {
        godown( c);
        return;
    }
    if ( Field == PAYFIELD && c != '\r')
    {
        e = &Entry[ Recno];
        if ( Character < ( edge = min(( PAYEESIZE - 1), strlen( e->payee))))
        {
            if ( c == CTRLF || c == '\t')
            {
                --edge;
                p = e->payee + Character;
                while ( Character < edge)
                {
                    ++Character;
                    ++p;
                    if ( isspace( *( p - 1)) && !isspace( *p))
                        return;
                }
            }
            ++Character;
            return;
        }
    }
    if ( ++Field >= MAXFIELD)
    {
        Field = 0;
        godown( c);
    }
}

goleft( c)                              /*  move left within current entry   */
char c;
{
    char *p;

    p = Entry[ Recno].payee;
    if ( Field == PAYFIELD && Character > 0)
    {
        if ( c == CTRLA)
        {
            p += Character;
            while ( --Character)
            {
                --p;
                if ( isspace( *( p - 1)) && !isspace( *p))
                    return;
            }
        }
        else --Character;
    }
    else if ( Field)
    {
        Character = PAYEESIZE;          /*  putcursor() will readjust        */
        --Field;
    }
    else if ( !Recno)
        return;
    else
    {
        Field = MAXFIELD - 1;
        Character = PAYEESIZE;          /*  putcursor() will readjust        */
        goup( c);
    }
}

gonext()                                /*  display next page                */
{
    int i, n;

    if ( Recno > Maxentry)
        return;
    if (( First + LAST) > Maxentry)
    {
        Recno = Maxentry;
        godown( 0);
        return;
    }
    i = First + PAGE;
    Last = min( Maxentry, ( i + LAST));
    i = max( 0, min( i, ( Last - (LAST - PAGE))));
    n = i - First;
    First = i;
    Recno = min(( Recno + n), ( Maxentry + 1));
    if ( Recno <= Maxentry)
        putrecord( Recno);              /*  get a head start                 */
    display( First);
    if ( Recno > Maxentry)
        godown( 0);
}

goprior()                               /*  display previous page            */
{
    int i, n;

    if ( !Recno)
        return;
    if ( !First)
    {
        Recno = 0;
        return;
    }
    i = max(( First - PAGE), 0);
    n = First - i;
    First = i;
    Last = min( Maxentry, ( First + LAST));
    putrecord( Recno = max(( Recno - n), 0));  /*  get a head start          */
    display( First);
}

gobottom()                              /*  display last page                */
{
    Recno = max( Recno, ( Maxentry + ( 1 - PAGE)));
    First = max( First, ( Recno - PAGE));
    gonext();
}

gotop()                                 /*  display first page               */
{
    First = min( First, 1);
    Recno = min( Recno, 1);
    goprior();
}

goupdown( c)                            /*  continuous up or down            */
char c;
{
    int i;

    prompt( "Press digit to change speed, or other key to stop");
    c -= CTRLTOA;
    FOREVER
    {
        if ( c == CTRLW)
        {
            if ( First)
                goup( c);
            else return;
        }
        else
        {
            if ( Last != First)
                godown( c);
            else return;
        }
        for ( i = Speed*Dloop; i && !kbhit(); --i)
            continue;                   /*  wait Speed/10 sec for key        */
        if ( i)                         /*  if key hit                       */
        {
            if ( isdigit( i = getchar()))
                Speed = ( i == '0' ? 10 : i - '0');
            else return;
        }
    }
}
