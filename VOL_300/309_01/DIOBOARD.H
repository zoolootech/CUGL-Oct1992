/* dioboard.h */

#define DIOBASE 0x9227
#define LEDS    0x9223
#define SW      0x9225
#define SEVENS  0x9224
#define KEYS    0x9226
#define ANALOG  0x9222

#const
char table[16] = { 0x3f,6,0x5b,0x4f,0x66,0x6d,0x7d,7,0x7f,
                   0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71 };
char rowval[4] = { 0x70, 0xb0, 0xd0, 0xe0 };
char key[20] =   {'0','4','8','C',
                  '3','7','B','F',
                  '2','6','A','E',
                  '1','5','9','D'};

#code

initdio()
{
        char *dio;
        dio = DIOBASE;
        *dio = 0x83;
}

writeleds( ch )
char ch;
{
        char *leds;
        leds = LEDS;
        *leds = ch;
}

readswitch()
{
        char *switches, data;
        switches = SW;
        data = *switches;
        return data;
}

readkey()            /* waits for a key press */
{
   int column, row;
   char keyval, columndata, *keys;

   keys = KEYS;
   row = 0; columndata = 15;
   while( columndata == 15)
   {
        *keys = rowval[row];
        columndata = *keys & 0x0f;
        row++; if( row > 3 ) row = 0;
   }
   if( columndata == 14 ) column = 0;
   else if( columndata == 13 ) column = 1;
   else if( columndata == 11 ) column = 2;
   else if( columndata == 7  ) column = 3;
   else column = 4;
   row = row << 2;
   if( column == 4 ) keyval = '\0'; else keyval = key[row+column];
   return keyval;
}

writeseg( ch )
char ch;
{
        char value, *sevenseg;

        if( ch >= 'a' )  ch = (ch - 'a') + 10;
        if (ch >= 'A') ch = (ch - 'A') + 10;
        if( ch >= '0' ) ch = ch - '0';
        if( ch > 15 ) return -1;
        value = table[ch];
        sevenseg = SEVENS;
        *sevenseg = value;
}

readanalog( chn )       /*multiply by 20 to give millivolts*/
int chn;
{
        char channel, data, *analog;

        if( chn > 7 ) return -1;
        analog = ANALOG;
        channel = chn;
        *analog = channel;
        while( (*analog & 1) != 1 )
                ;
        data = *(analog - 1);
        return data;
}
