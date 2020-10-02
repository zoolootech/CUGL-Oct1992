/* printer.h  for hercs card printer port */

#define PRNDATA         0x93bc
#define PRNSTATUS       0x93bd
#define PRNCNTRL        0x93be

#code

initprn()
{
        char ch, *prn;
        int delay;
        prn = PRNCNTRL;
        *prn = 8;
        for( delay = 0; delay < 2000; delay++ )
                ;
        *prn = 13;
}

prnstatus()
{
        char ch, *prn;
        prn = PRNSTATUS;
        ch = *prn & 0xf8;
        return ch;
}

prnchar(ch)     /* write char to prn port */
char ch;
{
        char status, *prn;
        int loop;
        prn = PRNDATA;
        status = prnstatus() & 128;
        while( status == 0 ) {
                status = prnstatus & 128;
                ++loop;
                if( loop == 250 ) { status = prnstatus(); return status; }
        }
        *prn = ch;
        *(prn+2) = 12;
        for( loop = 0; loop < 1000; loop++ )
                ;
        *(prn+2) = 13;
}

/* status port d3 = error
               d4 = select status 1=selected
               d5 = paper out if 1
               d6 = acknowledge, cleared pulse when ready to accept data
               d7 = busy 1 can accept new data, 0 = busy

   control     d0 = strobe, 0 to read
               d1 = line feed enable, 0 = off (normal)
               d2 = 0 resets printer
               d3 = 1 select printer
               d4 = irq7 enable
*/