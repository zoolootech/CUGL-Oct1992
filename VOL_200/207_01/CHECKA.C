/*  checka.c -- 2nd source file for check register program                   */

/*  copyright (c) 1986 by Jim Woolley and WoolleyWare, San Jose, CA          */

/*  vers. 1.0, 12/85 thru 5/86
 */

/*  this file contains:
 *      control()
 *      char chek( c, p)
 *      char eddate( c)
 *      char edpayee( c)
 *      char edcategory( c)
 *      char edamount( c)
 *      char eddeposit( c)
 *      char edclear( c)
 *      char edbbf( c)
 */

#include    "a:checks.h"

control()                               /*  control entry display and edit   */
{
    char c, cwitch, shocommand, s[ 6], chek();
    int i, oldrecno;

    shocommand = FALSE;                 /*  initialize                       */
    oldrecno = -1;
    FOREVER
    {
        if ( shocommand || oldrecno != Recno)
        {
            strcpy( s, CLRCOM);
            formatd( s, 3, ( Recno + 1));
            cursorto( 0, 0);
            putscr( Ivon);
            puts( s);                   /*  display Recno and clear command  */
            putscr( Ivoff);
            oldrecno = Recno;
            shocommand = TRUE;
        }
        putquery();
        putcursor( Recno, Field);
        c = chek( getchar(), &shocommand);  /*  wait for command             */
        if ( Recno > Maxentry)          /*  if beyond max entry, skip edit   */
            Oldfield = -1;
        else if ( isibbf( Recno))       /*  else, if a BBF entry             */
        {
            c = edbbf( c);  
            Oldfield = -1;
        }
        else                            /*  else, not a BBF entry            */
        {
            cwitch = Field;
            switch ( cwitch)
            {
            case MMFIELD:  case DDFIELD:  case YYFIELD:
                c = chek( eddate( c), &shocommand);
                break;
            case PAYFIELD:
                c = edpayee( c);
                break;
            case CATFIELD:
                c = edcategory( c);
                break;
            case AMTFIELD:
                c = chek( edamount( c), &shocommand);
                break;
            case DEPFIELD:
                c = eddeposit( c);
                break;
            case CLRFIELD:
                c = edclear( c);
                break;
            }
            Oldfield = Field;
        }
        switch( c)
        {
        case 0:
            break;
        case CTRLD:  case '\r':  case CTRLF:  case '\t':
            goright( c);
            break;
        case CTRLS:  case '\b':  case CTRLA:
            goleft( c);
            break;
        case CTRLX:  case CTRLZ:
            godown( c);
            break;
        case CTRLE:  case CTRLW:
            goup( c);
            break;
        case CTRLC:
            gonext();
            break;
        case CTRLR:
            goprior();
            break;
        case CTRLY:
            delete();
            break;
        case CTRLU:
            undo();
            break;
        case CTRLN:
            Today.yy = 0;               /*  use date of previous entry       */
            insert();
            break;
        case CTRLP:
            print();
            break;
        case CTRLB:  case CTRLO:
            order( Recno);
            break;
        case CTRLQ:  case CTRLK:
            ctrlqork();
            break;
        case CTRLV:
            if ( Field == PAYFIELD)
            {
                Inserton = !Inserton;
                Oldfield = -1;
                break;                  /*  break switch                     */
            }                           /*  else fall thru                   */
        default:
            putchar( BEL);
            break;
        }
    }
}

char chek( c, p)                        /*  check and return c               */
char c, *p;                             /*  update *p                        */
{
    if ( c == CTRLJ)
        c = help();                     /*  display help screen              */
    if ( *p = putcommand( c))
        putcursor( Recno, Field);
    return ( c);
}

