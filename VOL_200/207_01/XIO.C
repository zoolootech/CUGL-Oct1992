/*  xio.c -- direct bios input/output without bdos interaction
 *           plus other operating system dependent functions
 */

/*  copyright (c) 1986 by Jim Woolley and WoolleyWare, San Jose, CA          */

/*  vers. 1.0, 12/85 thru 4/86
 */

/*  this file contains:
 *      getchar()
 *      char ungetch( c)
 *      putchar( c)
 *      puts( s)
 *      setlst()
 *      resetlst()
 *      kbhit()
 *      resetdsk()
 *      defdsk()
 *  for use with the check register program
 */

#include    "a:checks.h"                /*  see this file for further info   */

getchar()                               /*  substitute for library function  */
{
    char c;

    if ( _Lastch)
    {
        c = _Lastch;
        _Lastch = 0;
        return ( c);
    }    
    return ( bios( 3));                 /*  direct keyboard input            */
}

char ungetch( c)                        /*  substitute for library function  */
char c;
{
    char a;

    a = _Lastch;
    _Lastch = c;
    return ( a);
}

putchar( c)                             /*  substitute for library function  */
char c;  
{
    if ( c == '\n')
        bios( _Outdev, CR);             /*  _Outdev is CONOUT or LSTOUT      */
    bios( _Outdev, c);
}

puts( s)                                /*  display string                   */
char *s;
{
    while ( *s)
        putchar( *s++);
}

setlst()                                /*  optionally redirect to printer   */
{                                       /*  return TRUE if redirected        */
                                        /*  else, return FALSE               */
    prompt(
"Press RETURN when printer is ready, or any other key to abandon print function"
           );
    if ( getchar() == CR)
    {
        prompt( "Printing ... ");
        Printing = TRUE;
        _Outdev = LSTOUT;               /*  direct putchar() to printer      */
        return ( TRUE);
    }
    else return ( FALSE);
}

resetlst()                              /*  redirect from list to console    */
{
    if ( Printing)
    {
        putchar( '\n');
        putchar( FF);
        Printing = FALSE;
    }
    _Outdev = CONOUT;                   /*  redirect putchar() to screen     */
}

kbhit()                                 /*  return TRUE if key pressed       */
{                                       /*  else, return FALSE               */
    return ( bios( 2));
}

resetdsk()                              /*  reset & reselect default disk    */
{
    int d;

    d = defdsk();                       /*  get default disk                 */
    bdos( 13, 0);                       /*  reset disk system                */
    bdos( 14, d);                       /*  reselect default disk            */
}

defdsk()                                /*  return default disk number       */
{
    return ( bdos( 25, 0) & 0x0f);      /*  result will be 0 thru 15         */
}
*  return default disk number       */
{
    return ( bdos( 25, 0) & 0x0f);  