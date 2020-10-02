/*  checkd.c -- 5th source file for check register program                   */

/*  copyright (c) 1986 by Jim Woolley and WoolleyWare, San Jose, CA          */

/*  vers. 1.0, 12/85 thru 5/86
 */

/*  this file contains:
 *      delete()
 *      undo()
 *      insert()
 *      ctrlqork()
 *      erase( c)
 *      abandon()
 *      save()
 *      reconcile()
 *      abreviations()
 *      print()
 *      order()
 *      reorder( f)
 *      datemax( md, d)
 */

#include    "a:checks.h"

delete()                                /*  delete Entry[ Recno]             */
{
    int r;

    if ( Recno > Maxentry)
        return;
    if ( isibbf( Recno))
    {
        prompt( "Cannot delete ");
        puts( BBF);
        waitesc();
        return;
    }
    r = Recno - First + HEAD;
    movmem( &Entry[ Recno], &Entryundo, RECSIZ);
    Modified = Ctrlyundo = TRUE;
    if ( Savrecno == Recno)
        Savrecno = -1;
    else if ( Savrecno > Recno)
        --Savrecno;
    if ( Recno < Maxentry)
        movmem( &Entry[ Recno + 1], &Entry[ Recno], ( Maxentry - Recno)*RECSIZ);
    --Maxentry;
    if ( Maxentry < 0)
    {
        First = 0;
        Last = Maxentry = -1;
    }
    else First = min( First, ( Last = min( Last, Maxentry)));
    dellin( r);
    update( Recno);
    if ( Recno > Maxentry)
        Field = 0;
    putcursor( Recno, Field);
}

undo()                                  /*  undo delete for Entry[ Recno]    */
{
    if ( Ctrlyundo && insert())
    {
        movmem( &Entryundo, &Entry[ Recno], RECSIZ);
        putrecord( Recno);
        update( Recno);
        putcursor( Recno, Field);
    }
}

insert()                                /*  insert Entry[ Recno]             */
{                                       /*  return TRUE if successful        */
    int i;

    if ( Maxentry == ( ENTRYSIZE - 1))
    {
        prompt( "Number of entries is maximum allowed");
        waitesc();
        return ( FALSE);
    }
    if ( Recno <= Maxentry)
    {
        i = Maxentry - Recno + 1;
        movmem( &Entry[ Recno], &Entry[ Recno + 1], i*RECSIZ);
        movmem( &Balance[ Recno], &Balance[ Recno + 1], i*sizeof( Balance[ 0]));
    }
    if ( Savrecno >= Recno)
        ++Savrecno;
    ++Maxentry;
    newentry( Recno);                   /*  Modified will be set TRUE        */
    Last = min(( Last + 1), ( First + LAST));
    inslin( Recno - First + HEAD);
    putcursor( Recno, Field);
    return ( TRUE);
}

ctrlqork( c)                            /*  process CTRLQ or CTRLK command   */
char c;
{
    char n;
    int i;

    cursorto( 0, 2);
    n = putnext();
    if ( c == CTRLQ)                    /*  process CTRLQ command            */
    {
        switch ( n)
        {
        case 'R':
            gotop();
            break;
        case 'C':
            gobottom();
            break;
        case 'D':
            if ( Recno > Maxentry)
                break;
            Character = PAYEESIZE;      /*  putcursor() will readjust        */
            putcursor( Recno, ( Field = MAXFIELD - 1));
            break;
        case 'S':  case 'H':
            Character = 0;
            putcursor( Recno, ( Field = 0));
            break;
        case 'E':
            putcursor(( Recno = First), Field);
            break;
        case 'X':
            Recno = min(( Maxentry + 1), ( First + ( LAST - 1)));
            if ( Recno > Maxentry)
                Field = 0;
            putcursor( Recno, Field);
            break;
        case 'Y':  case DEL:  case ( CTRL_ + CTRLTOA):
            if ( Field == PAYFIELD)
            {
                strcpy( Savpayee, Entry[ Recno].payee);
                Savrecno = Recno;
                putcursor( Recno, Field);
                erase( n);
            }
            break;
        case 'W':  case 'Z':
            goupdown( n);
            break;
        default:
            putchar( BEL);
            break;
        }
    }
    else                                /*  process CTRLK command            */
    {
        switch ( n)
        {
        case 'O':
            order( 0);
            break;
        case 'D':  case 'X':
            done();                     /*  never returns                    */
            break;
        case 'S':
            save();
            break;
        case 'Q':
            abandon();                  /*  may not return                   */
            break;
        case 'R':
            reconcile();
            break;
        case 'A':
            abreviations();
            break;
        case 'P':
            print();
            break;
        default:
            putchar( BEL);
            break;
        }
    }
}