char eddate( c)                         /*  edit MM/DD/YY fields             */
char c;
{
    char cwitch, count, new;
    int ic;
    struct record *e;

    if ( c == CTRLU)
        return ( CTRLU);                /*  undo delete                      */
    e = &Entry[ Recno];
    cwitch = Field;
    switch ( cwitch)
    {
    case MMFIELD:
        ic = e->date.mm;
        break;
    case DDFIELD:
        ic = e->date.dd;
        break;
    case YYFIELD:
        ic = e->date.yy;
        break;
    }
    new = TRUE;
    if ( ic < 10)
        count = 1;
    else count = 2;
    FOREVER
    {
        if ( c == '/')
            c = '\r';
        if ( isdigit( c) && ( count < 2 || new))
        {
            if ( new)
            {
                count = ic = 0;
                putchar( '\b');
                putchar( ' ');
                new = FALSE;
            }
            if ( ic)
            {
                putchar( '\b');
                putchar( '0' + ic);
            }
            putchar( c);
            putchar( '\b');
            ic = 10*ic + c - '0';
            ++count;
        }
        else if ( c == CTRLG || c == CTRL_ || c == DEL)
        {
            if ( count)
            {
                putchar( '\b');
                putchar( ' ');
                ic /= 10;
                if ( --count && ic)
                    putchar( '0' + ic);
                else putchar( ' ');
                putchar( '\b');
                new = FALSE;
            }
            else putchar( BEL);
        }
        else if ( c == CTRLU)
        {
            putrecord( Recno);
            putcursor( Recno, Field);
            return ( 0);
        }
        else if ( iscntrl( c))
        {
            if ( new)
                return ( c);
            if ( Field != MMFIELD && ic < 10)
            {
                putchar( '\b');
                putchar( '0');
            }
            if ( !ic)
                putchar( '0');
            break;                      /*  break FOREVER                    */
        }
        else putchar( BEL);
        c = getchar();                  /*  get another character            */
    }                                   /*  end FOREVER                      */
    switch ( cwitch)
    {
    case MMFIELD:
        e->date.mm = ic;
        break;
    case DDFIELD:
        e->date.dd = ic;
        break;
    case YYFIELD:
        e->date.yy = ic;
        break;
    }
    Modified = TRUE;
    return ( c);
}

char edpayee( c)                        /*  edit payee field                 */
char c;
{
    char *p, *ep, s[ 4], test, past;
    int len, count;
    struct nlist *np;

    p = ( ep = Entry[ Recno].payee) + Character;
    len = strlen( ep);
    if ( isprint( c) && len < ( PAYEESIZE - 1))
    {
        strcpy( Savpayee, ep);
        if ( Inserton)
        {
            movmem( p, ( p + 1), ( len - Character + 1));
            *p = c;
            puts( p);
        }
        else putchar( *p = c);
        ++Character;
        if (( len + 1) == 3)
        {
            strcpy(( p = s), ep);
            while ( *p = toupper( *p))
                ++p;
            if ( np = lookup( s))
            {
                for ( count = 5*Dloop; count && !kbhit(); --count)
                    continue;           /*  wait 0.5 sec for key             */
                if ( !count)            /*  if no key, expand abreviation    */
                {
                    strcpy( ep, np->fullname);
                    putrecord( Recno);
                    Character = strlen( ep);
                }
            }
        }
    }
    else if ( c == CTRLG && Character < len)
    {
        strcpy( Savpayee, ep);
        movmem(( p + 1), p, ( len - Character));
        puts( p);
        putchar( PAYEEFILL);
    }
    else if (( c == CTRL_ || c == DEL) && Character)
    {
        strcpy( Savpayee, ep);
        --Character;
        movmem( p, ( p - 1), ( len - Character));
        putchar( '\b');
        puts( p - 1);
        putchar( PAYEEFILL);
    }
    else if ( c == CTRLT)
    {
        strcpy( Savpayee, ep);
        past = isspace( *p);
        if ( Character)
            test = ( past || isspace( *( p - 1)));
        else test = TRUE;
        count = 0;
        len -= Character;
        while ( *p)
        {
            movmem(( p + 1), p, len);
            --len;
            ++count;
            if ( isspace( *p))
            {
                if ( !test)             /*  && isspace( *p)                  */
                    break;              /*  break while loop                 */
                past = TRUE;
            }
            else if ( test && past)     /*  && !isspace( *p)                 */
                break;                  /*  break while loop                 */
        }
        puts( p);
        while ( count--)
            putchar( PAYEEFILL);
    }
    else if ( c == CTRLU && Recno == Savrecno)
    {
        strcpy( ep, Savpayee);
        putrecord( Recno);
        putcursor( Recno, Field);
        Savrecno = -1;
        return ( 0);
    }
    else return ( c);
    putcursor( Recno, Field);
    Savrecno = Recno;
    Modified = TRUE;
    return ( 0);
}

