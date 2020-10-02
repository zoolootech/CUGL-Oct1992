/*  checks.h -- header file for check register program                       */

/*  copyright (c) 1986 by Jim Woolley and WoolleyWare, San Jose, CA          */

/*  vers. 1.0, 12/85 thru 5/86
 */

#include    "a:bdscio.h"

#define     BBF             "BALANCE BROUGHT FORWARD"
#define     PAYEEFILL       '_'         /*  used to fill Payee               */
#define     DEPCLRFIL       "    "      /*  used to fill DEP and CLR         */
#define     DEFCAT          ' '         /*  default category code            */
#define     CLRCOM          "     "     /*  clear command display            */

#define     ENTRYSIZE       128         /*  max entries (see Entry)          */
#define     PAYEESIZE       43          /*  max chars in Entry->payee        */
#define     HASHSIZE        64          /*  size of abrev. hash table        */
#define     FNAMSIZE        15          /*  max chars in Filename            */
#define     CLRSIZE         384         /*  max records in CLR file          */
#define     COLS            80          /*  number of screen columns         */
#define     ROWS            24          /*  number of screen rows            */
#define     HEAD            6           /*  number of heading rows           */
#define     LAST            ( ROWS - HEAD - 1)
#define     PAGE            ( ROWS - HEAD - 6)
#define     BALCOL          ( COLS - 9) /*  where Balance display starts     */
#define     AMTCOL          54          /*  where amount display starts      */
#define     DLOOP           250         /*  loops for 100 msec on 4 MHz Z80  */
#define     MAXDOLLAR       999         /*  max dollar entry (see edamount)  */

#define     MAXFIELD        8                /*  number of displayed fields  */
#define     CLRFIELD        ( MAXFIELD - 1)  /*  Entry->clear field          */
#define     DEPFIELD        ( MAXFIELD - 2)  /*  Entry->deposit field        */
#define     AMTFIELD        ( MAXFIELD - 3)  /*  Entry->amount field         */
#define     CATFIELD        ( MAXFIELD - 4)  /*  Entry->category field       */
#define     PAYFIELD        ( MAXFIELD - 5)  /*  Entry->payee field          */
#define     YYFIELD         ( MAXFIELD - 6)  /*  Entry->year field           */
#define     DDFIELD         ( MAXFIELD - 7)  /*  Entry->date field           */
#define     MMFIELD         ( MAXFIELD - 8)  /*  Entry->month field          */

#define     CTRLA           0x01        /*  ASCII definitions                */
#define     CTRLB           0x02
#define     CTRLC           0x03
#define     CTRLD           0x04
#define     CTRLE           0x05
#define     CTRLF           0x06
#define     CTRLG           0x07
#define     BEL             0x07
#define     CTRLH           0x08
#define     BS              0x08
#define     CTRLI           0x09
#define     HT              0x09
#define     CTRLJ           0x0a
#define     LF              0x0a
#define     CTRLK           0x0b
#define     CTRLL           0x0c
#define     FF              0x0c
#define     CTRLM           0x0d
#define     CR              0x0d
#define     CTRLN           0x0e
#define     CTRLO           0x0f
#define     CTRLP           0x10
#define     CTRLQ           0x11
#define     CTRLR           0x12
#define     CTRLS           0x13
#define     CTRLT           0x14
#define     CTRLU           0x15
#define     CTRLV           0x16
#define     CTRLW           0x17
#define     CTRLX           0x18
#define     CTRLY           0x19
#define     CTRLZ           0x1a
#define     CTRL_           0x1f
#define     ESC             0x1b
#define     DEL             0x7f
#define     CTRLTOA         0x40        /*  offset CTRL to upper case alpha  */

#define     DEFNAM          "CHECKS."   /*  default filename                 */
#define     SCRTYP          "SCR"       /*  screen controls/messages         */
#define     DATTYP          "DAT"       /*  Entry data                       */
#define     BAKTYP          "BAK"       /*  Entry data backup                */
#define     CLRTYP          "CLR"       /*  cleared entries                  */
#define     TMPTYP          "$$$"       /*  temporary CLR file               */
#define     INFTYP          "INF"       /*  TITLE, ABREV, and AUTOM info     */

#define     NO              FALSE
#define     YES             TRUE

#define     isprint( c)     !iscntrl( c)
#define     FOREVER         for ( ; ; )                /*  loop forever      */

/*  define additional global variables and data structures                   */

struct calendar                         /*  month, day, year                 */
    {
    char mm, dd, yy;
    } Today;

struct money                            /*  example:  $1234.56               */
    {                                   /*      dollar = 12, cent = 3456     */
    int dollar;                         /*  $/100 (range of signed int)      */
    int cent;                           /*  $%100 + cents (-9999 to +9999)   */
    } Memory, Balance[ ENTRYSIZE];

struct record                           /*  one entry                        */
    {
    struct calendar date;
    char payee[ PAYEESIZE], category;
    struct money amount;
    char deposit, clear;
    } Entry[ ENTRYSIZE], Entryundo;

#define     RECSIZ          ( sizeof( Entry[ 0]))

struct nlist                            /*  ref. K & R, p. 135               */
    {
    char *abrev, *fullname;
    struct nlist *next;
    } *Hashtab[ HASHSIZE], *install(), *lookup();

char *alloc(), *index(), *skipspace(), *strsave(), witch(), help();
char putcommand(), putcntrl(), putnext();
char eddate(), edamount(), eddeposit(), edclear(), edcategory(), edpayee();

char Modified, Ctrlyundo, Printing, Inserton;
char Filename[ FNAMSIZE], Savpayee[ PAYEESIZE], Ftoc[ MAXFIELD];
char Title[ COLS - FNAMSIZE - 2];       /*  (COLS-1) - (5+2+FNAMSIZE-5) + 1  */

int First, Last, Maxentry, Recno, Field, Oldfield, Character, Speed, Savrecno;
int Dloop;

/*  the following global variables mimic WordStar cursor/screen controls     */

char Clead1[ 9], Clead2[ 5], Ctrail[ 5], Cb4flg, Linoff, Coloff, Ascur;
char Eraeol[ 7], Lindel[ 7], Linins[ 7], Ivon[ 7], Ivoff[ 7], Trmini[ 9];

/*  The functions getchar(), ungetch( c), putchar( c), and kbhit() are defined
 *  in xio.c to provide char I/O without BDOS interaction; therefore, ^C, ^S,
 *  ^Q, and ^P will have no effect during console I/O.  For putchar( c), the
 *  global variable _Outdev MUST be initialized to CONOUT for screen output;
 *  _Outdev may be temporarily set to LSTOUT for printer output, then reset to
 *  CONOUT.  For getchar() and ungetch( c), the global variable _Lastch MUST be
 *  initialized to 0.  Note that getc( 0) may NOT be equivalent to getchar()
 *  and putc( c, 1) may NOT be equivalent to putchar( c); however, each may be
 *  expected to function normally (using BDOS).  The object file xio.crl MUST
 *  be linked with the Check Register Program.
 */

#define     CONOUT          4           /*  BIOS console output              */
#define     LSTOUT          5           /*  BIOS list output                 */
char        _Outdev;                    /*  global variable for putchar( c)  */
char        _Lastch;                    /*  global for getchar()/ungetch()   */