erase( c)                               /*  erase payee left or right        */
char c;
{
    char *p, *q;
    int count;

    q = Entry[ Recno].payee;
    p = q + Character;
    if ( c == 'Y')
    {
        count = strlen( p);
        *p = '\0';
    }
    else if ( !Character)
        return;
    else
    {
        strcpy( q, p);
        count = Character;
        Character = 0;
        putcursor( Recno, Field);
        puts( q);
    }
    while ( count--)
        putchar( PAYEEFILL);
    Modified = TRUE;
}

abandon()                               /*  abandon without resave           */
{
    if ( Modified)
    {
        prompt( "Abandon without saving changes (Y/N)? ");
        if ( getyesno( NO))
            aexit();
    }
    else
    {
        prompt( "Abandoning unchanged file");
        aexit();
    }
}

save()                                  /*  save entries and continue        */
{
    prompt( "");                        /*  clear prompt line                */
    if ( savedat())                     /*  if error                         */
        return;
    waitesc();                          /*  else, wait for recognition       */
    Modified = FALSE;
}

reconcile()                             /*  display summary by category      */
{
    char c, *spaces, *sh, *ss, scroll, nbr[ DEL];
    int i, j, count, lines, firstline, lastline, maxline;
    struct
        {
        struct calendar maxdate;
        char sumcategory;
        struct money beginbal, clrcheck, clrdeposit, allcheck, alldeposit;
        } sum, *a, *start;
    struct record *e;
    struct money *m, clrendbal, allendbal;

    spaces = "           ";
    sh =
"--------  --------  --------  --------  --------  --------  --------";
    ss = "-------- - ";
    prompt( "");
    setmem( nbr, DEL, 0);               /*  initialize                       */
    for ( j = 0; j <= Maxentry; ++j)    /*  count number in each category    */
        ++nbr[ Entry[ j].category & 0x7f];
    lines = 0;
    for ( i = ' '; i < DEL; ++i)        /*  count separate categories, then  */
        if ( nbr[ i])                   /*      allocate space for each      */
            ++lines;
    if ( !lines)
        return;
    if ( !( a = start = alloc( lines*sizeof( *a))))
    {
        prompt( "Insufficient memory to reconcile all categories");
        waitesc();
        return;
    }
    recheading( ss, sh);
    setmem( &sum, sizeof( sum), 0);      /*  initialize                       */
    setmem( a, ( lines*sizeof( *a)), 0);
    for ( i = ' '; i < DEL; ++i)        /*  summarize each category          */
    {
        if ( !nbr[ i])                  /*  if category does not exist       */
            continue;                   /*      next category                */
        a->sumcategory = i;
        count = 0;
        for ( j = 0; j <= Maxentry; ++j)
        {                               /*  loop thru entries                */
            e = &Entry[ j];
            if ((( c = e->category) & 0x7f) != i)
                continue;               /*  next entry                       */
            datemax( &( a->maxdate), &( e->date));
            m = &( e->amount);
            if ( c & 0x80)              /*  if a BBF entry                   */
                addmoney( &a->beginbal, &a->beginbal, m, e->deposit);
            else                        /*  not a BBF entry                  */
            {
                if ( e->deposit)        /*  if a deposit                     */
                {
                    addmoney( &a->alldeposit, &a->alldeposit, m, TRUE);
                    if ( e->clear)
                        addmoney( &a->clrdeposit, &a->clrdeposit, m, TRUE);
                }
                else                    /*  not a deposit                    */
                {
                    addmoney( &a->allcheck, &a->allcheck, m, TRUE);
                    if ( e->clear)
                        addmoney( &a->clrcheck, &a->clrcheck, m, TRUE);
                }
            }
            if ( ++count == nbr[ i])    /*  if all entries for this category */
                break;                  /*      break j loop over entries    */
        }                               /*  end j loop over entries          */
        datemax( &( sum.maxdate), &( a->maxdate));
        addmoney( &sum.beginbal, &sum.beginbal, &a->beginbal, TRUE);
        addmoney( &sum.alldeposit, &sum.alldeposit, &a->alldeposit, TRUE);
        addmoney( &sum.clrdeposit, &sum.clrdeposit, &a->clrdeposit, TRUE);
        addmoney( &sum.allcheck, &sum.allcheck, &a->allcheck, TRUE);
        addmoney( &sum.clrcheck, &sum.clrcheck, &a->clrcheck, TRUE);
        ++a;                            /*  next summary line                */
    }                                   /*  end i loop over categories       */
    firstline = i = 0;
    maxline = lines + 1;
    lastline = min( maxline, ( LAST - 1));
    Printing = scroll = FALSE;
    FOREVER                             /*  begin display loop               */
    {
        cursorto((( HEAD + 1) + i - firstline), 0);
        if ( i == lines)
        {
            puts( spaces);
            puts( sh);
        }
        else
        {
            if ( i < lines)
            {
                a = start + i;
                c = a->sumcategory;
            }
            else
            {
                a = &sum;
                c = ' ';
            }
            putdate( &( a->maxdate));
            putchar( c);
            addmoney( &clrendbal, &a->beginbal, &a->clrcheck, FALSE);
            addmoney( &clrendbal, &clrendbal, &a->clrdeposit, TRUE);
            addmoney( &allendbal, &a->beginbal, &a->allcheck, FALSE);
            addmoney( &allendbal, &allendbal, &a->alldeposit, TRUE);
            putchar( ' ');
            putmoney( &a->beginbal);
            putchar( ' ');
            putmoney( &a->clrcheck);
            putchar( ' ');
            putmoney( &a->clrdeposit);
            putchar( ' ');
            putmoney( &clrendbal);
            putchar( ' ');
            putmoney( &a->allcheck);
            putchar( ' ');
            putmoney( &a->alldeposit);
            putchar( ' ');
            putmoney( &allendbal);
        }
        if ( Printing)
        {
            if ( ++i > maxline)
            {
                resetlst();
                i = count;              /*  reset line index                 */
            }
        }
        else if ( !( scroll = ( scroll || i == lastline)))
        {
            ++i;                        /*  fill screen                      */
            scroll |= ( kbhit() && firstline && lastline < maxline);
        }
        else
        {
            prompt( "");
            if ( firstline)             /*  if line 0 is off screen          */
                puts( "^W to see prior, ");
            if ( lastline < maxline)    /*  if final line is off screen      */
                puts( "^Z to see next, ");
            puts( "^P to print, or ESCAPE to see full register");
            FOREVER                     /*  loop until valid key             */
            {
                if (( c = getchar()) == CTRLW && firstline)
                {
                    i = --firstline;
                    --lastline;
                    if ( !linins( HEAD + 1))
                        scroll = FALSE;
                    break;              /*  break FOREVER loop               */
                }
                else if ( c == CTRLZ && lastline < maxline)
                {
                    ++firstline;
                    i = ++lastline;
                    if ( !lindel( HEAD + 1))
                    {
                        i = firstline;
                        scroll = FALSE;
                    }
                    break;              /*  break FOREVER loop               */
                }
                else if ( c == CTRLP)
                {
                    if ( setlst())
                    {
                        recheading( ss, sh);
                        count = i;      /*  save current line index          */
                        i = 0;
                    }
                    break;              /*  break FOREVER loop               */
                }
                else if ( c == ESC)
                {
                    free( start);
                    clrscr();
                    disheading();
                    display( First);
                    return;
                }
            }                           /*  end FOREVER loop until valid key */
        }                               /*  end if ... else scroll test      */
    }                                   /*  end FOREVER display loop         */
}                                       /*  end reconcile()                  */