char edcategory( c)                     /*  edit category field              */
char c;
{
    if ( c == CTRLG || c == CTRL_ || c == DEL)
        c = ' ';
    if ( isprint( c))
    {
        putchar( c = toupper( c));
        putchar( '\b');
        Entry[ Recno].category = c;
        c = 0;
        Modified = TRUE;
    }
    return ( c);
}

char edamount( c)                       /*  edit amount field                */
char c;
{
    char new, action, s[ 20];
    int i, it, row, col;
    struct money m, r, *a;
    struct record *e;

    if ( c == CTRLU)
        return ( CTRLU);                /*  undo delete                      */
    row = Recno - First + HEAD;
    col = Ftoc[ AMTFIELD];
    e = &Entry[ Recno];
    a = &( e->amount);
    movmoney( &m, a, TRUE);             /*  initialize money display         */
    it = 3;                             /*  initialize display location      */
    movmoney( &r, a, TRUE);             /*  initialize calculator constant   */
    action = 0;                         /*  initialize calculator action     */
    new = TRUE;                         /*  TRUE until new amount entered    */
    FOREVER
    {
        if ( isdigit( c))
        {
            if ( new)                   /*  initialize, then reset new       */
                it = m.dollar = m.cent = new = 0;
            c = c - '0';
            if ( it)
            {
                if ( it == 1)
                {
                    m.cent += 10*c;
                    ++it;
                }
                else if ( it == 2)
                {
                    m.cent += c;
                    ++it;
                }
                else putchar( BEL);
            }
            else if (( i = 10*m.dollar + m.cent/1000) > MAXDOLLAR)
                putchar( BEL);
            else
            {
                m.dollar = i;
                m.cent = 10*( m.cent%1000) + 100*c;
            }
        }
        else if ( c == '.')
        {
            if ( new)                   /*  initialize, then reset new       */
                it = m.dollar = m.cent = new = 0;
            if ( it)
                putchar( BEL);
            else it = 1;
        }
        else if ( c == CTRLU)           /*  undo current changes             */
        {
undoit:     putrecord( Recno);
            cursorto( row, col);
            return ( 0);
        }
        else if ( c == CTRLG || c == CTRL_ || c == DEL)
        {
            new = FALSE;                /*  reset new                        */
            if ( it)
                --it;
            if ( it == 2)
                m.cent -= m.cent%10;
            else if ( it == 1)
                m.cent -= m.cent%100;
            else
            {
                m.cent = 1000*( m.dollar%10) + 100*( m.cent/1000);
                m.dollar /= 10;
            }
        }
        else if ( iscntrl( c) || index( "+-=m", ( c = tolower( c))))
        {
            while ( c == 'm')           /*  calculate with Memory            */
            {
                cursorto( row, ( AMTCOL - 1));
                puts( "m\b");           /*  prompt for 2nd character         */
                switch ( c = tolower( getchar()))
                {
                case '+':               /*  add to Memory                    */
                    c = 'p';
                    break;
                case '-':               /*  subtract from Memory             */
                    c = 'n';
                    break;
                case 'r':               /*  recall from Memory               */
                    if ( !action)       /*  force thru calculator logic      */
                    {
                        a->dollar = a->cent = 0;
                        action = '+';
                    }
                    movmoney( &r, &Memory, ( new = TRUE));
                    c = '=';
                    break;
                case 'c':               /*  clear Memory                     */
                    Memory.dollar = Memory.cent = 0;
                    break;
                case CTRLU:             /*  undo                             */
                    goto undoit;
                default:                /*  try again                        */
                    putchar( BEL);
                    c = 'm';
                    break;
                }
            }
            if ( action)                /*  pending calculator action?       */
            {
                if ( !new)              /*  if changed, update constant      */
                    movmoney( &r, &m, TRUE);
                addmoney( &m, a, &r, ( action == '+'));
                if ( m.dollar < 0 || m.cent < 0)
                {                       /*  want positive result             */
                    e->deposit = !e->deposit;
                    movmoney( &m, &m, FALSE);
                }                       /*  move result to amount            */
                movmoney( a, &m, ( Modified = TRUE));
            }
            else if ( !new)             /*  move display to amount           */
                movmoney( a, &m, ( Modified = TRUE));
            else if ( iscntrl( c))
                return ( c);            /*  current amount is unchanged      */
            if ( action || !new)
            {
                putrecord( Recno);
                update( Recno);         /*  update Balance                   */
                cursorto( row, col);
            }
            if ( iscntrl( c))
                return ( c);
            if ( index( "pnc", c))      /*  calculate/display Memory         */
            {
                if ( c != 'c')
                    addmoney( &Memory, &Memory, &m, ( c == 'p'));
                cursorto(( HEAD - 4), AMTCOL);
                putmoney( &Memory);
                cursorto( row, ( AMTCOL - 1));
                putchar( ' ');
                cursorto( row, col);
                Modified = TRUE;
                c = '=';
            }
            if ( c == '=')
            {
                movmoney( &r, a, TRUE); /*  new calculator constant          */
                action = 0;             /*  no pending calculator action     */
            }
            else
            {
                cursorto( row, ( AMTCOL - 1));
                putchar( action = c);   /*  new pending calculator action    */
                cursorto( row, col);
            }
            it = 3;                     /*  reinitialize                     */
            new = TRUE;
        }
        else putchar( BEL);
        if ( !new)                      /*  if amount is changed             */
        {
            cursorto( row, AMTCOL);
            putmoney( &m);              /*  display current amount           */
            if ( it)
                i = 2 - min( 2, it);
            else i = 3;
            cursorto( row, ( col - i));
        }
        c = getchar();                  /*  get another character            */
    }                                   /*  end FOREVER                      */
}

