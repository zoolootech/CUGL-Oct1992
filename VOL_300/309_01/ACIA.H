/* acia.h    for 6809 */
/* the acia is a MC6850 chip, located at address A000 */

#define ACIAPORT        0xA000

#code
aciastatus()
{
        char *acia;
        acia = ACIAPORT;
        return *acia;
}

initacia()      /* to 9600,n,8,2 interrupts off */
{
        char dummy, *acia, reset, baudrate;
        acia = ACIAPORT;
        baudrate = 0x11;
        reset = 3;
        *(acia) = reset;
        *(acia) = baudrate;
        dummy = *(acia+1);      /*clear RBR*/
}

kbhit()         /*check for arriving character, return 1 if avialable*/
{
        int ready;
        char *acia;
        acia = ACIAPORT;
        return ( (*acia) & 1 );
}

getchne()
{
        char ch, *acia;
        acia = ACIAPORT;
        while( kbhit() == 0)
                ;
        ch = *(acia+1);           /*read 0xA001*/
        return ch;
}

getchar()
{
        char ch;
        ch = getchne();
        putchar( ch );            /*echo character*/
        return ch;
}

putchar(ch)
char ch;
{
        int loop;
        char *acia;
        acia = ACIAPORT;
        while( ((*acia) & 0x02) == 0)    /* wait on TDRE */
                ;
        *(acia+1) = ch;
        for( loop = 0; loop < 200; loop++)      /*delay required for procomm*/
                ;
}

puts(s)  /* put string s, terminate with a cr,lf */
char *s;
{
        /* compiler handles \\ \' \n \r \f \b \t */
        while( *s ) {
               putchar(*s);
               s++;
        }
        putchar(10);
        putchar(13);
}

gets(s)  /* get string from acia int s, null terminate */
char *s;
{
        char ch, cr, nl;
        int lp;
        cr = 13; nl = 10; lp = 0;
        ch = getchne();
        while( (ch != cr ) && (ch != nl) ) {
                s[lp] = ch;
                lp++;
                putchar( ch );
                ch = getchne();
        }
        s[lp] = '\0';
        putchar(cr);  /* send CR/LF */
        putchar(nl);
}

getint()
{
   int value, lp;
   char ch, sign, str[8], nl, cr;
   nl = 10; cr = 13; lp = 0; value = 0;
   ch = getchar();
   while( (ch == ' ')||(ch == cr)||(ch == nl)||(ch == '\t') )
        ch = getchar();
   while( (ch != ' ')&&(ch != cr)&&(ch != nl)&&(ch != '\t')) {
        if(((ch >= 'A')&&(ch <= 'Z'))||((ch >= 'a')&&(ch <= 'z'))) break;
        str[lp] = ch; lp++; ch = getchar();
   }
   str[lp] = '\0';
   lp = 0;
   if( str[0] == '-' || str[0] == '+' ) { sign = str[0]; lp++; }
   while( str[lp] ) {
           value = 10 * value + (str[lp] - '0');
           lp++;
   }
   putchar(cr); putchar(nl);
   if( sign == '-' ) return (-value); else return value;
}

putint(value)
int value;
{
        /* right justified, field size 6 characters including sign*/
        char str[7], sign;
        int x;
        for( x = 0; x < 7; x++ )  str[x] = ' ';
        if( value < 0 ) {
                sign = '-';
                value = (-value);
        }
        else sign = '+';
        x = 6;
        do {
                str[x] = (value % 10) + '0';
                value = value / 10;
                x--;
        } while( value > 0 );
        str[x] = sign;
        for( x = 0; x < 7; x++ ) putchar( str[x] );
}

putnl()
{
        char nl;
        nl = 10;
        putchar('\r');
        putchar(nl);
}