abreviations()                          /*  display installed abreviations   */
{
    char c, scroll;
    int i, lines, hashval, firstline, lastline, compabrev();
    struct nlist *np, **start;

    prompt( "");
    lines = 0;                          /*  allocate abrev pointer space     */
    for ( hashval = 0; hashval < HASHSIZE; ++hashval)
        for ( np = Hashtab[ hashval]; np; np = np->next)
            ++lines;                    /*  count abreviations               */
    if ( !lines)
    {
        prompt( "There are no abreviations installed");
        goto quit;
    }
    if ( !( start = alloc( lines*sizeof( np))))
    {
        prompt( "Insufficient memory to display all abreviations");
quit:   waitesc();
        return;
    }
    clrscr();
    putscr( Ivon);
    puttitle();
    puts(
"            Abreviation  Payee                                                 "
         );
    putchar( '\n');
    puts(
"            -----------  ------------------------------------------            "
         );
    putscr( Ivoff);
    i = 0;
    for ( hashval = 0; hashval < HASHSIZE; ++hashval)
        for ( np = Hashtab[ hashval]; np; np = np->next)
            *( start + ( i++)) = np;    /*  save pointer to each abreviation */
    qsort( start, lines, sizeof( np), compabrev);
    firstline = i = 0;
    lastline = min( --lines, LAST);
    scroll = FALSE;
    FOREVER                             /*  begin display loop               */
    {
        cursorto(( HEAD + i - firstline), 0);
        np = *( start + i);
        puts( "                ");
        puts( np->abrev);
        puts( "      ");
        puts ( np->fullname);
        if ( !( scroll = ( scroll || i == lastline)))
        {
            ++i;                        /*  fill screen                      */
            scroll |= ( kbhit() && firstline && lastline < lines);
        }
        else
        {
            prompt( "");
            if ( firstline)             /*  if line 0 is off screen          */
                puts( "^W to see prior, or ");
            if ( lastline < lines)      /*  if final line is off screen  */
                puts( "^Z to see next, or ");
            puts( "ESCAPE to see full register");
            FOREVER                     /*  loop until valid key             */
            {
                if (( c = getchar()) == CTRLW && firstline)
                {
                    i = --firstline;
                    --lastline;
                    if ( !linins( HEAD))
                        scroll = FALSE;
                    break;              /*  break FOREVER loop               */
                }
                else if ( c == CTRLZ && lastline < lines)
                {
                    ++firstline;
                    i = ++lastline;
                    if ( !lindel( HEAD))
                    {
                        i = firstline;
                        scroll = FALSE;
                    }
                    break;              /*  break FOREVER loop               */
                }
                else if ( c == ESC)
                {
                    free( start);
                    clrscr();
                    disheading();
                    display( First);
                    return;
                }
            }                           /*  end FOREVER loop until valid key */
        }                               /*  end if ... else scroll test      */
    }                                   /*  end FOREVER display loop         */
}                                       /*  end abreviations()               */