char eddeposit( c)                      /*  edit deposit field               */
char c;
{
    char test;
    struct record *e;

    e = &Entry[ Recno];
    if (( c = toupper( c)) == ESC)
        test = !e->deposit;             /*  toggle TRUE/FALSE                */
    else if ( !(( test = ( c == 'Y')) || c == 'N'))
        return ( witch( c));            /*  response is not ESC or Y or N    */
    if ( e->deposit != test)            /*  test == TRUE if c == 'Y'         */
    {
        puts(( e->deposit = test) ? "DEP" : DEPCLRFIL);
        update( Recno);
        Modified = TRUE;
    }
    return ( 0);
}

char edclear( c)                        /*  edit clear field                 */
char c;
{
    char test;
    struct record *e;

    e = &Entry[ Recno];
    if (( c = toupper( c)) == ESC)
        test = !e->clear;               /*  toggle TRUE/FALSE                */
    else if ( !(( test = ( c == 'Y')) || c == 'N'))
        return ( witch( c));            /*  response is not ESC or Y or N    */
    if ( e->clear != test)              /*  test == TRUE if c == 'Y'         */
    {
        puts(( e->clear = test) ? "CLR" : DEPCLRFIL);
        Modified = TRUE;
    }
    return ( 0);
}

char edbbf( c)                          /*  edit BBF entry                   */
char c;
{
    char def, *p;
    struct record *e;

    def = NO;                           /*  default                          */
    while ( c == ESC)
    {
        def = !def;                     /*  reverse                          */
        putquery();                     /*  redisplay                        */
        putchar( '\b');
        putchar( def ? 'Y' : 'N');      /*  show default                     */
        putcursor( Recno, Field);
        c = getchar();
    }
    c = toupper( c);
    if ( c == 'N' || ( !def && c != 'Y'))
        return ( witch( c));
    prompt( "Confirm you wish to void ");
    puts( BBF);
    puts( " status (Y/N)? ");
    putchar( BEL);
    if ( !getyesno( NO))
        return ( 0);
    e = &Entry[ Recno];                 /*  else, void status as BBF         */
    e->category &= 0x7f;
    p = e->payee;
    while ( *p = tolower( *p))
        ++p;
    putrecord( Recno);
    putcursor( Recno, Field);
    Modified = TRUE;
    return ( 0);
}
