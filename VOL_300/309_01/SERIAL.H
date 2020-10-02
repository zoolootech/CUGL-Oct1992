/* SERIAL.H */

#define BASE        0x93f0
#define RBR         8
#define TBR         8
#define DIVL        8
#define DIVH        9
#define LCR         11
#define IER         9
#define LSR         13
#define MCR         11

#code

init8250()      /*initialises COM1 (PC serial card) to 9600,n,8,2 */
{
        char *com1;
        com1 = BASE;
        com1[LCR] = 0x80;       /*access BRG*/
        com1[DIVL] = 0x0c;
        com1[DIVH] = 0x00;
        com1[LCR] = 0x03;
        com1[IER] = 0x00;
        com1[MCR] = 0x03;     /*assert DTR and RTS*/
}

pcgetchne()       /* gets character from COM1, non-echo */
{
        char *com1;
        com1 = BASE;
        while( (com1[LSR] & 1) != 1 )
                ;
        return com1[RBR];
}

pcgetchar()     /*gets character from COM1, echoes to COM1*/
{
        char ch;
        ch = pcgetchne();
        pcputchar(ch);
        return ch;
}

pcgetint()      /*read integer from COM1*/
{
   int value, lp;
   char ch, sign, str[8], nl, cr;
   nl = 10; cr = 13; lp = 0; value = 0;
   ch = pcgetchar();
   while( (ch == ' ') || (ch == cr) || (ch == nl) || (ch == '\t') )
        ch = pcgetchar();
   while( (ch != ' ') && (ch != cr) && (ch != nl) && (ch != '\t') ) {
        if( ((ch >= 'A') && (ch <= 'Z')) || ((ch >= 'a') && (ch <= 'z')) ) break;
        str[lp] = ch; lp++; ch = pcgetchar();
   }
   str[lp] = '\0';
   lp = 0;
   if( str[0] == '-' || str[0] == '+' ) { sign = str[0]; lp++; }
   while( str[lp] ) {
           value = 10 * value + (str[lp] - '0');
           lp++;
   }
   pcputchar(cr); pcputchar(nl);
   if( sign == '-' ) return (-value); else return value;
}

pcgets(s)        /*read string from COM1*/
char *s;
{
        char ch, cr, nl;
        int lp;
        cr = 13; nl = 10; lp = 0;
        ch = pcgetchne();
        while( (ch != cr ) && (ch != nl) ) {
                s[lp] = ch;
                lp++;
                pcputchar( ch );
                ch = pcgetchne();
        }
        s[lp] = '\0';
        pcputchar(cr);  /* send CR/LF */
        pcputchar(nl);
}

pckbhit()       /*checks for recieve character from COM1, 1=ready, 0=not*/
{
        char *com1;
        com1 = BASE;
        return ( (com1[LSR] & 1) );
}

pcputchar(ch)     /*outputs character to COM1  */
char ch;
{
        char *com1;
        com1 = BASE;
        while( (com1[LSR] & 0x20) != 0x20)
                ;
        com1[TBR] = ch;
}

pcputint(value)      /*outputs integer as a string to COM1*/
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
        for( x = 0; x < 7; x++ ) pcputchar( str[x] );
}

pcputs(s)        /*outputs string to COM1, appends CR/LF*/
char *s;
{
        /* compiler handles \\ \' \n \r \f \b \t */
        while( *s ) {
               pcputchar(*s);
               s++;
        }
        pcputchar(10);
        pcputchar(13);
}


pcstatus()      /*returns LSR status of 8250*/
{
        char *com1;
        com1 = BASE;
        return (com1[LSR]);
}