print()                                 /*  print entries                    */
{
    int i;

    if ( setlst())
    {
        disheading();
        for ( i = 0; i <= Maxentry; ++i)
            putrecord( i);              /*  cannot use display()             */
        resetlst();
    }
}

order( i)                               /*  reorder entries i to Maxentry    */
int i;
{
    char c;
    int compdate(), comppayee(), compcateg(), compamount(), ( *f)();

    prompt(
"Order entries by Date, Payee, Category, Amount, or Neither (D/P/C/A/N)? N"
           );
    putchar( '\b');
    switch ( c = toupper( getchar()))
    {
    case 'D':
        f = compdate;
        break;
    case 'P':
        f = comppayee;
        break;
    case 'C':
        f = compcateg;
        break;
    case 'A':
        f = compamount;
        break;
    default:
        c = 0;
        break;
    }
    if ( c)
    {
        putchar( c);
        reorder( i, f);
    }
}

reorder( i, f)                          /*  reorder from i using function f  */
int i, ( *f)();
{
    qsort( &Entry[ i], ( Maxentry - i + 1), RECSIZ, f);
    for ( ; i <= Maxentry; ++i)    /*  update balance and display            */
    {
        newbalance( i);
        if ( First <= i && i <= Last)
            putrecord( i);
    }
    Character = 0;                      /*  avoid dangling cursor            */
}

datemax( md, d)                         /*  determine max date               */
struct calendar *md;                    /*  max date                         */
struct calendar *d;                     /*  trial date                       */
{
    if ( datecomp( md, d) < 0)
    {
        md->mm = d->mm;
        md->dd = d->dd;
        md->yy = d->yy;
    }
}
